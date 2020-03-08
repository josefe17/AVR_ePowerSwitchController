/*
 * ePowerSwitchController.c
 *
 *  Created on: 30 dic. 2019
 *      Author: josefe
 */

#include "ePowerSwitchController.h"
#include "serialAdapter.h"

// EpowerSwitch channels statuses on last read
uint8_t ePowerSwitchReadStatus;

// Write mask for next updates
uint8_t ePowerSwitchWriteMask;

// Global error flag
uint8_t serialError;

uint8_t serialErrorCounter;

// A channel must be updated
uint8_t pendingUpdate;

uint8_t errorNotTriggered;

void processChannel(Channel* channel);
void configureButtonAction(Channel* this);
void handleButtonAction(Channel* this);

void setChannelLED(Channel* channel);
void setEmergencyStopLeds();
uint8_t handleSerialError();

uint8_t getePowerSwitchChannelFlag(uint8_t channel);
uint8_t setePowerSwitchChannelFlag(uint8_t channel, uint8_t value);

void retrieveDataFromePowerSwitch();
void writeDataToePowerSwitch();

void processAllChannels()
{
	processEmergencyButton(&mushroom);
	retrieveDataFromePowerSwitch();
//	if (handleSerialError() > NUM_RETRIES)
//	{
//		return;
//	}
	for (uint8_t i = 0; i < NUM_CHANNELS; ++i)
	{
		processChannel(&channels[i]);
	}
	if (checkEmergencyFlag(&mushroom))
	{
		clearEmergencyFlag(&mushroom);
		for (uint8_t i = 0; i < NUM_CHANNELS; ++i)
		{
			setePowerSwitchChannelFlag(channels[i].stripChannel, 0); // OFF
			channels[i].nextButtonStatus = 0;
			forceRelease(&channels[i].physicalButton);
			pendingUpdate = 0xFF;
		}
		setEmergencyStopLeds();
	}
	writeDataToePowerSwitch();
//	if (handleSerialError() > NUM_RETRIES)
//	{
//		return;
//	}
}

void initePowerSwitch()
{
	ePowerSwitchReadStatus = 0;
	ePowerSwitchWriteMask = 0;
	pendingUpdate = 0;
	serialError = 0;
	errorNotTriggered = 1;
	for (uint8_t i = 0; i < NUM_CHANNELS; ++i)
	{
		channels[i].initialCycle = 0xFF;
		channels[i].linkedSwitched = 0;
	}
}

void processChannel(Channel* channel)
{
	// First, update sw status from epowerswitch
	// if state differs from button state, update leds, off or yellow
	// then, configure button to toggle the current state, setting the shutdown
	// longpress timeout if this mode is set
	// process the button
	uint8_t remoteStatus = getePowerSwitchChannelFlag(channel -> stripChannel);
	// Level sensitive on startup
	// State retrieved from ePowerSwitch is considered absolute
	if (channel -> initialCycle)
	{
		channel -> initialCycle = 0;
		channel -> switchStatus = remoteStatus;
		channel -> nextButtonStatus = remoteStatus;
		if (remoteStatus) // If on, remote mode
		{
			channel -> remoteSwitched = 0xFF;
		}
		else
		{
			channel -> remoteSwitched = 0;
		}
	}
	else
	{
		if (remoteStatus != (channel -> switchStatus)) //Remote change
		{
			if (remoteStatus == channel -> nextButtonStatus) // Last button press was successful processed
			{
				channel -> remoteSwitched = 0;
				channel -> switchStatus = channel -> nextButtonStatus;
			}
			else //Remote switching
			{
				channel -> remoteSwitched = 0xFF;
				channel -> switchStatus = remoteStatus;
				channel -> nextButtonStatus = remoteStatus;
			}
		}
	}
	setChannelLED(channel);
	configureButtonAction(channel);
	processButton(&(channel -> physicalButton));
	handleButtonAction(channel);
	ledUpdate(&(channel -> led));
}

void configureButtonAction(Channel* this)
{
	// If channel is on and longpress for shutdown is enabled,
	// button timeout is long, else is short
	if (this -> switchStatus)
	{
		setButtonRequiredPressedTime( &(this -> physicalButton), LONG_PRESSED_TIMEOUT_MS);
	}
	else
	{
		setButtonRequiredPressedTime( &(this -> physicalButton), DEFAULT_PRESSED_TIMEOUT_MS);
	}
}

void handleButtonAction(Channel* this)
{
	if (checkButton(&(this -> physicalButton))) // If button was pressed long enough
	{
		if (this -> switchStatus) // If on
		{
			setePowerSwitchChannelFlag(this -> stripChannel, 0); // OFF
			this -> nextButtonStatus = 0;
			for (uint8_t i = 0; i < NUM_CHANNELS; ++i)  // If any channel is linked, also turn it off
			{
				if (channels[i].linkedChannel == this)
				{
					if (channels[i].isLinked)// If that channel linking is enabled
					{
						setePowerSwitchChannelFlag(channels[i].stripChannel, 0);
						channels[i]. nextButtonStatus = 0;
						channels[i].linkedSwitched = 0xFF;
					}
				}
			}
			pendingUpdate = 0xFF;
		}
		else
		{
			if (this -> isLinked)
			{
				if ((this -> linkedChannel) -> switchStatus)
				{
					setePowerSwitchChannelFlag(this -> stripChannel, 1); // ON
					this -> nextButtonStatus = 1;
					pendingUpdate = 0xFF;
				}
			}
			else
			{
				setePowerSwitchChannelFlag(this -> stripChannel, 1); // ON
				this -> nextButtonStatus = 1;
				pendingUpdate = 0xFF;
			}
		}
	}
	else // Write mask to the current value to keep its consistency
	{
		if (!this -> linkedSwitched) // Avoid overriding the linked value
		{
			if (this -> switchStatus)
			{
				setePowerSwitchChannelFlag(this -> stripChannel, 1); // ON
			}
			else
			{
				setePowerSwitchChannelFlag(this -> stripChannel, 0); // OFF
			}
		}
		else
		{
			this -> linkedSwitched = 0;
		}
	}
}

