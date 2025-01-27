#ifndef OPERATION_H
#define OPERATION_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "nrf24l01p.h"

#define NODE_TIMEOUT 50000
#define BROADCAST_PERIOD 10000
#define DEFAULT_NODE_QUANTITY 5
#define MAX_RETRIES 10
#define ID_LENGTH 12

// Define the Node structure
typedef struct
{
    uint8_t id[ID_LENGTH];      // Node ID                  // use 6bytes instead of 12 to store mac address?
    bool awake;                 // Awake status
    uint32_t lastAckTime;       // Last acknowledgment time
    uint8_t retryCount;         // Retry count
} Node;

typedef enum
{
    Transmitter = 0,
    Receiver = 1,
    DEFAULT
} nrf_status_t;

// Define the Operation structure
typedef struct
{
    Node nodes[DEFAULT_NODE_QUANTITY];  // Fixed array for simplicity, replace with dynamic if needed
    uint8_t nodeCount;                  // Node count
    uint32_t lastWakeupTime;            // Last wakeup time
    uint8_t wakeup_msg[NRF24L01P_PAYLOAD_LENGTH]; // Wakeup message
    char msg[150];                      // Message buffer
    bool Tx_ACK_FLAG;                   // Transmission ACK flag
    bool Rx_ACK_FLAG;                   // Reception ACK flag
} Operation;

// Function prototypes
void setup(Operation *op);
bool loop(Operation *op);
void Rx_Tx_Callback_Handler(Operation *op);
nrf_status_t Operation_getNrfStatus(Operation *op);
void Operation_wakeUpSleepingNodes(Operation *op);
void Operation_sendWakeupMessage(Operation *op);
void Operation_receiveAckMessage(Operation *op);
bool Operation_checkNodeTimeouts(Operation *op);
uint32_t Operation_getCurrentTime(Operation *op);
int Operation_getActiveNodeCount(Operation *op);
bool Operation_isNodeExist(Operation *op, uint8_t node_id[ID_LENGTH]);
void Operation_addNewNode(Operation *op, uint8_t node_id[ID_LENGTH]);
void Operation_HAL_GPIO_EXTI_Callback_Tx(Operation *op);
void Operation_HAL_GPIO_EXTI_Callback_Rx(Operation *op);

#endif // OPERATION_H
