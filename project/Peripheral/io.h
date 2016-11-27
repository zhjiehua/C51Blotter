/*

*/

#ifndef __IO_H__
#define __IO_H__

#include "common.h"
#include "pwm.h"

sbit LED1 = P4^0;
sbit LED2 = P5^5;

sbit DRV_BEEP = P4^3;
sbit DRV_DELAY = P4^2;

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

typedef struct
{
	uint8_t curStatus;
	uint8_t preStatus;
	uint8_t fallingEdge;
}SensorStatus_TypeDef;

extern SensorStatus_TypeDef *pSensorStatus;

void IO_Init(void);
void beepAlarm(unsigned char mode);

void sensorCheck(void);


#endif