void setChannelLED(Channel* channel)
{
	// Button LED only will be on if epowerswitch processed it properly
	ledBlink(&(channel -> led), BLINK_OFF);
	if (!(channel -> switchStatus))
	{
		ledSetColor(&(channel -> led), OFF);
	}
	else
	{
		if (channel -> remoteSwitched)
		{
			ledSetColor(&(channel -> led), YELLOW);
		}
		else
		{
			ledSetColor(&(channel -> led), GREEN);
		}
	}
}

void setEmergencyStopLeds()
{
	for (uint8_t i = 0; i < NUM_CHANNELS; ++i)
	{
		ledBlink(&(channels[i].led), BLINK_OFF);
		ledSetColor(&(channels[i].led), RED);
	}
}

uint8_t handleSerialError()
{
	uint8_t serialErrorCounterBuffer = 0;
	if (serialErrorCounter >= NUM_RETRIES)
	{
		serialErrorCounterBuffer = serialErrorCounter;
		serialErrorCounter = 0;
		switch(serialError)
		{
			case 0:
			default:
				break;
			case TRANSMISSION_ERROR:
				ledSetColor(&(channels[0].led), RED);
				ledSetColor(&(channels[1].led), RED);
				ledSetColor(&(channels[2].led), OFF);
				ledSetColor(&(channels[3].led), OFF);
				break;
			case TRANSMISSION_FULL:
				ledSetColor(&(channels[0].led), RED);
				ledSetColor(&(channels[1].led), OFF);
				ledSetColor(&(channels[2].led), OFF);
				ledSetColor(&(channels[3].led), OFF);
				break;
			case TRANSMISSION_OTHER:
				ledSetColor(&(channels[0].led), OFF);
				ledSetColor(&(channels[1].led), RED);
				ledSetColor(&(channels[2].led), OFF);
				ledSetColor(&(channels[3].led), OFF);
				break;
			case RECEPTION_ERROR:
				ledSetColor(&(channels[0].led), OFF);
				ledSetColor(&(channels[1].led), OFF);
				ledSetColor(&(channels[2].led), RED);
				ledSetColor(&(channels[3].led), RED);
				break;
			case RECEPTION_BAD_COUNT:
				ledSetColor(&(channels[0].led), OFF);
				ledSetColor(&(channels[1].led), OFF);
				ledSetColor(&(channels[2].led), RED);
				ledSetColor(&(channels[3].led), OFF);
				break;
			case RECEPTION_WRONG_DATA:
				ledSetColor(&(channels[0].led), OFF);
				ledSetColor(&(channels[1].led), OFF);
				ledSetColor(&(channels[2].led), OFF);
				ledSetColor(&(channels[3].led), RED);
				break;
		}
	}
	return serialErrorCounterBuffer;
}

uint8_t getePowerSwitchChannelFlag(uint8_t channel)
{
	if (channel < 0 || channel > 3)
	{
		return 0;
	}
	if (ePowerSwitchReadStatus & _BV(channel))
	{
		return 1;
	}
	return 0;
}

uint8_t setePowerSwitchChannelFlag(uint8_t channel, uint8_t value)
{
	if (channel < 0 || channel > 3)
	{
		return 0xFF;
	}
	if (value)
	{
		ePowerSwitchWriteMask |= _BV(channel);
	}
	else
	{
		ePowerSwitchWriteMask &= ~_BV(channel);
	}
	return 0;
}

void retrieveDataFromePowerSwitch()
{
	uint8_t readBuff = readePowerSwitchState();
	if (readBuff & ERROR_MASK)
	{
		serialError = readBuff;
		++serialErrorCounter;
	}
	else
	{
		serialError = 0;
		serialErrorCounter = 0;
		ePowerSwitchReadStatus = readBuff;
	}
}

void writeDataToePowerSwitch()
{
	if (pendingUpdate)
	{
		uint8_t readBuff = writeePowerSwitchState(ePowerSwitchWriteMask);
		if (readBuff & ERROR_MASK)
		{
			serialError = readBuff;
			++serialErrorCounter;
		}
		else
		{
			// No serial error but eps returned values is different from sent (bad feedback)
			if (readBuff != (ePowerSwitchWriteMask & DATA_MASK))
			{
				serialError = 0xFF; // Bad write feedback error
				++serialErrorCounter;
			}
			else
			{
				serialError = 0;
				serialErrorCounter = 0;
				pendingUpdate = 0;
			}
		}
	}
}
