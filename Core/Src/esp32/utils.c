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
	for (int i = 0; i < SENSORS_COUNT; i++) {
		sensors[i].id = i;
    sensors[i].type = i + 1;
		sensors[i].value = 0;
    sensors[i].previous_value = 0;
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
  uint8_t cmp, sval, aval;
	for (int i = 0; i < SENSORS_COUNT; i++) {
    cmp = sensors[i].alert_compare;
    sval = sensors[i].value;
    aval = sensors[i].alert_value;
		if (sensors[i].alert_check && (    
        ((cmp == COMPARE_EQUAL) && (sval == aval)) ||
        ((cmp == COMPARE_LESS) && (sval < aval)) ||
        ((cmp == COMPARE_GREATER) && (sval > aval))
		    ))
    {
			sensors[i].alert_flag = 1;
			result = 1;
		} else sensors[i].alert_flag = 0;
	}   
	return result;
}

uint8_t checkSensorsPercents(uint8_t aPercents) {
  uint8_t result = 0;  
  uint8_t d;
  for (int i = 0; i < SENSORS_COUNT; i++) {
    d = (int)(((float)sensors[i].value / (float)sensors[i].previous_value) * 100);
    if (abs(100 - d) >= aPercents) {
      result = 1;
      break;
    }
  }
  return result;
}

void fillTxCRC(uint8_t *aTx) {
	uint8_t idx = aTx[1];
	aTx[idx] = getCRC(idx, aTx);
}
