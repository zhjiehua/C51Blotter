/*
	IN_NUM用于配置输入点的个数，输入点的顺序为0~IN_NUM（10进制）.
	对于PLC中的X10 X11等其实就是X8 X9

	OUT_NUM用于配置输出点的个数，输出点的顺序为0~OUT_NUM（10进制）.

	IO_Refresh()函数用于刷新所有IO的状态，包括所有输入输出。所以这个函数需要在while(1)中调用

	核心函数_GetX(uint8_t X, uint8_t Num)用于读取输入点，包括滤波功能。

	滤波时间是SHAKE_TIME*IO_FRE_DOUBLE个定时周期，其中SHAKE_TIME范围为0~31.
	现在的滤波时间是SHAKE_TIME*IO_FRE_DOUBLE个1ms
*/

#ifndef __IO_H__
#define __IO_H__

#include "common.h"

sbit AUX_LED = P4^1;
sbit A_MCU_MOD = P2^5;

//IM481
sbit A_PUL = P3^4;
sbit A_DIR = P5^5;
sbit A_RST = P5^4;

//DDC112
sbit AD_RANGE0 = P3^5;
sbit AD_RANGE1 = P0^2;
sbit AD_RANGE2 = P0^1;

sbit OUT_DXMIT1 = P2^6;
sbit OUT_DXMIT2 = P2^6;
sbit OUT_SCLK1 = P2^7;
sbit OUT_SCLK2 = P2^7;

sbit OUT_CONV1 = P2^1;
sbit OUT_CONV2 = P2^1;

sbit IN_VALID1 = P2^5;
sbit IN_VALID2 = P0^3;
sbit IN_DATA1 = P2^2;
sbit IN_DATA2 = P2^3;

void IO_Init(void);

#endif