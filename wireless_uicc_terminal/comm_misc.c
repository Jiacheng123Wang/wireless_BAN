#include "define_config.h"
#include "comm_misc.h"
#include "comm_UICC_terminal_interface.h"

extern volatile uint8_t WIRELESS_SIM_ON;
extern uint8_t USER_PASSWORD[KEY_LENGTH];
extern uint8_t SYSTEM_KEY[KEY_LENGTH];
extern uint8_t WATCH_PHONE_NAME[16];
extern uint8_t MASK_RECEIVE_CALL;
extern uint8_t PHONE_ADD_LOGICAL;
extern uint8_t AUTH_CHALL_DATA[KEY_LENGTH];
extern uint8_t UART_LOG;

/********************************************************************************/
uint32_t start_oscillator_16m(void)
/*--------------------------------------------------------------------------------
| Function for initialization 16M oscillators
|
--------------------------------------------------------------------------------*/
{
  uint32_t initial_timer;
  uint32_t osc_time_out_ms = 2;
    
  /* Start 16 MHz crystal oscillator */
  NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_HFCLKSTART = 1;

  /* get the initial real time counter */
  initial_timer = NRF_RTC0->COUNTER;
  /* Wait for the external oscillator to start up */
  while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) 
  {
    /*  oscillator start up time out */	
    if (((NRF_RTC0->COUNTER - initial_timer) > osc_time_out_ms) || (NRF_RTC0->COUNTER < initial_timer))
    {		
#if (IF_PRINT_LOG)
			if (UART_LOG)
			{
        printf("..................16M oscillator start time out..................\r\n" );
			}
#endif  
#if (PRINT_DELAY_MS)
      nrf_delay_ms(PRINT_DELAY_MS);
#endif
      return (1);
    }
  }
  
  return(0);
}

/********************************************************************************/
__INLINE void stop_oscillator_16m(void)
/*--------------------------------------------------------------------------------
| Function for initialization 16M oscillators
|
--------------------------------------------------------------------------------*/
{
  /* Stop 16 MHz crystal oscillator */
  NRF_CLOCK->TASKS_HFCLKSTOP = 1;
}

/********************************************************************************/
void flash_page_erase(uint32_t *page_address)
/*--------------------------------------------------------------------------------
| Function for erasing a page in flash
| page_address: Address of the first word in the page to be erased.
|
--------------------------------------------------------------------------------*/
{
  // Turn on flash erase enable and wait until the NVMC is ready:
  NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos);
  while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
  {
    // Do nothing.
  }
    
  // Erase page:
  NRF_NVMC->ERASEPAGE = (uint32_t)page_address;
    
  while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
  {
    // Do nothing.
  }
    
  // Turn off flash erase enable and wait until the NVMC is ready:
  NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
    
  while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
  {
    // Do nothing.
  }
}

/********************************************************************************/
void flash_word_write(uint32_t *address, uint32_t value)
/*--------------------------------------------------------------------------------
| Function for filling a page in flash with a value
| address: Address of the first word in the page to be filled
| value: Value to be written to flash
|
--------------------------------------------------------------------------------*/
{
  // Turn on flash write enable and wait until the NVMC is ready:
  NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos);
  
  while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
  {
      // Do nothing.
  }

  *address = value;

  while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
  {
      // Do nothing.
  }

  // Turn off flash write enable and wait until the NVMC is ready:
  NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);

  while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
  {
      // Do nothing.
  }
}

/********************************************************************************/
void flash_byte_string_update(uint32_t *address_swap_page, uint32_t *address_update_page, 
     uint32_t address_offset_in_word, uint32_t update_length_in_byte, uint8_t *byte_string)
