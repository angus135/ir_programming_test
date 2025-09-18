#ifndef MACROS_H
#define MACROS_H
// UART status register information
#define STATUS_REGISTER_ADDRESS 0x80000120
#define RX_NOT_EMPTY_BIT 0
#define TX_NOT_FULL_BIT 1
#define RX_ERROR_BIT 2
#define TX_ENABLE_BIT 13
#define RX_ENABLE_BIT 14
#define INTERRUPT_ENABLE_BIT 15

#define RX_INTERRUPT_BITS ((1U << RX_NOT_EMPTY_BIT) | (1U << RX_ENABLE_BIT)) // Bit mask for an RX interrupt
#define TX_INTERRUPT_BITS ((1U << TX_NOT_FULL_BIT) | (1U << TX_ENABLE_BIT)) // Bit mas for a TX interrupt

// UART data register information
#define DATA_REGISTER_ADDRESS 0x80000122 // Access most recently received byte here

// Queues
#define QUEUE_SIZE 256

// Status class to be used globally
typedef enum Status {
    SUCCESS,
    FAILURE
} Status;

#endif