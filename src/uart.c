#include "uart.h"
#include "queue.h"
#include "processor_interface.h"

static Queue* transmit_queue;
static Queue* receive_queue;

static volatile uint32_t bytes_received;

static volatile uint8_t receive_queue_error; // 0 if no error, 1 if error
// Transmit error not required but implemented in case of queue issues
static volatile uint8_t transmit_queue_error; // 0 if no error, 1 if error


// If this was running on a true processor would use:
// void __attribute__((interrupt)) uart_isr(void)
void uart_isr(void) {
    // Assuming here that some combination of the reads and writes done here clear the interrupt
    // and that there's no dedicated interrupt clearing required

    uint16_t status_register = read_address_16bit(STATUS_REGISTER_ADDRESS);
    if ((status_register & RX_INTERRUPT_BITS) == RX_INTERRUPT_BITS) {
        // Read from UART and add to receive queue
        uint8_t data = read_address_8bit(DATA_REGISTER_ADDRESS);
        // Assume here received data means received from UART rather than received and added to queue
        // If the latter then should check if queue full first then if there's no error read data register and increment counter
        bytes_received++;
        // Check if receive queue is full
        if (is_queue_full(receive_queue)) {
            receive_queue_error = 1;
        } else {
            Status return_status = enqueue(receive_queue, data); // Note here would use ISR safe enqueuing
            if (return_status == FAILURE) {
                receive_queue_error = 1;
            }
        }
    }

    if ((status_register & TX_INTERRUPT_BITS) == TX_INTERRUPT_BITS) {
        // If transmit queue is empty will just skip this
        if (!is_queue_empty(transmit_queue)) {
            // Read from transmit queue and add to UART
            uint8_t data;
            Status return_status = dequeue(transmit_queue, &data); // Note here would use ISR safe dequeuing
            // If there's an error in dequeuing flag error
            if (return_status == FAILURE) {
                transmit_queue_error = 1;
            } else {
                write_address_8bit(DATA_REGISTER_ADDRESS, data);
            }
        }
    }
}

Status initialise_uart(void) {
    // Initialise queues
    transmit_queue = initialise_queue(QUEUE_SIZE);
    receive_queue = initialise_queue(QUEUE_SIZE);
    
    if (transmit_queue == NULL || receive_queue == NULL) {
        return FAILURE;
    }

    // Initialise bytes received to 0
    bytes_received = 0;

    // Initialise that the receive queue has not overflown
    receive_queue_error = 0;
    transmit_queue_error = 0;

    // Initialising UART
    // Get status register state
    uint16_t status_register_state = read_address_16bit(STATUS_REGISTER_ADDRESS);
    // Set interrupt bit to enable interrupts
    status_register_state |= (1U << INTERRUPT_ENABLE_BIT);
    // Set Tx Enable bit
    status_register_state |= (1U << TX_ENABLE_BIT);
    // Set Rx Enable bit
    status_register_state |= (1U << RX_ENABLE_BIT);
    // Write updated status register
    write_address_16bit(STATUS_REGISTER_ADDRESS, status_register_state);

    INTERRUPT_ENABLE(); // Assuming here that we want to enable global interrupts

    return SUCCESS;
}


int main() {
    printf("Hello Testing\n");
    return 0;
}