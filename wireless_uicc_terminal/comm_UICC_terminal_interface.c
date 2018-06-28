#include "comm_UICC_terminal_interface.h"
#include "comm_misc.h"

extern uint8_t UART_LOG;
static volatile uint32_t ETU_TICK_FLAG = 1;

/********************************************************************************/
void timer0_initialization(void)
{
  NVIC_EnableIRQ(TIMER0_IRQn);
  NVIC_SetPriority(TIMER0_IRQn, TIMER0_IRQ_PRIORITY);  
  
  NRF_TIMER0->MODE = TIMER_MODE_MODE_Timer;
  NRF_TIMER0->PRESCALER = 0;
  NRF_TIMER0->BITMODE = TIMER_BITMODE_BITMODE_32Bit << TIMER_BITMODE_BITMODE_Pos;
  NRF_TIMER0->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;
  NRF_TIMER0->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;
  NRF_TIMER0->TASKS_CLEAR = 1;
  NRF_TIMER0->CC[0] = 16000000;
  NRF_TIMER0->TASKS_STOP = 1;

}

/********************************************************************************/
void TIMER0_IRQHandler()
/*--------------------------------------------------------------------------------
| Handler for timer0 events, mark the ETU tick for UICC read/write
|
--------------------------------------------------------------------------------*/
{
  NRF_TIMER0->EVENTS_COMPARE[0] = 0; 
#if (ARGET_CHIP == NRF52)
  (void)NRF_TIMER0->EVENTS_COMPARE[0]; 
#endif 
   
  ETU_TICK_FLAG = 1 - ETU_TICK_FLAG;
}

