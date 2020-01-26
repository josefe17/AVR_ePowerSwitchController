/*
 * emergencyButton.h
 *
 *  Created on: 15 ene. 2020
 *      Author: josefe
 */

#ifndef EMERGENCYBUTTON_H_
#define EMERGENCYBUTTON_H_


#include <avr/io.h>
#include "timer0_tick.h"


#define EMERGENCY_PRESSED_TIMEOUT_MS 500
#define EMERGENCY_DEBOUNCE_TIMEOUT_MS 20

typedef enum
{
	EMERGENCY_RELEASED,
	EMERGENCY_PRESSED,
	DEBOUNCE_RELEASE,
	DEBOUNCE_PRESS
}EmergencyButtonState;

typedef struct
{
	volatile uint8_t* pinReg;
	volatile uint8_t* ddrReg;
	volatile uint8_t* portReg;
	uint8_t pinBit;
	TickTimerEntity pressedTimer;
	uint16_t pressedTimeout_ms;
	TickTimerEntity debounceTimer;
	uint16_t debounceTimeout_ms;
	uint8_t pendingPressedFlag;
	EmergencyButtonState state;
}EmergencyButtonData;

//Creates the emergency button instance and configures its pin
void emergencyButtonBegin(EmergencyButtonData* button, volatile uint8_t* pin, volatile uint8_t* port, volatile uint8_t* ddr, uint8_t bit);

// Updates the emergency button state
void processEmergencyButton(EmergencyButtonData* button);

// Checks the emergency flag
uint8_t checkEmergencyFlag(EmergencyButtonData* button);

// Clears the emergency flag
void clearEmergencyFlag(EmergencyButtonData* button);

#endif /* EMERGENCYBUTTON_H_ */
