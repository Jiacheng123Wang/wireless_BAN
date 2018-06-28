#include "comm_wireless_interface.h"
#include "comm_UICC_terminal_interface.h"
#include "comm_misc.h"
#include "define_config.h"

extern uint8_t UART_LOG;
uint8_t PHONE_ADD_LOGICAL = 0;
uint8_t DATA_CARRIER_FREQ = 0;
uint8_t AUTHENTICATE_RESULT[KEY_LENGTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//uint8_t USER_PASSWORD[KEY_LENGTH] = {'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w'}; 
uint8_t MASK_RECEIVE_CALL = 0x2;
uint8_t FREQ_BIN_PATTEN[83] = { 82,    27,    74,     9,    75,    68,     2,    56,    42,    24,    
    35,    65,     1,    26,    23,    66,    31,    39,     4,    44,    47,    60,    50,    11,    
    48,    17,    64,    76,     6,    54,    32,     3,    38,    45,    79,    15,    12,    67,    
    63,    40,    33,    70,    41,     5,    77,    62,    20,    13,    28,    46,    55,    43,    
    61,    69,    49,    73,    37,    78,    19,    22,    30,    71,    21,    72,    18,    81,    
    53,    10,    59,    80,    51,    36,    58,    29,    57,     8,    14,     7,    82,    34,    
    52,    16,    25};
uint8_t PASSWORD_SHUFFLE[16] = {0, 5, 8, 2, 6, 13, 7, 15, 11, 1, 9, 4, 3, 12, 14, 10};	 
uint8_t SYSTEM_KEY_SHUFFLE[16] = {8, 14, 1, 5, 2, 9, 12, 13, 6, 11, 3, 10, 15, 0, 7, 4};	 

#if (IF_CC_CODING)
uint8_t INTERLEAVE_PATTERN[256] = 
  {91, 38, 24, 27, 198, 223, 219, 199, 52, 192, 232, 229, 161, 95, 249, 84, 178, 209,
   60, 145, 44, 70, 50, 240, 208, 237, 220, 217, 31, 62, 110, 242, 54, 68, 73, 153, 
   206, 104, 122, 160, 231, 171, 130, 127, 108, 143, 1, 72, 139, 156, 168, 238, 193, 255, 
   162, 96, 69, 4, 118, 83, 32, 157, 87, 34, 239, 61, 212, 176, 40, 109, 59, 230, 
   66, 115, 205, 74, 177, 26, 71, 128, 214, 204, 197, 36, 225, 123, 81, 174, 7, 11, 
   90, 105, 142, 119, 201, 211, 135, 39, 221, 8, 241, 216, 133, 98, 248, 3, 103, 93, 
   112, 126, 215, 25, 6, 245, 250, 37, 233, 228, 56, 55, 149, 19, 148, 12, 246, 14, 
   48, 218, 152, 151, 35, 172, 46, 182, 236, 170, 173, 29, 164, 117, 165, 207, 99, 235, 
   18, 9, 57, 179, 49, 42, 202, 16, 15, 150, 137, 147, 196, 2, 163, 138, 159, 203, 
   132, 144, 194, 113, 134, 191, 195, 131, 77, 80, 146, 51, 244, 167, 111, 169, 89, 64, 
   65, 79, 227, 47, 121, 213, 30, 53, 251, 43, 23, 85, 254, 63, 188, 210, 155, 125, 
   175, 67, 200, 22, 140, 75, 33, 21, 106, 224, 116, 0, 92, 17, 120, 166, 5, 184, 
   252, 190, 141, 94, 154, 20, 189, 101, 10, 82, 28, 100, 76, 226, 41, 129, 107, 247, 
   158, 181, 253, 86, 183, 186, 114, 243, 222, 187, 185, 102, 58, 180, 124, 136, 45, 88, 
   234, 13, 97, 78};
#endif
   
/********************************************************************************/
uint32_t transmit_power_level_set(uint32_t power_level)
/*--------------------------------------------------------------------------------
| Radio Tx power level:
| 0: -30dBm for nRF51, -40dBm for nRF52
| 1: -20dBm
| 2: -16dBm
| 3: -12dBm
| 4: -8dBm
| 5: -4dBm
| 6: 0dBm
| 7: 4dBm
| 8: 3dBm, nRF52 only
| default:  4dBm
|
--------------------------------------------------------------------------------*/
{
  switch(power_level)
  {
    case 0:
#if (ARGET_CHIP == NRF52)
      NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Neg40dBm << RADIO_TXPOWER_TXPOWER_Pos);
#else
      NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Neg30dBm << RADIO_TXPOWER_TXPOWER_Pos);
#endif      
      break;
    
    case 1:
      NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Neg20dBm << RADIO_TXPOWER_TXPOWER_Pos);
      break;
    
    case 2:
      NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Neg16dBm << RADIO_TXPOWER_TXPOWER_Pos);
      break;
    
    case 3:
      NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Neg12dBm << RADIO_TXPOWER_TXPOWER_Pos);
      break;
    
    case 4:
      NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Neg8dBm << RADIO_TXPOWER_TXPOWER_Pos);
      break;
    
    case 5:
      NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Neg4dBm << RADIO_TXPOWER_TXPOWER_Pos);
      break;
    
    case 6:
      NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_0dBm << RADIO_TXPOWER_TXPOWER_Pos);
      break;
    
    case 7:
      NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Pos4dBm << RADIO_TXPOWER_TXPOWER_Pos);
      break;
    
#if (ARGET_CHIP == NRF52)
    case 8:
      NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Pos3dBm << RADIO_TXPOWER_TXPOWER_Pos);
      break;
#endif
    
    default:
      NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Pos4dBm << RADIO_TXPOWER_TXPOWER_Pos);
      break;
  }
  
  return(0);
}

/********************************************************************************/
uint32_t radio_mode_set(uint32_t mode)
/*--------------------------------------------------------------------------------
| Radio Tx mode level:
| 0: 250kbps
| 1: 1Mbps
| 2: 2Mbps
| 3: 1Mbps, BLE
|
--------------------------------------------------------------------------------*/
{
  switch(mode)
  {
    case 0:
      NRF_RADIO->MODE = (RADIO_MODE_MODE_Nrf_250Kbit << RADIO_MODE_MODE_Pos);
      break;
    
    case 1:
      NRF_RADIO->MODE = (RADIO_MODE_MODE_Nrf_1Mbit << RADIO_MODE_MODE_Pos);
      break;
    
    case 2:
      NRF_RADIO->MODE = (RADIO_MODE_MODE_Nrf_2Mbit << RADIO_MODE_MODE_Pos);
      break;
    
    case 3:
      NRF_RADIO->MODE = (RADIO_MODE_MODE_Ble_1Mbit << RADIO_MODE_MODE_Pos);
      break;
    
    default:
      NRF_RADIO->MODE = (RADIO_MODE_MODE_Nrf_250Kbit << RADIO_MODE_MODE_Pos);
      break;
  }
  
  return(0);
}

/********************************************************************************/
uint32_t transmit_info_check(uint32_t packet_length, uint8_t *tx_info_byte)
/*--------------------------------------------------------------------------------
| data packet wireless transmission
|
--------------------------------------------------------------------------------*/
{
  uint32_t tx_interval = DATA_TX_TIME_INTERVAL;
  uint32_t rx_ack_interval = ACK_RX_TIME;
  uint32_t tx_power_level_data;
  uint32_t max_tx_time_ms;
  uint32_t frequency;
  uint32_t logical_address_tx;
  uint32_t logical_address_rx;
  uint32_t initial_timer;
  uint8_t ack_rx[4] = {0, 0, 0, 0};  
 
  tx_power_level_data = RADIO_TX_POWER_DATA;
  max_tx_time_ms = DATA_TX_TIME;    
  logical_address_tx = PHONE_ADD_LOGICAL;
  logical_address_rx = (1 << logical_address_tx);
  frequency = DATA_CARRIER_FREQ;
  
  /* get the initial real time counter */
  initial_timer = NRF_RTC0->COUNTER;
  while (1)
  {  
    /* data carrier collision detection */
    radio_carrier_detection(MAX_CD_TIME_MS, TX_RSSI_THRED, frequency);
    /* data packet transmit */
    tx_radio_config_parameter(tx_info_byte, frequency, logical_address_tx, tx_interval, tx_power_level_data); 
     
    /* ACK receive */                          
    if(!(rx_radio_config_parameter(ack_rx, frequency, logical_address_rx, rx_ack_interval)))
    {
      return(0);
    }
    /* data packet transmit time out */
    if ((NRF_RTC0->COUNTER - initial_timer) > max_tx_time_ms)
    {
      return(1);
    }
  }  
}

