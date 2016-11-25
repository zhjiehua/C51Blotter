/*
	该定时器资源共有TIMER_NUM个，每个都是1ms（看定时器中断时间）为最小单位。

	定时器的ID从0~TIMER_NUM-1，注意ID不要超过这个范围。

	使用定时器时首先要创建定时器，调用Timer_Create(uint8_t ID, uint16_t Time, uint8_t Mode)，Mode模式设置未实现。
	创建成功则返回0；其他值表示不成功。创建完定时器要调用Timer_Start(uint8_t ID)启动定时器。	
	
	注意慎用Timer_Delete(uint8_t ID)，因为删除定时器的时候会屏蔽定时器中断片刻时间，可能会导致遗失一次中断。
	删除定时器的功能主要是从定时器链表中移除，这样可以减少定时器链表的扫描时间。
	如果该定时器可以作它用，最好不要使用删除函数，你可以用Timer_ResetTime(uint8_t ID, uint16_t Time)修改定时时间作它用。

	使用Timer_IsTimeOut(uint8_t ID)可以检查定时时间是否到达，如果是一次性的定时，需要手动清除时间溢出标志位，
	即调用Timer_ResetTimeOutFlag(uint8_t ID)，当然使用Timer_Restart(uint8_t ID)也可以，但是这个函数会重新启动定时器，
	这个函数适合周期性定时的情况。

*/

#ifndef __TIMER_H__
#define __TIMER_H__

#include "common.h"

#define TIMER_NUM 15	//定时器的数量

#define T1MS (65536-FOSC/12/1000)   //1ms timer calculation method in 12T mode
#define T5MS (65536-5*FOSC/12/1000) //5ms

#define TIMEOUT T1MS  //定时器重装载值

//定时器链表结构体
typedef struct _Timer_TypeDef
{
	uint8_t Flag;//标志位
	/*
	bit7	定时器时间到	
	bit6	定时器启动/停止

	bit1	定时器已使用，1：已使用	0：未使用
	bit0	定时器运行模式，1：自动重装载	0：单次运行（未实现）
	*/
	uint16_t Count;//定时器计数器
	uint16_t Time;//定时时间（1ms单位）

	struct _Timer_TypeDef *Next;//指向下一个定时器
}Timer_TypeDef;

void Timer0_Init(void);
uint8_t Timer_Create(uint8_t ID, uint16_t Time, uint8_t Mode);
uint8_t Timer_Delete(uint8_t ID);
void Timer_Start(uint8_t ID);
void Timer_Restart(uint8_t ID);
void Timer_Stop(uint8_t ID);
void Timer_Reset(uint8_t ID);
void Timer_ResetTime(uint8_t ID, uint16_t Time);
uint16_t Timer_GetTime(uint8_t ID);
uint16_t Timer_GetCount(uint8_t ID);
uint8_t Timer_IsTimeOut(uint8_t ID);
void Timer_ResetTimeOutFlag(uint8_t ID);


void Delay1us();
void Delay10us();
void Delay100us();
void Delay1ms();
void Delay100ms();

#endif