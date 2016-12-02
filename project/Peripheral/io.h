/*

*/

#ifndef __IO_H__
#define __IO_H__

#include "common.h"

sbit LED1 = P4^0;
sbit LED2 = P5^5;

sbit DRV_BEEP = P4^3;
sbit DRV_DELAY = P4^2;

void IO_Init(void);
void beepAlarm(unsigned char mode);

#endif