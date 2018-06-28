#include "define_config.h"
#include "comm_phone_command.h"
#include "comm_UICC_terminal_interface.h"
#include "comm_misc.h"
#include "wireless_sim_phone.h"
#include "flash_file_address.h"
#include "phone_wireless_interface.h"

#if (ARGET_CHIP == NRF52)
  static nrf_saadc_value_t     m_buffer_pool[2][1];
  static int32_t               SAADC_VALUE;
#endif

/* 0:  phone book index number, 
|  1: name length, 2 ~ (PHONE_BOOK_NUMBER_START_INDEX - 1): name 
|  PHONE_BOOK_NUMBER_START_INDEX: phone number length, 
|  (PHONE_BOOK_NUMBER_START_INDEX + 1) ~ (PHONE_BOOK_LENGTH_CHAR - 1): phone number */
uint8_t PHONE_BOOK[PHONE_BOOK_LENGTH_CHAR]; 
/* 0:  SMS index number, 
|  1: SMS content length in char number,
|  2 ~ (PREDEFINED_SMS_LENGTH_CHAR - 1): SMS content */
uint8_t PREDEFINED_SMS[PREDEFINED_SMS_LENGTH_CHAR]; 
uint8_t PHONE_BOOK_INDEX_STRING[] = "Phone Book Index Added";
uint8_t PHONE_BOOK_NAME_STRING[] = "Phone Book Name Added";
uint8_t PHONE_BOOK_NUMBER_STRING[] = "Phone Book Number Added";
uint8_t SMS_INDEX_STRING[] = "SMS Index Added";
uint8_t SMS_CONTENT_STRING[] = "SMS Content Added";
volatile uint8_t FLAG_USIM_INITIALIZATION = 0;   
volatile uint8_t FLAG_PASSWORD = 0;   
volatile uint8_t FLAG_SYSTEM_KEY = 0;   
volatile uint8_t FLAG_WATCH_CALL_SWITCH = 0;   
volatile uint8_t IF_CONNECTED_PHONE = 0;   
  
uint8_t SIM_ON_STRING_PHONE[] = "Wireless SIM On, Address: ";
uint8_t SIM_OFF_STRING_PHONE[] = "Wireless SIM Off, Address: ";
uint8_t SIM_ON_CONNECTED_STRING_PHONE[] = "Wireless SIM On, Connected";  
uint8_t SIM_ON_DISCONNECTED_STRING_PHONE[] = "Wireless SIM On, Disonnected";  
uint8_t WATCH_CALL_ON_STRING[] = "Watch Call On Request Send";
uint8_t WATCH_CALL_OFF_STRING[] = "Watch Call Off Request Send";
  
uint8_t SIM_RESET[] = "Wireless SIM Reseted";
uint8_t SIM_ADD[] = "Set Phone SIM Logical Address as: ";
uint8_t NAME_CHG[] = "Phone Name Changed";
uint8_t USIM_INITIAL_REQ[] = "USIM Initialization Request Send";
uint8_t PASSWORD_REQ[] = "Password Change Request Send";
uint8_t SYSTEM_KEY_REQ[] = "System Key Change Request Send";
uint8_t INPUT_PASSWORD[KEY_LENGTH];
uint8_t INPUT_SYSTEM_KEY[KEY_LENGTH];
  
extern uint8_t UPDATED_BYTES_2FE2[0xb];
extern uint8_t UPDATED_BYTES_2F05[0x5];
extern uint8_t UPDATED_BYTES_7FFF_6F07[0x10];

uint8_t ABOUT_STRING[] = "Wireless SIM designed by Jiacheng Wang (+86-13641015183, jiacheng.wang AT icloud.com). Chinese patent pending";
uint8_t SIM_ON_STRING[] = "Wireless SIM On";
uint8_t SIM_OFF_STRING[] = "Wireless SIM Off";
uint8_t RX_CALL[] = "Set as Default Call Receiving Phone";
uint8_t PASSWORD_ON_STRING[] =  "Password Check On";
uint8_t PASSWORD_OFF_STRING[] = "Password Check Off";
uint8_t PASSWD_CHG[] = "Wireless SIM Password Changed";
uint8_t SYATEM_KEY_UPDATE_STRING[] = "Wireless SIM System Key Updated";
uint8_t SETTING_STRING[] = "S3 Watch Settings";
uint8_t WATCH_PHONE_NAME[16] = {15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 
volatile uint8_t WIRELESS_SIM_ON = 1;
volatile uint8_t WATCH_CALL_ON_FLASH = 1;
volatile uint8_t WATCH_CALL_ON = 1;
volatile uint32_t CONNECTION_STATE = 0x00000003;
uint8_t USER_PASSWORD[KEY_LENGTH] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'}; 
uint8_t SYSTEM_KEY[KEY_LENGTH] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'}; 
volatile uint32_t USAT_BYTE_LENGTH_BACK = 0;
volatile uint8_t FETCH_COMMAND_MENU = 0;
uint8_t PHONE_COMMAND[6];
uint8_t INFO_BYTE[PACKET_MAX_SIZE + 20];
uint8_t CHECK_BIT[PACKET_MAX_SIZE + 20];

/* sensor data of body temperature */
extern uint8_t BODY_TEMPERATURE[256];
/* sensor data of heart rate */
extern uint8_t HEART_RATE[256];
/* sensor data of ECG value */
extern uint8_t ECG_VALUE[256];
/* sensor data of blood presure */
extern uint8_t BLOOD_PRESURE[256];
/* sensor data of blood oxygen */
extern uint8_t BLOOD_OXYGEN[256];

uint8_t TEXT_SENSOR_DATA_BODY_TEMPERATURE[] = "Body Temperature: ";
uint8_t TEXT_SENSOR_DATA_HEART_RATE[] = "Heart Rate: ";
uint8_t TEXT_SENSOR_DATA_ECG_VALUE[] = "ECG Value: ";
uint8_t TEXT_SENSOR_DATA_BLOOD_PRESURE[] = "Blood Presure: ";
uint8_t TEXT_SENSOR_DATA_BLOOD_OXYGEN[] = "Blood Oxygen: ";

extern uint8_t MASK_RECEIVE_CALL;
extern uint8_t PHONE_ADD_LOGICAL;
extern uint8_t SCREEN_ON;
extern uint8_t SCREEN_STATE;
extern volatile uint32_t ETU_TICKS_PHONE;
extern volatile uint32_t ETU_TICKS_PHONE_INITIAL;
extern uint8_t UART_LOG;

/********************************************************************************/
void phone_sim_pin_setup(void)
/*--------------------------------------------------------------------------------
| Phone-SIM interface GPIO pin setting
|
--------------------------------------------------------------------------------*/
{
  nrf_gpio_cfg_input(PIN_RESET_PHONE, NRF_GPIO_PIN_PULLUP);
  nrf_gpio_cfg_input(PIN_CLOCK_PHONE, NRF_GPIO_PIN_PULLUP);
  nrf_gpio_cfg_output_S0D1(PIN_DATA_PHONE);
  nrf_gpio_pin_write(PIN_DATA_PHONE, 1);  
}

#if (ARGET_CHIP == NRF52)
/********************************************************************************/
void saadc_callback(nrf_drv_saadc_evt_t const *p_event)
/*--------------------------------------------------------------------------------
| Handler for SAADC events
|
--------------------------------------------------------------------------------*/
{
  if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
  {
		nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, 1);
  
    SAADC_VALUE = p_event->data.done.p_buffer[0];
  }
}

/********************************************************************************/
void saadc_init(uint8_t channel_number)
/*--------------------------------------------------------------------------------
| SAADC initialization, the default resolution defined in nrf_drv_config.h
|
--------------------------------------------------------------------------------*/
{
  // ret_code_t err_code;
  nrf_saadc_input_t pin_reset_phone; 
   
  /* nRF52 analog-to-digital PIN selection for phone RESET pin */  
   
  switch(PIN_RESET_PHONE)
  {
  case 2:
    pin_reset_phone = NRF_SAADC_INPUT_AIN0;
    break;
  
  case 3:
    pin_reset_phone = NRF_SAADC_INPUT_AIN1;
    break;
  
  case 4:
    pin_reset_phone = NRF_SAADC_INPUT_AIN2;
    break;
  
  case 5:
    pin_reset_phone = NRF_SAADC_INPUT_AIN3;
    break;
  
  case 28:
    pin_reset_phone = NRF_SAADC_INPUT_AIN4;
    break;
  
  case 29:
    pin_reset_phone = NRF_SAADC_INPUT_AIN5;
    break;
  
  case 30:
    pin_reset_phone = NRF_SAADC_INPUT_AIN6;
    break;
  
  case 31:
    pin_reset_phone = NRF_SAADC_INPUT_AIN7;
    break;
  
  default:
    pin_reset_phone = NRF_SAADC_INPUT_DISABLED;
    break;
  }  
  
  nrf_saadc_channel_config_t channel_config =
      NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(pin_reset_phone);

  nrf_drv_saadc_init(NULL, saadc_callback);

  nrf_drv_saadc_channel_init(channel_number, &channel_config);
  
  nrf_drv_saadc_buffer_convert(m_buffer_pool[0], 1);
  
  nrf_drv_saadc_buffer_convert(m_buffer_pool[1], 1);
}

/********************************************************************************/
int32_t saadc_sample_value_get(void)
/*--------------------------------------------------------------------------------
| get one ADC sample value
|
--------------------------------------------------------------------------------*/
{
  uint32_t sample_value;
  uint8_t channel_number = 0;

  saadc_init(channel_number);
  
  NRF_SAADC->TASKS_SAMPLE = 1;
  
  /* wait for the ADC result transfer, 1 sample value */
  while (NRF_SAADC->RESULT.AMOUNT == 0)				
  {
  }

  sample_value = SAADC_VALUE;
  
  nrf_drv_saadc_channel_uninit(channel_number);
  nrf_drv_saadc_uninit( );
  return (sample_value);
}

#elif (ARGET_CHIP == NRF51)
/********************************************************************************/
void adc_initialization(void)
/*--------------------------------------------------------------------------------
| ADC initialization for nRF51
|
--------------------------------------------------------------------------------*/
{
	
  // config ADC
  NRF_ADC->CONFIG	= (ADC_CONFIG_EXTREFSEL_None << ADC_CONFIG_EXTREFSEL_Pos) 
                    | (ADC_CONFIG_REFSEL_SupplyOneThirdPrescaling << ADC_CONFIG_REFSEL_Pos)	
                    | (ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos) 	
                    | (ADC_CONFIG_RES_8bit << ADC_CONFIG_RES_Pos);	/*!< 8bit ADC resolution. */ 
#if (PIN_RESET_PHONE == 26)
  NRF_ADC->CONFIG	|= (ADC_CONFIG_PSEL_AnalogInput0 << ADC_CONFIG_PSEL_Pos);	
#elif (PIN_RESET_PHONE == 27)
  NRF_ADC->CONFIG	|= (ADC_CONFIG_PSEL_AnalogInput1 << ADC_CONFIG_PSEL_Pos);	
#elif (PIN_RESET_PHONE == 1)
  NRF_ADC->CONFIG	|= (ADC_CONFIG_PSEL_AnalogInput2 << ADC_CONFIG_PSEL_Pos);	
#elif (PIN_RESET_PHONE == 2)
  NRF_ADC->CONFIG	|= (ADC_CONFIG_PSEL_AnalogInput3 << ADC_CONFIG_PSEL_Pos);	
#elif (PIN_RESET_PHONE == 3)
  NRF_ADC->CONFIG	|= (ADC_CONFIG_PSEL_AnalogInput4 << ADC_CONFIG_PSEL_Pos);	
#elif (PIN_RESET_PHONE == 4)
  NRF_ADC->CONFIG	|= (ADC_CONFIG_PSEL_AnalogInput5 << ADC_CONFIG_PSEL_Pos);	
#elif (PIN_RESET_PHONE == 5)
  NRF_ADC->CONFIG	|= (ADC_CONFIG_PSEL_AnalogInput6 << ADC_CONFIG_PSEL_Pos);	
#elif (PIN_RESET_PHONE == 6)
  NRF_ADC->CONFIG	|= (ADC_CONFIG_PSEL_AnalogInput7 << ADC_CONFIG_PSEL_Pos);	
#endif
}

/********************************************************************************/
int32_t saadc_sample_value_get(void)
/*--------------------------------------------------------------------------------
| get one ADC sample value
|
--------------------------------------------------------------------------------*/
{
  uint32_t sample_value;

  NRF_ADC->TASKS_START = 1;
  while (NRF_ADC->EVENTS_END == 0)				
  {
  }
  
  sample_value = (int32_t)NRF_ADC->RESULT;
  
  return (sample_value);
}
#endif

/* =========== to reduce power consumption, use GPIOTE port event ============= */
/********************************************************************************/
void gpio_event_in_config(uint32_t clock_pin)
/*--------------------------------------------------------------------------------
| GPIO pin input event interrupt configuration
|
--------------------------------------------------------------------------------*/
{
  /* Enable interrupt */
  NVIC_EnableIRQ(GPIOTE_IRQn);
  NVIC_SetPriority(GPIOTE_IRQn, GPIOTE_IRQ_PRIORITY); 

  /* GPIOTE channel 2 */
  nrf_gpio_cfg_sense_input(clock_pin, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_HIGH);
  NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_PORT_Msk; 
}

