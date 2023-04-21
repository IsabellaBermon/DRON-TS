# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "C:/Program Files/Raspberry Pi/Pico SDK v1.5.0/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(telemetria_v1 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_subdirectory(pico-nrf24)

# Add executable. Default name is the project name, version 0.1

add_executable(telemetria_v1 telemetria_v1.c )

pico_set_program_name(telemetria_v1 "telemetria_v1")
pico_set_program_version(telemetria_v1 "0.1")

pico_enable_stdio_uart(telemetria_v1 0)
pico_enable_stdio_usb(telemetria_v1 1)

# Add the standard library to the build
target_link_libraries(telemetria_v1 pico_stdlib nrf24_driver)

pico_add_extra_outputs(telemetria_v1)

