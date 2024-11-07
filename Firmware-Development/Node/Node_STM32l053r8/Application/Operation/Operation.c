#include "Operation.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>   // For rand, srand
#include <time.h>     // For time to seed rand
#include "nrf24l01p.h"
#include "spi.h"
#include "usart.h"    // Include for UART handling

#define SERVER_MESSAGE "Wake"

// Define default NRF status
nrf_status_t NRF_STATUS = Transmitter; // Default status

// Implementation of the functions

void HAL_GPIO_EXTI_Callback_Tx(void)
{
    nrf24l01p_tx_irq(); // Clear interrupt flag
}

void HAL_GPIO_EXTI_Callback_Rx(Operation *op)
{
    // Receive data into the rx_data array of the Operation struct
    nrf24l01p_rx_receive(op->rx_data);

    // Check if the received data matches the predefined server message
    if (strcmp((char*)op->rx_data, SERVER_MESSAGE) == 0)
    {
        // Set the flag in the Operation struct to true
        op->Rx_Server_MSG_FLAG = true;
    }
}


void Rx_Tx_Callback_Handler(Operation *op)
{
    // Get the current status of the NRF24L01p (Transmitter or Receiver)
    nrf_status_t currentStatus = getNrfStatus();

    switch (currentStatus)
    {
        case Transmitter:
            // Handle the case when the device is in Transmitter mode
            HAL_GPIO_EXTI_Callback_Tx();
            break;

        case Receiver:
            // Handle the case when the device is in Receiver mode
            op->Rx_MSG_FLAG = true;
            // Pass the entire Operation struct to the callback function
            HAL_GPIO_EXTI_Callback_Rx(op);
            break;

        default:
            break;
    }
}


nrf_status_t getNrfStatus()
{
    uint8_t mode = nrf24l01p_get_rx_tx_mode();
    char setupMsg[50];
    snprintf(setupMsg, sizeof(setupMsg), "NRF Mode: %s\n", (mode ? "Receiver" : "Transmitter"));
    HAL_UART_Transmit(&huart2, (uint8_t*)setupMsg, strlen(setupMsg), 60);
    return (mode ? Receiver : Transmitter);
}

void generateAckMessage(Operation *op)
{
    // Get the UID values
    op->uid[0] = HAL_GetUIDw0();
    op->uid[1] = HAL_GetUIDw1();
    op->uid[2] = HAL_GetUIDw2();
    strcpy(op->message, "Waked up");

    size_t totalSize = sizeof(op->uid) + strlen(op->message) + 1; // +1 for null terminator

    memcpy(op->ack_msg, op->uid, sizeof(op->uid));
}

void setup(Operation *op)
{
    nrf24l01p_rx_init(2500, _1Mbps); // Initialize to receive data
    generateAckMessage(op);
    const char *setupMsg = "Setup complete, ready to receive data.\n";
    HAL_UART_Transmit(&huart2, (uint8_t*)setupMsg, strlen(setupMsg), 100);
}

void loop(Operation *op)
{
    if(op->Rx_MSG_FLAG)
    {
        char setupMsg[20]= "Recieve something.\n";
        HAL_UART_Transmit(&huart2, (uint8_t*)setupMsg, strlen(setupMsg), 60);
        op->Rx_MSG_FLAG = false;
    }

    if(op->Tx_Server_MSG_FLAG)
    {
        char setupMsg[20]= "Transmit ACk.\n";
        HAL_UART_Transmit(&huart2, (uint8_t*)setupMsg, strlen(setupMsg), 60);
        op->Tx_Server_MSG_FLAG = false;
    }

    if(op->Rx_Server_MSG_FLAG)
    {
        char setupMsg[50] = "Get Server message and send Ack\n";
        HAL_UART_Transmit(&huart2, (uint8_t*)setupMsg, strlen(setupMsg), 60);
        nrf24l01p_tx_init(2500, _1Mbps); // Initialize to transmit ack data
        nrf24l01p_tx_transmit(op->Ack);  // Send acknowledgment message back to the server
        HAL_Delay(10);
        nrf24l01p_rx_init(2500, _1Mbps); // Re-initialize to receive ack data
        op->Rx_Server_MSG_FLAG = false;
    }

    HAL_Delay(1000);
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}
