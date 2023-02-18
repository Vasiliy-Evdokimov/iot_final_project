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
	sensors[0].type = SENSOR_TEMPERATURE;
	sensors[1].type = SENSOR_HUMIDITY;
	sensors[2].type = SENSOR_AMBIENT;
	//
	for (int i = 0; i < SENSORS_COUNT; i++) {
		sensors[i].id = i;
		sensors[i].data = 0;
		sensors[i].alert_check = 0;
		sensors[i].alert_compare = 0;
		sensors[i].alert_value = 0;
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
		if (sensors[i].alert_check && (sensors[i].data > sensors[i].alert_value)) {
			sensors[i].alert_flag = 1;
			result = 1;
		} else sensors[i].alert_flag = 0;
	return result;
}

void fillTxCRC(uint8_t *aTx) {
	uint8_t idx = aTx[1];
	aTx[idx] = getCRC(idx, aTx);
}
