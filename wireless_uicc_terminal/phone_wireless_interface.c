#include "define_config.h"
#include "radio_config_update.h"
#include "phone_wireless_interface.h"
#include "phone_command_sim.h"
#include "comm_misc.h"
#include "comm_wireless_interface.h"
#include "comm_phone_command.h"
#include "comm_UICC_terminal_interface.h"
#include "flash_file_address.h"
#include "file_data_usim_extern.h"

extern uint8_t PHONE_ADD_LOGICAL;
extern uint8_t USER_PASSWORD[KEY_LENGTH];
extern uint8_t DATA_CARRIER_FREQ;
extern uint8_t AUTHENTICATE_RESULT[KEY_LENGTH];
extern uint8_t SYSTEM_KEY[KEY_LENGTH];
extern uint8_t PHONE_COMMAND[6];
extern uint8_t INFO_BYTE[PACKET_MAX_SIZE + 20];
extern uint8_t CHECK_BIT[PACKET_MAX_SIZE + 20];
extern volatile uint8_t FETCH_COMMAND_MENU;
extern volatile uint8_t WIRELESS_SIM_ON;
extern volatile uint32_t USAT_BYTE_LENGTH_BACK;
extern volatile uint32_t CONNECTION_STATE;
extern uint8_t SIM_ON_CONNECTED_STRING_PHONE[];  
extern uint8_t SIM_ON_DISCONNECTED_STRING_PHONE[];  
extern volatile uint8_t IF_CONNECTED_PHONE; 
extern uint8_t PASSWORD_SHUFFLE[16];	 
extern uint8_t SYSTEM_KEY_SHUFFLE[16];	
#if (IF_CC_CODING)
  uint8_t CC_CODED_BYTE[256];	   
#endif   
extern volatile uint32_t ETU_TICKS_PHONE;
extern uint8_t UART_LOG;

/********************************************************************************/
void main_smart_phone_sim_local_wireless(void)
/*--------------------------------------------------------------------------------
| phone command between phone-SIM interface at smart phone side locally and 
| over the air between phone and watch
| wireless phine-watch:    0x88: authentication 
| local phone-SIM command: all other phone cammand
|
--------------------------------------------------------------------------------*/
{
  switch(*(PHONE_COMMAND + 2))
  {
    case 0xa4:
      phone_command_select_mandatory(PHONE_COMMAND, ETU_TICKS_PHONE);
      break;    
        
    case 0xb0:
      phone_command_read_binary_mandatory(PHONE_COMMAND, ETU_TICKS_PHONE);
      break;
    
    case 0x10:
      phone_command_terminal_profile(PHONE_COMMAND, ETU_TICKS_PHONE, PIN_DATA_PHONE);
      break;
    
    case 0x14:
      phone_command_terminal_response(PHONE_COMMAND, ETU_TICKS_PHONE);
      break;
    
    case 0xb2:
      phone_command_read_record_mandatory(PHONE_COMMAND, ETU_TICKS_PHONE);
      break;
    
    case 0x2c:
      phone_command_unblock_pin(PHONE_COMMAND, ETU_TICKS_PHONE);
      break;
    
    case 0x20:
      /* initial USAT menu setting */
      USAT_BYTE_LENGTH_BACK = USAT_BYTE_LENGTH_BACK_INIT;
      FETCH_COMMAND_MENU = 250;
  
      phone_command_verify_20(PHONE_COMMAND, ETU_TICKS_PHONE);
      break;
    
    case 0xa2:
      phone_command_search_record(PHONE_COMMAND, ETU_TICKS_PHONE);
      break;
    
    case 0xf2:
      phone_command_f2_response(PHONE_COMMAND, ETU_TICKS_PHONE, PIN_DATA_PHONE);
      break;
    
    case 0x12:
      phone_command_fetch_all(PHONE_COMMAND, ETU_TICKS_PHONE, PIN_DATA_PHONE);
      break;
    
    case 0xd6:
      phone_command_update_binary_mandatory(PHONE_COMMAND, ETU_TICKS_PHONE);
      break;
    
    case 0x88:
      authenticate_SIM_response(ETU_TICKS_PHONE);
      break;

    case 0xc2:
      phone_command_envelope(PHONE_COMMAND, ETU_TICKS_PHONE);
      break;
      
    case 0x28:
      phone_command_PIN(PHONE_COMMAND, ETU_TICKS_PHONE);
      break;
      
    case 0x26:
      phone_command_PIN(PHONE_COMMAND, ETU_TICKS_PHONE);
      break;
      
    case 0x24:
      phone_command_PIN(PHONE_COMMAND, ETU_TICKS_PHONE);
      break;      
    
    case 0xdc:
      phone_command_update_record(PHONE_COMMAND, ETU_TICKS_PHONE);
      break;
	   
    case 0x70:
	  phone_command_manage_channel(PHONE_COMMAND, ETU_TICKS_PHONE);
	  break;
    
    default:
      /* not a phone command, do nothing */ 
#if (IF_PRINT_LOG)
		  if (UART_LOG)
		  {
        printf("\r\n**********************************************************\r\n");      
        printf("Not supported USIM command,  command=%x,  skipped......\r\n", *(PHONE_COMMAND + 2));
			}
#endif
      break;  
  }
  
  /* re-load watch dog request register */
  NRF_WDT->RR[0] = 0x6E524635;
  /* clear RTC1 */
  NRF_RTC1->TASKS_CLEAR = 1;  
  while (NRF_RTC1->COUNTER)
  {
    NRF_RTC1->TASKS_CLEAR = 1;
  }	
}

