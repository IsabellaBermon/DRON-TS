#include <stdio.h>
#include "nrf24_driver.h"
#include "pico/stdlib.h"
#include <tusb.h> // TinyUSB tud_cdc_connected()
#include "MPU6050_i2c.h"
#include "cy273.h"

int main(void)
{
  // initialize all present standard stdio types
  stdio_init_all();

  // wait until the CDC ACM (serial port emulation) is connected
  while (!tud_cdc_connected()) 
  {
    sleep_ms(10);
  }
  
  // initialize sensors
  mpu6050_init();
  cy273_init();
  mpu6050_reset();
  cy273_reset();
  int16_t acceleration[3], gyro[3], temp, magneto[3];

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
    .channel = 120,

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

  nrf_client_t my_nrf;

  // initialise my_nrf
  nrf_driver_create_client(&my_nrf);

  // configure GPIO pins and SPI
  my_nrf.configure(&my_pins, my_baudrate);

  // not using default configuration (my_nrf.initialise(NULL)) 
  my_nrf.initialise(&my_config);

  // set to Standby-I Mode
  my_nrf.standby_mode();

  // DATA STRUCTURE TO SEND ---------------
  // payload sent to receiver data pipe 0
  typedef struct payload_accel_s 
  {
    uint16_t accel_x = 0;
    uint16_t accel_y = 0;
    uint16_t accel_z = 0;
  } payload_accel_t;
  // payload sent to receiver data pipe 1
  typedef struct payload_mag_s 
  {
    uint16_t vang_theta = 0;
    uint16_t vang_phi = 0;
    uint16_t vang_w = 0; 
  } payload_mag_t;
  // payload sent to receiver data pipe 2
  uint16_t magnetometro = 0; 

  // result of packet transmission
  fn_status_t success = 0;
  uint64_t time_sent = 0; // time packet was sent
  uint64_t time_reply = 0; // response time after packet sent

  while (1) {
    // These are the raw numbers from the chip, so will need tweaking to be really useful.
    mpu6050_read_raw(acceleration, gyro, &temp);
    cy273_read(magneto);
    
    // INIT DATA -----------------------------
    payload_accel_t datos = { .accel_x = acceleration[0], .accel_y = acceleration[1], .accel_z = acceleration[2]};
    payload_mag_t datos1 = { .vang_theta = gyro[0], .vang_phi = gyro[1], .vang_w = gyro[2]};
    magnetometro = 10;

    // send to receiver's DATA_PIPE_0 address
    my_nrf.tx_destination((uint8_t[]){0x37,0x37,0x37,0x37,0x37});
    // time packet was sent
    time_sent = to_us_since_boot(get_absolute_time()); // time sent
    // send packet to receiver's DATA_PIPE_0 address
    success = my_nrf.send_packet(&datos, sizeof(datos));
    // time auto-acknowledge was received
    time_reply = to_us_since_boot(get_absolute_time()); // response time
    if (success)
    {
      printf("\nPacket sent:- Response: %lluμS | Payload: Acc. X = %d, Y = %d, Z = %d\n", time_reply - time_sent, datos.accel_x, datos.accel_y, datos.accel_z);

    } else {

      printf("\nPacket not sent:- Receiver not available.\n");
    }
    sleep_ms(3000);

    // send to receiver's DATA_PIPE_1 address
    my_nrf.tx_destination((uint8_t[]){0xC7,0xC7,0xC7,0xC7,0xC7});
    // time packet was sent
    time_sent = to_us_since_boot(get_absolute_time()); // time sent
    // send packet to receiver's DATA_PIPE_1 address
    success = my_nrf.send_packet(&datos1, sizeof(datos1));    
    // time auto-acknowledge was received
    time_reply = to_us_since_boot(get_absolute_time()); // response time
    if (success)
    {
      printf("\nPacket sent:- Response: %lluμS | Payload: Gyro. X = %d, Y = %d, Z = %d\n", time_reply - time_sent, datos1.vang_theta, datos1.vang_phi, datos1.vang_w);
    } else {
      printf("\nPacket not sent:- Receiver not available.\n");
    }
    sleep_ms(3000);

    // send to receiver's DATA_PIPE_2 address
    my_nrf.tx_destination((uint8_t[]){0xC8,0xC7,0xC7,0xC7,0xC7});
    // time packet was sent
    time_sent = to_us_since_boot(get_absolute_time()); // time sent
    // send packet to receiver's DATA_PIPE_2 address
    success = my_nrf.send_packet(magnetometro, sizeof(magnetometro));    
    // time auto-acknowledge was received
    time_reply = to_us_since_boot(get_absolute_time()); // response time
    if (success)
    {
      printf("\nPacket sent:- Response: %lluμS | Payload: %d & %d\n",time_reply - time_sent, magnetometro);
    } else {
      printf("\nPacket not sent:- Receiver not available.\n");
    }
    sleep_ms(3000);
  }
  
}
