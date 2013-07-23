#ifndef F_CPU
	#define F_CPU 4915200UL
#endif

#include "UART.h"
#include <avr/io.h>

#define UART_UBRR ((F_CPU/(UART_BAUD<<4))-1) 

void UARTInit()
{
	//Setting Baud Rate 
	UBRRL = (unsigned char) UART_UBRR; 
	UBRRH = (unsigned char) (UART_UBRR>>8); 


	//Enable Receiver 
	UCSRB = (1<<RXEN)|(1<<RXCIE); 

	//Set Frame Format: 8 data, 1 stop 
	UCSRC=(1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1); //asynchronous, no parity, 1 stop bit, 8 bit size 
}

void UART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) );
	/* Put data into buffer, sends the data */
	UDR = data;
}

void UARTWrite(char str[])
{
	unsigned char i = 0;
	while(str[i])
	{
		UART_Transmit(str[i]);
		i++;
	}
}

