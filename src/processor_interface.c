#include "processor_interface.h"
// UART status register information
#define STATUS_REGISTER_ADDRESS 0x80000120
#define RX_NOT_EMPTY_BIT 0
#define TX_NOT_FULL_BIT 1
#define RX_ERROR_BIT 2
#define TX_ENABLE_BIT 13
#define RX_ENABLE_BIT 14
#define INTERRUPT_ENABLE_BIT 15

// UART data register information
#define DATA_REGISTER_ADDRESS 0x80000122 // Access most recently received byte here
// Byte 0 - First byte of status register
// Byte 1 - Second byte of status register
// Byte 2 - First Byte of data register
// Byte 3 - Second Byte of data register
static volatile uint8_t register_values[4] = {0, 0, 0, 0};

uint8_t read_address_8bit(void* address) {
    switch ((uintptr_t)address) {
        case STATUS_REGISTER_ADDRESS:
        return (uint8_t)(register_values[0]);
        break;
        case STATUS_REGISTER_ADDRESS + 1:
        return (uint8_t)(register_values[1]);
        break;
        case DATA_REGISTER_ADDRESS:
        return (uint8_t)(register_values[2]);
        break;
        case DATA_REGISTER_ADDRESS + 1:
        return (uint8_t)(register_values[3]);
        break;
        default:
        return 0x00U;
    }
}

uint16_t read_address_16bit(void* address) {
    switch ((uintptr_t)address) {
        case STATUS_REGISTER_ADDRESS-1:
        return (uint16_t)(register_values[0]<<8);
        break;
        case STATUS_REGISTER_ADDRESS:
        return (uint16_t)((register_values[1]<<8) | (register_values[0]));
        break;
        case STATUS_REGISTER_ADDRESS + 1:
        return (uint16_t)((register_values[2]<<8) | (register_values[1]));
        break;
        case DATA_REGISTER_ADDRESS:
        return (uint16_t)((register_values[3]<<8) | (register_values[2]));
        break;
        case DATA_REGISTER_ADDRESS + 1:
        return (uint16_t)(register_values[3]);
        break;
        default:
        return 0x00U;
    }
}

uint32_t read_address_32bit(void* address) {
    switch ((uintptr_t)address) {
        case STATUS_REGISTER_ADDRESS-3:
        return (uint32_t)(register_values[0]<<24);
        break;
        case STATUS_REGISTER_ADDRESS-2:
        return (uint32_t)((register_values[1]<<24) | (register_values[0]<<16));
        break;
        case STATUS_REGISTER_ADDRESS-1:
        return (uint32_t)((register_values[2]<<24) | (register_values[1]<<16) | (register_values[0]<<8));
        break;
        case STATUS_REGISTER_ADDRESS:
        return (uint32_t)((register_values[3]<<24) | (register_values[2]<<16) | (register_values[1]<<8) | (register_values[0]));
        break;
        case STATUS_REGISTER_ADDRESS + 1:
        return (uint32_t)((register_values[3]<<16) | (register_values[2]<<8) | (register_values[1]));
        break;
        case DATA_REGISTER_ADDRESS:
        return (uint32_t)((register_values[3]<<8) | (register_values[2]));
        break;
        case DATA_REGISTER_ADDRESS + 1:
        return (uint32_t)(register_values[3]);
        break;
        default:
        return 0x00U;
    }
}

uint16_t write_address_16bit(void* address);