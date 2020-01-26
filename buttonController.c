/*
 * buttonController.c
 *
 *  Created on: 28 dic. 2019
 *      Author: josefe
 */


#include "buttonController.h"
#include <avr/cpufunc.h>

void buttonBegin(ButtonData* button, volatile uint8_t* pin, volatile uint8_t* port, volatile uint8_t* ddr, uint8_t bit)
{
	button -> pinReg = pin;
	button -> portReg = port;
	button -> ddrReg = ddr;
	button -> pinBit = bit;
	button -> pressedTimeout_ms = DEFAULT_PRESSED_TIMEOUT_MS;
	button -> debounceTimeout_ms = DEFAULT_DEBOUNCE_TIMEOUT_MS;
	*(button -> ddrReg) &= ~_BV(button -> pinBit); // Input
	*(button -> portReg) |= _BV(button -> pinBit); // Pullup
	MCUCR &= ~_BV(PUD); // Global enable
	_NOP();
	if (((*(button -> pinReg)) & _BV(button -> pinBit)) ^ _BV(button -> pinBit))
	{
		button -> state = RELEASE_REQUIRED;
	}
	else
	{
		button -> state = IDLE;
	}
	button -> releaseRequiredFlag = 0;

}

void processButton(ButtonData* button)
{
	// Inverted logic
	uint8_t buttonStatus = ((*(button -> pinReg)) & _BV(button -> pinBit)) ^ _BV(button -> pinBit);
	switch(button -> state)
	{
		case IDLE:
			if (buttonStatus && !(button -> releaseRequiredFlag))
			{
				timer0UpdateTimer(&(button -> pressedTimer), TIMER_SET, button -> pressedTimeout_ms);
				button -> state = PRESSED;
			}
			else
			{
				button -> releaseRequiredFlag = 0;
				button -> state = IDLE;
			}
			break;
		case PRESSED:
			if (!buttonStatus)
			{
				timer0UpdateTimer(&(button -> pressedTimer), TIMER_RESET, 0xFFFF);
				timer0UpdateTimer(&(button -> debounceTimer), TIMER_SET, button -> debounceTimeout_ms);
				button -> releaseRequiredFlag = 0;
				button -> state = DEBOUNCE;
				break;
			}
			if (button -> releaseRequiredFlag)
			{
				timer0UpdateTimer(&(button -> pressedTimer), TIMER_RESET, 0xFFFF);
				button -> releaseRequiredFlag = 0;
				button -> state = RELEASE_REQUIRED;
				break;
			}
			if (timer0UpdateTimer(&(button -> pressedTimer), TIMER_CHECK_MATCH,  0))
			{
				timer0UpdateTimer(&(button -> pressedTimer), TIMER_RESET, 0xFFFF);
				button -> state = PRESS_VALID;
				break;
			}
			if (buttonStatus)
			{
				button -> state = PRESSED;
				break;
			}
			break;
		case PRESS_VALID:
			if (!buttonStatus)
			{
				timer0UpdateTimer(&(button -> debounceTimer), TIMER_SET, button -> debounceTimeout_ms);
				button -> releaseRequiredFlag = 0;
				button -> state = DEBOUNCE;
			}
			else
			{
				if (button -> releaseRequiredFlag)
				{
					button -> releaseRequiredFlag = 0;
					button -> state = RELEASE_REQUIRED;
					break;
				}
				else
				{
					button -> state = PRESS_VALID;
					break;
				}
			}
			break;
		case RELEASE_REQUIRED:
			if (!buttonStatus)
			{
				timer0UpdateTimer(&(button -> debounceTimer), TIMER_SET, button -> debounceTimeout_ms);
				button -> releaseRequiredFlag = 0;
				button -> state = DEBOUNCE;
				break;
			}
			else
			{
				button -> releaseRequiredFlag = 0;
				button -> state = RELEASE_REQUIRED;
				break;
			}
		case DEBOUNCE:
			if (timer0UpdateTimer(&(button -> debounceTimer), TIMER_CHECK_MATCH,  0)) // TODO check timeout
			{
				timer0UpdateTimer(&(button -> debounceTimer), TIMER_RESET, 0xFFFF);
				button -> state = IDLE;
				break;
			}
			else
			{
				button -> state = DEBOUNCE;
				break;
			}
	default:
		break;

	}
}

//Call this function after the button update function to perform a release check before
uint8_t checkButton(ButtonData* button)
{
	if (button -> state == PRESS_VALID && !(button -> releaseRequiredFlag))
	{
		button -> state = RELEASE_REQUIRED;
		return 1;
	}
	return 0;
}

void setButtonRequiredPressedTime(ButtonData* button, uint16_t time)
{
	button -> pressedTimeout_ms = time;
}

void forceRelease(ButtonData* button)
{
	button -> releaseRequiredFlag = 1;
}
