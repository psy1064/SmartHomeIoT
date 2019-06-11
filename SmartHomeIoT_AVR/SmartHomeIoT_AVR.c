#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h> 
#include <util/delay.h> 
#include <string.h>
#include "lcd.h"

<<<<<<< HEAD
// ¼­º¸¸ğÅÍ »ç¿ë
static volatile unsigned char    Pos_CMD = 0 ;    // ¼­º¸ À§Ä¡ ¸í·É ( ¹üÀ§ : 0 - 180,  ´ÜÀ§:  µµ )
=======
// ì„œë³´ëª¨í„° ì‚¬ìš©
static volatile unsigned char    Pos_CMD = 0 ;    // ì„œë³´ ìœ„ì¹˜ ëª…ë ¹ ( ë²”ìœ„ : 0 - 180,  ë‹¨ìœ„:  ë„ )
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
void Servo_On();
void Servo_Off();

// ì˜¨ìŠµë„ ì„¼ì„œ
void getDHT();
uint8_t I_RH,D_RH,I_Temp,D_Temp,CheckSum;
char i_rh[5], d_rh[5], i_temp[5], d_temp[5];

<<<<<<< HEAD
// ¹Ì¼¼¸ÕÁö ¼¾¼­
static char dust_count = 0;
char dust[32] = {0};
char pm10[2], pm10d[2];

=======
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
// Blue tooth communication
void init_serial(void) ;  //  Serial í† ì‹ í¬íŠ¸ ì´ˆê¸°í™”
void SerialPutChar(char ch);
void SerialPutString(char str[]);
void sendDHT();
void sendDust();

static volatile  char  recv_cnt = 0, rdata = 0, new_recv_flag = 0, rdata_old = 0 ;  
static volatile  char recv_data[3] = {0,0,0};
static volatile unsigned char   Command_Error_Flag = 0 ; 

static volatile char Cmd_Message_1[] = {"on" } ;     //  Blutooth Command
static volatile char Cmd_Message_2[] = {"off"} ;  

<<<<<<< HEAD
// ¹ü¿ë 
void Display_Number_LCD( unsigned int num, unsigned char digit ) ;    // ºÎÈ£¾ø´Â Á¤¼öÇü º¯¼ö¸¦ 10Áø¼ö ÇüÅÂ·Î LCD ¿¡ µğ½ºÇÃ·¹ÀÌ 
=======
// ë²”ìš© 
void Display_Number_LCD( unsigned int num, unsigned char digit ) ;    // ë¶€í˜¸ì—†ëŠ” ì •ìˆ˜í˜• ë³€ìˆ˜ë¥¼ 10ì§„ìˆ˜ í˜•íƒœë¡œ LCD ì— ë””ìŠ¤í”Œë ˆì´ 
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
void HexToDec( unsigned short num, unsigned short radix); 
char NumToAsc( unsigned char Num ); 
void msec_delay(unsigned int n);
void usec_delay(unsigned int n);
<<<<<<< HEAD
void pin_init();		// ÇÉ ¼³Á¤ ÃÊ±âÈ­
void init();			// ÃÊ±â ¼³Á¤
=======
void pin_init();		// í•€ ì„¤ì • ì´ˆê¸°í™”
void init();			// ì´ˆê¸° ì„¤ì •
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
static volatile unsigned char cnumber[5] = {0, 0, 0, 0, 0}; 

int main() 
{   
	char eq_count1=0, eq_count2=0, cmd_data = 0xFF  ;  	  
    unsigned char i=0 ;
	
<<<<<<< HEAD
	pin_init();		  // Pin ÃÊ±âÈ­
	init();			  // Interrupt , Timer, Register ÃÊ±âÈ­ 
	init_serial() ;   // Serial Port (USART0, 1) ÃÊ±âÈ­
=======
	pin_init();		  // Pin ì´ˆê¸°í™”
	init();			  // Interrupt , Timer, Register ì´ˆê¸°í™” 
	init_serial() ;   // Serial Port (USART0, 1) ì´ˆê¸°í™”
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db

	while (1) 
	{ 
		if( new_recv_flag == 1)
		{
			for( i=0; i < recv_cnt ; i++) 
			{
				if( recv_data[i] == Cmd_Message_1[i] ) eq_count1++ ;
			    if( recv_data[i] == Cmd_Message_2[i] ) eq_count2++ ; 
            }
	
			if(eq_count1 == 2) cmd_data = 1;
			else if(eq_count2 == 3) cmd_data = 2;
			else cmd_data = 0xFE;
			
			eq_count1 = 0, eq_count2 = 0 , new_recv_flag = 0;
		}
		if(cmd_data == 1)
		{
			Servo_On();
			LcdCommand(ALLCLR);
			LcdMove(0,0);
			LcdPuts("Turn On!!");
		}
		else if(cmd_data == 2)
		{
			Servo_Off();
			LcdCommand(ALLCLR);
			LcdMove(0,0);
			LcdPuts("Turn Off!!");
		}
		else if(cmd_data == 0xFE)
		{
			LcdCommand(ALLCLR);
			LcdMove(0,0);
			LcdPuts("Command Error!!");
			LcdMove(1,0);
			LcdPuts(recv_data);
		}

		cmd_data = 0xFF;
	}
} 

