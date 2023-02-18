#ifndef UTILS_H
#define UTILS_H

#define BUFFER_SIZE		16

#define SENSORS_COUNT 3
#define DEVICES_COUNT 2 

#define MQTT_ROOT "Evdokimov_VI/bte_final"

#define MODE_PERIODIC 1
#define MODE_IFCHANGED 2
#define MODE_BYCOMMAND 3

#define SENSOR_TEMPERATURE 1
#define SENSOR_HUMIDITY 2
#define SENSOR_AMBIENT 3

#define DEVICE_LED 1
#define DEVICE_FAN 2

#define CMD_SET_MODE 1
#define CMD_SET_PERIOD 2
#define CMD_SET_PERCENTS 3
#define CMD_SET_ALERT_LEVEL 4
#define CMD_SET_ALERT_FLAG 5

#define COMPARE_EQUAL 1
#define COMPARE_LESS 2
#define COMPARE_GREATER 3

#define CMD_GET_SENSORS_DATA 1
#define CMD_SET_DEVICE_STATUS 2
#define CMD_GET_DEVICE_STATUS 3

#define MSG_SENSORS_DATA 1

#define ERR_CRC_FAILED 1

#define DEVICE_STATE_OFF 1
#define DEVICE_STATE_ON 2

typedef struct {
	uint8_t id;
	uint8_t type;
	uint8_t data;
	uint8_t check_alert;
	uint8_t alert_level;
	uint8_t alert_flag;
} sensor;

uint8_t getCRC(uint8_t aDataLen, uint8_t *aData);

void initSensors();

sensor* getSensorByType(uint8_t aSensorType);

uint8_t checkSensorsAlert();

#endif
