#include <stdint.h>
#include <stdio.h>
#include "nrf.h"
#include "define_config.h"
#include "parameter_config.h"

/* Phone side chip type
* 0: MTK chip, the SIM clock frequency is 3.25MHz 
* 1: QCM chip, the SIM clock frequency is 3.84MHz */
uint8_t PHONE_CHIP_TYPE = 1;

/* if log information is forward to UART 
* 0: no log information to UART 
* 1: log information to UART */
uint8_t UART_LOG = 1;

/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/
/* phone SIM interface ETU define, for ATR initial stage */
volatile uint32_t ETU_TICKS_PHONE_INITIAL;
/* phone SIM interface ETU define */
volatile uint32_t ETU_TICKS_PHONE;

#if (SENSOR == 1)
#include "nrf_delay.h"
void sensor_data(void);
#endif
/**
 * @brief Function for main application entry.
 */
/********************************************************************************/
int main(void)
{
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/
  /* UART initialization, for log information output */
  uart_initialization( );
	
  /* chip peripheral initialization */	
  device_peripheral_initialization( );  
    
  /* update user configuration setting */           
  update_user_configuration( );
    
  /* Set radio configuration parameters */
  radio_configure();

  /* RTC1 initialization for radio link monitoring */
  rtc1_radio_link_config( );

/*-----------------------------------------------------------------------------*/
#if (ARGET_CHIP == NRF51) 
  /* enable code read back protection */
  if((uint32_t)((NRF_UICR->RBPCONF & UICR_RBPCONF_PALL_Msk) >> UICR_RBPCONF_PALL_Pos) != UICR_RBPCONF_PALL_Enabled) 
  {
    flash_word_write((uint32_t *)&(NRF_UICR->RBPCONF), (NRF_UICR->RBPCONF & (~UICR_RBPCONF_PALL_Msk)));
    USIM_initialization( );
  }
#endif /* end of #if (ARGET_CHIP == NRF51)  */

#if (ARGET_CHIP == NRF52) 
  /* enable CPU registers and memory mapped address protection */
  if((uint32_t)NRF_UICR->APPROTECT != 0) 
  {
    flash_word_write((uint32_t *)&(NRF_UICR->APPROTECT), 0);
    USIM_initialization( );
  }
#endif /* end of #if (ARGET_CHIP == NRF52)  */
      
/*-----------------------------------------------------------------------------*/
#if (SENSOR == 1)
  /* generate random sensor data */
  while (1)
  {
    sensor_data( );
    nrf_delay_ms(5000);
  }
#endif
  /* go to sleep to wait for interrupt */
  while (1)
  {
    power_management( );
  }
  
  return(0);
}