/********************************************************************************/
void read_byte(uint8_t *info_byte, uint8_t *check_bit, uint32_t etu_ticks, uint32_t pin_number)
/*--------------------------------------------------------------------------------
|read a byte and its check bit from the pin_number
|The initial delay has some dajustment to adapt the ETU error for each bit
--------------------------------------------------------------------------------*/
{
  volatile uint32_t etu_record; 
  uint8_t read_bit1; 
  uint8_t read_bit2; 
  uint8_t read_bit3; 
  uint8_t read_bit4; 
  uint8_t read_bit5; 
  uint8_t bit;

  /* initialization the read byte */
  *info_byte = 0;
   
  /* clear ETU timer tick */
  NRF_TIMER0->TASKS_CLEAR = 1;
  NRF_TIMER0->EVENTS_COMPARE[0] = 0; 
#if (ARGET_CHIP == NRF52)
  (void)NRF_TIMER0->EVENTS_COMPARE[0]; 
#endif 

  /* waiting time for the initial bit */
  etu_record = ETU_TICK_FLAG;
  while (etu_record == ETU_TICK_FLAG)
  {
  }  
	
  /*** read bit 0 ***********************/
  etu_record = ETU_TICK_FLAG;
  read_bit1 = nrf_gpio_pin_read(pin_number);
  read_bit2 = nrf_gpio_pin_read(pin_number);
  read_bit3 = nrf_gpio_pin_read(pin_number);
  read_bit4 = nrf_gpio_pin_read(pin_number);
  read_bit5 = nrf_gpio_pin_read(pin_number);
  bit = (read_bit1 + read_bit2 + read_bit3 + read_bit4 + read_bit5) > 2 ? 1 : 0;
  *info_byte |= (bit << 0);
  while (etu_record == ETU_TICK_FLAG)
  {
  }

  /*** read bit 1 ***********************/
  etu_record = ETU_TICK_FLAG;
  read_bit1 = nrf_gpio_pin_read(pin_number);
  read_bit2 = nrf_gpio_pin_read(pin_number);
  read_bit3 = nrf_gpio_pin_read(pin_number);
  read_bit4 = nrf_gpio_pin_read(pin_number);
  read_bit5 = nrf_gpio_pin_read(pin_number);
  bit = (read_bit1 + read_bit2 + read_bit3 + read_bit4 + read_bit5) > 2 ? 1 : 0;
  *info_byte |= (bit << 1);
  while (etu_record == ETU_TICK_FLAG)
  {
  }

  /*** read bit 2 ***********************/
  etu_record = ETU_TICK_FLAG;
  read_bit1 = nrf_gpio_pin_read(pin_number);
  read_bit2 = nrf_gpio_pin_read(pin_number);
  read_bit3 = nrf_gpio_pin_read(pin_number);
  read_bit4 = nrf_gpio_pin_read(pin_number);
  read_bit5 = nrf_gpio_pin_read(pin_number);
  bit = (read_bit1 + read_bit2 + read_bit3 + read_bit4 + read_bit5) > 2 ? 1 : 0;
  *info_byte |= (bit << 2);
  while (etu_record == ETU_TICK_FLAG)
  {
  }

  /*** read bit 3 ***********************/
  etu_record = ETU_TICK_FLAG;
  read_bit1 = nrf_gpio_pin_read(pin_number);
  read_bit2 = nrf_gpio_pin_read(pin_number);
  read_bit3 = nrf_gpio_pin_read(pin_number);
  read_bit4 = nrf_gpio_pin_read(pin_number);
  read_bit5 = nrf_gpio_pin_read(pin_number);
  bit = (read_bit1 + read_bit2 + read_bit3 + read_bit4 + read_bit5) > 2 ? 1 : 0;
  *info_byte |= (bit << 3);
  while (etu_record == ETU_TICK_FLAG)
  {
  }

  /*** read bit 4 ***********************/
  etu_record = ETU_TICK_FLAG;
  read_bit1 = nrf_gpio_pin_read(pin_number);
  read_bit2 = nrf_gpio_pin_read(pin_number);
  read_bit3 = nrf_gpio_pin_read(pin_number);
  read_bit4 = nrf_gpio_pin_read(pin_number);
  read_bit5 = nrf_gpio_pin_read(pin_number);
  bit = (read_bit1 + read_bit2 + read_bit3 + read_bit4 + read_bit5) > 2 ? 1 : 0;
  *info_byte |= (bit << 4);
  while (etu_record == ETU_TICK_FLAG)
  {
  }

  /*** read bit 5 ***********************/
  etu_record = ETU_TICK_FLAG;
  read_bit1 = nrf_gpio_pin_read(pin_number);
  read_bit2 = nrf_gpio_pin_read(pin_number);
  read_bit3 = nrf_gpio_pin_read(pin_number);
  read_bit4 = nrf_gpio_pin_read(pin_number);
  read_bit5 = nrf_gpio_pin_read(pin_number);
  bit = (read_bit1 + read_bit2 + read_bit3 + read_bit4 + read_bit5) > 2 ? 1 : 0;
  *info_byte |= (bit << 5);
  while (etu_record == ETU_TICK_FLAG)
  {
  }

  /*** read bit 6 ***********************/
  etu_record = ETU_TICK_FLAG;
  read_bit1 = nrf_gpio_pin_read(pin_number);
  read_bit2 = nrf_gpio_pin_read(pin_number);
  read_bit3 = nrf_gpio_pin_read(pin_number);
  read_bit4 = nrf_gpio_pin_read(pin_number);
  read_bit5 = nrf_gpio_pin_read(pin_number);
  bit = (read_bit1 + read_bit2 + read_bit3 + read_bit4 + read_bit5) > 2 ? 1 : 0;
  *info_byte |= (bit << 6);
  while (etu_record == ETU_TICK_FLAG)
  {
  }

  /*** read bit 7 ***********************/
  etu_record = ETU_TICK_FLAG;
  read_bit1 = nrf_gpio_pin_read(pin_number);
  read_bit2 = nrf_gpio_pin_read(pin_number);
  read_bit3 = nrf_gpio_pin_read(pin_number);
  read_bit4 = nrf_gpio_pin_read(pin_number);
  read_bit5 = nrf_gpio_pin_read(pin_number);
  bit = (read_bit1 + read_bit2 + read_bit3 + read_bit4 + read_bit5) > 2 ? 1 : 0;
  *info_byte |= (bit << 7);
  while (etu_record == ETU_TICK_FLAG)
  {
  }
  
  /*** read check bit ***********************/
  etu_record = ETU_TICK_FLAG;
  read_bit1 = nrf_gpio_pin_read(pin_number);
  read_bit2 = nrf_gpio_pin_read(pin_number);
  read_bit3 = nrf_gpio_pin_read(pin_number);
  read_bit4 = nrf_gpio_pin_read(pin_number);
  read_bit5 = nrf_gpio_pin_read(pin_number);
  *check_bit = (read_bit1 + read_bit2 + read_bit3 + read_bit4 + read_bit5) > 2 ? 1 : 0;
  while (etu_record == ETU_TICK_FLAG)
  {
  }
    
  /* wait for the guard time */
  etu_record = ETU_TICK_FLAG;
  while (etu_record == ETU_TICK_FLAG)
  {
  }
}