#if (SENSOR == 0)
/********************************************************************************/
void GPIOTE_IRQHandler(void)
/*--------------------------------------------------------------------------------
| phone GPIO input event interrupt handler
|
--------------------------------------------------------------------------------*/
{
  static volatile uint8_t CLOCK_PIN_COUNTER;

  /* Event from clock signal interupt */
  if ((NRF_GPIOTE->EVENTS_PORT != 0) && (NRF_GPIOTE->INTENSET & GPIOTE_INTENSET_PORT_Msk))
  {
    NRF_GPIOTE->EVENTS_PORT = 0;
    CLOCK_PIN_COUNTER++;
    
    if (CLOCK_PIN_COUNTER > 3)
    {
      /* disable GPIO pin in event from clock signal interrupt */
      NVIC_DisableIRQ(GPIOTE_IRQn);  
      CLOCK_PIN_COUNTER = 0;
      
      /* read phone command (5 bytes) form PIN_DATA_PHONE */
      if (read_phone_command(PHONE_COMMAND, CHECK_BIT, PIN_DATA_PHONE, ETU_TICKS_PHONE))
      {
        /* enable GPIO pin in event from clock signal interrupt, waiting for the next phone command session */  
        NVIC_EnableIRQ(GPIOTE_IRQn);              
        
        return;  
      }
	  
      /* phone command session */
      main_smart_phone_sim_local_wireless( );  
	  
      /* enable GPIO pin in event from clock signal interrupt, waiting for the next phone command session */  
      NVIC_EnableIRQ(GPIOTE_IRQn);              
    }       
  }
}
/* ============================================================================ */	
#endif

/********************************************************************************/
uint32_t read_phone_command(uint8_t *bytes_command, uint8_t *parity_bit, uint32_t pin_number, 
     uint32_t etu_length)
/*--------------------------------------------------------------------------------
| phone command read, 5 bytes
|
--------------------------------------------------------------------------------*/
{
  uint8_t i;  
  uint32_t initial_timer;

  /* set CC register value for timer0 interrupt event time interval, for ETU timing */
  NRF_TIMER0->CC[0] = etu_length;
  NRF_TIMER0->TASKS_START = 1;
  /* get the initial real time counter */
  initial_timer = NRF_RTC0->COUNTER;
  
  /* re-load watch dog request register */
  NRF_WDT->RR[0] = 0x6E524635;
  
  /* read command from phone, 5 bytes */  
  for (i=1; i<6; i++)
  {
    while (nrf_gpio_pin_read(pin_number))
    {
      /* reset signal from phone is read */
      if (nrf_gpio_pin_read(PIN_RESET_PHONE) == 0)
      { 
        nrf_delay_us(10);
        if (nrf_gpio_pin_read(PIN_RESET_PHONE) == 0)
        { 
          /* stop ETU timer tick */
          NRF_TIMER0->TASKS_STOP = 1;
          
          /* reset session */
          if (read_soft_warm_reset_phone(ETU_TICKS_PHONE_INITIAL, pin_number))
          {
            *(PHONE_COMMAND + 2) = 0;

            return(1);
          }
    
          /* re-set CC register value for timer0 interrupt event time interval, for ETU timing */
          NRF_TIMER0->CC[0] = etu_length;
          NRF_TIMER0->TASKS_START = 1;
          /* re-set time */
          initial_timer = NRF_RTC0->COUNTER;
          i = 1;
        }
      }
    
      /* read phone command time out */
      if ((NRF_RTC0->COUNTER - initial_timer) > PHONE_COMMAND_TIME_OUT_MS)
      {
        *(PHONE_COMMAND + 2) = 0;
         /* stop ETU timer tick */
        NRF_TIMER0->TASKS_STOP = 1;
           
        return(1);
      }
    }
  
    read_byte(bytes_command + i, parity_bit + i, etu_length, pin_number);
  
    /* clock signal, false alarm read, discard the byte */  
    if (*(bytes_command + 1) == 0xff && *(parity_bit + 1) == 0x1)
    {
      i = 1;
    }
  }

#if (IF_PRINT_LOG)  
  if (UART_LOG)
	{
    printf("*********************** NRF_RTC0->COUNTER = %ld, **********************\r\n", NRF_RTC0->COUNTER);
    printf_log_tx(5, bytes_command + 1);
	}
#endif  
#if (PRINT_DELAY_MS)
  nrf_delay_ms(PRINT_DELAY_MS);
#endif

  /* re-set real time counter 0 */  
  NRF_RTC0->TASKS_CLEAR = 1;
  while (NRF_RTC0->COUNTER)
  {
    NRF_RTC0->TASKS_CLEAR = 1;
  }
      
  /* stop ETU timer tick */
  NRF_TIMER0->TASKS_STOP = 1;
  
  return(0);
}

/********************************************************************************/
uint32_t read_soft_warm_reset_phone(uint32_t etu_ticks_initial, uint32_t pin_number)
/*--------------------------------------------------------------------------------
| warm reset phone-SIM session
|
--------------------------------------------------------------------------------*/
{
  uint32_t initial_timer;
  int32_t saadc_value;
  uint32_t return_value_reset;
  
  /* get the initial real time counter */
  initial_timer = NRF_RTC0->COUNTER;
  
#if (ARGET_CHIP == NRF51)
  NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;
#endif  
  while (1)
  {
    /* get ADC sample value from Reset PIN */
#if (ARGET_CHIP == NRF52)
    saadc_value = (int32_t)saadc_sample_value_get( );
#elif (ARGET_CHIP == NRF51)
    NRF_ADC->TASKS_START = 1;
    while (NRF_ADC->EVENTS_END == 0)        
    {
    }
    saadc_value = (int32_t)(NRF_ADC->RESULT);
#endif 
 
    /* the ADC resolution is 8bit, the ADC value is 256 if high */
    if (saadc_value > 200)
    {  
#if (IF_PRINT_LOG)  
		  if (UART_LOG)
			{
        printf("------------------ NRF_ADC->RESULT = %ld, \r\n", saadc_value);
			}
#else
      nrf_delay_ms(1);
#endif

#if (ARGET_CHIP == NRF51)
      NRF_ADC->TASKS_STOP = 1;
      NRF_ADC->ENABLE =  ADC_ENABLE_ENABLE_Disabled;
#endif
  
      break;
    }
    else
    {
#if (IF_PRINT_LOG)  
		  if (UART_LOG)
			{
        printf("================== NRF_ADC->RESULT = %ld, \r\n", saadc_value);
			}
#else
      nrf_delay_ms(1);
#endif
    }
  
    if((NRF_RTC0->COUNTER - initial_timer) > PHONE_COMMAND_TIME_OUT_MS)
    {
      *(PHONE_COMMAND + 2) = 0;
    
#if (IF_PRINT_LOG)  
		  if (UART_LOG)
			{
        printf("Reset session read Reset pin time out ......, time used = %ldms\r\n", NRF_RTC0->COUNTER - initial_timer);
			}
#endif
#if (ARGET_CHIP == NRF51)
      NRF_ADC->TASKS_STOP = 1;
      NRF_ADC->ENABLE =  ADC_ENABLE_ENABLE_Disabled;
#endif
      /* time out return */
      return(1);
    }
  }
    
  NRF_TIMER0->CC[0] = etu_ticks_initial;
  NRF_TIMER0->TASKS_START = 1;
  /* reset UICC-terminal session at phone side */
  return_value_reset = soft_warm_reset_phone(etu_ticks_initial, pin_number);  
  NRF_TIMER0->TASKS_STOP = 1;
  
  return (return_value_reset);
}

/********************************************************************************/
uint32_t soft_warm_reset_phone(uint32_t etu_length, uint32_t pin_number_io)
/*--------------------------------------------------------------------------------
| read/write initial bytes og phone-SIM reset session
|
--------------------------------------------------------------------------------*/
{
  uint32_t i;
  uint32_t initial_timer;

  /* get the initial real time counter */
  initial_timer = NRF_RTC0->COUNTER;
  
  /* write TS byte to phone */  
  nrf_delay_ms(1); 
  write_byte(0x3b, 1, etu_length, pin_number_io);
 
  /* ATR information bytes */
  INFO_BYTE[0] = 0x9d;
  INFO_BYTE[1] = 0x94;  
  INFO_BYTE[2] = 0x80;  
  INFO_BYTE[3] = 0x1f;  
  INFO_BYTE[4] = 0xc7;
  INFO_BYTE[5] = 0x80;
  INFO_BYTE[6] = 0x31;
  INFO_BYTE[7] = 0xe0;
  INFO_BYTE[8] = 0x73;
  INFO_BYTE[9] = 0xfe;
  INFO_BYTE[10] = 0x21;
  INFO_BYTE[11] = 0x13;
  INFO_BYTE[12] = 0x65;
  INFO_BYTE[13] = 0xd0;
  INFO_BYTE[14] = 0x1;
  INFO_BYTE[15] = 0x87;
  INFO_BYTE[16] = 0xf;
  INFO_BYTE[17] = 0x75;
  INFO_BYTE[18] = 0xf6;

  for (i=0; i<19; i++)
  {
    get_parity_bit(INFO_BYTE[i], CHECK_BIT + i);
  }
  
#if (IF_PRINT_LOG)  
  if (UART_LOG)
	{
    printf_log_rx(19, INFO_BYTE);
	}
#endif  
#if (PRINT_DELAY_MS)
  nrf_delay_ms(PRINT_DELAY_MS);
#endif

  nrf_delay_ms(10);

  /* write ATR bytes to phone */
  for (i=0; i<19; i++)
  {
    nrf_delay_us(100);
    write_byte(*(INFO_BYTE + i), *(CHECK_BIT + i), etu_length, pin_number_io);
  }
  
  /* re-load watch dog request register */
  NRF_WDT->RR[0] = 0x6E524635;
  /* read PPS bytes from phone */
  for (i=0; i<4; i++)
  {
    while (nrf_gpio_pin_read(pin_number_io))
    {
      if ((NRF_RTC0->COUNTER - initial_timer) > (DATA_TX_TIME << 1))
      {
        return(1);
      }    
    }
    
    nrf_delay_us(10);
    read_byte(INFO_BYTE + i, CHECK_BIT + i, etu_length, pin_number_io);
  }
    
  /* re-load watch dog request register */
  NRF_WDT->RR[0] = 0x6E524635;
  
#if (IF_PRINT_LOG)
  if (UART_LOG)
	{
    printf_log_tx(4, INFO_BYTE);
	}
#endif  
#if (PRINT_DELAY_MS)
  nrf_delay_ms(PRINT_DELAY_MS);
#endif

  nrf_delay_ms(10);
  
  /* PPS bytes */
  INFO_BYTE[0] = 0xff;
  INFO_BYTE[1] = 0x10;
  INFO_BYTE[2] = 0x94;
  INFO_BYTE[3] = 0x7b;  

  for (i=0; i<4; i++)
  {
    get_parity_bit(INFO_BYTE[i], CHECK_BIT + i);
  }

#if (IF_PRINT_LOG)  
  if (UART_LOG)
	{
    printf_log_rx(4, INFO_BYTE);
	}
#endif  
#if (PRINT_DELAY_MS)
  nrf_delay_ms(PRINT_DELAY_MS);
#endif

  /* write PPS bytes to phone */
  for (i=0; i<4; i++)
  {
    write_byte(*(INFO_BYTE + i), *(CHECK_BIT + i), etu_length, pin_number_io);  
  }  
  
  return(0);
}  

/********************************************************************************/
uint32_t write_phone_response(uint32_t bytes_length, uint8_t *bytes_info, uint32_t etu_length)
/*--------------------------------------------------------------------------------
| phone command response feedback session, command 0xc0
|
--------------------------------------------------------------------------------*/
{
  uint32_t tmp_length = bytes_length + 3;
  uint32_t i;
  uint8_t tmp_bytes[tmp_length];
  
  tmp_bytes[0] = 0xc0;
  for (i=1; i<bytes_length + 1; i++)
  {
    tmp_bytes[i] = *(bytes_info + i - 1);
  }
  if (USAT_BYTE_LENGTH_BACK)
  {
    tmp_bytes[bytes_length + 1] = 0x91;
    tmp_bytes[bytes_length + 2] = USAT_BYTE_LENGTH_BACK;
  } 
  else
  { 
    tmp_bytes[bytes_length + 1] = 0x90;
    tmp_bytes[bytes_length + 2] = 0x00;
  }
   
  write_bytes (tmp_length, tmp_bytes, etu_length, PIN_DATA_PHONE);

  return(0);  
}

/********************************************************************************/
uint32_t phone_command_PIN(uint8_t *bytes_command, uint32_t etu_length_phone)
/*--------------------------------------------------------------------------------
| phone command session, supported phone command:
| 24: change PIN
| 26: disable PIN
| 28: enable PIN
|
--------------------------------------------------------------------------------*/
{
  uint32_t tmp_length;
  uint8_t tmp_bytes[2] = {0x90, 0x0};
  
  if ((*(bytes_command + 5)) == 0x00)
  {
    tmp_length = 256;
  }
  else
  {
    tmp_length = (*(bytes_command + 5));
  }
  
  /* write the SIM confirm bytes to phone */
  write_bytes(1, bytes_command + 2, etu_length_phone, PIN_DATA_PHONE);

  /* read phone command data */  
  if (read_bytes_phone(tmp_length, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length_phone))
  {  
    return(1);
  }  
  
  /* write the SIM returned status bytes to phone */
  write_bytes(2, tmp_bytes, etu_length_phone, PIN_DATA_PHONE);
   
  return(0);
}