/********************************************************************************/
uint32_t receive_info_check(uint32_t *packet_length, uint8_t *rx_info_byte)
/*--------------------------------------------------------------------------------
| data packet wireless receiving
|
--------------------------------------------------------------------------------*/
{
  uint32_t ack_tx_time_ms = ACK_TX_TIME;
  uint32_t logical_address_tx;
  uint32_t logical_address_rx;
  uint32_t max_rx_time_ms;
  uint32_t frequency;
  uint32_t tx_power_level_ack;
  uint8_t ack_tx[4] = {3, 1, 1, 1};
  uint32_t initial_timer;
  
  max_rx_time_ms = DATA_RX_TIME;
  logical_address_tx = PHONE_ADD_LOGICAL;
  logical_address_rx = (1 << logical_address_tx);
  frequency = DATA_CARRIER_FREQ;
  tx_power_level_ack = POWER_LEVEL_ACK;
  *rx_info_byte = 0;
  
  /* get the initial real time counter */
  initial_timer = NRF_RTC0->COUNTER;
  while(1)
  {  
    /* data packet receive and RCR check OK */                          
    if (!rx_radio_config_parameter(rx_info_byte, frequency, logical_address_rx, max_rx_time_ms))
    {
      /* received packet length */
      *packet_length = *rx_info_byte;   
	  if (NRF_RADIO->CRCSTATUS == 1U)
	  {                                              
        /* ACK transmission */
        ack_wireless_tx(ack_tx, frequency, logical_address_tx, ack_tx_time_ms, tx_power_level_ack);
        return(0);
	  }
    }

    /* data packet receiving time out */
    if ((NRF_RTC0->COUNTER - initial_timer) > max_rx_time_ms)
    {
	  if (*rx_info_byte)
	  {
        /* ACK transmission */
        ack_wireless_tx(ack_tx, frequency, logical_address_tx, ack_tx_time_ms, tx_power_level_ack);	  	
        return(0);
	  }	
      else
	  {
        return(1);
	  }  
    }	
  }    
}

/********************************************************************************/
uint32_t tx_radio_config_parameter(uint8_t *tx_packet, uint32_t frequency, 
       uint32_t logical_address, uint32_t wait_time_out_ms, uint32_t power_level)
/*--------------------------------------------------------------------------------
| Radio package transmission with configured parameters
|
--------------------------------------------------------------------------------*/
{
  uint32_t time_out = 0;
  uint32_t radio_mode = RADIO_MODE;  
  uint32_t initial_timer;
  uint32_t radio_time_out_ms = 2;

  /* re-load watch dog request register */
  NRF_WDT->RR[0] = 0x6E524635;

  /* Set radio mode */
  radio_mode_set(radio_mode);
  /* Set radio Tx power level */
  transmit_power_level_set(power_level);  
  /* Set Frequency bin */
  NRF_RADIO->FREQUENCY = frequency;   
  /* Set device address 0 to use when transmitting */
  NRF_RADIO->TXADDRESS = logical_address;      
  
  /* Set payload pointer */
  NRF_RADIO->PACKETPTR = (uint32_t)tx_packet;  
  
  NRF_RADIO->EVENTS_READY = 0U;
  /* Enable radio and wait for ready */
  NRF_RADIO->TASKS_TXEN = 1U;
  /* get the initial real time counter */
  initial_timer = NRF_RTC0->COUNTER;  
  while (NRF_RADIO->EVENTS_READY == 0U)
  {
    /* radio enable time out */  
    if ((NRF_RTC0->COUNTER - initial_timer) > radio_time_out_ms)
    {    
#if (IF_PRINT_LOG)
			if (UART_LOG)
			{
        printf("..................Radio enable time out in Tx..................\r\n" );
			}
#endif      
      /* system reset */
      NVIC_SystemReset( );
    }
    if (NRF_RTC0->COUNTER < initial_timer)
    {
      initial_timer = NRF_RTC0->COUNTER;
    }
  }
  
  /* Start radio Tx task and waiting for complete */
  NRF_RADIO->TASKS_START = 1U;
  NRF_RADIO->EVENTS_END = 0U;  
  /* get the initial real time counter */
  initial_timer = NRF_RTC0->COUNTER;  
  while(NRF_RADIO->EVENTS_END == 0U)
  {    
    /* radio transmit time out */  
    if ((NRF_RTC0->COUNTER - initial_timer) > wait_time_out_ms)
    {    
      time_out = 1;
      break;
    }
    if (NRF_RTC0->COUNTER < initial_timer)
    {
      initial_timer = NRF_RTC0->COUNTER;
    }
  }
  
  NRF_RADIO->EVENTS_DISABLED = 0U;
  /* Disable radio */
  NRF_RADIO->TASKS_DISABLE = 1U;
  /* get the initial real time counter */
  initial_timer = NRF_RTC0->COUNTER;  
  while(NRF_RADIO->EVENTS_DISABLED == 0U)
  {
    /* radio disable time out */  
    if ((NRF_RTC0->COUNTER - initial_timer) > radio_time_out_ms)
    {    
#if (IF_PRINT_LOG)
			if (UART_LOG)
			{
        printf("..................Radio disable time out in Tx..................\r\n" );
			}
#endif
      /* system reset */
      NVIC_SystemReset( );
    }
    if (NRF_RTC0->COUNTER < initial_timer)
    {
      initial_timer = NRF_RTC0->COUNTER;
    }
  }
  
  return(time_out);
}

/********************************************************************************/
uint32_t rx_radio_config_parameter(uint8_t *rx_packet, uint32_t frequency, 
       uint32_t logical_address, uint32_t wait_time_out_ms)
/*--------------------------------------------------------------------------------
| radio receiving with time out
|
--------------------------------------------------------------------------------*/
{
  uint32_t time_out = 0;
  uint32_t radio_mode = RADIO_MODE;
  uint32_t initial_timer;
  uint32_t radio_time_out_ms = 2;
  
  /* re-load watch dog request register */
  NRF_WDT->RR[0] = 0x6E524635;
    
  /* Set radio mode */
  radio_mode_set(radio_mode);
  /* Set Frequency bin */
  NRF_RADIO->FREQUENCY = frequency; 
  /* Enable device address to use which receiving */
  NRF_RADIO->RXADDRESSES = logical_address;
  
  /* Set payload pointer */
  NRF_RADIO->PACKETPTR = (uint32_t)rx_packet;
  
  NRF_RADIO->EVENTS_READY = 0U;
  /* Enable radio and wait for ready */
  NRF_RADIO->TASKS_RXEN = 1U;    
  /* get the initial real time counter */
  initial_timer = NRF_RTC0->COUNTER;
  while(NRF_RADIO->EVENTS_READY == 0U)
  {  
    /* radio enable time out */  
    if ((NRF_RTC0->COUNTER - initial_timer) > radio_time_out_ms)
    {    
#if (IF_PRINT_LOG)
			if (UART_LOG)
			{
        printf("..................Radio enable time out in Rx ..................\r\n");
			}
#endif   
      /* system reset */
      NVIC_SystemReset( );
    }
    if (NRF_RTC0->COUNTER < initial_timer)
    {
      initial_timer = NRF_RTC0->COUNTER;
    }
  }
  
  NRF_RADIO->EVENTS_END = 0U;
  /* Start listening and wait for address received event */
  NRF_RADIO->TASKS_START = 1U;  
  /* Wait for end of packet */
  /* get the initial real time counter */
  initial_timer = NRF_RTC0->COUNTER;
  while (NRF_RADIO->EVENTS_END == 0U)
  {
    /* radio receive time out */  
    if ((NRF_RTC0->COUNTER - initial_timer) > wait_time_out_ms)
    {    
      time_out = 1;
      break;
    }
    if (NRF_RTC0->COUNTER < initial_timer)
    {
      initial_timer = NRF_RTC0->COUNTER;
    }
    /* re-load watch dog request register */
    NRF_WDT->RR[0] = 0x6E524635;
  }  
  
  NRF_RADIO->EVENTS_DISABLED = 0U;
  /* Disable radio */
  NRF_RADIO->TASKS_DISABLE = 1U;
  /* get the initial real time counter */
  initial_timer = NRF_RTC0->COUNTER;
  while(NRF_RADIO->EVENTS_DISABLED == 0U)
  {
    /* radio disable time out */  
    if ((NRF_RTC0->COUNTER - initial_timer) > radio_time_out_ms)
    {    
#if (IF_PRINT_LOG)
			if (UART_LOG)
			{
        printf("..................Radio disable time out in Rx..................\r\n" );
			}
#endif
      /* system reset */
      NVIC_SystemReset( );
    }
    if (NRF_RTC0->COUNTER < initial_timer)
    {
      initial_timer = NRF_RTC0->COUNTER;
    }
  }
  
  /* 1: time out, 0: packet recceived */ 
  return (time_out);
}

