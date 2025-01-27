#include "Operation.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>   // For rand, srand
#include <time.h>     // For time to seed rand
#include "nrf24l01p.h"
//#include "spi.h"
//#include "usart.h"    // Include for UART handling
#include "stm32l0xx_hal_uart.h"
extern UART_HandleTypeDef huart2;

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

void send_uint32_as_string(UART_HandleTypeDef *huart, uint32_t data)
{
    char buffer[12];

    sprintf(buffer, "%lu", data);

    if (HAL_UART_Transmit(huart, (uint8_t *)buffer, strlen(buffer), 100) != HAL_OK) {
        Error_Handler();
    }

    char newline[] = "\r\n";
    HAL_UART_Transmit(huart, (uint8_t *)newline, strlen(newline), 100);
}

void combine_data(uint32_t var1[3], const char* msg, uint8_t final[32])
{
    memset(final, 0, 32);

    for (int i = 0; i < 3; i++) {
        final[i * 4]     = (var1[i] >> 24) & 0xFF;
        final[i * 4 + 1] = (var1[i] >> 16) & 0xFF;
        final[i * 4 + 2] = (var1[i] >> 8) & 0xFF;
        final[i * 4 + 3] = var1[i] & 0xFF;
    }

    size_t msgLength = strlen(msg);
    if (msgLength > 20) {
        msgLength = 20;
    }
    memcpy(&final[12], msg, msgLength);
}

void generateAckMessage(Operation *op)
{
    // Get the UID values
    op->uid[0] = HAL_GetUIDw0();
    op->uid[1] = HAL_GetUIDw1();
    op->uid[2] = HAL_GetUIDw2();
//    send_uint32_as_string(&huart2, op->uid[0]);
//    send_uint32_as_string(&huart2, op->uid[1]);
//    send_uint32_as_string(&huart2, op->uid[2]);
    op->message = (char *)calloc(20, sizeof(char));
    strcpy(op->message, WAKEUP_MESSAGE);
//    HAL_UART_Transmit(&huart2, (uint8_t*)op->message, strlen(op->message), 100);

    size_t totalSize = (sizeof(op->uid[0])*3) + strlen(op->message) + 1; // +1 for null terminator

//	memcpy(op->ack_msg, op->uid, totalSize);
    combine_data(op->uid, op->message, op->ack_msg);
    const char *ackMessage = "Ack message generated.\n ack: ";
    HAL_UART_Transmit(&huart2, (uint8_t*)ackMessage, strlen(ackMessage), 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)op->ack_msg, sizeof(op->ack_msg), 100);
    char newline[] = "\r\n";
    HAL_UART_Transmit(&huart2, (uint8_t *)newline, strlen(newline), 100);
}

void setup(Operation *op)
{
    generateAckMessage(op);
    nrf24l01p_rx_init(2500, _1Mbps); // Initialize to receive data
    const char *setupMsg = "Setup complete, ready to receive data.\n";
    HAL_UART_Transmit(&huart2, (uint8_t*)setupMsg, strlen(setupMsg), 100);
}

void loop(Operation *op)
{
    if(op->Rx_MSG_FLAG)
    {
        char setupMsg[21]= "Recieved something.\n";
        HAL_UART_Transmit(&huart2, (uint8_t*)setupMsg, strlen(setupMsg), 60);
        HAL_UART_Transmit(&huart2, (uint8_t*)op->rx_data, strlen(op->rx_data), 60);
		char newline[] = "\r\n";
		HAL_UART_Transmit(&huart2, (uint8_t *)newline, strlen(newline), 100);
        op->Rx_MSG_FLAG = false;
    }

    if(op->Tx_Server_MSG_FLAG)
    {
        char setupMsg[20]= "Transmitting ACk.\n";
        HAL_UART_Transmit(&huart2, (uint8_t*)setupMsg, strlen(setupMsg), 60);
        op->Tx_Server_MSG_FLAG = false;
    }

    if(op->Rx_Server_MSG_FLAG)
    {
    	uint8_t delayPeriod = rand() % (250+1);
        char setupMsg[50] = "Getting Server message and sending Ack\n";
        HAL_UART_Transmit(&huart2, (uint8_t*)setupMsg, strlen(setupMsg), 60);
//        HAL_UART_Transmit(&huart2, (uint8_t*)delayPeriod, 1, 60);

        nrf24l01p_tx_init(2500, _1Mbps); // Initialize to transmit ack data
        HAL_Delay(delayPeriod);
        nrf24l01p_tx_transmit(op->ack_msg);  // Send acknowledgment message back to the server
        HAL_Delay(10);
        nrf24l01p_rx_init(2500, _1Mbps); // Re-initialize to receive ack data
        op->Rx_Server_MSG_FLAG = false;
    }

    HAL_Delay(1000);
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}
