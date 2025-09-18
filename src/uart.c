#include "uart.h"
#include "queue.h"
#include "processor_interface.h"
#include <stdio.h>

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

    uint16_t status_register = read_address_16bit((uint16_t*)STATUS_REGISTER_ADDRESS);
    if ((status_register>>RX_ERROR_BIT)&0x1) {
        // Check the Rx Error bit
        receive_queue_error = 1;
        // Assuming here that still increment bytes received even if there is an error, 
        // if didn't want to do this then remove this line
        bytes_received++; 
    } else if ((status_register & RX_INTERRUPT_BITS) == RX_INTERRUPT_BITS) {
        // Read from UART and add to receive queue
        uint8_t data = read_address_8bit((uint16_t*)DATA_REGISTER_ADDRESS);
        // Assume here received data means received from UART rather than received and added to queue
        // If the latter then should check if queue full first then if there's no error read data register and increment counter
        bytes_received++;

        if (is_queue_full(receive_queue)) {
            // Check if receive queue is full
            receive_queue_error = 1;
        } else {
            Status return_status = enqueue(receive_queue, data); // Note here would use ISR safe enqueuing
            if (return_status != SUCCESS) {
                receive_queue_error = 1;
            }
        }
    }

    if ((status_register & TX_INTERRUPT_BITS) == TX_INTERRUPT_BITS) {
        // Read from transmit queue and add to UART
        uint8_t data;
        Status return_status = dequeue(transmit_queue, &data); // Note here would use ISR safe dequeuing
        // If transmit queue is empty will just skip this
        if (return_status == SUCCESS) {
            write_address_8bit((uint16_t*)DATA_REGISTER_ADDRESS, data);
        } else if (return_status == FAILURE) {
            // If there's an error in dequeuing flag error
            transmit_queue_error = 1;
        }
        // If the transmit queue is empty then nothing happens
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
    uint16_t status_register_state = read_address_16bit((uint16_t*)STATUS_REGISTER_ADDRESS);
    // Set interrupt bit to enable interrupts
    status_register_state |= (1U << INTERRUPT_ENABLE_BIT);
    // Set Tx Enable bit
    status_register_state |= (1U << TX_ENABLE_BIT);
    // Set Rx Enable bit
    status_register_state |= (1U << RX_ENABLE_BIT);
    // Write updated status register
    write_address_16bit((uint16_t*)STATUS_REGISTER_ADDRESS, status_register_state);

    INTERRUPT_ENABLE(); // Assuming here that we want to enable global interrupts

    return SUCCESS;
}

void stop_uart(void) {
    // Disable global interrupts
    INTERRUPT_DISABLE();

    // Disable UART peripheral
    // Get status register state
    uint16_t status_register_state = read_address_16bit((uint16_t*)STATUS_REGISTER_ADDRESS);
    // Set interrupt bit to disable interrupts
    status_register_state &= ~(1U << INTERRUPT_ENABLE_BIT) ;
    // Set Tx Enable bit to disable
    status_register_state &= ~(1U << TX_ENABLE_BIT);
    // Set Rx Enable bit to disable
    status_register_state &= ~(1U << RX_ENABLE_BIT);
    // Write updated status register
    write_address_16bit((uint16_t*)STATUS_REGISTER_ADDRESS, status_register_state);

    // Deleting queues and freeing memory
    delete_queue(transmit_queue);
    delete_queue(receive_queue);
}

Status uart_write_bytes_to_transmit_queue(uint8_t* data, size_t size) {
    uint32_t counter = 0; // This wouldn't be here in real system, just here for this purpose
    Status status;
    for (uint16_t index = 0; index < size; index++) {
        status = enqueue(transmit_queue, data[index]);
        while (status == BUSY) {
            // Some sort of delay here that allows other tasks to continue
            // eg: vTaskDelay(1);
            counter++;

            status = enqueue(transmit_queue, data[index]);
        }
        // If there's some sort of failure of the queue, this wouldn't happen in my implementation
        // But if there were some sort of FreeRTOS Queue error that would be caught here
        if (status == FAILURE) { 
            return FAILURE;
        }
    }
    return SUCCESS;
}

Status uart_read_bytes_from_receive_queue_blocking(uint8_t* data, size_t size) {
    uint32_t counter = 0; // This wouldn't be here in real system, just here for this purpose
    Status status;
    for (uint16_t index = 0; index < size; index++) {
        status = dequeue(receive_queue, &data[index]);
        while (status == EMPTY) {
            // Some sort of delay here that allows other tasks to continue
            // eg: vTaskDelay(1);
            counter++;

            status = dequeue(receive_queue, &data[index]);
        }
        // If there's some sort of failure of the queue, this wouldn't happen in my implementation
        // But if there were some sort of FreeRTOS Queue error that would be caught here
        if (status == FAILURE) { 
            return FAILURE;
        }
    }
    return SUCCESS;
}

Status uart_read_bytes_from_receive_queue_nonblocking(uint8_t* data, size_t size, size_t* bytes_read) {
    Status status;
    for (uint16_t index = 0; index < size; index++) {
        status = dequeue(receive_queue, &data[index]);
        if (status == EMPTY) {
            if (bytes_read != NULL) {
                *bytes_read = index;
            }
            return SUCCESS;
        }
        // If there's some sort of failure of the queue, this wouldn't happen in my implementation
        // But if there were some sort of FreeRTOS Queue error that would be caught here
        if (status == FAILURE) { 
            return FAILURE;
        }
    }
    *bytes_read = size;
    return SUCCESS;
}

size_t uart_receive_queue_length(void) {
    pthread_mutex_lock(&receive_queue->mutex);
    size_t size = receive_queue->size;
    pthread_mutex_unlock(&receive_queue->mutex);
    return size;
}

size_t uart_transmit_queue_length(void) {
    pthread_mutex_lock(&transmit_queue->mutex);
    size_t size = transmit_queue->size;
    pthread_mutex_unlock(&transmit_queue->mutex);
    return size;
}

uint32_t uart_total_bytes_received(void) {
    return bytes_received;
}

Status uart_receive_error(void) {
    if (receive_queue_error == 0) {
        return SUCCESS;
    } else {
        return FAILURE;
    }
}

Status uart_transmit_error(void) {
    if (transmit_queue_error == 0) {
        return SUCCESS;
    } else {
        return FAILURE;
    }
}

void display_uart_status(void) {
    printf("UART Receive Queue length : %ld\n", uart_receive_queue_length());
    printf("UART Transmit Queue length : %ld\n", uart_transmit_queue_length());
    printf("Total bytes received : %d\n", uart_total_bytes_received());
    printf("UART Receive Receive Error Status : ");
    if (uart_receive_error() == SUCCESS) {
        printf("No Error\n");
    } else {
        printf("Error\n");
    }
    printf("UART Receive Transmit Error Status : ");
    if (uart_transmit_error() == SUCCESS) {
        printf("No Error\n");
    } else {
        printf("Error\n");
    }
}