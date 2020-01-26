/*
 * LEDController.c
 *
 *  Created on: 3 ene. 2020
 *      Author: josefe
 */

#include "LEDController.h"

void ledSwitchLamp(LEDData* led, LEDColor color);

void ledBegin(LEDData* led, volatile uint8_t* redPort, volatile uint8_t* redDdr, uint8_t redBit, volatile uint8_t* greenPort, volatile uint8_t* greenDdr, uint8_t greenBit)
{
	led -> redPortReg = redPort;
	led -> redDdrReg = redDdr;
	led -> redBit = redBit;
	led -> greenPortReg = greenPort;
	led -> greenDdrReg = greenDdr;
	led -> greenBit = greenBit;
	// Leds are off
	*(led -> redDdrReg) |= _BV(led -> redBit);
	*(led -> redPortReg) &= ~_BV(led -> redBit);
	*(led -> greenDdrReg) |= _BV(led -> greenBit);
	*(led -> greenPortReg) &= ~_BV(led -> greenBit);
	led -> color = OFF;
	(led -> blink).status = BLINK_OFF;
	(led -> blink).tOn = 0;
	(led -> blink).tOff = 0;
	timer0UpdateTimer(&(led -> blink).blinkTimer, TIMER_RESET, 0);
}

void ledSetColor(LEDData* led, LEDColor color)
{
	led -> color = color;
	ledSwitchLamp(led, led -> color);
}

void ledSwitchLamp(LEDData* led, LEDColor color)
{
	switch (led -> color)
	{
		case GREEN:
			*(led -> redPortReg) &= ~_BV(led -> redBit);
			*(led -> greenPortReg) |= _BV(led -> greenBit);
			return;
		case RED:
			*(led -> redPortReg) |= _BV(led -> redBit);
			*(led -> greenPortReg) &= ~_BV(led -> greenBit);
			return;
		case YELLOW:
			*(led -> redPortReg) |= _BV(led -> redBit);
			*(led -> greenPortReg) |= _BV(led -> greenBit);
			return;
		case OFF:
		default:
			*(led -> redPortReg) &= ~_BV(led -> redBit);
			*(led -> greenPortReg) &= ~_BV(led -> greenBit);
			//ledBlink(led, BLINK_OFF);
			return;
	}
}

void ledSetBlinkPeriods(LEDData* led, uint8_t tOn_100ms, uint8_t tOff_100ms)
{
	(led -> blink).tOn = tOn_100ms;
	(led -> blink).tOff = tOff_100ms;
}

void ledBlink(LEDData* led, BlinkStatus initialStatus)
{
	(led -> blink).status = initialStatus;
	switch ((led -> blink).status)
	{
		case DUTY_ON:
			ledSwitchLamp(led, led -> color);
			timer0UpdateTimer(&(led -> blink).blinkTimer, TIMER_SET, ((uint16_t) (led -> blink).tOn) * 100);
			return;
		case DUTY_OFF:
			ledSwitchLamp(led, OFF);
			timer0UpdateTimer(&(led -> blink).blinkTimer, TIMER_SET, ((uint16_t) (led -> blink).tOff) * 100);
			return;
		default:
		case BLINK_OFF:
			// LED will be turned off when blink cycle ends
			return;
	}
}

void ledUpdate(LEDData* led)
{
	if (timer0UpdateTimer(&(led -> blink).blinkTimer, TIMER_CHECK_MATCH, 0))
	{
		switch ((led -> blink).status)
		{
			case DUTY_OFF:
				(led -> blink).status = DUTY_ON;
				ledSwitchLamp(led, led -> color);
				timer0UpdateTimer(&(led -> blink).blinkTimer, TIMER_SET, ((uint16_t) (led -> blink).tOn) * 100);
				return;
			case DUTY_ON:
				(led -> blink).status = DUTY_OFF;
				ledSwitchLamp(led, OFF);
				timer0UpdateTimer(&(led -> blink).blinkTimer, TIMER_SET, ((uint16_t) (led -> blink).tOff) * 100);
				return;
			default:
			case BLINK_OFF:
				(led -> blink).status = BLINK_OFF;
				ledSwitchLamp(led, led -> color);
				timer0UpdateTimer(&(led -> blink).blinkTimer, TIMER_RESET, 0);
				return;
		}
	}
}
