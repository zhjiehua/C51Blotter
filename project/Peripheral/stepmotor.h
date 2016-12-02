/*

*/

#ifndef __STEPMOTOR_H__
#define __STEPMOTOR_H__
#include "common.h"

sbit S_ENABLE1 = P5^1;
sbit S_DIR1 = P3^6;
sbit S_STEP1 = P3^5;
sbit S_SLEEP1 = P3^7;

sbit S_ENABLE2 = P5^0;
sbit S_DIR2 = P4^1;
sbit S_STEP2 = P3^4;

#define CCP_S0 0x10                 //P_SW1.4
#define CCP_S1 0x20                 //P_SW1.5

/*�����������*/
#define STEPMOTOR_ANGLEPERSTEP  1.8  //������������  
#define STEPMOTOR_FREDIV  128  //���������Ƶϵ��
#define STEPMOTOR_PULSEPERROUND  ((360/STEPMOTOR_ANGLEPERSTEP)*STEPMOTOR_FREDIV)  //�������ÿת��Ҫ���ٸ�����

#define SPEED_CONST  600 //���ڼӼ��ٵĲ���ʱ�䳣��

/*��Ƭ����ʱ��1������12Tģʽʱ*/
//#define F0R1 (65536-FOSC/2/12/(STEPMOTOR_PULSEPERROUND*0.1))
//#define F0R2 (65536-FOSC/2/12/(STEPMOTOR_PULSEPERROUND*0.2)) //���������ٶ�
//#define F0R3 (65536-FOSC/2/12/(STEPMOTOR_PULSEPERROUND*0.3))
//#define F0R5 (65536-FOSC/2/12/(STEPMOTOR_PULSEPERROUND*0.5))
//#define F0R7 (65536-FOSC/2/12/(STEPMOTOR_PULSEPERROUND*0.7))
//#define F1R  (65536-FOSC/2/12/(STEPMOTOR_PULSEPERROUND*1))
//#define F1R3 (65536-FOSC/2/12/(STEPMOTOR_PULSEPERROUND*1.3))
//#define F1R6 (65536-FOSC/2/12/(STEPMOTOR_PULSEPERROUND*1.6))

/*��Ƭ����ʱ��1������1Tģʽʱ*/
#define F0R1 (65536-FOSC/2/(STEPMOTOR_PULSEPERROUND*0.1))
#define F0R2 (65536-FOSC/2/(STEPMOTOR_PULSEPERROUND*0.2)) //���������ٶ�
#define F0R3 (65536-FOSC/2/(STEPMOTOR_PULSEPERROUND*0.3))
#define F0R5 (65536-FOSC/2/(STEPMOTOR_PULSEPERROUND*0.5))
#define F0R7 (65536-FOSC/2/(STEPMOTOR_PULSEPERROUND*0.7))
#define F1R  (65536-FOSC/2/(STEPMOTOR_PULSEPERROUND*1))
#define F1R3 (65536-FOSC/2/(STEPMOTOR_PULSEPERROUND*1.3))
#define F1R6 (65536-FOSC/2/(STEPMOTOR_PULSEPERROUND*1.6))
#define F2R  (65536-FOSC/2/(STEPMOTOR_PULSEPERROUND*2))
#define F2R5 (65536-FOSC/2/(STEPMOTOR_PULSEPERROUND*2.5))
#define F3R  (65536-FOSC/2/(STEPMOTOR_PULSEPERROUND*3))
#define F3R5 (65536-FOSC/2/(STEPMOTOR_PULSEPERROUND*3.5))
#define F4R  (65536-FOSC/2/(STEPMOTOR_PULSEPERROUND*4))

#define STEPMOTOR_FREQ(r) (65536-FOSC/2/(STEPMOTOR_PULSEPERROUND*(r)))

#define SIZEOF(x) (sizeof(x)/sizeof((x)[0]))

typedef struct
{
	uint16_t speed;
	uint16_t speedConst;
}SpeedLevel_TypeDef;

typedef enum
{
	SPEED_NONE,	//��
	SPEED_ACC,  //����
	SPEED_DEC,	//����
	SPEED_STOP, //��������ֹͣ�������
}SpeedStatus_TypeDef;

typedef struct
{
	uint8_t origin;
	uint8_t curPos;  //��ǰλ��
	uint8_t desPos;  //Ŀ��λ��
	uint8_t cur;

	SpeedStatus_TypeDef speedStatus;
	SpeedLevel_TypeDef *pSpeedLevel;  //�ٶȼ�������
	uint16_t curSpeedIndex; //��ǰ�ٶȣ����ڼӼ���
	uint16_t desSpeedIndex; //Ŀ���ٶ�

	uint8_t control;  //�������ʹ�ܿ���

	void (*SetSpeed)(uint8_t speedIndex);  //���ò�������ٶ�
	void (*SetDir)(Direction_TypeDef dir);  //���ò����������
	void (*SetCMD)(Status sta);  //���ò����������/ֹͣ
	void (*Stop)(void); //��������ֹͣ�������
	void (*SetTimer)(Status sta); //����ֱ��������ֹͣ��ʱ����һ������������
}StepMotor_TypeDef;

extern StepMotor_TypeDef *pStepMotor;
extern const uint8_t speedLevelSize;

void StepMotor_Init(void);
void StepMotor_Test(void);
#endif