/********************************************************************************/
void phone_connection_state_check(void)
/*--------------------------------------------------------------------------------
| radio link connection status check at phone side, phone transmit an access request 
| and listen the ACK feedback to check if there is radio link between phone and watch  
|
--------------------------------------------------------------------------------*/
{
  static uint32_t volatile DISCONNECTION_COUNTER = 0;
  static uint32_t volatile CONNECTION_COUNTER = 0;
  uint8_t return_connection_request;
    
  if (WIRELESS_SIM_ON)
  {
	  return_connection_request = connection_request_phone(0x11);
    if (return_connection_request)
    {
      DISCONNECTION_COUNTER++;
    }
    else
    {
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
	  
	    /* clear disconnection counter */
      DISCONNECTION_COUNTER = 0;
	    /* increase connection counter */
      CONNECTION_COUNTER++;	      
    }
    
	  if (return_connection_request != 3)
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
      printf("++++++++++++++++++++ Wireless Off, no request transmitted ++++++++++++++++++++ \r\n");
		}
#endif
  } 
    
  /* disconnection counter overflow, phone assume there is no connection between phone-USIM */
  if (DISCONNECTION_COUNTER >= PHONE_DISCONNECT_TIME)
  {
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
	
	  /* clear disconnection and connection counter */      
    DISCONNECTION_COUNTER = 0;
    CONNECTION_COUNTER = 0;   
  
    /* connection state is from connection to disconnection */      
    if((CONNECTION_STATE & 0x3) == 2)
    {
      /* phone disconnect from the network */
      /* to be added */      
    }    
      
#if (IF_PRINT_LOG)  
		if (UART_LOG)
		{
      if (WIRELESS_SIM_ON)
      {
        printf("==================== Wireless On, Not connected, CONNECTION_STATE = %ld ==================== \r\n", CONNECTION_STATE & 0x7);
      }
      else
      {
        printf("++++++++++++++++++++ Wireless Off, Not connected, CONNECTION_STATE = %ld ++++++++++++++++++++ \r\n", CONNECTION_STATE & 0x7);
      }
		}
#endif
	
	  /* if current phone-watch is connected */
	  if ((IF_CONNECTED_PHONE == 1) && WIRELESS_SIM_ON)
	  {
	    /* mark the phone-watch as disconnection state */
	    IF_CONNECTED_PHONE = 0; 
      FETCH_COMMAND_MENU = 151;  
      USAT_BYTE_LENGTH_BACK = 14 + strlen((char *)SIM_ON_DISCONNECTED_STRING_PHONE);  
    }
  } 

  /* connection counter overflow, USIM assume there is connection between phone-USIM */
  if (CONNECTION_COUNTER)
  {
    CONNECTION_COUNTER = 0; 
     
    /* connection state is from disconnection to connection */ 
    if((CONNECTION_STATE & 0x3) == 1)
    {
      /* mark the USAT bit for reset NAA, let phone access the network */      
      CONNECTION_STATE |= 0x4;
      /* mark the first byte of EF data, upate the EF contents over the air */
      UPDATED_BYTES_2FE2[0] = 0xff;
      UPDATED_BYTES_2F05[0] = 0xff;
      UPDATED_BYTES_7FFF_6F07[0] = 0xff;
    }
    
#if (IF_PRINT_LOG)  
		if (UART_LOG)
		{
      printf("==================== Wireless On, Connected, CONNECTION_STATE = %ld  ==================== \r\n", CONNECTION_STATE & 0x7);
    }
#endif
	  
	/* if current phone-watch is disconnected */
	if (IF_CONNECTED_PHONE == 0)
	{
	  /* mark the phone-watch as disconnection state */
	  IF_CONNECTED_PHONE = 1; 
      FETCH_COMMAND_MENU = 150;  
      USAT_BYTE_LENGTH_BACK = 14 + strlen((char *)SIM_ON_CONNECTED_STRING_PHONE);  
    }
  }
}