/*--------------------------------------------------------------------------------
| update_length_in_byte should be a multiple of 4, i.e., integer number word 
|
--------------------------------------------------------------------------------*/
{
  uint32_t i;
  uint32_t update_length_in_word = update_length_in_byte / 4;
  uint32_t tmp_word = 0;
  uint32_t word_to_write = 0;
  uint32_t page_size_in_word = FLASH_PAGE_SIZE / 4;
  

  /* save the data in the page to the swap page */
  flash_page_erase(address_swap_page);
  for (i=0; i<page_size_in_word; i++)
  {   
    flash_word_write(address_swap_page + i, *(address_update_page + i));    
  }
  
  flash_page_erase(address_update_page); 
  
  /* update the word data */  	
  for (i=0; i<address_offset_in_word; i++)
  {   
    flash_word_write(address_update_page + i, *(address_swap_page  + i));    
  }     
  for (i=0; i<update_length_in_word; i++)
  {    
    word_to_write = (*(byte_string + i * 4 + 0));
    tmp_word = *(byte_string + i * 4 + 1);
    word_to_write |= (tmp_word << 8);
    tmp_word = *(byte_string + i * 4 + 2);
    word_to_write |= (tmp_word << 16);
    tmp_word = *(byte_string + i * 4 + 3);
    word_to_write |= (tmp_word << 24);
  
    flash_word_write(address_update_page + address_offset_in_word + i, word_to_write); 
  }  
  for (i=address_offset_in_word + update_length_in_word; i<page_size_in_word; i++)
  {   
    flash_word_write(address_update_page + i, *(address_swap_page  + i));    
  }  
}

/********************************************************************************/
uint8_t byte_string_comparison(uint8_t byte_length, uint8_t *byte_1, uint8_t *byte_2)
/*--------------------------------------------------------------------------------
| 1: the 2 trsing are different
| 0: the 2 string are the same 
|
--------------------------------------------------------------------------------*/
{
  uint8_t i;
	
  for (i=0; i<byte_length; i++)
  {
    if (*(byte_1 + i) != *(byte_2 + i))
    {
      return(1);
    }
  }

  return(0);
}

/********************************************************************************/
uint8_t random_vector_generate(uint8_t *p_buff, uint8_t size)
/*--------------------------------------------------------------------------------
| Function for getting vector of random numbers
| p_buff: Pointer to unit8_t buffer for storing the bytes
| length: Number of bytes to take from pool and place in p_buff
|
--------------------------------------------------------------------------------*/
{
  uint8_t i;
  uint32_t initial_timer;
  uint32_t rnd_time_out_ms = RANDOM_BYTE_NUMBER_TIME_OUT_MS;
  
  NRF_RNG->CONFIG = 0;
  NRF_RNG->TASKS_START = 1;

  /* get the initial real time counter */
  initial_timer = NRF_RTC0->COUNTER;
  
  for (i=0; i<size; i++)
  {
    NRF_RNG->EVENTS_VALRDY = 0;
    while(NRF_RNG->EVENTS_VALRDY == 0)
    {
       /*  random number generation ready time out */  
      if (((NRF_RTC0->COUNTER - initial_timer) > rnd_time_out_ms) || (NRF_RTC0->COUNTER < initial_timer))
      {    
#if (IF_PRINT_LOG)
				if (UART_LOG)
				{
          printf(".................. Random number generation time out, i = %d..................\r\n", i );
				}
#endif	  
        return(1);
      }
    }  
    *(p_buff + i) = (uint8_t)NRF_RNG->VALUE; 
  }
  
  NRF_RNG->TASKS_STOP = 1;
  
  return(0);
}

/********************************************************************************/
uint8_t password_check(uint8_t password_length, uint8_t *password_saved, uint8_t *password_input)
/*--------------------------------------------------------------------------------
| 1: password check error
| 0: password check correct
| 
--------------------------------------------------------------------------------*/
{
  uint8_t i;
  uint8_t flag_auth = *(uint8_t *)FLAG_PASSWORD_FLASH_ADDR;
	
  if (flag_auth)
  {	
    for (i=0; i<password_length; i++)
    {
      if (*(password_saved + i) != *(password_input + i))
      {
        return(1);
      }
    }
    return(0);
  }
  else
  {	
    return(0);
  }
}

/********************************************************************************/
void power_management(void)
/*--------------------------------------------------------------------------------
| simple power management, go to sleep to wait for interrupt event
|
--------------------------------------------------------------------------------*/
{
  while(1)
  {
  /* Wait for interrupt */
    __WFI();
  }
}