/********************************************************************************/
uint32_t radio_rssi_measurement(uint32_t frequency)
/*--------------------------------------------------------------------------------
| RSSI measurement 
|
--------------------------------------------------------------------------------*/
{
  uint32_t initial_timer;
  uint32_t radio_time_out_ms = 2;
  uint32_t rssi_value;
  
  
  /* Set radio mode */
  radio_mode_set(RADIO_MODE);
  /* Set Frequency bin */
  NRF_RADIO->FREQUENCY = frequency; 
  /* Enable device address to use which receiving */
  NRF_RADIO->RXADDRESSES = 0xff;
//  /* disable shorts */
//  NRF_RADIO->SHORTS = 0;
  
  NRF_RADIO->EVENTS_READY = 0U;
  /* Enable radio in Rx mode and wait for ready */
  NRF_RADIO->TASKS_RXEN = 1U;    
  /* get the initial real time counter */
  initial_timer = NRF_RTC0->COUNTER;
  while(NRF_RADIO->EVENTS_READY == 0U)
  {      
    /* radio enable time out */  
    if ((NRF_RTC0->COUNTER - initial_timer) > radio_time_out_ms)
    {    
#if (IF_PRINT_LOG)
			if (UART_LOG)
			{
        printf("..................Radio enable time out in RSSI measurement..................\r\n" );
			}
#endif   
      /* system reset */
      NVIC_SystemReset( );
    }
    if (NRF_RTC0->COUNTER < initial_timer)
    {
      initial_timer = NRF_RTC0->COUNTER;
    }
  }

///////////////////////////////////////////////////////////////////  
  // Start RSSI sample
  NRF_RADIO->EVENTS_RSSIEND = 0;
  NRF_RADIO->TASKS_RSSISTART = 1U;
  while (NRF_RADIO->EVENTS_RSSIEND == 0)
  {
    /* radio RSSI sampling time out */  
    if ((NRF_RTC0->COUNTER - initial_timer) > radio_time_out_ms)
    {    
#if (IF_PRINT_LOG)
			if (UART_LOG)
			{
        printf("..................RSSI value get time out in RSSI measurement..................\r\n" );
			}
#endif   
      /* system reset */
      NVIC_SystemReset( );
    }      
    if (NRF_RTC0->COUNTER < initial_timer)
    {
      initial_timer = NRF_RTC0->COUNTER;
    }
  }

  rssi_value = NRF_RADIO->RSSISAMPLE;
  NRF_RADIO->TASKS_RSSISTOP = 1U;
///////////////////////////////////////////////////////////////////  
  
  NRF_RADIO->EVENTS_DISABLED = 0U;
  // Disable radio
  NRF_RADIO->TASKS_DISABLE = 1U;
  /* get the initial real time counter */
  initial_timer = NRF_RTC0->COUNTER;
  while(NRF_RADIO->EVENTS_DISABLED == 0U)
  {
    /* radio enable time out */  
    if ((NRF_RTC0->COUNTER - initial_timer) > radio_time_out_ms)
    {    
#if (IF_PRINT_LOG)
			if (UART_LOG)
			{
        printf("..................Radio disable time out in RSSI measurement..................\r\n" );

			}
#endif   
      /* system reset */
      NVIC_SystemReset( );
    }
    if (NRF_RTC0->COUNTER < initial_timer)
    {
      initial_timer = NRF_RTC0->COUNTER;
    }	
  }  
  
  return (rssi_value);
}

/********************************************************************************/
uint32_t radio_carrier_detection(uint32_t max_CD_ms, uint32_t noise_threshold, uint32_t frequency)
{
  uint32_t initial_timer;
  
  /* get the initial real time counter */
  initial_timer = NRF_RTC0->COUNTER;

  /* if the carrier frequency quality is not good, wait some time */  
  while (1)
  {
    if (((NRF_RTC0->COUNTER - initial_timer) > max_CD_ms) || (NRF_RTC0->COUNTER < initial_timer) || (radio_rssi_measurement(frequency) > noise_threshold))
    {
      break;
    }
  }  
  
  nrf_delay_us(300);
  
  return(0);  
}


/********************************************************************************/
uint32_t wireless_sim_authentication_algorithm(uint8_t *user_passwaord, uint8_t *system_key, 
         uint8_t *challenge_data, volatile uint8_t *response_data)
/*--------------------------------------------------------------------------------
| wireless SIM interfac access channel authentication algorithm example
|
--------------------------------------------------------------------------------*/
{
  uint32_t key_length = KEY_LENGTH;
  uint32_t i;
  
  for (i=0; i<key_length; i++)
  {
    *(response_data + i) = (((*(user_passwaord + i)) | (*(system_key + i))) & (*(challenge_data + i)));
  }
  
  return(0);
}

/********************************************************************************/
uint32_t wireless_sim_authenrication_initialization(void)
/*--------------------------------------------------------------------------------
| initialize the authentication results value with all zero
|
--------------------------------------------------------------------------------*/
{
  uint32_t key_length = KEY_LENGTH;
  uint32_t i;
  
  for (i=0; i<key_length; i++)
  {
    AUTHENTICATE_RESULT[i] = 0;
  }  
  
  return(0);
}
  
/*------------------------------------------------------------------------------*/
/********************************************************************************/
uint32_t ack_wireless_tx(uint8_t *ack_tx, uint32_t frequency, 
                         uint32_t logical_address_tx,
                         uint32_t ack_tx_time_ms, uint32_t tx_power_level_ack)
/*--------------------------------------------------------------------------------
| ACK wireless transmission
|
--------------------------------------------------------------------------------*/
{
  radio_carrier_detection(MAX_CD_TIME_MS, TX_RSSI_THRED, frequency);
  tx_radio_config_parameter(ack_tx, frequency, logical_address_tx, ack_tx_time_ms, tx_power_level_ack);                            
                            
  return(0);
}

/*------------------------------------------------------------------------------*/
/*----------------------- CC channel coding ------------------------------------*/
/*------------------------------------------------------------------------------*/
#if (IF_CC_CODING)
/***************************************************************************/
uint32_t encoder_halfrate (uint8_t *input_bit, uint32_t bit_length, 
             uint8_t initial_state, uint8_t *encoded_bit)
/* --------------------------------------------------------------------------
   memory_length = 3
   encoder_number = 2
   encoder0 {1, 0, 0, 1} 
   encoder1 {1, 1, 0, 1}   
   input_bit --> the input bit sequence to be cc encoded
   bit_length --> the length of bit sequence
   initial_state --> the cc encoder initial state 
   
   encoded_bit <--
 * --------------------------------------------------------------------------
 */
{
  uint32_t i;
  uint8_t current_state;
  uint8_t output_bit0[16] = {0,  0,  0,  0,  1,  1,  1,  1,
                            0,  1,  0,  1,  1,  0,  1,  0};
  uint8_t output_bit1[16] = {1,  1,  1,  1,  0,  0,  0,  0,
                            1,  0,  1,  0,  0,  1,  0,  1};
  uint8_t to_state0[8] = {0,  2,  4,  6,  0,  2,  4,  6};
  uint8_t to_state1[8] = {1,  3,  5,  7,  1,  3,  5,  7};

  /** current state is the initial state **/
  current_state = initial_state;
    
  for(i = 0; i < bit_length; i++)
  {
    /** calculate output due to input bit 0 **/
    if(*(input_bit + i) == 0)
    {
      *(encoded_bit + 2 * i) = output_bit0[current_state];
      *(encoded_bit + 2 * i + 1) = output_bit0[8 + current_state];
      /** calculate the new state **/
      current_state = to_state0[current_state];
    }
    /** calculate output due to input bit 1 **/
    else
    {
      *(encoded_bit + 2 * i) = output_bit1[current_state];
      *(encoded_bit + 2 * i + 1) = output_bit1[8 + current_state];
      /** calculate the new state **/
      current_state = to_state1[current_state];
    }
  }
  
  return(0);
}

/***************************************************************************/
uint32_t decoder_hallfrate(uint8_t *input_receivebit, uint32_t bit_length,
       uint8_t initial_state, uint8_t *decoded_bit)
