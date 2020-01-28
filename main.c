/*
 * main.c
 *
 *  Created on: 28 dic. 2019
 *      Author: josefe
 */

#include <avr/io.h>
#include <util/delay.h>
#include "UART2.h"
#include "timer0_tick.h"
#include "ePowerSwitchController.h"
#include "emergencyButton.h"
uint8_t dipSwitchesMask;

void initDipSwitches();
uint8_t readDipSwitches();
void configureChannel1();
void configureChannel2();
void configureChannel3();
void configureChannel4();


int main()
{
	initDipSwitches();
	timer0TicktimerInit(T0_PRESCALER_64, MS_1_TIMER_COUNT);
	UART2_init();
	dipSwitchesMask = readDipSwitches();
	initePowerSwitch();
	emergencyButtonBegin(&mushroom, &PINC, &PORTC, &DDRC, 5);
	configureChannel1();
	configureChannel2();
	configureChannel3();
	configureChannel4();
	while(1)
	{
		processAllChannels();
		_delay_ms(100);
	}
}

void initDipSwitches()
{
	DDRC &= ~(_BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3) | _BV(PC4));
	// Pullup enabled
	PORTC |= (_BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3) | _BV(PC4));
	MCUCR &= ~_BV(PUD); // Global enable
}

uint8_t readDipSwitches()
{
	uint8_t rawReading = PINC & (_BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3) | _BV(PC4));
	return (~rawReading & (_BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3) | _BV(PC4)));
}


void configureChannel1()
{
	channels[0].stripChannel = 0;
	channels[0].longPressShutdownMode = 0;
	channels[0].isLinked = 0;
	buttonBegin(&channels[0].physicalButton, &PIND, &PORTD, &DDRD, 2);
	ledBegin(&(channels[0].led), &PORTB, &DDRB, 5, &PORTD, &DDRD, 6);
	if (dipSwitchesMask & _BV(PC0))
	{
		channels[0].longPressShutdownMode = 0xFF;
	}
}

void configureChannel2()
{
	channels[1].stripChannel = 1;
	channels[1].longPressShutdownMode = 0;
	channels[1].isLinked = 0;
	buttonBegin(&channels[1].physicalButton, &PIND, &PORTD, &DDRD, 3);
	ledBegin(&channels[1].led, &PORTB, &DDRB, 4, &PORTD, &DDRD, 7);
	if (dipSwitchesMask & _BV(PC1))
	{
		channels[1].longPressShutdownMode = 0xFF;
	}
}

void configureChannel3()
{
	channels[2].stripChannel = 3;
	channels[2].longPressShutdownMode = 0;
	channels[2].isLinked = 0;
	buttonBegin(&channels[2].physicalButton, &PIND, &PORTD, &DDRD, 4);//d4
	ledBegin(&channels[2].led, &PORTB, &DDRB, 3, &PORTB, &DDRB, 0);//b3 b0
	if (dipSwitchesMask & _BV(PC2))
	{
		channels[2].longPressShutdownMode = 0xFF;
	}
}

void configureChannel4()
{
	channels[3].stripChannel = 2;
	channels[3].longPressShutdownMode = 0;
	channels[3].isLinked = 0;
	buttonBegin(&channels[3].physicalButton, &PIND, &PORTD, &DDRD, 5); //d5
	ledBegin(&channels[3].led, &PORTB, &DDRB, 2, &PORTB, &DDRB, 1); //b2 b1
	if (dipSwitchesMask & _BV(PC3))
	{
		channels[3].longPressShutdownMode = 0xFF;
	}
	if (dipSwitchesMask & _BV(PC4))
	{
		channels[3].isLinked = 0xFF;
		channels[3].linkedChannel = &channels[2];
	}
}