ISR(TIMER0_OVF_vect)   // Timer0 overflow interrupt( 10 msec)  service routine
{

	static unsigned short  time_index = 0, send_time_index = 0;

    TCNT0 = 256 - 156;       //  ë‚´ë¶€í´ëŸ­ì£¼ê¸° = 1024/ (16x10^6) = 64 usec,  
                             //  ì˜¤ë²„í”Œë¡œì¸í„°ëŸ½íŠ¸ ì£¼ê¸° = 10msec
                             //  156 = 10msec/ 64usec

    time_index++ ; 
<<<<<<< HEAD


    if( time_index == 200 )    // »ùÇÃ¸µÁÖ±â 10msec
    {       	
	   	getDHT();
	   	sendDHT();
	   	sendDust();
		if(sensor_time_index == 0)
		{
			// UCSR0B &= ~0x80;   // UART0 ¼ö½Å(RX) ¿Ï·á ÀÎÅÍ·´Æ® ±İÁö 
			sensor_time_index = 1;
		}
		else
		{
			// UCSR0B |= 0x80;   // UART0 ¼ö½Å(RX) ¿Ï·á ÀÎÅÍ·´Æ® Çã¿ë
			sensor_time_index = 0;
		}
		
		time_index = 0; 
   }
}

ISR( USART0_RX_vect)
{
	dust[dust_count] = UDR0;
	
	LcdCommand(ALLCLR);
	LcdMove(0,0);
	//LcdPutchar(dust[dust_count]);
	dust_count ++;

	/*if(dust_count >= 32)
	{
		if((dust[0] == 0x42)&&(dust[1] == 0x4d))
		{
			itoa(dust[8],pm10,10);
			itoa(dust[9],pm10d,10);

			LcdMove(1,5);
			LcdPuts(pm10);
			LcdMove(1,7);
			LcdPuts(pm10d);
		}
		dust_count = 0 ;
		for(int i = 0 < i ; 32 ; i++)
		{
			dust[i] = 0;
		}
	}*/
}

ISR( USART1_RX_vect )
=======
	
    if( time_index == 200 )    // ìƒ˜í”Œë§ì£¼ê¸° 10msec
    {
       time_index = 0; 
	   getDHT();
	   sendDHT();
	   sendDust();
   }
}
ISR( USART0_RX_vect )
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
{
    static unsigned char r_cnt = 0 ;

    rdata = UDR1; 

    if( rdata != '.' )                      // ìˆ˜ì‹ ëœ ë°ì´í„°ê°€ ë§ˆì§€ë§‰ ë¬¸ìë¥¼ ë‚˜íƒ€ë‚´ëŠ” ë°ì´í„°(ë§ˆì¹¨í‘œ)ê°€ ì•„ë‹ˆë©´
    {
        recv_data[r_cnt] = rdata;        //  ìˆ˜ì‹ ëœ ë¬¸ì ì €ì¥ 
	    r_cnt++;                         //  ìˆ˜ì‹  ë¬¸ì ê°¯ìˆ˜ ì¦ê°€ 

		new_recv_flag = 0;
    }
    else if(  rdata == '.' )                // ìˆ˜ì‹ ëœë°ì´í„°ê°€ ë§ˆì§€ë§‰ ë¬¸ìë¥¼ ë‚˜íƒ€ë‚´ëŠ” ë°ì´í„°(ë§ˆì¹¨í‘œ) ì´ë©´
    {
        recv_cnt = r_cnt ;                  // ìˆ˜ì‹ ëœ ë°ì´í„° ë°”ì´íŠ¸ìˆ˜ ì €ì¥
        r_cnt = 0;  
        
		new_recv_flag = 1;
    }
}


