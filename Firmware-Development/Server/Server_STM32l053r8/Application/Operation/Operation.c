#include "Operation.h"
//#include "spi.h"
//#include "usart.h"
#include "stm32l0xx_hal_uart.h"
extern UART_HandleTypeDef huart2;

// Function to initialize the operation
void setup(Operation *op)
{
    nrf24l01p_tx_init(2500, _1Mbps);  // Initialize for transmission
    Operation_sendWakeupMessage(op);  // Send wake-up message
}

// Function to loop through operations
bool loop(Operation *op)
{
    Operation_wakeUpSleepingNodes(op);
    if (op->Tx_ACK_FLAG)
    {
        char setupMsg[50] = "Transmitted\n";
        HAL_UART_Transmit(&huart2, (uint8_t*)setupMsg, strlen(setupMsg), 60);
        nrf24l01p_rx_init(2500, _1Mbps);  // Initialize to receive ACK
        op->Tx_ACK_FLAG = false;
    }
    if (op->Rx_ACK_FLAG)
    {
        char setupMsg[50] = "Waiting to Receive.....\n";
        HAL_UART_Transmit(&huart2, (uint8_t*)setupMsg, strlen(setupMsg), 60);
        op->Rx_ACK_FLAG = false;
    }
    if (Operation_checkNodeTimeouts(op))
        return true;
    return false;

    HAL_Delay(1000);
}

// Function to handle Tx interrupt
void Operation_HAL_GPIO_EXTI_Callback_Tx(Operation *op)
{
    op->Tx_ACK_FLAG = true;
    nrf24l01p_tx_irq();  // Clear interrupt flag
}

// Function to handle Rx interrupt
void Operation_HAL_GPIO_EXTI_Callback_Rx(Operation *op)
{
    op->Rx_ACK_FLAG = true;
    Operation_receiveAckMessage(op);
}

// Function to handle Rx/Tx callback
void Rx_Tx_Callback_Handler(Operation *op)
{
    nrf_status_t currentStatus = Operation_getNrfStatus(op);

    switch (currentStatus)
    {
        case Transmitter:
            Operation_HAL_GPIO_EXTI_Callback_Tx(op);
            break;

        case Receiver:
            Operation_HAL_GPIO_EXTI_Callback_Rx(op);
            break;

        default:
            break;
    }
}

// Function to get NRF status (Transmitter or Receiver)
nrf_status_t Operation_getNrfStatus(Operation *op)
{
    uint8_t mode = nrf24l01p_get_rx_tx_mode();
    char setupMsg[50];
    snprintf(setupMsg, sizeof(setupMsg), "NRF Mode: %s\n", (mode ? "Receiver" : "Transmitter"));
    HAL_UART_Transmit(&huart2, (uint8_t*)setupMsg, strlen(setupMsg), 60);
    return (mode ? Receiver : Transmitter);
}

// Function to wake up sleeping nodes
void Operation_wakeUpSleepingNodes(Operation *op)
{
    if (Operation_getCurrentTime(op) - op->lastWakeupTime > BROADCAST_PERIOD)
    {
        nrf24l01p_tx_init(2500, _1Mbps);  // Re-initialize for transmission
        Operation_sendWakeupMessage(op);
        op->lastWakeupTime = Operation_getCurrentTime(op);
    }
}


void send_uint32_as_string(UART_HandleTypeDef *huart, uint32_t data) {
    char buffer[12];

    sprintf(buffer, "%lu", data);

    if (HAL_UART_Transmit(huart, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY) != HAL_OK) {
        Error_Handler();
    }

    char newline[] = "\r\n";
    HAL_UART_Transmit(huart, (uint8_t *)newline, strlen(newline), HAL_MAX_DELAY);
}

void combine_data(uint32_t var1[3], const char* msg, uint8_t final[32]) {
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


// Function to send wake-up message
void Operation_sendWakeupMessage(Operation *op)
{
    snprintf(op->msg, sizeof(op->msg), "Server sends %s\n", op->wakeup_msg);
    HAL_UART_Transmit(&huart2, (uint8_t*)op->msg, strlen(op->msg), 100);
    nrf24l01p_tx_transmit(op->wakeup_msg);

    HAL_Delay(100);

    nrf24l01p_rx_init(2500, _1Mbps);  // Initialize to receive ACK
    HAL_Delay(100);
}

// Function to receive acknowledgment message
void Operation_receiveAckMessage(Operation *op)
{
    uint8_t rx_data[16];
    nrf24l01p_rx_receive(rx_data);

    if (!Operation_isNodeExist(op, rx_data))
    {
        Operation_addNewNode(op, rx_data);
    }
}

// Function to check for node timeouts
bool Operation_checkNodeTimeouts(Operation *op)
{
    char msg[100];
    int activeNodeCount = Operation_getActiveNodeCount(op);

    if (activeNodeCount <= DEFAULT_NODE_QUANTITY)
    {
        snprintf(msg, sizeof(msg), "Connected Nodes are: %d\n", activeNodeCount);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
        HAL_Delay(1000);
    }
    else
    {
        snprintf(msg, sizeof(msg), "All Nodes are present\n");
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
        return true;
    }

    return false;
}

// Function to get the current time
uint32_t Operation_getCurrentTime(Operation *op)
{
    return HAL_GetTick();
}

// Function to check if the node exists
bool Operation_isNodeExist(Operation *op, uint8_t node_id[ID_LENGTH])
{
    char msg[50];

    for (size_t i = 0; i < op->nodeCount; i++)
    {
        if (memcmp(op->nodes[i].id, node_id, sizeof(op->nodes[i].id)) == 0)
        {
            uint32_t timeDiff = Operation_getCurrentTime(op) - op->nodes[i].lastAckTime;
            snprintf(msg, sizeof(msg), "Time since last ACK for node %d: %lu ms\n", op->nodes[i].id, timeDiff);
            HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);

            if (timeDiff > NODE_TIMEOUT)
            {
                op->nodes[i].awake = false;
            }
            else
            {
                op->nodes[i].awake = true;
                op->nodes[i].lastAckTime = Operation_getCurrentTime(op);
            }
            return true;
        }
    }

    return false;
}

// Function to add a new node
void Operation_addNewNode(Operation *op, uint8_t node_id[ID_LENGTH])
{
    if (op->nodeCount < DEFAULT_NODE_QUANTITY)
    {
        Node newNode;
        memcpy(newNode.id, node_id, sizeof(newNode.id));
        newNode.awake = true;
        newNode.lastAckTime = Operation_getCurrentTime(op);
        newNode.retryCount = 0;

        op->nodes[op->nodeCount++] = newNode;

        uint16_t len = snprintf(op->msg, sizeof(op->msg), "New node added with ID: %d\n", newNode.id);
        HAL_UART_Transmit(&huart2, (uint8_t*)op->msg, len, 100);
    }
    else
    {
        uint16_t len = snprintf(op->msg, sizeof(op->msg), "Node limit reached!\n");
        HAL_UART_Transmit(&huart2, (uint8_t*)op->msg, len, 100);
    }
}

// Function to get active node count
int Operation_getActiveNodeCount(Operation *op)
{
    int activeNodeCount = 0;
    for (size_t i = 0; i < op->nodeCount; i++)
    {
        if (op->nodes[i].awake)
            activeNodeCount++;
    }
    return activeNodeCount;
}