/* -------------------------------------------------------------------------- 
   cc decoder by Viterbi algorithm
   memory_length = 3
   encoder_number = 2
   encoder0 {1, 0, 0, 1} 
   encoder1 {1, 1, 0, 1}   
   
   input_receivebit --> the received hard bit sequence to be decoded
   bit_length --> the decoding  bit length
   initial_state --> the cc encoder initial state 
   
   decoded_bit <--
 * --------------------------------------------------------------------------
 */
{
  /** loop control variable **/	
  uint32_t i;
  uint32_t j;	
  uint32_t k;	
  uint8_t previous_state[16] = 
          {0,  4,  0,  4,  1,  5,  1,  5,  2,  6,  2,  6,  3,  7,  3,  7};
  uint8_t encoding_bit0[16] = 
          {0,  0,  0,  1,  0,  0,  0,  1,  1,  1,  1,  0,  1,  1,  1,  0};
  uint8_t encoding_bit1[16] =
          {1,  1,  1,  0,  1,  1,  1,  0,  0,  0,  0,  1,  0,  0,  0,  1};                              

  uint8_t encoder_number = 2;
  uint8_t memory_length = 3;
  /** the encoder register state number: = 2 ^ memory_length **/    
  uint8_t states_number = 8;
  /** variables for trellis generation **/    
  uint8_t to_state0[8] = {0,  2,  4,  6,  0,  2,  4,  6};
  uint8_t to_state1[8] = {1,  3,  5,  7,  1,  3,  5,  7};
  /** variables for state store **/
  uint8_t *state_history;
  uint8_t *state_sequence;
 
  uint32_t accum_distance_metric[8] = {0, 0, 0, 0, 0, 0, 0, 0};  
  uint32_t accum_distance_metric_history[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  uint32_t tmp_metric_0;
  uint32_t tmp_metric_1;
  uint8_t tmp_state_0[8];
  uint8_t tmp_state_1[8];
  uint8_t tmp_number;    
    
  uint32_t min_value;
  uint32_t min_index;
  uint8_t depth_of_trellis;
  
  depth_of_trellis = bit_length;
  
  state_history = (uint8_t *)malloc(sizeof(uint8_t) * states_number *
                  (depth_of_trellis + 1));
  if (state_history == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
      printf("memory allocation error in viterbi_decoding...\r\n");
		}
#endif
    
    return(1); 
  }

  state_sequence = (uint8_t *)malloc(sizeof(uint8_t) * states_number *
                  (depth_of_trellis + 1));
  if (state_sequence == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
      printf("memory allocation error in viterbi_decoding...\r\n");
		}
#endif
    
    return(1); 
  }
  
  	  
  for(i = 0; i < states_number * depth_of_trellis; i++)
  {
	  *(state_history + i) = 0;
	  *(state_sequence + i) = 0;
  }
  for(i = 0; i < states_number; i++)
  {
	  *(state_history + i) = initial_state;
  }
	   
  *(tmp_state_0 + 0) = initial_state; 
  for(i = 0; i < memory_length; i++)
  {
 	  tmp_number = (1 << i);	  	  
	  	  
		for(j = 0; j < tmp_number; j++)
		{ 
		  *(tmp_state_1 + 2 * j + 0) = *(to_state0 + (*(tmp_state_0 + j)));
      *(tmp_state_1 + 2 * j + 1) = * (to_state1 + (*(tmp_state_0 + j)));
		}
			  
		for(j = 0; j < tmp_number; j++)
		{ 
		  *(state_sequence + (i + 1) * states_number + (*(tmp_state_1 + 2 * j + 0))) = 
			  	      (*(tmp_state_1 + 2 * j + 0));
		  *(state_sequence + (i + 1) * states_number + (*(tmp_state_1 + 2 * j + 1))) = 
			  	      (*(tmp_state_1 + 2 * j + 1));
	  	      
	    for ( k = 0; k < i + 1 ; k++ )
	    {
	      *(state_sequence + k * states_number + (*(tmp_state_1 + 2 * j + 0))) = 
	      	   	       *(state_history + k * states_number + (*(tmp_state_0 + j)));
	      *(state_sequence + k * states_number + (*(tmp_state_1 + 2 * j + 1))) = 
	      	   	       *(state_history + k * states_number + (*(tmp_state_0 + j)));
	    }
	      	   
	    (*(accum_distance_metric + (*(tmp_state_1 + 2 * j + 0)))) = 
	      (*(accum_distance_metric_history + (*(tmp_state_0 + j)))) + 
	      distance_between_vector(encoding_bit0 + (*(tmp_state_0 + j)) * encoder_number, 
		    input_receivebit + i * encoder_number, encoder_number);    
			           
	    (*(accum_distance_metric + (*(tmp_state_1 + 2 * j + 1)))) = 
	      (*(accum_distance_metric_history + (*(tmp_state_0 + j)))) + 
	      distance_between_vector(encoding_bit1 + (*(tmp_state_0 + j)) * encoder_number, 
			  input_receivebit + i * encoder_number, encoder_number);         	   
		}
			         	  
		for(j = 0; j < tmp_number; j++)
		{          
		  *(accum_distance_metric_history + (*(tmp_state_1 + 2 * j + 0))) = 
			      *(accum_distance_metric + (*(tmp_state_1 + 2 * j + 0))); 
		  *(accum_distance_metric_history + (*(tmp_state_1 + 2 * j + 1))) = 
			      *(accum_distance_metric + (*(tmp_state_1 + 2 * j + 1))); 
      for(k = 0; k < i + 2 ; k++)
      {
      	*(state_history + k * states_number + (*(tmp_state_1 + 2 * j + 0))) = 
      	    *(state_sequence + k * states_number + (*(tmp_state_1 + 2 * j + 0)));
      	*(state_history + k * states_number + (*(tmp_state_1 + 2 * j + 1))) = 
      	    *(state_sequence + k * states_number + (*(tmp_state_1 + 2 * j + 1)));
      }
	  }
			  
		for(j = 0; j < tmp_number; j++)
		{ 
		  *(tmp_state_0 + 2 * j + 0) = *(tmp_state_1 + 2 * j + 0);
		  *(tmp_state_0 + 2 * j + 1) = *(tmp_state_1 + 2 * j + 1);
		}
  }
		
  for(i = memory_length; i < depth_of_trellis; i++)
  {
		for(j = 0; j < states_number / 2; j++)
		{
		  *(state_sequence + (i + 1) * states_number + 2 * j + 0) =  2 * j + 0;
		  tmp_metric_0 = distance_between_vector(encoding_bit0 + 
		                (*(previous_state + 4 * j + 0)) * encoder_number, 
			              input_receivebit + i * encoder_number, encoder_number) + 
			              (*(accum_distance_metric_history + 
			              (*(previous_state + 4 * j + 0))));
		  tmp_metric_1 = distance_between_vector(encoding_bit0 + 
		                (*(previous_state + 4 * j + 1)) * encoder_number, 
			              input_receivebit + i * encoder_number, encoder_number) +
			              (*(accum_distance_metric_history + 
			              (*(previous_state + 4 * j + 1))));
            
		  if(tmp_metric_0 < tmp_metric_1)
		  {
			  *(accum_distance_metric + 2 * j + 0) = tmp_metric_0;
			  for ( k = 0; k < i + 1 ; k++ )
			  {
			    *(state_sequence + k * states_number + 2 * j + 0) = 
			        *(state_history + k * states_number + (*(previous_state + 4 * j + 0)));
			  }
		  }
		  else
		  {
			  *(accum_distance_metric + 2 * j + 0) = tmp_metric_1;
			  for(k = 0; k < i + 1 ; k++)
			  {
			    *(state_sequence + k * states_number + 2 * j + 0) = 
			        *(state_history + k * states_number + (*(previous_state + 4 * j + 1)));
			  }
		  }        
			              					  
		  *(state_sequence + (i + 1) * states_number + 2 * j + 1) =  2 * j + 1;
      tmp_metric_0 = distance_between_vector(encoding_bit1 + 
              (*(previous_state + 4 * j + 2)) * encoder_number, 
			        input_receivebit + i * encoder_number, encoder_number) + 
			        (*(accum_distance_metric_history + (*(previous_state + 4 * j + 2))));
		  tmp_metric_1 = distance_between_vector(encoding_bit1 + 
		          (*(previous_state + 4 * j + 3)) * encoder_number, 
			        input_receivebit + i * encoder_number, encoder_number) +
			        (*(accum_distance_metric_history + (*(previous_state + 4 * j + 3))));
		  if(tmp_metric_0 < tmp_metric_1)
		  {
			  *(accum_distance_metric + 2 * j + 1) = tmp_metric_0;
			  for ( k = 0; k < i + 1 ; k++ )
			  {
			    *(state_sequence + k * states_number + 2 * j + 1) = 
			        *(state_history + k * states_number + (*(previous_state + 4 * j + 2)));
			  }
		  }
		  else
		  {
			  *(accum_distance_metric + 2 * j + 1) = tmp_metric_1;
			  for(k = 0; k < i + 1 ; k++)
			  {
			    *(state_sequence + k * states_number + 2 * j + 1) = 
			        *(state_history + k * states_number + (*(previous_state + 4 * j + 3)));
			  }
			      
		  }  
		}
			      
		for ( j = 0; j < states_number; j++ )
		{          
		  *(accum_distance_metric_history + j) = *(accum_distance_metric + j); 
      for(k = 0; k < i + 2 ; k++)
      {
      	*(state_history + k * states_number + j) = 
      	    	       *(state_sequence + k * states_number + j);
      }
	  }
  }
	  
  min_index_value(accum_distance_metric_history, states_number, 
	                /**/&min_value, &min_index);
	  	  
  for(i = 0; i < depth_of_trellis; i++)
  {
	  if((*(to_state0 + (*(state_sequence + i * states_number + min_index)))) == 
	  	  	   (*(state_sequence + (i + 1) * states_number + min_index)))
	  {
	    *(decoded_bit + i) = 0;
	  }
	  else if((*(to_state1 + (*(state_sequence + i * states_number + min_index)))) == 
	  	  	   (*(state_sequence + (i + 1) * states_number + min_index)))
	  {
	    *(decoded_bit + i) = 1;
	  }
	  else 
	  {
#if (IF_PRINT_LOG)    
			if (UART_LOG)
			{
	      printf("There is something wrong in viterbi_decoding..., i = %ld\r\n", i);
		  }
#endif
	    return(1);
	  }
  } 
	  	   
  free(state_history);
  free(state_sequence);

  return(0);
}