/********************************************************************************/
uint32_t connection_request_phone(uint8_t connection_type)
/*--------------------------------------------------------------------------------
| radio link transmission at phone side for random access request
|
--------------------------------------------------------------------------------*/
{
  uint8_t i;  
  uint8_t connection_byte[CONNECTION_REQUEST_BYTE];
  uint32_t tx_logical_address;
  uint32_t rx_logical_address;
  uint32_t connection_request_intervel_tx_ms = PHONE_REQUEST_TX_TIME;
  uint32_t connection_request_wait_feedback_ms = PHONE_REQUEST_WAIT_TIME;
  uint32_t initial_timer;

  if (WIRELESS_SIM_ON == 0)
  {
    return(3);
  }
  
  if (start_oscillator_16m( ))
  {
    return(3);
  }
    
  tx_logical_address = (PHONE_ADD_LOGICAL & 7);
  rx_logical_address = (1 << tx_logical_address);
  
  /* transmit byte length */
  connection_byte[0] = 25;
  /* phone logical address, repeat 3 times */
  connection_byte[1] = (PHONE_ADD_LOGICAL & 7);
  connection_byte[2] = connection_byte[1];
  connection_byte[3] = connection_byte[1];
  /* connection request type, repeat 3 times */
  connection_byte[4] = connection_type;  
  connection_byte[5] = connection_type;  
  connection_byte[6] = connection_type; 
  /*  */ 
  connection_byte[7] = DATA_CARRIER_FREQ;  
  connection_byte[8] = DATA_CARRIER_FREQ;  
  connection_byte[9] = DATA_CARRIER_FREQ; 
  /* insert the user passwaord in the connect request message */
  for (i=0; i<KEY_LENGTH; i++)
  {
    connection_byte[i + 10] = USER_PASSWORD[PASSWORD_SHUFFLE[i]] ^ 
		                     SYSTEM_KEY[SYSTEM_KEY_SHUFFLE[i]];
  } 
  
#if (IF_CC_CODING)
  /* connection request message byte length, coding rate = 1/2 */
  CC_CODED_BYTE[0] = (CONNECTION_REQUEST_BYTE * 2);  
  /* cc encoding for the connection request information bytes */
  ccencoder_byte_sequence_32(connection_byte, ENCODE_BLOCK_INDEX, CC_CODED_BYTE + 1);
#endif
    						  
  /* get the initial real time counter */
  initial_timer = NRF_RTC0->COUNTER;
  
  /* the first time connection request transmission try */  
  while (1)
  {
    radio_carrier_detection(MAX_CD_TIME_MS, TX_RSSI_THRED, FREQUENCY_CARRIER_ACCESS);
    /* transmitted successfully */
#if (IF_CC_CODING)	
    if (!tx_radio_config_parameter(CC_CODED_BYTE, FREQUENCY_CARRIER_ACCESS, 
           tx_logical_address, connection_request_intervel_tx_ms, POWER_LEVEL_CONNECTION))
#else
    if (!tx_radio_config_parameter(connection_byte, FREQUENCY_CARRIER_ACCESS, 
		   tx_logical_address, connection_request_intervel_tx_ms, POWER_LEVEL_CONNECTION))
#endif			   
    {
      break;
    }  
    /* Tx transmission tried time out */    
    if (((NRF_RTC0->COUNTER - initial_timer) > DATA_RX_TIME) || (NRF_RTC0->COUNTER < initial_timer))
    {
      /* wireless link mantenance, transmission fails, return successful */
      if (connection_type == 0x11)
      {
        return(0);
      }
      /* data transmission, transmission fails, return error */
      else
      {
        return(1);
      }
    }
  }
  
  *CHECK_BIT = 0;
  /* connection request transmission try again, if connection response not received */  
  while (rx_radio_config_parameter(CHECK_BIT, DATA_CARRIER_FREQ, rx_logical_address, 
         connection_request_wait_feedback_ms))
  {
    /* connection request try transmission */
    while (1)
    {
      radio_carrier_detection(MAX_CD_TIME_MS, TX_RSSI_THRED, FREQUENCY_CARRIER_ACCESS);
      /* transmitted successfully */
#if (IF_CC_CODING)	  
      if (!tx_radio_config_parameter(CC_CODED_BYTE, FREQUENCY_CARRIER_ACCESS, tx_logical_address, 
             connection_request_intervel_tx_ms, POWER_LEVEL_CONNECTION))
#else
	  if (!tx_radio_config_parameter(connection_byte, FREQUENCY_CARRIER_ACCESS, tx_logical_address, 
		     connection_request_intervel_tx_ms, POWER_LEVEL_CONNECTION))
#endif				 				 
      {
        break;
      }  
      /* Tx transmission tried time out */    
      if (((NRF_RTC0->COUNTER - initial_timer) > DATA_TX_TIME) || (NRF_RTC0->COUNTER < initial_timer))
      {
        /* wireless link mantenance, transmission fails, return successful */
        if (connection_type == 0x11)
        {
          return(0);
        }
        /* data transmission, transmission fails, return error */
        else
        {
          return(1);
        }
      }
    }

    /* Rx tried time out */    
    if (((NRF_RTC0->COUNTER - initial_timer) > DATA_RX_TIME) || (NRF_RTC0->COUNTER < initial_timer))
    {      
#if (IF_PRINT_LOG)
			if (UART_LOG)
			{
        printf("\r\n----------- Time out for connection request, time used = %ld -----------\r\n", NRF_RTC0->COUNTER - initial_timer); 
      }
#endif
      
      return(1);
    }
  }
  
  /* connection response message received */  
#if (IF_CC_CODING)  
  /* cc decoding for connection response message */
  ccdecoder_byte_sequence_32(CHECK_BIT + 1, ENCODE_BLOCK_INDEX, connection_byte);
#else
  for (i=0; i<32; i++)
  {
    *(connection_byte + i) = *(CHECK_BIT + i);
  }      
#endif  
					  	
  /* simple receiver error check */	    	 
  if ((*(connection_byte + 1) != *(connection_byte + 2)) || (*(connection_byte + 1) != *(connection_byte + 3)))
    {
#if (IF_PRINT_LOG)
			if (UART_LOG)
			{
        printf("\r\n------------------------- Logical address receive error in connection_request_phone -------------------------\r\n");
        printf_log_rx(CONNECTION_REQUEST_BYTE, connection_byte);
      }
#endif
      return(1);		
    }
  if ((*(connection_byte + 4) != *(connection_byte + 5)) || (*(connection_byte + 4) != *(connection_byte + 6)))
  {
#if (IF_PRINT_LOG)
		if (UART_LOG)
		{
      printf("\r\n------------------------- Frequency carrier receive error in connection_request_phone -------------------------\r\n");
      printf_log_rx(CONNECTION_REQUEST_BYTE, connection_byte);
    }
#endif
    return(1);		
  }
    
  /* save the carrier frequency for data transmission use later */
  DATA_CARRIER_FREQ = connection_byte[4];
  PHONE_ADD_LOGICAL = (connection_byte[1] & 7);
  
#if (IF_PRINT_LOG)
	if (UART_LOG)
	{
    printf("\r\n---------------------------- Data carrier frequency = %d, Logical address = %d, ----------------------------\r\n", DATA_CARRIER_FREQ, PHONE_ADD_LOGICAL); 
  }
#endif
  
  /* phone SIM reset */    
  if ((connection_byte[7] == 0x33) && (PHONE_ADD_LOGICAL != 7))
  {
    /* mark the USAT bit for reset NAA, let phone access the network */      
    CONNECTION_STATE |= 0x4;
#if (IF_PRINT_LOG)
		if (UART_LOG)
		{
      printf("\r\n------------------- Phone Re-access Network-----------------------\r\n"); 
    }
#endif
  }
  
  /* authentication for data transmission */
  if (connection_type  == 0x0)
  {
    /* authentication algorithm running result */
    wireless_sim_authentication_algorithm(USER_PASSWORD, SYSTEM_KEY, connection_byte + 10, 
	                                      AUTHENTICATE_RESULT);
  }
        
  return(0);
}

