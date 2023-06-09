#include "spi_manager.h"
#include "pico/time.h"


// see spi_manager.h
void spi_manager_init_spi(spi_inst_t *instance, uint32_t baudrate) {

  // initialise SPI instance at the specified baudrate (Hz)
  spi_init(instance, baudrate);

  /**
   * spi_set_format is also called within the pico-sdk spi_init function, with 
   * the same arguments as below. Function called here to illustrate settings.
   */
  spi_set_format(instance, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

  return;
} 


// see spi_manager.h
void spi_manager_deinit_spi(spi_inst_t *instance) {

  // deinitialise SPI instance
  spi_deinit(instance);

  return;
} 


// see spi_manager.h
fn_status_t spi_manager_transfer(spi_inst_t *instance, const uint8_t *tx_buffer, uint8_t *rx_buffer, size_t len) {

  sleep_us(2);
  uint8_t bytes = spi_write_read_blocking(instance, tx_buffer, rx_buffer, len);
  sleep_us(2);

  // check that bytes written/read match bytes in tx_buffer & rx_buffer
  fn_status_t status = (bytes == len) ? SPI_MNGR_OK : ERROR;

  return status;
}
