#include "Operation.h"
#include "spi.h"
#include "usart.h"

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
bool Operation_isNodeExist(Operation *op, uint8_t node_id[12])
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
void Operation_addNewNode(Operation *op, uint8_t node_id[12])
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
