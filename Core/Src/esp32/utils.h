#ifndef UTILS_H
#define UTILS_H

#define BUFFER_SIZE		16

#define SENSORS_COUNT	3

#define MQTT_ROOT "Evdokimov_VI/bte_final/"

#define MODE_PERIODIC			0x11
#define MODE_IFCHANGED			0x12
#define MODE_BYCOMMAND			0x13

#define SENSOR_DHT				0x21
#define SENSOR_APDS				0x22

#define DATA_TEMPERATURE		0x31
#define DATA_HUMIDITY			0x32
#define DATA_AMBIENT			0x33

#define DEVICE_LED				0x41
#define DEVICE_FAN				0x42

#define CMD_SET_MODE			0x51
#define CMD_SET_PERIOD			0x52
#define CMD_SET_PERCENTS		0x53
#define CMD_SET_ALERT_LEVEL		0x54

#define CMD_GET_SENSORS_DATA	0x55
#define CMD_SET_DEVICE_STATUS	0x56
#define CMD_GET_DEVICE_STATUS	0x57

#define MSG_SENSORS_DATA		0x61

#define ERR_CRC_FAILED			0x71

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