/********************************************************************************/
void write_byte(uint8_t word_byte, uint8_t parity_bit, uint32_t etu_ticks, uint32_t pin_number)
/*--------------------------------------------------------------------------------
|write a byte and its check bit to the pin_number
|
--------------------------------------------------------------------------------*/
{
  volatile uint32_t etu_record = 0; 
  
  /* clear ETU timer tick */
  NRF_TIMER0->TASKS_CLEAR = 1; 
  NRF_TIMER0->EVENTS_COMPARE[0] = 0; 
#if (ARGET_CHIP == NRF52)
  (void)NRF_TIMER0->EVENTS_COMPARE[0]; 
#endif 

  /* initial bit */	
  etu_record = ETU_TICK_FLAG;
  nrf_gpio_pin_write(pin_number, 0);
  nrf_gpio_pin_write(pin_number, 0);
  nrf_gpio_pin_write(pin_number, 0);
  while (etu_record == ETU_TICK_FLAG)
  {
  }
	
  /* bit 0 */
  etu_record = ETU_TICK_FLAG;
  nrf_gpio_pin_write(pin_number, (word_byte >> 0) & 0x01);
  nrf_gpio_pin_write(pin_number, (word_byte >> 0) & 0x01);
  nrf_gpio_pin_write(pin_number, (word_byte >> 0) & 0x01);
  while (etu_record == ETU_TICK_FLAG)
  {
  }  	
  /* bit 1 */
  etu_record = ETU_TICK_FLAG;
  nrf_gpio_pin_write(pin_number, (word_byte >> 1) & 0x01);
  nrf_gpio_pin_write(pin_number, (word_byte >> 1) & 0x01);
  nrf_gpio_pin_write(pin_number, (word_byte >> 1) & 0x01);
  while (etu_record == ETU_TICK_FLAG)
  {
  }
  /* bit 2 */
  etu_record = ETU_TICK_FLAG;
  nrf_gpio_pin_write(pin_number, (word_byte >> 2) & 0x01);
  nrf_gpio_pin_write(pin_number, (word_byte >> 2) & 0x01);
  nrf_gpio_pin_write(pin_number, (word_byte >> 2) & 0x01);
  while (etu_record == ETU_TICK_FLAG)
  {
  }
  /* bit 3 */
  etu_record = ETU_TICK_FLAG;
  nrf_gpio_pin_write(pin_number, (word_byte >> 3) & 0x01);
  nrf_gpio_pin_write(pin_number, (word_byte >> 3) & 0x01);
  nrf_gpio_pin_write(pin_number, (word_byte >> 3) & 0x01);
  while (etu_record == ETU_TICK_FLAG)
  {
  }
  /* bit 4 */
  etu_record = ETU_TICK_FLAG;
  nrf_gpio_pin_write(pin_number, (word_byte >> 4) & 0x01);
  nrf_gpio_pin_write(pin_number, (word_byte >> 4) & 0x01);
  nrf_gpio_pin_write(pin_number, (word_byte >> 4) & 0x01);
  while (etu_record == ETU_TICK_FLAG)
  {
  }
  /* bit 5 */
  etu_record = ETU_TICK_FLAG;
  nrf_gpio_pin_write(pin_number, (word_byte >> 5) & 0x01);
  nrf_gpio_pin_write(pin_number, (word_byte >> 5) & 0x01);
  nrf_gpio_pin_write(pin_number, (word_byte >> 5) & 0x01);
  while (etu_record == ETU_TICK_FLAG)
  {
  }
  /* bit 6 */
  etu_record = ETU_TICK_FLAG;
  nrf_gpio_pin_write(pin_number, (word_byte >> 6) & 0x01);
  nrf_gpio_pin_write(pin_number, (word_byte >> 6) & 0x01);
  nrf_gpio_pin_write(pin_number, (word_byte >> 6) & 0x01);
  while (etu_record == ETU_TICK_FLAG)
  {
  }
  /* bit 7 */
  etu_record = ETU_TICK_FLAG;
  nrf_gpio_pin_write(pin_number, (word_byte >> 7) & 0x01);
  nrf_gpio_pin_write(pin_number, (word_byte >> 7) & 0x01);
  nrf_gpio_pin_write(pin_number, (word_byte >> 7) & 0x01);
  while (etu_record == ETU_TICK_FLAG)
  {
  }
	
  /* parity bit */
  etu_record = ETU_TICK_FLAG;
  nrf_gpio_pin_write(pin_number, parity_bit);
  nrf_gpio_pin_write(pin_number, parity_bit);
  nrf_gpio_pin_write(pin_number, parity_bit);
  while (etu_record == ETU_TICK_FLAG)
  {
  }
	
  /* guard time, 2 ETU */	
  etu_record = ETU_TICK_FLAG;
  nrf_gpio_pin_write(pin_number, 1);
  nrf_gpio_pin_write(pin_number, 1);
  nrf_gpio_pin_write(pin_number, 1);
  while (etu_record == ETU_TICK_FLAG)
  {
  }
  etu_record = ETU_TICK_FLAG;
  while (etu_record == ETU_TICK_FLAG)
  {
  } 
  /* 1 additional ETU for guard time */     
  if (pin_number == PIN_DATA_PHONE)
  {
    etu_record = ETU_TICK_FLAG;
    while (etu_record == ETU_TICK_FLAG)
    {
    } 
  }     
}

