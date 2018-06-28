#ifndef COMM_UICC_TERMINAL_INTERFACE_H__
#define COMM_UICC_TERMINAL_INTERFACE_H__

#include <stdio.h>
#include "nrf_delay.h"
#include "define_config.h"
#include "nrf_library_update.h"
#include "app_error.h"

void timer0_initialization(void);
void read_byte(uint8_t *info_byte, uint8_t *check_bit, uint32_t etu_ticks, uint32_t pin_number);
void write_byte(uint8_t word_byte, uint8_t parity_bit, uint32_t etu_ticks, uint32_t pin_number);
void write_byte_last(uint8_t word_byte, uint8_t parity_bit, uint32_t etu_ticks, uint32_t pin_number);
void printf_log_tx(uint32_t bytes_size, uint8_t *bytes_infor);
void printf_log_rx(uint32_t bytes_size, uint8_t *bytes_infor);
void get_parity_byte(uint32_t length_byte, uint8_t *byte_in, uint8_t *byte_parity);
void get_parity_bit(uint8_t byte_in, uint8_t *bit_parity);
uint32_t write_bytes(uint32_t bytes_length, uint8_t *bytes_info, uint32_t etu_length, 
        uint32_t pin_number);
uint32_t read_bytes_phone(uint32_t read_length, uint8_t *bytes_info, uint8_t *check_bits, 
        uint32_t pin_number, uint32_t etu_length);
uint32_t parity_bit_check(uint32_t length_byte, uint8_t *byte_in, uint8_t *bit_parity);

#endif

