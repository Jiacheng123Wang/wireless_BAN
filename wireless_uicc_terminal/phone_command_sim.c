#include "define_config.h"
#include "phone_command_sim.h"
#include "phone_wireless_interface.h"
#include "file_data_usim.h"
#include "comm_UICC_terminal_interface.h"
#include "comm_phone_command.h"

uint64_t SELECTED_FILE = 0x0;
uint64_t MASK_1 = 0xFFFFFFFFFFFF0000;

extern uint8_t INFO_BYTE[PACKET_MAX_SIZE + 20];
extern uint8_t CHECK_BIT[PACKET_MAX_SIZE + 20];
extern volatile uint32_t USAT_BYTE_LENGTH_BACK;
extern uint8_t UART_LOG;

/********************************************************************************/
uint32_t phone_reset_initial(uint32_t etu_length, uint32_t pin_number_io)
/*--------------------------------------------------------------------------------
| phone initial cold reset when power on
|
--------------------------------------------------------------------------------*/
{
  NRF_TIMER0->CC[0] = etu_length;
  NRF_TIMER0->TASKS_START = 1;
    
  /* wait phone sent reset signal */	
  while (nrf_gpio_pin_read(PIN_RESET_PHONE) == 0)
  {
  }	
  write_byte(0x3b, 1, etu_length, pin_number_io);	
		
  write_byte(0x9d, 1, etu_length, pin_number_io);		
  write_byte(0x94, 1, etu_length, pin_number_io);		
  write_byte(0x80, 1, etu_length, pin_number_io);		
  write_byte(0x1f, 1, etu_length, pin_number_io);		
  write_byte(0xc7, 1, etu_length, pin_number_io);		
  write_byte(0x80, 1, etu_length, pin_number_io);	
  write_byte(0x31, 1, etu_length, pin_number_io);	
  write_byte(0xe0, 1, etu_length, pin_number_io);		
  write_byte(0x73, 1, etu_length, pin_number_io);	
  write_byte(0xfe, 1, etu_length, pin_number_io);	
  write_byte(0x21, 0, etu_length, pin_number_io);	
  write_byte(0x13, 1, etu_length, pin_number_io);		
  write_byte(0x65, 0, etu_length, pin_number_io);	
  write_byte(0xd0, 1, etu_length, pin_number_io);		
  write_byte(0x1, 1, etu_length, pin_number_io);	
  write_byte(0x87, 0, etu_length, pin_number_io);	
  write_byte(0xf, 0, etu_length, pin_number_io);		
  write_byte(0x75, 1, etu_length, pin_number_io);		
  write_byte(0xf6, 0, etu_length, pin_number_io);	
		
#if (IF_PRINT_LOG)		
	if (UART_LOG)
	{
    printf("=================== Phone initial reset =========================\r\n");
	}
#else
  nrf_delay_ms(PRINT_DELAY_MS);
#endif

  /* read PPS bytes from phone */
  nrf_delay_ms(10);

#if (IF_PRINT_LOG)
	if (UART_LOG)
	{
    printf("=================== Read PPS bytes from phone =========================\r\n");
	}
#else
  nrf_delay_ms(PRINT_DELAY_MS);
#endif

  nrf_delay_ms(10);
	
  /* PPS bytes */
  write_byte(0xff, 0, etu_length, pin_number_io);	
  write_byte(0x10, 1, etu_length, pin_number_io);	
  write_byte(0x94, 1, etu_length, pin_number_io);		
  write_byte(0x7b, 0, etu_length, pin_number_io);	

#if (IF_PRINT_LOG)		
	if (UART_LOG)
	{
    printf("=================== Write PPS bytes to phone =========================\r\n");
	}
#else
  nrf_delay_ms(PRINT_DELAY_MS);
#endif

  NRF_TIMER0->TASKS_STOP = 1;
  return(0);
}	

