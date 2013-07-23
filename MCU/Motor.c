#include <avr/io.h>
#include "Motor.h"

unsigned char Temp;

inline void InitializeMotors()
{
	Temp = DDRD;
	DDRD = Temp | (0b00011110);
}

inline void DeInitializeMotors()
{
	Temp = PORTD;
	Temp = Temp & (~(1<<PD1)) & (~(1<<PD2)) & (~(1<<PD3)) & (~(1<<PD4));
	PORTD = Temp;
}

inline void Forward()
{
	//PortD = 0bxxx0101x;
	Temp = PORTD;
	Temp = Temp | (1<<PD1) | (1 <<PD3);
	Temp = Temp & (~(1<<PD2)) & (~(1<<PD4));
	PORTD = Temp;
}
inline void Backward()
{
	//PortD = 0bxxx1010x;
	Temp = PORTD;
	Temp = Temp | (1<<PD2) | (1 <<PD4);
	Temp = Temp & (~(1<<PD1)) & (~(1<<PD3));
	PORTD = Temp;
}
inline void TurnLeft()
{
	//PortD = 0bxxx1101x;
	Temp = PORTD;
	Temp = Temp | (1<<PD1) | (1 <<PD3) | (1 <<PD4);
	Temp = Temp & (~(1<<PD2));
	PORTD = Temp;
}
inline void TurnRight()
{
	//PortD = 0bxxx0111x;
	Temp = PORTD;
	Temp = Temp | (1<<PD1) | (1 <<PD2) | (1 <<PD3);
	Temp = Temp & (~(1<<PD4));
	PORTD = Temp;
}

inline void Stop()
{
	//PortD = 0bxxx1111x;
	Temp = PORTD;
	Temp = Temp | (1<<PD1) | (1 <<PD2) | (1 <<PD3) | (1 <<PD4);
	PORTD = Temp;
}