/********************************************************************************/
void phone_write_sim_files_data(void)
/*--------------------------------------------------------------------------------
| update EFs content data
|
--------------------------------------------------------------------------------*/
{
  uint32_t i;
  
  uint32_t offset_2fe2 = 0x1;
  uint32_t offset_2f05 = 0x1 + 0xa;
  uint32_t offset_6f07 = 0x1 + 0xa + 0x4;
  uint32_t offset_6f08 = 0x1 + 0xa + 0x4 + 0x9;
  uint32_t offset_6f09 = 0x1 + 0xa + 0x4 + 0x9 + 0x21;
  uint32_t offset_6f31 = 0x1 + 0xa + 0x4 + 0x9 + 0x21 + 0x21;
  uint32_t offset_6f78 = 0x1 + 0xa + 0x4 + 0x9 + 0x21 + 0x21 + 0x1;
  uint32_t offset_6f7e = 0x1 + 0xa + 0x4 + 0x9 + 0x21 + 0x21 + 0x1 + 0x2;
  uint32_t offset_6f73 = 0x1 + 0xa + 0x4 + 0x9 + 0x21 + 0x21 + 0x1 + 0x2 + 0xb;
  uint32_t offset_6f7b = 0x1 + 0xa + 0x4 + 0x9 + 0x21 + 0x21 + 0x1 + 0x2 + 0xb + 0xe;
  uint32_t offset_6fad = 0x1 + 0xa + 0x4 + 0x9 + 0x21 + 0x21 + 0x1 + 0x2 + 0xb + 0xe + 0xc;
  uint32_t offset_6f5c = 0x1 + 0xa + 0x4 + 0x9 + 0x21 + 0x21 + 0x1 + 0x2 + 0xb + 0xe + 0xc + 0x4;
  uint32_t offset_6f5b = 0x1 + 0xa + 0x4 + 0x9 + 0x21 + 0x21 + 0x1 + 0x2 + 0xb + 0xe + 0xc + 0x4 + 0x3;
  uint32_t offset_6fc4 = 0x1 + 0xa + 0x4 + 0x9 + 0x21 + 0x21 + 0x1 + 0x2 + 0xb + 0xe + 0xc + 0x4 + 0x3 + 0x6;
  uint32_t offset_6f56 = 0x1 + 0xa + 0x4 + 0x9 + 0x21 + 0x21 + 0x1 + 0x2 + 0xb + 0xe + 0xc + 0x4 + 0x3 + 0x6 + 0x2e;
  uint32_t offset_4f20 = 0x1 + 0xa + 0x4 + 0x9 + 0x21 + 0x21 + 0x1 + 0x2 + 0xb + 0xe + 0xc + 0x4 + 0x3 + 0x6 + 0x2e + 0x1;
  uint32_t offset_4f52 = 0x1 + 0xa + 0x4 + 0x9 + 0x21 + 0x21 + 0x1 + 0x2 + 0xb + 0xe + 0xc + 0x4 + 0x3 + 0x6 + 0x2e + 0x1 + 0x9;
  uint32_t offset_6f42 = 0x1 + 0xa + 0x4 + 0x9 + 0x21 + 0x21 + 0x1 + 0x2 + 0xb + 0xe + 0xc + 0x4 + 0x3 + 0x6 + 0x2e + 0x1 + 0x9 + 0x9;

  /* split files data to each EF */  
  /* file 1: 2fe2, fixed length: 0xa */
  UPDATED_BYTES_2FE2[0] = 0xa;
  for (i=0; i<0xa; i++)
  {
    UPDATED_BYTES_2FE2[i + 1] = SIM_FILES_1[offset_2fe2 + i];
  }
  
  /* file 2: 2f05, fixed length: 0x4 */
  UPDATED_BYTES_2F05[0] = 0x4;
  for (i=0; i<0x4; i++)
  {
    UPDATED_BYTES_2F05[i + 1] = SIM_FILES_1[offset_2f05 + i];
  }
  
  /* file 3: 6f07, fixed length: 0x9 */
  UPDATED_BYTES_7FFF_6F07[0] = 0x9;
  for (i=0; i<0x9; i++)
  {
    UPDATED_BYTES_7FFF_6F07[i + 1] = SIM_FILES_1[offset_6f07 + i];
  }
  
  /* file 4: 6f08, fixed length: 0x21 */
  UPDATED_BYTES_7FFF_6F08[0] = 0x21;
  for (i=0; i<0x21; i++)
  {
    UPDATED_BYTES_7FFF_6F08[i + 1] = SIM_FILES_1[offset_6f08 + i];
  }
  
  /* file 5: 6f09, fixed length: 0x21 */
  UPDATED_BYTES_7FFF_6F09[0] = 0x21;
  for (i=0; i<0x21; i++)
  {
    UPDATED_BYTES_7FFF_6F09[i + 1] = SIM_FILES_1[offset_6f09 + i];
  }
  
  /* file 6: 6f31, fixed length: 0x1 */
  UPDATED_BYTES_7FFF_6F31[0] = 0x1;
  for (i=0; i<0x1; i++)
  {
    UPDATED_BYTES_7FFF_6F31[i + 1] = SIM_FILES_1[offset_6f31 + i];
  }
  
  /* file 7: 6f78, fixed length: 0x2 */
  UPDATED_BYTES_7FFF_6F78[0] = 0x2;
  for (i=0; i<0x2; i++)
  {
    UPDATED_BYTES_7FFF_6F78[i + 1] = SIM_FILES_1[offset_6f78 + i];
  }
  
  /* file 8: 6f7e, fixed length: 0xb */
  UPDATED_BYTES_7FFF_6F7E[0] = 0xb;
  for (i=0; i<0xb; i++)
  {
    UPDATED_BYTES_7FFF_6F7E[i + 1] = SIM_FILES_1[offset_6f7e + i];
  }
  
  /* file 9: 6f73, fixed length: 0xe */
  UPDATED_BYTES_7FFF_6F73[0] = 0xe;
  for (i=0; i<0xe; i++)
  {
    UPDATED_BYTES_7FFF_6F73[i + 1] = SIM_FILES_1[offset_6f73 + i];
  }
  
  /* file 10: 6f7b, fixed length: 0xc */
  UPDATED_BYTES_7FFF_6F7B[0] = 0xc;
  for (i=0; i<0xc; i++)
  {
    UPDATED_BYTES_7FFF_6F7B[i + 1] = SIM_FILES_1[offset_6f7b + i];
  }
  
  /* file 11: 6fad, fixed length: 0x4 */
  UPDATED_BYTES_7FFF_6FAD[0] = 0x4;
  for (i=0; i<0x4; i++)
  {
    UPDATED_BYTES_7FFF_6FAD[i + 1] = SIM_FILES_1[offset_6fad + i];
  }
  
  /* file 12: 6f5c, fixed length: 0x3 */
  UPDATED_BYTES_7FFF_6F5C[0] = 0x3;
  for (i=0; i<0x3; i++)
  {
    UPDATED_BYTES_7FFF_6F5C[i + 1] = SIM_FILES_1[offset_6f5c + i];
  }
  
  /* file 13: 6f5b, fixed length: 0x6 */
  UPDATED_BYTES_7FFF_6F5B[0] = 0x6;
  for (i=0; i<0x6; i++)
  {
    UPDATED_BYTES_7FFF_6F5B[i + 1] = SIM_FILES_1[offset_6f5b + i];
  }
  
  /* file 14: 6fc4, fixed length: 0x2e */
  UPDATED_BYTES_7FFF_6FC4[0] = 0x2e;
  for (i=0; i<0x2e; i++)
  {
    UPDATED_BYTES_7FFF_6FC4[i + 1] = SIM_FILES_1[offset_6fc4 + i];
  }
  
  /* file 15: 6f56, fixed length: 0x1 */
  UPDATED_BYTES_7FFF_6F56[0] = 0x1;
  for (i=0; i<0x1; i++)
  {
    UPDATED_BYTES_7FFF_6F56[i + 1] = SIM_FILES_1[offset_6f56 + i];
  }
  
  /* file 16: 4f20, fixed length: 0x9 */
  UPDATED_BYTES_7FFF_5F3B_4F20[0] = 0x9;
  for (i=0; i<0x9; i++)
  {
    UPDATED_BYTES_7FFF_5F3B_4F20[i + 1] = SIM_FILES_1[offset_4f20 + i];
  }
  
  /* file 17: 4f52, fixed length: 0x9 */
  UPDATED_BYTES_7FFF_5F3B_4F52[0] = 0x9;
  for (i=0; i<0x9; i++)
  {
    UPDATED_BYTES_7FFF_5F3B_4F52[i + 1] = SIM_FILES_1[offset_4f52 + i];
  }
  
  /* file 18: 6f42, fixed length: 0x28 */
  UPDATED_BYTES_7FFF_6F42[0] = 0x28;
  for (i=0; i<0x28; i++)
  {
    UPDATED_BYTES_7FFF_6F42[i + 1] = SIM_FILES_1[offset_6f42 + i];
  }
}