/********************************************************************************/
uint32_t phone_command_manage_channel(uint8_t *bytes_command, uint32_t etu_length_phone)
/*--------------------------------------------------------------------------------
| phone command session, supported phone command:
| 70: manage channel 
|
--------------------------------------------------------------------------------*/
{
  uint8_t tmp_bytes[2] = {0x90, 0x0};
    
  /* write the SIM returned status bytes to phone */
  write_bytes(2, tmp_bytes, etu_length_phone, PIN_DATA_PHONE);
   
  return(0);
}

/********************************************************************************/
uint32_t phone_command_terminal_profile(uint8_t *phone_command, uint32_t etu_length, 
     uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| phone command terminal profile session, command 0x10, only read the data from 
| phone, not forward the data to SIM 
|
--------------------------------------------------------------------------------*/
{
  uint8_t phone_command_byte[1] = {0x10};
  uint8_t sim_status_byte[2] = {0x90, 0x0};
  
#if (IF_PRINT_LOG)  
  if (UART_LOG)
	{
    printf_log_tx(1, phone_command + 2);
	}
#endif  
#if (PRINT_DELAY_MS)
  nrf_delay_ms(PRINT_DELAY_MS);
#endif
  
  write_bytes(1, phone_command_byte, etu_length, pin_number_phone);  
   
  if(read_bytes_phone(*(phone_command + 5), INFO_BYTE, CHECK_BIT, pin_number_phone, etu_length))
  {
    return(1);
  }
  
  write_bytes(2, sim_status_byte, etu_length, pin_number_phone);  
  
  return(0);
}

/********************************************************************************/
void phone_command_fetch_all(uint8_t *bytes_command, uint32_t etu_length, uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| phone command terminal fetch session, command 0x12
|
--------------------------------------------------------------------------------*/
{
  /* USAT menu switch */  
  switch (FETCH_COMMAND_MENU)
  {
    case 100:
		{
      /* NAA reset */
      phone_command_fetch_reset(bytes_command, etu_length, pin_number_phone);
      /* clear the USAT NAA reset mark bit */
      CONNECTION_STATE &= 0xFFFFFFFB;
  
      /* set idle mode text to indicate Wireless SIM ON/OFF state */  
      if (WIRELESS_SIM_ON)
      {
  	    /* update phone idle text */
		    if (IF_CONNECTED_PHONE) 
		    {   
          FETCH_COMMAND_MENU = 150;  
  	      USAT_BYTE_LENGTH_BACK = 14 + strlen((char *)SIM_ON_CONNECTED_STRING_PHONE);  	
		    } 
		    else 
		    {   
          FETCH_COMMAND_MENU = 151;  
  	      USAT_BYTE_LENGTH_BACK = 14 + strlen((char *)SIM_ON_DISCONNECTED_STRING_PHONE);  	
		    } 
      }
      else
      {
        FETCH_COMMAND_MENU = 204;
        USAT_BYTE_LENGTH_BACK = 14 + strlen((char *)SIM_OFF_STRING_PHONE) + 1; 
      } 
	    break;
		}	  
	  	  
    case SENSOR_DATA_TYPE_BODY_TEMPERATURE:
		{
      phone_command_fetch_diaplay_sensor_data(bytes_command, strlen((char *)TEXT_SENSOR_DATA_BODY_TEMPERATURE), TEXT_SENSOR_DATA_BODY_TEMPERATURE, etu_length, pin_number_phone);  
		  FETCH_COMMAND_MENU = 0;  
      USAT_BYTE_LENGTH_BACK = 0;
      break;
		}
      
    case SENSOR_DATA_TYPE_HEART_RATE:
		{
      phone_command_fetch_diaplay_sensor_data(bytes_command, strlen((char *)TEXT_SENSOR_DATA_HEART_RATE), TEXT_SENSOR_DATA_HEART_RATE, etu_length, pin_number_phone);  
      FETCH_COMMAND_MENU = 0;  
      USAT_BYTE_LENGTH_BACK = 0;
      break;
		}
      
    case SENSOR_DATA_TYPE_ECG_VALUE:
		{
      phone_command_fetch_diaplay_sensor_data(bytes_command, strlen((char *)TEXT_SENSOR_DATA_ECG_VALUE), TEXT_SENSOR_DATA_ECG_VALUE, etu_length, pin_number_phone);  
      FETCH_COMMAND_MENU = 0;  
      USAT_BYTE_LENGTH_BACK = 0;
      break;
		}
      
    case SENSOR_DATA_TYPE_BLOOD_PRESURE:
		{
      phone_command_fetch_diaplay_sensor_data(bytes_command, strlen((char *)TEXT_SENSOR_DATA_BLOOD_PRESURE), TEXT_SENSOR_DATA_BLOOD_PRESURE, etu_length, pin_number_phone);  
      FETCH_COMMAND_MENU = 0;  
      USAT_BYTE_LENGTH_BACK = 0;
      break;
		}
      
    case SENSOR_DATA_TYPE_BLOOD_OXYGEN:
		{
      phone_command_fetch_diaplay_sensor_data(bytes_command, strlen((char *)TEXT_SENSOR_DATA_BLOOD_OXYGEN), TEXT_SENSOR_DATA_BLOOD_OXYGEN, etu_length, pin_number_phone);  
      FETCH_COMMAND_MENU = 0;  
      USAT_BYTE_LENGTH_BACK = 0;
      break;
		}
      

    case 6:
      phone_command_fetch_diaplay_text(bytes_command, strlen((char *)RX_CALL), RX_CALL, etu_length, pin_number_phone);  
      FETCH_COMMAND_MENU = 0;  
      USAT_BYTE_LENGTH_BACK = 0;
       
      /* mark the USAT bit for reset NAA, let phone/watch access the network */    
      CONNECTION_STATE |= 0x4;
      break;
      
    case 7:
      phone_command_fetch_at_cfun0(bytes_command, etu_length, pin_number_phone);  
      FETCH_COMMAND_MENU = 0;  
      USAT_BYTE_LENGTH_BACK = 0;
      break;
      
    case 10:
      phone_command_fetch_get_input_key(bytes_command, etu_length, pin_number_phone);  
      USAT_BYTE_LENGTH_BACK = 0;
      break;
      
	  case 20:
	    phone_command_fetch_diaplay_text(bytes_command, strlen((char *)SYATEM_KEY_UPDATE_STRING), 
		       SYATEM_KEY_UPDATE_STRING, etu_length, pin_number_phone);  
	    FETCH_COMMAND_MENU = 0;  
	    USAT_BYTE_LENGTH_BACK = 0;
	    break;
      
    case 101:
      phone_command_fetch_get_input_address(bytes_command, etu_length, pin_number_phone);  
      USAT_BYTE_LENGTH_BACK = 0;
      break;
      
    case 102:
      phone_command_fetch_diaplay_text(bytes_command, strlen((char *)SIM_RESET), SIM_RESET, etu_length, pin_number_phone);  
      FETCH_COMMAND_MENU = 0;  
      USAT_BYTE_LENGTH_BACK = 0;
      /* mark the USAT bit for reset NAA, let phone access the network */    
      CONNECTION_STATE |= 0x4;
      /* mark the first byte of EF data, upate the EF contents over the air */
      UPDATED_BYTES_2FE2[0] = 0xff;
      UPDATED_BYTES_2F05[0] = 0xff;
      UPDATED_BYTES_7FFF_6F07[0] = 0xff;
      break;

	  /* phone book item set up */
    case 103:
	    phone_command_fetch_set_item_phonebook(etu_length, pin_number_phone);
      FETCH_COMMAND_MENU = 0;  
      USAT_BYTE_LENGTH_BACK = 0;
      break;
    
	  /* watch remote configuration item setup */    
    case 104:
  	  phone_command_fetch_set_item_CFG(etu_length, pin_number_phone);
      FETCH_COMMAND_MENU = 0;  
      USAT_BYTE_LENGTH_BACK = 0;
      break;
        
	  /* phone book get input - index */
    case 121:
  	  phone_command_fetch_get_input_phonebook_index(etu_length, pin_number_phone);  
      USAT_BYTE_LENGTH_BACK = 0;
      break;
        
	  /* phone book get input - name */
    case 122:
      phone_command_fetch_get_input_phonebook_name(etu_length, pin_number_phone);  
      USAT_BYTE_LENGTH_BACK = 0;
      break;
        
  	/* phone book get input - name */
    case 123:
      phone_command_fetch_get_input_phonebook_number(etu_length, pin_number_phone);  
      USAT_BYTE_LENGTH_BACK = 0;
      break;
        
  	/* pre-defined SMS get input - index */
    case 124:
      phone_command_fetch_get_input_sms_index(etu_length, pin_number_phone);  
      USAT_BYTE_LENGTH_BACK = 0;
      break;
        
    /* pre-defined SMS get input - content */
    case 125:
      phone_command_fetch_get_input_sms_content(etu_length, pin_number_phone);  
      USAT_BYTE_LENGTH_BACK = 0;
      break;

    /* phone book get input - index, conformation information display */
    case 131:
      phone_command_fetch_diaplay_text(bytes_command, strlen((char *)PHONE_BOOK_INDEX_STRING), PHONE_BOOK_INDEX_STRING, etu_length, pin_number_phone);  
      USAT_BYTE_LENGTH_BACK = 0;
      FETCH_COMMAND_MENU = 0;  
      break;
		
    /* phone book get input - index, conformation information display */
    case 132:
      phone_command_fetch_diaplay_text(bytes_command, strlen((char *)PHONE_BOOK_NAME_STRING), PHONE_BOOK_NAME_STRING, etu_length, pin_number_phone);  
      USAT_BYTE_LENGTH_BACK = 0;
      FETCH_COMMAND_MENU = 0;  
      break;
		
    /* phone book get input - index, conformation information display */
    case 133:
      phone_command_fetch_diaplay_text(bytes_command, strlen((char *)PHONE_BOOK_NUMBER_STRING), PHONE_BOOK_NUMBER_STRING, etu_length, pin_number_phone);  
      USAT_BYTE_LENGTH_BACK = 0;
      FETCH_COMMAND_MENU = 0;  
      break;
		
    /* phone book get input - index, conformation information display */
    case 134:
      phone_command_fetch_diaplay_text(bytes_command, strlen((char *)SMS_INDEX_STRING), SMS_INDEX_STRING, etu_length, pin_number_phone);  
      USAT_BYTE_LENGTH_BACK = 0;
      FETCH_COMMAND_MENU = 0;  
      break;
		
    /* phone book get input - index, conformation information display */
    case 135:
      phone_command_fetch_diaplay_text(bytes_command, strlen((char *)SMS_CONTENT_STRING), SMS_CONTENT_STRING, etu_length, pin_number_phone);  
      USAT_BYTE_LENGTH_BACK = 0;
      FETCH_COMMAND_MENU = 0;  
      break;
	  	     
	  case 141:
      phone_command_fetch_diaplay_text(bytes_command, strlen((char *)USIM_INITIAL_REQ), USIM_INITIAL_REQ, etu_length, pin_number_phone);  
      FETCH_COMMAND_MENU = 0;  
      USAT_BYTE_LENGTH_BACK = 0;
      break;
	  
    case 142:
      phone_command_fetch_get_input(bytes_command, etu_length, pin_number_phone);  
      USAT_BYTE_LENGTH_BACK = 0;
      break;
      
    case 143:
      phone_command_fetch_get_input_key(bytes_command, etu_length, pin_number_phone);  
      USAT_BYTE_LENGTH_BACK = 0;
      break;
      
    /* watch remote configuration - watch call on/off switch information display */
	  case 146:
  	  if (WATCH_CALL_ON)
      {
    	  phone_command_fetch_diaplay_text(bytes_command, strlen((char *)WATCH_CALL_ON_STRING), WATCH_CALL_ON_STRING, etu_length, pin_number_phone);  			
      }
      else
  	  {
      	phone_command_fetch_diaplay_text(bytes_command, strlen((char *)WATCH_CALL_OFF_STRING), WATCH_CALL_OFF_STRING, etu_length, pin_number_phone);  						
      }
      break;
	  
    /* watch remote configuration - password, conformation information display */
    case 147:
      phone_command_fetch_diaplay_text(bytes_command, strlen((char *)PASSWORD_REQ), PASSWORD_REQ, etu_length, pin_number_phone);  
      USAT_BYTE_LENGTH_BACK = 0;
      FETCH_COMMAND_MENU = 0;  
      break;
	  	     
    /* watch remote configuration - system key, conformation information display */
    case 148:
      phone_command_fetch_diaplay_text(bytes_command, strlen((char *)SYSTEM_KEY_REQ), SYSTEM_KEY_REQ, etu_length, pin_number_phone);  
      USAT_BYTE_LENGTH_BACK = 0;
      FETCH_COMMAND_MENU = 0;  
      break;
	  	  
    case 150:
      phone_command_fetch_idle_mode_text(bytes_command, strlen((char *)SIM_ON_CONNECTED_STRING_PHONE), SIM_ON_CONNECTED_STRING_PHONE, etu_length, pin_number_phone);  
      USAT_BYTE_LENGTH_BACK = 0;
      FETCH_COMMAND_MENU = 0;  
      break;
	  
    case 151:
      phone_command_fetch_idle_mode_text(bytes_command, strlen((char *)SIM_ON_DISCONNECTED_STRING_PHONE), SIM_ON_DISCONNECTED_STRING_PHONE, etu_length, pin_number_phone);  
      USAT_BYTE_LENGTH_BACK = 0;
      FETCH_COMMAND_MENU = 0;  
      break;      
      
    case 203:
      phone_command_fetch_idle_mode_text_variable(bytes_command, strlen((char *)SIM_ON_STRING_PHONE) + 1, SIM_ON_STRING_PHONE, etu_length, pin_number_phone);  
      FETCH_COMMAND_MENU = 0;  
      USAT_BYTE_LENGTH_BACK = 0;
      break;
      
    case 204:
      phone_command_fetch_idle_mode_text_variable(bytes_command, strlen((char *)SIM_OFF_STRING_PHONE) + 1, SIM_OFF_STRING_PHONE, etu_length, pin_number_phone);  
      FETCH_COMMAND_MENU = 0;  
      USAT_BYTE_LENGTH_BACK = 0;
      break;
      
    case 205:
      phone_command_fetch_diaplay_text(bytes_command, strlen((char *)PASSWD_CHG), PASSWD_CHG, etu_length, pin_number_phone);  
      FETCH_COMMAND_MENU = 0;  
      USAT_BYTE_LENGTH_BACK = 0;
      break;
      
    case 206:
      phone_command_fetch_diaplay_text(bytes_command, strlen((char *)NAME_CHG), NAME_CHG, etu_length, pin_number_phone);  
      FETCH_COMMAND_MENU = 0;  
      USAT_BYTE_LENGTH_BACK = 0;
      break;
      
    case 207:
      phone_command_fetch_diaplay_text_variable(bytes_command, strlen((char *)SIM_ADD) + 1, SIM_ADD, etu_length, pin_number_phone);  
      if (WIRELESS_SIM_ON)
      {
    	  /* update phone idle text */
  		  if (IF_CONNECTED_PHONE) 
  		  {   
          FETCH_COMMAND_MENU = 150;  
    	    USAT_BYTE_LENGTH_BACK = 14 + strlen((char *)SIM_ON_CONNECTED_STRING_PHONE);  	
  		  } 
  		  else 
  		  {   
          FETCH_COMMAND_MENU = 151;  
    	    USAT_BYTE_LENGTH_BACK = 14 + strlen((char *)SIM_ON_DISCONNECTED_STRING_PHONE);  	
  		  } 
      }
      else
      {
        FETCH_COMMAND_MENU = 204;  
        USAT_BYTE_LENGTH_BACK = 14 + strlen((char *)SIM_OFF_STRING_PHONE) + 1;
      }
      break;
      
    /* set up menu */
    case 250:
#if (SET_WATCH_USAT_MENU) 
      phone_command_fetch_set_menu(bytes_command, etu_length, pin_number_phone); 
      /* set idle mode text to indicate Wireless SIM ON/OFF state */  
      if (WIRELESS_SIM_ON)
      {
    	/* update phone idle text */
  		if (IF_CONNECTED_PHONE) 
  		{   
          FETCH_COMMAND_MENU = 150;  
    	  USAT_BYTE_LENGTH_BACK = 14 + strlen((char *)SIM_ON_CONNECTED_STRING_PHONE);  	
  		} 
  		else 
  		{   
          FETCH_COMMAND_MENU = 151;  
    	  USAT_BYTE_LENGTH_BACK = 14 + strlen((char *)SIM_ON_DISCONNECTED_STRING_PHONE);  	
        } 
      }
      else
      {
        FETCH_COMMAND_MENU = 204;
        USAT_BYTE_LENGTH_BACK = 14 + strlen((char *)SIM_OFF_STRING_PHONE) + 1; 
      } 
#else
/*	  
	    phone_command_fetch_set_initial_item(bytes_command, etu_length, pin_number_phone);
      FETCH_COMMAND_MENU = 0;
      USAT_BYTE_LENGTH_BACK = 0;
*/	  
      phone_command_fetch_diaplay_text(bytes_command, strlen((char *)SETTING_STRING), SETTING_STRING, etu_length, pin_number_phone);  
      USAT_BYTE_LENGTH_BACK = 0;
#endif       
      break;

#if (SET_WATCH_USAT_MENU == 0) 
    case 251:
      phone_command_fetch_set_menu(bytes_command, etu_length, pin_number_phone); 
      FETCH_COMMAND_MENU = 0;  
      USAT_BYTE_LENGTH_BACK = 0;
      break;
#endif      
	default:
      break;  
  }
}

/********************************************************************************/
uint32_t phone_command_fetch_reset(uint8_t *bytes_command, uint32_t etu_length, uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| USAT command NAA reset, phone start to access the network again
|
--------------------------------------------------------------------------------*/
{
  uint8_t usat_reset_bytes[14] = {0x12, 0xd0, 0x9, 0x81, 0x3, 0x1, 0x1, 0x3, 0x82, 0x2, 0x81, 0x82, 0x90, 0x0};
  
  /* write SIM response to phone */   
  write_bytes(14, usat_reset_bytes, etu_length, pin_number_phone);   
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_fetch_diaplay_text(uint8_t *bytes_command, uint8_t string_length, 
     uint8_t *display_string, uint32_t etu_length, uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| USAT command text display, string display_string displayed on the phone screen 
|
--------------------------------------------------------------------------------*/
{
  uint8_t start_bytes[15] = {0x12, 0xd0, 0x15, 0x81, 0x3, 0x1, 0x21, 0x00, 0x82, 0x2, 0x81, 0x2, 0x8d, 0x4, 0x4};
  uint8_t end_bytes[2] = {0x90, 0x0};
  
  start_bytes[2] = 0xc + string_length; 
  start_bytes[13] = 0x1 + string_length; 
  
  /* write SIM response to phone */   
  write_bytes(15, start_bytes, etu_length, pin_number_phone);   
  write_bytes(string_length, display_string, etu_length, pin_number_phone);   
  write_bytes(2, end_bytes, etu_length, pin_number_phone);   
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_fetch_diaplay_sensor_data(uint8_t *bytes_command, uint8_t string_length, 
     uint8_t *display_string, uint32_t etu_length, uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| USAT command text display, string display_string displayed on the phone screen 
|
--------------------------------------------------------------------------------*/
{
  uint8_t start_bytes[15] = {0x12, 0xd0, 0x15, 0x81, 0x3, 0x1, 0x21, 0x81, 0x82, 0x2, 0x81, 0x2, 0x8d, 0x4, 0x4};
  uint8_t end_bytes[2] = {0x90, 0x0};
  uint8_t display_sensor_data[SIM_APPLICATION_DISPLAY_LENGTH];
  uint8_t i;
  uint8_t sensor_data_length;
	
	for (i=0; i<SIM_APPLICATION_DISPLAY_LENGTH; i++)
	{
		display_sensor_data[i] = ' ';
	}
	
	switch (FETCH_COMMAND_MENU)
	{
		case SENSOR_DATA_TYPE_BODY_TEMPERATURE:
		{
			sensor_data_length = SIM_APPLICATION_DISPLAY_LENGTH / 2;
						
      for (i=0; i<sensor_data_length; i++)
			{ 
				if (BODY_TEMPERATURE[i + 2] == 0) 	
				{
				  sprintf((char *)(display_sensor_data + 2 * i), "0");
				  sprintf((char *)(display_sensor_data + 2 * i + 1), "0");
				}	
				else if ((BODY_TEMPERATURE[i + 2] & 0xf0) == 0)
				{
					sprintf((char *)(display_sensor_data + 2 * i), "0");
				  sprintf((char *)(display_sensor_data + 2 * i + 1), "%x", BODY_TEMPERATURE[i + 2]);
				}
				else
				{
				  sprintf((char *)(display_sensor_data + 2 * i), "%x", BODY_TEMPERATURE[i + 2]);
				}
			}
			break;
		}

		case SENSOR_DATA_TYPE_HEART_RATE:
		{
			sensor_data_length = SIM_APPLICATION_DISPLAY_LENGTH / 2;
						
      for (i=0; i<sensor_data_length; i++)
			{ 
				if (HEART_RATE[i + 2] == 0) 	
				{
				  sprintf((char *)(display_sensor_data + 2 * i), "0");
				  sprintf((char *)(display_sensor_data + 2 * i + 1), "0");
				}	
				else if ((HEART_RATE[i + 2] & 0xf0) == 0)
				{
					sprintf((char *)(display_sensor_data + 2 * i), "0");
				  sprintf((char *)(display_sensor_data + 2 * i + 1), "%x", HEART_RATE[i + 2]);
				}
				else
				{
				  sprintf((char *)(display_sensor_data + 2 * i), "%x", HEART_RATE[i + 2]);
				}
			}
			break;
		}

		case SENSOR_DATA_TYPE_ECG_VALUE:
		{
			sensor_data_length = SIM_APPLICATION_DISPLAY_LENGTH / 2;
						
      for (i=0; i<sensor_data_length; i++)
			{ 
				if (ECG_VALUE[i + 2] == 0) 	
				{
				  sprintf((char *)(display_sensor_data + 2 * i), "0");
				  sprintf((char *)(display_sensor_data + 2 * i + 1), "0");
				}	
				else if ((ECG_VALUE[i + 2] & 0xf0) == 0)
				{
					sprintf((char *)(display_sensor_data + 2 * i), "0");
				  sprintf((char *)(display_sensor_data + 2 * i + 1), "%x", ECG_VALUE[i + 2]);
				}
				else
				{
				  sprintf((char *)(display_sensor_data + 2 * i), "%x", ECG_VALUE[i + 2]);
				}
			}
			break;
		}

		case SENSOR_DATA_TYPE_BLOOD_PRESURE:
		{
			sensor_data_length = SIM_APPLICATION_DISPLAY_LENGTH / 2;
						
      for (i=0; i<sensor_data_length; i++)
			{ 
				if (BLOOD_PRESURE[i + 2] == 0) 	
				{
				  sprintf((char *)(display_sensor_data + 2 * i), "0");
				  sprintf((char *)(display_sensor_data + 2 * i + 1), "0");
				}	
				else if ((BLOOD_PRESURE[i + 2] & 0xf0) == 0)
				{
					sprintf((char *)(display_sensor_data + 2 * i), "0");
				  sprintf((char *)(display_sensor_data + 2 * i + 1), "%x", BLOOD_PRESURE[i + 2]);
				}
				else
				{
				  sprintf((char *)(display_sensor_data + 2 * i), "%x", BLOOD_PRESURE[i + 2]);
				}
			}
			break;
		}

		case SENSOR_DATA_TYPE_BLOOD_OXYGEN:
		{
			sensor_data_length = SIM_APPLICATION_DISPLAY_LENGTH / 2;
						
      for (i=0; i<sensor_data_length; i++)
			{ 
				if (BLOOD_OXYGEN[i + 2] == 0) 	
				{
				  sprintf((char *)(display_sensor_data + 2 * i), "0");
				  sprintf((char *)(display_sensor_data + 2 * i + 1), "0");
				}	
				else if ((BLOOD_OXYGEN[i + 2] & 0xf0) == 0)
				{
					sprintf((char *)(display_sensor_data + 2 * i), "0");
				  sprintf((char *)(display_sensor_data + 2 * i + 1), "%x", BLOOD_OXYGEN[i + 2]);
				}
				else
				{
				  sprintf((char *)(display_sensor_data + 2 * i), "%x", BLOOD_OXYGEN[i + 2]);
				}
			}
			break;
		}

	}
  
  start_bytes[2] = 0xc + string_length + SIM_APPLICATION_DISPLAY_LENGTH; 
  start_bytes[13] = 0x1 + string_length + SIM_APPLICATION_DISPLAY_LENGTH; 
  
  /* write SIM response to phone */   
  write_bytes(15, start_bytes, etu_length, pin_number_phone);   
  write_bytes(string_length, display_string, etu_length, pin_number_phone);   
  write_bytes(SIM_APPLICATION_DISPLAY_LENGTH, display_sensor_data, etu_length, pin_number_phone);   
  write_bytes(2, end_bytes, etu_length, pin_number_phone);   
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_fetch_diaplay_text_variable(uint8_t *bytes_command, uint8_t string_length, 
     uint8_t *display_string, uint32_t etu_length, uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| USAT command text display, string display_string displayed on the phone screen 
\ with the number of phone SIM logical address
|
--------------------------------------------------------------------------------*/
{
  uint8_t tmp_bytes1[15] = {0x12, 0xd0, 0x15, 0x81, 0x3, 0x1, 0x21, 0x00, 0x82, 0x2, 0x81, 0x2, 0x8d, 0x4, 0x4};
  uint8_t tmp_bytes2[2] = {0x90, 0x0};
  uint8_t variable_bytes = (*((uint8_t*)PHONE_ADD_FLASH_ADDR)) | 0x30;
  
  tmp_bytes1[2] = 0xc + string_length; 
  tmp_bytes1[13] = 0x1 + string_length; 
  
  /* write SIM response to phone */   
  write_bytes(15, tmp_bytes1, etu_length, pin_number_phone);   
  write_bytes(string_length - 1, display_string, etu_length, pin_number_phone);   
  write_bytes(1, &variable_bytes, etu_length, pin_number_phone);   
  write_bytes(2, tmp_bytes2, etu_length, pin_number_phone);   
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_fetch_get_input(uint8_t *bytes_command, uint32_t etu_length, 
     uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| USAT command to set password for wireless SIM interface
|
--------------------------------------------------------------------------------*/
{
  uint8_t USAT_bytes[50] = {0x12, 0xd0, 45, 0x81, 0x3, 0x1, 0x23, 0x1, 0x82, 0x2, 0x81, 0x82, 0x8d, 30, 0x4, 
               'W', 'i', 'r', 'e', 'l', 'e', 's', 's', ' ', 'S', 'I', 'M', ' ', 
               'P', 'a', 's', 's', 'w', 'o', 'r', 'd', ':', ' ', '(', '6', '-', '1', '6', ')', 
               0x91, 0x2, 0x6, 0x10, 0x90, 0x0};
   
#if (IF_PRINT_LOG)  
  if (UART_LOG)
  {  
	  printf_log_rx(50, USAT_bytes);
	}
#endif  
#if (PRINT_DELAY_MS)
  nrf_delay_ms(PRINT_DELAY_MS);
#endif
  
  /* write SIM response to phone */   
  write_bytes(50, USAT_bytes, etu_length, pin_number_phone);   
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_fetch_get_input_key(uint8_t *bytes_command, uint32_t etu_length, 
     uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| USAT command to set system key for wireless SIM interface
|
--------------------------------------------------------------------------------*/
{
  uint8_t USAT_bytes[52] = {0x12, 0xd0, 47, 0x81, 0x3, 0x1, 0x23, 0x1, 0x82, 0x2, 0x81, 0x82, 0x8d, 32, 0x4, 
               'W', 'i', 'r', 'e', 'l', 'e', 's', 's', ' ', 'S', 'I', 'M', ' ', 
               'S', 'y', 's', 't', 'e', 'm', ' ', 'K', 'e', 'y', ':', ' ', '(', '6', '-', '1', '6', ')', 
               0x91, 0x2, 0x6, 0x10, 0x90, 0x0};
   
#if (IF_PRINT_LOG)  
  if (UART_LOG)
	{
    printf_log_rx(52, USAT_bytes);
	}
#endif  
#if (PRINT_DELAY_MS)
  nrf_delay_ms(PRINT_DELAY_MS);
#endif
  
  /* write SIM response to phone */   
  write_bytes(52, USAT_bytes, etu_length, pin_number_phone);   
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_fetch_get_input_name(uint8_t *bytes_command, uint32_t etu_length, 
     uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| USAT command to set the phone/watch name
|
--------------------------------------------------------------------------------*/
{
  uint8_t bytes_start[12] = {0x12, 0xd0, 40, 0x81, 0x3, 0x1, 0x23, 0x1, 0x82, 0x2, 0x81, 0x82};
  uint8_t bytes_text[27] = {0x8d, 25, 0x4, 'I', 'n', 'p', 'u', 't', ' ', 'P', 'h', 'o', 'n', 'e', ' ', 
              'N', 'a', 'm', 'e', ':', ' ', '(', '6', '-', '1', '5', ')'};
  uint8_t bytes_end[6] = {0x91, 0x2, 0x6, 0xf, 0x90, 0x0};
  
  /* write SIM response to phone */   
  write_bytes(12, bytes_start, etu_length, pin_number_phone);   
  write_bytes(27, bytes_text, etu_length, pin_number_phone);   
  write_bytes(6, bytes_end, etu_length, pin_number_phone);   
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_fetch_get_input_address(uint8_t *bytes_command, uint32_t etu_length, 
     uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| USAT command for input at phone side, to set the phone wireless SIM logical address
|
--------------------------------------------------------------------------------*/
{
  uint8_t bytes_start[12] = {0x12, 0xd0, 73, 0x81, 0x3, 0x1, 0x23, 0x0, 0x82, 0x2, 0x81, 0x82};
  uint8_t bytes_text[60] = {0x8d, 58, 0x4, 'S', 'e', 't', ' ', 'P', 'h', 'o', 'n', 'e', ' ', 'S', 
              'I', 'M', ' ', 'A', 'd', 'd', 'r', 'e', 's', 's', ':', ' ', 
              '1', ',', '2', ',', '3', ',', '4', ',', '5', ',', '6', ' ', 'f', 'o', 
              'r', ' ', 'i', 'P', 'h', 'o', 'n', 'e',
              '.', ' ', '7', ' ', 'f', 'o', 'r', ' ', 'i', 'P', 'a', 'd'};
  uint8_t bytes_end[6] = {0x91, 0x2, 0x1, 0x1, 0x90, 0x0};
  
  /* write SIM response to phone */   
  write_bytes(12, bytes_start, etu_length, pin_number_phone);   
  write_bytes(60, bytes_text, etu_length, pin_number_phone);   
  write_bytes(6, bytes_end, etu_length, pin_number_phone);   
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_fetch_get_input_phonebook_index(uint32_t etu_length, uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| phone book input - index
|
--------------------------------------------------------------------------------*/
{
  uint8_t bytes_start[12] = {0x12, 0xd0, 50, 0x81, 0x3, 0x1, 0x23, 0x0, 0x82, 0x2, 0x81, 0x82};
  uint8_t bytes_text[37] = {0x8d, 35, 0x4, 'P', 'h', 'o', 'n', 'e', ' ', 'b', 'o', 'o', 'k', ' ', 
              'i', 't', 'e', 'm', ' ', 'a', 'd', 'd', ' ', '-', ' ', 'i', 'n', 'd', 'e', 'x', '(',
		      '0', '-', '2', '5', '5', ')'};
  uint8_t bytes_end[6] = {0x91, 0x2, 0x1, 0x3, 0x90, 0x0};
  
  /* write SIM response to phone */   
  write_bytes(12, bytes_start, etu_length, pin_number_phone);   
  write_bytes(37, bytes_text, etu_length, pin_number_phone);   
  write_bytes(6, bytes_end, etu_length, pin_number_phone);   
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_fetch_get_input_phonebook_name(uint32_t etu_length, uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| phone book input - name
|
--------------------------------------------------------------------------------*/
{
  uint8_t bytes_start[12] = {0x12, 0xd0, 54, 0x81, 0x3, 0x1, 0x23, 0x1, 0x82, 0x2, 0x81, 0x82};
  uint8_t bytes_text[41] = {0x8d, 39, 0x4, 'P', 'h', 'o', 'n', 'e', ' ', 'b', 'o', 'o', 'k', ' ', 
              'i', 't', 'e', 'm', ' ', 'a', 'd', 'd', ' ', '-', ' ', 'n', 'a', 'm', 'e', ' ', '(',
		      '1', '-', '3', '1', ' ', 'C', 'h', 'a', 'r', ')'};
  uint8_t bytes_end[6] = {0x91, 0x2, 0x1, PHONE_BOOK_NUMBER_START_INDEX - 3, 0x90, 0x0};
  
  /* write SIM response to phone */   
  write_bytes(12, bytes_start, etu_length, pin_number_phone);   
  write_bytes(41, bytes_text, etu_length, pin_number_phone);   
  write_bytes(6, bytes_end, etu_length, pin_number_phone);   
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_fetch_get_input_phonebook_number(uint32_t etu_length, uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| phone book input - index
|
--------------------------------------------------------------------------------*/
{
  uint8_t bytes_start[12] = {0x12, 0xd0, 57, 0x81, 0x3, 0x1, 0x23, 0x0, 0x82, 0x2, 0x81, 0x82};
  uint8_t bytes_text[44] = {0x8d, 42, 0x4, 'P', 'h', 'o', 'n', 'e', ' ', 'b', 'o', 'o', 'k', ' ', 
              'i', 't', 'e', 'm', ' ', 'a', 'd', 'd', ' ', '-', ' ', 'n', 'u', 'm', 'b', 'e', 'r', ' ', '(',
		      '1', '-', '2', '0', ' ', 'd', 'i', 'g', 'i', 't', ')'};
  uint8_t bytes_end[6] = {0x91, 0x2, 0x1, PHONE_BOOK_LENGTH_CHAR - PHONE_BOOK_NUMBER_START_INDEX - 2, 0x90, 0x0};
  
  /* write SIM response to phone */   
  write_bytes(12, bytes_start, etu_length, pin_number_phone);   
  write_bytes(44, bytes_text, etu_length, pin_number_phone);   
  write_bytes(6, bytes_end, etu_length, pin_number_phone);   
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_fetch_get_input_sms_index(uint32_t etu_length, uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| phone book input - index
|
--------------------------------------------------------------------------------*/
{
  uint8_t bytes_start[12] = {0x12, 0xd0, 54, 0x81, 0x3, 0x1, 0x23, 0x0, 0x82, 0x2, 0x81, 0x82};
  uint8_t bytes_text[41] = {0x8d, 39, 0x4, 'P', 'r', 'e', 'd', 'e', 'f', 'i', 'n', 'e', 'd', ' ', 
              'S', 'M', 'S', ' ', 'i', 't', 'e', 'm', ' ', 'a', 'd', 'd', ' ', '-', ' ', 'i', 'n', 'd', 'e', 'x', '(',
		      '0', '-', '2', '5', '5', ')'};
  uint8_t bytes_end[6] = {0x91, 0x2, 0x1, 0x3, 0x90, 0x0};
  
  /* write SIM response to phone */   
  write_bytes(12, bytes_start, etu_length, pin_number_phone);   
  write_bytes(41, bytes_text, etu_length, pin_number_phone);   
  write_bytes(6, bytes_end, etu_length, pin_number_phone);   
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_fetch_get_input_sms_content(uint32_t etu_length, uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| phone book input - name
|
--------------------------------------------------------------------------------*/
{
  uint8_t bytes_start[12] = {0x12, 0xd0, 61, 0x81, 0x3, 0x1, 0x23, 0x1, 0x82, 0x2, 0x81, 0x82};
  uint8_t bytes_text[48] = {0x8d, 46, 0x4, 'P', 'r', 'e', 'd', 'e', 'f', 'i', 'n', 'e', 'd', ' ', 
			              'S', 'M', 'S', ' ', 'i', 't', 'e', 'm', ' ', 'a', 'd', 'd', ' ', '-', 
						  ' ', 'c', 'o', 'n', 't', 'e', 'n', 't', '(',
					      '1', '-', '1', '6', '0', ' ', 'C', 'h', 'a', 'r', ')'};
  uint8_t bytes_end[6] = {0x91, 0x2, 0x1, PREDEFINED_SMS_LENGTH_CHAR - 3, 0x90, 0x0};
  
  /* write SIM response to phone */   
  write_bytes(12, bytes_start, etu_length, pin_number_phone);   
  write_bytes(48, bytes_text, etu_length, pin_number_phone);   
  write_bytes(6, bytes_end, etu_length, pin_number_phone);   
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_fetch_at_cfun0(uint8_t *bytes_command, uint32_t etu_length, 
     uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| USAT command for baseband minimal function mode (AT command interface)
|
--------------------------------------------------------------------------------*/
{
  uint8_t bytes_start[12] = {0x12, 0xd0, 25, 0x81, 0x3, 0x1, 0x34, 0x1, 0x82, 0x2, 0x81, 0x82};
  uint8_t bytes_text[16] = {0x28, 14, 0x4, 
               'a', 't', '+', 'c', 'f', 'u', 'n', '=', '0', '\\', 'r', '\\', 'n'};
  uint8_t bytes_end[2] = {0x90, 0x0};
   
  /* write SIM response to phone */   
  write_bytes(12, bytes_start, etu_length, pin_number_phone);   
  write_bytes(16, bytes_text, etu_length, pin_number_phone);   
  write_bytes(2, bytes_end, etu_length, pin_number_phone);   
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_fetch_set_menu_(uint8_t *bytes_command, uint32_t etu_length, 
     uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| USAT command set menu/item, for configuration parameters setting of wireless 
| SIM interface
|
--------------------------------------------------------------------------------*/
{
  uint8_t bytes_start[32] = {0x12,  0xd0,  0x81,  111,  0x81,  0x3,  0x1,  0x25,  0x0,  0x82,  0x2,  0x81,  0x82,  
              0x85,  17, 'S',  '3', ' ',  'W',  'a',  't',  'c',  'h',  ' ',  'S',  'e', 't', 't',
              'i', 'n', 'g', 's'};               
#if (SET_WATCH_USAT_MENU == 0) 
  uint8_t bytes_menu_0[22] = {0x8f,  20,  0x0,  'W', 'i', 'r',  'e',  'l',  'e',  's',  's', 
            ' ',  'S',  'I', 'M', ' ', 'S',  't',  'a',  't',  'u',  's'}; 
#endif
  uint8_t bytes_menu_1[21] = {0x8f,  19,  0x01,  'A', 'b', 'o',  'u',  't',  ' ',  'W',  'i', 
              'r',  'e',  'l', 'e', 's', 's',  ' ',  'S',  'I',  'M'};  
                            
  uint8_t bytes_menu_2[22] = {0x8f,  20,  0x02,  'W', 'i', 'r',  'e',  'l',  'e',  's',  's', 
              ' ',  'S',  'I', 'M', ' ', 'O',  'n', '/', 'O',  'f',  'f'}; 
  uint8_t bytes_menu_4[19] = {0x8f,  17,  0x04,  'P',  'a',  's',  's',  'w',  'o', 'r',  'd', ' ', 'S', 
              'e',  't', 't', 'i', 'n',  'g'};
  uint8_t bytes_menu_key[21] = {0x8f,  19,  10,  'S',  'y',  's',  't',  'e',  'm', ' ',  'K', 'e', 'y', ' ', 'S', 
			              'e',  't', 't', 'i', 'n',  'g'};
  /* menu for phone side only */
  uint8_t bytes_menu_5[13] = {0x8f,  11,  0x05,  'P',  'h',  'o',  'n',  'e',  ' ', 'N',  'a', 'm', 'e'};
  uint8_t bytes_menu_101[20] = {0x8f,  18,  101,  'P',  'h',  'o',  'n',  'e',  ' ', 'S',  'I', 'M', ' ',  'A',  'd',  'd',  'r',  'e',  's',  's'};
  uint8_t bytes_menu_102[21] = {0x8f,  19,  102,  'W',  'i',  'r',  'e',  'l',  'e', 's',  's', ' ', 'S',  'I',  'M',  ' ',  'R',  'e',  's',  'e', 't'};
  uint8_t bytes_menu_104[19] = {0x8f,  17,  104,  'W',  'a',  't',  'c',  'h',  ' ', 'R',  'e', 'm', 'o',  't',  'e',  ' ',  'C',  'F',  'G'};
			                              
  uint8_t bytes_end[2] = {0x90,  0x0};
  
  bytes_start[3] = 28 + 21 + 22 + 19 + 21;
#if (SET_WATCH_USAT_MENU == 0) 
  bytes_start[3] += 22;
#endif
  bytes_start[3] += (13 + 20 + 21 + 19);
       
#if (SET_WATCH_USAT_MENU) 
  /* set up menu */
  bytes_start[7] = 0x25; 
#else 
  /* set up item */
  bytes_start[7] = 0x24; 
#endif 
    
  /* write SIM response to phone */  
  /* menu set beginning */   
  write_bytes(32, bytes_start, etu_length, pin_number_phone); 

#if (SET_WATCH_USAT_MENU == 0) 
  write_bytes(22, bytes_menu_0, etu_length, pin_number_phone); 
#endif  
  
  /* menu 1 */   
  write_bytes(21, bytes_menu_1, etu_length, pin_number_phone);   
  /* menu 4 */   
  write_bytes(19, bytes_menu_4, etu_length, pin_number_phone); 
  /* menu system key */   
  write_bytes(21, bytes_menu_key, etu_length, pin_number_phone);   
  /* menu 2 */   
  write_bytes(22, bytes_menu_2, etu_length, pin_number_phone);   
  /* menu 5 */   
  write_bytes(13, bytes_menu_5, etu_length, pin_number_phone); 
  write_bytes(20, bytes_menu_101, etu_length, pin_number_phone); 
  write_bytes(21, bytes_menu_102, etu_length, pin_number_phone); 
  write_bytes(19, bytes_menu_104, etu_length, pin_number_phone); 
  
  /* menu set end */   
  write_bytes(2, bytes_end, etu_length, pin_number_phone);   
  
  return(0);
}
              
/********************************************************************************/
uint32_t phone_command_fetch_set_menu(uint8_t *bytes_command, uint32_t etu_length, 
     uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| USAT command set menu/item, for configuration parameters setting of wireless 
| SIM interface
|
--------------------------------------------------------------------------------*/
{
  uint8_t bytes_start[31] = {0x12,  0xd0,  0x81,  111,  0x81,  0x3,  0x1,  0x25,  0x0,  0x82,  0x2,  0x81,  0x82,  
              0x85,  16, 'B',  'o', 'd',  'y',  ' ',  'S',  'e',  'n',  's',  'o',  'r', ' ', 'D',
              'a', 't', 'a'};               
  uint8_t bytes_menu_body_temperature[19] = {0x8f,  17,  SENSOR_DATA_TYPE_BODY_TEMPERATURE,  'b', 'o', 'd',  'y',  ' ',  't',  'e',  'm', 
              'p',  'e',  'r', 'a', 't', 'u',  'r',  'e'};  
                            
  uint8_t bytes_menu_heart_rate[13] = {0x8f,  11,  SENSOR_DATA_TYPE_HEART_RATE,  'h', 'e', 'a',  'r',  't',  ' ',  'r',  'a', 
              't',  'e'}; 
  uint8_t bytes_menu_ecg_value[12] = {0x8f,  10,  SENSOR_DATA_TYPE_ECG_VALUE,  'E',  'C',  'G',  ' ',  'v',  'a', 'l',  'u', 'e'};
  uint8_t bytes_menu_blood_presure[16] = {0x8f,  14,  SENSOR_DATA_TYPE_BLOOD_PRESURE,  'b',  'l',  'o',  'o',  'd',  ' ', 'p',  'r', 'e', 's', 'u', 'r', 'e'};
  uint8_t bytes_menu_blood_oxygen[15] = {0x8f,  13,  SENSOR_DATA_TYPE_BLOOD_OXYGEN,  'b',  'l',  'o',  'o',  'd',  ' ', 'o',  'x', 'y', 'g', 'e', 'n'};
			                              
  uint8_t bytes_end[2] = {0x90,  0x0};
  
  bytes_start[3] = 27 + 19 + 13 + 12 + 16 + 15;
       
  /* write SIM response to phone */  
  /* menu set beginning */   
  write_bytes(31, bytes_start, etu_length, pin_number_phone); 
  
  /* menu body temperature */   
  write_bytes(19, bytes_menu_body_temperature, etu_length, pin_number_phone);   
  /* menu heart rate */   
  write_bytes(13, bytes_menu_heart_rate, etu_length, pin_number_phone); 
  /* menu system ECG value */   
  write_bytes(12, bytes_menu_ecg_value, etu_length, pin_number_phone);   
  /* menu blood presure */   
  write_bytes(16, bytes_menu_blood_presure, etu_length, pin_number_phone);   
  /* menu blood oxygen */   
  write_bytes(15, bytes_menu_blood_oxygen, etu_length, pin_number_phone); 

  /* menu set end */   
  write_bytes(2, bytes_end, etu_length, pin_number_phone);   
  
  return(0);
}
              
#if (SET_WATCH_USAT_MENU == 0)               
/********************************************************************************/
uint32_t phone_command_fetch_set_initial_item(uint8_t *bytes_command, uint32_t etu_length, 
     uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| USAT command set menu/item, for configuration parameters setting of wireless 
| SIM interface
|
--------------------------------------------------------------------------------*/
{
  uint8_t set_item_start[32] = {0x12,  0xd0,  0x81,  111,  0x81,  0x3,  0x1,  0x24,  0x0,  0x82,  0x2,  0x81,  0x82,  
              0x85,  17, 'S',  '3', ' ',  'W',  'a',  't',  'c',  'h',  ' ',  'S',  'e', 't', 't',
              'i', 'n', 'g', 's'};               
  uint8_t item_menu_1[22] = {0x8f,  20,  255,  'W', 'i', 'r',  'e',  'l',  'e',  's',  's', 
              '-',  'S',  'I', 'M', ' ', 'C',  'o',  'n',  'f',  'i',  'g'};                              
  uint8_t item_end[2] = {0x90,  0x0};
  
  set_item_start[3] = 28 + 22;

  /* item start bytes */
  write_bytes(32, set_item_start, etu_length, pin_number_phone); 
  /* item No. 1 */   
  write_bytes(22, item_menu_1, etu_length, pin_number_phone);   
  /* item end bytes */   
  write_bytes(2, item_end, etu_length, pin_number_phone);   
  
  return(0);
}
#endif

/********************************************************************************/
uint32_t phone_command_fetch_set_item_phonebook(uint32_t etu_length, uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| USAT command set item, for phone book and SMS input
|
--------------------------------------------------------------------------------*/
{
  uint8_t bytes_start[40] = {0x12,  0xd0,  0x81,  111,  0x81,  0x3,  0x1,  0x24,  0x0,  0x82,  0x2,  0x81,  0x82,  
              0x85,  25, 'A', 'd', 'd', ' ', 'P',  'h',  'o',  'n',  'e',  ' ', 'B',  'o', 'o', 'k',  ' ',  '&',  ' ',  
			  'S',  'M', 'S', ' ', 'I', 't', 'e', 'm'};               
  uint8_t bytes_item_1[19] = {0x8f,  17,  121,  'P', 'h', 'o',  'n',  'e',  ' ',  'B',  'o', 
              'o', 'k', '-', 'i', 'n', 'd',  'e',  'x'};  
  uint8_t bytes_item_2[18] = {0x8f,  16,  122,  'P', 'h', 'o',  'n',  'e',  ' ',  'B',  'o', 
			  'o', 'k', '-', 'n', 'a', 'm',  'e'};  
  uint8_t bytes_item_3[20] = {0x8f,  18,  123,  'P', 'h', 'o',  'n',  'e',  ' ',  'B',  'o', 
			  'o', 'k', '-', 'n', 'u', 'm',  'b', 'e', 'r'};  
  uint8_t bytes_item_4[12] = {0x8f,  10,  124,  'S', 'M', 'S',  '-',  'i',  'n',  'd',  'e', 'x'};  
  uint8_t bytes_item_5[14] = {0x8f,  12,  125,  'S', 'M', 'S',  '-',  'c',  'o',  'n',  't', 'e', 'n', 't'};                              
  uint8_t bytes_end[2] = {0x90,  0x0};
  
  bytes_start[3] = 36 + 19 + 18 + 20 + 12 +14;
  
  
  /* item set beginning */   
  write_bytes(40, bytes_start, etu_length, pin_number_phone);   
  /* item 1 */   
  write_bytes(19, bytes_item_1, etu_length, pin_number_phone);   
  /* item 2 */   
  write_bytes(18, bytes_item_2, etu_length, pin_number_phone);   
  /* item 3 */   
  write_bytes(20, bytes_item_3, etu_length, pin_number_phone);   
  /* item 4 */   
  write_bytes(12, bytes_item_4, etu_length, pin_number_phone);   
  /* item 5 */   
  write_bytes(14, bytes_item_5, etu_length, pin_number_phone);   
  
  /* menu set end */   
  write_bytes(2, bytes_end, etu_length, pin_number_phone);   
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_fetch_set_item_CFG(uint32_t etu_length, uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| USAT command set item, for watch setting remotely by phone
|
--------------------------------------------------------------------------------*/
{
  uint8_t bytes_start[41] = {0x12,  0xd0,  0x81,  111,  0x81,  0x3,  0x1,  0x24,  0x0,  0x82,  0x2,  0x81,  0x82,  
              0x85,  26, 'W', 'a', 't', 'c', 'h',  ' ',  'R',  'e',  'm',  'o', 't',  'e', ' ', 'C',  'o',  'n',  'f',  
			  'i',  'g', 'u', 'r', 'a', 't', 'i', 'o', 'n'};               
  uint8_t bytes_item_1[22] = {0x8f,  20,  141,  'U', 'S', 'I',  'M',  ' ',  'I',  'n',  'i', 
              't', 'i', 'a', 'l', 'i', 'z',  'a',  't', 'i',  'o',  'n'};  
  uint8_t bytes_item_2[19] = {0x8f,  17,  142,  'P', 'a', 's',  's',  'w',  'o',  'r',  'd', 
			  ' ', 'S', 'e', 't', 't', 'i', 'n',  'g'};  
  uint8_t bytes_item_3[21] = {0x8f,  19,  143,  'S', 'y', 's',  't',  'e',  'm',  ' ',  'K', 
			  'e', 'y', ' ', 'S', 'e', 't',  't',  'i', 'n',  'g'};  
  uint8_t bytes_item_6[20] = {0x8f,  18,  146,  'W', 'a', 't',  'c',  'h',  ' ',  'C',  'a', 
			  'l', 'l', ' ', 'O', 'n', '/',  'O',  'f', 'f'};  
  uint8_t bytes_end[2] = {0x90,  0x0};
  
  bytes_start[3] = 37 + 22 + 19 + 21 + 20;
  
  
  /* item set beginning */   
  write_bytes(41, bytes_start, etu_length, pin_number_phone);   
  /* item 1 */   
  write_bytes(22, bytes_item_1, etu_length, pin_number_phone);   
  /* item 2 */   
  write_bytes(19, bytes_item_2, etu_length, pin_number_phone);   
  /* item 3 */   
  write_bytes(21, bytes_item_3, etu_length, pin_number_phone);   
  /* item 6 */   
  write_bytes(20, bytes_item_6, etu_length, pin_number_phone);   
  
  /* menu set end */   
  write_bytes(2, bytes_end, etu_length, pin_number_phone);   
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_fetch_idle_mode_text(uint8_t *bytes_command, uint8_t string_length, 
     uint8_t *display_string, uint32_t etu_length, uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| USAT command text display in idle mode, string display_string displayed on the 
| phone screen
|
--------------------------------------------------------------------------------*/
{
  uint8_t tmp_bytes1[15] = {0x12, 0xd0, 0x15, 0x81, 0x3, 0x1, 0x28, 0x00, 0x82, 
              0x2, 0x81, 0x2, 0x8d, 0x4, 0x4};
  uint8_t tmp_bytes2[2] = {0x90, 0x0};
  
  tmp_bytes1[2] = 0xc + string_length; 
  tmp_bytes1[13] = 0x1 + string_length; 
  
  /* write SIM response to phone */   
  write_bytes(15, tmp_bytes1, etu_length, pin_number_phone);   
  write_bytes(string_length, display_string, etu_length, pin_number_phone);   
  write_bytes(2, tmp_bytes2, etu_length, pin_number_phone);   
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_fetch_idle_mode_text_variable(uint8_t *bytes_command, uint8_t string_length, 
     uint8_t *display_string, uint32_t etu_length, uint32_t pin_number_phone)
/*--------------------------------------------------------------------------------
| USAT command text display in idle mode, string display_string displayed on the 
| phone screen with number
|
--------------------------------------------------------------------------------*/
{
  uint8_t tmp_bytes1[15] = {0x12, 0xd0, 0x15, 0x81, 0x3, 0x1, 0x28, 0x00, 0x82, 0x2, 
              0x81, 0x2, 0x8d, 0x4, 0x4};
  uint8_t tmp_bytes2[2] = {0x90, 0x0};
  uint8_t variable_bytes = ((*((uint8_t*)PHONE_ADD_FLASH_ADDR)) & 7) | 0x30;
  
  tmp_bytes1[2] = 0xc + string_length; 
  tmp_bytes1[13] = 0x1 + string_length; 
  
  /* write SIM response to phone */   
  write_bytes(15, tmp_bytes1, etu_length, pin_number_phone);   
  write_bytes(string_length - 1, display_string, etu_length, pin_number_phone);   
  write_bytes(1, &variable_bytes, etu_length, pin_number_phone);   
  write_bytes(2, tmp_bytes2, etu_length, pin_number_phone);   
  
  return(0);
}

/********************************************************************************/
uint32_t phone_command_search_record(uint8_t *phone_command, uint32_t etu_length)
/*--------------------------------------------------------------------------------
| phone command search record session, command 0xa2
|
--------------------------------------------------------------------------------*/
{
  uint8_t record_confirm[1] = {0xa2};
  uint8_t record_status[2] = {0x62, 0x82};
  
  write_bytes(1, record_confirm, etu_length, PIN_DATA_PHONE);   

  if(read_bytes_phone(*(phone_command + 5), INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
  {  
    return(1);
  }
  
#if (IF_PRINT_LOG)
	if (UART_LOG)
	{
    printf("search record, to be updated......\r\n");
	}
#endif  
#if (PRINT_DELAY_MS)
  nrf_delay_ms(PRINT_DELAY_MS);
#endif
  
  write_bytes(2, record_status, etu_length, PIN_DATA_PHONE);   

  return(0);  
}

/********************************************************************************/
uint32_t phone_command_envelope(uint8_t *phone_command, uint32_t etu_length)
/*--------------------------------------------------------------------------------
| phone command envelope session, command 0xc2
|
--------------------------------------------------------------------------------*/
{
  uint8_t envelope_confirm[1] = {0xc2};
  uint8_t status_bytes[2] = {0x90, 0x0};
  uint32_t tmp_length;
  uint8_t *tmp_save_data;
  uint32_t i;

  if ((*(phone_command + 5)) == 0x00)
  {
    tmp_length = 256;	
  }	
  else
  {
    tmp_length = (*(phone_command + 5));
  }
  tmp_save_data = (uint8_t *)malloc((tmp_length) * sizeof(uint8_t));
  if (tmp_save_data == NULL)
  {
#if (IF_PRINT_LOG)    
		if (UART_LOG)
		{
      printf("memory allocation error in phone_command_send_data...\r\n");
		}
#endif    
    return(1);
  }
  
#if (IF_PRINT_LOG)  
	if (UART_LOG)
	{
    printf_log_tx(1, phone_command + 2);
	}
#endif  
#if (PRINT_DELAY_MS)
  nrf_delay_ms(PRINT_DELAY_MS);
#endif
  
  write_bytes(1, envelope_confirm, etu_length, PIN_DATA_PHONE);   
  
  if(read_bytes_phone(tmp_length, INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
  {
    return(1);
  }
  /* save the read bytes */
  for (i=0; i<tmp_length; i++)
  {
	  *(tmp_save_data + i) = *(INFO_BYTE + i + 1);
  }
  
  if (*(INFO_BYTE + 1) == 0xd3) /* phone SAT menu selection */
  {
    switch(*(INFO_BYTE + 9))
    {
      case SENSOR_DATA_TYPE_BODY_TEMPERATURE:
			{
        FETCH_COMMAND_MENU = SENSOR_DATA_TYPE_BODY_TEMPERATURE;
    
        status_bytes[0] = 0x91;
        status_bytes[1] = 14 + strlen((char *)TEXT_SENSOR_DATA_BODY_TEMPERATURE) + SIM_APPLICATION_DISPLAY_LENGTH;
        break;
			}
    
      case SENSOR_DATA_TYPE_HEART_RATE:
			{
        FETCH_COMMAND_MENU = SENSOR_DATA_TYPE_HEART_RATE;
    
        status_bytes[0] = 0x91;
        status_bytes[1] = 14 + strlen((char *)TEXT_SENSOR_DATA_HEART_RATE) + SIM_APPLICATION_DISPLAY_LENGTH;
        break;
			}
    
      case SENSOR_DATA_TYPE_ECG_VALUE:
			{
        FETCH_COMMAND_MENU = SENSOR_DATA_TYPE_ECG_VALUE;
    
        status_bytes[0] = 0x91;
        status_bytes[1] = 14 + strlen((char *)TEXT_SENSOR_DATA_ECG_VALUE) + SIM_APPLICATION_DISPLAY_LENGTH;
        break;
			}
    
      case SENSOR_DATA_TYPE_BLOOD_PRESURE:
			{
        FETCH_COMMAND_MENU = SENSOR_DATA_TYPE_BLOOD_PRESURE;
    
        status_bytes[0] = 0x91;
        status_bytes[1] = 14 + strlen((char *)TEXT_SENSOR_DATA_BLOOD_PRESURE) + SIM_APPLICATION_DISPLAY_LENGTH;
        break;
			}
    
      case SENSOR_DATA_TYPE_BLOOD_OXYGEN:
			{
        FETCH_COMMAND_MENU = SENSOR_DATA_TYPE_BLOOD_OXYGEN;
    
        status_bytes[0] = 0x91;
        status_bytes[1] = 14 + strlen((char *)TEXT_SENSOR_DATA_BLOOD_OXYGEN) + SIM_APPLICATION_DISPLAY_LENGTH;
        break;
			}
    
        
      default:
          break;  
    }  

    write_bytes(2, status_bytes, etu_length, PIN_DATA_PHONE); 
  }
    
  return(0);  
}

/********************************************************************************/
uint32_t phone_command_terminal_response(uint8_t *phone_command, uint32_t etu_length)
/*--------------------------------------------------------------------------------
| phone command terminal response session, command 0x14
|
--------------------------------------------------------------------------------*/
{
  uint8_t i;
  uint8_t status_bytes[2] = {0x90, 0x0};  
  uint8_t input_bytes[4] = {0x0, 0x0, 0x0, 0x0};   
  uint8_t flag_bytes[4] = {0, 0, 0, 0}; 
  uint8_t response_confirm[1] = {0x14};
    
  write_bytes(1, response_confirm, etu_length, PIN_DATA_PHONE);   
  
  if(read_bytes_phone(*(phone_command + 5), INFO_BYTE, CHECK_BIT, PIN_DATA_PHONE, etu_length))
  {  
    return(1);
  }
  
  /* phone feedback for get input */
  if (((*(INFO_BYTE + 1) == 0x81) || (*(INFO_BYTE + 1) == 0x1)) && (*(INFO_BYTE + 4) == 0x23) && 
    ((*(INFO_BYTE + 10) == 0x83) || (*(INFO_BYTE + 10) == 0x3))
    && (*(INFO_BYTE + 12) == 0x0))
  {
    switch (FETCH_COMMAND_MENU)
    {
      case 4:
        /* get the user input password, pad 0 at the end */
        for (i=0; i<(*(INFO_BYTE + 14) - 1); i++)
        {
          USER_PASSWORD[i] = *(INFO_BYTE + 16 + i);
        } 
        for (i=(*(INFO_BYTE + 14) - 1); i<KEY_LENGTH; i++)  
        {
          USER_PASSWORD[i] = 0;
        }  
    
        /* save the input password to flash */    
        flash_byte_string_update((uint32_t *)SWAP_FLASH_ADDR, 
          (uint32_t *)USER_CONFIG_FLASH_ADDR, 0, KEY_LENGTH, USER_PASSWORD);       
        
        FETCH_COMMAND_MENU = 205;  
        status_bytes[0] = 0x91;
        status_bytes[1] = 14 + strlen((char *)PASSWD_CHG);  
        break;
     
      case 5:    
        /* get the user input phone/watch name, pad 0 at the end */
        WATCH_PHONE_NAME[0] = *(INFO_BYTE + 14) - 1;
        for (i=0; i<WATCH_PHONE_NAME[0]; i++)
        {
          WATCH_PHONE_NAME[i + 1] = *(INFO_BYTE + 16 + i);
        } 
        for (i=WATCH_PHONE_NAME[0]; i<15; i++)  
        {
          WATCH_PHONE_NAME[i + 1] = 0;
        }  
    
        /* save the input phone/watch name to flash */    
        flash_byte_string_update((uint32_t *)SWAP_FLASH_ADDR, 
          (uint32_t *)USER_CONFIG_FLASH_ADDR, 		
	      (NAME_FLASH_ADDR - USER_CONFIG_FLASH_ADDR) / 4, 16, WATCH_PHONE_NAME);
               
        FETCH_COMMAND_MENU = 206;  
        status_bytes[0] = 0x91;
        status_bytes[1] = 14 + strlen((char *)NAME_CHG);      
        break;
    
      case 10:
        /* get the user input system key, pad 0 at the end */
        for (i=0; i<(*(INFO_BYTE + 14) - 1); i++)
        {
        SYSTEM_KEY[i] = *(INFO_BYTE + 16 + i);
        } 
        for (i=(*(INFO_BYTE + 14) - 1); i<KEY_LENGTH; i++)  
        {
          SYSTEM_KEY[i] = 0;
        }  
    
        /* save the input system key to flash */    
        flash_byte_string_update((uint32_t *)SWAP_FLASH_ADDR, 
          (uint32_t *)USER_CONFIG_FLASH_ADDR, 
		      (SYSTEM_KEY_FLASH_ADDR - USER_CONFIG_FLASH_ADDR) / 4, KEY_LENGTH, SYSTEM_KEY);       
        
        FETCH_COMMAND_MENU = 20;  
        status_bytes[0] = 0x91;
        status_bytes[1] = 14 + strlen((char *)SYATEM_KEY_UPDATE_STRING);  
        break;
		
      case 101:    
        /* get the user input phone SIM logical address for wireless phone-SIM connection */
        PHONE_ADD_LOGICAL = (*(INFO_BYTE + 16) & 7);
        input_bytes[0] = (*(INFO_BYTE + 16) & 7);

        /* save the phone logical address to flash */    
        flash_byte_string_update((uint32_t *)SWAP_FLASH_ADDR, 
          (uint32_t *)USER_CONFIG_FLASH_ADDR, 9, 4, input_bytes);
               
        FETCH_COMMAND_MENU = 207;  
        status_bytes[0] = 0x91;
        status_bytes[1] = 14 + strlen((char *)SIM_ADD) + 1;      
        break; 
         
	  /* phone book get input - index */
	  case 121:  
      if (*(INFO_BYTE + 14) == 2)
		  {
		    PHONE_BOOK[0] = (*(INFO_BYTE + 16) & 0x0F);	
      }
		  else if (*(INFO_BYTE + 14) == 3)
		  {
  		  PHONE_BOOK[0] = (*(INFO_BYTE + 16) & 0x0F) * 10 + (*(INFO_BYTE + 17) & 0x0F);				
		  }
	    else if (*(INFO_BYTE + 14) == 4)
		  {
  		  PHONE_BOOK[0] = (*(INFO_BYTE + 16) & 0x0F) * 100 + (*(INFO_BYTE + 17) & 0x0F) * 10 + (*(INFO_BYTE + 18) & 0x0F);			
		  }
		
      FETCH_COMMAND_MENU = 131;  		
      status_bytes[0] = 0x91;
      status_bytes[1] = 14 + strlen((char *)PHONE_BOOK_INDEX_STRING);      
	    break; 
		
	    /* phone book get input - name */
  	  case 122:
	      /* name length in char nunmber */
	      PHONE_BOOK[1] = *(INFO_BYTE + 14) - 1;
		    for (i=0; i<PHONE_BOOK[1]; i++)
		    {
		      PHONE_BOOK[2 + i] = *(INFO_BYTE + 16 + i);
		    }	
			
        FETCH_COMMAND_MENU = 132;  		
        status_bytes[0] = 0x91;
        status_bytes[1] = 14 + strlen((char *)PHONE_BOOK_NAME_STRING);      
	    break;  
		
  	  /* phone book get input - phone number */
      case 123:
  	    /* phone number length in char nunmber */
  	    PHONE_BOOK[PHONE_BOOK_NUMBER_START_INDEX] = *(INFO_BYTE + 14) - 1;
		    for (i=0; i<PHONE_BOOK[PHONE_BOOK_NUMBER_START_INDEX]; i++)
		    {
		      PHONE_BOOK[PHONE_BOOK_NUMBER_START_INDEX + 1 + i] = *(INFO_BYTE + 16 + i);
		    }		

        FETCH_COMMAND_MENU = 133;  		
        status_bytes[0] = 0x91;
        status_bytes[1] = 14 + strlen((char *)PHONE_BOOK_NUMBER_STRING);      
		    break;
		
  	  /* phone book get input - index */
  	  case 124:  
        if (*(INFO_BYTE + 14) == 2)
  		  {
  		    PREDEFINED_SMS[0] = (*(INFO_BYTE + 16) & 0x0F);	
  		  }
  		  else if (*(INFO_BYTE + 14) == 3)
  		  {
          PREDEFINED_SMS[0] = (*(INFO_BYTE + 16) & 0x0F) * 10 + (*(INFO_BYTE + 17) & 0x0F);				
  		  }
  	    else if (*(INFO_BYTE + 14) == 4)
  		  {
    	    PREDEFINED_SMS[0] = (*(INFO_BYTE + 16) & 0x0F) * 100 + (*(INFO_BYTE + 17) & 0x0F) * 10 + (*(INFO_BYTE + 18) & 0x0F);			
  		  }

        FETCH_COMMAND_MENU = 134;  		
        status_bytes[0] = 0x91;
        status_bytes[1] = 14 + strlen((char *)SMS_INDEX_STRING);      
  	    break; 
		
  	  /* phone book get input - name */
      case 125:
  	    /* name length in char nunmber */
  	    PREDEFINED_SMS[1] = *(INFO_BYTE + 14) - 1;
  		  for (i=0; i<PREDEFINED_SMS[1]; i++)
  		  {
  		    PREDEFINED_SMS[2 + i] = *(INFO_BYTE + 16 + i);
  		  }		

        FETCH_COMMAND_MENU = 135;  		
        status_bytes[0] = 0x91;
        status_bytes[1] = 14 + strlen((char *)SMS_CONTENT_STRING);      
  	    break;  
		
      case 142:
        FLAG_PASSWORD = 1; 
		
        /* get the user input system key, pad 0 at the end */
        for (i=0; i<(*(INFO_BYTE + 14) - 1); i++)
        {
          INPUT_PASSWORD[i] = *(INFO_BYTE + 16 + i);
        } 
        for (i=(*(INFO_BYTE + 14) - 1); i<KEY_LENGTH; i++)  
        {
          INPUT_PASSWORD[i] = 0;
        }   
		 
        FETCH_COMMAND_MENU = 147;  
        status_bytes[0] = 0x91;
        status_bytes[1] = 14 + strlen((char *)PASSWORD_REQ);  
		
		    FLAG_PASSWORD = 1;  
        break;
     
      case 143:
        FLAG_SYSTEM_KEY = 1; 
	
        /* get the user input system key, pad 0 at the end */
        for (i=0; i<(*(INFO_BYTE + 14) - 1); i++)
        {
          INPUT_SYSTEM_KEY[i] = *(INFO_BYTE + 16 + i);
        } 
        for (i=(*(INFO_BYTE + 14) - 1); i<KEY_LENGTH; i++)  
        {
          INPUT_SYSTEM_KEY[i] = 0;
        }     
		 
        FETCH_COMMAND_MENU = 148;  
        status_bytes[0] = 0x91;
        status_bytes[1] = 14 + strlen((char *)SYSTEM_KEY_REQ);  
		
		    FLAG_SYSTEM_KEY = 1; 
        break;    
         
      default:
        break;  
    }
  }
  /* phone feedback for item selection */
  else if (((*(INFO_BYTE + 1) == 0x81) || (*(INFO_BYTE + 1) == 0x1)) && (*(INFO_BYTE + 4) == 0x24) && 
    ((*(INFO_BYTE + 10) == 0x83) || (*(INFO_BYTE + 10) == 0x3))
    && (*(INFO_BYTE + 12) == 0x0))
  {
    switch (*(INFO_BYTE + 15))
    {
#if (SET_WATCH_USAT_MENU == 0)   
      case 0:
        /* set idle mode text to indicate Wireless SIM ON/OFF state */  
        if (WIRELESS_SIM_ON)
        {
    	  /* update phone idle text */
  		  if (IF_CONNECTED_PHONE) 
  		  {   
            FETCH_COMMAND_MENU = 150;  
    	    USAT_BYTE_LENGTH_BACK = 14 + strlen((char *)SIM_ON_CONNECTED_STRING_PHONE);  	
  		  } 
  		  else 
  		  {   
            FETCH_COMMAND_MENU = 151;  
    	    USAT_BYTE_LENGTH_BACK = 14 + strlen((char *)SIM_ON_DISCONNECTED_STRING_PHONE);  	
  		  } 
        }
        else
        {
          FETCH_COMMAND_MENU = 204;
          USAT_BYTE_LENGTH_BACK = 14 + strlen((char *)SIM_OFF_STRING_PHONE) + 1; 
        } 
		
        break;    
#endif  
    
      case 1:
        FETCH_COMMAND_MENU = 1;
    
         status_bytes[0] = 0x91;
         status_bytes[1] = 14 + strlen((char *)ABOUT_STRING);
        break;    
    
      case 2:
        FETCH_COMMAND_MENU = 2;
        WIRELESS_SIM_ON = 1 - WIRELESS_SIM_ON; 
		
		if (WIRELESS_SIM_ON)
		{
          flag_bytes[0] = 1;
          /* save the wireless SIM flag byte to flash */    
          flash_byte_string_update((uint32_t *)SWAP_FLASH_ADDR, 
            (uint32_t *)USER_CONFIG_FLASH_ADDR, 11, 4, flag_bytes);  
      
          status_bytes[0] = 0x91;
          status_bytes[1] = 14 + strlen((char *)SIM_ON_STRING);  
		}
		else
		{
	      flag_bytes[0] = 0;
	      /* save the wireless SIM flag byte to flash */    
	      flash_byte_string_update((uint32_t *)SWAP_FLASH_ADDR, 
	        (uint32_t *)USER_CONFIG_FLASH_ADDR, 11, 4, flag_bytes);  
      
	      status_bytes[0] = 0x91;
	      status_bytes[1] = 14 + strlen((char *)SIM_OFF_STRING);     				
		}   
        break;
		   
      case 4:
        FETCH_COMMAND_MENU = 4;
        status_bytes[0] = 0x91;
        status_bytes[1] = 47;     
        break;
    
      case 5:
        FETCH_COMMAND_MENU = 5;
    
        status_bytes[0] = 0x91;
        status_bytes[1] = 42;     
        break;
    
      case 6:
        FETCH_COMMAND_MENU = 6;
        
        connection_request_phone(0x22);
        stop_oscillator_16m( );  


         /* mark the default call receiving phone */
        MASK_RECEIVE_CALL = (0x1 << PHONE_ADD_LOGICAL);
         
        status_bytes[0] = 0x91;
        status_bytes[1] =  14 + strlen((char *)RX_CALL);     
        break;
    
      case 7:
        FETCH_COMMAND_MENU = 7;
        WIRELESS_SIM_ON = 0;
    
        status_bytes[0] = 0x91;
        status_bytes[1] = 27;     
        break;
		
	  case 10:
        FETCH_COMMAND_MENU = 10;
  
        status_bytes[0] = 0x91;
        status_bytes[1] = 49;     
	    break;
         
      case 101:
        FETCH_COMMAND_MENU = 101;
    
        status_bytes[0] = 0x91;
        status_bytes[1] = 75;     
        break;
    
      case 102:
        FETCH_COMMAND_MENU = 102;

        status_bytes[0] = 0x91;
        status_bytes[1] = 14 + strlen((char *)SIM_RESET);     
      break;
	  
      case 103:
        FETCH_COMMAND_MENU = 103;

        status_bytes[0] = 0x91;
        status_bytes[1] = 122;     
        break;	
	  
	  /* phone book get input - index */
      case 121:
        FETCH_COMMAND_MENU = 121;
        status_bytes[0] = 0x91;		
        status_bytes[1] = 52;				   
      break;
	  
	  /* phone book get input - name */
      case 122:
        FETCH_COMMAND_MENU = 122;
        status_bytes[0] = 0x91;		
        status_bytes[1] = 56;				   
      break;
	  
	  /* phone book get input - name */
      case 123:
        FETCH_COMMAND_MENU = 123;
        status_bytes[0] = 0x91;		
        status_bytes[1] = 59;				   
      break;
	  
	  /* phone book get input - name */
      case 124:
        FETCH_COMMAND_MENU = 124;
        status_bytes[0] = 0x91;		
        status_bytes[1] = 56;				   
      break;
	  
	  /* phone book get input - name */
      case 125:
        FETCH_COMMAND_MENU = 125;
        status_bytes[0] = 0x91;		
        status_bytes[1] = 63;				   
      break;
	  
	  /* watch remote configuration - USIM initialization */
      case 141:
	    FLAG_USIM_INITIALIZATION = 1; 
		
        FETCH_COMMAND_MENU = 141;		
		
        status_bytes[0] = 0x91;
        status_bytes[1] = 14 + strlen((char *)USIM_INITIAL_REQ);      
      break;
	  
	  /* watch remote configuration - password change */
      case 142:		
        FETCH_COMMAND_MENU = 142;		
		
        status_bytes[0] = 0x91;
        status_bytes[1] = 47;     
      break;
	  
	  /* watch remote configuration - system key change */
      case 143:
        FETCH_COMMAND_MENU = 143;		
		
        status_bytes[0] = 0x91;
        status_bytes[1] = 49;     
      break;
	  
	  /* watch remote configuration - watch call on/off switch */
      case 146:
        FLAG_WATCH_CALL_SWITCH = 1;
	    WATCH_CALL_ON = 1 - WATCH_CALL_ON;

        FETCH_COMMAND_MENU = 146;	
        status_bytes[0] = 0x91;
	    /* WATCH_CALL_ON: bit 0: authenticate on/off */
		if (WATCH_CALL_ON)
		{    
          status_bytes[1] = 14 + strlen((char *)WATCH_CALL_ON_STRING);     
		}
		else
		{
          status_bytes[1] = 14 + strlen((char *)WATCH_CALL_OFF_STRING);     
		}
        break;
		 
#if (SET_WATCH_USAT_MENU == 0)
      case 255:
        FETCH_COMMAND_MENU = 251;     
                  
        status_bytes[0] = 0x91;
        status_bytes[1] = USAT_BYTE_LENGTH_ITEM;      
			  
	    break;
#endif		 		 

      default:
        break;  
    }
  
  }
  
#if (SET_WATCH_USAT_MENU == 0)   
  else if (((*(INFO_BYTE + 1) == 0x81) || (*(INFO_BYTE + 1) == 0x1)) && (*(INFO_BYTE + 4) == 0x21) && 
    ((*(INFO_BYTE + 10) == 0x83) || (*(INFO_BYTE + 10) == 0x3)))
  {
    /* display text proactive UICC session performed successfully, for initial item setting */
    if (*(INFO_BYTE + 12) == 0x0)
    {
      if (FETCH_COMMAND_MENU == 250)
      {
        FETCH_COMMAND_MENU = 251;  
        USAT_BYTE_LENGTH_BACK = USAT_BYTE_LENGTH_ITEM;
     
        status_bytes[0] = 0x91;
        status_bytes[1] = USAT_BYTE_LENGTH_ITEM;      
      }
    }  	
  }		
#endif  

  write_bytes(2, status_bytes, etu_length, PIN_DATA_PHONE); 

  return(0);  
}

/********************************************************************************/
uint32_t authenticate_SIM_response(uint32_t etu_length)
/*--------------------------------------------------------------------------------
| phone command authenticate session if no SIM, command 0x88
|
--------------------------------------------------------------------------------*/
{
  uint8_t authenticate_confirm = 0x88;
  uint8_t authenticate_get_length[0x2] = {0x61, 0x35};
  uint8_t authenticate_data[0x38] = {0xc0, 0xdb,  0x8,  0xca,  0xcc,  0xc9,  0x75,  0x2b,  
	  0x57,  0xf6,  0x6a,  0x10,  0x7f,  0xda,  0x44,  0xa,  0xec,  0xfa,  0xfb,  0xbe,  
	  0x1c,  0x4c,  0x52,  0x16,  0x69,  0x42,  0x5c,  0x6f,  0x10,  0x66,  0xef,  0x73,  
	  0xb4,  0x24,  0xbf,  0x3e,  0x41,  0x9a,  0x6c,  0x7d,  0x1b,  0xdf,  0x9e,  0xf1,  
	  0xdf,  0x8,  0x9f,  0x15,  0x18,  0xb3,  0x7e,  0x99,  0x68,  0x4f,  0x90,  0x0};
		
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
