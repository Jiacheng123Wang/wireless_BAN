#ifndef COMM_WIRELESS_INTERFACE_H__
#define COMM_WIRELESS_INTERFACE_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "nrf_delay.h"
#include "define_config.h"

uint32_t transmit_power_level_set(uint32_t power_level);
uint32_t radio_mode_set(uint32_t mode);
uint32_t transmit_info_check(uint32_t packet_length, uint8_t *tx_info_byte);
uint32_t receive_info_check(uint32_t *packet_length, uint8_t *rx_info_byte);
uint32_t tx_radio_config_parameter(uint8_t *tx_packet, uint32_t frequency, 
	     uint32_t logical_address, uint32_t wait_time_out_ms, uint32_t power_level);
uint32_t rx_radio_config_parameter(uint8_t *rx_packet, uint32_t frequency, 
	     uint32_t logical_address, uint32_t wait_time_out_ms);
uint32_t radio_rssi_measurement(uint32_t frequency);
uint32_t radio_carrier_detection(uint32_t max_CD_ms, uint32_t noise_threshold, uint32_t frequency);
uint32_t wireless_sim_authentication_algorithm(uint8_t *user_passwaord, uint8_t *system_key, 
         uint8_t *challenge_data, volatile uint8_t *response_data);
uint32_t wireless_sim_authenrication_initialization(void);
uint32_t ack_wireless_tx(uint8_t *ack_tx, uint32_t frequency, 
                         uint32_t logical_address_tx,
                         uint32_t ack_tx_time_ms, uint32_t tx_power_level_ack);
/*------------------------------------------------------------------------------*/
#if (IF_CC_CODING)
uint32_t int2bit(uint8_t state, uint8_t bit_length, /**/uint8_t *bit_array);
uint32_t bit2int(uint8_t *bit_array, uint8_t bit_length, /**/uint8_t *state);
uint32_t cc_trellis_table_generation(uint8_t memory_length, uint8_t *cc_encoder,
                                /**/ uint8_t *output_bit0, uint8_t *output_bit1, 
                                uint8_t *from_state0, uint8_t *from_state1,
                                uint8_t *to_state0, uint8_t *to_state1);
uint32_t cc_encode_n(uint8_t *input_bit, uint32_t bit_length, 
                uint8_t memory_length, uint8_t *cc_encoder, 
                uint8_t encoder_number, uint8_t initial_state, 
                /**/ uint8_t *encoded_bit);
uint32_t state_trellis_table(uint8_t memory_length,
                /**/uint8_t *next_state, uint8_t *previous_state);
uint32_t distance_between_vector(uint8_t *input1_bit, uint8_t *input2_bit,
                   uint8_t bit_length);
uint32_t min_index_value(uint32_t *vector_in, uint32_t vector_length,
     /**/ uint32_t *min_value, uint32_t *min_index);
uint32_t viterbi_decoding(uint8_t *input_receivebit, 
                uint8_t memory_length, uint8_t encoder_number,
                uint8_t *cc_encoder, uint8_t initial_state, uint8_t depth_of_trellis, 
                /**/ uint8_t *decoded_bit);
uint32_t encoder_halfrate (uint8_t *input_bit, uint32_t bit_length, 
             uint8_t initial_state, uint8_t *encoded_bit);
uint32_t decoder_hallfrate(uint8_t *input_receivebit, uint32_t bit_length,
       uint8_t initial_state, uint8_t *decoded_bit);
uint32_t char2bit_sequence(uint8_t *char_sequence, uint8_t char_length, 
	                       uint8_t *bit_sequence);
uint32_t bit2char_sequence(uint8_t *bit_sequence, uint8_t char_length, 
						   uint8_t *char_sequence);
uint32_t ccencoder_byte_sequence_32(uint8_t *input_bytes, uint8_t encode_byte_length, 
         uint8_t *encoded_bytes);
uint32_t ccdecoder_byte_sequence_32(uint8_t *input_bytes, uint8_t encode_byte_length, 
         uint8_t *decoded_bytes);
uint32_t ccencoder_byte_sequence_64(uint8_t *input_bytes, uint8_t encode_byte_length, 
		 uint8_t *encoded_bytes);
uint32_t ccdecoder_byte_sequence_64(uint8_t *input_bytes, uint8_t encode_byte_length, 
		 uint8_t *decoded_bytes);
#endif
#endif