/***************************************************************************/
uint32_t int2bit(uint8_t state, uint8_t bit_length, /**/uint8_t *bit_array)
/* -------------------------------------------------------------------------- 
 * Map the state integer number to a bit array
 * --------------------------------------------------------------------------
 */
{
  uint8_t i;
	for(i = 0; i < bit_length; i++)
  {
    *(bit_array + i) = ((state >> i) & 1);
  }
  
  return(0);
}

/***************************************************************************/
uint32_t bit2int(uint8_t *bit_array, uint8_t bit_length, /**/uint8_t *state)
/* -------------------------------------------------------------------------- 
 * Map a bit array to the corresponding integer state number
 * --------------------------------------------------------------------------
 */
{
  uint8_t i;
    
	*state = 0;
	for (i = 0; i < bit_length; i++)
  {
    (*state ) |= (*(bit_array + i) << i);
  }
  
  return(0);
}

/***************************************************************************/
uint32_t char2bit_sequence(uint8_t *char_sequence, uint8_t char_length, 
                    uint8_t *bit_sequence)
/* -------------------------------------------------------------------------- 
 * Map the char (uint8_t) sequence to bit sequence
 * --------------------------------------------------------------------------
 */
{
  uint8_t i;
  
  for(i = 0; i < char_length; i++)
  {
    int2bit(*(char_sequence + i), 8, bit_sequence + i * 8);
  }
  
  return(0);
}

/***************************************************************************/
uint32_t bit2char_sequence(uint8_t *bit_sequence, uint8_t char_length, 
                    uint8_t *char_sequence)
/* -------------------------------------------------------------------------- 
 * Map the bit sequence to char (uint8_t) sequence
 * --------------------------------------------------------------------------
 */
{
  uint8_t i;
  
  for(i = 0; i < char_length; i++)
  {
	bit2int(bit_sequence + i * 8, 8, char_sequence + i);
  }
  
  return(0);
}

/***************************************************************************/
uint32_t cc_trellis_table_generation(uint8_t memory_length, uint8_t *cc_encoder,
                                /**/ uint8_t *output_bit0, uint8_t *output_bit1, 
                                uint8_t *from_state0, uint8_t *from_state1,
                                uint8_t *to_state0, uint8_t *to_state1)
/* -------------------------------------------------------------------------- 
 * Create convolutional encoder arrays used by encode and decode
   
   memory_length --> The encoder state's bit length
   cc_encoder --> binary sequence to determine the encoder, for example
                  g ( D ) = 1 + D^2 + D^3 + D^5 + D^6, then
                  cc_encoder -> 1011011, the bit length is memory_length + 1
   
   output_bit0: <-- The output bit of each state for input bit 0
   output_bit1: <-- The output bit of each state for input bit 1
   from_state0: <-- The state comes from of each state for input bit 0
   from_state1: <-- The state comes from of each state for input bit 1
   to_state0:   <-- The state comes to of each state for input bit 0
   to_state1:   <-- The state comes to of each state for input bit 1
 * --------------------------------------------------------------------------
 */
{
  uint8_t i;
  uint8_t j;
  uint8_t *state_bit_array;
  uint8_t *new_state_array;
  uint8_t states_number; 

  state_bit_array = (uint8_t *)malloc(sizeof(uint8_t) * memory_length);
  if (state_bit_array == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
	    printf("memory allocation error in cc_trellis_table_generation...\r\n");
	  }
#endif
    
    return(1);
  }
  
  new_state_array = (uint8_t *)malloc(sizeof(uint8_t) * memory_length);
  if (new_state_array == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
	    printf("memory allocation error in cc_trellis_table_generation...\r\n");
	  }
#endif
    
    return(1);
  }

  states_number = (1 << memory_length);

/* loop for the all possible state number: 0, ..., states_number */        
  for(i = 0; i < states_number; i++)
  {
    int2bit(i, memory_length, /**/state_bit_array);
        
    /** calculate output due to the input 0 **/
    *(output_bit0 + i ) = 0;
    for(j = 0; j < memory_length; j++)
    {
      if (*(cc_encoder + j + 1))
      {
        *(output_bit0 + i) ^= (*(state_bit_array + j));
      }
    }

    /** calculate new states **/
    for(j = memory_length - 1; j > 0; j--)
    {
      *(new_state_array + j) = *(state_bit_array + j - 1);
    }
    *(new_state_array + 0 ) = 0;

    /** from s' to s **/
    bit2int(new_state_array, memory_length, /**/(to_state0 + i));
    /** from s to s' **/
    *(from_state0 + (*(to_state0 + i))) = i;           
        
    /** calculate output due to the input 1 **/
    *(output_bit1 + i) = 1;
    for(j = 0; j < memory_length; j++)
    {
      if(*(cc_encoder + j + 1))
      {
        *(output_bit1 + i) ^= (*(state_bit_array + j));
      }
    }

    /** calculate new states **/
    for(j = memory_length - 1; j > 0; j--)
    {
      *(new_state_array + j) = *(state_bit_array + j - 1);
    }
    *(new_state_array + 0) = 1;
    /** from s' to s **/
    bit2int(new_state_array, memory_length, /**/(to_state1 + i));
    /** from s to s' **/
    *(from_state1 + (*(to_state1 + i))) = i;     
  }
   
  for (i = 0; i < 8; i++)  
  {
    
  }
  free(state_bit_array);
  free(new_state_array);
  
  return(0);
}

/***************************************************************************/
uint32_t cc_encode(uint8_t *input_bit, uint32_t bit_length, uint8_t memory_length,  
               uint8_t *cc_encoder, uint8_t initial_state, 
               /**/ uint8_t *encoded_bit)
/* -------------------------------------------------------------------------- 
   tail-bitting cc encoder
   
   input_bit --> the input bit sequence to be cc encoded
   bit_length --> the length of bit sequence
   memory_length --> The encoder state's bit length
   cc_encoder --> binary sequence to determine the encoder, for example
                  g ( D ) = 1 + D^2 + D^3 + D^5 + D^6, then
                  cc_encoder -> 1011011, the bit length is memory_length + 1
   
   encoded_bit <--
 * --------------------------------------------------------------------------
 */
{
  uint32_t i;	
  uint8_t *from_state0;
  uint8_t *from_state1;
  uint8_t *to_state0;
  uint8_t *to_state1;
  uint8_t current_state;
  uint8_t *output_bit0;
  uint8_t *output_bit1;
  uint8_t states_number;
    
  states_number = (1 << memory_length);
    
  output_bit0 = (uint8_t *)malloc(sizeof(uint8_t) *  states_number);
  if (output_bit0 == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
	    printf("memory allocation error in cc_encode...\r\n");
	  }
#endif
    
    return(1);
  }
  
  output_bit1 = (uint8_t *)malloc(sizeof(uint8_t) *  states_number);
  if (output_bit1 == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
	    printf("memory allocation error in cc_encode...\r\n");
	  }
#endif
    
    return(1);
  }
  
  from_state0 = (uint8_t *)malloc(sizeof(uint8_t) *  states_number);
  if (from_state0 == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
	    printf("memory allocation error in cc_encode...\r\n");
	  }
#endif
    
    return(1);
  }
  
  from_state1 = (uint8_t *)malloc(sizeof(uint8_t) *  states_number);
  if (from_state1 == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
	    printf("memory allocation error in cc_encode...\r\n");
	  }
#endif
    
    return(1);
  }
  
  to_state0 = (uint8_t *)malloc(sizeof(uint8_t) *  states_number);
  if (to_state0 == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
	    printf("memory allocation error in cc_encode...\r\n");
	  }
#endif
    
    return(1);
  }
  
  to_state1 = (uint8_t *)malloc(sizeof(uint8_t) *  states_number);
  if (to_state1 == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
	    printf("memory allocation error in cc_encode...\r\n");
	  }
#endif
    
    return(1);
  }
  
  cc_trellis_table_generation(memory_length, cc_encoder,
                         /**/ output_bit0, output_bit1, from_state0, from_state1, 
                         to_state0, to_state1);
                                                  
  /** current state is the initial state **/
  current_state = initial_state;
    
  for(i = 0; i < bit_length; i++)
  {
    /** calculate output due to input bit 0 **/
    if(*(input_bit + i) == 0)
    {
      *(encoded_bit + i) = *(output_bit0 + current_state);
      /** calculate the new state **/
      current_state = *(to_state0 + current_state);
    }
    /** calculate output due to input bit 1 **/
    else
    {
      *(encoded_bit + i) = *(output_bit1 + current_state);
      /** calculate the new state **/
      current_state = *(to_state1 + current_state);
    }
  }
	
  free(output_bit0);
  free(output_bit1);
  free(from_state0);
  free(from_state1);
  free(to_state0);
  free(to_state1);

  return(0);
}

