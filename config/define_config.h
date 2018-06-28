#ifndef DEFINE_CONFIG_H__
#define DEFINE_CONFIG_H__

/* 1: sensor side, 0: smart phone side */
#define SENSOR                            0

/* NRF52, NRF51 */
#define ARGET_CHIP                        NRF52

/* sensor data */
/* the data package header is 22 bytes (6 + 16), and the maximum length of radio data is 255 */
#define SENSOR_DATA_LENGTH_CHAR           233    
#define WIRELESS_DATA_TYPE_SENSOR_DATA    0xaa 

/* sensor data type */
#define SENSOR_DATA_TYPE_BODY_TEMPERATURE 0x1 
#define SENSOR_DATA_TYPE_HEART_RATE       0x2 
#define SENSOR_DATA_TYPE_ECG_VALUE        0x3 
#define SENSOR_DATA_TYPE_BLOOD_PRESURE    0x4 
#define SENSOR_DATA_TYPE_BLOOD_OXYGEN     0x5 

#define SIM_APPLICATION_DISPLAY_LENGTH    16 
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/* 1: cc coding is used for wireless link, 0: cc coding not used */
#define IF_CC_CODING           1

/* if print out the log information, for debug purpose */
#define IF_PRINT_LOG           1

/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
#if (ARGET_CHIP == NRF52)
  #ifndef NRF52
    #undef  NRF51
    #define NRF52
  #endif  
#elif (ARGET_CHIP == NRF51) 
  #ifndef NRF51
    #undef  NRF52
    #define NRF51
  #endif  
#else
  #error "not valid ARGET_CHIP value ..." 
#endif

#if (ARGET_CHIP == NRF52)
  /* sim-phone interface PIN define, watch side for SIM application interface */
  #define PIN_DATA_PHONE       11
  #define PIN_RESET_PHONE      3
  #define PIN_CLOCK_PHONE      4
  
  /* UART pin define */
  #define RX_PIN_NUMBER        8
  #define TX_PIN_NUMBER        6
  #define CTS_PIN_NUMBER       7
  #define RTS_PIN_NUMBER       5
#elif (ARGET_CHIP == NRF51) 
  /* sim-phone interface PIN define, phone side for SIM application interface */
  #define PIN_DATA_PHONE       28
  #define PIN_RESET_PHONE      1
  #define PIN_CLOCK_PHONE      25
  
  /* UART pin define */
  #define RX_PIN_NUMBER        11
  #define TX_PIN_NUMBER        9
  #define CTS_PIN_NUMBER       10
  #define RTS_PIN_NUMBER       8
#endif /* end of #if (ARGET_CHIP == NRF52) */

/* delay between SIM interface Tx and Rx */
#define PRINT_DELAY_MS              5

/* maximal radio packet size */
#define PACKET_MAX_SIZE             250
#define MAX_PRINTF_SIZE             260

/* time out for phone command read, in ms */
#define PHONE_COMMAND_TIME_OUT_MS   50  
/* watchdog's timeout period, in 250ms */
#define WATCH_DOG_TIME_OUT_S        4

/* set USAT menu/item, 1: menu, 0: item */
#define SET_WATCH_USAT_MENU    1

#if (SET_WATCH_USAT_MENU)  
  #define USAT_BYTE_LENGTH_BACK_INIT  105
#else
  #define USAT_BYTE_LENGTH_BACK_INIT  31
  #define USAT_BYTE_LENGTH_ITEM       209 
#endif

/* predefined phone book item and SMS contenet item from phone side */
#define PHONE_BOOK_LENGTH_CHAR          56 
#define PHONE_BOOK_NUMBER_START_INDEX   34 
#define PREDEFINED_SMS_LENGTH_CHAR      164

/**< max number of test bytes to be used for tx and rx. */        
/**< UART TX buffer size. */
#define UART_TX_BUF_SIZE       256             
/**< UART RX buffer size. */
#define UART_RX_BUF_SIZE       256   
/* UART RX buffer receive time out, in ms */
#define UART_RX_TIME_OUT_MS    1000   

/* time for 16 bytes random number generation, in ms */
#define RANDOM_BYTE_NUMBER_TIME_OUT_MS    6  

/*--------------------------------------------------------------------------------*/
/* wireless interface protocol related parameter config */
/*--------------------------------------------------------------------------------*/
/* time interval for watch dog time reset, in 125ms */
#define WATCH_DOG_RESET_TIME            2

/* time interval for phone to transmit a wireless connection state package, in 250ms (=WATCH_DOG_RESET_TIME * 125ms)*/
// #define CONNECT_STATE_TIME_PHONE      40
#define CONNECT_STATE_TIME_PHONE      10
/* time for phone do not receive feedback confirm message for wireless connection request from watch, 
*  in CONNECT_STATE_TIME * CONNECT_STATE_TIME_PHONE * 500ms, phone assume there is no connection between SIM-phone */
#define PHONE_DISCONNECT_TIME         3
  
/* time for SIM do not receive wireless connection request from phone, in CONNECT_STATE_TIME * 500ms, 
|  SIM assume there is no connection between SIM-phone */
#define SIM_DISCONNECT_TIME           120