void pin_init()
{
<<<<<<< HEAD
	DDRB |= 0x80;    //  PWM Æ÷Æ®: OC2( PB7 ) Ãâ·Â¼³Á¤
=======
	DDRB |= 0x80;    //  PWM í¬íŠ¸: OC2( PB7 ) ì¶œë ¥ì„¤ì •
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
}
void init()
{
	LcdInit();

	LcdCommand(ALLCLR);
	LcdMove(0,0);  
	LcdPuts("HM=");
	LcdMove(0,8); 
	LcdPuts("TP= ");
	LcdMove(1,0);
<<<<<<< HEAD
	LcdPuts("Dust=");
=======
	LcdPuts("Dust(PM10) =");
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db


/**** Timer0 Overflow Interrupt  ******/

	TCCR0 = 0x00; 
    TCNT0 = 256 - 100;       //  ë‚´ë¶€í´ëŸ­ì£¼ê¸° = 8/ (16x10^6) = 0.5 usec,  
                             //  ì˜¤ë²„í”Œë¡œì¸í„°ëŸ½íŠ¸ ì£¼ê¸° = 50usec
                             //  156 = 50usec/ 0.5use

	TIMSK = 0x01;  // Timer0 overflow interrupt enable 
	
	TCCR0 |= 0x07; // Clock Prescaler N=1024 (Timer 0 Start)

/**** Motor PWM  ******/

<<<<<<< HEAD
	TCCR2 |= 0x68;   //  Trigger signal (OC2)   ¹ß»ı :  WGM20(bit6)=1,  WGM21(bit3)=1,  COM21(bit5)=1, COM20(bit4)=0 ,  
	TCCR2 |= 0x05;   //  1024ºĞÁÖ,  ³»ºÎÅ¬·°ÁÖ±â = 64usec  : CS22(bit2) = 1, CS21(bit1) = 0,  CS20(bit0) = 1 
=======
	TCCR2 |= 0x68;   //  Trigger signal (OC2)   ë°œìƒ :  WGM20(bit6)=1,  WGM21(bit3)=1,  COM21(bit5)=1, COM20(bit4)=0 ,  
	TCCR2 |= 0x05;   //  1024ë¶„ì£¼,  ë‚´ë¶€í´ëŸ­ì£¼ê¸° = 64usec  : CS22(bit2) = 1, CS21(bit1) = 0,  CS20(bit0) = 1 
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db

	Pos_CMD = 90 ;
    OCR2 = ( 135 * Pos_CMD )/900 + 10 ; 

	sei();         // Global Interrupt Enable 
}
void init_serial(void)
{
    UCSR0A = 0x00;                    //ì´ˆê¸°í™”
    UCSR0B = 0x18;                    //ì†¡ìˆ˜ì‹ í—ˆìš©,  ì†¡ìˆ˜ì‹  ì¸í„°ëŸ½íŠ¸ ê¸ˆì§€
    UCSR0C = 0x06;                    //ë°ì´í„° ì „ì†¡ë¹„íŠ¸ ìˆ˜ 8ë¹„íŠ¸ë¡œ ì„¤ì •.
    
    UBRR0H = 0x00;
    UBRR0L = 103;                     //Baud Rate 9600 
<<<<<<< HEAD
	UCSR0B |= 0x80;   // UART0 ¼ö½Å(RX) ¿Ï·á ÀÎÅÍ·´Æ® Çã¿ë ºí·çÅõ½º Åë½Å

	UCSR1A = 0x00;                    //ÃÊ±âÈ­
    UCSR1B = 0x18;                    //¼Û¼ö½ÅÇã¿ë,  ¼Û¼ö½Å ÀÎÅÍ·´Æ® ±İÁö
    UCSR1C = 0x06;                    //µ¥ÀÌÅÍ Àü¼ÛºñÆ® ¼ö 8ºñÆ®·Î ¼³Á¤.
=======

	UCSR0B |= 0x80;   // UART0 ì†¡ì‹ (RX) ì™„ë£Œ ì¸í„°ëŸ½íŠ¸ í—ˆìš©

	UCSR1A = 0x00;                    //ì´ˆê¸°í™”
    UCSR1B = 0x18;                    //ì†¡ìˆ˜ì‹ í—ˆìš©,  ì†¡ìˆ˜ì‹  ì¸í„°ëŸ½íŠ¸ ê¸ˆì§€
    UCSR1C = 0x06;                    //ë°ì´í„° ì „ì†¡ë¹„íŠ¸ ìˆ˜ 8ë¹„íŠ¸ë¡œ ì„¤ì •.
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
    
    UBRR1H = 0x00;
    UBRR1L = 103;                     //Baud Rate 9600 

<<<<<<< HEAD
	UCSR1B |= 0x80;   // UART1 ¼ö½Å(RX) ¿Ï·á ÀÎÅÍ·´Æ® Çã¿ë ºí·çÅõ½º Åë½Å
=======
	UCSR1B |= 0x80;   // UART1 ì†¡ì‹ (RX) ì™„ë£Œ ì¸í„°ëŸ½íŠ¸ í—ˆìš©
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
}

