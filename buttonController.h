/*
 * buttonController.h
 *
 *  Created on: 28 dic. 2019
 *      Author: josefe
 */

#ifndef BUTTONCONTROLLER_H_
#define BUTTONCONTROLLER_H_

#include <avr/io.h>
#include "timer0_tick.h"

#define DEFAULT_PRESSED_TIMEOUT_MS 20
#define LONG_PRESSED_TIMEOUT_MS 500
#define DEFAULT_DEBOUNCE_TIMEOUT_MS 20

typedef enum
{
	IDLE,
	PRESSED,
	PRESS_VALID,
	RELEASE_REQUIRED,
	DEBOUNCE
}ButtonState;

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
	uint8_t releaseRequiredFlag;
	ButtonState state;
}ButtonData;

//Creates the button instance and configures its pin
void buttonBegin(ButtonData* button, volatile uint8_t* pin, volatile uint8_t* port, volatile uint8_t* ddr, uint8_t bit);

// Updates the button state
void processButton(ButtonData* button);

//Checks if button was pressed enough time
//Call this function after the button update function to perform a release check before
uint8_t checkButton(ButtonData* button);

// Configures the required time the button shoul be pressed
void setButtonRequiredPressedTime(ButtonData* button, uint16_t time);

void forceRelease(ButtonData* button);


#endif /* BUTTONCONTROLLER_H_ */
