/*

*/
#ifndef __PWM_H__
#define __PWM_H__

#include "common.h"

sbit PWM3 = P2^1;
sbit PWM4 = P2^2;
sbit PWM5 = P2^3;

sbit EN_MOT01 = P0^4;
sbit EN_MOT02 = P5^2;
sbit EN_MOT03 = P5^3;
sbit EN_MOT04 = P0^5;
sbit EN_MOT05 = P0^6;
sbit EN_MOT06 = P0^7;
sbit EN_MOT07 = P4^7;
sbit EN_MOT08 = P1^2;
sbit EN_MOT09 = P1^3;
sbit EN_MOT10 = P1^4;
sbit EN_MOT11 = P1^5;
sbit EN_MOT12 = P5^4;

typedef enum
{
	DISABLE = 0,
	ENABLE = 1,
}Status;

typedef void (*pMotorFun_Typedef)(Status sta);

#define CYCLE   0x1000L     //定义PWM周期(最大值为32767)
#define DUTY    10L         //定义占空比为10%

#define DCMOTOR_COUNT 12

//#define PWM4OUT

extern pMotorFun_Typedef pDCMotorEnable[DCMOTOR_COUNT];

void PWM_Init(void);

#endif