/********************************************************************************/
uint32_t phone_command_get_file_data(uint16_t file_ID, uint8_t *file_data)
/*--------------------------------------------------------------------------------
| get the EFs data from the watch over the air, send phone command: 0xbb
|
--------------------------------------------------------------------------------*/
{
  uint8_t i;
  uint8_t *addr_EF_files;
  
  addr_EF_files = (uint8_t *)(USIM_EF_FLASH_ADDR);    
  
  switch (file_ID)
  {
    case 0xffff:
      /* update SIM file data */
      for (i=0; i<248; i++)
      {
        *(file_data + i) = *(addr_EF_files + i);
      }
    break;
    
    default:
#if (IF_PRINT_LOG)
		if (UART_LOG)
		{
      printf("\r\n============= Not supported file ID in phone_command_get_file_data =============\r\n");
    }
#endif
    break;    
  }
  
#if (IF_PRINT_LOG)    
	if (UART_LOG)
	{
    printf_log_rx(*(file_data + 0), file_data + 1);
  }
#endif  
#if (PRINT_DELAY_MS) 
  nrf_delay_ms(PRINT_DELAY_MS);
#endif
  
  if (file_ID == 0xffff)
  {
    phone_write_sim_files_data( );
  }
  else
  {
  	
  }
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_send_data(uint8_t *send_data, uint8_t data_length, uint8_t data_type)
/*--------------------------------------------------------------------------------
| phone send data to watch, phone command: 0xcc, 
| data type: data_type, for different data 
|
--------------------------------------------------------------------------------*/
{
  uint8_t i;
  uint8_t return_connection_request;
  uint32_t received_length;
  uint8_t data_received_confirm[6];
        
  *(INFO_BYTE + 0) = 5 + KEY_LENGTH + data_length;
  *(INFO_BYTE + 1) = 0x0;
  *(INFO_BYTE + 2) = 0xcc;
  *(INFO_BYTE + 3) = 0x0;
  *(INFO_BYTE + 4) = 0x0;
  *(INFO_BYTE + 5) = data_type;

  /* re-load watch dog request register */
  NRF_WDT->RR[0] = 0x6E524635;
  return_connection_request = connection_request_phone(0x0);
  if (return_connection_request)    
  {
#if (IF_PRINT_LOG)
		if (UART_LOG)
		{
      printf("\r\n++++++++++++++++++++++++++++++++++++++ Hi, connection is not valid ++++++++++++++++++++++++++++++++++++++\r\n");
		}
#endif
	  if (return_connection_request != 3)
	  {
	    stop_oscillator_16m( ); 	
	  }
	
	  return(1);
  }
  else
  {
    /* mark the phone-watch connection state */
    IF_CONNECTED_PHONE = 1; 
    /* addtional time for phone to wait for random challange data generation at watch side */
    nrf_delay_ms(RANDOM_BYTE_NUMBER_TIME_OUT_MS + 1);
	
	  /* add the wireless UICC-terminal authentication running result data */
	  for (i=6; i<6 + KEY_LENGTH; i++)
	  {
	    *(INFO_BYTE + i) = AUTHENTICATE_RESULT[i - 6];
	  }
	  /* add the phone send data */
	  for (i=6 + KEY_LENGTH; i<6 + KEY_LENGTH + data_length; i++)
	  {
	    *(INFO_BYTE + i) = *(send_data + i - (6 + KEY_LENGTH));
	  }
   
#if (IF_PRINT_LOG)
	  if (UART_LOG)
	  {
	    printf_log_tx(5 + KEY_LENGTH + data_length, INFO_BYTE + 1);  
    }
#endif
		  
#if (IF_CC_CODING)
    *(CC_CODED_BYTE + 0) = 128;
    ccencoder_byte_sequence_64(INFO_BYTE, ENCODE_BLOCK_INDEX, CC_CODED_BYTE + 1);
    if (transmit_info_check(CC_CODED_BYTE[0], CC_CODED_BYTE))
#else  
    if (transmit_info_check(5 + KEY_LENGTH + data_length, INFO_BYTE))
#endif
    {
#if (IF_PRINT_LOG)
			if (UART_LOG)
			{
        printf("++++++++++++++++++++++++++++++++++++++ send data time out in  ++++++++++++++++++++++++++++++++++++++\r\n");
      }
#endif
	    stop_oscillator_16m( );
  
	    return(1);	  
    }
	
    /* re-load watch dog request register */
    NRF_WDT->RR[0] = 0x6E524635;
	  received_length = 0;
	  if (receive_info_check(&received_length, data_received_confirm))
	  {
#if (IF_PRINT_LOG)
		  if (UART_LOG)
		  {
        printf("------------------------------ confirm information receiving time out ------------------------------\r\n");
      }
#endif
      stop_oscillator_16m( );  
	    
      return(1);		
	  }
	  	
	  stop_oscillator_16m( );
	
	  if ((*(data_received_confirm + 0) == 5) && (*(data_received_confirm + 1) == 0x11) && (*(data_received_confirm + 2) == 0x22))
    {
	    return(0);
    }
	  else
	  {
	    return(1);
	  }	
  }	  
}


