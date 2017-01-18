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

#define CYCLE   0x1000L*5     //����PWM����(���ֵΪ32767)
#define DUTY    50L         //����ռ�ձ�Ϊ10%

#define DCMOTOR_COUNT 12   //ֱ���������

#define PUMP_VACUUM DCMOTOR9   //��ձ�
#define PUMP_WASTE  DCMOTOR10  //��Һ��
#define PUMP_PINCH  DCMOTOR12  //�йܷ�

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
	DCMOTOR10,  //��Һ��
	DCMOTOR11,
	DCMOTOR12,
}DCMotorEnum_TypeDef;

typedef struct
{
	uint8_t control;

	uint8_t curCount;    //��ǰ�ڼ����ź�
	uint8_t desCount; 	 //����ڼ����źź�ֹͣ�����0��ʾ��ͣ

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