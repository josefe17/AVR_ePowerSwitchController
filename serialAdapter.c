/*
 * serialAdapter.c
 *
 *  Created on: 30 dic. 2019
 *      Author: josefe
 */


#include "serialAdapter.h"
#include "UART2.h"
#include <util/delay.h>

uint8_t readePowerSwitchState()
{
	uint8_t rxCache;
	// Flush RX buffer
	while (UART2_receiveAvailable() > 0)
	{
		(void) UART2_receive(&rxCache);
	}
	// Issue command
	if (UART2_transmit('R'))
	{
		return TRANSMISSION_ERROR; //Failed
	}
	if (UART2_transmit(0x7F))
	{
		return TRANSMISSION_ERROR; //Failed
	}
	if (UART2_transmit(0xC0))
	{
		return TRANSMISSION_ERROR; //Failed
	}
	_delay_ms(10);
	// Receive data
	if(!(UART2_receiveAvailable() >= 2))
	{
		return RECEPTION_BAD_COUNT;
	}
	if (UART2_receive(&rxCache))
	{
		return RECEPTION_ERROR;
	}
	if (rxCache != 0x5B)
	{
		return RECEPTION_WRONG_DATA;
	}
	if (UART2_receive(&rxCache))
	{
		return TRANSMISSION_OTHER;//RECEPTION_ERROR;
	}
	return rxCache & 0x0F;
}


uint8_t writeePowerSwitchState(uint8_t swMask)
{
	uint8_t rxCache;
	uint8_t feedback;
	// Flush RX buffer
	while (UART2_receiveAvailable() > 0)
	{
		(void) UART2_receive(&rxCache);
	}
	// Issue command
	if (UART2_transmit('W'))
	{
		return TRANSMISSION_ERROR; //Failed
	}
	if (UART2_transmit(0x7F))
	{
		return TRANSMISSION_ERROR; //Failed
	}
	if (UART2_transmit(0xC0))
	{
		return TRANSMISSION_ERROR; //Failed
	}
	// Check CTS
	_delay_ms(10);
	if(UART2_receiveAvailable() != 1)
	{
		return RECEPTION_BAD_COUNT;
	}
	if (UART2_receive(&rxCache))
	{
		return RECEPTION_ERROR;
	}
	if (rxCache != 0x5B)
	{
		return RECEPTION_WRONG_DATA;
	}
	// Issue command
	if (UART2_transmit(swMask & DATA_MASK))
	{
		return TRANSMISSION_ERROR; //Failed
	}
	for (uint8_t i = 31; i > 0; --i)
	{
		if (UART2_transmit(0))
		{
			return TRANSMISSION_ERROR; //Failed
		}
	}
	// Check data
	_delay_ms(10);
	if(UART2_receiveAvailable() != 2)
	{
		return RECEPTION_BAD_COUNT;
	}
	if (UART2_receive(&feedback))
	{
		return RECEPTION_ERROR;
	}
	if (UART2_receive(&rxCache))
	{
		return RECEPTION_ERROR;
	}
	if (rxCache != 0x5D)
	{
		return RECEPTION_WRONG_DATA;
	}
	while (UART2_receiveAvailable() > 0)
	{
		(void) UART2_receive(&rxCache);
	}
	// Issue store command
	if (UART2_transmit('W'))
	{
		return TRANSMISSION_ERROR; //Failed
	}
	if (UART2_transmit(0xFF))
	{
		return TRANSMISSION_ERROR; //Failed
	}
	if (UART2_transmit(0xFF))
	{
		return TRANSMISSION_ERROR; //Failed
	}
	// Check CTS
	_delay_ms(10);
	if(UART2_receiveAvailable() != 1)
	{
		return RECEPTION_BAD_COUNT;
	}
	if (UART2_receive(&rxCache))
	{
		return RECEPTION_ERROR;
	}
	if (rxCache != 0x5B)
	{
		return RECEPTION_WRONG_DATA;
	}
	return feedback & DATA_MASK;
}
