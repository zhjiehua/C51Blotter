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

/*步进电机参数*/
#define STEPMOTOR_ANGLEPERSTEP  1.8  //步进电机步距角  
#define STEPMOTOR_FREDIV  128  //步进电机分频系数
#define STEPMOTOR_PULSEPERROUND  ((360/STEPMOTOR_ANGLEPERSTEP)*STEPMOTOR_FREDIV)  //步进电机每转需要多少个脉冲

#define SPEED_CONST  600 //用于加减速的步进时间常数

/*单片机定时器1工作于12T模式时*/
//#define F0R1 (65536-FOSC/2/12/(STEPMOTOR_PULSEPERROUND*0.1))
//#define F0R2 (65536-FOSC/2/12/(STEPMOTOR_PULSEPERROUND*0.2)) //正常工作速度
//#define F0R3 (65536-FOSC/2/12/(STEPMOTOR_PULSEPERROUND*0.3))
//#define F0R5 (65536-FOSC/2/12/(STEPMOTOR_PULSEPERROUND*0.5))
//#define F0R7 (65536-FOSC/2/12/(STEPMOTOR_PULSEPERROUND*0.7))
//#define F1R  (65536-FOSC/2/12/(STEPMOTOR_PULSEPERROUND*1))
//#define F1R3 (65536-FOSC/2/12/(STEPMOTOR_PULSEPERROUND*1.3))
//#define F1R6 (65536-FOSC/2/12/(STEPMOTOR_PULSEPERROUND*1.6))

/*单片机定时器1工作于1T模式时*/
#define F0R1 (65536-FOSC/2/(STEPMOTOR_PULSEPERROUND*0.1))
#define F0R2 (65536-FOSC/2/(STEPMOTOR_PULSEPERROUND*0.2)) //正常工作速度
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

//typedef enum
//{
//	POS_PUMP1 = 1,
//	POS_PUMP2 = 50,
//	POS_PUMP3 = 49,
//	POS_PUMP4 = 48,
//	POS_PUMP5 = 47,
//	POS_PUMP6 = 46,
//	POS_PUMP7 = 45,
//	POS_PUMP8 = 44,
//
//	POS_WASTE = 23,  //废液口
//	POS_HANDLE = 28, //手动点
//}AbsPos_TypeDef;

typedef struct
{
	uint16_t speed;
	uint16_t speedConst;
}SpeedLevel_TypeDef;

typedef enum
{
	SPEED_NONE,	//静止或者匀速中
	SPEED_ACC,  //加速
	SPEED_DEC,	//减速
	SPEED_STOP, //用于立即停止步进电机
}SpeedStatus_TypeDef;

typedef struct
{
	uint8_t curCount;    //当前第几个信号
	uint8_t desCount; 	 //计算第几个信号后停止电机，0表示不停	

	int8_t curPos;  //当前位置，如果是顺时针旋转就++，否则--
	//int8_t desPos;  //目标位置

	Direction_TypeDef direction;  //记录方向

	SpeedStatus_TypeDef speedStatus;
	SpeedLevel_TypeDef *pSpeedLevel;  //速度级别数组
	uint16_t curSpeedIndex; //当前速度，用于加减速
	uint16_t desSpeedIndex; //目标速度

	uint8_t control;  //步进电机使能控制

	void (*SetSpeed)(uint8_t speedIndex);  //设置步进电机速度
	void (*SetDir)(Direction_TypeDef dir);  //设置步进电机方向
	void (*SetCMD)(Status sta);  //设置步进电机加速/减速
	void (*Stop)(void); //用于立即停止步进电机

	void (*SetPos)(uint8_t pos);
	void (*UpdatePos)();
	uint8_t (*IsOnPos)(void);

	void (*Home)(void); //回原点
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