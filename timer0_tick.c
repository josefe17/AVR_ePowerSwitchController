/*
 * timer0_tick.c
 *
 * Created: 01/11/2017 12:05:01
 *  Author: josefe
 */ 

#include "timer0_tick.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t globalInterruptCounter;

void timer0TicktimerInit(uint8_t prescaler, uint8_t count)
{
	globalInterruptCounter = 0;				//Clear global timer0 interrupt counter (this varible is incremented forever, overflowing)
	TCCR0A=0b00000010;							//CTC with compare match threshold on OCRA. No HW pin toggling.
	OCR0A=count;								//Set threshold
	TCNT0=0;									//Clear count
	TIFR0|=	0b00000010;							//Clear flag
	TIMSK0|= 0b00000010;						//Interrupt enable	
	TCCR0B=prescaler & 0b00000111;				//Timer on	
}

volatile uint16_t timer0GetGlobalInterruptCounter(void)
{
	volatile uint16_t intBuffer;
	cli();
	intBuffer = globalInterruptCounter;
	sei();
	return intBuffer;
}

ISR (TIMER0_COMPA_vect)
{
	++globalInterruptCounter;					//Increments global counter
}


// This function must be called at least once time between timer counting over the same value (1/2^16-1 ms)
uint8_t timer0UpdateTimer(TickTimerEntity* entity, const TickTimerAction action, uint16_t value)
{
	volatile uint16_t currentCount = timer0GetGlobalInterruptCounter();
	switch (action)
	{
		case TIMER_SET:
			entity -> timerThreshold = value;// + currentCount;
			entity -> lastCount = currentCount;
			entity -> timerFlag = 0;
			return 0;

		case TIMER_RESET:
			entity -> timerThreshold = 0;
			entity -> lastCount = currentCount;
			entity -> timerFlag = 0;
			return 0;

		case TIMER_CHECK_MATCH:
			if (entity -> timerFlag)
			{
				return 1;
			}
			if (currentCount >= entity -> lastCount + entity -> timerThreshold)
			{
				entity -> timerFlag = 1;
				return 1;
			}
			return 0;
	}
	return 0;
}
