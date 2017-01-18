/*

*/
#ifndef __DCMOTOR_H__
#define __DCMOTOR_H__

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

sbit MOT10_DIR = P3^6;

#define CYCLE   0x1000L*5     //定义PWM周期(最大值为32767)
#define DUTY    50L         //定义占空比为10%

#define DCMOTOR_COUNT 12   //直流电机数量

#define PUMP_VACUUM DCMOTOR9   //真空泵
#define PUMP_WASTE  DCMOTOR10  //废液泵
#define PUMP_PINCH  DCMOTOR12  //夹管阀

typedef enum
{
	UP = 0,
	DOWN,
}Position_TypeDef;

typedef enum
{
	DCMOTOR1 = 0,
	DCMOTOR2,
	DCMOTOR3,
	DCMOTOR4,
	DCMOTOR5,
	DCMOTOR6,
	DCMOTOR7,
	DCMOTOR8,
	DCMOTOR9,
	DCMOTOR10,  //废液泵
	DCMOTOR11,
	DCMOTOR12,
}DCMotorEnum_TypeDef;

typedef struct
{
	uint8_t control;

	uint8_t curCount;    //当前第几个信号
	uint8_t desCount; 	 //计算第几个信号后停止电机，0表示不停

	void (*SetCMD)(DCMotorEnum_TypeDef num, Status sta);
	void (*SetDir)(DCMotorEnum_TypeDef num, Direction_TypeDef dir);
	void (*SetSpeed)(DCMotorEnum_TypeDef num, uint16_t speed);

	void (*SetPos)(uint8_t pos);
	void (*UpdatePos)();
	uint8_t (*IsOnPos)(void);	

	void (*WastePump_SetPos)(Position_TypeDef pos);	
}DCMotor_TypeDef;

extern DCMotor_TypeDef *pDCMotor;

void DCMotor_Init(void);
void DCMotor_Test(void);

#endif