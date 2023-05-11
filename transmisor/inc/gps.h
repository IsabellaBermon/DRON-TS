#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdio.h>

#define UART_ID uart0

void gps_init(int8_t uartTX, int8_t uartRX, int16_t baudrate, int8_t data_bits, int8_t parity);
void read_gps(uint8_t buf[256]);