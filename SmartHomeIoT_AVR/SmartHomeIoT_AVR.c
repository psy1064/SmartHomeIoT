#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h> 
#include <util/delay.h> 
#include <string.h>
#include "lcd.h"


// �½��� ����
void getDHT();
uint8_t I_RH,D_RH,I_Temp,D_Temp,CheckSum;
char i_rh[5], d_rh[5], i_temp[5], d_temp[5];

// LCD
void Display_Number_LCD( unsigned int num, unsigned char digit ) ;    // ��ȣ���� ������ ������ 10���� ���·� LCD �� ���÷��� 

// Blue tooth communication
void init_serial(void) ;  //  Serial �����Ʈ �ʱ�ȭ
void SerialPutChar(char ch);
void SerialPutString(char str[]);
void sendDHT();

static volatile  char  recv_cnt = 0, rdata = 0, new_recv_flag = 0, rdata_old = 0 ;  
static volatile  char recv_data[3] = {0,0,0};
static volatile unsigned char   Command_Error_Flag = 0 ; 

static volatile char Cmd_Message_1[] = {"on" } ;     //  Blutooth Command
static volatile char Cmd_Message_2[] = {"off"} ;  

// ���� 
void HexToDec( unsigned short num, unsigned short radix); 
char NumToAsc( unsigned char Num ); 
void msec_delay(unsigned int n);
void usec_delay(unsigned int n);
void pin_init();		// �� ���� �ʱ�ȭ
void init();			// �ʱ� ����

static volatile unsigned char cnumber[5] = {0, 0, 0, 0, 0}; 

int main() 
{   
	char eq_count1=0, eq_count2=0, cmd_data = 0xFF  ;  	  
    unsigned char i=0 ;
	DDRA |= 0x10;
	PORTA &= ~0x10;
	
	pin_init();		  // Pin �ʱ�ȭ
	init();			  // Interrupt , Timer, Register �ʱ�ȭ 
	init_serial() ;   // Serial Port (USART0) �ʱ�ȭ

	UCSR0B |=  0x80  ;      // UART1 �۽�(RX) �Ϸ� ���ͷ�Ʈ ���

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
			// Servo_On();
			PORTA |= 0x10;
			LcdCommand(ALLCLR);
			LcdMove(0,0);
			LcdPuts("Turn On!!");
		}
		else if(cmd_data == 2)
		{
			// Servo_On();
			PORTA &= ~0x10;
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

    TCNT0 = 256 - 156;       //  ����Ŭ���ֱ� = 1024/ (16x10^6) = 64 usec,  
                             //  �����÷����ͷ�Ʈ �ֱ� = 10msec
                             //  156 = 10msec/ 64usec

    time_index++ ; 
	
    if( time_index == 500 )    // ���ø��ֱ� 10msec
    {
       time_index = 0; 
	   getDHT();
	   sendDHT();
	   
   }
}
ISR(  USART0_RX_vect )
{
    static unsigned char r_cnt = 0 ;

    rdata = UDR0; 

    if( rdata != '.' )                      // ���ŵ� �����Ͱ� ������ ���ڸ� ��Ÿ���� ������(��ħǥ)�� �ƴϸ�
    {
        recv_data[r_cnt] = rdata;        //  ���ŵ� ���� ���� 
	    r_cnt++;                         //  ���� ���� ���� ���� 

		new_recv_flag = 0;
    }
    else if(  rdata == '.' )                // ���ŵȵ����Ͱ� ������ ���ڸ� ��Ÿ���� ������(��ħǥ) �̸�
    {
        recv_cnt = r_cnt ;                  // ���ŵ� ������ ����Ʈ�� ����
        r_cnt = 0;  
        
		new_recv_flag = 1;
    }
}
void pin_init()
{
	DDRB |= 0x10;     // LED (PB4 : ��¼��� )
	PORTB &= ~0x10;   // PB4  : High ( LED OFF) 
}
void init()
{
	LcdInit();

	LcdCommand(ALLCLR);
	LcdMove(0,0);  
	LcdPuts("HUM=");
	LcdMove(1,0); 
	LcdPuts("TMP= ");


/**** Timer0 Overflow Interrupt  ******/

	TCCR0 = 0x00; 
    TCNT0 = 256 - 100;       //  ����Ŭ���ֱ� = 8/ (16x10^6) = 0.5 usec,  
                             //  �����÷����ͷ�Ʈ �ֱ� = 50usec
                             //  156 = 50usec/ 0.5use

	TIMSK = 0x01;  // Timer0 overflow interrupt enable 
	
	sei();         // Global Interrupt Enable 
	TCCR0 |= 0x07; // Clock Prescaler N=1024 (Timer 0 Start)
}
void init_serial(void)
{
    UCSR0A = 0x00;                    //�ʱ�ȭ
    UCSR0B = 0x18;                    //�ۼ������,  �ۼ��� ���ͷ�Ʈ ����
    UCSR0C = 0x06;                    //������ ���ۺ�Ʈ �� 8��Ʈ�� ����.
    
    UBRR0H = 0x00;
    UBRR0L = 103;                     //Baud Rate 9600 
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
		LcdCommand(ALLCLR);
		LcdMove(0,0);  
		LcdPuts("HUM=");
		LcdMove(1,0); 
		LcdPuts("TMP= ");

		itoa(I_RH,i_rh,10);
		LcdMove(0,4);
		LcdPuts(i_rh);
		LcdMove(0,6);
		LcdPuts(".");
			
		itoa(D_RH,d_rh,10);
		LcdMove(0,7);
		LcdPuts(d_rh);
		LcdMove(0,8);
		LcdPuts("%");
		itoa(I_Temp,i_temp,10);
		LcdMove(1,4);
		LcdPuts(i_temp);
		LcdMove(1,6);
		LcdPuts(".");
			
		itoa(D_Temp,d_temp,10);
		LcdMove(1,7);
		LcdPuts(d_temp);
		LcdMove(1,8);
		LcdPuts("C");
		
	}
				
	_delay_ms(10);
}


