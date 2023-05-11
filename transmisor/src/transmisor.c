#include <stdio.h>

#include "nrf24_driver.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include <tusb.h> // TinyUSB tud_cdc_connected()

#include "gps.h"
#include "cy273.h"
#include "MPU6050_i2c.h"

// gps
#define uartTX 12 //16
#define uartRX 13 //17
int baudrate = 9600; // Velocidad de transmisión en baudios
int data_bits = 8;   // Longitud de datos en bits
int parity = UART_PARITY_NONE; // Paridad: none para sin paridad, odd para paridad impar, even para paridad par

// magnetometro
#define SDA_cy 20 //pin 26
#define SCL_cy 21 //pin 27

// imu
#define SDA_MPU 26 //pin 31
#define SCL_MPU 27 //pin 32

int main(void)
{
  // initialize all present standard stdio types
  stdio_init_all();

  gps_init(uartTX, uartRX, baudrate, data_bits, parity);
  cy273_init(SDA_cy, SCL_cy);
  mpu6050_init(SDA_MPU, SCL_MPU);

  int16_t acceleration[3], gyro[3], magneto[3];
  // wait until the CDC ACM (serial port emulation) is connected
  // while (!tud_cdc_connected()) 
  // {
  //   sleep_ms(10);
  // }

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
  uint8_t lat[14] = "";
  uint8_t lon[14] = "";
  uint8_t N[2] = "";
  uint8_t W[2] = "";
  uint8_t buf[256], payload_gps[40];
  uint8_t coor[29] = "" ;

  mpu6050_reset();
  cy273_reset();
  
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
  
  // INIT DATA -----------------------------
  // payload sent to receiver data pipe 2

  payload_mpu_t payload_mpu = { 
    .accel_x = 0, 
    .accel_y = 0, 
    .accel_z = 0, 
    .magne_x = 0, 
    .magne_y = 0, 
    .magne_z = 0, 
    .gyro_x = 0, 
    .gyro_y = 0, 
    .gyro_z = 0
    };

  // result of packet transmission
  fn_status_t success = 0;
  uint64_t time_sent = 0; // time packet was sent
  uint64_t time_reply = 0; // response time after packet sent

  uint64_t t_actual = 0;
  uint64_t t_anterior = 0;

  while (1) {
    t_actual = time_us_64();
    if(t_actual >= t_anterior + 1000*1000){
      uart_read_blocking(UART_ID, buf, sizeof(buf)-1);
      sleep_ms(200);
      uint8_t *gpgga_data = strstr(buf, "GPGGA");
      uint8_t *end_gpgga = strchr(gpgga_data, '$');
      *end_gpgga = '\0';
      if(gpgga_data != NULL){
        // printf("%s\n",gpgga_data);
        uint8_t delim[] = ",";
        uint8_t *token = strtok(gpgga_data, delim);
        if(token != NULL){
          uint8_t i = 1;
          // payload_gps[50] = "";
          memset(payload_gps, 0, sizeof(payload_gps));
          while(token != NULL){
              // printf("Token: %s\n", token);
              token = strtok(NULL, delim);
              
              if (i >= 2 && i <=5)
              {
                strcat(payload_gps,token);
                strcat(payload_gps,delim);
              }
              i += 1;
              
          }
          payload_gps[strlen(payload_gps) - 1] = '\0';
          //printf("%s\n", payload_gps);

          uint8_t lim[] = ",";
          uint8_t *token = strtok(payload_gps, lim);
          while(token != NULL) {
          // Asignar el valor del token a la variable correspondiente
            if(strcmp(lat, "") == 0) {
              strcpy(lat, token);
            } else if(strcmp(N, "") == 0) {
              strcpy(N, token);
            } else if(strcmp(lon, "") == 0) {
              strcpy(lon, token);
            } else if(strcmp(W, "") == 0) {
              strcpy(W, token);
            }
            // Obtener el siguiente token
            token = strtok(NULL, lim);
          }

          strcat(coor, lat);
          strcat(coor, ",");
          strcat(coor, lon);

        }
      }
      else {
          printf("No se encontro la cadena GPGGA.\n");
      }

      //--------------------------------------

      mpu6050_read(acceleration, gyro);
      cy273_read(magneto);

      payload_mpu_t payload_mpu =
      {
        .accel_x = acceleration[0],
        .accel_y = acceleration[1],
        .accel_z = acceleration[2],
        .magne_x = magneto[0], 
        .magne_y = magneto[2],
        .magne_z = magneto[1],
        .gyro_x = gyro[0],
        .gyro_y = gyro[1],
        .gyro_z = gyro[2],
      };

      
      // send to receiver's DATA_PIPE_0 address
      my_nrf.tx_destination((uint8_t[]){0x37,0x37,0x37,0x37,0x37});

      // time packet was sent
      time_sent = to_us_since_boot(get_absolute_time()); // time sent

      // send packet to receiver's DATA_PIPE_0 address
      success = my_nrf.send_packet(coor, sizeof(coor));

      // time auto-acknowledge was received
      time_reply = to_us_since_boot(get_absolute_time()); // response time

      if (success)
      {
        printf("\nPacket sent:- Response: %lluuS | Payload: %s\n", time_reply - time_sent, coor);
        memset(coor, 0, sizeof(coor));
      } else {

        printf("\nPacket not sent:- Receiver not available.\n");
        memset(coor, 0, sizeof(coor));
      }

      // send to receiver's DATA_PIPE_2 address
      my_nrf.tx_destination((uint8_t[]){0xC8,0xC7,0xC7,0xC7,0xC7});

      // time packet was sent
      time_sent = to_us_since_boot(get_absolute_time()); // time sent

      // send packet to receiver's DATA_PIPE_2 address
      success = my_nrf.send_packet(&payload_mpu, sizeof(payload_mpu));
      
      // time auto-acknowledge was received
      time_reply = to_us_since_boot(get_absolute_time()); // response time

      if (success)
      {
        printf("\nPacket sent:- Response: %lluuS | Payload: %d, %d, %d, %d, %d, %d, %d, %d, %d\n",time_reply - time_sent, payload_mpu.accel_x, payload_mpu.accel_y, payload_mpu.accel_z, 
        payload_mpu.gyro_x, payload_mpu.gyro_y, payload_mpu.gyro_z, payload_mpu.magne_x, payload_mpu.magne_y, payload_mpu.magne_z);

      } else {

        printf("\nPacket not sent:- Receiver not available.\n");
      }
    }
  }
}
 