/***************************************************************************/
uint32_t cc_encode_n(uint8_t *input_bit, uint32_t bit_length, 
                uint8_t memory_length, uint8_t *cc_encoder, 
                uint8_t encoder_number, uint8_t initial_state, 
                /**/ uint8_t *encoded_bit)
/* -------------------------------------------------------------------------- 
   tail-bitting cc encoder
   
   input_bit --> the input bit sequence to be cc encoded
   bit_length --> the length of bit sequence
   memory_length --> The encoder state's bit length
   encoder_number --> the cc component encoder number 
   cc_encoder --> binary sequence to determine the encoder, for example
                  g1 ( D ) = 1 + D^1  + D^2 + D^3 + D^6 
                  g2 ( D ) = 1 + D^2 + D^3 + D^5 + D^6, then
                  cc_encoder -> 1111001 1011011, the bit length is 
                  encoder_number * ( memory_length + 1 )
  encoded_bit <--
 * --------------------------------------------------------------------------
 */
{
/* loop control variable */	
  uint32_t i;
  uint32_t j;
  uint8_t *encoded_bit_tmp;
    
  encoded_bit_tmp = (uint8_t *)malloc(sizeof(uint8_t) * bit_length * encoder_number);
  if (encoded_bit_tmp == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
	    printf("memory allocation error in cc_encode_n...\r\n");
	  }
#endif
    
    return(1);
  }
  
  for(i = 0; i < encoder_number; i++)
  {
		cc_encode(input_bit, bit_length, memory_length, 
		          cc_encoder + i * ( memory_length + 1 ), initial_state, 
          /**/ encoded_bit_tmp + i * bit_length);
  }
    
  for(i = 0; i < bit_length; i++)
  {
    for(j = 0; j < encoder_number; j++)
	  {
	    *(encoded_bit + i * encoder_number + j ) =
	    	      * ( encoded_bit_tmp + j * bit_length + i);
	  }       
  }
    
  free(encoded_bit_tmp); 
    
  return(0);      	
}

/***************************************************************************/
uint32_t state_trellis_table(uint8_t memory_length,
                /**/uint8_t *next_state, uint8_t *previous_state)
{
  uint8_t i;
  uint8_t j;
  uint8_t *state_bit_array;
  uint8_t *new_state_array;
  uint8_t states_number; 
	
  states_number = (1 << memory_length);
    
  state_bit_array = (uint8_t *)malloc(sizeof(uint8_t) * memory_length);
  if (state_bit_array == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
	    printf("memory allocation error in state_trellis_table...\r\n");
	  }
#endif
    
    return(1);
  }
  
  new_state_array = (uint8_t *)malloc(sizeof(uint8_t) * memory_length);
  if (new_state_array == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
	    printf("memory allocation error in state_trellis_table...\r\n");
	  }
#endif
    
    return(1);
  }
        
  /** loop for the all possible state number: 0, ..., states_number **/        
  for(i = 0; i < states_number; i++)
  {
    int2bit(i, memory_length, /**/state_bit_array );
        
    /** calculate new states for input bit 0 **/
    for(j = memory_length - 1; j > 0; j--)
    {
      *(new_state_array + j) = *(state_bit_array + j - 1);
    }
    *(new_state_array + 0) = 0;

    bit2int(new_state_array, memory_length, /**/(next_state + 2 * i + 0));
                
    for(j = 0; j < memory_length - 1; j++)
    {
      *(new_state_array + j) = *(state_bit_array + j + 1);
    }
    *(new_state_array + memory_length - 1) = 0;

    bit2int(new_state_array, memory_length, /**/(previous_state + 2 * i + 0));
                
    /** calculate new states for input bit 1 **/
    for(j = memory_length - 1; j > 0; j--)
    {
		  *(new_state_array + j) = *(state_bit_array + j - 1);
    }
		*(new_state_array + 0) = 1;

    bit2int(new_state_array, memory_length, /**/(next_state + 2 * i + 1));
                
    for(j = 0; j < memory_length - 1; j++)
    {
		  *(new_state_array + j ) = *(state_bit_array + j + 1);
    }
		*(new_state_array + memory_length - 1) = 1;

    bit2int(new_state_array, memory_length, /**/(previous_state + 2 * i + 1));                
  }
       
  free(state_bit_array);
  free(new_state_array);
  
  return(0);
}

/***************************************************************************/
uint32_t distance_between_vector(uint8_t *input1_bit, uint8_t *input2_bit,
                   uint8_t bit_length)
{
  uint32_t tmp = 0;
  uint8_t i;
    
  tmp = 0;
  for(i = 0; i < bit_length; i++)
  {
    if(*(input1_bit + i) != *(input2_bit + i))
    {
      tmp++;
    }
  }     
    
  return(tmp);   
}

/***************************************************************************/
uint32_t min_index_value(uint32_t *vector_in, uint32_t vector_length,
     /**/ uint32_t *min_value, uint32_t *min_index)
/* -------------------------------------------------------------------------- 
 * Minimal value and its index in the input vector  
 *
 * --------------------------------------------------------------------------
 */
{
  uint32_t i;
    
  *min_index = 0;
  *min_value = *(vector_in + 0);
    
  for(i = 0; i < vector_length; i++)
  {
    if(*(vector_in + i ) < *min_value)
    {
      *min_value = *(vector_in + i);
      *min_index = i;
    }
  }
  
  return(0);
}

/***************************************************************************/
uint32_t viterbi_decoding(uint8_t *input_receivebit, 
                uint8_t memory_length, uint8_t encoder_number,
                uint8_t *cc_encoder, uint8_t initial_state, uint8_t depth_of_trellis, 
                /**/ uint8_t *decoded_bit)
/* -------------------------------------------------------------------------- 
   cc decoder by Viterbi algorithm
   
   input_bit --> the received hard bit sequence to be decoded
   memory_length --> The cc  state's bit length
   encoder_number --> the cc component encoder number 
   cc_encoder --> binary sequence to determine the encoder, for example
                  g1 ( D ) = 1 + D^1  + D^2 + D^3 + D^6 
                  g2 ( D ) = 1 + D^2 + D^3 + D^5 + D^6, then
                  cc_encoder -> 1111001 1011011, the bit length is 
                  encoder_number * ( memory_length + 1 )
   initial_state --> the cc encoder initial state 
   depth_of_trellis --> the Viterbi decoder's trellis depth length'
   
   decoded_bit <--
 * --------------------------------------------------------------------------
 */
{
  /** loop control variable **/	
  uint32_t i;
  uint32_t j;	
  uint32_t k;	
  /** the encoder register state number: = 2 ^ memory_length **/    
  uint8_t states_number;
  /** variables for trellis generation **/    
  uint8_t *from_state0;
  uint8_t *from_state1;
  uint8_t *to_state0;
  uint8_t *to_state1;
  uint8_t *output_bit0;
  uint8_t *output_bit1;
  /** variables for state store **/
  uint8_t *state_history;
  uint8_t *state_sequence;
 
  uint32_t *accum_distance_metric;
  uint32_t *accum_distance_metric_history;
  uint32_t tmp_metric_0;
  uint32_t tmp_metric_1;
  uint8_t *encoding_bit0;
  uint8_t *encoding_bit1;    
  uint8_t *next_state;
  uint8_t *previous_state;
  uint8_t *tmp_state_0;
  uint8_t *tmp_state_1;
	uint8_t tmp_number;    
    
  uint32_t min_value;
  uint32_t min_index;
    
  states_number = (1 << memory_length);
    
  output_bit0 = (uint8_t *)malloc(sizeof(uint8_t) * states_number * encoder_number);
  if (output_bit0 == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
      printf("memory allocation error in viterbi_decoding...\r\n");
	  }
#endif
    
    return(1); 
  }
  
  output_bit1 = (uint8_t *)malloc(sizeof(uint8_t) * states_number * encoder_number);
  if (output_bit1 == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
      printf("memory allocation error in viterbi_decoding...\r\n");
	  }
#endif
    
    return(1); 
  }
  
  from_state0 = (uint8_t *)malloc(sizeof(uint8_t) * states_number * encoder_number);
  if (from_state0 == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
      printf("memory allocation error in viterbi_decoding...\r\n");
	  }
#endif
    
    return(1); 
  }
  
  from_state1 = (uint8_t *)malloc(sizeof(uint8_t) * states_number * encoder_number);
  if (from_state1 == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
      printf("memory allocation error in viterbi_decoding...\r\n");
	  }
#endif
    
    return(1); 
  }
  
  to_state0 = (uint8_t *)malloc(sizeof(uint8_t) * states_number * encoder_number);
  if (to_state0 == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
      printf("memory allocation error in viterbi_decoding...\r\n");
	  }
#endif
    
    return(1); 
  }
  
  to_state1 = (uint8_t *)malloc(sizeof(uint8_t) * states_number * encoder_number);
  if (to_state1 == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
      printf("memory allocation error in viterbi_decoding...\r\n");
	  }
#endif
    
    return(1); 
  }

  state_history = (uint8_t *)malloc(sizeof(uint8_t) * states_number *
                  (depth_of_trellis + 1));
  if (state_history == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
      printf("memory allocation error in viterbi_decoding...\r\n");
	  }