void sendDust()
{
<<<<<<< HEAD
	SerialPutString(pm10);
=======

>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
}

void sendDHT()
{
	SerialPutString(i_temp);
	SerialPutChar('.');
	SerialPutString(d_temp);
	SerialPutChar(',');
	SerialPutString(i_rh);
	SerialPutChar('.');
	SerialPutString(d_rh);
}

void getDHT()
{
	Request();		/* send start pulse */
	Response();		/* receive response */
	I_RH=Receive_data();	/* store first eight bit in I_RH */
	D_RH=Receive_data();	/* store next eight bit in D_RH */
	I_Temp=Receive_data();	/* store next eight bit in I_Temp */
	D_Temp=Receive_data();	/* store next eight bit in D_Temp */
	CheckSum=Receive_data();/* store next eight bit in CheckSum */
	
	if ((I_RH + D_RH + I_Temp + D_Temp) != CheckSum)
	{
		LcdMove(0,0);
		LcdPuts("Error");
	}
		
	else
	{	
<<<<<<< HEAD
		LcdCommand(ALLCLR);
		LcdMove(0,0);  
		LcdPuts("HM=");
		LcdMove(0,8); 
		LcdPuts("TP= ");
		LcdMove(1,0);
		LcdPuts("Dust=");

=======
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
		itoa(I_RH,i_rh,10);
		LcdMove(0,3);
		LcdPuts(i_rh);
		LcdMove(0,5);
		LcdPuts(".");
			
		itoa(D_RH,d_rh,10);
		LcdMove(0,6);
		LcdPuts(d_rh);
		LcdMove(0,7);
		LcdPuts("%");

<<<<<<< HEAD
		//////////// ½Àµµ Display
=======
		//////////// ìŠµë„ Display
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db

		itoa(I_Temp,i_temp,10);
		LcdMove(0,11);
		LcdPuts(i_temp);
		LcdMove(0,13);
		LcdPuts(".");
			
		itoa(D_Temp,d_temp,10);
		LcdMove(0,14);
		LcdPuts(d_temp);
		LcdMove(0,15);
		LcdPuts("C");
		
	}
				
	_delay_ms(10);
}

void Servo_On()
{
<<<<<<< HEAD
	Pos_CMD = 0 ;   		                 // ¼­º¸ À§Ä¡ ¸í·É =  0 µµ (¿ŞÂÊ ³¡)  
=======
	Pos_CMD = 0 ;   		                 // ì„œë³´ ìœ„ì¹˜ ëª…ë ¹ =  0 ë„ (ì™¼ìª½ ë)  
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
    OCR2 = ( 135 * Pos_CMD )/900  + 10  ;   
}
void Servo_Off()
{
<<<<<<< HEAD
	Pos_CMD = 180 ;   		                 // ¼­º¸ À§Ä¡ ¸í·É =  180 µµ (¿À¸¥ÂÊ ³¡)  
=======
	Pos_CMD = 180 ;   		                 // ì„œë³´ ìœ„ì¹˜ ëª…ë ¹ =  180 ë„ (ì˜¤ë¥¸ìª½ ë)  
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
    OCR2 = ( 135 * Pos_CMD )/900  + 10  ;   
}

