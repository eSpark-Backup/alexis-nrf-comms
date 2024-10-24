#include "Operation.hpp"
#include "spi.h"
#include "usart.h"    // Include for UART handling
#include <stdio.h>    // For snprintf
#include <stdlib.h>   // For rand, srand
#include <time.h>     // For time to seed rand

void Operation::HAL_GPIO_EXTI_Callback_Tx()
{
    nrf24l01p_tx_irq(); // Clear interrupt flag
}

void Operation::HAL_GPIO_EXTI_Callback_Rx()
{
    nrf24l01p_rx_receive(rx_data);


    if (strcmp((char*)rx_data, SERVER_MESSAGE) == 0)
    {
    	Rx_Server_MSG_FLAG = true;
       // nrf24l01p_tx_init(2500, _1Mbps); // Initialize to transmit ack data
       // nrf24l01p_print_config_status();
        //nrf24l01p_tx_transmit(Ack); // Send acknowledgment message back to the serve
        //nrf24l01p_rx_init(2500, _1Mbps); // Re-initialize to receive ack data
//        nrf24l01p_print_config_status();
    }
//    Rx_MSG_FLAG = false;

}

void Operation::Rx_Tx_Callback_Handler()
{

    nrf_status_t currentStatus = getNrfStatus();

    switch (currentStatus)
    {
        case Transmitter:
            HAL_GPIO_EXTI_Callback_Tx();
            break;

        case Receiver:
        	Rx_MSG_FLAG = true;
            HAL_GPIO_EXTI_Callback_Rx();
            break;

        default:
            break;
    }
}

nrf_status_t Operation::getNrfStatus()
{
    uint8_t mode = nrf24l01p_get_rx_tx_mode();
    char setupMsg[50];
    snprintf(setupMsg, sizeof(setupMsg), "NRF Mode: %s\n", (mode ? "Receiver" : "Transmitter"));
    HAL_UART_Transmit(&huart2, (uint8_t*)setupMsg, strlen(setupMsg), 60);
    return (mode ? Receiver : Transmitter);
}

void Operation::generateAckMessage()
{
    // Get the UID values
    uid[0] = HAL_GetUIDw0();
    uid[1] = HAL_GetUIDw1();
    uid[2] = HAL_GetUIDw2();
    message = "Waked up";

    size_t totalSize = sizeof(uid) + message.size() + 1; // +1 for null terminator

    ack_msg.resize(totalSize);
    memcpy(ack_msg.data(), uid, sizeof(uid));
}

void Operation::setup()
{
    nrf24l01p_rx_init(2500, _1Mbps); // Initialize to receive data
    generateAckMessage();
    const char *setupMsg = "Setup complete, ready to receive data.\n";
    HAL_UART_Transmit(&huart2, (uint8_t*)setupMsg, strlen(setupMsg), 100);
//    HAL_PWR_EnableSleepOnExit();
//   	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

void Operation::loop()
{
    // Add logic if necessary for the loop
	// Add logic if necessary for the loop
//	char setupMsg[20] =".";
//    HAL_UART_Transmit(&huart2, (uint8_t*)setupMsg, strlen(setupMsg), 60);
//        nrf24l01p_print_config_status();

		if(Rx_MSG_FLAG)
		{
		  char setupMsg[20]= "Recieve something.\n";
		   HAL_UART_Transmit(&huart2, (uint8_t*)setupMsg, strlen(setupMsg), 60);
		   Rx_MSG_FLAG = false;
		}

		if(Tx_Server_MSG_FLAG)
		{
		  char setupMsg[20]= "Transmit ACk.\n";
		   HAL_UART_Transmit(&huart2, (uint8_t*)setupMsg, strlen(setupMsg), 60);
		   Tx_Server_MSG_FLAG = false;
		}

		if(Rx_Server_MSG_FLAG)
		{
			char setupMsg[50] = "Get Server message and send Ack\n";
			HAL_UART_Transmit(&huart2, (uint8_t*)setupMsg, strlen(setupMsg), 60);
			nrf24l01p_tx_init(2500, _1Mbps); // Initialize to transmit ack data
		   // nrf24l01p_print_config_status();
			nrf24l01p_tx_transmit(Ack); // Send acknowledgment message back to the serve
			HAL_Delay(10);
			nrf24l01p_rx_init(2500, _1Mbps); // Re-initialize to receive ack data
			 Rx_Server_MSG_FLAG = false;
		}

		HAL_Delay(1000);
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}
