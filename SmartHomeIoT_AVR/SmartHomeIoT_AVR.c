#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h> 
#include <util/delay.h> 
#include <string.h>
#include "lcd.h"

<<<<<<< HEAD
// 서보모터 사용
static volatile unsigned char    Pos_CMD = 0 ;    // 서보 위치 명령 ( 범위 : 0 - 180,  단위:  도 )
=======
// �꽌蹂대え�꽣 �궗�슜
static volatile unsigned char    Pos_CMD = 0 ;    // �꽌蹂� �쐞移� 紐낅졊 ( 踰붿쐞 : 0 - 180,  �떒�쐞:  �룄 )
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
void Servo_On();
void Servo_Off();

// �삩�뒿�룄 �꽱�꽌
void getDHT();
uint8_t I_RH,D_RH,I_Temp,D_Temp,CheckSum;
char i_rh[5], d_rh[5], i_temp[5], d_temp[5];

<<<<<<< HEAD
// 미세먼지 센서
static char dust_count = 0;
char dust[32] = {0};
char pm10[2], pm10d[2];

=======
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
// Blue tooth communication
void init_serial(void) ;  //  Serial �넗�떊�룷�듃 珥덇린�솕
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
// 범용 
void Display_Number_LCD( unsigned int num, unsigned char digit ) ;    // 부호없는 정수형 변수를 10진수 형태로 LCD 에 디스플레이 
=======
// 踰붿슜 
void Display_Number_LCD( unsigned int num, unsigned char digit ) ;    // 遺��샇�뾾�뒗 �젙�닔�삎 蹂��닔瑜� 10吏꾩닔 �삎�깭濡� LCD �뿉 �뵒�뒪�뵆�젅�씠 
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
void HexToDec( unsigned short num, unsigned short radix); 
char NumToAsc( unsigned char Num ); 
void msec_delay(unsigned int n);
void usec_delay(unsigned int n);
<<<<<<< HEAD
void pin_init();		// 핀 설정 초기화
void init();			// 초기 설정
=======
void pin_init();		// ��� �꽕�젙 珥덇린�솕
void init();			// 珥덇린 �꽕�젙
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
static volatile unsigned char cnumber[5] = {0, 0, 0, 0, 0}; 