void SerialPutChar(char ch)
{
<<<<<<< HEAD
	while(!(UCSR1A & (1<<UDRE)));			// ¹öÆÛ°¡ ºô ¶§¸¦ ±â´Ù¸²
  	UDR1 = ch;								// ¹öÆÛ¿¡ ¹®ÀÚ¸¦ ¾´´Ù
} // ÇÑ ¹®ÀÚ¸¦ ¼Û½ÅÇÑ´Ù.
=======
	while(!(UCSR0A & (1<<UDRE)));			// ë²„í¼ê°€ ë¹Œ ë•Œë¥¼ ê¸°ë‹¤ë¦¼
  	UDR0 = ch;								// ë²„í¼ì— ë¬¸ìë¥¼ ì“´ë‹¤
} // í•œ ë¬¸ìë¥¼ ì†¡ì‹ í•œë‹¤.
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db

void SerialPutString(char *str)
 {

    while(*str != '\0')          // ìˆ˜ì‹ ëœ ë¬¸ìê°€ Null ë¬¸ì( 0x00 )ê°€ ì•„ë‹ˆë©´ 
    {
        SerialPutChar(*str++);
    }
} // ë¬¸ìì—´ì„ ì†¡ì‹ í•œë‹¤.
  // ì…ë ¥   : str - ì†¡ì‹ í•œ ë¬¸ìì—´ì„ ì €ì¥í•  ë²„í¼ì˜ ì£¼ì†Œ


void Display_Number_LCD( unsigned int num, unsigned char digit )       // ë¶€í˜¸ì—†ëŠ” ì •ìˆ˜í˜• ë³€ìˆ˜ë¥¼ 10ì§„ìˆ˜ í˜•íƒœë¡œ LCD ì— ë””ìŠ¤í”Œë ˆì´ 
{

	HexToDec( num, 10); //10ì§„ìˆ˜ë¡œ ë³€í™˜ 

	if( digit == 0 )     digit = 1 ;
	if( digit > 5 )      digit = 5 ;
 
    if( digit >= 5 )     LcdPutchar( NumToAsc(cnumber[4]) );  // 10000ìë¦¬ ë””ìŠ¤í”Œë ˆì´
	
	if( digit >= 4 )     LcdPutchar(NumToAsc(cnumber[3]));    // 1000ìë¦¬ ë””ìŠ¤í”Œë ˆì´ 

	if( digit >= 3 )     LcdPutchar(NumToAsc(cnumber[2]));    // 100ìë¦¬ ë””ìŠ¤í”Œë ˆì´ 

	if( digit >= 2 )     LcdPutchar(NumToAsc(cnumber[1]));    // 10ìë¦¬ ë””ìŠ¤í”Œë ˆì´

	if( digit >= 1 )     LcdPutchar(NumToAsc(cnumber[0]));    //  1ìë¦¬ ë””ìŠ¤í”Œë ˆì´

}


void Display_TMP_LCD( unsigned int tp  )       // ì˜¨ë„ë¥¼ 10ì§„ìˆ˜ í˜•íƒœë¡œ LCD ì— ë””ìŠ¤í”Œë ˆì´ 
{

	HexToDec( tp, 10); //10ì§„ìˆ˜ë¡œ ë³€í™˜ 

    LcdPutchar(NumToAsc(cnumber[2]) );   // 10ìë¦¬ ë””ìŠ¤í”Œë ˆì´
	
    LcdPutchar(NumToAsc(cnumber[1]));    // 1ìë¦¬ ë””ìŠ¤í”Œë ˆì´ 

    LcdPuts( ".");                       // ì†Œìˆ«ì (.) ë””ìŠ¤í”Œë ˆì´ 

    LcdPutchar(NumToAsc(cnumber[0]));    // 0.1 ìë¦¬ ë””ìŠ¤í”Œë ˆì´ 
}


void HexToDec( unsigned short num, unsigned short radix) 
{
	int j ;

	for(j=0; j<5 ; j++) cnumber[j] = 0 ;

	j=0;
	do
	{
		cnumber[j++] = num % radix ; 
		num /= radix; 

	} while(num);
} 

char NumToAsc( unsigned char Num )
{
	if( Num <10 ) Num += 0x30; 
	else          Num += 0x37; 

	return Num ;
}

void msec_delay(unsigned int n)
{	
	for(; n>0; n--)		// 1msec ì‹œê°„ ì§€ì—°ì„ níšŒ ë°˜ë³µ
		_delay_ms(1);		// 1msec ì‹œê°„ ì§€ì—°
}

void usec_delay(unsigned int n)
{	
	for(; n>0; n--)		// 1usec ì‹œê°„ ì§€ì—°ì„ níšŒ ë°˜ë³µ
		_delay_us(1);		// 1usec ì‹œê°„ ì§€ì—°
}
