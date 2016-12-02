/*

*/

#ifndef __PCA_H__
#define __PCA_H__

#include "common.h"
#include "dcmotor.h"

#define CCP_S0 0x10                 //P_SW1.4
#define CCP_S1 0x20                 //P_SW1.5

#define STEPMOTOR_ANGLEPERSTEP  1.8  //步进电机步距角  
#define STEPMOTOR_FREDIV  128  //步进电机分频系数
#define STEPMOTOR_PULSEPERROUND  ((360/STEPMOTOR_ANGLEPERSTEP)*STEPMOTOR_FREDIV)  //步进电机每转多少脉冲

sbit S_ENABLE1 = P5^1;
sbit S_DIR1 = P3^6;
sbit S_STEP1 = P3^5;
sbit S_SLEEP1 = P3^7;

sbit S_ENABLE2 = P5^0;
sbit S_DIR2 = P4^1;
sbit S_STEP2 = P3^4;

typedef void (*pDriveStepMotorFun_TypeDef)(void);
typedef void (*pStepMotorEnableFun_TypeDef)(Status sta); 

typedef enum
{
	SPEED_NONE,	//无
	SPEED_ACC,  //加速
	SPEED_DEC,	//减速
}SpeedStatus_TypeDef;

typedef struct
{
	uint8_t origin;
	uint8_t curPos;  //当前位置
	uint8_t desPos;  //目标位置
	uint8_t cur;

	SpeedStatus_TypeDef speedStatus;
	uint16_t maxSpeed; //最大速度
	uint16_t minSpeed; //最小速度
	uint16_t curSpeed; //当前速度，用于加减速
	uint16_t desSpeed; //目标速度
	uint8_t stepMotorControl;  //步进电机使能控制
	pDriveStepMotorFun_TypeDef pDriveStepMotor;
	pStepMotorEnableFun_TypeDef pStepMotorEnable;  //步进电机使能函数
}StepMotor_TypeDef;

extern StepMotor_TypeDef *pStepMotor;

void PCA_Init(void);
void driveStepMotor(void);
void stepMotorEnable(Status sta);

#endif