#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h> 
#include <util/delay.h> 
#include <string.h>
#include "lcd.h"

<<<<<<< HEAD
// �������� ���
static volatile unsigned char    Pos_CMD = 0 ;    // ���� ��ġ ��� ( ���� : 0 - 180,  ����:  �� )
=======
// 서보모터 사용
static volatile unsigned char    Pos_CMD = 0 ;    // 서보 위치 명령 ( 범위 : 0 - 180,  단위:  도 )
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
void Servo_On();
void Servo_Off();

// 온습도 센서
void getDHT();
uint8_t I_RH,D_RH,I_Temp,D_Temp,CheckSum;
char i_rh[5], d_rh[5], i_temp[5], d_temp[5];

<<<<<<< HEAD
// �̼����� ����
static char dust_count = 0;
char dust[32] = {0};
char pm10[2], pm10d[2];

=======
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
// Blue tooth communication
void init_serial(void) ;  //  Serial 토신포트 초기화
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
// ���� 
void Display_Number_LCD( unsigned int num, unsigned char digit ) ;    // ��ȣ���� ������ ������ 10���� ���·� LCD �� ���÷��� 
=======
// 범용 
void Display_Number_LCD( unsigned int num, unsigned char digit ) ;    // 부호없는 정수형 변수를 10진수 형태로 LCD 에 디스플레이 
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
void HexToDec( unsigned short num, unsigned short radix); 
char NumToAsc( unsigned char Num ); 
void msec_delay(unsigned int n);
void usec_delay(unsigned int n);
<<<<<<< HEAD
void pin_init();		// �� ���� �ʱ�ȭ
void init();			// �ʱ� ����
=======
void pin_init();		// 핀 설정 초기화
void init();			// 초기 설정
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
static volatile unsigned char cnumber[5] = {0, 0, 0, 0, 0}; 

