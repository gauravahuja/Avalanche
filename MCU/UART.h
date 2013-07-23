#ifndef _UART_H
#define _UART_H

#define UART_BAUD 1200ul 

extern void UARTInit();
extern void UART_Transmit(unsigned char data);
extern void UARTWrite(char Data[]);//Data should be NULL terminated

#endif
