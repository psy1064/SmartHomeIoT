#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h> 
#include <util/delay.h> 
#include <string.h>
#include "lcd.h"

// �������� ���
static volatile unsigned char Pos_CMD = 0 ;    	// ���� ��ġ ��� ( ���� : 0 - 180,  ����:  �� )
void Servo_On();							  	// ���� ����ġ On
void Servo_Off();								// ���� ����ġ Off
void Servo_SET();								// ���� ����ġ �߰����� 

// �½��� ����
void getDHT();									// ���� ������ ����
char i_rh[5], d_rh[5], i_temp[5], d_temp[5];	// ����, �µ� ����(8��Ʈ + '.' 8��Ʈ)
uint8_t I_RH,D_RH,I_Temp,D_Temp,CheckSum;		// 8��Ʈ ����

// �̼����� ����
static char dust_count = 0;						
uint8_t dust[32] = {0};							// 32���� �����͸� ���� �迭
uint16_t pm10 ;									// 16��Ʈ �̼����� ��
uint16_t pm10_old = 0;							// ���� Ƣ�°��� �����ϱ� ���� ���� ��
char pm[3] = {0};								// �̼����� �� ���� �迭

// ������� ���
void init_serial(void) ;  						// Serial �����Ʈ ����(USART0, USART1)
void SerialPutChar(char ch);					// �ѱ��� ����
void SerialPutString(char str[]);				// ���ڿ� ����
void sendData();								// ���� �� ����

static volatile char recv_cnt = 0, rdata = 0, new_recv_flag = 0 ;  
static volatile char recv_data[3] = {0,0,0}; 
static volatile unsigned char Command_Error_Flag = 0 ; 

static volatile char Cmd_Message_1[] = {"on" }; // Motor On  ��ɾ�
static volatile char Cmd_Message_2[] = {"off"}; // Motor Off ��ɾ�

// ���� 
void Display_Number_LCD( unsigned int num, unsigned char digit ) ;  // ��ȣ���� ������ ������ 10���� ���·� LCD �� ���÷��� 
void HexToDec( unsigned short num, unsigned short radix); 
char NumToAsc( unsigned char Num ); 
void msec_delay(unsigned int n);
void usec_delay(unsigned int n);

void pin_init();													// �� ���� ����
void init();														// Interrupt , Timer, Register ����

static volatile unsigned char cnumber[5] = {0, 0, 0, 0, 0}; 

int main() 
{   
	char eq_count1=0, eq_count2=0, cmd_data = 0xFF ;  	  
    unsigned char i=0 ;
	
	pin_init();		  	// �� ����
	init();			  	// Interrupt , Timer, Register ����
	init_serial() ;   	// Serial �����Ʈ ����(USART0, USART1)
	Servo_SET();		// ���� �߾ӿ� ��ġ
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
			msec_delay(100);
			LcdCommand(ALLCLR);
			LcdMove(0,0);
			LcdPuts("Turn On!!");
		} // On ��� ������ ��
		else if(cmd_data == 2)
		{
			Servo_Off();
			msec_delay(100);
			LcdCommand(ALLCLR);
			LcdMove(0,0);
			LcdPuts("Turn Off!!");

		} // Off ��� ������ ��
		else if(cmd_data == 0xFE)
		{
			LcdCommand(ALLCLR);
			LcdMove(0,0);
			LcdPuts("Command Error!!");
			LcdMove(1,0);
			LcdPuts(recv_data);
		}
		Servo_SET();
		cmd_data = 0xFF;
	}
} 

ISR(TIMER0_OVF_vect)   // Timer0 overflow interrupt (10 msec)  service routine
{

	static unsigned short  time_index = 0, send_time_index = 0;

    TCNT0 = 256 - 156;       	// ����Ŭ���ֱ� = 1024/ (16x10^6) = 64 usec,  
                             	// �����÷����ͷ�Ʈ �ֱ� = 10msec
                             	// 156 = 10msec/ 64usec

    time_index++ ; 

    if( time_index == 200 )		// ���ø��ֱ� 10msec * 200 = 2sec
    {       	
	   	getDHT();				// DHT ���� ������ ����
	   	sendData();				// ������ ������ ����
		
		time_index = 0; 
   }
}

ISR( USART0_RX_vect)
{
	dust[dust_count] = UDR0;
	dust_count ++;
	static unsigned char first = 0;
	if(dust_count >= 32)
	{
		if((dust[0] == 0x42)&&(dust[1] == 0x4d))
		{
			pm10 = (dust[14] << 8) + (dust[15]);
			
			if(first == 0)
			{
				first = 1;
				pm10_old = pm10;
			}
			else
			{
				if((pm10 - pm10_old) > 20)
				{
					pm10 = pm10_old;
				}
				else
				{
					pm10_old = pm10;
				}
			} // ���� Ƣ�°��� ����
			itoa(pm10,pm,10);

			LcdMove(1,5);
			LcdPuts(pm);
		}
		dust_count = 0 ;		
	}
} // PMS7003 ���� ������ ���� ���ͷ�Ʈ 

ISR( USART1_RX_vect )
{
    static unsigned char r_cnt = 0 ;

    rdata = UDR1; 

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
} // ������� ���� ���ͷ�Ʈ 

