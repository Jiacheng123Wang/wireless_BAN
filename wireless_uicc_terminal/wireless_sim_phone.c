#include "wireless_sim_phone.h"
#include "define_config.h"
#include "comm_phone_command.h"
#include "comm_UICC_terminal_interface.h"
#include "comm_misc.h"
#include "phone_command_sim.h"
#include "phone_wireless_interface.h"
#include "watch_wireless_interface.h"
extern uint8_t PHONE_BOOK[PHONE_BOOK_LENGTH_CHAR]; 
extern uint8_t PREDEFINED_SMS[PREDEFINED_SMS_LENGTH_CHAR]; 
extern volatile uint8_t FLAG_USIM_INITIALIZATION;   
extern volatile uint8_t FLAG_PASSWORD;   
extern volatile uint8_t FLAG_SYSTEM_KEY;   
extern volatile uint8_t FLAG_WATCH_CALL_SWITCH;   
extern uint8_t INPUT_PASSWORD[KEY_LENGTH];
extern uint8_t INPUT_SYSTEM_KEY[KEY_LENGTH];
extern uint8_t USER_PASSWORD[KEY_LENGTH];
extern uint8_t SYSTEM_KEY[KEY_LENGTH];
  
extern uint8_t PHONE_COMMAND[6];
extern uint8_t INFO_BYTE[PACKET_MAX_SIZE + 20];
extern uint8_t CHECK_BIT[PACKET_MAX_SIZE + 20];
extern volatile uint32_t CONNECTION_STATE;
extern volatile uint8_t FETCH_COMMAND_MENU;
extern volatile uint32_t USAT_BYTE_LENGTH_BACK;
extern volatile uint8_t WATCH_CALL_ON;
extern uint8_t PHONE_CHIP_TYPE;
/* phone SIM interface ETU define, for ATR initial stage */
extern volatile uint32_t ETU_TICKS_PHONE_INITIAL;
/* phone SIM interface ETU define */
extern volatile uint32_t ETU_TICKS_PHONE;
extern uint8_t UART_LOG;

/* sensor data to be sent
|  0:  data frame package index number, 0 ~ 255
|  1: content length in char number, 0 ~ 231
|  2 ~ (SENSOR_DATA_LENGTH_CHAR - 1): sensor data */
uint8_t SENSOR_DATA[SENSOR_DATA_LENGTH_CHAR] = 
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 	
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 	
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 	
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 	
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 	
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 	
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 	
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 	
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 

/********************************************************************************/
void device_peripheral_initialization(void)
/*--------------------------------------------------------------------------------
| device peripheral driver intialization 
|
--------------------------------------------------------------------------------*/
{
	if (PHONE_CHIP_TYPE == 1)
	{
	  /* phone SIM interface ETU define, in 16M timer ticks
	  ** clock signal is 3.84M for SIM interface */
	  /* 1 / 3.84 * 372 * 16 = 1550, initial ETU is 372 clock cycle */
		ETU_TICKS_PHONE_INITIAL = 1550;
    /* 1 / 3.84 * 64 * 16 = 266, PPS byes is fixed as 0x94, ETU is 64 clock cycle */  
		ETU_TICKS_PHONE = 266;
	}
	else
	{
	  /* phone SIM interface ETU define, in 16M timer ticks
	  ** clock signal is 3.25M for SIM interface */
	  /* 1 / 3.25 * 372 * 16 = 1831, initial ETU is 372 clock cycle */
		ETU_TICKS_PHONE_INITIAL = 1831;
    /* 1 / 3.25 * 64 * 16 = 315, PPS byes is fixed as 0x94, ETU is 64 clock cycle */  
		ETU_TICKS_PHONE = 315;
	}		
	
  /* start watch dog timer */   
  start_WDT(WATCH_DOG_TIME_OUT_S); 
  
  /* RTC0 initialization */
  rtc0_time_out_config( );
  
#if (SENSOR == 0)
  /* timer0 initialization */ 
  timer0_initialization( );
      
  /* GPIO pin configure for SIM-phone contactor interface */
  phone_sim_pin_setup( );

#if (ARGET_CHIP == NRF51)
  adc_initialization( );
#endif

  /* GPIOTE event for GPIO in event initialization */
  gpio_event_in_config(PIN_CLOCK_PHONE);    
#endif
}