#endif
    
    return(1); 
  }

  state_sequence = (uint8_t *)malloc(sizeof(uint8_t) * states_number *
                  (depth_of_trellis + 1));
  if (state_sequence == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
      printf("memory allocation error in viterbi_decoding...\r\n");
	  }
#endif
    
    return(1); 
  }
  
  accum_distance_metric = (uint32_t *)malloc(sizeof(uint32_t) * states_number);
  if (accum_distance_metric == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
      printf("memory allocation error in viterbi_decoding...\r\n");
	  }
#endif
    
    return(1); 
  }
  
  accum_distance_metric_history = (uint32_t *)malloc(sizeof(uint32_t) * states_number);
  if (accum_distance_metric_history == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
      printf("memory allocation error in viterbi_decoding...\r\n");
	  }
#endif
    
    return(1);  
  }
  
  encoding_bit0 = (uint8_t *)malloc(sizeof(uint8_t) * states_number * encoder_number);
  if (encoding_bit0 == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
      printf("memory allocation error in viterbi_decoding...\r\n");
	  }
#endif
    
    return(1);  
  }
  
  encoding_bit1 = (uint8_t *)malloc(sizeof(uint8_t) * states_number * encoder_number);
  if (encoding_bit1 == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
      printf("memory allocation error in viterbi_decoding...\r\n");
	  }
#endif
    
    return(1);  
  }
  
  next_state = (uint8_t *)malloc(sizeof(uint8_t) * states_number * 2);
  if (next_state == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
      printf("memory allocation error in viterbi_decoding...\r\n");
	  }
#endif
    
    return(1);  
  }
  
  previous_state = (uint8_t *)malloc(sizeof(uint8_t) * states_number * 2);
  if (previous_state == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
      printf("memory allocation error in viterbi_decoding...\r\n");
	  }
#endif
    
    return(1);  
  }
  
  tmp_state_0 = (uint8_t *)malloc(sizeof(uint8_t) * states_number );
  if (tmp_state_0 == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
      printf("memory allocation error in viterbi_decoding...\r\n");
	  }
#endif
    
    return(1); 
  }
  
  tmp_state_1 = (uint8_t *)malloc(sizeof(uint8_t) * states_number);
  if (tmp_state_1 == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
      printf("memory allocation error in viterbi_decoding...\r\n");
	  }
#endif
    
    return(1);  
  }

      
  for(i = 0; i < encoder_number; i++)
  {
	  cc_trellis_table_generation(memory_length, cc_encoder + i * (memory_length + 1),
	       /**/ output_bit0 + i * states_number, output_bit1 + i * states_number, 
	       from_state0 + i * states_number, from_state1 + i * states_number, 
	       to_state0 + i * states_number, to_state1 + i * states_number );
  }	  
	  
  state_trellis_table(memory_length, /**/ next_state, previous_state);
    
  for(i = 0; i < states_number; i++)
  {
		for(j = 0; j < encoder_number; j++)
		{
		  *(encoding_bit0 + i * encoder_number + j) = 
		            *(output_bit0 + j * states_number + i);
		  *(encoding_bit1 + i * encoder_number + j ) = 
			  		    *(output_bit1 + j * states_number + i);
		}
  }
	  
  for(i = 0; i < states_number * depth_of_trellis; i++)
  {
	  *(state_history + i) = 0;
	  *(state_sequence + i) = 0;
  }
  for(i = 0; i < states_number; i++)
  {
	  *(accum_distance_metric_history + i) = 0;
	  *(accum_distance_metric + i) = 0;
	  *(state_history + i) = initial_state;
  }
	   
  *(tmp_state_0 + 0) = initial_state; 
  for(i = 0; i < memory_length; i++)
  {
 	  tmp_number = (1 << i);	  	  
	  	  
		for(j = 0; j < tmp_number; j++)
		{ 
		  *(tmp_state_1 + 2 * j + 0) = *(to_state0 + (*(tmp_state_0 + j)));
      *(tmp_state_1 + 2 * j + 1) = * (to_state1 + (*(tmp_state_0 + j)));
		}
			  
		for(j = 0; j < tmp_number; j++)
		{ 
		  *(state_sequence + (i + 1) * states_number + (*(tmp_state_1 + 2 * j + 0))) = 
			  	      (*(tmp_state_1 + 2 * j + 0));
		  *(state_sequence + (i + 1) * states_number + (*(tmp_state_1 + 2 * j + 1))) = 
			  	      (*(tmp_state_1 + 2 * j + 1));
	  	      
	    for ( k = 0; k < i + 1 ; k++ )
	    {
	      *(state_sequence + k * states_number + (*(tmp_state_1 + 2 * j + 0))) = 
	      	   	       *(state_history + k * states_number + (*(tmp_state_0 + j)));
	      *(state_sequence + k * states_number + (*(tmp_state_1 + 2 * j + 1))) = 
	      	   	       *(state_history + k * states_number + (*(tmp_state_0 + j)));
	    }
	      	   
	    (*(accum_distance_metric + (*(tmp_state_1 + 2 * j + 0)))) = 
	      (*(accum_distance_metric_history + (*(tmp_state_0 + j)))) + 
	      distance_between_vector(encoding_bit0 + (*(tmp_state_0 + j)) * encoder_number, 
		    input_receivebit + i * encoder_number, encoder_number);    
			           
	    (*(accum_distance_metric + (*(tmp_state_1 + 2 * j + 1)))) = 
	      (*(accum_distance_metric_history + (*(tmp_state_0 + j)))) + 
	      distance_between_vector(encoding_bit1 + (*(tmp_state_0 + j)) * encoder_number, 
			  input_receivebit + i * encoder_number, encoder_number);         	   
		}
			         	  
		for(j = 0; j < tmp_number; j++)
		{          
		  *(accum_distance_metric_history + (*(tmp_state_1 + 2 * j + 0))) = 
			      *(accum_distance_metric + (*(tmp_state_1 + 2 * j + 0))); 
		  *(accum_distance_metric_history + (*(tmp_state_1 + 2 * j + 1))) = 
			      *(accum_distance_metric + (*(tmp_state_1 + 2 * j + 1))); 
      for(k = 0; k < i + 2 ; k++)
      {
      	*(state_history + k * states_number + (*(tmp_state_1 + 2 * j + 0))) = 
      	    *(state_sequence + k * states_number + (*(tmp_state_1 + 2 * j + 0)));
      	*(state_history + k * states_number + (*(tmp_state_1 + 2 * j + 1))) = 
      	    *(state_sequence + k * states_number + (*(tmp_state_1 + 2 * j + 1)));
      }
	  }
			  
		for(j = 0; j < tmp_number; j++)
		{ 
		  *(tmp_state_0 + 2 * j + 0) = *(tmp_state_1 + 2 * j + 0);
		  *(tmp_state_0 + 2 * j + 1) = *(tmp_state_1 + 2 * j + 1);
		}
  }
		
  for(i = memory_length; i < depth_of_trellis; i++)
  {
		for(j = 0; j < states_number / 2; j++)
		{
		  *(state_sequence + (i + 1) * states_number + 2 * j + 0) =  2 * j + 0;
		  tmp_metric_0 = distance_between_vector(encoding_bit0 + 
		                (*(previous_state + 4 * j + 0)) * encoder_number, 
			              input_receivebit + i * encoder_number, encoder_number) + 
			              (*(accum_distance_metric_history + 
			              (*(previous_state + 4 * j + 0))));
		  tmp_metric_1 = distance_between_vector(encoding_bit0 + 
		                (*(previous_state + 4 * j + 1)) * encoder_number, 
			              input_receivebit + i * encoder_number, encoder_number) +
			              (*(accum_distance_metric_history + 
			              (*(previous_state + 4 * j + 1))));
            
		  if(tmp_metric_0 < tmp_metric_1)
		  {
			  *(accum_distance_metric + 2 * j + 0) = tmp_metric_0;
			  for ( k = 0; k < i + 1 ; k++ )
			  {
			    *(state_sequence + k * states_number + 2 * j + 0) = 
			        *(state_history + k * states_number + (*(previous_state + 4 * j + 0)));
			  }
		  }
		  else
		  {
			  *(accum_distance_metric + 2 * j + 0) = tmp_metric_1;
			  for(k = 0; k < i + 1 ; k++)
			  {
			    *(state_sequence + k * states_number + 2 * j + 0) = 
			        *(state_history + k * states_number + (*(previous_state + 4 * j + 1)));
			  }
		  }        
			              					  
		  *(state_sequence + (i + 1) * states_number + 2 * j + 1) =  2 * j + 1;
      tmp_metric_0 = distance_between_vector(encoding_bit1 + 
              (*(previous_state + 4 * j + 2)) * encoder_number, 
			        input_receivebit + i * encoder_number, encoder_number) + 
			        (*(accum_distance_metric_history + (*(previous_state + 4 * j + 2))));
		  tmp_metric_1 = distance_between_vector(encoding_bit1 + 
		          (*(previous_state + 4 * j + 3)) * encoder_number, 
			        input_receivebit + i * encoder_number, encoder_number) +
			        (*(accum_distance_metric_history + (*(previous_state + 4 * j + 3))));
		  if(tmp_metric_0 < tmp_metric_1)
		  {
			  *(accum_distance_metric + 2 * j + 1) = tmp_metric_0;
			  for ( k = 0; k < i + 1 ; k++ )
			  {
			    *(state_sequence + k * states_number + 2 * j + 1) = 
			        *(state_history + k * states_number + (*(previous_state + 4 * j + 2)));
			  }
		  }
		  else
		  {
			  *(accum_distance_metric + 2 * j + 1) = tmp_metric_1;
			  for(k = 0; k < i + 1 ; k++)
			  {
			    *(state_sequence + k * states_number + 2 * j + 1) = 
			        *(state_history + k * states_number + (*(previous_state + 4 * j + 3)));
			  }
			      
		  }  
		}
			      
		for ( j = 0; j < states_number; j++ )
		{          
		  *(accum_distance_metric_history + j) = *(accum_distance_metric + j); 
      for(k = 0; k < i + 2 ; k++)
      {
      	*(state_history + k * states_number + j) = 
      	    	       *(state_sequence + k * states_number + j);
      }
	  }
  }
	  
  min_index_value(accum_distance_metric_history, states_number, 
	                /**/&min_value, &min_index);
	  	  
  for(i = 0; i < depth_of_trellis; i++)
  {
	  if((*(to_state0 + (*(state_sequence + i * states_number + min_index)))) == 
	  	  	   (*(state_sequence + (i + 1) * states_number + min_index)))
	  {
	    *(decoded_bit + i) = 0;
	  }
	  else if((*(to_state1 + (*(state_sequence + i * states_number + min_index)))) == 
	  	  	   (*(state_sequence + (i + 1) * states_number + min_index)))
	  {
	    *(decoded_bit + i) = 1;
	  }
	  else 
	  {
#if (IF_PRINT_LOG)    
			if (UART_LOG)
			{
        printf("There is something wrong in viterbi_decoding..., i = %ld\r\n", i);
			}
#endif
	    return(1);
	  }
  } 
	  	   
  free(output_bit0);
  free(output_bit1);
  free(from_state0);
  free(from_state1);
  free(to_state0);
  free(to_state1);
  free(state_history);
  free(state_sequence);
  free(accum_distance_metric);
  free(accum_distance_metric_history);        
  free(encoding_bit0);
  free(encoding_bit1);
  free(next_state);
  free(previous_state);
  free(tmp_state_0);
  free(tmp_state_1);
    
  return(0);
}

