#include "Operation.hpp"
#include "spi.h"
#include "usart.h"

void Operation::setup()
{
    nrf24l01p_tx_init(2500, _1Mbps); // initialize it to transmit wake up to all active nodes
}

void Operation::loop()
{
    wakeUpSleepingNodes();
    checkNodeTimeouts();
}

void Operation::HAL_GPIO_EXTI_Callback_Tx()
{
        nrf24l01p_tx_irq(); // clear interrupt flag
}

void Operation::HAL_GPIO_EXTI_Callback_Rx()
{
        receiveAckMessage();
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
    uint8_t status = nrf24l01p_get_status();

    if (TX_DS(status) == 0x20)
    {
        return TX_DONE;
    }
    else if (RX_DS(status) == 0x40)
    {
        return RX_DONE;
    }

    return TX_DONE;
}

void Operation::wakeUpSleepingNodes()
{
    if (getCurrentTime() - lastWakeupTime > BROADCAST_PERIOD)  //Time Difference between 1st and 2nd broadcast will be 10sec and during this period device will be receiving data from nodes
    {
        sendWakeupMessage();
        lastWakeupTime = getCurrentTime();
    }
}

void Operation::sendWakeupMessage(void)
{
    char msg[50];
    snprintf(msg, sizeof(msg), "Server send %s\n", wakeup_msg);  // Format message to include wakeup_msg
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
    nrf24l01p_tx_transmit(wakeup_msg);
}


void Operation::receiveAckMessage(void)
{
    uint8_t rx_data[32];
    nrf24l01p_rx_receive(rx_data);

    if (!isNodeExist(rx_data[0]))
    {
        addNewNode(rx_data[0]);
    }
}

void Operation::checkNodeTimeouts(void)
{
    uint32_t currentTime = getCurrentTime();
    nodeCount = 0;
    char msg[100];  // Buffer for UART messages

    for (size_t i = 0; i < nodes.size(); i++)
    {
        if (nodes[i].awake)
            nodeCount++;

        snprintf(msg, sizeof(msg), "Node ID: %d, error: \n", nodes[i].id);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
    }

    if (nodeCount != DEFAULT_NODE_QUANTITY)
    {
        snprintf(msg, sizeof(msg), "Connected Nodes are: %d\n", nodeCount);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
    }
    else
    {
        snprintf(msg, sizeof(msg), "All Nodes are present\n");
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
    }
}

uint32_t Operation::getCurrentTime(void)
{
    return HAL_GetTick();
}

bool Operation::isNodeExist(uint8_t node_id)
{
	for (size_t i = 0; i < nodes.size(); i++)
    {
        if (nodes[i].id == node_id)
        {
            // Node exists, update its state and ack time
            nodes[i].awake = true;
            nodes[i].lastAckTime = getCurrentTime();
            return true;
        }
    }
    return false;
}

void Operation::addNewNode(uint8_t node_id)
{
    Node newNode;
    newNode.id = node_id;
    newNode.awake = true;
    newNode.lastAckTime = getCurrentTime();
    newNode.retryCount = 0;

    nodes.push_back(newNode);

    char msg[100];
    snprintf(msg, sizeof(msg), "New node with ID %d added to environment.\n", node_id);
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
}

void Operation::retryWakeUpNode(int nodeId)
{
    char msg[100];

    if (nodes[nodeId].retryCount < MAX_RETRIES)
    {
        snprintf(msg, sizeof(msg), "Retrying wake-up for Node %d (Attempt %d)\n", nodeId, nodes[nodeId].retryCount + 1);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);

        nrf24l01p_tx_transmit(wakeup_msg);
        nodes[nodeId].retryCount++;
    }
    else
    {
        snprintf(msg, sizeof(msg), "Max retries reached for Node %d. Stopping wake-up attempts.\n", nodeId);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
    }
}

void Operation::removeNode(int index)
{
    nodes.erase(nodes.begin() + index);

    char msg[100];
    snprintf(msg, sizeof(msg), "Node at index %d removed. Current node count: %zu\n", index, nodes.size());
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
}
