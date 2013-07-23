#ifndef F_CPU
	#define F_CPU 4915200UL
#endif

#include "Timer.h"
#include <avr/io.h>

inline void SetTimer1()
{
	/**
		Fast PWM
		TOP is IRC1
		Clear OC1A/PD5 on Compare match set OC1A/PD5 at TOP
		Prescaler Not Set
	**/
	TCCR1A = 0x82;
	TCCR1B = 0x18;
	ICR1 = 12288;	//***For 50 hz at CLKio/8, To change
	TCNT1 = 0;
}

inline void StartTimer1()
{
	TCNT1 = 0;
	TCCR1B |= (1<<CS01); //***CLKio/8 To Change
}

inline void StopTimer1()
{
	TCCR1B &= ~(1<<CS01);	//No Prescaler
}
