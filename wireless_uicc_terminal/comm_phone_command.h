#ifndef COMM_PHONE_COMMAND_H__
#define COMM_PHONE_COMMAND_H__

#include "define_config.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nrf_gpiote.h"
#if (ARGET_CHIP == NRF52)
  #include "nrf_drv_saadc.h" 
#endif  
#include "define_config.h"
#include "nrf_delay.h"
#include "app_error.h"

void phone_sim_pin_setup(void);
#if (ARGET_CHIP == NRF52)
  void saadc_config(void);
  void get_saadc_sample_value(uint32_t *sample_value);
  void saadc_callback(nrf_drv_saadc_evt_t const *p_event);
  void saadc_init(uint8_t channel_number);
#else
  void adc_initialization(void);
#endif  
int32_t saadc_sample_value_get(void);
void gpio_event_in_config(uint32_t clock_pin);
uint32_t read_phone_command(uint8_t *bytes_command, uint8_t *parity_bit, uint32_t pin_number, 
         uint32_t etu_length);
uint32_t read_soft_warm_reset_phone(uint32_t etu_ticks_initial, uint32_t pin_number);
uint32_t soft_warm_reset_phone(uint32_t etu_length, uint32_t pin_number_io);
uint32_t write_phone_response(uint32_t bytes_length, uint8_t *bytes_info, uint32_t etu_length);
uint32_t phone_command_PIN(uint8_t *bytes_command, uint32_t etu_length_phone);
uint32_t phone_command_terminal_profile(uint8_t *phone_command, uint32_t etu_length, 
         uint32_t pin_number_phone);
void phone_command_fetch_all(uint8_t *bytes_command, uint32_t etu_length, uint32_t pin_number_phone);
uint32_t phone_command_fetch_reset(uint8_t *bytes_command, uint32_t etu_length, uint32_t pin_number_phone);
uint32_t phone_command_fetch_diaplay_text(uint8_t *bytes_command, uint8_t string_length, 
         uint8_t *display_string, uint32_t etu_length, uint32_t pin_number_phone);
uint32_t phone_command_fetch_diaplay_text_variable(uint8_t *bytes_command, uint8_t string_length, 
         uint8_t *display_string, uint32_t etu_length, uint32_t pin_number_phone);
uint32_t phone_command_fetch_get_input(uint8_t *bytes_command, uint32_t etu_length, 
         uint32_t pin_number_phone);
uint32_t phone_command_fetch_get_input_key(uint8_t *bytes_command, uint32_t etu_length, 
		 uint32_t pin_number_phone);
uint32_t phone_command_fetch_get_input_name(uint8_t *bytes_command, uint32_t etu_length, 
         uint32_t pin_number_phone);
uint32_t phone_command_fetch_get_input_address(uint8_t *bytes_command, uint32_t etu_length, 
         uint32_t pin_number_phone);
uint32_t phone_command_fetch_at_cfun0(uint8_t *bytes_command, uint32_t etu_length, 
         uint32_t pin_number_phone);
uint32_t phone_command_fetch_set_menu(uint8_t *bytes_command, uint32_t etu_length, 
         uint32_t pin_number_phone);
uint32_t phone_command_fetch_idle_mode_text(uint8_t *bytes_command, uint8_t string_length, 
         uint8_t *display_string, uint32_t etu_length, uint32_t pin_number_phone);
uint32_t phone_command_fetch_idle_mode_text_variable(uint8_t *bytes_command, uint8_t string_length, 
         uint8_t *display_string, uint32_t etu_length, uint32_t pin_number_phone);
uint32_t phone_command_search_record(uint8_t *phone_command, uint32_t etu_length);
uint32_t phone_command_envelope(uint8_t *phone_command, uint32_t etu_length);
uint32_t phone_command_terminal_response(uint8_t *phone_command, uint32_t etu_length);
uint32_t phone_command_fetch_set_initial_item(uint8_t *bytes_command, uint32_t etu_length, 
     uint32_t pin_number_phone);
uint32_t phone_command_manage_channel(uint8_t *bytes_command, uint32_t etu_length_phone);
uint32_t phone_command_fetch_diaplay_sensor_data(uint8_t *bytes_command, uint8_t string_length, 
     uint8_t *display_string, uint32_t etu_length, uint32_t pin_number_phone);
         
uint32_t phone_command_fetch_set_item_phonebook(uint32_t etu_length, uint32_t pin_number_phone);
uint32_t phone_command_fetch_get_input_phonebook_index(uint32_t etu_length, uint32_t pin_number_phone);
uint32_t phone_command_fetch_get_input_phonebook_name(uint32_t etu_length, uint32_t pin_number_phone);
uint32_t phone_command_fetch_get_input_phonebook_number(uint32_t etu_length, uint32_t pin_number_phone);
uint32_t phone_command_fetch_get_input_sms_index(uint32_t etu_length, uint32_t pin_number_phone);
uint32_t phone_command_fetch_get_input_sms_content(uint32_t etu_length, uint32_t pin_number_phone);
uint32_t phone_command_fetch_set_item_CFG(uint32_t etu_length, uint32_t pin_number_phone);
uint32_t authenticate_SIM_response(uint32_t etu_length);
#endif
         
