#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#define MPU6050_i2c i2c1
#define MPU6050_addr 0x68

void mpu6050_init(int8_t SDA_MPU, int8_t SCL_MPU);
void mpu6050_reset();
void mpu6050_read(int16_t accel[3], int16_t gyro[3]);