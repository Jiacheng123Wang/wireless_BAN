#ifndef PHONE_WIRELESS_INTERFACE_H__
#define PHONE_WIRELESS_INTERFACE_H__

#include <stdint.h>
#include "define_config.h"

void main_smart_phone_sim_local_wireless(void);
void phone_connection_state_check(void);
uint32_t connection_request_phone(uint8_t connection_type);
void phone_write_sim_files_data(void);
uint32_t phone_command_get_file_data(uint16_t file_ID, uint8_t *file_data);
uint32_t phone_command_send_data(uint8_t *send_data, uint8_t data_length, uint8_t data_type);

#endif

