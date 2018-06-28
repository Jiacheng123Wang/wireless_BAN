#ifndef COMM_MISC_H__
#define COMM_MISC_H__

#include <stdint.h>
#include <stdio.h>
#include "app_error.h"
#include "flash_file_address.h"

uint32_t start_oscillator_16m(void);
void stop_oscillator_16m(void);
void flash_page_erase(uint32_t *page_address);
void flash_word_write(uint32_t *address, uint32_t value);
void flash_byte_string_update(uint32_t *address_swap_page, uint32_t *address_update_page, 
     uint32_t address_offset_in_word, uint32_t update_length_in_byte, uint8_t *byte_string);
uint8_t byte_string_comparison(uint8_t byte_length, uint8_t *byte_1, uint8_t *byte_2);
uint8_t password_check(uint8_t password_length, uint8_t *password_saved, uint8_t *password_input);
uint8_t random_vector_generate(uint8_t * p_buff, uint8_t size);
void uart_initialization(void);
void simple_uart_config(uint8_t rts_pin_number, uint8_t txd_pin_number,
     uint8_t cts_pin_number, uint8_t rxd_pin_number, bool hwfc);
uint8_t simple_uart_get(void);
void simple_uart_put(uint8_t cr);
void simple_uart_getstring(uint8_t *str, uint8_t *str_length);
void simple_uart_getstring_with_timeout(uint32_t timeout_ms, uint8_t max_length, uint8_t *str, uint8_t *str_length);
void simple_uart_putstring(const uint8_t *str);
void power_management(void);
void USIM_initialization(void);
void start_WDT(uint32_t time_out_s); 
void update_user_configuration(void);
void uart_rx_buffer_clear(void);

#endif
