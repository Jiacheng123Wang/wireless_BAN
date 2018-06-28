#include "define_config.h"
#include "comm_phone_command.h"
#include "watch_wireless_interface.h"
#include "comm_wireless_interface.h"
#include "comm_UICC_terminal_interface.h"
#include "flash_file_address.h"
#include "comm_misc.h"
#include "file_data_usim_extern.h"

uint8_t AUTH_CHALL_DATA[KEY_LENGTH];
uint8_t PHONE_ADD_LOGICAL_TABLE[8] = {0, 0, 0, 0, 0, 0, 0, 0};
uint8_t WATCH_RECEIVED_COMMAND[256];
volatile uint8_t FLAG_DEFAULT_RECEIVER = 1;
volatile uint8_t FLAG_RECEIVE_CALL_UPDATE = 0; 
/* sensor data of body temperature */
uint8_t BODY_TEMPERATURE[256];
/* sensor data of heart rate */
uint8_t HEART_RATE[256];
/* sensor data of ECG value */
uint8_t ECG_VALUE[256];
/* sensor data of blood presure */
uint8_t BLOOD_PRESURE[256];
/* sensor data of blood oxygen */
uint8_t BLOOD_OXYGEN[256];

extern uint8_t FLAG_USIM_INITIALIZATION;   

extern uint8_t INFO_BYTE[PACKET_MAX_SIZE + 20];
extern uint8_t CHECK_BIT[PACKET_MAX_SIZE + 20];
extern uint8_t PHONE_ADD_LOGICAL;
extern uint8_t DATA_CARRIER_FREQ; 
extern uint8_t USER_PASSWORD[KEY_LENGTH];
extern uint8_t SYSTEM_KEY[KEY_LENGTH];
extern uint8_t MASK_RECEIVE_CALL;
extern uint8_t FREQ_BIN_PATTEN[83];
extern uint8_t AUTHENTICATE_RESULT[KEY_LENGTH];
extern volatile uint8_t WIRELESS_SIM_ON;
extern volatile uint8_t WATCH_CALL_ON;
extern volatile uint8_t WATCH_CALL_ON_FLASH;
extern volatile uint32_t CONNECTION_STATE;
extern uint8_t PASSWORD_SHUFFLE[16];	 
extern uint8_t SYSTEM_KEY_SHUFFLE[16];	 
#if (IF_CC_CODING)
  uint8_t CC_CODED_BYTE[256];	   
#endif   
extern uint8_t UART_LOG;

/********************************************************************************/
void main_watch_phone_sim_wireless(void)
/*--------------------------------------------------------------------------------
| phone command over the air between phone and watch
| 0x88: authentication 
| 0xbb: wireless get the SIM EF data 
|
--------------------------------------------------------------------------------*/
{
  if (received_command_sim(WATCH_RECEIVED_COMMAND, CHECK_BIT))
  {
    *(WATCH_RECEIVED_COMMAND + 2) = 0x00;
  }
	  
  switch(*(WATCH_RECEIVED_COMMAND + 2))
  {
    // case 0x88:
    //   authenticate_SIM_response(ETU_TICKS_PHONE);
    //   break;
    //
    // case 0xbb:
    //   sim_command_send(WATCH_RECEIVED_COMMAND);
    //   break;
      
    case 0xcc:
      watch_command_receive_data(WATCH_RECEIVED_COMMAND);
      break;                
      
    default:
#if (IF_PRINT_LOG)
		  if (UART_LOG)
		  {
        printf("\r\n**********************************************************\r\n");      
        printf("Not supported USIM command,  command=%x,  skipped......\r\n", *(WATCH_RECEIVED_COMMAND + 2));
      }
#endif
      break;
  }
  
  /* re-load watch dog request register */
  NRF_WDT->RR[0] = 0x6E524635;
}      

