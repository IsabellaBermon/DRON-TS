#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "cy273.h"

void cy273_init(int8_t SDA_cy,int8_t SCL_cy){
    i2c_init(cy273_i2c, 100 * 1000);
    gpio_set_function(SCL_cy, GPIO_FUNC_I2C);
    gpio_set_function(SDA_cy, GPIO_FUNC_I2C);
    gpio_pull_up(SCL_cy);
    gpio_pull_up(SDA_cy);
}

void cy273_reset(){
    uint8_t data[1] = {0x3C};
    i2c_write_blocking(i2c0, cy273_addr, data, sizeof(data), true);
}

void cy273_read(int16_t magneto[3]){
    uint8_t buffer[6];

    uint8_t val = 0x03;

    i2c_write_blocking(cy273_i2c, cy273_addr, &val, sizeof(val), true);
    i2c_read_blocking(cy273_i2c, cy273_addr, buffer, sizeof(buffer), false);

    magneto[0] = (buffer[0] << 8) | buffer[1];  //x
    magneto[1] = (buffer[2] << 8) | buffer[3];  //z
    magneto[2] = (buffer[4] << 8) | buffer[5];  //y
}


