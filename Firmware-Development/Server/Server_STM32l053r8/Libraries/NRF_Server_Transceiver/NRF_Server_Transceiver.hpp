#pragma once
#include <cstring>
#include <cstdio>
#include "nrf24l01p.h"

#define NODE_TIMEOUT 10000
#define INVALID_NODE_ID 255
#define MAX_RETRIES 10

struct Node {
    uint8_t id;             // Node ID, initialized to INVALID_NODE_ID
    bool awake;             // Awake status
    uint32_t lastAckTime;   // Last acknowledgment time
    uint8_t retryCount;     // Retry count
};

class NRF_Server_Transceiver
{
private:
    Node* nodes;
    uint32_t lastWakeupTime = 0; // To manage wakeup message sending
    uint8_t wakeup_msg[NRF24L01P_PAYLOAD_LENGTH] = {0, 1, 2, 3, 4, 5, 6, 7}; // Wakeup message to be sent to nodes
    int maxNodes;           // Maximum number of nodes (can be set dynamically)

    void setupNodeEnvironment();
    void wakeUpSleepingNodes();
    void sendWakeupMessage(void);
    void receiveAckMessage(void);
    void checkNodeTimeouts(void);
    uint32_t getCurrentTime(void);
    bool isNodeExist(uint8_t node_id);
    void addNewNode(uint8_t node_id);
    void retryWakeUpNode(int nodeId);
    void removeNode(int index);

public:
    NRF_Server_Transceiver(int nodeCount = 10);  // Constructor with dynamic size option
    ~NRF_Server_Transceiver();  // Destructor to clean up dynamic memory

    void setup();
    void loop();
    void HAL_GPIO_EXTI_Callback_Tx(uint16_t GPIO_Pin); // for tx interrupt
    void HAL_GPIO_EXTI_Callback_Rx(uint16_t GPIO_Pin); // for rx interrupt
};