/********************************************************************************/
void sim_connection_state_check(void)
/*--------------------------------------------------------------------------------
| radio link connection status check at SIM side, watch listen to if there is an
| access resuest from phone  
|
--------------------------------------------------------------------------------*/
{
  uint32_t conncetion_listen;
  static uint32_t volatile DISCONNECTION_COUNTER = 0;
  static uint32_t volatile CONNECTION_COUNTER = 0;
  
  if (WIRELESS_SIM_ON)
  {
    conncetion_listen = connection_listening_sim(  );
    
    /* if connection request received, and receive correct */
    if (conncetion_listen == 0)  
    {  
	  /* clear disconnection counter */	
      DISCONNECTION_COUNTER = 0;
	  /* increase connection counter */	
      CONNECTION_COUNTER++;
	  
      /* move b0 to b1, mark the previous connection state */
      if (CONNECTION_STATE & 0x1)
      {
        CONNECTION_STATE |= 0x2;
      }
      else
      {
        CONNECTION_STATE &= 0xFFFFFFFD;
      }
	  /* mark b0 = 1 */
      CONNECTION_STATE |= 0x1;
	  
	  /* phone command by wireless */
      main_watch_phone_sim_wireless( );
    }
    else if (conncetion_listen == 2)  
    {    
	  /* clear disconnection counter */	
      DISCONNECTION_COUNTER = 0;
	  /* increase connection counter */	
      CONNECTION_COUNTER++;
	  
      /* move b0 to b1, mark the previous connection state */
      if (CONNECTION_STATE & 0x1)
      {
        CONNECTION_STATE |= 0x2;
      }
      else
      {
        CONNECTION_STATE &= 0xFFFFFFFD;
      }
	  /* mark b0 = 1 */
      CONNECTION_STATE |= 0x1;
    }
    else
    {
      DISCONNECTION_COUNTER++;
    }
    
	if (conncetion_listen != 3)
	{
      stop_oscillator_16m( );
    }
  }
  else
  {
    DISCONNECTION_COUNTER++;
#if (IF_PRINT_LOG)  
		if (UART_LOG)
		{
      printf("++++++++++++++++++++ Wireless Off, not receive ++++++++++++++++++++ \r\n");
    }
#endif
  }
  
  /* disconnection counter overflow, USIM assume there is no connection between phone-USIM */
  if (DISCONNECTION_COUNTER >= SIM_DISCONNECT_TIME)
  {
  	/* clear the connection and disconnection counter */    
    DISCONNECTION_COUNTER = 0;
    CONNECTION_COUNTER = 0; 
	  
    /* move b0 to b1, mark the previous connection state */
    if (CONNECTION_STATE & 0x1)
    {
      CONNECTION_STATE |= 0x2;
    }
    else
    {
      CONNECTION_STATE &= 0xFFFFFFFD;
    }
	/* mark b0 = 0 */
    CONNECTION_STATE &= 0xFFFFFFFE;   
      
    /* from connection to disconnection */
    if ((CONNECTION_STATE & 0x3) == 2)
    {
      /* watch NAA access to the network */
	  /* set WATCH_CALL_ON as 1, watch perform authenticate from SIM connect to the network */
      WATCH_CALL_ON = 1;
      /* mark the USAT bit for reset NAA, let watch access the network */      
      CONNECTION_STATE |= 0x4;
    } 
	/* if watch call off, since no connection between phone-watch, turn on watch call */
	if (WATCH_CALL_ON == 0)
	{
      /* watch NAA access to the network */
  	  /* set WATCH_CALL_ON as 1, watch perform authenticate from SIM connect to the network */
      WATCH_CALL_ON = 1;
      /* mark the USAT bit for reset NAA, let watch access the network */      
      CONNECTION_STATE |= 0x4;		
	}

#if (IF_PRINT_LOG)  
	  if (UART_LOG)
	  {
      if (WIRELESS_SIM_ON)
      {
        printf("==================== Wireless On, Not connected, CONNECTION_STATE = %ld, ==================== \r\n", CONNECTION_STATE & 0x7);
      }
      else
      {
        printf("++++++++++++++++++++ Wireless Off, Not connected, CONNECTION_STATE = %ld, ++++++++++++++++++++ \r\n", CONNECTION_STATE & 0x7);
      }
	  }
#endif
  }
  
  /* connection counter overflow, USIM assume there is connection between phone-USIM */
  if (CONNECTION_COUNTER)
  {   
	/* clear the counter */   
    CONNECTION_COUNTER = 0;
      
    /* from disconnection to connection */
    if ((CONNECTION_STATE & 0x3) == 1)
    {
       /* watch NAA the network, according user WATCH_CALL_ON setting */
	   WATCH_CALL_ON = (*((uint8_t *)FLAG_WATCH_CALL_FLASH_ADDR) & 1);
       /* watch NAA access to the network */
       CONNECTION_STATE |= 0x4;
    }
	
	if (WATCH_CALL_ON != WATCH_CALL_ON_FLASH)
	{
       /* watch NAA the network, according user WATCH_CALL_ON setting */
 	   WATCH_CALL_ON = WATCH_CALL_ON_FLASH;
       /* watch NAA access to the network */
       CONNECTION_STATE |= 0x4;		
	}
	
#if (IF_PRINT_LOG)  
	if (UART_LOG)
	{
    printf("==================== Wireless On, Connected, CONNECTION_STATE = %ld ==================== \r\n", CONNECTION_STATE & 0x7);
  }
#endif
  }
}