/********************************************************************************/
void USIM_initialization(void)
/*--------------------------------------------------------------------------------
| USIM EF data initialization
|
--------------------------------------------------------------------------------*/
{
  uint8_t EF_data_USIM[248] = {247, 0x98,  0x68,  0x10,  0x51,  0x18,  0x1,  
             0x91,  0x37,  0x78,  0x76,  0xff,  0xff,  0xff,  
             0xff,  0x8,  0x49,  0x6,  0x10,  0x40,  0x6,  0x3,  0x21,  0x87,  
             0x2,  0x31,  0x15,  0x26,  0xeb,  0xb7,  0x69,  
             0xf3,  0xc8,  0xcf,  0xdf,  0x44,  0xe0,  0x36,  0x45,  0xef,  
             0x5e,  0x96,  0xf8,  0xc2,  0x75,  0xcb,  0x32,  
             0x55,  0xd5,  0xb7,  0x39,  0x76,  0xc3,  0x48,  0x67,  0x7c,  
             0x6b,  0x2,  0x15,  0x68,  0x8,  0x0,  0x77,  
             0xec,  0x49,  0xae,  0xe1,  0x71,  0xda,  0xd1,  0x9c,  0xce,  
             0x78,  0xb8,  0x28,  0xcb,  0xa8,  0x76,  0x6f,  
             0xb3,  0x63,  0x0,  0xf,  0xf0,  0x61,  0xf2,  0x76,  0x32,  0xf0,  
             0x32,  0xff,  0x0,  0x40,  0x9c,  0x1a,  
             0x8f,  0xb2,  0x64,  0xf0,  0x10,  0x10,  0xd7,  0x0,  0x0,  0xe4,  
             0x9,  0xeb,  0xc4,  0x9d,  0xf,  0x39,  0x64,  
             0xf0,  0x10,  0xa5,  0x4d,  0x2,  0x0,  0x64,  0xf0,  0x0,  0x64,  
             0xf0,  0x20,  0x64,  0xf0,  0x40,  0x64,  
             0xf0,  0x70,  0x0,  0x0,  0x0,  0x2,  0xff,  0xff,  0xff,  0xf0,  
             0x0,  0x2,  0xf0,  0x0,  0x2,  0xa0,  0x2e,  
             0x80,  0x2,  0x23,  0xcd,  0x81,  0x28,  0x23,  0xbe,  0x23,  0xbf,  
             0x23,  0xc1,  0x23,  0xc2,  0x23,  0xc4,  
             0x23,  0xc6,  0x23,  0xc7,  0x23,  0xc8,  0x23,  0xc9,  0x23,  0xca,  
             0x23,  0xce,  0x23,  0xcf,  0x23,  0xec,  
             0x12,  0xb1,  0x12,  0xb3,  0x12,  0xb4,  0x12,  0xb7,  0x12,  0xb9,  
             0x12,  0xbc,  0x0,  0xdf,  0xb,  0xd6,  
             0xbd,  0x2,  0x79,  0x35,  0x28,  0x2,  0xd3,  0x22,  0x1b,  0x55,  
             0xf2,  0xa3,  0xa2,  0x24,  0x2,  0xff,  
             0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  
             0xff,  0xfd,  0xff,  0xff,  0xff,  0xff,  
             0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0x8,  0x91,  
             0x68,  0x31,  0x10,  0x10,  0x21,  0x5,  
             0xf0,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff};
  uint8_t flag_bytes_on[4] = {1, 0, 0, 0};
  uint8_t user_password[16] = {'0', '0', '0', '0', '0', '0', 0, 0, 0, 0, 0, 0, 
             0, 0, 0, 0};
  
/* wireless SIM  on */        
  flash_byte_string_update((uint32_t *)SWAP_FLASH_ADDR, (uint32_t *)USER_CONFIG_FLASH_ADDR, 
      (FLAG_WIRELESS_SIM_FLASH_ADDR - USER_CONFIG_FLASH_ADDR) / 4, 4, flag_bytes_on);  
/* USIM file data initialization*/  
  flash_byte_string_update((uint32_t *)SWAP_FLASH_ADDR, (uint32_t *)(USIM_EF_FLASH_ADDR), 0, 248, EF_data_USIM);
  
/* initial user password and system key */
/* random initial password for phone side, '000000' for watch side */        
  // random_vector_generate(user_password, 16);
/* password initialization */        
  flash_byte_string_update((uint32_t *)SWAP_FLASH_ADDR, (uint32_t *)USER_CONFIG_FLASH_ADDR, 
      (USER_PASSWORD_FLASH_ADDR - USER_CONFIG_FLASH_ADDR) / 4, 16, user_password); 
/* random initial system key for phone side, '000000' for watch side */        
  // random_vector_generate(user_password, 16);
/* system key initialization */        
  flash_byte_string_update((uint32_t *)SWAP_FLASH_ADDR, (uint32_t *)USER_CONFIG_FLASH_ADDR, 
	  (SYSTEM_KEY_FLASH_ADDR - USER_CONFIG_FLASH_ADDR) / 4, 16, user_password); 
	  
/* phone USIM logical address */
  flash_byte_string_update((uint32_t *)SWAP_FLASH_ADDR, (uint32_t *)USER_CONFIG_FLASH_ADDR, 
      (PHONE_ADD_FLASH_ADDR - USER_CONFIG_FLASH_ADDR) / 4, 4, flag_bytes_on);
}