/********************************************************************************/
uint32_t phone_command_f2_response(uint8_t *bytes_command, uint32_t etu_length, uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| phone command 0xf2: status
|
--------------------------------------------------------------------------------*/
{
  uint8_t status_bytes[2];
  
#if (IF_PRINT_LOG)  
  uint32_t i;  
  
	if (UART_LOG)
	{
    printf("Phone status command: ");
    for (i=0; i<5; i++)
    {
      printf("%x,  ", *(bytes_command + i + 1));
    }
    printf("\r\n");
  
    /* 0xf2 phone command, directly feedback */
    printf("Status command feedback: ");
    printf("%x,  ", 0x90);
    printf("%x,  \r\n", 0x0);
	}
#endif
#if (PRINT_DELAY_MS)  
  nrf_delay_ms(PRINT_DELAY_MS);
#endif
  
  if (USAT_BYTE_LENGTH_BACK)
  {
    status_bytes[0] = 0x91;
    status_bytes[1] = USAT_BYTE_LENGTH_BACK;
  }
  else
  {
    status_bytes[0] = 0x90;
    status_bytes[1] = 0;
  }
  
  /* write SIM response to phone */    
  write_bytes(2, status_bytes, etu_length, pin_number_phone);   
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_unblock_pin(uint8_t *phone_command, uint32_t etu_length)
/*--------------------------------------------------------------------------------
| phone command 0x2c: unlock user PIN
|
--------------------------------------------------------------------------------*/
{
uint8_t response_bytes[2] = {0x63, 0xca};

#if (IF_PRINT_LOG)    
  if (UART_LOG)
  {
    printf("unblocked successfully......\r\n");
	}
#endif  
#if (PRINT_DELAY_MS)
  nrf_delay_ms(PRINT_DELAY_MS);
#endif
  
  write_bytes(2, response_bytes, etu_length, PIN_DATA_PHONE);   
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_verify_20(uint8_t *phone_command, uint32_t etu_length)
/*--------------------------------------------------------------------------------
| phone command 0x20: verify
|
--------------------------------------------------------------------------------*/
{
uint8_t response_bytes[2] = {0x63, 0xc3};

#if (IF_PRINT_LOG)    
  if (UART_LOG)
  {
    printf("verified successfully......\r\n");
	}
#endif  
#if (PRINT_DELAY_MS)
  nrf_delay_ms(PRINT_DELAY_MS);
#endif
  
  write_bytes(2, response_bytes, etu_length, PIN_DATA_PHONE);   

  return(0);  
}

/********************************************************************************/
uint32_t phone_command_select_mandatory(uint8_t *phone_command, uint32_t etu_length)
/*--------------------------------------------------------------------------------
| phone command 0xa4: file selection for necessary EFs
|
--------------------------------------------------------------------------------*/
{
  uint8_t selecet_fd[2];
  uint8_t confirm_bytes[1] = {0xa4};
  uint8_t no_file_bytes[2] = {0x6a, 0x82};
  uint8_t wrong_parameter_bytes[2] = {0x6a, 0x87};
  uint8_t status_bytes[2] = {0x90, 0x0};
  
  if (USAT_BYTE_LENGTH_BACK)
  {
    status_bytes[0] = 0x91;
    status_bytes[1] = USAT_BYTE_LENGTH_BACK;
  }  

  if (*(phone_command + 4) == 0x4)
  {
    selecet_fd[0] = 0x61;    
    if (*(phone_command + 5) == 0x2)
    { 
      write_bytes(1, confirm_bytes, etu_length, PIN_DATA_PHONE);     
      
      if (read_bytes_phone(*(phone_command + 5), INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
      {
        return(1);
      }

      if ((*(INFO_BYTE + 1) ==0x7f) && (*(INFO_BYTE + 2) ==0xff))
      {
        SELECTED_FILE  = 0x7fff;
#if (IF_PRINT_LOG)
				if (UART_LOG)
				{
          printf_selected_file(SELECTED_FILE);
				}
#endif
        
        selecet_fd[1] = 0x39;    
        write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
        
        if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
        {
          return(1);
        }
        write_phone_response(0x39, M_FILE_CONTROL_PARAMETER_ADF + 1, etu_length);
      }
      else if ((*(INFO_BYTE + 1) ==0x3f) && (*(INFO_BYTE + 2) ==0x0))
      {
        SELECTED_FILE  = 0;
#if (IF_PRINT_LOG)
				if (UART_LOG)
				{
          printf_selected_file(SELECTED_FILE);
				}
#endif
        
        selecet_fd[1] = 0x2b;    
        write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
        
        if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
        {
          return(1);
        }
        write_phone_response(0x2b, M_FILE_CONTROL_PARAMETER_3F00 + 1, etu_length);
      }
      else
      {
        if (SELECTED_FILE == 0x7fff || SELECTED_FILE == 0x7fff5f3b)
        {
          SELECTED_FILE <<= 16;
        }
        SELECTED_FILE  &= MASK_1;
        SELECTED_FILE  |= (*(INFO_BYTE + 1) * 0x100);
        SELECTED_FILE  |= (*(INFO_BYTE + 2));
        
#if (IF_PRINT_LOG)
				if (UART_LOG)
				{
          printf_selected_file(SELECTED_FILE);
				}
#endif
        
        switch (SELECTED_FILE)
        {
          case 0x2fe2:
    		selecet_fd[1] = 0x25;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_2FE2 + 1, etu_length);
            break;

          case 0x2f05:
            selecet_fd[1] = 0x25;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_2F05 + 1, etu_length);
            break;

          case 0x2f00:
            selecet_fd[1] = 0x28;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(0x28, M_FILE_CONTROL_PARAMETER_2F00 + 1, etu_length);
            break;

          case 0x7fff6fb7:
            selecet_fd[1] = 0x28;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(0x28, M_FILE_CONTROL_PARAMETER_7FFF_6FB7 + 1, etu_length);
            break;

          case 0x7fff6fad:
            selecet_fd[1] = 0x25;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6FAD + 1, etu_length);
            break;

          case 0x7fff6f07:
            selecet_fd[1] = 0x25;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F07 + 1, etu_length);
            break;

          case 0x7fff6f78:
            selecet_fd[1] = 0x25;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F78 + 1, etu_length);
            break;

          case 0x7fff6f7e:
            selecet_fd[1] = 0x25;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F7E + 1, etu_length);
            break;

          case 0x7fff6f38:
            selecet_fd[1] = 0x25;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F38 + 1, etu_length);
            break;

          case 0x7fff6f08:
            selecet_fd[1] = 0x25;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F08 + 1, etu_length);
            break;

          case 0x7fff6f09:
            selecet_fd[1] = 0x25;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F09 + 1, etu_length);
            break;

          case 0x7fff6f5b:
            selecet_fd[1] = 0x25;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F5B + 1, etu_length);
            break;

          case 0x7fff6f5c:
            selecet_fd[1] = 0x25;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F5C + 1, etu_length);
            break;

          case 0x7fff6fc4:
            selecet_fd[1] = 0x24;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(0x24, M_FILE_CONTROL_PARAMETER_7FFF_6FC4 + 1, etu_length);
            break;
        
          case 0x7fff6f7b:
            selecet_fd[1] = 0x25;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F7B + 1, etu_length);
            break;
                
          case 0x7fff6f06:
            selecet_fd[1] = 0x28;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(0x28, M_FILE_CONTROL_PARAMETER_7FFF_6F06 + 1, etu_length);
            break;
        
          case 0x7fff6f31:
            selecet_fd[1] = 0x25;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F31 + 1, etu_length);
            break;

          case 0x7fff6f73:
            selecet_fd[1] = 0x25;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F73 + 1, etu_length);
            break;

          case 0x7fff6f56:
            selecet_fd[1] = 0x25;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F56 + 1, etu_length);
            break;

          case 0x7fff6f4b:
            selecet_fd[1] = 0x27;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(selecet_fd[1], M_FILE_CONTROL_PARAMETER_7FFF_6F4B + 1, etu_length);
            break;

          case 0x7fff6f49:
            selecet_fd[1] = 0x27;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(selecet_fd[1], M_FILE_CONTROL_PARAMETER_7FFF_6F49 + 1, etu_length);
            break;

          case 0x7fff6f3c:
            selecet_fd[1] = 0x27;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(selecet_fd[1], M_FILE_CONTROL_PARAMETER_7FFF_6F3C + 1, etu_length);
            break;

          case 0x7fff6f43:
            selecet_fd[1] = 0x24;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(selecet_fd[1], M_FILE_CONTROL_PARAMETER_7FFF_6F43 + 1, etu_length);
            break;

          case 0x7fff6f42:
            selecet_fd[1] = 0x1e;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(selecet_fd[1], M_FILE_CONTROL_PARAMETER_7FFF_6F42 + 1, etu_length);
            break;

          case 0x7fff5f3b4f52:
            selecet_fd[1] = 0x25;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_5F3B_4F52 + 1, etu_length);
            break;

          case 0x7fff5f3b4f20:
            selecet_fd[1] = 0x25;    
            write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
            
            if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
            {
              return(1);
            }
            write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_5F3B_4F20 + 1, etu_length);
            break;
      
          default:
