#ifndef OPERATION_H
#define OPERATION_H

#include <stdint.h>
#include <stdbool.h>
#include <main.h>

#define SERVER_MESSAGE "Wake"
#define WAKEUP_MESSAGE "25.4,50.4,2,100.2,1"			// 19 characters
#define NRF24L01P_PAYLOAD_LENGTH 32  // Define payload length (adjust if needed)

typedef enum
{
    Transmitter = 0,
    Receiver = 1,
    DEFAULT
} nrf_status_t;

// Define the Operation structure
typedef struct
{
    uint8_t ack_msg[NRF24L01P_PAYLOAD_LENGTH];          // Acknowledgment message
    uint8_t Ack[8];              // Static acknowledgment data
    uint32_t uid[3];             // UID for the message
    char* message;               // Message string
    uint8_t rx_data[NRF24L01P_PAYLOAD_LENGTH];  // Data array for receiving data
    bool Rx_MSG_FLAG;            // Flag for received message
    bool Rx_Server_MSG_FLAG;     // Flag for server message
    bool Tx_Server_MSG_FLAG;     // Flag for transmitting server message

    void (*setup)(void);         // Function pointer for setup
    void (*loop)(void);          // Function pointer for loop
    void (*Rx_Tx_Callback_Handler)(void);  // Function pointer for callback
    void (*generateAckMessage)(void);  // Function pointer for generating ack message
    nrf_status_t (*getNrfStatus)(void);  // Function pointer for getting NRF status
} Operation;

void setup(Operation* op);
void loop(Operation* op);
void Rx_Tx_Callback_Handler(Operation* op);
void generateAckMessage(Operation* op);
nrf_status_t getNrfStatus(void);
void send_uint32_as_string(UART_HandleTypeDef *huart, uint32_t data);
void combine_data(uint32_t var1[3], const char* msg, uint8_t final[32]);

#endif // OPERATION_H