/********************************************************************************/
void start_WDT(uint32_t time_out_s)
/*--------------------------------------------------------------------------------
| watch dog timer initialization, the time resolution is 1/4 second = 250ms
|
--------------------------------------------------------------------------------*/
{
  NRF_WDT->CRV = time_out_s * 8192 - 1;
  NRF_WDT->CONFIG = 1;
  NRF_WDT->RREN = 1;
  NRF_WDT->TASKS_START = 1;
}

/********************************************************************************/
void update_user_configuration(void)
/*--------------------------------------------------------------------------------
| read the user configured parameters stored in flash
|
--------------------------------------------------------------------------------*/
{
  uint8_t i;
  
  for (i=0; i<KEY_LENGTH; i++)
  {
    USER_PASSWORD[i] = *((uint8_t *)USER_PASSWORD_FLASH_ADDR + i);
    SYSTEM_KEY[i] = *((uint8_t *)SYSTEM_KEY_FLASH_ADDR + i);
    AUTH_CHALL_DATA[i] = USER_PASSWORD[i];
  }
  for (i=0; i<16; i++)
  {
    WATCH_PHONE_NAME[i] = *((uint8_t *)NAME_FLASH_ADDR + i);
  }
  PHONE_ADD_LOGICAL = ((*((uint8_t *)PHONE_ADD_FLASH_ADDR)) & 7);

  WIRELESS_SIM_ON = (*((uint8_t *)FLAG_WIRELESS_SIM_FLASH_ADDR) & 1);

}

#if (IF_PRINT_LOG)		
/********************************************************************************/
void uart_initialization(void)
/*--------------------------------------------------------------------------------
| UART initialization
|
--------------------------------------------------------------------------------*/
{
  simple_uart_config(RTS_PIN_NUMBER, TX_PIN_NUMBER, CTS_PIN_NUMBER, RX_PIN_NUMBER, 0);
}
#endif

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
uint8_t simple_uart_get(void)
{
  while (NRF_UART0->EVENTS_RXDRDY != 1)
  {
    // Wait for RXD data to be received
  }
  
  NRF_UART0->EVENTS_RXDRDY = 0;
  return (uint8_t)NRF_UART0->RXD;
}

/********************************************************************************/
uint32_t simple_uart_get_with_timeout(uint32_t timeout_ms, uint8_t *rx_data)
{
  uint32_t initial_timer;

  /* get the initial real time counter */
  initial_timer = NRF_RTC0->COUNTER;
  
  while (NRF_UART0->EVENTS_RXDRDY != 1)
  {
    if ((NRF_RTC0->COUNTER - initial_timer) > timeout_ms)
    {
	  return (1);
    }
    if (NRF_RTC0->COUNTER < initial_timer)
    {
	  initial_timer = NRF_RTC0->COUNTER;
    }
    /* re-load watch dog request register */
    NRF_WDT->RR[0] = 0x6E524635;
	
  }  // Wait for RXD data to be received

  // clear the event and set rx_data with received byte
  NRF_UART0->EVENTS_RXDRDY = 0;
  *rx_data = (uint8_t)NRF_UART0->RXD;

  return (0);
}