int main() 
{   
	char eq_count1=0, eq_count2=0, cmd_data = 0xFF  ;  	  
    unsigned char i=0 ;
	
<<<<<<< HEAD
	pin_init();		  // Pin 초기화
	init();			  // Interrupt , Timer, Register 초기화 
	init_serial() ;   // Serial Port (USART0, 1) 초기화
=======
	pin_init();		  // Pin 珥덇린�솕
	init();			  // Interrupt , Timer, Register 珥덇린�솕 
	init_serial() ;   // Serial Port (USART0, 1) 珥덇린�솕
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

    TCNT0 = 256 - 156;       //  �궡遺��겢�윮二쇨린 = 1024/ (16x10^6) = 64 usec,  
                             //  �삤踰꾪뵆濡쒖씤�꽣�읇�듃 二쇨린 = 10msec
                             //  156 = 10msec/ 64usec

    time_index++ ; 
<<<<<<< HEAD


    if( time_index == 200 )    // 샘플링주기 10msec
    {       	
	   	getDHT();
	   	sendDHT();
	   	sendDust();
		if(sensor_time_index == 0)
		{
			// UCSR0B &= ~0x80;   // UART0 수신(RX) 완료 인터럽트 금지 
			sensor_time_index = 1;
		}
		else
		{
			// UCSR0B |= 0x80;   // UART0 수신(RX) 완료 인터럽트 허용
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
	
    if( time_index == 200 )    // �깦�뵆留곸＜湲� 10msec
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

    if( rdata != '.' )                      // �닔�떊�맂 �뜲�씠�꽣媛� 留덉��留� 臾몄옄瑜� �굹����궡�뒗 �뜲�씠�꽣(留덉묠�몴)媛� �븘�땲硫�
    {
        recv_data[r_cnt] = rdata;        //  �닔�떊�맂 臾몄옄 ����옣 
	    r_cnt++;                         //  �닔�떊 臾몄옄 媛��닔 利앷�� 

		new_recv_flag = 0;
    }
    else if(  rdata == '.' )                // �닔�떊�맂�뜲�씠�꽣媛� 留덉��留� 臾몄옄瑜� �굹����궡�뒗 �뜲�씠�꽣(留덉묠�몴) �씠硫�
    {
        recv_cnt = r_cnt ;                  // �닔�떊�맂 �뜲�씠�꽣 諛붿씠�듃�닔 ����옣
        r_cnt = 0;  
        
		new_recv_flag = 1;
    }
}


void pin_init()
{
<<<<<<< HEAD
	DDRB |= 0x80;    //  PWM 포트: OC2( PB7 ) 출력설정
=======
	DDRB |= 0x80;    //  PWM �룷�듃: OC2( PB7 ) 異쒕젰�꽕�젙
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
    TCNT0 = 256 - 100;       //  �궡遺��겢�윮二쇨린 = 8/ (16x10^6) = 0.5 usec,  
                             //  �삤踰꾪뵆濡쒖씤�꽣�읇�듃 二쇨린 = 50usec
                             //  156 = 50usec/ 0.5use

	TIMSK = 0x01;  // Timer0 overflow interrupt enable 
	
	TCCR0 |= 0x07; // Clock Prescaler N=1024 (Timer 0 Start)

/**** Motor PWM  ******/

<<<<<<< HEAD
	TCCR2 |= 0x68;   //  Trigger signal (OC2)   발생 :  WGM20(bit6)=1,  WGM21(bit3)=1,  COM21(bit5)=1, COM20(bit4)=0 ,  
	TCCR2 |= 0x05;   //  1024분주,  내부클럭주기 = 64usec  : CS22(bit2) = 1, CS21(bit1) = 0,  CS20(bit0) = 1 
=======
	TCCR2 |= 0x68;   //  Trigger signal (OC2)   諛쒖깮 :  WGM20(bit6)=1,  WGM21(bit3)=1,  COM21(bit5)=1, COM20(bit4)=0 ,  
	TCCR2 |= 0x05;   //  1024遺꾩＜,  �궡遺��겢�윮二쇨린 = 64usec  : CS22(bit2) = 1, CS21(bit1) = 0,  CS20(bit0) = 1 
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db

	Pos_CMD = 90 ;
    OCR2 = ( 135 * Pos_CMD )/900 + 10 ; 

	sei();         // Global Interrupt Enable 
}
void init_serial(void)
{
    UCSR0A = 0x00;                    //珥덇린�솕
    UCSR0B = 0x18;                    //�넚�닔�떊�뿀�슜,  �넚�닔�떊 �씤�꽣�읇�듃 湲덉��
    UCSR0C = 0x06;                    //�뜲�씠�꽣 �쟾�넚鍮꾪듃 �닔 8鍮꾪듃濡� �꽕�젙.
    
    UBRR0H = 0x00;
    UBRR0L = 103;                     //Baud Rate 9600 
<<<<<<< HEAD
	UCSR0B |= 0x80;   // UART0 수신(RX) 완료 인터럽트 허용 블루투스 통신

	UCSR1A = 0x00;                    //초기화
    UCSR1B = 0x18;                    //송수신허용,  송수신 인터럽트 금지
    UCSR1C = 0x06;                    //데이터 전송비트 수 8비트로 설정.
=======

	UCSR0B |= 0x80;   // UART0 �넚�떊(RX) �셿猷� �씤�꽣�읇�듃 �뿀�슜

	UCSR1A = 0x00;                    //珥덇린�솕
    UCSR1B = 0x18;                    //�넚�닔�떊�뿀�슜,  �넚�닔�떊 �씤�꽣�읇�듃 湲덉��
    UCSR1C = 0x06;                    //�뜲�씠�꽣 �쟾�넚鍮꾪듃 �닔 8鍮꾪듃濡� �꽕�젙.
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
    
    UBRR1H = 0x00;
    UBRR1L = 103;                     //Baud Rate 9600 

<<<<<<< HEAD
	UCSR1B |= 0x80;   // UART1 수신(RX) 완료 인터럽트 허용 블루투스 통신
=======
	UCSR1B |= 0x80;   // UART1 �넚�떊(RX) �셿猷� �씤�꽣�읇�듃 �뿀�슜
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
		//////////// 습도 Display
=======
		//////////// �뒿�룄 Display
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
	Pos_CMD = 0 ;   		                 // 서보 위치 명령 =  0 도 (왼쪽 끝)  
=======
	Pos_CMD = 0 ;   		                 // �꽌蹂� �쐞移� 紐낅졊 =  0 �룄 (�쇊履� �걹)  
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
    OCR2 = ( 135 * Pos_CMD )/900  + 10  ;   
}
void Servo_Off()
{
<<<<<<< HEAD
	Pos_CMD = 180 ;   		                 // 서보 위치 명령 =  180 도 (오른쪽 끝)  
=======
	Pos_CMD = 180 ;   		                 // �꽌蹂� �쐞移� 紐낅졊 =  180 �룄 (�삤瑜몄そ �걹)  
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
    OCR2 = ( 135 * Pos_CMD )/900  + 10  ;   
}

void SerialPutChar(char ch)
{
<<<<<<< HEAD
	while(!(UCSR1A & (1<<UDRE)));			// 버퍼가 빌 때를 기다림
  	UDR1 = ch;								// 버퍼에 문자를 쓴다
} // 한 문자를 송신한다.
=======
	while(!(UCSR0A & (1<<UDRE)));			// 踰꾪띁媛� 鍮� �븣瑜� 湲곕떎由�
  	UDR0 = ch;								// 踰꾪띁�뿉 臾몄옄瑜� �벖�떎
} // �븳 臾몄옄瑜� �넚�떊�븳�떎.
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db

void SerialPutString(char *str)
 {

    while(*str != '\0')          // �닔�떊�맂 臾몄옄媛� Null 臾몄옄( 0x00 )媛� �븘�땲硫� 
    {
        SerialPutChar(*str++);
    }
} // 臾몄옄�뿴�쓣 �넚�떊�븳�떎.
  // �엯�젰   : str - �넚�떊�븳 臾몄옄�뿴�쓣 ����옣�븷 踰꾪띁�쓽 二쇱냼


void Display_Number_LCD( unsigned int num, unsigned char digit )       // 遺��샇�뾾�뒗 �젙�닔�삎 蹂��닔瑜� 10吏꾩닔 �삎�깭濡� LCD �뿉 �뵒�뒪�뵆�젅�씠 
{

	HexToDec( num, 10); //10吏꾩닔濡� 蹂��솚 

	if( digit == 0 )     digit = 1 ;
	if( digit > 5 )      digit = 5 ;
 
    if( digit >= 5 )     LcdPutchar( NumToAsc(cnumber[4]) );  // 10000�옄由� �뵒�뒪�뵆�젅�씠
	
	if( digit >= 4 )     LcdPutchar(NumToAsc(cnumber[3]));    // 1000�옄由� �뵒�뒪�뵆�젅�씠 

	if( digit >= 3 )     LcdPutchar(NumToAsc(cnumber[2]));    // 100�옄由� �뵒�뒪�뵆�젅�씠 

	if( digit >= 2 )     LcdPutchar(NumToAsc(cnumber[1]));    // 10�옄由� �뵒�뒪�뵆�젅�씠

	if( digit >= 1 )     LcdPutchar(NumToAsc(cnumber[0]));    //  1�옄由� �뵒�뒪�뵆�젅�씠

}


void Display_TMP_LCD( unsigned int tp  )       // �삩�룄瑜� 10吏꾩닔 �삎�깭濡� LCD �뿉 �뵒�뒪�뵆�젅�씠 
{

	HexToDec( tp, 10); //10吏꾩닔濡� 蹂��솚 

    LcdPutchar(NumToAsc(cnumber[2]) );   // 10�옄由� �뵒�뒪�뵆�젅�씠
	
    LcdPutchar(NumToAsc(cnumber[1]));    // 1�옄由� �뵒�뒪�뵆�젅�씠 

    LcdPuts( ".");                       // �냼�닽�젏(.) �뵒�뒪�뵆�젅�씠 

    LcdPutchar(NumToAsc(cnumber[0]));    // 0.1 �옄由� �뵒�뒪�뵆�젅�씠 
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
	for(; n>0; n--)		// 1msec �떆媛� 吏��뿰�쓣 n�쉶 諛섎났
		_delay_ms(1);		// 1msec �떆媛� 吏��뿰
}

void usec_delay(unsigned int n)
{	
	for(; n>0; n--)		// 1usec �떆媛� 吏��뿰�쓣 n�쉶 諛섎났
		_delay_us(1);		// 1usec �떆媛� 吏��뿰
}
