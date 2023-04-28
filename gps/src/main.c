#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdio.h>

#define uartTX 0
#define uartRX 1
#define UART_ID uart0

int baudrate = 9600; // Velocidad de transmisión en baudios
int data_bits = 8;   // Longitud de datos en bits
int parity = UART_PARITY_NONE; // Paridad: none para sin paridad, odd para paridad impar, even para paridad par

int main()
{
    stdio_init_all();
    
    // Configurar la UART
    uart_init(UART_ID, baudrate);
    // Configurar los pines GPIO 0 y GPIO 1 para la UART
    gpio_set_function(uartTX, GPIO_FUNC_UART);
    gpio_set_function(uartRX, GPIO_FUNC_UART);
    //uart_set_hw_flow(UART_ID, false, false);
    
    uart_set_format(UART_ID, data_bits, 1, parity);
    //uart_set_fifo_enabled(UART_ID, false);

    // Configure GPS module
    //uart_puts(UART_ID, "$PMTK220,1000*1F\r\n"); // Set update rate to 1 Hz
    //uart_puts(UART_ID, "$PMTK251,9600*17\r\n"); // Change baud rate to 9600

    char buf[256];
    while(1){

        if (uart_is_readable(UART_ID)){
            uart_read_blocking(UART_ID, buf, sizeof(buf)-1);
            printf("-------------------\n");
            printf("GPS: %s\n", buf);
            printf("-------------------\n");
            sleep_ms(20);
        } 

        
    }

    return 0;
}
