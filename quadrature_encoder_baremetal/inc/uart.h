#ifndef UART_H
#define UART_H

#include "main.h"

#define FOSC 72000000
#define BAUD 9600

char USART_ReadChar();

void USART_sendChar(char data);

void USART_sendString(char *data);

void Uart1Init();

#endif