#if (IF_PRINT_LOG)
					  if (UART_LOG)
					  {
              printf("selected file not found......\r\n");
						}
#endif

            write_bytes(2, no_file_bytes, etu_length, PIN_DATA_PHONE);
            break;      
        }
      }
    }
    else if (*(phone_command + 5) == 0x4)
    {
      write_bytes(1, confirm_bytes, etu_length, PIN_DATA_PHONE); 
      if (read_bytes_phone(*(phone_command + 5), INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
      {
        return(1);
      }
      SELECTED_FILE  = 0;
      SELECTED_FILE  |= (*(INFO_BYTE + 1) * 0x1000000);
      SELECTED_FILE  |= (*(INFO_BYTE + 2) * 0x10000);
      SELECTED_FILE  |= (*(INFO_BYTE + 3) * 0x100);
      SELECTED_FILE  |= (*(INFO_BYTE + 4));
#if (IF_PRINT_LOG)
			if (UART_LOG)
			{
        printf_selected_file(SELECTED_FILE);
			}
#endif
      
      switch (SELECTED_FILE)
      {    
        case 0x7fff6fb7:
          selecet_fd[1] = 0x28;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(0x28, M_FILE_CONTROL_PARAMETER_7FFF_6FB7 + 1, etu_length);
          break;

        case 0x7fff6fad:
          selecet_fd[1] = 0x25;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6FAD + 1, etu_length);
          break;

        case 0x7fff6f07:
          selecet_fd[1] = 0x25;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F07 + 1, etu_length);
          break;

        case 0x7fff6f78:
          selecet_fd[1] = 0x25;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F78 + 1, etu_length);
          break;

        case 0x7fff6f7e:
          selecet_fd[1] = 0x25;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F7E + 1, etu_length);
          break;

        case 0x7fff6f38:
          selecet_fd[1] = 0x25;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F38 + 1, etu_length);
          break;

        case 0x7fff6f08:
          selecet_fd[1] = 0x25;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F08 + 1, etu_length);
          break;

        case 0x7fff6f09:
          selecet_fd[1] = 0x25;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F09 + 1, etu_length);
          break;

        case 0x7fff6f5b:
          selecet_fd[1] = 0x25;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F5B + 1, etu_length);
          break;

        case 0x7fff6f5c:
          selecet_fd[1] = 0x25;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F5C + 1, etu_length);
          break;

        case 0x7fff6fc4:
          selecet_fd[1] = 0x24;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(0x24, M_FILE_CONTROL_PARAMETER_7FFF_6FC4 + 1, etu_length);
          break;
      
        case 0x7fff6f7b:
          selecet_fd[1] = 0x25;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F7B + 1, etu_length);
          break;
              
        case 0x7fff6f06:
          selecet_fd[1] = 0x28;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(0x28, M_FILE_CONTROL_PARAMETER_7FFF_6F06 + 1, etu_length);
          break;
      
        case 0x7fff6f31:
          selecet_fd[1] = 0x25;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F31 + 1, etu_length);
          break;

        case 0x7fff6f73:
          selecet_fd[1] = 0x25;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F73 + 1, etu_length);
          break;

        case 0x7fff6f56:
          selecet_fd[1] = 0x25;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_6F56 + 1, etu_length);
          break;
          
        case 0x7fff6f4b:
          selecet_fd[1] = 0x27;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(selecet_fd[1], M_FILE_CONTROL_PARAMETER_7FFF_6F4B + 1, etu_length);
          break;
        
        case 0x7fff6f49:
          selecet_fd[1] = 0x27;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(selecet_fd[1], M_FILE_CONTROL_PARAMETER_7FFF_6F49 + 1, etu_length);
          break;

        case 0x7fff6f3c:
          selecet_fd[1] = 0x28;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(selecet_fd[1], M_FILE_CONTROL_PARAMETER_7FFF_6F3C + 1, etu_length);
          break;

        case 0x7fff6f43:
          selecet_fd[1] = 0x25;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(selecet_fd[1], M_FILE_CONTROL_PARAMETER_7FFF_6F43 + 1, etu_length);
          break;

        case 0x7fff6f42:
          selecet_fd[1] = 0x1e;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(selecet_fd[1], M_FILE_CONTROL_PARAMETER_7FFF_6F42 + 1, etu_length);
          break;

        default:
