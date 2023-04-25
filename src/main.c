#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "MPU6050_i2c.h"
#include "cy273.h"

#define SDA_MPU 18 //pin 24
#define SCL_MPU 19 //pin 25
#define SDA_cy 8 //pin 11
#define SCL_cy 9 //pin 12

int main()
{
    stdio_init_all();

    i2c_init(MPU6050_i2c, 100 * 1000);
    i2c_init(cy273_i2c, 100 * 1000);
    gpio_set_function(SCL_MPU, GPIO_FUNC_I2C);
    gpio_set_function(SDA_MPU, GPIO_FUNC_I2C);
    gpio_set_function(SCL_cy, GPIO_FUNC_I2C);
    gpio_set_function(SDA_cy, GPIO_FUNC_I2C);
    gpio_pull_up(SCL_MPU);
    gpio_pull_up(SDA_MPU);
    gpio_pull_up(SCL_cy);
    gpio_pull_up(SDA_cy);

    mpu6050_reset();
    cy273_reset();

    int16_t acceleration[3], gyro[3], temp, magneto[3];

    while (1) {
        mpu6050_read_raw(acceleration, gyro, &temp);
        cy273_read(magneto);
        // These are the raw numbers from the chip, so will need tweaking to be really useful.
        // See the datasheet for more information
        printf("Acc. X = %d, Y = %d, Z = %d\n", acceleration[0], acceleration[1], acceleration[2]);
        printf("Gyro. X = %d, Y = %d, Z = %d\n", gyro[0], gyro[1], gyro[2]);
        // Temperature is simple so use the datasheet calculation to get deg C.
        // Note this is chip temperature.
        printf("Temp. = %f\n", (temp / 340.0) + 36.53);
        printf("Magne. X = %d, Y = %d, Z = %d\n", magneto[0], magneto[2], magneto[1]);

        sleep_ms(1000);
    }

    return 0;
}