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
#define STEPMOTOR_OFFSET  1400  //��Һ��Ҫ��׼�ۿף���Ҫƫ��

#define STEPMOTOR_PUMP_OFFSET  100 //�䶯�ö�׼

#define SPEED_CONST  600 //���ڼӼ��ٵĲ���ʱ�䳣��

#if 0
	#define SPEED_POSITION1 3  //��λ����Ϊ1���ٶ�
	#define SPEED_POSITION2 9  //��λ����Ϊ2���ٶ�
	#define SPEDD_POSITION  13  //��λ�������3���ٶ�
	#define SPEDD_HOME	    13  //��ԭ����ٶ�
	#define SPEDD_SLOW	    9  //�����ٶ�-��
	#define SPEDD_MIDDLE    14  //�����ٶ�-��
	#define SPEDD_FAST	    17 //�����ٶ�-��
#else
	#define SPEED_POSITION1 2  //��λ����Ϊ1���ٶ�
	#define SPEED_POSITION2 5  //��λ����Ϊ2���ٶ�
	#define SPEDD_POSITION  8  //��λ�������3���ٶ�
	#define SPEDD_HOME	    8  //��ԭ����ٶ�
	#define SPEDD_SLOW	    5  //�����ٶ�-��
	#define SPEDD_MIDDLE    9  //�����ٶ�-��
	#define SPEDD_FAST	    13 //�����ٶ�-��
#endif

#define TANK_COUNT 50
#define TANK_PER_PLATE 5

#define STEPMOTOR_FREQ(r) (65536-FOSC/2/(STEPMOTOR_PULSEPERROUND*(r)))

#define SIZEOF(x) (sizeof(x)/sizeof((x)[0]))

typedef struct
{
	uint16_t speed;
	uint16_t speedConst;
}SpeedLevel_TypeDef;

typedef enum
{
	SPEED_NONE,	//��ֹ����������
	SPEED_ACC,  //����
	SPEED_DEC,	//����
	SPEED_STOP, //��������ֹͣ�����������
	SPEED_POSOFFSET,  //��������ڴ�����λ��ֹͣ������������װ�̻ζ�ʱ�ᵼ�´�������⵽����
}SpeedStatus_TypeDef;

typedef struct
{
	uint8_t curCount;    //��ǰ�ڼ����ź�
	uint8_t desCount; 	 //����ڼ����źź�ֹͣ�����0��ʾ��ͣ	

	int8_t curPos;  //��ǰλ�ã������˳ʱ����ת��++������--
	//int8_t desPos;  //Ŀ��λ��
	uint16_t offset;  //λ��ƫ�ƣ�����У��ת�̣�Ĭ��450

	Direction_TypeDef direction;  //��¼����

	SpeedStatus_TypeDef speedStatus;
	SpeedLevel_TypeDef *pSpeedLevel;  //�ٶȼ�������
	uint16_t curSpeedIndex; //��ǰ�ٶȣ����ڼӼ���
	uint16_t desSpeedIndex; //Ŀ���ٶ�

	uint8_t control;  //�������ʹ�ܿ���

	void (*SetSpeed)(uint8_t speedIndex);  //���ò�������ٶ�
	void (*SetDir)(Direction_TypeDef dir);  //���ò����������
	void (*SetCMD)(Status sta);  //���ò����������/����
	void (*Stop)(void); //��������ֹͣ�������

	void (*SetPos)(uint8_t pos);
	void (*UpdatePos)();
	uint8_t (*IsOnPos)(void);
	uint8_t (*IsStop)(void);
	void (*StopAndAlign)(uint8_t len);

	void (*Home)(void); //��ԭ��
	uint8_t (*Abs2Rel)(uint8_t absCoord);
	void (*Position)(Direction_TypeDef dir, uint8_t dis);
	void (*RelativePosition)(uint8_t desTank, uint8_t srcTank);
}StepMotor_TypeDef;

extern StepMotor_TypeDef *pStepMotor;
extern const uint8_t speedLevelSize;
extern const uint8_t AbsCoordinate[];

void StepMotor_Init(void);
void StepMotor_Test(void);
#endif