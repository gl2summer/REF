#ifndef _UART_H
#define _UART_H


#define hd_fifo_size			64



void Uart_0_Isr(void);



void Uart_0_Init(void);



void Uart_Printf(char *fmt,...);

void Uart_0_SendByte(char b);

void Uart_0_SendString(char *s);


char Uart_0_ReceiveByte(void);

int Uart_0_ReceiveString(void *buf, int cnt, int timeout);

#endif  //_UART_H

