#include "Operation.hpp"
#include "spi.h"


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
    nrf24l01p_tx_transmit(wakeup_msg);
}

void Operation::receiveAckMessage(void)
{
    uint8_t rx_data[32];
    nrf24l01p_rx_receive(rx_data);

    // If the node does not exist, assign it a new slot
    if (!isNodeExist(rx_data[0]))
    {
        addNewNode(rx_data[0]);
    }
}

void Operation::checkNodeTimeouts(void)
{
    uint32_t currentTime = getCurrentTime();
	nodeCount = 0;
    for (size_t i = 0; i < nodes.size(); i++)
    {
        if (nodes[i].awake)
           nodeCount++;

        printf("Node ID : %d error : \n", nodes[i].id);
    }
    if(nodeCount != DEFAULT_NODE_QUANTITY)
    	printf("Connected Nodes are : %d\n", nodeCount);
    else
    	printf("All Nodes are present\n");

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

    nodes.push_back(newNode); // Dynamically add new node
    printf("New node with ID %f added to environment.\n", node_id);
}

void Operation::retryWakeUpNode(int nodeId)
{
    if (nodes[nodeId].retryCount < MAX_RETRIES)
    {
        printf("Retrying wake-up for Node %f (Attempt %d)\n", nodeId, nodes[nodeId].retryCount + 1);
        nrf24l01p_tx_transmit(wakeup_msg);  // Send wake-up message
        nodes[nodeId].retryCount++;  // Increment retry count
    }
    else
    {
        printf("Max retries reached for Node %f. Stopping wake-up attempts.\n", nodeId);
    }
}

void Operation::removeNode(int index)
{
    nodes.erase(nodes.begin() + index);
    printf("Node at index %f removed. Current node count: %zu\n", index, nodes.size());

}
