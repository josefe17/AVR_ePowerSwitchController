/*
 * ePowerSwitchController.h
 *
 *  Created on: 30 dic. 2019
 *      Author: josefe
 */

#ifndef EPOWERSWITCHCONTROLLER_H_
#define EPOWERSWITCHCONTROLLER_H_

#include "buttonController.h"
#include "LEDController.h"
#include "emergencyButton.h"

#define NUM_CHANNELS 4

struct _channel
{
	uint8_t stripChannel;
	ButtonData physicalButton;
	LEDData led;
	uint8_t switchStatus;
	uint8_t nextButtonStatus;
	uint8_t remoteSwitched;
	uint8_t linkedSwitched;
	uint8_t initialCycle;
	uint8_t longPressShutdownMode;
	uint8_t isLinked;
	struct _channel* linkedChannel;
};

typedef struct _channel Channel;

static const uint8_t NUM_RETRIES = 4;

Channel channels[NUM_CHANNELS];

EmergencyButtonData mushroom;

void initePowerSwitch();
void processAllChannels();

#endif /* EPOWERSWITCHCONTROLLER_H_ */
