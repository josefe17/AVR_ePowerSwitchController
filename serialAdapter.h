/*
 * serialAdapter.h
 *
 *  Created on: 30 dic. 2019
 *      Author: josefe
 */

#ifndef SERIALADAPTER_H_
#define SERIALADAPTER_H_

#include <avr/io.h>

#define DATA_MASK 0x0F
#define ERROR_MASK 0xF0
#define TRANSMISSION_ERROR_MASK 0xF3
#define RECEPTION_ERROR_MASK 0xFC
#define TRANSMISSION_ERROR 0xF1
#define TRANSMISSION_FULL 0xF2
#define TRANSMISSION_OTHER 0xF3
#define RECEPTION_ERROR 0xF4
#define RECEPTION_BAD_COUNT 0xF8
#define RECEPTION_WRONG_DATA 0xFC

uint8_t readePowerSwitchState();
uint8_t writeePowerSwitchState(uint8_t swMask);

#endif /* SERIALADAPTER_H_ */
