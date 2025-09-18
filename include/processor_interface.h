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

/* Some points about how this has been implemented
 - Addresses that are not part of these two registers are treated as read only that are set to
 0 always. Not really important to be creating a whole processor emulator here.
 - All possible ways of interacting with the register contents are implemented, even if they won't be
 used just so that I'm doing the best that I can to actually "simulate" interacting with the processor
 - Assume that second byte of data register is reserved and reads always as 0s and can't be written to
*/

// Global interrupt macros
static volatile uint8_t are_interrupts_enabled = 0;
#define INTERRUPT_DISABLE() are_interrupts_enabled = 0
#define INTERRUPT_ENABLE() are_interrupts_enabled = 1
#define IS_INTERRUPT_ENABLED() (are_interrupts_enabled)

uint8_t read_address_8bit(void* address);
uint16_t read_address_16bit(void* address);
uint32_t read_address_32bit(void* address);

void write_address_8bit(void* address, uint8_t value);
void write_address_16bit(void* address, uint16_t value);
void write_address_32bit(void* address, uint32_t value);

#endif