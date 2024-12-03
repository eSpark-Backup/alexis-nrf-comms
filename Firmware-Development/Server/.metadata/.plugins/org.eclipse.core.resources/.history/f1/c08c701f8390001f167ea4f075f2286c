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

    char msg[100];
    snprintf(msg, sizeof(msg), "Received Message From Server: %s\n", (char*)rx_data);
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);

    if (strcmp((char*)rx_data, SERVER_MESSAGE) == 0)
    {
        nrf24l01p_tx_init(2500, _1Mbps); // Initialize to transmit ack data
        nrf24l01p_tx_transmit(ack_msg.data()); // Send acknowledgment message back to the server


        snprintf(msg, sizeof(msg), "Transmit Ack: %s\n", ack_msg.data());
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);

        nrf24l01p_rx_init(2500, _1Mbps); // Re-initialize to receive ack data
    }
}

void Operation::Rx_Tx_Callback_Handler()
{
    nrf_status_t currentStatus = getNrfStatus();

    switch (currentStatus)
    {
        case TX_DONE:
            HAL_GPIO_EXTI_Callback_Tx();
            break;

        case RX_DONE:
            HAL_GPIO_EXTI_Callback_Rx();
            break;

        default:
            break;
    }
}

nrf_status_t Operation::getNrfStatus()
{
    if (get_Tx_Config() == 0x00)
    {
        return TX_DONE;
    }
    else if (get_Rx_Config() == 0x01)
    {
        return RX_DONE;
    }
    return DEFAULT;
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
    HAL_PWR_EnableSleepOnExit();
   	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
    const char *setupMsg = "Setup complete, ready to receive data.\n";
    HAL_UART_Transmit(&huart2, (uint8_t*)setupMsg, strlen(setupMsg), 100);
}

void Operation::loop()
{
    // Add logic if necessary for the loop
}
