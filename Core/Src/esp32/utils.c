#include <stdio.h>
#include <stdint.h>

#include "utils.h"

sensor sensors[SENSORS_COUNT];

uint8_t getCRC(uint8_t aDataLen, uint8_t *aData) {
    uint8_t res = 0;
    for (int i = 0; i < aDataLen; i++)
        res += aData[i];
    res = (uint8_t)(0x100 - res);
    return res;
}

void initSensors() {
	sensors[0].type = DATA_TEMPERATURE;
	sensors[1].type = DATA_HUMIDITY;
	sensors[2].type = DATA_AMBIENT;
	//
	for (int i = 0; i < SENSORS_COUNT; i++) {
		sensors[i].id = i;
		sensors[i].data = 0;
		sensors[i].check_alert = 0;
		sensors[i].alert_level = 0;
		sensors[i].alert_flag = 0;
	}
}

sensor* getSensorByType(uint8_t aSensorType) {
	for (int i = 0; i < SENSORS_COUNT; i++)
		if (sensors[i].type == aSensorType)
			return &sensors[i];
	return NULL;
}

uint8_t checkSensorsAlert() {
	uint8_t result = 0;
	for (int i = 0; i < SENSORS_COUNT; i++)
		if (sensors[i].check_alert && (sensors[i].data > sensors[i].alert_level)) {
			sensors[i].alert_flag = 1;
			result = 1;
		}
	return result;
}
