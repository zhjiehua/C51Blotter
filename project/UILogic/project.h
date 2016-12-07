#ifndef _PROJECT_H_
#define _PROJECT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "pageCommon.h"

#define NAME_SIZE 11  //动作名称或者项目名称的最大长度
#define ACTION_COUNT_PER_PROJECT  4  //每个项目有多少个动作
#define PROJECT_COUNT  2  //项目数量

#define PUMP_COUNT  8  //蠕动泵数量


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
/* 动作结构体                                                           */
/************************************************************************/
typedef struct
{
	Pump_TypeDef pump;  //泵编号
	Tips_TypeDef tips;  //提示
	Voice_TypeDef voice;  //声音
	float addAmount;  //加注量
	uint8 imbiAmount;  //吸液量
	ShakeSpeed_TypeDef shakeSpeed;  //摇动速度
	ShakeTime_TypeDef shakeTime;  //摇动时间
	uint8 loopTime;  //循环次数

	char name[NAME_SIZE];  //动作名
}Action_TypeDef;

/************************************************************************/
/* 项目结构体                                                           */
/************************************************************************/
typedef struct
{
	Action_TypeDef action[ACTION_COUNT_PER_PROJECT];

	char name[NAME_SIZE];  //项目名
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
