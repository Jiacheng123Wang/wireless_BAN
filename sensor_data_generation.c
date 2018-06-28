#include "define_config.h"
#include "stdint.h"

/* sensor data to be sent
|  index 0:  data frame package index number, 0 ~ 255
|  index 1: content length in char number, 0 ~ 231 (without channel coding), 0 ~ 62 (channel coding)
|  index 2: sensor date type: SENSOR_DATA_TYPE_BODY_TEMPERATURE, SENSOR_DATA_TYPE_HEART_RATE,
|                             SENSOR_DATA_TYPE_ECG_VALUE, SENSOR_DATA_TYPE_BLOOD_PRESURE,
|                             SENSOR_DATA_TYPE_BLOOD_OXYGEN 
|  index 3 ~ : sensor data, the length is the value of SENSOR_DATA[1] */
extern uint8_t SENSOR_DATA[SENSOR_DATA_LENGTH_CHAR];

/* sensor data of body temperature 
|  index 0:  data length, 0 ~ 255
|  index 1:  data type, SENSOR_DATA_TYPE_BODY_TEMPERATURE
|  index 2 ~ : sensor data of body temperature */
extern uint8_t BODY_TEMPERATURE[256];

/* sensor data of heart rate 
|  index 0:  data length, 0 ~ 255
|  index 1:  data type, SENSOR_DATA_TYPE_HEART_RATE
|  index 2 ~ : sensor data of heart rate */
extern uint8_t HEART_RATE[256];

/* sensor data of ECG value
|  index 0:  data length, 0 ~ 255
|  index 1:  data type, SENSOR_DATA_TYPE_ECG_VALUE
|  index 2 ~ : sensor data of ECG value */
extern uint8_t ECG_VALUE[256];

/* sensor data of blood presure
|  index 0:  data length, 0 ~ 255
|  index 1:  data type, SENSOR_DATA_TYPE_BLOOD_PRESURE
|  index 2 ~ : sensor data of blood presure */
extern uint8_t BLOOD_PRESURE[256];

/* sensor data of blood oxygen
|  index 0:  data length, 0 ~ 255
|  index 1:  data type, SENSOR_DATA_TYPE_BLOOD_OXYGEN
|  index 2 ~ : sensor data of blood oxygen */
extern uint8_t BLOOD_OXYGEN[256];

void sensor_data(void);
uint8_t random_vector_generate(uint8_t *p_buff, uint8_t size);
	
/********************************************************************************/
void sensor_data(void)
/*--------------------------------------------------------------------------------
| random sensor data generation 
|
--------------------------------------------------------------------------------*/
{
	static uint32_t sensor_type_index = 0;
	
	sensor_type_index++;
  /* data package index */
  SENSOR_DATA[0] = 5;
  /* data package length, include 1 byte of sensor data type */
  SENSOR_DATA[1] = 9;
	/* sensor data type */
	switch (sensor_type_index % 5)
	{
		case 0:
		{
			SENSOR_DATA[2] = SENSOR_DATA_TYPE_BODY_TEMPERATURE;
			break;
		}

		case 1:
		{
			SENSOR_DATA[2] = SENSOR_DATA_TYPE_HEART_RATE;
			break;
		}

		case 2:
		{
			SENSOR_DATA[2] = SENSOR_DATA_TYPE_ECG_VALUE;
			break;
		}

		case 3:
		{
			SENSOR_DATA[2] = SENSOR_DATA_TYPE_BLOOD_PRESURE;
			break;
		}

		case 4:
		{
			SENSOR_DATA[2] = SENSOR_DATA_TYPE_BLOOD_OXYGEN;
			break;
		}
	}
  /* random sensor data package to transmit */
  random_vector_generate(SENSOR_DATA + 3, SENSOR_DATA[1] - 1);	
}

