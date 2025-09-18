#include <stdio.h>
#include "uart.h"
#include "processor_interface.h"

int main(void) {
    printf("Beginning UART tests...\n\n\n");
    printf("Initial Register values:\n\n");
    display_register_status();

    printf("\nInitialising UART...\n");
    Status return_status = initialise_uart();
    if (return_status != SUCCESS) {
        printf("Initialising UART failed!\n");
        return 0;
    }

    printf("Register values after UART initilisation:\n\n");
    display_register_status();

    printf("Initial UART status:\n\n");
    display_uart_status();

    printf("\n\nSimulating sending data via transmit\n\n");

    printf("Adding \'H\', \'i\' to the transmit queue\n");
    char test_message[] = {'H', 'i'};
    size_t message_len = 2;
    uart_write_bytes_to_transmit_queue((uint8_t*)test_message, message_len);
    printf("Message added!\n\n");

    printf("UART status after adding to transmit queue:\n");
    display_uart_status();

    printf("Simulating UART peripheral setting TX Register Not Full to high\n");
    set_tx_not_full(1);
    display_register_status();

    printf("\nTriggering 3 interrupts (should only be 2 but to demonstrate no errors)\n");
    for (int i = 0; i < 3; i++) {
        uart_isr();
        printf("\nRegister Values after %d interrupts:\n", i+1);
        display_register_status();
        printf("\nUART status after %d interrupts:\n", i+1);
        display_uart_status();
    }

    printf("\n\nSimulating receiving data\n\n");
    printf("Setting Tx not full back to 0\n");
    set_tx_not_full(0);
    printf("Setting Rx not empty to 1\n");
    set_rx_not_empty(1);
    printf("New register values:\n");
    display_register_status();

    printf("\nTriggering 2 interrupts and putting \'H\', \'i\' through receive \n");

    set_data_register((uint8_t)('H'));
    uart_isr();
    printf("\nRegister Values after 1 interrupt:\n");
    display_register_status();
    printf("\nUART status after 1 interrupts:\n");
    display_uart_status();

    set_data_register((uint8_t)('i'));
    uart_isr();
    printf("\nRegister Values after 2 interrupts:\n");
    display_register_status();
    printf("\nUART status after 2 interrupts:\n");
    display_uart_status();

    uint8_t returning[5];
    size_t bytes_read;
    uart_read_bytes_from_receive_queue_nonblocking(returning, 5, &bytes_read);
    printf("Read %ld bytes from the buffer:", bytes_read);
    for (size_t i=0; i<bytes_read; i++) {
        printf("%c, ", returning[i]);
    }


    printf("\n\nFilling queue with 257 messages:\n");
    for (int i = 0; i<257; i++) {
        uart_isr();
    }
    printf("\nUART status after 257 messages:\n");
    display_uart_status();

    printf("\nStopping UART\n");
    stop_uart();

    return 0;
}