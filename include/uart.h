#ifndef UART_H
#define UART_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// If this was running on a true processor would use:
// void __attribute__((interrupt)) uart_isr(void);
void uart_isr(void);

Status initialise_uart(void);

void stop_uart(void);

int main();

#endif