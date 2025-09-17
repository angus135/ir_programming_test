#ifndef PROCESSOR_INTERFACE_H
#define PROCESSOR_INTERFACE_H

#include <stdlib.h>
#include <stdint.h>

/* processor_interface.h and processor_interface.c are meant to act as interfaces for interaction
with the direct hardware of the processor. Things that are included in here are:
 - Accessing global interrupt mask through macros
 - Reading and writing to status register
 - Reading and writing to data register
*/

// Global interrupt macros
uint8_t are_interrupts_enabled = 0;
#define INTERRUPT_DISABLE() are_interrupts_enabled = 0
#define INTERRUPT_ENABLE() are_interrupts_enabled = 1
#define IS_INTERRUPT_ENABLED() (are_interrupts_enabled)

uint8_t read_address_8bit(void* address);
uint16_t read_address_16bit(void* address);
uint32_t read_address_32bit(void* address);

uint16_t write_address_16bit(void* address);

#endif