void pin_init()
{
	DDRB |= 0x80;    //  PWM ��Ʈ: OC2( PB7 ) ��¼���
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
	LcdPuts("Dust=");

	TCCR0 = 0x00; 
    TCNT0 = 256 - 100;      // ����Ŭ���ֱ� = 8/ (16x10^6) = 0.5 usec,  
                            // �����÷����ͷ�Ʈ �ֱ� = 50usec
                            // 156 = 50usec/ 0.5use

	TIMSK = 0x01;  			// Timer0 overflow interrupt enable 
	
	TCCR0 |= 0x07; 			// Clock Prescaler N=1024 (Timer 0 Start)

	// Timer0 Overflow Interrupt ���� 



	TCCR2 |= 0x68;   		// Trigger signal (OC2)   �߻� :  WGM20(bit6)=1,  WGM21(bit3)=1,  COM21(bit5)=1, COM20(bit4)=0 ,  
	TCCR2 |= 0x05;   		// 1024����,  ����Ŭ���ֱ� = 64usec  : CS22(bit2) = 1, CS21(bit1) = 0,  CS20(bit0) = 1 

	Pos_CMD = 90 ;
    OCR2 = ( 135 * Pos_CMD )/900 + 10 ; 
	
	// Motor PWM ���� 

	sei();         // Global Interrupt Enable 
}
void init_serial(void)
{
    UCSR0A = 0x00;          // �ʱ�ȭ
    UCSR0B = 0x18;          // �ۼ������,  �ۼ��� ���ͷ�Ʈ ����
    UCSR0C = 0x06;          // ������ ���ۺ�Ʈ �� 8��Ʈ�� ����.
    
    UBRR0H = 0x00;
    UBRR0L = 103;           // Baud Rate 9600 

	UCSR0B |= 0x80;   		// UART0 ����(RX) �Ϸ� ���ͷ�Ʈ ��� PMS7003 ������ ����

	UCSR1A = 0x00;          // �ʱ�ȭ
    UCSR1B = 0x18;          // �ۼ������,  �ۼ��� ���ͷ�Ʈ ����
    UCSR1C = 0x06;			// ������ ���ۺ�Ʈ �� 8��Ʈ�� ����.
    
    UBRR1H = 0x00;
    UBRR1L = 103;			// Baud Rate 9600 

	UCSR1B |= 0x80;   		// UART1 ����(RX) �Ϸ� ���ͷ�Ʈ ��� ������� ���
}

void sendData()
{
	SerialPutString(i_temp);
	SerialPutChar('.');
	SerialPutString(d_temp);
	SerialPutChar(',');
	SerialPutString(i_rh);
	SerialPutChar('.');
	SerialPutString(d_rh);
	SerialPutChar(',');
	SerialPutString(pm);
	SerialPutChar('/');
}

void getDHT()
{
	Request();					// ������ ���� ��û
	Response();					// ��û ���
	I_RH=Receive_data();		// ���� ���� 8��Ʈ ����
	D_RH=Receive_data();		// ���� ���� 8��Ʈ ����
	I_Temp=Receive_data();		// �µ� ���� 8��Ʈ ����
	D_Temp=Receive_data();		// �µ� ���� 8��Ʈ ����
	CheckSum=Receive_data();	// CheckSum ��Ʈ = ���� 16��Ʈ + �µ� 16��Ʈ 
	
	if ((I_RH + D_RH + I_Temp + D_Temp) != CheckSum)
	{
		LcdMove(0,0);
		LcdPuts("Error");
	}
		
	else
	{	
		LcdCommand(ALLCLR);
		LcdMove(0,0);  
		LcdPuts("HM=");
		LcdMove(0,8); 
		LcdPuts("TP= ");
		LcdMove(1,0);
		LcdPuts("Dust=");
		LcdMove(1,5);
		LcdPuts(pm);

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

		// ���� Display

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
		
		// �µ� Display
	}
				
	msec_delay(10);
}

void Servo_On()
{
	Pos_CMD = 150 ;   		                 // ���� ��ġ ��� =  0 �� (���� ��)  
    OCR2 = ( 135 * Pos_CMD )/900  + 10  ;   
}
void Servo_Off()
{
	Pos_CMD = 30 ;   		                 // ���� ��ġ ��� =  180 �� (������ ��)  
    OCR2 = ( 135 * Pos_CMD )/900  + 10  ;   
}
void Servo_SET()
{
	Pos_CMD = 90 ;   		                 // ���� ��ġ ��� =  90 �� (������ ��)  
    OCR2 = ( 135 * Pos_CMD )/900  + 10  ;   
}

void SerialPutChar(char ch)
{
	while(!(UCSR1A & (1<<UDRE)));			// ���۰� �� ���� ��ٸ�
  	UDR1 = ch;								// ���ۿ� ���ڸ� ����
} // �� ���ڸ� �۽��Ѵ�.

void SerialPutString(char *str)
{
    while(*str != '\0')          			// ���ŵ� ���ڰ� Null ����( 0x00 )�� �ƴϸ� 
    {
        SerialPutChar(*str++);
    }
} // ���ڿ��� �۽��Ѵ�.

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
