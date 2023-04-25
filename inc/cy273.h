#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#define cy273_i2c i2c0
#define cy273_addr 0x1E

void cy273_reset();
void cy273_read(int16_t magneto[3]);