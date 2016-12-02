#ifndef __COMMON_H__
#define __COMMON_H__

//#include <REG52.H>                /* special function register declarations   */
//                                  /* for the intended 8051 derivative         */
#include "stc15f2k60s2.h"
#include "intrins.h"
#include "RTX51TNY.H"

//typedef unsigned char BYTE; //sizeof(BYTE) = 1
//typedef unsigned int WORD;  //sizeof(WORD) = 2

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;

//----------------------------------------------- 
/* define constants */
//#define FOSC 11059200L      //System frequency
#define FOSC 20000000L      //System frequency

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

#endif