#1. specify the CMake minimum version
cmake_minimum_required(VERSION 3.19)

#2. include the pico sdk here
include(pico_sdk_import.cmake)

#3. Specify the project name
project(gw)

#4. initialize the pico sdk
pico_sdk_init()

#5. add executable
add_executable(gw
    src/gw.c
    src/spi_manager.c
    src/nrf24_driver.c
    src/pin_manager.c
    
)

target_include_directories(gw PUBLIC inc/)

pico_enable_stdio_usb(gw 1)
pico_enable_stdio_uart(gw 0)
#6. Add extra outputs to the blink project
pico_add_extra_outputs(gw)

#7. add target libraries
target_link_libraries(gw pico_stdlib hardware_gpio hardware_i2c hardware_spi)