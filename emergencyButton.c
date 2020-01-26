/*
 * emergencyButton.c
 *
 *  Created on: 15 ene. 2020
 *      Author: josefe
 */

#include "emergencyButton.h"
#include <avr/cpufunc.h>

void emergencyButtonBegin(EmergencyButtonData* button, volatile uint8_t* pin, volatile uint8_t* port, volatile uint8_t* ddr, uint8_t bit)
{
	button -> pinReg = pin;
	button -> portReg = port;
	button -> ddrReg = ddr;
	button -> pinBit = bit;
	button -> pressedTimeout_ms = EMERGENCY_PRESSED_TIMEOUT_MS;
	button -> debounceTimeout_ms = EMERGENCY_DEBOUNCE_TIMEOUT_MS;
	*(button -> ddrReg) &= ~_BV(button -> pinBit); // Input
	*(button -> portReg) |= _BV(button -> pinBit); // Pullup
	MCUCR &= ~_BV(PUD); // Global enable
	_NOP();
	if (((*(button -> pinReg)) & _BV(button -> pinBit)) ^ _BV(button -> pinBit))
	{
		button -> state = EMERGENCY_PRESSED;
		button -> pendingPressedFlag  = 1;
	}
	else
	{
		button -> state = EMERGENCY_RELEASED;
		button -> pendingPressedFlag  = 0;
	}
}

void processEmergencyButton(EmergencyButtonData* button)
{
	// Inverted logic
	uint8_t buttonStatus = ((*(button -> pinReg)) & _BV(button -> pinBit)) ^ _BV(button -> pinBit);
	switch(button -> state)
	{
		case EMERGENCY_RELEASED:
			if (buttonStatus)
			{
				timer0UpdateTimer(&(button -> pressedTimer), TIMER_SET, button -> pressedTimeout_ms);
				button -> state = DEBOUNCE_PRESS;
			}
			else
			{
				button -> state = EMERGENCY_RELEASED;
			}
			break;
		case DEBOUNCE_PRESS:
			if (!buttonStatus)
			{
				timer0UpdateTimer(&(button -> pressedTimer), TIMER_RESET, 0xFFFF);
				button -> state = EMERGENCY_RELEASED;
				break;
			}
			if (timer0UpdateTimer(&(button -> pressedTimer), TIMER_CHECK_MATCH,  0))
			{
				timer0UpdateTimer(&(button -> pressedTimer), TIMER_RESET, 0xFFFF);
				button -> state = EMERGENCY_PRESSED;
				button -> pendingPressedFlag = 1;
				break;
			}
			if (buttonStatus)
			{
				button -> state = DEBOUNCE_PRESS;
				break;
			}
			break;
		case EMERGENCY_PRESSED:
			if (!buttonStatus)
			{
				timer0UpdateTimer(&(button -> debounceTimer), TIMER_SET, button -> debounceTimeout_ms);
				button -> state = DEBOUNCE_RELEASE;
			}
			else
			{
				button -> state = EMERGENCY_PRESSED;
				button -> pendingPressedFlag = 1;
			}
			break;
		case DEBOUNCE_RELEASE:
			if (timer0UpdateTimer(&(button -> debounceTimer), TIMER_CHECK_MATCH,  0)) // TODO check timeout)
			{
				timer0UpdateTimer(&(button -> debounceTimer), TIMER_RESET, 0xFFFF);
				button -> state = EMERGENCY_RELEASED;
				break;
			}
			else
			{
				button -> state = DEBOUNCE_RELEASE;
				break;
			}
	default:
		break;

	}
}

uint8_t checkEmergencyFlag(EmergencyButtonData* button)
{
	return button -> pendingPressedFlag;
}

void clearEmergencyFlag(EmergencyButtonData* button)
{
	button -> pendingPressedFlag = 0;
}
