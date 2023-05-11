#1. specify the CMake minimum version
cmake_minimum_required(VERSION 3.19)

#2. include the pico sdk here
include(pico_sdk_import.cmake)

#3. Specify the project name
project(transmisor)

#4. initialize the pico sdk
pico_sdk_init()

#5. add executable
add_executable(transmisor
    src/transmisor.c
    src/spi_manager.c
    src/nrf24_driver.c
    src/pin_manager.c
    src/gps.c
    src/cy273_i2c.c
    src/MPU6050_i2c.c
)

target_include_directories(transmisor PUBLIC inc/)

pico_enable_stdio_usb(transmisor 1)
pico_enable_stdio_uart(transmisor 0)
#6. Add extra outputs to the blink project
pico_add_extra_outputs(transmisor)

#7. add target libraries
target_link_libraries(transmisor pico_stdlib hardware_gpio hardware_i2c hardware_spi pico_time)