#if (IF_PRINT_LOG)
				  if (UART_LOG)
				  {
            printf("selected file not found......\r\n");
					}
#endif
        
          write_bytes(2, no_file_bytes, etu_length, PIN_DATA_PHONE);
          break;      
      }  
    }
    else if (*(phone_command + 5) == 0x6)
    {
      write_bytes(1, confirm_bytes, etu_length, PIN_DATA_PHONE);
      if (read_bytes_phone(*(phone_command + 5), INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
      {
        return(1);
      }

      SELECTED_FILE  = 0;
      SELECTED_FILE  |= (*(INFO_BYTE + 1) * 0x10000000000);
      SELECTED_FILE  |= (*(INFO_BYTE + 2) * 0x100000000);
      SELECTED_FILE  |= (*(INFO_BYTE + 3) * 0x1000000);
      SELECTED_FILE  |= (*(INFO_BYTE + 4) * 0x10000);
      SELECTED_FILE  |= (*(INFO_BYTE + 5) * 0x100);
      SELECTED_FILE  |= (*(INFO_BYTE + 6));
#if (IF_PRINT_LOG)
			if (UART_LOG)
			{
        printf_selected_file(SELECTED_FILE);
			}
#endif	  
      switch (SELECTED_FILE)
      {
        case 0x7fff5f3b4f20:
          selecet_fd[1] = 0x25;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_5F3B_4F20 + 1, etu_length);
          break;
      
        case 0x7fff5f3b4f52:
          selecet_fd[1] = 0x25;    
          write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
          
          if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
          {
            return(1);
          }
          write_phone_response(0x25, M_FILE_CONTROL_PARAMETER_7FFF_5F3B_4F52 + 1, etu_length);
          break;
            
        default:
#if (IF_PRINT_LOG)
				if (UART_LOG)
				{
          printf("selected file not found......\r\n");
				}
#endif
        
          write_bytes(2, no_file_bytes, etu_length, PIN_DATA_PHONE);
          break;      
      }      
    }
    else if (*(phone_command + 5) == 0x10)
    {
      write_bytes(1, confirm_bytes, etu_length, PIN_DATA_PHONE);
      if (read_bytes_phone(*(phone_command + 5), INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
      {
        return(1);
      }

      SELECTED_FILE  = 0;
      SELECTED_FILE  |= (0x7f * 0x1000000);
      SELECTED_FILE  |= (0xff * 0x10000);
      
      selecet_fd[1] = 0x39;    
      write_bytes(2, selecet_fd, etu_length, PIN_DATA_PHONE);
      
      if (read_bytes_phone(5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
      {
        return(1);
      }
      write_phone_response(0x39, M_FILE_CONTROL_PARAMETER_ADF + 1, etu_length);
    }
    else
    {
#if (IF_PRINT_LOG)
			if (UART_LOG)
			{
        printf("selected file wrong parameters P1 and P2......\r\n");
			}
#endif
      
      write_bytes(2, wrong_parameter_bytes, etu_length, PIN_DATA_PHONE);

      return(1);      
    }
  }
  else
  {    
    write_bytes(1, confirm_bytes, etu_length, PIN_DATA_PHONE);
    if (read_bytes_phone(*(phone_command + 5), INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
    {
      return(1);
    }
    if (*(phone_command + 5) == 2)
    {
      if (*(INFO_BYTE + 1) == 0x2f && *(INFO_BYTE + 2) == 0xe2)
      {
        SELECTED_FILE  = 0x2fe2;
      }
      else if (*(INFO_BYTE + 1) == 0x3f && *(INFO_BYTE + 2) == 0x0)
      {
        SELECTED_FILE  = 0;
      }
      else if (*(INFO_BYTE + 1) == 0x7f && *(INFO_BYTE + 2) == 0xff)
      {
        SELECTED_FILE  = 0x7fff;
      }
      else
      {
        if (SELECTED_FILE == 0x7fff || SELECTED_FILE == 0x7fff5f3b)
        {
          SELECTED_FILE <<= 16;
        }
        SELECTED_FILE  &= MASK_1;
        SELECTED_FILE  |= (*(INFO_BYTE + 1) * 0x100);
        SELECTED_FILE  |= (*(INFO_BYTE + 2));
      }
    }
    else if (*(phone_command + 5) == 4)
    {
      SELECTED_FILE  = 0;
      SELECTED_FILE  |= (*(INFO_BYTE + 1) * 0x1000000);
      SELECTED_FILE  |= (*(INFO_BYTE + 2) * 0x10000);
      SELECTED_FILE  |= (*(INFO_BYTE + 3) * 0x100);
      SELECTED_FILE  |= (*(INFO_BYTE + 4));
    }
    else if (*(phone_command + 5) == 6)
    {
      SELECTED_FILE  = 0;
      SELECTED_FILE  |= (*(INFO_BYTE + 1) * 0x10000000000);
      SELECTED_FILE  |= (*(INFO_BYTE + 2) * 0x100000000);
      SELECTED_FILE  |= (*(INFO_BYTE + 3) * 0x1000000);
      SELECTED_FILE  |= (*(INFO_BYTE + 4) * 0x10000);
      SELECTED_FILE  |= (*(INFO_BYTE + 5) * 0x100);
      SELECTED_FILE  |= (*(INFO_BYTE + 6));
    }
    else if (*(phone_command + 5) == 0x10)
    {
      SELECTED_FILE  = 0x7fff;
    }
    else
    {
#if (IF_PRINT_LOG)
			if (UART_LOG)
			{
        printf("selected file wrong parameters P1 and P2......\r\n");
			}
#endif
      
      write_bytes(2, wrong_parameter_bytes, etu_length, PIN_DATA_PHONE);
      
      return(1);      
    }
#if (IF_PRINT_LOG)
		if (UART_LOG)
		{
      printf_selected_file(SELECTED_FILE);
		}
#endif
      
    write_bytes(2, status_bytes, etu_length, PIN_DATA_PHONE);
  }
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_read_binary_mandatory(uint8_t *phone_command, uint32_t etu_length)
/*--------------------------------------------------------------------------------
| phone command 0xb0: read binary for necessary EFs
|
--------------------------------------------------------------------------------*/
{
  uint8_t *bytes_read;
  uint32_t read_offset;
  uint32_t i;
  uint32_t tmp_length;
  uint8_t tmp_bytes[2] = {0x69, 0x86};
  uint8_t confirm_bytes[1] = {0xb0};
  uint8_t status_bytes[2] = {0x90, 0x0};

  if (USAT_BYTE_LENGTH_BACK)
  {
    status_bytes[0] = 0x91;
    status_bytes[1] = USAT_BYTE_LENGTH_BACK;
  }
  
  if ((*(phone_command + 5)) == 0x00)
    tmp_length = 256;
  else
    tmp_length = (*(phone_command + 5));

  bytes_read = (uint8_t *)malloc(*(phone_command + 5) * sizeof(uint8_t));
  if (bytes_read == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
      printf("memory allocation error...\r\n");
		}
#endif
    
    return(1);
  }
  if ((*(phone_command + 3)) & 0x80)
  {
    read_offset = *(phone_command + 4);
	
    switch ((*(phone_command + 3)) & 0x1f)
    {
	  case 0x03:
        if (UPDATED_BYTES_7FFF_6FAD[0] == 0xff)
        {
          phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6FAD[read_offset + i + 1];
        }
        break;
		
  	  case 0x07:
        if (UPDATED_BYTES_7FFF_6F07[0] == 0xff)
        {
          phone_command_get_file_data(0xffff, SIM_FILES_1);
//          phone_command_get_file_data(0x6f07, UPDATED_BYTES_7FFF_6F07);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6F07[read_offset + i + 1];
        }      
        break;
		
      case 0x06:
        if (UPDATED_BYTES_7FFF_6F78[0] == 0xff)
        {
            phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6F78[read_offset + i + 1];
        }
        break;

      case 0x12:
        if (UPDATED_BYTES_7FFF_6F31[0] == 0xff)
        {
            phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6F31[read_offset + i + 1];
        }
        break;
		
      case 0x0f:
        if (UPDATED_BYTES_7FFF_6F5B[0] == 0xff)
        {
            phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6F5B[read_offset + i + 1];
        }
        break;
		
      case 0x10:
        if (UPDATED_BYTES_7FFF_6F5C[0] == 0xff)
        {
            phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6F5C[read_offset + i + 1];
        }
        break;
		
      case 0x0c:
        if (UPDATED_BYTES_7FFF_6F73[0] == 0xff)
        {
            phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6F73[read_offset + i + 1];
        }
        break;
		
      case 0x08:
        if (UPDATED_BYTES_7FFF_6F08[0] == 0xff)
        {
            phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6F08[read_offset + i + 1];
        }
        break;

      case 0x09:
        if (UPDATED_BYTES_7FFF_6F09[0] == 0xff)
        {
            phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6F09[read_offset + i + 1];
        }
        break;

      default:
#if (IF_PRINT_LOG)
			  if (UART_LOG)
			  {
	        printf("--------------------------------------------------------------------\r\n");  
          printf("Wrong SFI in phone_command_read_binary_mandatory, error...\r\n");
          printf("SFI = %x, \r\n", (*(phone_command + 3)) & 0x1f);
	        printf("--------------------------------------------------------------------\r\n");    
				}
#endif
        /* write SIM response to phone, Command not allowed, no EF selected */    
        write_bytes(2, tmp_bytes, etu_length, PIN_DATA_PHONE);
      
        return(1);		
	}
  }
  else
  {
    read_offset = (*(phone_command + 3)) * 0x100 + (*(phone_command + 4));
	
    switch (SELECTED_FILE)
    {
      case 0x2fe2:
        if (UPDATED_BYTES_2FE2[0] == 0xff)
        {
          phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_2FE2[read_offset + i + 1];
        }
        break;
      
      case 0x2f05:
        if (UPDATED_BYTES_2F05[0] == 0xff)
        {
          phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_2F05[read_offset + i + 1];
        }
        break;

      case 0x7fff6fad:
        if (UPDATED_BYTES_7FFF_6FAD[0] == 0xff)
        {
          phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6FAD[read_offset + i + 1];
        }
        break;

      case 0x7fff6f07:      
        if (UPDATED_BYTES_7FFF_6F07[0] == 0xff)
        {
          phone_command_get_file_data(0xffff, SIM_FILES_1);
  //        phone_command_get_file_data(0x6f07, UPDATED_BYTES_7FFF_6F07);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6F07[read_offset + i + 1];
        }      
        break;

      case 0x7fff6f78:
        if (UPDATED_BYTES_7FFF_6F78[0] == 0xff)
        {
          phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6F78[read_offset + i + 1];
        }
        break;

      case 0x7fff6f7e:
        if (UPDATED_BYTES_7FFF_6F7E[0] == 0xff)
        {
          phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6F7E[read_offset + i + 1];
        }
        break;

      case 0x7fff6f73:
        if (UPDATED_BYTES_7FFF_6F73[0] == 0xff)
        {
          phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6F73[read_offset + i + 1];
        }
        break;

      case 0x7fff6f38:
        if (UPDATED_BYTES_7FFF_6F38[0] == 0xff)
        {
          phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6F38[read_offset + i + 1];
        }
        break;

      case 0x7fff6f08:
        if (UPDATED_BYTES_7FFF_6F08[0] == 0xff)
        {
          phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6F08[read_offset + i + 1];
        }
        break;

      case 0x7fff6f09:
        if (UPDATED_BYTES_7FFF_6F09[0] == 0xff)
        {
          phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6F09[read_offset + i + 1];
        }
        break;

      case 0x7fff6f5b:
        if (UPDATED_BYTES_7FFF_6F5B[0] == 0xff)
        {
          phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6F5B[read_offset + i + 1];
        }
        break;

      case 0x7fff6f5c:
        if (UPDATED_BYTES_7FFF_6F5C[0] == 0xff)
        {
          phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6F5C[read_offset + i + 1];
        }
        break;

      case 0x7fff5f3b4f20:
        if (UPDATED_BYTES_7FFF_5F3B_4F20[0] == 0xff)
        {
          phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_5F3B_4F20[read_offset + i + 1];
        }
        break;

      case 0x7fff5f3b4f52:
        if (UPDATED_BYTES_7FFF_5F3B_4F52[0] == 0xff)
        {
          phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_5F3B_4F52[read_offset + i + 1];
        }
        break;

      case 0x7fff6fc4:
        if (UPDATED_BYTES_7FFF_6FC4[0] == 0xff)
        {
          phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6FC4[read_offset + i + 1];
        }
        break;

      case 0x7fff6f31:
        if (UPDATED_BYTES_7FFF_6F31[0] == 0xff)
        {
          phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6F31[read_offset + i + 1];
        }
        break;

      case 0x7fff6f7b:
        if (UPDATED_BYTES_7FFF_6F7B[0] == 0xff)
        {
          phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6F7B[read_offset + i + 1];
        }
        break;

      case 0x7fff6f56:
        if (UPDATED_BYTES_7FFF_6F56[0] == 0xff)
        {
          phone_command_get_file_data(0xffff, SIM_FILES_1);
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6F56[read_offset + i + 1];
        }
        break;

      case 0x7fff6f43:
        if (UPDATED_BYTES_7FFF_6F56[0] == 0xff)
        {
          /*to be updated */
        }
        for (i=0; i<tmp_length; i++)
        {
          *(bytes_read + i) = UPDATED_BYTES_7FFF_6F43[read_offset + i + 1];
        }
        break;

      default:
  #if (IF_PRINT_LOG)
			  if (UART_LOG)
			  {
          printf("Wrong file ID in phone_command_read_binary_mandatory, error...\r\n");
          printf_selected_file(SELECTED_FILE);
				}
	#endif
        /* write SIM response to phone, Command not allowed, no EF selected */    
        write_bytes(2, tmp_bytes, etu_length, PIN_DATA_PHONE);
      
        return(1);
    }
  }
    
  write_bytes(1, confirm_bytes, etu_length, PIN_DATA_PHONE);
  
  write_bytes(tmp_length, bytes_read, etu_length, PIN_DATA_PHONE);
  
  write_bytes(2, status_bytes, etu_length, PIN_DATA_PHONE);
  
  free(bytes_read);
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_update_binary_mandatory(uint8_t *phone_command, uint32_t etu_length)
/*--------------------------------------------------------------------------------
| phone command 0xd6: update binary for necessary EFs
|
--------------------------------------------------------------------------------*/
{
  uint32_t update_offset;
  uint32_t i;
  uint32_t tmp_length;
  uint8_t tmp_bytes[2] = {0x69, 0x86};
  uint8_t confirm_bytes[1] = {0xd6};
  uint8_t status_bytes[2] = {0x90, 0x0};

  if (USAT_BYTE_LENGTH_BACK)
  {
    status_bytes[0] = 0x91;
    status_bytes[1] = USAT_BYTE_LENGTH_BACK;
  }
  
  if ((*(phone_command + 5)) == 0x00)
    tmp_length = 256;
  else
    tmp_length = (*(phone_command + 5));
  
#if (IF_PRINT_LOG)    
	if (UART_LOG)
	{
    printf_log_tx(5, phone_command + 1);
	}
#endif
  
  if ((*(phone_command + 3)) & 0x80)
  {
    update_offset = *(phone_command + 4);
  }
  else
  {
    update_offset = (*(phone_command + 3)) * 0x100 + (*(phone_command + 4));
  }
  
  write_bytes(1, confirm_bytes, etu_length, PIN_DATA_PHONE);
  
  if (read_bytes_phone(*(phone_command + 5), INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
  {
    return(1);
  }
  
  switch (SELECTED_FILE)
  {
    case 0x7fff6f08:
      for (i=0; i<tmp_length; i++)
      {
        UPDATED_BYTES_7FFF_6F08[update_offset + i + 1] = *(INFO_BYTE + 1 + i);
      }
      break;
      
    case 0x7fff6f09:
      for (i=0; i<tmp_length; i++)
      {
        UPDATED_BYTES_7FFF_6F09[update_offset + i + 1] = *(INFO_BYTE + 1 + i);
      }
      break;
      
    case 0x7fff6f5b:
      for (i=0; i<tmp_length; i++)
      {
        UPDATED_BYTES_7FFF_6F5B[update_offset + i + 1] = *(INFO_BYTE + 1 + i);
      }
      break;
      
    case 0x7fff5f3b4f52:
      for (i=0; i<tmp_length; i++)
      {
        UPDATED_BYTES_7FFF_5F3B_4F52[update_offset + i + 1] = *(INFO_BYTE + 1 + i);
      }
      break;
      
    case 0x7fff5f3b4f20:
      for (i=0; i<tmp_length; i++)
      {
        UPDATED_BYTES_7FFF_5F3B_4F20[update_offset + i + 1] = *(INFO_BYTE + 1 + i);
      }
      break;
      
    case 0x7fff6f73:
      for (i=0; i<tmp_length; i++)
      {
        UPDATED_BYTES_7FFF_6F73[update_offset + i + 1] = *(INFO_BYTE + 1 + i);
      }
      break;
      
    case 0x7fff6f7e:
      for (i=0; i<tmp_length; i++)
      {
        UPDATED_BYTES_7FFF_6F7E[update_offset + i + 1] = *(INFO_BYTE + 1 + i);
      }
      break;
      
    case 0x7fff6f7b:
      for (i=0; i<tmp_length; i++)
      {
        UPDATED_BYTES_7FFF_6F7B[update_offset + i + 1] = *(INFO_BYTE + 1 + i);
      }
      break;
      
    case 0x7fff6f43:
      for (i=0; i<tmp_length; i++)
      {
        UPDATED_BYTES_7FFF_6F43[update_offset + i + 1] = *(INFO_BYTE + 1 + i);
      }
      break;
      
    default:
#if (IF_PRINT_LOG)
		  if (UART_LOG)
		  {
        printf("update binary file in phone_command_update_binary_mandatory error......\r\n");
        printf_selected_file(SELECTED_FILE);
			}
#endif
      /* write SIM response to phone, Command not allowed, no EF selected */    
      write_bytes(2, tmp_bytes, etu_length, PIN_DATA_PHONE);
    
      return(1);
  }
  
  write_bytes(2, status_bytes, etu_length, PIN_DATA_PHONE);

  return(0);  
}  

/********************************************************************************/
uint32_t phone_command_read_record_mandatory(uint8_t *phone_command, uint32_t etu_length)
/*--------------------------------------------------------------------------------
| phone command 0xb2: read record for necessary EFs
|
--------------------------------------------------------------------------------*/
{
  uint8_t *bytes_read;
  uint32_t read_offset;
  uint32_t i;
  uint32_t tmp_length;
  uint8_t tmp_bytes[2] = {0x69, 0x86};
  uint8_t confirm_bytes[1] = {0xb2};
  uint8_t status_bytes[2] = {0x90, 0x0};
  
  if (USAT_BYTE_LENGTH_BACK)
  {
    status_bytes[0] = 0x91;
    status_bytes[1] = USAT_BYTE_LENGTH_BACK;
  }
  
  if ((*(phone_command + 5)) == 0x00)
    tmp_length = 256;
  else
    tmp_length = (*(phone_command + 5));
    
#if (IF_PRINT_LOG)    
	if (UART_LOG)
	{
    printf_log_tx(5, phone_command + 1);
	}
#endif
  
  bytes_read = (uint8_t *)malloc(*(phone_command + 5) * sizeof(uint8_t));
  if (bytes_read == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
      printf("memory allocation error...\r\n");
		}
#endif
    
    return(1);
  }
  
  if ((*(phone_command + 3)))
  {
    if (SELECTED_FILE == 0x7fff6f3c)
    {
      read_offset = 0;
    }
    else
    {
      read_offset = (*(phone_command + 3) - 1) * (*(phone_command + 5));
    }
  }
  else
  {
    /* to be added */
    read_offset = 0;
  }
  
  switch (SELECTED_FILE)
  {
    case 0x2f00:
      for (i=0; i<tmp_length; i++)
      {
        *(bytes_read + i) = UPDATED_BYTES_2F00[read_offset + i + 1];
      }
      break;
            
    case 0x7fff6fb7:
      for (i=0; i<tmp_length; i++)
      {
        *(bytes_read + i) = UPDATED_BYTES_7FFF_6FB7[read_offset + i + 1];
      }
      break;
      
    case 0x7fff6f06:
      for (i=0; i<tmp_length; i++)
      {
        *(bytes_read + i) = UPDATED_BYTES_7FFF_6F06[read_offset + i + 1];
      }
      break;

    case 0x7fff6f49:
      for (i=0; i<tmp_length; i++)
      {
        *(bytes_read + i) = UPDATED_BYTES_7FFF_6F49[read_offset + i + 1];
      }
      break;

    case 0x7fff6f4b:
      for (i=0; i<tmp_length; i++)
      {
        *(bytes_read + i) = UPDATED_BYTES_7FFF_6F4B[read_offset + i + 1];
      }
      break;

    case 0x7fff6f3c:
      for (i=0; i<tmp_length; i++)
      {
        *(bytes_read + i) = UPDATED_BYTES_7FFF_6F3C[read_offset + i + 1];
      }
      break;

    case 0x7fff6f42:
      for (i=0; i<tmp_length; i++)
      {
        *(bytes_read + i) = UPDATED_BYTES_7FFF_6F42[read_offset + i + 1];
      }
      break;

    default:
#if (IF_PRINT_LOG)
  		if (UART_LOG)
		  {
        printf("read record file in phone_command_read_record_mandatory error......\r\n");
        printf_selected_file(SELECTED_FILE);
			}
#endif
      /* write SIM response to phone, Command not allowed, no EF selected */    
      write_bytes(2, tmp_bytes, etu_length, PIN_DATA_PHONE);
        
      return(1);
  }
  
  write_bytes(1, confirm_bytes, etu_length, PIN_DATA_PHONE);
  
  write_bytes(tmp_length, bytes_read, etu_length, PIN_DATA_PHONE);
  
  write_bytes(2, status_bytes, etu_length, PIN_DATA_PHONE);    
  
  free(bytes_read);
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_update_record(uint8_t *phone_command, uint32_t etu_length)
/*--------------------------------------------------------------------------------
| phone command 0xdc: update record
|
--------------------------------------------------------------------------------*/
{
   uint8_t status_bytes[2] = {0x69, 0x85};
   
   write_bytes(2, status_bytes, etu_length, PIN_DATA_PHONE); 
      	
   return(0);
}

/********************************************************************************/
uint32_t authenticate_sim_response(uint32_t etu_length)
/*--------------------------------------------------------------------------------
| phone command 0x88: authentication locally
|
--------------------------------------------------------------------------------*/
{
  uint8_t authenticate_confirm = 0x88;
  uint8_t authenticate_get_length[0x2] = {0x61, 0x35};
  uint8_t authenticate_data[0x38] = {0xc0, 0xdb,  0x8,  0xca,  0xcc,  0xc9,  0x75,  0x2b,  0x57,  0xf6,  0x6a,  0x10,  0x7f,  0xda,  0x44,  0xa,  0xec,  0xfa,  0xfb,  0xbe,  0x1c,  0x4c,  0x52,  0x16,  0x69,  0x42,  0x5c,  0x6f,  0x10,  0x66,  0xef,  0x73,  0xb4,  0x24,  0xbf,  0x3e,  0x41,  0x9a,  0x6c,  0x7d,  0x1b,  0xdf,  0x9e,  0xf1,  0xdf,  0x8,  0x9f,  0x15,  0x18,  0xb3,  0x7e,  0x99,  0x68,  0x4f,  0x90,  0x0};
    
  write_bytes(0x1, &authenticate_confirm, etu_length, PIN_DATA_PHONE);
  if (read_bytes_phone(0x22, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
  {
    return(1);
  }
  write_bytes(0x2, authenticate_get_length, etu_length, PIN_DATA_PHONE);
  if (read_bytes_phone(0x5, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
  {
    return(1);
  }
  write_bytes(0x38, authenticate_data, etu_length, PIN_DATA_PHONE);
  
  return(0);
}

/********************************************************************************/
void printf_selected_file(uint64_t selected_file)
/*--------------------------------------------------------------------------------
| ptint the selected fine name
|
--------------------------------------------------------------------------------*/
{
  printf("selected file: %x, %x, %x, %x, %x, %x\r\n", (uint8_t)(selected_file >> 40), (uint8_t)(selected_file >> 32), 
    (uint8_t)(selected_file >> 24), (uint8_t)(selected_file >> 16), (uint8_t)(selected_file >> 8), (uint8_t)selected_file);
}