/********************************************************************************/
void simple_uart_put(uint8_t cr)
{
  NRF_UART0->TXD = (uint8_t)cr;

  while (NRF_UART0->EVENTS_TXDRDY!=1)
  {
    // Wait for TXD data to be sent
  }

  NRF_UART0->EVENTS_TXDRDY=0;
}

/********************************************************************************/
void simple_uart_config(  uint8_t rts_pin_number,
                          uint8_t txd_pin_number,
                          uint8_t cts_pin_number,
                          uint8_t rxd_pin_number,
                          bool    hwfc)
{
/* @snippet [Configure UART RX and TX pin] */
  nrf_gpio_cfg_output_H0H1(txd_pin_number);
  nrf_gpio_cfg_input(rxd_pin_number, NRF_GPIO_PIN_NOPULL);  

  NRF_UART0->PSELTXD = txd_pin_number;
  NRF_UART0->PSELRXD = rxd_pin_number;
             
/* snippet [Configure UART RX and TX pin] */   
  if (hwfc)
  {
    nrf_gpio_cfg_output(rts_pin_number);
    nrf_gpio_cfg_input(cts_pin_number, NRF_GPIO_PIN_NOPULL);
    NRF_UART0->PSELCTS = cts_pin_number;
    NRF_UART0->PSELRTS = rts_pin_number;
    NRF_UART0->CONFIG  = (UART_CONFIG_HWFC_Enabled << UART_CONFIG_HWFC_Pos);
  }

  NRF_UART0->BAUDRATE         = (UART_BAUDRATE_BAUDRATE_Baud115200 << UART_BAUDRATE_BAUDRATE_Pos);
  NRF_UART0->ENABLE           = (UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos);
  NRF_UART0->TASKS_STARTTX    = 1;
  NRF_UART0->TASKS_STARTRX    = 1;
  NRF_UART0->EVENTS_RXDRDY    = 0;
}

/********************************************************************************/
void simple_uart_putstring(const uint8_t *str)
{
  uint8_t i = 0;
  uint8_t ch = str[i++];
  
  while (ch != '\0')
  {
    simple_uart_put(ch);
    ch = str[i++];
  }
}

/********************************************************************************/
void simple_uart_getstring(uint8_t *str, uint8_t *str_length)
{
  uint_fast8_t i = 0;
  uint8_t ch;

  while (1)
  {
    ch = simple_uart_get(); 
    if (ch != '\n')
      str[i++] = ch;
    else
    {
      str[i++] = '\0';
	  *str_length = i;
      return;
    }
  }
}

/********************************************************************************/
void simple_uart_getstring_with_timeout(uint32_t timeout_ms, uint8_t max_length, uint8_t *str, uint8_t *str_length)
{
  uint_fast8_t i = 0;
  uint8_t ch;  
  
  while (1)
  {
	if ((!simple_uart_get_with_timeout(timeout_ms, &ch)) && (i < max_length))
	{
      if (ch != '\n')
        str[i++] = ch;
      else
      {
        str[i++] = '\n';
	    *str_length = i;
        return;
      }
    }
	else
	{
	  *str_length = i;
      return;		
	}
  }
}

/********************************************************************************/
int _write(int fd, const char *str, int len)
{
	int i;
	
	for (i = 0; i < len; i++)
	{
		simple_uart_put(*str++);
	}
	return len;
}

/********************************************************************************/
int _read(int file, char *p_char, int len)
{
  int ret_len = len;
  uint8_t input;

  while (len--)
  {
    input = simple_uart_get();
    while (input)
    {
        // No implementation needed.
    }
    *p_char++ = input;
  }

  return ret_len;
}