/********************************************************************************/
uint32_t connection_listening_sim(void)
/*--------------------------------------------------------------------------------
| radio link connection listen at SIM side  
|
--------------------------------------------------------------------------------*/
{
#if (IF_CC_CODING)
  uint8_t rx_connect_request_data[CONNECTION_REQUEST_BYTE];
#else	  
  uint8_t rx_connect_request_data[32];
#endif  
  uint32_t tx_logical_address;
  uint32_t rx_logical_address;
  uint32_t i;
  uint8_t connection_type;
  uint8_t connection_carrier;
  uint8_t flag_bytes[4] = {0, 0, 0, 0};  
  uint8_t user_password_rx[KEY_LENGTH];  
  
  if (start_oscillator_16m( ))
  {
    return (3);
  }
  
  rx_logical_address = 0xff; 
  
  /* if connection request received */
#if (IF_CC_CODING)
  *CC_CODED_BYTE = 0;
  if ((!rx_radio_config_parameter(CC_CODED_BYTE, FREQUENCY_CARRIER_ACCESS, rx_logical_address, 
        SIM_LISTEN_TIME)))  
  {
	/* cc decoding */
    ccdecoder_byte_sequence_32(CC_CODED_BYTE + 1, ENCODE_BLOCK_INDEX,  rx_connect_request_data); 
#else
  *CHECK_BIT = 0;	
  if ((!rx_radio_config_parameter(CHECK_BIT, FREQUENCY_CARRIER_ACCESS, rx_logical_address, 
          SIM_LISTEN_TIME)))  
  {
    for (i=0; i<32; i++)
    {
      *(rx_connect_request_data + i) = CHECK_BIT[i];
    }
#endif
	  			    
    /* simple receiver error check */	    	 
	if ((*(rx_connect_request_data + 1) != *(rx_connect_request_data + 2)) || 
		(*(rx_connect_request_data + 1) != *(rx_connect_request_data + 3)))
	{
#if (IF_PRINT_LOG)
		if (UART_LOG)
		{
      printf("\r\n------------------------- Logical address receive error in connection_listening_SIM -------------------------\r\n");
      printf_log_rx(CONNECTION_REQUEST_BYTE, rx_connect_request_data);
    }
#endif
      return(1);		
	}
	if ((*(rx_connect_request_data + 4) != *(rx_connect_request_data + 5)) ||
		(*(rx_connect_request_data + 4) != *(rx_connect_request_data + 6)))
	{
#if (IF_PRINT_LOG)
		if (UART_LOG)
		{
      printf("\r\n------------------------- Connection request type receive error in connection_listening_SIM -------------------------\r\n");
      printf_log_rx(CONNECTION_REQUEST_BYTE, rx_connect_request_data);
    }
#endif
      return(1);		
	}
	if ((*(rx_connect_request_data + 7) != *(rx_connect_request_data + 8)) ||
		(*(rx_connect_request_data + 7) != *(rx_connect_request_data + 9)))
	{
#if (IF_PRINT_LOG)
		if (UART_LOG)
		{
      printf("\r\n------------------------- Connection request frequency carrier error in connection_listening_SIM -------------------------\r\n");
      printf_log_rx(CONNECTION_REQUEST_BYTE, rx_connect_request_data);
    }
#endif
      return(1);		
	}
	
	for (i=0; i<KEY_LENGTH; i++) 
	{
	  user_password_rx[PASSWORD_SHUFFLE[i]] = *(rx_connect_request_data + 10 + i) ^ 
		                                       SYSTEM_KEY[SYSTEM_KEY_SHUFFLE[i]];	
	} 
    /* wireless UICC-terminal interface password check. if fails, return with error code 1 */
    if (password_check(KEY_LENGTH, USER_PASSWORD, user_password_rx))
    {
#if (IF_PRINT_LOG)
			if (UART_LOG)
			{
        printf("\r\n------------------------- password check error in connection_listening_SIM, Logical address = %d -------------------------\r\n", rx_connect_request_data[1]);
        printf("Received Password: ");
        for (i=0; i<16; i++)
        {
          printf("%x,  ", *(user_password_rx + i));
        }
        printf("\r\n");
			}
#endif
      return(1);
    }
    
    /* save the connection type */
    connection_type = rx_connect_request_data[4];
    /* save the connection frequency carrier */	
	connection_carrier = rx_connect_request_data[7];
    
    /* the connection type is to update the defaut call receiver phone */
    if (connection_type == 0x22)
    {
      /* mark the default call receiving phone */
      MASK_RECEIVE_CALL = (0x1 << (PHONE_ADD_LOGICAL & 7));
      /* save the default call receiver mask byte to flash */
      flag_bytes[0]  = MASK_RECEIVE_CALL;
      flash_byte_string_update((uint32_t *)SWAP_FLASH_ADDR, (uint32_t *)USER_CONFIG_FLASH_ADDR, 10, 4, flag_bytes); 
    }          
    
    /* save the logical address to send ACK message */
    tx_logical_address = (rx_connect_request_data[1] & 7);
    
    /* The default initial phone logical address is 0, if a phone use logical address 0
    |  to request access, allocate a different logical address between 1 : 7 */
    if ((rx_connect_request_data[1] & 7) == 0)
    {      
      rx_connect_request_data[1] = (phone_logical_address_search(10) & 7);
      rx_connect_request_data[2] = rx_connect_request_data[1];
      rx_connect_request_data[3] = rx_connect_request_data[1];
    }
      
    /* search the best carrier frequency for data transmission */
    rx_connect_request_data[4] = radio_carrier_search(CARRIER_FREQ_SEARCH);
    rx_connect_request_data[5] = rx_connect_request_data[4];
    rx_connect_request_data[6] = rx_connect_request_data[4];
    /* save the carrier frequency for data transmission between SIM-Phone */
    DATA_CARRIER_FREQ = rx_connect_request_data[4];
    	
	/* set mark bit to phone SIM reset */
    if (FLAG_RECEIVE_CALL_UPDATE && (rx_connect_request_data[1] != 7))
    {
      rx_connect_request_data[7] = 0x33;
      rx_connect_request_data[8] = 0x33;
      rx_connect_request_data[9] = 0x33;
      FLAG_RECEIVE_CALL_UPDATE = 0;
    }
	else
	{
      rx_connect_request_data[7] = 0;
      rx_connect_request_data[8] = 0;
      rx_connect_request_data[9] = 0;		
	}
    
    /* get authentication challenge data for phone connection request */
    for (i=0; i<KEY_LENGTH; i++)
    {
      *(rx_connect_request_data + 10 + i) = AUTH_CHALL_DATA[i];
    }
	/* transmit byte length */
	*(rx_connect_request_data + 0) = 25;
	
#if (IF_CC_CODING)
    /* connection response information byte length, coding rate = 1/2 */
    *(CC_CODED_BYTE + 0) = (CONNECTION_REQUEST_BYTE * 2);
    /* cc encoding for connection response message trfansmission */
    ccencoder_byte_sequence_32(rx_connect_request_data, ENCODE_BLOCK_INDEX, CC_CODED_BYTE + 1);
#endif
		
    radio_carrier_detection(MAX_CD_TIME_MS, TX_RSSI_THRED, FREQUENCY_CARRIER_ACCESS);
    /* if connection response transmission successfully, return with successful code */
#if (IF_CC_CODING)
    if (!tx_radio_config_parameter(CC_CODED_BYTE, connection_carrier, 
             tx_logical_address, ACK_TX_TIME, POWER_LEVEL_ACK))
    {
#else
	if (!tx_radio_config_parameter(rx_connect_request_data, connection_carrier, 
	         tx_logical_address, ACK_TX_TIME, POWER_LEVEL_ACK))
	{
#endif				
      PHONE_ADD_LOGICAL = (rx_connect_request_data[1] & 7);
               
#if (IF_PRINT_LOG)
			if (UART_LOG)
			{
        printf("\r\n--------------------- Data carrier frequency = %d, Logical address = %d, Connection type = %d,   -----------------------\r\n", DATA_CARRIER_FREQ, PHONE_ADD_LOGICAL, connection_type);
			}
#endif
      /* connection request for wireless link mantenance, connection_type = 0x11, return 2 */
      if (connection_type) 
      {
        return(2);
      }
	  /* connection request for data transmission, connection_type = 0x0 */
      else  
      {    
        /* authentication algorithm running result */
        wireless_sim_authentication_algorithm(USER_PASSWORD, SYSTEM_KEY, rx_connect_request_data + 10, 
		                                      AUTHENTICATE_RESULT);
        /* generate random challenge data for next use */
        random_vector_generate(AUTH_CHALL_DATA, KEY_LENGTH);
        
        return(0);
      }
    }
    /* connection response message transmission fails, return with error code */
    else
    {      
      return(1);
    }
  }
  /* no connection request message received, return with error code */
  else
  {
    return(1);
  }
}

/********************************************************************************/
uint32_t radio_carrier_search(uint32_t max_serach_time_ms)
/*--------------------------------------------------------------------------------
| search a radio carrier according the received RSSI
|
--------------------------------------------------------------------------------*/
{
  uint32_t i;
  uint32_t threshold = CARRIER_FREQ_THRE;
  uint32_t default_carrier_frequency = 83;
  uint32_t measured_RSSI;
  static uint32_t frequency_bin = 0;
  uint32_t initial_timer;

  /* get the initial real time counter */
  initial_timer = NRF_RTC0->COUNTER;
  
  while (1)
  {
    for (i=0; i<83; i++)
    {
      /* frequency hopping */
      if (IF_FREQ_HOP)
      {  
        frequency_bin++;
        if (frequency_bin == 83)
        {
          frequency_bin = 0;
        }
      }
      /* no frequency hopping, use default carrier frequency */
      else
      {        
        return(default_carrier_frequency);  
      }
      
      measured_RSSI = radio_rssi_measurement(FREQ_BIN_PATTEN[frequency_bin]);
      if (measured_RSSI > threshold)
      {
        return(FREQ_BIN_PATTEN[frequency_bin]);
      } 
    }
    
    /* if maximal search reached, break out, use default carrier frequency */
    if (((NRF_RTC0->COUNTER - initial_timer) > max_serach_time_ms) || (NRF_RTC0->COUNTER < initial_timer))
    {
      break;
    }
    /* decrease the threshold, search again */
    else
    {
      threshold -= 3;
    }    
  }  

  /* use default carrier frequency */
  return(default_carrier_frequency);  
}

/********************************************************************************/
uint8_t phone_logical_address_search(uint8_t max_serach_time_ms)
/*--------------------------------------------------------------------------------
| search logical address for wireless phone-SIM interface
|
--------------------------------------------------------------------------------*/
{
  uint8_t i;
  uint8_t default_logical_address = 0;
  uint8_t n_phone_coadd = 1;
  uint32_t initial_timer;

  /* get the initial real time counter */
  initial_timer = NRF_RTC0->COUNTER;
   
  while (1)
  {
    for (i=1; i<8; i++)
    {
      if (PHONE_ADD_LOGICAL_TABLE[i] < n_phone_coadd)
      {
        PHONE_ADD_LOGICAL_TABLE[i]++;
        return (i);        
      }
    }
    /* if maximal search reached, break out, use default phone logical address */
    if (((NRF_RTC0->COUNTER - initial_timer) > max_serach_time_ms) || (NRF_RTC0->COUNTER < initial_timer))
    {
      break;
    }
    else
    {
      n_phone_coadd++;  
    }
  }
  
  /* use default phone logical address */
  return (default_logical_address);  
}

/********************************************************************************/
uint32_t received_command_sim(uint8_t *bytes_command, uint8_t *parity_bit)
/*--------------------------------------------------------------------------------
| watch receive the phone command over the air
|
--------------------------------------------------------------------------------*/
{
  uint32_t length_command = 5;
  
#if (IF_CC_CODING)
  if (receive_info_check(&length_command, CC_CODED_BYTE))
  {
    return(1);
  }
  ccdecoder_byte_sequence_64(CC_CODED_BYTE + 1, ENCODE_BLOCK_INDEX, bytes_command);
  length_command = *(bytes_command + 0); 
#else
  if (receive_info_check(&length_command, bytes_command))
  {
    return(1);
  }
#endif
  
#if (IF_PRINT_LOG)
	if (UART_LOG)
	{
    printf_log_rx(length_command, bytes_command + 1);
  }
#endif

  /* wireless UICC-terminal interface authentication check */
  if (password_check(KEY_LENGTH, bytes_command + 6, AUTHENTICATE_RESULT))
  {
#if (IF_PRINT_LOG)
		if (UART_LOG)
		{
      printf("\r\n------------------------- authentication error in received_command_sim -------------------------\r\n");
      printf("transmitted: ");
      printf_log_tx(KEY_LENGTH, AUTHENTICATE_RESULT);
		}
#endif

    return(1);
  }
  
  return(0);
}

/********************************************************************************/
uint32_t watch_command_receive_data(uint8_t *received_data)
/*--------------------------------------------------------------------------------
| watch receive the data send from phone wireless
|
--------------------------------------------------------------------------------*/
{
  uint8_t data_received_confirm[6] = {5, 0x11, 0x22, 0x33, 0x44, 0x55};
  uint8_t i;
    
  switch(*(received_data + 5))
  {	  	  
    case WIRELESS_DATA_TYPE_SENSOR_DATA:
		{
      /* re-load watch dog request register */
      NRF_WDT->RR[0] = 0x6E524635;
  
      if (transmit_info_check(data_received_confirm[0], data_received_confirm))
  	  {
  #if (IF_PRINT_LOG)
				if (UART_LOG)
				{
  	      printf("\r\n---------------- Time out for data package transmission in watch_command_receive --------------------\r\n");    
        }
  #endif  	  	
  #if (PRINT_DELAY_MS)
  	    nrf_delay_ms(PRINT_DELAY_MS);
  #endif
  	    return(1);
  	  }
			
			switch (*(received_data + 24))
			{
				case SENSOR_DATA_TYPE_BODY_TEMPERATURE:
				{
					for (i=0; i<*(received_data + 23) + 1; i++)
					{
						BODY_TEMPERATURE[i] = *(received_data + 23 + i);
					}
					break;
				}
					
				case SENSOR_DATA_TYPE_HEART_RATE:
				{
					for (i=0; i<*(received_data + 23) + 1; i++)
					{
						HEART_RATE[i] = *(received_data + 23 + i);
					}
					break;
				}
					
				case SENSOR_DATA_TYPE_ECG_VALUE:
				{
					for (i=0; i<*(received_data + 23) + 1; i++)
					{
						ECG_VALUE[i] = *(received_data + 23 + i);
					}
					break;
				}
					
				case SENSOR_DATA_TYPE_BLOOD_PRESURE:
				{
					for (i=0; i<*(received_data + 23) + 1; i++)
					{
						BLOOD_PRESURE[i] = *(received_data + 23 + i);
					}
					break;
				}
					
				case SENSOR_DATA_TYPE_BLOOD_OXYGEN:
				{
					for (i=0; i<*(received_data + 23) + 1; i++)
					{
						BLOOD_OXYGEN[i] = *(received_data + 23 + i);
					}
					break;
				}
					
			}  	  	  	  
      break; 
		}  
	  	  
	  default:	  
	  {
#if (IF_PRINT_LOG)
			if (UART_LOG)
			{
        printf("\r\n---------------- not supported phone command data type --------------------\r\n");    
      }
#endif  
	    break;	
		}  			     
  }
  
  return(0);
}





