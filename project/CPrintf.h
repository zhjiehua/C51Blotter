#ifndef _CPRINTF_H_
#define _CPRINTF_H_

#define _CDebug

#ifdef _CDebug
	#ifdef __cplusplus
		void myCPrintf(const char *fmt, ...);
		#define cDebug(format, ...)     myCPrintf(format, ##__VA_ARGS__)
		//#define cDebug(format, args...)    printf(format, ##args)
	#else
		#include "./Peripheral/uart.h"
		#define cDebug uart_printf
	#endif
#else
	#ifdef __cplusplus
		#define cDebug(format, ...)
	#else
		#include "./Peripheral/uart.h"
		#define cDebug uart_printf_none
	#endif
#endif
#endif