/********************************************************************************/
void printf_log_tx(uint32_t bytes_size, uint8_t *bytes_infor)
/*--------------------------------------------------------------------------------
| bytes stream information print at TX side 
|
--------------------------------------------------------------------------------*/
{
  uint32_t i;
  uint32_t tmp_size;
	
  tmp_size = (bytes_size < MAX_PRINTF_SIZE) ? bytes_size : MAX_PRINTF_SIZE; 	

  printf("Transmitted_length=%ld: ", bytes_size);
  for (i=0; i<tmp_size; i++)
  {
    printf("%x,  ", *(bytes_infor + i));
  }	
  printf("\r\n");
}

/********************************************************************************/
void printf_log_rx(uint32_t bytes_size, uint8_t *bytes_infor)
/*--------------------------------------------------------------------------------
| bytes stream information print at RX side 
|
--------------------------------------------------------------------------------*/
{
  uint32_t i;
  uint32_t tmp_size;
	
  tmp_size = (bytes_size < MAX_PRINTF_SIZE) ? bytes_size : MAX_PRINTF_SIZE; 

  printf("Received_length=%ld: ", bytes_size);
  for (i=0; i<tmp_size; i++)
  {
    printf("%x,  ", *(bytes_infor + i));	
  }
  printf("\r\n");
}

/********************************************************************************/
void get_parity_byte(uint32_t length_byte, uint8_t *byte_in, uint8_t *byte_parity)
/*--------------------------------------------------------------------------------
| get the parity byte of bytes vector 
|
--------------------------------------------------------------------------------*/
{
  uint32_t i;

  *byte_parity = *byte_in;
  for (i=1; i<length_byte; i++)
  {
    *byte_parity ^= *(byte_in + i); 
  }
}
	
/********************************************************************************/
void get_parity_bit(uint8_t byte_in, uint8_t *bit_parity)
/*--------------------------------------------------------------------------------
| get the parity bit of a byte 
|
--------------------------------------------------------------------------------*/
{
  *bit_parity = ((byte_in >> 0) & 0x01) ^ ((byte_in >> 1) & 0x01);
  *bit_parity ^= ((byte_in >> 2) & 0x01);
  *bit_parity ^= ((byte_in >> 3) & 0x01);
  *bit_parity ^= ((byte_in >> 4) & 0x01);
  *bit_parity ^= ((byte_in >> 5) & 0x01);
  *bit_parity ^= ((byte_in >> 6) & 0x01);
  *bit_parity ^= ((byte_in >> 7) & 0x01);
}

/********************************************************************************/
uint32_t write_bytes(uint32_t bytes_length, uint8_t *bytes_info, uint32_t etu_length, 
         uint32_t pin_number)
