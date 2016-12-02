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

//标志串口正在发送数据
static bit Uart_Busy = 0;
static bit Uart2_Busy = 0;

//串口初始化，串口波特率发生器用的是定时器2
void Uart_Init(void)
{
	if(BAUD > 9600)
		PCON |= 0x80;	//波特率加倍
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

	//AUXR = 0x40;      		//定时器工作在1T模式，这一位一定要设置，否则进入不了中断
	//AUXR = 0x54;
	//AUXR = 0xD4;			//T0为1T模式；T1位1T模式，T1为串口1波特率发生器；T2为1T模式，并启动定时器2，T2为串口2波特率发生器
	AUXR = 0x55;			//定时器2为串口1的波特率发生器
	//AUXR = 0x15;
	
	AUXR1 &= ~0xC0;			//串口1在P30/P31

//	TMOD |= 0x20;           //Set Timer1 as 8-bit auto reload mode
//	if(BAUD <= 9600)
//	{
//		TH1 = TL1 = 256 - (FOSC/32/BAUD); //Set auto-reload vaule
//	}	 
//	else
//	{
//		TH1 = TL1 = 256 - (FOSC/32/BAUD*2);
//	}
	
	T2L = (65536 - (FOSC/4/BAUD));   //设置波特率重装值
	T2H = (65536 - (FOSC/4/BAUD))>>8;

	IE2 = 0x01; 			//使能串口2中断
	ES = 1;                 //使能串口1中断
	//TR1 = 1;                //启动定时器1

	IP |= 0x10;  //串口1优先级高
	IP2 |= 0x01;  //串口2优先级高
}

/*----------------------------
UART interrupt service routine
----------------------------*/
void Uart_Isr() interrupt 4 using 3   //RTX定时中断用了1
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
void Uart2_Isr() interrupt 8 using 3   //RTX定时中断用了1
{
	uint8_t Buf = S2BUF;
	qsize pos;

    if (S2CON & S2RI)
    {
        S2CON &= ~S2RI;         //??S2RI?

#if 1
		//queue_push(Buf);

		//调用上面的push函数有问题
		pos = (que._head+1)%QUEUE_MAX_SIZE;
		if(pos!=que._tail)//非满状态
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
        Uart2_Busy = 0;         //清忙标志
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

void uart_printf(char *fmt,...)//这个是我们的printf函数
{
	va_list ap;//typedef char *va_list; va_list是char型的指针
	
	char xdata string[1024];//访问外部RAM 两字节对齐
	
	va_start(ap,fmt);//这个函数的功能是，找到第一个可变形参的地址，并把地址赋给ap
	vsprintf(string,fmt,ap);//其实这个函数才是核心函数，没研究。。。
	Uart_SendString(string);//这个函数就是发送字符串函数，通过上一个函数，就把该提取的东西都提取了
	va_end(ap);//结束函数
}

void uart_printf_none(char *fmt,...)
{
	;
}
