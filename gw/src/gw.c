/**
 * Copyright (C) 2021, A. Ridyard.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v2.0 as
 * published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 * 
 * @file primary_receiver.c
 * 
 * @brief example of NRF24L01 setup as a primary receiver using the 
 * NRF24L01 driver. Different payload widths are set for the data pipes,
 * in order to receive different data structures from the transmitter.
 */

#include <stdio.h>

#include "nrf24_driver.h"
#include "pico/stdlib.h"

#include <tusb.h> // TinyUSB tud_cdc_connected()

int main(void)
{
  // initialize all present standard stdio types
  stdio_init_all();

  // wait until the CDC ACM (serial port emulation) is connected
  while (!tud_cdc_connected()) 
  {
    sleep_ms(10);
  }

  // GPIO pin numbers
  pin_manager_t my_pins = { 
    .sck = 2,
    .copi = 3, 
    .cipo = 4, 
    .csn = 5, 
    .ce = 6 
  };

  /**
   * nrf_manager_t can be passed to the nrf_client_t
   * initialise function, to specify the NRF24L01 
   * configuration. If NULL is passed to the initialise 
   * function, then the default configuration will be used.
   */
  nrf_manager_t my_config = {
    // RF Channel 
    .channel = 125,

    // AW_3_BYTES, AW_4_BYTES, AW_5_BYTES
    .address_width = AW_5_BYTES,

    // dynamic payloads: DYNPD_ENABLE, DYNPD_DISABLE
    .dyn_payloads = DYNPD_ENABLE,

    // data rate: RF_DR_250KBPS, RF_DR_1MBPS, RF_DR_2MBPS
    .data_rate = RF_DR_1MBPS,

    // RF_PWR_NEG_18DBM, RF_PWR_NEG_12DBM, RF_PWR_NEG_6DBM, RF_PWR_0DBM
    .power = RF_PWR_NEG_12DBM,

    // retransmission count: ARC_NONE...ARC_15RT
    .retr_count = ARC_10RT,

    // retransmission delay: ARD_250US, ARD_500US, ARD_750US, ARD_1000US
    .retr_delay = ARD_500US 
  };

  // SPI baudrate
  uint32_t my_baudrate = 5000000;

  // provides access to driver functions
  nrf_client_t my_nrf;

  // initialise my_nrf
  nrf_driver_create_client(&my_nrf);

  // configure GPIO pins and SPI
  my_nrf.configure(&my_pins, my_baudrate);

  // not using default configuration (my_nrf.initialise(NULL)) 
  my_nrf.initialise(&my_config);

  /**
   * set addresses for DATA_PIPE_0 - DATA_PIPE_3.
   * These are addresses the transmitter will send its packets to.
   */
  my_nrf.rx_destination(DATA_PIPE_0, (uint8_t[]){0x37,0x37,0x37,0x37,0x37});
  my_nrf.rx_destination(DATA_PIPE_1, (uint8_t[]){0xC7,0xC7,0xC7,0xC7,0xC7});
  my_nrf.rx_destination(DATA_PIPE_2, (uint8_t[]){0xC8,0xC7,0xC7,0xC7,0xC7});
  my_nrf.rx_destination(DATA_PIPE_3, (uint8_t[]){0xC9,0xC7,0xC7,0xC7,0xC7});

  // set to RX Mode
  my_nrf.receiver_mode();

  uint8_t coor[32];

  typedef struct payload_mpu_s
  {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t magne_x;
    int16_t magne_y;
    int16_t magne_z;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
  } payload_mpu_t;

  payload_mpu_t payload_mpu;
  // data pipe number a packet was received on
  uint8_t pipe_number = 0;

  while (1)
  {
    if (my_nrf.is_packet(&pipe_number))
    {
      switch (pipe_number)
      {
        case DATA_PIPE_0:
          // read payload
          my_nrf.read_packet(coor, sizeof(coor));

          // receiving a one byte uint8_t payload on DATA_PIPE_0
          //printf("\n%s ,", coor, pipe_number);
        break;
        
        case DATA_PIPE_1:
        break;
        
        case DATA_PIPE_2:

        // se manda: lat, long, acelx, acely, acelz, gyrox, gyroy, gyroz, magnex, magney, magnez
          // read payload
          my_nrf.read_packet(&payload_mpu, sizeof(payload_mpu));

          // receiving a two byte struct payload on DATA_PIPE_2
          printf("\n%s ,", coor);
          printf("%f, %f, %f, %d, %d, %d, %d, %d, %d\n", payload_mpu.accel_x*9.8/16384, payload_mpu.accel_y*9.8/16384, payload_mpu.accel_z*9.8/16384, payload_mpu.gyro_x,
          payload_mpu.gyro_y, payload_mpu.gyro_z, payload_mpu.magne_x, payload_mpu.magne_y, payload_mpu.magne_z);
          
        break;
        
        case DATA_PIPE_3:
        break;
        
        case DATA_PIPE_4:
        break;
        
        case DATA_PIPE_5:
        break;
        
        default:
        break;
      }
    }
  }
  
}