/*--------------------------------------------------------------------------------
| write bytes vector with length bytes_length to PIN number pin_number
|
--------------------------------------------------------------------------------*/
{
  uint32_t i;
  uint8_t check_bits;
	
#if (IF_PRINT_LOG)		
	if (UART_LOG)
	{
    printf_log_rx(bytes_length, bytes_info);
	}
#endif  
#if (PRINT_DELAY_MS)
  nrf_delay_ms(PRINT_DELAY_MS);
#endif
	
  /* set CC register value for timer0 interrupt event time interval, for ETU timing */
  NRF_TIMER0->CC[0] = etu_length;
  NRF_TIMER0->TASKS_START = 1;
  
  for (i=0; i<bytes_length; i++)
  {
    get_parity_bit(*(bytes_info + i), &check_bits );
    write_byte(*(bytes_info + i), check_bits, etu_length, pin_number);
  }  

  /* stop ETU timer tick */
  NRF_TIMER0->TASKS_STOP = 1;
  
  return(0);
}

/********************************************************************************/
uint32_t read_bytes_phone(uint32_t read_length, uint8_t *bytes_info, uint8_t *check_bits, 
         uint32_t pin_number, uint32_t etu_length)
/*--------------------------------------------------------------------------------
| read bytes vector with length read_length from PIN number pin_number of phone side
|
--------------------------------------------------------------------------------*/
{
  uint32_t i;
  uint32_t initial_timer;
	
  /* set CC register value for timer0 interrupt event time interval, for ETU timing */
  NRF_TIMER0->CC[0] = etu_length;
  NRF_TIMER0->TASKS_START = 1;

  /* get the initial real time counter */
  initial_timer = NRF_RTC0->COUNTER;
  
  for (i=1; i<read_length + 1; i++)
  {
    while (nrf_gpio_pin_read(pin_number))
    {
      /* re-load watch dog request register */
      NRF_WDT->RR[0] = 0x6E524635;          			
      if ((NRF_RTC0->COUNTER - initial_timer) > DATA_TX_TIME)
      {
        /* stop ETU timer tick */
        NRF_TIMER0->TASKS_STOP = 1;
        return(1);
      }
    }	
    read_byte(bytes_info + i, check_bits + i, etu_length, pin_number);

    /* clock signal, false alarm read, discard the byte */		
    if (*(bytes_info + 1) == 0xff && *(check_bits + 1) == 0x1)
    {
      i = 1;
    }
  }
	
  /* To be updated, read_length maybe larger than 255 */
  /* for wireless SIM-phone interface, the read bytes length is less than 255,
  * since the authentication command (0x88) data length is less than 255 */
  *(bytes_info + 0)	= read_length;
  *(check_bits + 0)	= read_length;
	
#if (IF_PRINT_LOG)
	if (UART_LOG)
	{
    printf_log_tx(read_length, bytes_info + 1);
	}
#endif  
#if (PRINT_DELAY_MS)
  nrf_delay_ms(PRINT_DELAY_MS);
#endif

  if (parity_bit_check(read_length, bytes_info + 1, check_bits + 1))
  {
#if (IF_PRINT_LOG)
		if (UART_LOG)
		{
      printf("--------Parity bits check error in read_bytes --------\r\n");
		}
#endif
  }
	
  /* stop ETU timer tick */
  NRF_TIMER0->TASKS_STOP = 1;

  return(0);  	
}

/********************************************************************************/
uint32_t parity_bit_check(uint32_t length_byte, uint8_t *byte_in, uint8_t *bit_parity)
/*--------------------------------------------------------------------------------
| check if the parity is crrect for each byte of length length_byte
|
--------------------------------------------------------------------------------*/
{
  uint32_t i;
  uint8_t tmp_parity_bit;
	
  for (i=0; i<length_byte; i++)
  {
    get_parity_bit(*(byte_in + i), &tmp_parity_bit);
    if (tmp_parity_bit != *(bit_parity + i))
    {
#if (IF_PRINT_LOG)		
			if (UART_LOG)
			{
        printf("\r\n-----------Parity bit check error, byte=%x, parity bit =%x, \r\n", 
			    *(byte_in + i), *(bit_parity + i));
		  }
#endif
      return (1);			
    }
  }	
	
  return (0);		
}


