#ifndef FLASH_FILE_ADDRESS_H__
#define FLASH_FILE_ADDRESS_H__

#include "define_config.h"
#if (ARGET_CHIP == NRF52)
  #include "nrf52.h"
#elif (ARGET_CHIP == NRF51) 
  #include "nrf51.h"
#else
  #error "not valid ARGET_CHIP value ..." 
#endif  

/* flash page size: 4K for nRF52, 1K for nRF51 */
#define FLASH_PAGE_SIZE (NRF_FICR->CODEPAGESIZE)
/* flash page number: 128 for nRF52_xxaa, 256 for nRF51_xxaa */
#define FLASH_PAGE_NUMBER (NRF_FICR->CODESIZE)
/* code flash size, in K bytes, other flash used for user data */
#define FLASH_CODE_SIZE 128

/* start page number for flash data */
#define EF_FILES_START_PAGE (FLASH_CODE_SIZE / (FLASH_PAGE_SIZE / 1024))

/* offset 0: swap page for flash data update */
#define SWAP_FLASH_ADDR (FLASH_PAGE_SIZE * EF_FILES_START_PAGE)

/* offset 1: user setting configurations 
| byte 1 ~ 16: user password for wireless SIM connection authentication
| byte 17 ~ 20: wireless SIM authentication flag, 1: authentication on, 0: authentication off
*/
#define USER_CONFIG_FLASH_ADDR (FLASH_PAGE_SIZE * (EF_FILES_START_PAGE + 1))

/* offset 2: mandatory EF files data for USIM
| byte 1 ~ 248: mandatory USIM EF data
*/
#define USIM_EF_FLASH_ADDR (FLASH_PAGE_SIZE * (EF_FILES_START_PAGE + 2))

/* offset 3: mandatory EF files data for SIM application 
| byte 1 ~ 252: mandatory SIM EF data
*/
#define SIM_EF_FLASH_ADDR (FLASH_PAGE_SIZE * (EF_FILES_START_PAGE + 3))

/* offset 10: phone book 
*/
#define PHONE_BOOK_FLASH_ADDR (FLASH_PAGE_SIZE * (EF_FILES_START_PAGE + 10))

/* offset 20: pre-define SMS content 
*/
#define PREDEFINED_SMS_FLASH_ADDR (FLASH_PAGE_SIZE * (EF_FILES_START_PAGE + 20))

/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/* user password: 4 words = 16 bytes*/
#define USER_PASSWORD_FLASH_ADDR (USER_CONFIG_FLASH_ADDR)

/* flag for authentication check (wireless SIM phone connection request: 1 word = 4 bytes */
/* 0: no authentication check */
/* 1: password check for each phone connection check, and authentication check for each phone command */
#define FLAG_PASSWORD_FLASH_ADDR (USER_CONFIG_FLASH_ADDR + 16)

/* Phone/Watch name: 4 words = 16 bytes */
#define NAME_FLASH_ADDR (USER_CONFIG_FLASH_ADDR + 20)

/* logical address: 1 word = 4 bytes */
#define PHONE_ADD_FLASH_ADDR (USER_CONFIG_FLASH_ADDR + 36)

/* call receiver defaut seting mask byte: 1 word = 4 bytes */
#define RECEIVE_CALL_FLASH_ADDR (USER_CONFIG_FLASH_ADDR + 40)

/* wireless SIM On/Off flag: 1 word = 4 bytes */
/* 0: wireless SIM off */
/* 1: wireless SIM on */
#define FLAG_WIRELESS_SIM_FLASH_ADDR (USER_CONFIG_FLASH_ADDR + 44)

/* Watch call On/Off flag: 1 word = 4 bytes */
/* 0: watch call off */
/* 1: watch call on */
#define FLAG_WATCH_CALL_FLASH_ADDR (USER_CONFIG_FLASH_ADDR + 48)

/* system key: 4 words = 16 bytes*/
#define SYSTEM_KEY_FLASH_ADDR (USER_CONFIG_FLASH_ADDR + 52)


/* start page number of code flash to store USIM EF data */
#define ICCID_FLASH_ADDR (USIM_EF_FLASH_ADDR + 1)

#endif