void SerialPutChar(char ch)
{
	while(!(UCSR0A & (1<<UDRE)));			// ���۰� �� ���� ��ٸ�
  	UDR0 = ch;								// ���ۿ� ���ڸ� ����
} // �� ���ڸ� �۽��Ѵ�.

void SerialPutString(char *str)
 {

    while(*str != '\0')          // ���ŵ� ���ڰ� Null ����( 0x00 )�� �ƴϸ� 
    {
        SerialPutChar(*str++);
    }
} // ���ڿ��� �۽��Ѵ�.
  // �Է�   : str - �۽��� ���ڿ��� ������ ������ �ּ�


void Display_Number_LCD( unsigned int num, unsigned char digit )       // ��ȣ���� ������ ������ 10���� ���·� LCD �� ���÷��� 
{

	HexToDec( num, 10); //10������ ��ȯ 

	if( digit == 0 )     digit = 1 ;
	if( digit > 5 )      digit = 5 ;
 
    if( digit >= 5 )     LcdPutchar( NumToAsc(cnumber[4]) );  // 10000�ڸ� ���÷���
	
	if( digit >= 4 )     LcdPutchar(NumToAsc(cnumber[3]));    // 1000�ڸ� ���÷��� 

	if( digit >= 3 )     LcdPutchar(NumToAsc(cnumber[2]));    // 100�ڸ� ���÷��� 

	if( digit >= 2 )     LcdPutchar(NumToAsc(cnumber[1]));    // 10�ڸ� ���÷���

	if( digit >= 1 )     LcdPutchar(NumToAsc(cnumber[0]));    //  1�ڸ� ���÷���

}


void Display_TMP_LCD( unsigned int tp  )       // �µ��� 10���� ���·� LCD �� ���÷��� 
{

	HexToDec( tp, 10); //10������ ��ȯ 

    LcdPutchar(NumToAsc(cnumber[2]) );   // 10�ڸ� ���÷���
	
    LcdPutchar(NumToAsc(cnumber[1]));    // 1�ڸ� ���÷��� 

    LcdPuts( ".");                       // �Ҽ���(.) ���÷��� 

    LcdPutchar(NumToAsc(cnumber[0]));    // 0.1 �ڸ� ���÷��� 
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
	for(; n>0; n--)		// 1msec �ð� ������ nȸ �ݺ�
		_delay_ms(1);		// 1msec �ð� ����
}

void usec_delay(unsigned int n)
{	
	for(; n>0; n--)		// 1usec �ð� ������ nȸ �ݺ�
		_delay_us(1);		// 1usec �ð� ����
}



