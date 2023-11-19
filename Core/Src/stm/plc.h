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

#define PLC_INPUTS_COUNT 4
#define PLC_OUTPUTS_COUNT 4

typedef struct
{
	uint8_t index;
	GPIO_TypeDef* port;
	uint16_t pin;
	//
	uint8_t mask;
	uint8_t all_bits;
} PlcStruct;

void plc_init();

#endif /* SRC_STM_PLC_H_ */
