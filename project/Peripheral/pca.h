/*

*/

#ifndef __PCA_H__
#define __PCA_H__

#include "common.h"
#include "pwm.h"

#define CCP_S0 0x10                 //P_SW1.4
#define CCP_S1 0x20                 //P_SW1.5

sbit S_ENABLE1 = P5^1;
sbit S_DIR1 = P3^6;
sbit S_STEP1 = P3^5;
sbit S_SLEEP1 = P3^7;

sbit S_ENABLE2 = P5^0;
sbit S_DIR2 = P4^1;
sbit S_STEP2 = P3^4;

typedef void (*pDriveStepMotorFun_TypeDef)(void);
typedef void (*pStepMotorEnableFun_TypeDef)(Status sta); 


typedef struct
{
	uint8_t origin;
	uint8_t curPos;  //当前位置
	uint8_t desPos;  //目标位置
	uint8_t cur;

	uint16_t curSpeed;
	pDriveStepMotorFun_TypeDef pDriveStepMotor;
	pStepMotorEnableFun_TypeDef pStepMotorEnable;
}StepMotor_TypeDef;

extern StepMotor_TypeDef *pStepMotor;

void PCA_Init(void);
void driveStepMotor(void);
void stepMotorEnable(Status sta);

#endif