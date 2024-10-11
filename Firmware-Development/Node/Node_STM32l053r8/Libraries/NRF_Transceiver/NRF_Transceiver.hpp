#pragma once
#include <cstring>
#include <cstdio>
#include "nrf24l01p.h"

#define SERVER_MESSAGE "Wake up all Device Nodes"


typedef enum
{
    _WakeUp  = 0,
    _Sleep  = 1

} node_status;


class NRF_Transceiver
{
private:
	uint8_t ack_msg[20] = "25.4,50.4,2,100.2,1"; // Message to send back to the server
	uint8_t rx_data[NRF24L01P_PAYLOAD_LENGTH] = { 0, };  // data array to be read

	void HAL_GPIO_EXTI_Callback_Tx(uint16_t GPIO_Pin); // for tx interrupt
	void HAL_GPIO_EXTI_Callback_Rx(uint16_t GPIO_Pin); // for rx interrupt
	void handleWakeUp();
	void handleSleep();

public:
	//node status
	node_status node_current_status = _Sleep;

	void setup();
	void loop();
};
