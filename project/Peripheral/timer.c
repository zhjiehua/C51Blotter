#include "timer.h"
#include "io.h"
#include "stddef.h"

idata Timer_TypeDef Timer[TIMER_NUM];//定时器资源
Timer_TypeDef *hTimer = NULL;//定时器链表头指针

//定时器0初始化
void Timer0_Init(void)
{
    TMOD |= 0x01;                    //set timer0 as mode1 (16-bit)
    TL0 = TIMEOUT;                   //initial timer0 low byte
    TH0 = TIMEOUT >> 8;              //initial timer0 high byte
    TR0 = 1;                         //timer1 start running
    ET0 = 1;                         //enable timer0 interrupt
}

//-----------------------------------------------
/* Timer0 interrupt routine */
void Timer0_Isr() interrupt 1 using 1
{
	static uint8_t IO_FreDouble = 0;
	uint8_t i;
	Timer_TypeDef *pTimer = NULL;

    TL0 = TIMEOUT;                     //reload timer0 low byte
    TH0 = TIMEOUT >> 8;                //reload timer0 high byte

	//遍历定时器链表，更新定时器数据
	pTimer = hTimer;
	while(pTimer != NULL)
	{
		if(pTimer->Flag & 0x40)//定时器启动
		{
			pTimer->Count++;
			if(pTimer->Count == pTimer->Time)
			{
				pTimer->Flag |= 0x80;//表示定时时间到
				pTimer->Count = 0;

//				if(pTimer->Flag & 0x01)//自动重装载模式
//					pTimer->Count = 0;
//				else
					pTimer->Flag &= ~0x40;//停止定时器	
			}
		}

		pTimer = pTimer->Next;
	}
}

//创建定时器
//ID：定时器编号，具有唯一性，所有定时器都可以根据自己的ID找到
//Time：定时时间
//Mode：定时器模式，1：自动重装载	0：单次运行（未实现）
uint8_t Timer_Create(uint8_t ID, uint16_t Time, uint8_t Mode)
{
	Timer_TypeDef *pTimer = NULL;

	if(Timer[ID].Flag & 0x02)//定时器已使用
		return 1;
	
	Timer[ID].Time = Time; //记录定时时间
	Timer[ID].Count = 0;//从0开始计数
	Timer[ID].Next = 0;//标志链表尾
	Timer[ID].Flag = 0x02;//标志改定时器已使用

	if(Mode)
		Timer[ID].Flag |= 0x01;//自动重装载模式

	//如果当前没有用到定时器
	if(hTimer == NULL)
		hTimer =  &Timer[ID];
	else
	{ 	
		//遍历链表，找到链表尾
		pTimer = hTimer;
		while(pTimer->Next != NULL)
			pTimer = pTimer->Next;

		pTimer->Next = &Timer[ID];//加入到链表尾	
	}

	return 0;
}

//定时器删除
uint8_t Timer_Delete(uint8_t ID)
{
	Timer_TypeDef *pTimer = hTimer;
	Timer_TypeDef *pTimer_Last = hTimer;

	//遍历链表，找到链表尾
	while(pTimer != NULL)
	{
		if(pTimer == &Timer[ID])
			break;

		pTimer_Last = pTimer;
		pTimer = pTimer->Next;
	}

	if(pTimer == NULL)
		return 1;
	
	if(pTimer == hTimer)//如果删除的是头结点
	{
		ET0 = 0;//不能进入定时中断
		hTimer = pTimer->Next;
		ET0 = 1;
	}
	else
	{
	 	ET0 = 0;//不能进入定时中断
		pTimer_Last->Next = pTimer->Next;
		ET0 = 1;
	}

	pTimer->Flag = 0x00;//置定时器未使用标志

	return 0;

}

//定时器启动
void Timer_Start(uint8_t ID)
{
	Timer[ID].Flag |= 0x40;	
}

//定时器重新启动，先复位定时器时间到标志，再启动定时器
void Timer_Restart(uint8_t ID)
{	
	Timer[ID].Flag &= ~0x80;
	Timer[ID].Flag |= 0x40;	
}

//定时器停止
void Timer_Stop(uint8_t ID)
{
	Timer[ID].Flag &= ~0x40;	
}

//定时器复位
void Timer_Reset(uint8_t ID)
{
	Timer[ID].Count = 0;	
}

//定时器重新设置时间
void Timer_ResetTime(uint8_t ID, uint16_t Time)
{
	Timer[ID].Time = Time;	
}

//得到定时器定时时间
uint16_t Timer_GetTime(uint8_t ID)
{
	return Timer[ID].Time;	
}

//得到定时器计数时间
uint16_t Timer_GetCount(uint8_t ID)
{
	return Timer[ID].Count;	
}

//检查是否定时时间到
uint8_t Timer_IsTimeOut(uint8_t ID)
{
	return (Timer[ID].Flag & 0x80);
}

//复位定时时间到标志
void Timer_ResetTimeOutFlag(uint8_t ID)
{
	Timer[ID].Flag &= ~0x80;
}

#if 0
//----------------------------------------------------------------
void Delay1us()		//@24.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 3;
	while (--i);
}

void Delay10us()		//@24.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 57;
	while (--i);
}

void Delay100us()		//@24.000MHz
{
	unsigned char i, j;

	i = 3;
	j = 82;
	do
	{
		while (--j);
	} while (--i);
}

void Delay1ms()		//@24.000MHz
{
	unsigned char i, j;

	i = 24;
	j = 85;
	do
	{
		while (--j);
	} while (--i);
}

void Delay100ms()		//@24.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 10;
	j = 31;
	k = 147;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

#endif
