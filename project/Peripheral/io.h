/*
	IN_NUM�������������ĸ�����������˳��Ϊ0~IN_NUM��10���ƣ�.
	����PLC�е�X10 X11����ʵ����X8 X9

	OUT_NUM�������������ĸ�����������˳��Ϊ0~OUT_NUM��10���ƣ�.

	IO_Refresh()��������ˢ������IO��״̬��������������������������������Ҫ��while(1)�е���

	���ĺ���_GetX(uint8_t X, uint8_t Num)���ڶ�ȡ����㣬�����˲����ܡ�

	�˲�ʱ����SHAKE_TIME*IO_FRE_DOUBLE����ʱ���ڣ�����SHAKE_TIME��ΧΪ0~31.
	���ڵ��˲�ʱ����SHAKE_TIME*IO_FRE_DOUBLE��1ms
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