#ifndef ERROR_MANAGER_H
#define ERROR_MANAGER_H

typedef enum fn_status_e
{
  ERROR,
  PIN_MNGR_OK,
  SPI_MNGR_OK,
  NRF_MNGR_OK
} fn_status_t;

// return values for STATUS register functions
typedef enum fn_status_irq_e
{
  NONE_ASSERTED, // no IRQ bits asserted
  RX_DR_ASSERTED, // RX_DR bit asserted
  TX_DS_ASSERTED, // TX_DS bit asserted
  MAX_RT_ASSERTED // MAX_RT bit asserted
} fn_status_irq_t;

#endif // ERROR_MANAGER_H