#ifndef PARAMETER_CONFIG_H__
#define PARAMETER_CONFIG_H__

void uart_initialization(void);
void update_user_configuration(void);
void power_management(void);
void USIM_initialization(void);
void flash_word_write(uint32_t *address, uint32_t value);
void radio_configure(void);
void reader_sim_init(void);
void device_peripheral_initialization(void);
void rtc1_radio_link_config(void);
    
#endif

