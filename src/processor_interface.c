#include "processor_interface.h"
#include <stdio.h>
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
#define STATUS_WRITABLE_MASK    ((1U << 13) | (1U << 14) | (1U << 15))
// Byte 0 - First byte of status register
// Byte 1 - Second byte of status register
// Byte 2 - First Byte of data register
// Byte 3 - Second Byte of data register
static volatile uint8_t register_values[4] = {0, 0, 0, 0};

uint8_t read_address_8bit(uint16_t* address) {
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

uint16_t read_address_16bit(uint16_t* address) {
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

uint32_t read_address_32bit(uint16_t* address) {
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

// Used generative AI to fill these out but had some slight adjustments
// But I can explain that this is essentially acting to mask so that only writable bits are written to
// And read only bits are left as is
static void apply_status_write(uint16_t value) {
    uint16_t current = (register_values[1] << 8) | register_values[0];
    uint16_t new_val = (current & ~STATUS_WRITABLE_MASK) | (value & STATUS_WRITABLE_MASK);
    register_values[0] = new_val & 0xFF;
    register_values[1] = (new_val >> 8) & 0xFF;
}

void write_address_8bit(uint16_t* address, uint8_t value) {
    switch ((uintptr_t)address) {
        case STATUS_REGISTER_ADDRESS:
            apply_status_write((register_values[1] << 8) | value);
            break;
        case STATUS_REGISTER_ADDRESS + 1:
            apply_status_write((value << 8) | register_values[0]);
            break;
        case DATA_REGISTER_ADDRESS:
            register_values[2] = value;
            break;
        case DATA_REGISTER_ADDRESS + 1:
            // register_values[3] = value;
            register_values[3] = 0x00; 
            break;
        default:
            // ignore invalid writes
            break;
    }
}

void write_address_16bit(uint16_t* address, uint16_t value) {
    switch ((uintptr_t)address) {
        case STATUS_REGISTER_ADDRESS:
            apply_status_write(value);
            break;
        case DATA_REGISTER_ADDRESS:
            register_values[2] = value & 0xFF;
            // register_values[3] = (value >> 8) & 0xFF;
            register_values[3] = 0x00; 
            break;
        default:
            // ignore invalid writes
            break;
    }
}

void write_address_32bit(uint16_t* address, uint32_t value) {
    switch ((uintptr_t)address) {
        case STATUS_REGISTER_ADDRESS:
            apply_status_write(value & 0xFFFF);
            // Ignore writing to data register
            break;
        case DATA_REGISTER_ADDRESS:
            register_values[2] = value & 0xFF;
            break;
        default:
            // ignore invalid writes
            break;
    }
}

void display_register_status(void) {
    printf("RX Register Not Empty: %d\n", register_values[0]&0x1);
    printf("TX Register Not Full : %d\n", (register_values[0]>>1)&0x1);
    printf("RX Error : %d\n", (register_values[0]>>2)&0x1);
    printf("TX Enable : %d\n", (register_values[1]>>5)&0x1);
    printf("RX Enable : %d\n", (register_values[1]>>6)&0x1);
    printf("UART Interrupt Enable : %d\n", (register_values[1]>>7)&0x1);
    printf("Global Interrupt Enable : %d\n", IS_INTERRUPT_ENABLED());
    printf("Data Register Data : Hex: %X, ascii: %c\n", register_values[2], register_values[2]);
}

void set_rx_not_empty(uint8_t value) {
    if (value == 0) {
        register_values[0] &= ~(1U << RX_NOT_EMPTY_BIT);
    } else {
        register_values[0] |= (1U << RX_NOT_EMPTY_BIT);
    }
    
}

void set_tx_not_full(uint8_t value) {
    if (value == 0) {
        register_values[0] &= ~(1U << TX_NOT_FULL_BIT);
    } else {
        register_values[0] |= (1U << TX_NOT_FULL_BIT);
    }
}

void set_rx_error(uint8_t value) {
    if (value == 0) {
        register_values[0] &= ~(1U << RX_ERROR_BIT);
    } else {
        register_values[0] |= (1U << RX_ERROR_BIT);
    }
}

void set_data_register(uint8_t value) {
    register_values[2] = value;
}