/* password bytes length for random access request */
#define KEY_LENGTH                   16
/* parameters for access channel, they are pre-defined between Tx and Rx */
#define FREQUENCY_CARRIER_ACCESS     40UL
#define LOGICAL_CHANNEL_TX_ACCESS    0UL
#define LOGICAL_CHANNEL_RX_ACCESS    1UL

/* radio transmission mode
|  0: 250Kbps
|  1: 1Mbps
|  2: 2Mpbs
|  3: 1Mbps, BLE
|  default: 250Kbps */
#if (ARGET_CHIP == NRF52)
  #define RADIO_MODE          1
#else
  #define RADIO_MODE          0
#endif

/* radio transmission power for data transmissiom 
|  0: -30dBm for nRF51, -40dBm for nRF52
|  1: -20dBm
|  2: -16dBm
|  3: -12dBm
|  4: -8dBm
|  5: -4dBm
|  6: 0dBm
|  7: 4dBm 
|  8: 3dBm, nRF52 only */
#define RADIO_TX_POWER_DATA 7

/* Tx power level for ACK/NACK feedback */
#define POWER_LEVEL_ACK 7

/* Tx power level for phone connection request */
#define POWER_LEVEL_CONNECTION 7

/* if data carrier frequency hopping
|  0: no hopping, use fixed carrier frequency
|  1: random frequency hopping */
#define IF_FREQ_HOP 1

/* if ACK meeasge is send for each data package received
|  0: no ACK message, only the data package send out
|  1: ACK message is send out for each data package received. if no ACK message, send data package again.
|     If CRC check fails, a NCK massage is send to Tx, and Tx also send out data package again */
#define IF_DATA_ACK 1

/* time resolution is 1ms */
#if (ARGET_CHIP == NRF52)
  /* time duration for SIM to listen command from phone */
  #define SIM_LISTEN_TIME             7
  /* time duration for phone to send out random access request message */
  #define PHONE_REQUEST_TX_TIME       1
  /* time duration for phone to wait confirm message of random access from SIM */
  #define PHONE_REQUEST_WAIT_TIME     4
#else
  #if (IF_CC_CODING)
    /* time duration for SIM to listen command from phone */
    #define SIM_LISTEN_TIME             11
    /* time duration for phone to send out random access request message */
    #define PHONE_REQUEST_TX_TIME       3
    /* time duration for phone to wait confirm message of random access from SIM */
    #define PHONE_REQUEST_WAIT_TIME     8
  #else
    /* time duration for SIM to listen command from phone */
    #define SIM_LISTEN_TIME             7
    /* time duration for phone to send out random access request message */
    #define PHONE_REQUEST_TX_TIME       1
    /* time duration for phone to wait confirm message of random access from SIM */
    #define PHONE_REQUEST_WAIT_TIME     4
  #endif
#endif

/* time resolution is 1ms */
/* max time for a data package transmission, may be several transmission try */
#define DATA_TX_TIME            1000

/* time interval for each data package transmission try, if no ACK confirm message received */
#define DATA_TX_TIME_INTERVAL   20
/* max waiting time for a data package receiving */
#define DATA_RX_TIME            1000

/* ACK transmission time out */
#define ACK_TX_TIME             5
/* ACK receiving time out */
#define ACK_RX_TIME             10

/* time resolution is 1ms */
/* maximal carrier frequency search time to find out good carrier, in ms */
#define CARRIER_FREQ_SEARCH     10
/* carrier frequency bin search threshold */
#if (RADIO_MODE == 0)
  #define CARRIER_FREQ_THRE     100
#elif (RADIO_MODE == 1)
  #define CARRIER_FREQ_THRE     94
#else
  #define CARRIER_FREQ_THRE     90
#endif  

/* maximal carrier frequency colliding detection time, in ms */
#define MAX_CD_TIME_MS          10
/* carrier colliding detection, background signal level in -dBm */
/* Set the threshold for good carrier frequency search, for radio mode 2Mbps, the minimal measured 
|  RSSI is -93dBm, for mode 250Kbps and 1Mbps, the minimal measured RSSI is -102dBm */
#if (RADIO_MODE == 0)
  #define TX_RSSI_THRED CARRIER_FREQ_THRE
#elif (RADIO_MODE == 1)
  #define TX_RSSI_THRED CARRIER_FREQ_THRE
#else
  #define TX_RSSI_THRED CARRIER_FREQ_THRE
#endif 


/* */
/*--------------------------------------------------------------------------------*/
/* conncetion request information byte number */
#define CONNECTION_REQUEST_BYTE     32
#if (IF_CC_CODING)
/* cc encoder byte number for each coding block = (1 << ENCODE_BLOCK_INDEX ) */
#define ENCODE_BLOCK_INDEX          0
#endif 

#define ADC_IRQ_PRIORITY            0
#define GPIOTE_IRQ_PRIORITY         1
#define RADIO_LINK_IRQ_PRIORITY     7
#define TIMER0_IRQ_PRIORITY         0
    
#endif

