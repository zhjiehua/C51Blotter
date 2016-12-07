#ifndef __COMMON_H__
#define __COMMON_H__

//#include <REG52.H>                /* special function register declarations   */
//                                  /* for the intended 8051 derivative         */
#include "stc15f2k60s2.h"
#include "intrins.h"
#include "RTX51TNY.H"

//typedef unsigned char BYTE; //sizeof(BYTE) = 1
//typedef unsigned int WORD;  //sizeof(WORD) = 2

typedef signed char int8_t;
typedef signed int int16_t;
typedef signed long int32_t;

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;

//----------------------------------------------- 
/* define constants */
//#define FOSC 11059200L      //System frequency
#define FOSC 20000000L      //System frequency

#define TASK_STARTUP 0
#define TASK_UI		 1
#define TASK_PROJECT 2
#define TASK_PURGE	 3
#define TASK_HOME	 4
#define TASK_CALIBRA 5
#define TASK_TEST	 10

typedef enum
{
	DISABLE = 0,
	ENABLE = 1,
}Status;

typedef enum
{
	CW = 0,	//正转
	CCW,	//反转
}Direction_TypeDef;

void createTask(uint8_t task);

#endif