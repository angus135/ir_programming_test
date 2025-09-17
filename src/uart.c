#include "uart.h"
#include "queue.h"
#include "processor_interface.h"

static Queue* transmit_queue;
static Queue* receive_queue;

static volatile uint32_t bytes_received;

static volatile uint8_t receive_queue_overflow; // 0 if not overflown, 1 if overflown

// If this was running on a true processor would use:
// void __attribute__((interrupt)) uart_isr(void)
void uart_isr(void) {
    // interrupt service routine
}

Status initialise_uart(void) {
    // Initialise queues
    transmit_queue = initialise_queue(QUEUE_SIZE);
    receive_queue = initialise_queue(QUEUE_SIZE);

    // Initialise bytes received to 0
    bytes_received = 0;

    // Initialise that the receive queue has not overflown
    receive_queue_overflow = 0;
}


int main() {
    printf("Hello Testing\n");
    return 0;
}