/*
 * plc.h
 *
 *  Created on: Nov 19, 2023
 *      Author: Vasilii Evdokimov
 */

#ifndef SRC_STM_PLC_H_
#define SRC_STM_PLC_H_

#include "main.h"
#include "cmsis_os.h"

typedef struct
{
	uint8_t index;
	GPIO_TypeDef* port;
	uint16_t pin;
} PlcAddress;

void vUpdatePlcData();

void vApplyPlcMasks(uint8_t inputs_states);

void plc_init();

#endif /* SRC_STM_PLC_H_ */
