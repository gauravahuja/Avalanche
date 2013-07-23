#define F_CPU 4915200UL
			  
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "Motor.h"
//#include "lcd.h"
#include "Messages.h"
#include "UART.h"
#include "Timer.h"


void Delay1s()
{
	_delay_ms(200);
	_delay_ms(200);
	_delay_ms(200);
	_delay_ms(200);
	_delay_ms(200);
}

int Initialize()
{
	Delay1s();
	//InitLCD(LS_ULINE);
	//LCDClear();
	InitializeMotors();
	UARTInit();
	SetTimer1();
	DDRB |= (1<<PB1);
	StopTimer1();
	OCR1A = 701;
	StartTimer1();
	return 1;
}

int main()
{
	Initialize();
	//LCDWriteStringXY(0, 0, "Avalanche...");
	//LCDWriteStringXY(0, 1, "Techfest2k11");
	sei();
	
	while(1);
	
	return 0;
}

char Data = 0;
char CurrentMessage = 0;
char PrevMessage = 0;
char PushData = 0;
char DataByte = 0;
unsigned int Time = 0;

void InterpretMessage()
{
	if(PrevMessage == SetTime)
	{
		DataByte++;
		if(DataByte == 1)
		{
			Time = CurrentMessage;
		}
		if(DataByte == 2)
		{
			
			Time = Time*100;
			Time += CurrentMessage;
			Time *= 10;			
			
			PrevMessage = 0;
			DataByte = 0;
		}				
	}
	else
	{
		switch(CurrentMessage)
		{
		case TiltLeft:
			PrevMessage = TiltLeft;
			StopTimer1();
			OCR1A = 549;
			StartTimer1();
			break;

		case TiltRight:
			PrevMessage = TiltRight;
			StopTimer1();
			OCR1A = 854;
			StartTimer1();
			break;

		case Neutral:
			PrevMessage = Neutral;
			StopTimer1();
			OCR1A = 701;
			StartTimer1();
			break;
		
		case StartTimer:

			PrevMessage = CurrentMessage;
			
			//LCDClear();
			//LCDWriteStringXY(0,0, "Time Set to (us):");
			//LCDWriteIntXY(0, 1, Time, -1);
			
			Time = Time/10;
			Time = Time*61;
			Time = Time/10;
			OCR1A = Time;

			//LCDWriteIntXY(5, 1, Time, -1);

			StartTimer1();

			break;

		case StopTimer:

			PrevMessage = CurrentMessage;
			StopTimer1();

			break;

		case SetTime:

			PrevMessage = CurrentMessage;
			break;

		case MoveForward:
			
			PrevMessage = CurrentMessage;

			//LCDClear();
			//LCDWriteStringXY(0,0, "Forward");
			Forward();
			break;
		
		case MoveBackward:
			
			PrevMessage = CurrentMessage;

			//LCDClear();
			//LCDWriteStringXY(0,0, "Back");
			
			Backward();
			break;
		
		case MoveLeft:
			
			PrevMessage = CurrentMessage;
			
			//LCDClear();
			//LCDWriteStringXY(0,0, "Left");
			
			TurnLeft();
			break;
		
		case MoveRight:
		
			PrevMessage = CurrentMessage;
			
			//LCDClear();
			//LCDWriteStringXY(0,0, "Right");
			
			TurnRight();
			break;
		
		case StopMoving:
			
			PrevMessage = CurrentMessage;

			//LCDClear();
			//LCDWriteStringXY(0,0, "Stop");
			
			Stop();
			break;
		
		default:
			
			PrevMessage = 0;
			break;		
		}
	}
	CurrentMessage = 0;
}	


ISR(USART_RXC_vect)
{	
	Data = UDR;
	if(Data == EndByte)
	{
		PushData = 0;
		InterpretMessage();
	}
	if(PushData)
	{
		CurrentMessage = Data;
	}
	if(Data == StartByte)
	{
		PushData = 1;
	}
}