int main() 
{   
	char eq_count1=0, eq_count2=0, cmd_data = 0xFF  ;  	  
    unsigned char i=0 ;
	
<<<<<<< HEAD
	pin_init();		  // Pin �ʱ�ȭ
	init();			  // Interrupt , Timer, Register �ʱ�ȭ 
	init_serial() ;   // Serial Port (USART0, 1) �ʱ�ȭ
=======
	pin_init();		  // Pin 초기화
	init();			  // Interrupt , Timer, Register 초기화 
	init_serial() ;   // Serial Port (USART0, 1) 초기화
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

    TCNT0 = 256 - 156;       //  내부클럭주기 = 1024/ (16x10^6) = 64 usec,  
                             //  오버플로인터럽트 주기 = 10msec
                             //  156 = 10msec/ 64usec

    time_index++ ; 
<<<<<<< HEAD


    if( time_index == 200 )    // ���ø��ֱ� 10msec
    {       	
	   	getDHT();
	   	sendDHT();
	   	sendDust();
		if(sensor_time_index == 0)
		{
			// UCSR0B &= ~0x80;   // UART0 ����(RX) �Ϸ� ���ͷ�Ʈ ���� 
			sensor_time_index = 1;
		}
		else
		{
			// UCSR0B |= 0x80;   // UART0 ����(RX) �Ϸ� ���ͷ�Ʈ ���
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
	
    if( time_index == 200 )    // 샘플링주기 10msec
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

    if( rdata != '.' )                      // 수신된 데이터가 마지막 문자를 나타내는 데이터(마침표)가 아니면
    {
        recv_data[r_cnt] = rdata;        //  수신된 문자 저장 
	    r_cnt++;                         //  수신 문자 갯수 증가 

		new_recv_flag = 0;
    }
    else if(  rdata == '.' )                // 수신된데이터가 마지막 문자를 나타내는 데이터(마침표) 이면
    {
        recv_cnt = r_cnt ;                  // 수신된 데이터 바이트수 저장
        r_cnt = 0;  
        
		new_recv_flag = 1;
    }
}


void pin_init()
{
<<<<<<< HEAD
	DDRB |= 0x80;    //  PWM ��Ʈ: OC2( PB7 ) ��¼���
=======
	DDRB |= 0x80;    //  PWM 포트: OC2( PB7 ) 출력설정
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
    TCNT0 = 256 - 100;       //  내부클럭주기 = 8/ (16x10^6) = 0.5 usec,  
                             //  오버플로인터럽트 주기 = 50usec
                             //  156 = 50usec/ 0.5use

	TIMSK = 0x01;  // Timer0 overflow interrupt enable 
	
	TCCR0 |= 0x07; // Clock Prescaler N=1024 (Timer 0 Start)

/**** Motor PWM  ******/

<<<<<<< HEAD
	TCCR2 |= 0x68;   //  Trigger signal (OC2)   �߻� :  WGM20(bit6)=1,  WGM21(bit3)=1,  COM21(bit5)=1, COM20(bit4)=0 ,  
	TCCR2 |= 0x05;   //  1024����,  ����Ŭ���ֱ� = 64usec  : CS22(bit2) = 1, CS21(bit1) = 0,  CS20(bit0) = 1 
=======
	TCCR2 |= 0x68;   //  Trigger signal (OC2)   발생 :  WGM20(bit6)=1,  WGM21(bit3)=1,  COM21(bit5)=1, COM20(bit4)=0 ,  
	TCCR2 |= 0x05;   //  1024분주,  내부클럭주기 = 64usec  : CS22(bit2) = 1, CS21(bit1) = 0,  CS20(bit0) = 1 
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db

	Pos_CMD = 90 ;
    OCR2 = ( 135 * Pos_CMD )/900 + 10 ; 

	sei();         // Global Interrupt Enable 
}
void init_serial(void)
{
    UCSR0A = 0x00;                    //초기화
    UCSR0B = 0x18;                    //송수신허용,  송수신 인터럽트 금지
    UCSR0C = 0x06;                    //데이터 전송비트 수 8비트로 설정.
    
    UBRR0H = 0x00;
    UBRR0L = 103;                     //Baud Rate 9600 
<<<<<<< HEAD
	UCSR0B |= 0x80;   // UART0 ����(RX) �Ϸ� ���ͷ�Ʈ ��� ������� ���

	UCSR1A = 0x00;                    //�ʱ�ȭ
    UCSR1B = 0x18;                    //�ۼ������,  �ۼ��� ���ͷ�Ʈ ����
    UCSR1C = 0x06;                    //������ ���ۺ�Ʈ �� 8��Ʈ�� ����.
=======

	UCSR0B |= 0x80;   // UART0 송신(RX) 완료 인터럽트 허용

	UCSR1A = 0x00;                    //초기화
    UCSR1B = 0x18;                    //송수신허용,  송수신 인터럽트 금지
    UCSR1C = 0x06;                    //데이터 전송비트 수 8비트로 설정.
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
    
    UBRR1H = 0x00;
    UBRR1L = 103;                     //Baud Rate 9600 

<<<<<<< HEAD
	UCSR1B |= 0x80;   // UART1 ����(RX) �Ϸ� ���ͷ�Ʈ ��� ������� ���
=======
	UCSR1B |= 0x80;   // UART1 송신(RX) 완료 인터럽트 허용
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
		//////////// ���� Display
=======
		//////////// 습도 Display
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
	Pos_CMD = 0 ;   		                 // ���� ��ġ ��� =  0 �� (���� ��)  
=======
	Pos_CMD = 0 ;   		                 // 서보 위치 명령 =  0 도 (왼쪽 끝)  
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
    OCR2 = ( 135 * Pos_CMD )/900  + 10  ;   
}
void Servo_Off()
{
<<<<<<< HEAD
	Pos_CMD = 180 ;   		                 // ���� ��ġ ��� =  180 �� (������ ��)  
=======
	Pos_CMD = 180 ;   		                 // 서보 위치 명령 =  180 도 (오른쪽 끝)  
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db
    OCR2 = ( 135 * Pos_CMD )/900  + 10  ;   
}

void SerialPutChar(char ch)
{
<<<<<<< HEAD
	while(!(UCSR1A & (1<<UDRE)));			// ���۰� �� ���� ��ٸ�
  	UDR1 = ch;								// ���ۿ� ���ڸ� ����
} // �� ���ڸ� �۽��Ѵ�.
=======
	while(!(UCSR0A & (1<<UDRE)));			// 버퍼가 빌 때를 기다림
  	UDR0 = ch;								// 버퍼에 문자를 쓴다
} // 한 문자를 송신한다.
>>>>>>> a05713a968cfd7a8610d6accf49ef231102ee6db

void SerialPutString(char *str)
 {

    while(*str != '\0')          // 수신된 문자가 Null 문자( 0x00 )가 아니면 
    {
        SerialPutChar(*str++);
    }
} // 문자열을 송신한다.
  // 입력   : str - 송신한 문자열을 저장할 버퍼의 주소


void Display_Number_LCD( unsigned int num, unsigned char digit )       // 부호없는 정수형 변수를 10진수 형태로 LCD 에 디스플레이 
{

	HexToDec( num, 10); //10진수로 변환 

	if( digit == 0 )     digit = 1 ;
	if( digit > 5 )      digit = 5 ;
 
    if( digit >= 5 )     LcdPutchar( NumToAsc(cnumber[4]) );  // 10000자리 디스플레이
	
	if( digit >= 4 )     LcdPutchar(NumToAsc(cnumber[3]));    // 1000자리 디스플레이 

	if( digit >= 3 )     LcdPutchar(NumToAsc(cnumber[2]));    // 100자리 디스플레이 

	if( digit >= 2 )     LcdPutchar(NumToAsc(cnumber[1]));    // 10자리 디스플레이

	if( digit >= 1 )     LcdPutchar(NumToAsc(cnumber[0]));    //  1자리 디스플레이

}


void Display_TMP_LCD( unsigned int tp  )       // 온도를 10진수 형태로 LCD 에 디스플레이 
{

	HexToDec( tp, 10); //10진수로 변환 

    LcdPutchar(NumToAsc(cnumber[2]) );   // 10자리 디스플레이
	
    LcdPutchar(NumToAsc(cnumber[1]));    // 1자리 디스플레이 

    LcdPuts( ".");                       // 소숫점(.) 디스플레이 

    LcdPutchar(NumToAsc(cnumber[0]));    // 0.1 자리 디스플레이 
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
	for(; n>0; n--)		// 1msec 시간 지연을 n회 반복
		_delay_ms(1);		// 1msec 시간 지연
}

void usec_delay(unsigned int n)
{	
	for(; n>0; n--)		// 1usec 시간 지연을 n회 반복
		_delay_us(1);		// 1usec 시간 지연
}
