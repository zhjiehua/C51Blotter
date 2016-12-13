#ifndef __UART_H__
#define __UART_H__

#include "common.h"

#define BAUD 38400          //UART baudrate

/*Define UART parity mode*/
#define NONE_PARITY     0   //None parity
#define ODD_PARITY      1   //Odd parity
#define EVEN_PARITY     2   //Even parity
#define MARK_PARITY     3   //Mark parity
#define SPACE_PARITY    4   //Space parity

#define PARITYBIT NONE_PARITY   //Testing even parity

void Uart_Init(void);
void Uart_SendData(uint8_t dat);
void Uart_SendString(uint8_t *s);

void Uart2_SendData(uint8_t dat);
void Uart2_SendString(char *s);

void uart_printf(char *fmt,...);
void uart_printf_none(char *fmt,...);
#endif