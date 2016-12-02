/*

*/

#ifndef __PCA_H__
#define __PCA_H__

#include "common.h"
#include "dcmotor.h"

#define CCP_S0 0x10                 //P_SW1.4
#define CCP_S1 0x20                 //P_SW1.5

#define STEPMOTOR_ANGLEPERSTEP  1.8  //������������  
#define STEPMOTOR_FREDIV  128  //���������Ƶϵ��
#define STEPMOTOR_PULSEPERROUND  ((360/STEPMOTOR_ANGLEPERSTEP)*STEPMOTOR_FREDIV)  //�������ÿת��������

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
	SPEED_NONE,	//��
	SPEED_ACC,  //����
	SPEED_DEC,	//����
}SpeedStatus_TypeDef;

typedef struct
{
	uint8_t origin;
	uint8_t curPos;  //��ǰλ��
	uint8_t desPos;  //Ŀ��λ��
	uint8_t cur;

	SpeedStatus_TypeDef speedStatus;
	uint16_t maxSpeed; //����ٶ�
	uint16_t minSpeed; //��С�ٶ�
	uint16_t curSpeed; //��ǰ�ٶȣ����ڼӼ���
	uint16_t desSpeed; //Ŀ���ٶ�
	uint8_t stepMotorControl;  //�������ʹ�ܿ���
	pDriveStepMotorFun_TypeDef pDriveStepMotor;
	pStepMotorEnableFun_TypeDef pStepMotorEnable;  //�������ʹ�ܺ���
}StepMotor_TypeDef;

extern StepMotor_TypeDef *pStepMotor;

void PCA_Init(void);
void driveStepMotor(void);
void stepMotorEnable(Status sta);

#endif