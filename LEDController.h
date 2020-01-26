/*
 * LEDController.h
 *
 *  Created on: 3 ene. 2020
 *      Author: josefe
 */

#ifndef LEDCONTROLLER_H_
#define LEDCONTROLLER_H_

#include "timer0_tick.h"

typedef enum
{
	OFF,
	GREEN,
	YELLOW,
	RED
}LEDColor;

typedef enum
{
	BLINK_OFF,
	DUTY_ON,
	DUTY_OFF
}BlinkStatus;

typedef struct
{
	BlinkStatus status;
	uint8_t tOn;
	uint8_t tOff;
	TickTimerEntity blinkTimer;
}LEDBlink;

typedef struct
{
	volatile uint8_t* redPortReg;
	volatile uint8_t* redDdrReg;
	volatile uint8_t* greenPortReg;
	volatile uint8_t* greenDdrReg;
	uint8_t redBit;
	uint8_t greenBit;
	LEDColor color;
	LEDBlink blink;
}LEDData;

// initializes the tri-color LED controller
void ledBegin(LEDData* led, volatile uint8_t* redPort, volatile uint8_t* redDdr, uint8_t redBit, volatile uint8_t* greenPort, volatile uint8_t* greenDdr, uint8_t greenBit);

// Turns the LED ON or OFF
void ledSetColor(LEDData* led, LEDColor color);

// Configures the blinking periods
void ledSetBlinkPeriods(LEDData* led, uint8_t tOn_100ms, uint8_t tOff_100ms);

// Starts and stops the blinking
void ledBlink(LEDData* led, BlinkStatus initialStatus);

// Handles the LED background tasks (blink updates)
void ledUpdate(LEDData* led);

#endif /* LEDCONTROLLER_H_ */