/***************************************************************************/
uint32_t ccencoder_byte_sequence_32(uint8_t *input_bytes, uint8_t encode_block_index,
         uint8_t *encoded_bytes)
/* -------------------------------------------------------------------------- 
*  CC encoder for length of byte 32	
* encode_block_index --> cc encoder byte length for each encoding block = 
*			            (1 << encode_block_index ) 			 
 -------------------------------------------------------------------------- */
{
  uint32_t i;
  uint8_t bytes_length = 32; 
  uint8_t bit_sequence[256];
  uint8_t bit_coded[512];
  uint8_t bit_coded_interleave[512];
  
  /* byte to bits map */
  char2bit_sequence(input_bytes, bytes_length, bit_sequence);
  /* 1/2 cc encoding for each bytes */
  for (i=0; i<(bytes_length >> encode_block_index); i++)
  {
  	encoder_halfrate (bit_sequence + i * (8 << encode_block_index), 
	        (8 << encode_block_index), 0, bit_coded + i * (16 << encode_block_index));
  }
  /* bit interleave */
  for (i=0; i<256; i++)
  {
  	*(bit_coded_interleave + i      ) = *(bit_coded + INTERLEAVE_PATTERN[i] * 2 + 0);
  	*(bit_coded_interleave + 256 + i) = *(bit_coded + INTERLEAVE_PATTERN[i] * 2 + 1);
  }
  /* bits to byte map */
  bit2char_sequence(bit_coded_interleave, bytes_length << 1, encoded_bytes);
  
  return(0);								 	
}

/***************************************************************************/
uint32_t ccdecoder_byte_sequence_32(uint8_t *input_bytes, uint8_t encode_block_index,
         uint8_t *decoded_bytes)
/* -------------------------------------------------------------------------- 
*  CC decoder for length of byte 32	
* encode_block_index --> cc encoder byte length for each encoding block = 
*			            (1 << encode_block_index ) 			 
 -------------------------------------------------------------------------- */
{
  uint32_t i;
  uint8_t bytes_length = 32; 
  uint8_t bit_sequence[256];
  uint8_t bit_coded[512];
  uint8_t bit_coded_interleave[512];
   
  /* byte to bit map */
  char2bit_sequence(input_bytes, 64, bit_coded_interleave);
  /* bit de-interleave */
  for (i=0; i<256; i++)
  {
  	*(bit_coded + INTERLEAVE_PATTERN[i] * 2    ) = *(bit_coded_interleave +       i);
  	*(bit_coded + INTERLEAVE_PATTERN[i] * 2 + 1) = *(bit_coded_interleave + 256 + i);
   }
  /* 1/2 cc decode for each bytes */
  for (i=0; i<(bytes_length >> encode_block_index); i++)
  {
 	decoder_hallfrate(bit_coded + i * (16 << encode_block_index), 
	         (8 << encode_block_index), 0, bit_sequence + i * (8 << encode_block_index));
  }
  /* bits to byte map */
  bit2char_sequence(bit_sequence, bytes_length, decoded_bytes);
  
  return(0);								 	
}


/***************************************************************************/
uint32_t ccencoder_byte_sequence_64(uint8_t *input_bytes, uint8_t encode_block_index,
         uint8_t *encoded_bytes)
/* -------------------------------------------------------------------------- 
*									 
 -------------------------------------------------------------------------- */
{
  uint32_t i;
  uint8_t bytes_length = 64; 
  uint8_t bit_sequence[512];
  uint8_t bit_coded[1024];
  uint8_t bit_coded_interleave[1024];

  /* byte to bits map */
  char2bit_sequence(input_bytes, bytes_length, bit_sequence);
  /* 1/2 cc encoding for each bytes */
  for (i=0; i<(bytes_length >> encode_block_index); i++)
  {
  	encoder_halfrate (bit_sequence + i * (8 << encode_block_index), 
	        (8 << encode_block_index), 0, bit_coded + i * (16 << encode_block_index));
  }
  /* bit interleave */
  for (i=0; i<256; i++)
  {
  	*(bit_coded_interleave + i      ) = *(bit_coded + INTERLEAVE_PATTERN[i] * 4 + 0);
  	*(bit_coded_interleave + 256 + i) = *(bit_coded + INTERLEAVE_PATTERN[i] * 4 + 1);
  	*(bit_coded_interleave + 512 + i) = *(bit_coded + INTERLEAVE_PATTERN[i] * 4 + 2);
  	*(bit_coded_interleave + 768 + i) = *(bit_coded + INTERLEAVE_PATTERN[i] * 4 + 3);
  }
  /* bits to byte map */
  bit2char_sequence(bit_coded_interleave, bytes_length << 1, encoded_bytes);
  
  return(0);								 	
}

/***************************************************************************/
uint32_t ccdecoder_byte_sequence_64(uint8_t *input_bytes, uint8_t encode_block_index,
         uint8_t *decoded_bytes)
/* -------------------------------------------------------------------------- 
*									 
 -------------------------------------------------------------------------- */
{
  uint32_t i;
  uint8_t bytes_length = 64; 
  uint8_t bit_sequence[512];
  uint8_t bit_coded[1024];
  uint8_t bit_coded_interleave[1024];
  
  /* byte to bit map */
  char2bit_sequence(input_bytes, bytes_length << 1, bit_coded_interleave);
  /* bit de-interleave */
  for (i=0; i<256; i++)
  {
  	*(bit_coded + INTERLEAVE_PATTERN[i] * 4    ) = *(bit_coded_interleave +       i);
  	*(bit_coded + INTERLEAVE_PATTERN[i] * 4 + 1) = *(bit_coded_interleave + 256 + i);
  	*(bit_coded + INTERLEAVE_PATTERN[i] * 4 + 2) = *(bit_coded_interleave + 512 + i);
  	*(bit_coded + INTERLEAVE_PATTERN[i] * 4 + 3) = *(bit_coded_interleave + 768 + i);
  }
  /* 1/2 cc decode for each bytes */
  for (i=0; i<(bytes_length >> encode_block_index); i++)
  {
 	decoder_hallfrate(bit_coded + i * (16 << encode_block_index), 
	         (8 << encode_block_index), 0, bit_sequence + i * (8 << encode_block_index));
  }
  /* bits to byte map */
  bit2char_sequence(bit_sequence, bytes_length, decoded_bytes);
  
  return(0);								 	
}
#endif

