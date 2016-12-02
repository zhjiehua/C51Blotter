#include "stdio.h"
#include "stdarg.h"
#include "uart.h"
#include "dcmotor.h"
#include "stepmotor.h"
#include "io.h"

#include "../HMI/cmd_queue.h"

#define S2RI  0x01              //S2CON.0
#define S2TI  0x02              //S2CON.1
#define S2RB8 0x04              //S2CON.2
#define S2TB8 0x08              //S2CON.3

//��־�������ڷ�������
static bit Uart_Busy = 0;
static bit Uart2_Busy = 0;

//���ڳ�ʼ�������ڲ����ʷ������õ��Ƕ�ʱ��2
void Uart_Init(void)
{
	if(BAUD > 9600)
		PCON |= 0x80;	//�����ʼӱ�
	else
		PCON &= ~0x80;

#if (PARITYBIT == NONE_PARITY)
    SCON = 0x50;            //8-bit variable UART
	S2CON = 0x50;
#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
    SCON = 0xda;            //9-bit variable UART, parity bit initial to 1
	S2CON = 0xda;
#elif (PARITYBIT == SPACE_PARITY)
    SCON = 0xd2;            //9-bit variable UART, parity bit initial to 0
	S2CON = 0xd2;
#endif

	//AUXR = 0x40;      		//��ʱ��������1Tģʽ����һλһ��Ҫ���ã�������벻���ж�
	//AUXR = 0x54;
	//AUXR = 0xD4;			//T0Ϊ1Tģʽ��T1λ1Tģʽ��T1Ϊ����1�����ʷ�������T2Ϊ1Tģʽ����������ʱ��2��T2Ϊ����2�����ʷ�����
	AUXR = 0x55;			//��ʱ��2Ϊ����1�Ĳ����ʷ�����
	//AUXR = 0x15;
	
	AUXR1 &= ~0xC0;			//����1��P30/P31

//	TMOD |= 0x20;           //Set Timer1 as 8-bit auto reload mode
//	if(BAUD <= 9600)
//	{
//		TH1 = TL1 = 256 - (FOSC/32/BAUD); //Set auto-reload vaule
//	}	 
//	else
//	{
//		TH1 = TL1 = 256 - (FOSC/32/BAUD*2);
//	}
	
	T2L = (65536 - (FOSC/4/BAUD));   //���ò�������װֵ
	T2H = (65536 - (FOSC/4/BAUD))>>8;

	IE2 = 0x01; 			//ʹ�ܴ���2�ж�
	ES = 1;                 //ʹ�ܴ���1�ж�
	//TR1 = 1;                //������ʱ��1

	IP |= 0x10;  //����1���ȼ���
	IP2 |= 0x01;  //����2���ȼ���
}

/*----------------------------
UART interrupt service routine
----------------------------*/
void Uart_Isr() interrupt 4 using 3   //RTX��ʱ�ж�����1
{
	uint8_t Buf = SBUF;
	static uint8_t flag = 0;

    if (RI)
    {			
        RI = 0;             //Clear receive interrupt flag

		//---------------------------------------------------------
		S2BUF = Buf;

//		if(flag)
//		{
//			flag = 0;
//			LED2 = 0;
//				
//			//pDCMotor->control = Buf;
//
//			pStepMotor->SetSpeed(Buf%speedLevelSize);
//			pStepMotor->SetCMD(ENABLE);	
//		}
//		else
//		{
//			flag = 1;
//			LED2 = 1;
//
//			//pDCMotor->control = 0x00;
//
//			pStepMotor->SetSpeed(Buf%speedLevelSize);
//			pStepMotor->SetCMD(DISABLE);
//		}
		//---------------------------------------------------------
    }
    if (TI)
    {
        TI = 0;             //Clear transmit interrupt flag
        Uart_Busy = 0;           //Clear transmit busy flag
    }
}

/*----------------------------
Send a byte data to UART
Input: Data (data to be sent)
Output:None
----------------------------*/
void Uart_SendData(uint8_t Data)
{
    while (Uart_Busy);    //Wait for the completion of the previous data is sent

    ACC = Data;             //Calculate the even parity bit P (PSW.0)
    if (P)                  //Set the parity bit according to P
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 0;            //Set parity bit to 0
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 1;            //Set parity bit to 1
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 1;            //Set parity bit to 1
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 0;            //Set parity bit to 0
#endif
    }
    Uart_Busy = 1;
    SBUF = ACC;             //Send data to UART buffer
}

/*----------------------------
Send a string to UART
Input: s (address of string)
Output:None
----------------------------*/
void Uart_SendString(uint8_t *s)
{
    while (*s)              //Check the end of the string
    {
        Uart_SendData(*s++);     //Send current char and increment string ptr
    }
}

/**********************************************************************************************************/

/*----------------------------
UART2 interrupt service routine
----------------------------*/
void Uart2_Isr() interrupt 8 using 3   //RTX��ʱ�ж�����1
{
	uint8_t Buf = S2BUF;
	qsize pos;

    if (S2CON & S2RI)
    {
        S2CON &= ~S2RI;         //??S2RI?

#if 1
		//queue_push(Buf);

		//���������push����������
		pos = (que._head+1)%QUEUE_MAX_SIZE;
		if(pos!=que._tail)//����״̬
		{
			que._data[que._head] = Buf;
			que._head = pos;
		}

		//Buf = 0x00;
	 	//queue_pop(&Buf);
#endif

		SBUF = Buf;
    }
    if (S2CON & S2TI)
    {
        S2CON &= ~S2TI;         //??S2TI?
        Uart2_Busy = 0;         //��æ��־
    }
}

/*----------------------------
Send a byte data to UART2
Input: Data (data to be sent)
Output:None
----------------------------*/
void Uart2_SendData(uint8_t dat)
{
    while (Uart2_Busy);               //???????????
    ACC = dat;                  //?????P (PSW.0)
    if (P)                      //??P??????
    {
#if (PARITYBIT == ODD_PARITY)
        S2CON &= ~S2TB8;        //??????0
#elif (PARITYBIT == EVEN_PARITY)
        S2CON |= S2TB8;         //??????1
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        S2CON |= S2TB8;         //??????1
#elif (PARITYBIT == EVEN_PARITY)
        S2CON &= ~S2TB8;        //??????0
#endif
    }
    Uart2_Busy = 1;
    S2BUF = ACC;                //????UART2?????
}

/*----------------------------
Send a string to UART2
Input: s (address of string)
Output:None
----------------------------*/
void Uart2_SendString(char *s)
{
    while (*s)                  //?????????
    {
        Uart2_SendData(*s++);         //??????
    }
}

/**********************************************************************************************************/

void uart_printf(char *fmt,...)//��������ǵ�printf����
{
	va_list ap;//typedef char *va_list; va_list��char�͵�ָ��
	
	char xdata string[1024];//�����ⲿRAM ���ֽڶ���
	
	va_start(ap,fmt);//��������Ĺ����ǣ��ҵ���һ���ɱ��βεĵ�ַ�����ѵ�ַ����ap
	vsprintf(string,fmt,ap);//��ʵ����������Ǻ��ĺ�����û�о�������
	Uart_SendString(string);//����������Ƿ����ַ���������ͨ����һ���������ͰѸ���ȡ�Ķ�������ȡ��
	va_end(ap);//��������
}

void uart_printf_none(char *fmt,...)
{
	;
}
