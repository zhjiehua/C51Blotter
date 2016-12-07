#ifndef _PROJECT_H_
#define _PROJECT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "pageCommon.h"

#define NAME_SIZE 11  //�������ƻ�����Ŀ���Ƶ���󳤶�
#define ACTION_COUNT_PER_PROJECT  4  //ÿ����Ŀ�ж��ٸ�����
#define PROJECT_COUNT  2  //��Ŀ����

#define PUMP_COUNT  8  //�䶯������


typedef enum
{
	PUMP1 = 0,
	PUMP2,
	PUMP3,
	PUMP4,
	PUMP5,
	PUMP6,
	PUMP7,
	PUMP8,
}Pump_TypeDef;

typedef enum
{
	NO_TIPS = 0,
	SAMPLE_TIPS,
	MEMBRANE_TIPS,
}Tips_TypeDef;

typedef enum
{
	NO_VOICE = 0,
	SHORT_VOICE,
	MIDDLE_VOICE,
	LONG_VOICE,
}Voice_TypeDef;

typedef enum
{
	SLOW_SPEED = 0,
	MIDDLE_SPEED,
	FAST_SPEED,
}ShakeSpeed_TypeDef;

typedef struct 
{
	uint8 hour;
	uint8 minute;
}ShakeTime_TypeDef;

/************************************************************************/
/* �����ṹ��                                                           */
/************************************************************************/
typedef struct
{
	Pump_TypeDef pump;  //�ñ��
	Tips_TypeDef tips;  //��ʾ
	Voice_TypeDef voice;  //����
	float addAmount;  //��ע��
	uint8 imbiAmount;  //��Һ��
	ShakeSpeed_TypeDef shakeSpeed;  //ҡ���ٶ�
	ShakeTime_TypeDef shakeTime;  //ҡ��ʱ��
	uint8 loopTime;  //ѭ������

	char name[NAME_SIZE];  //������
}Action_TypeDef;

/************************************************************************/
/* ��Ŀ�ṹ��                                                           */
/************************************************************************/
typedef struct
{
	Action_TypeDef action[ACTION_COUNT_PER_PROJECT];

	char name[NAME_SIZE];  //��Ŀ��
}Project_TypeDef;

extern Project_TypeDef project[PROJECT_COUNT];
extern float caliPumpPara[PUMP_COUNT];

void initProjectStruct(Project_TypeDef* pro, char *name);
void initCaliPumpPara(float para);
void wasteFluidAbsorb(void);
void projectProgram(void);
void purgeProgram(void);
void homeProgram(void);
void calibraProgram(void);

#ifdef __cplusplus
}
#endif

#endif
