/*

*/

#ifndef __SENSOR_H__
#define __SENSOR_H__

#include "common.h"

sbit INT0_INPUT = P3^2;

sbit EAIN1 = P2^6;
sbit EAIN2 = P2^7;
sbit EAIN3 = P4^5;
sbit EAIN4 = P4^6;
sbit EAIN5 = P0^0;
sbit EAIN6 = P0^1;
sbit EAIN7 = P0^2;
sbit EAIN8 = P0^3;

#define SENSOR1_MASK 0x01
#define SENSOR2_MASK 0x02
#define SENSOR3_MASK 0x04
#define SENSOR4_MASK 0x08
#define SENSOR5_MASK 0x10
#define SENSOR6_MASK 0x20
#define SENSOR7_MASK 0x40
#define SENSOR8_MASK 0x80
#define SENSORALL_MASK 0xFF

#define SENSOR_POS  SENSOR1
#define SENSOR_HOME SENSOR2
#define SENSOR_UP   SENSOR3
#define SENSOR_DOWN SENSOR4

typedef enum
{
	SENSOR1 = 0,
	SENSOR2,
	SENSOR3,
	SENSOR4,
	SENSOR5,
	SENSOR6,
	SENSOR7,
	SENSOR8,
}SensorEnum_TypeDef;

typedef enum
{
	RASINGEDGE = 0,
	FALLINGEDGE,
}CheckEdge_TypeDef;

typedef struct
{
	uint8_t curStatus;   //当前状态
	uint8_t preStatus;	 //上一状态，在查询方式用
	uint8_t fallingEdge; //上升沿，在查询方式用
	uint8_t rasingEdge; //上升沿，在查询方式用

	uint8_t curCount;    //当前第几个信号
	uint8_t desCount; 	 //计算第几个信号后停止电机，0表示不停
	CheckEdge_TypeDef checkEdge;   //检测边沿类型

	void (*Scan)(void);
	void (*SetCheckEdge)(CheckEdge_TypeDef edge);
	//void (*SetPos)(uint8_t pos);
	//uint8_t (*IsOnPos)(uint8_t sensorNum);
	uint8_t (*GetStatus)(uint8_t sensorNum);
}Sensor_TypeDef;

extern Sensor_TypeDef *pSensor;

void Sensor_Init(void);

#endif