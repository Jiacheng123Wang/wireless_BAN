#ifndef WATCH_WIRELESS_INTERFACE_H__
#define WATCH_WIRELESS_INTERFACE_H__

#include <stdint.h>
#include <stdio.h>

void main_watch_phone_sim_wireless(void);
void sim_connection_state_check(void);
uint32_t connection_listening_sim(void);
uint32_t radio_carrier_search(uint32_t max_serach_time_ms);
uint8_t phone_logical_address_search(uint8_t max_serach_time_ms);
uint32_t received_command_sim(uint8_t *bytes_command, uint8_t *parity_bit);
uint32_t watch_command_receive_data(uint8_t *received_data);

#endif