/********************************************************************************/
void  rtc0_time_out_config(void)
/*--------------------------------------------------------------------------------
| RTC0 initialization, to control the running time of each function
|
--------------------------------------------------------------------------------*/
{
  /* Start 32 KHz clock */
  NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_RC << CLOCK_LFCLKSRC_SRC_Pos);
  NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;

  NRF_CLOCK->TASKS_LFCLKSTART = 1;
  while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0)
  {
  }
  NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;

  /* RTC0 configure, 1 ms clock tick */   
  NRF_RTC0->TASKS_STOP = 1;
  NRF_RTC0->TASKS_CLEAR = 1;
  NRF_RTC0->PRESCALER  = 32; 
  NRF_RTC0->TASKS_START = 1;
}

/********************************************************************************/
void RTC1_IRQHandler(void)
/*--------------------------------------------------------------------------------
| RTC1 tick interrupt handler, for radio link connection
|
--------------------------------------------------------------------------------*/
{
  static volatile uint8_t CONNECTION_STATE_CHECK_COUNTER = 0;
#if (SENSOR)
  uint8_t i;
#endif
  
  if ((NRF_RTC1->EVENTS_COMPARE[0] != 0) && ((NRF_RTC1->INTENSET & RTC_INTENSET_COMPARE0_Msk) != 0))
  {  
    NRF_RTC1->EVENTS_COMPARE[0] = 0;
    /* re-load watch dog request register */
    NRF_WDT->RR[0] = 0x6E524635;
    
    /* increase the radio link check counter */
    CONNECTION_STATE_CHECK_COUNTER++;
		
#if (SENSOR)
	  if (SENSOR_DATA[1])
	  {
      if (!phone_command_send_data(SENSOR_DATA, SENSOR_DATA[1] + 2, WIRELESS_DATA_TYPE_SENSOR_DATA))
	    {
#if (IF_PRINT_LOG)
				if (UART_LOG)
				{
          printf("-------------------- sensor data send successfully ----------------------\r\n");
        }
#endif   
		    for (i=0; i<SENSOR_DATA_LENGTH_CHAR; i++)
		    {
	        SENSOR_DATA[i] = 0;
	      }		  			
      }		  	
    }
#if (IF_PRINT_LOG)
		if (UART_LOG)
		{
      printf("Hi, check sensor data to send, NRF_RTC0->COUNTER = %ld -------- \r\n",  NRF_RTC0->COUNTER);
		}
#endif	
			
	  if (CONNECTION_STATE_CHECK_COUNTER >= CONNECT_STATE_TIME_PHONE) 
    {
	    CONNECTION_STATE_CHECK_COUNTER = 0;
	    phone_connection_state_check( );    	
    }  
#else	
    sim_connection_state_check( );
#if (IF_PRINT_LOG)
		if (UART_LOG)
		{
      printf ("Hi, No sensor data received, NRF_RTC0->COUNTER = %ld -------- \r\n",  NRF_RTC0->COUNTER);
    } 
#endif	
#endif	
	
	/* clear RTC1 */
	  NRF_RTC1->TASKS_CLEAR = 1;
    while (NRF_RTC1->COUNTER)
    {
      NRF_RTC1->TASKS_CLEAR = 1;
    }	
  }
}

/********************************************************************************/
void rtc1_radio_link_config(void)
/*--------------------------------------------------------------------------------
| RTC1 initial configuration, for radio link connection
|
--------------------------------------------------------------------------------*/
{
  /* set the interrupt priority of RTC1 as 2, lower than read phone command 
     interrupt GPIOTE_IRQn (1) */                                         
  NVIC_SetPriority(RTC1_IRQn, RADIO_LINK_IRQ_PRIORITY);
   
  NVIC_ClearPendingIRQ(RTC1_IRQn);
  /* Enable Interrupt for the RTC1 in the core. */
  NVIC_EnableIRQ(RTC1_IRQn);   
     
  /* set the real time couter resolution is about 125ms */   
  NRF_RTC1->TASKS_STOP = 1;
  NRF_RTC1->PRESCALER  = 4095; 

  /* Compare0 after approx WATCH_DOG_RESET_TIME * 125ms. */
  NRF_RTC1->CC[0] = WATCH_DOG_RESET_TIME;
  /* Enable COMPARE0 event and COMPARE0 interrupt */
  NRF_RTC1->EVTENSET = RTC_EVTENSET_COMPARE0_Msk;
  NRF_RTC1->INTENSET = RTC_INTENSET_COMPARE0_Msk; 
   
  NRF_RTC1->TASKS_START = 1;
}

