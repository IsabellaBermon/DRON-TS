#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdio.h>
#include "gps.h"



void gps_init(int8_t uartTX, int8_t uartRX, int16_t baudrate, int8_t data_bits, int8_t parity){
    uart_init(UART_ID, baudrate);
    // Configurar los pines GPIO 0 y GPIO 1 para la UART
    gpio_set_function(uartTX, GPIO_FUNC_UART);
    gpio_set_function(uartRX, GPIO_FUNC_UART);
    //uart_set_hw_flow(UART_ID, false, false);
    uart_set_format(UART_ID, data_bits, 1, parity);
}

void read_gps(uint8_t buf[256]){
    uart_read_blocking(UART_ID, buf, sizeof(buf)-1);
}

