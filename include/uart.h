#ifndef UART_H
#define UART_H

#include <stdlib.h>
#include <stdint.h>
#include "macros.h"

// If this was running on a true processor would use:
// void __attribute__((interrupt)) uart_isr(void);
void uart_isr(void);

Status initialise_uart(void);

void stop_uart(void);

Status uart_write_bytes_to_transmit_queue(uint8_t* data, size_t size);

Status uart_read_bytes_from_receive_queue_blocking(uint8_t* data, size_t size);

Status uart_read_bytes_from_receive_queue_nonblocking(uint8_t* data, size_t size, size_t* bytes_read);

size_t uart_receive_queue_length(void);

size_t uart_transmit_queue_length(void);

uint32_t uart_total_bytes_received(void);

Status uart_receive_error(void);

Status uart_transmit_error(void);

#endif