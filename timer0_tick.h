/*
 * timer0_tick.h
 *
 * Created: 01/11/2017 12:03:13
 *  Author: josefe
 */ 


#ifndef TIMER0_TICK_H_
#define TIMER0_TICK_H_

#include <avr/io.h>

#define T0_DISABLED				0			
#define T0_PRESCALER_DISABLED	0b00000001
#define T0_PRESCALER_8			0b00000010
#define T0_PRESCALER_64			0b00000011
#define T0_PRESCALER_256		0b00000100
#define T0_PRESCALER_1024		0b00000101
#define T0_EXTERNAL_FALLING		0b00000110
#define T0_EXTERNAL_RISING		0b00000111

#define MS_1_TIMER_COUNT			249

typedef enum
{
	TIMER_SET,
	TIMER_RESET,
	TIMER_CHECK_MATCH
}TickTimerAction;

typedef struct
{
	uint16_t timerThreshold; // former int32_t
	uint8_t timerFlag;
	uint16_t lastCount;
}TickTimerEntity;

void timer0TicktimerInit(uint8_t prescaler, uint8_t count);
volatile uint16_t timer0GetGlobalInterruptCounter(void);
uint8_t timer0UpdateTimer(TickTimerEntity* entity, const TickTimerAction action, uint16_t value);

#endif /* TIMER0_TICK_H_ */
