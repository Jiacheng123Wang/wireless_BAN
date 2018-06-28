#ifndef PHONE_COMMAND_SIM_H__
#define PHONE_COMMAND_SIM_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "define_config.h"

uint32_t phone_reset_initial(uint32_t etu_length, uint32_t pin_number_io);
uint32_t phone_command_f2_response(uint8_t *bytes_command, uint32_t etu_length, uint32_t pin_number_phone);
uint32_t phone_command_unblock_pin(uint8_t *phone_command, uint32_t etu_length);
uint32_t phone_command_verify_20(uint8_t *phone_command, uint32_t etu_length);
uint32_t phone_command_select_mandatory(uint8_t *phone_command, uint32_t etu_length);
uint32_t phone_command_read_binary_mandatory(uint8_t *phone_command, uint32_t etu_length);
uint32_t phone_command_update_binary_mandatory(uint8_t *phone_command, uint32_t etu_length);
uint32_t phone_command_read_record_mandatory(uint8_t *phone_command, uint32_t etu_length);
uint32_t phone_command_update_record(uint8_t *phone_command, uint32_t etu_length);
uint32_t authenticate_sim_response(uint32_t etu_length);
void printf_selected_file(uint64_t selected_file);

#endif

