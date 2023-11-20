/*
 * plc.c
 *
 *  Created on: Nov 19, 2023
 *      Author: Vasilii Evdokimov
 */

#include "stm_utils.h"
#include "utils.hpp"
#include "plc.h"

PlcAddress plc_inputs[PLC_INPUTS_COUNT];
PlcAddress plc_outputs[PLC_OUTPUTS_COUNT];

TaskHandle_t xPlcInputsHandlerTask;
TaskHandle_t xPlcOutputsHandlerTask;
QueueHandle_t xPlcQueue;

void initPlcInputs()
{
	plc_inputs[0].index = 0;
	plc_inputs[0].port = BTN_1_GPIO_Port;
	plc_inputs[0].pin = BTN_1_Pin;
	//
	plc_inputs[1].index = 1;
	plc_inputs[1].port = BTN_2_GPIO_Port;
	plc_inputs[1].pin = BTN_2_Pin;
	//
	plc_inputs[2].index = 2;
	plc_inputs[2].port = BTN_3_GPIO_Port;
	plc_inputs[2].pin = BTN_3_Pin;
	//
	plc_inputs[3].index = 3;
	plc_inputs[3].port = BTN_4_GPIO_Port;
	plc_inputs[3].pin = BTN_4_Pin;
}

void initPlcOutputs()
{
	plc_outputs[0].index = 0;
	plc_outputs[0].port = PLC_LED_1_GPIO_Port;
	plc_outputs[0].pin = PLC_LED_1_Pin;
	//
	plc_outputs[1].index = 1;
	plc_outputs[1].port = PLC_LED_2_GPIO_Port;
	plc_outputs[1].pin = PLC_LED_2_Pin;
	//
	plc_outputs[2].index = 2;
	plc_outputs[2].port = PLC_LED_3_GPIO_Port;
	plc_outputs[2].pin = PLC_LED_3_Pin;
	//
	plc_outputs[3].index = 3;
	plc_outputs[3].port = PLC_LED_4_GPIO_Port;
	plc_outputs[3].pin = PLC_LED_4_Pin;
}

void initPlcOutputsMasks()
{
	plc_outputs_masks[0].mask = 0x9;
	plc_outputs_masks[0].all_bits = 1;
	//
	plc_outputs_masks[1].mask = 0x1;
	plc_outputs_masks[1].all_bits = 1;
	//
	plc_outputs_masks[2].mask = 0xC;
	plc_outputs_masks[2].all_bits = 1;
	//
	plc_outputs_masks[3].mask = 0xF;
	plc_outputs_masks[3].all_bits = 0;
}

void vPlcInputsHahdlerTask(void * pvParameters)
{
	uint8_t	input_statuses[PLC_INPUTS_COUNT];
	uint8_t	input_statuses_p[PLC_INPUTS_COUNT];
	uint8_t input_index;
	uint8_t inputs_states = 0, inputs_states_p = 0;
	//
	for(;;)	{
		for (int i = 0; i < PLC_INPUTS_COUNT; i++)
		{
			input_statuses[i] = HAL_GPIO_ReadPin(plc_inputs[i].port, plc_inputs[i].pin);
			input_index = plc_inputs[i].index;
			if (input_statuses[i] == input_statuses_p[i])
			{
				if (input_statuses[i])
				{
					inputs_states ^= (1 << input_index);
				}
			} else input_statuses_p[i] = input_statuses[i];
		}
		//
		if (inputs_states_p != inputs_states)
		{
			printf("inputs_states_changed!\n");
			//
			xQueueSend(xPlcQueue, &inputs_states, portMAX_DELAY);
			//
			inputs_states_p = inputs_states;
			//
			plc_inputs_states = inputs_states;
			printByte("plc_inputs_states", plc_inputs_states);
		}
		//
		osDelay(50);
	}
}

void vPlcOutputsHahdlerTask(void * pvParameters)
{
	uint8_t inputs_states;
	//
	for(;;)
	{
		if (xQueueReceive(xPlcQueue, &inputs_states, portMAX_DELAY))
		{
			for (int i = 0; i < PLC_OUTPUTS_COUNT; i++)
			{
				PlcMask plcm = plc_outputs_masks[i];
				uint8_t fl1 = (plcm.all_bits) && !(plcm.mask ^ (inputs_states & plcm.mask));
				uint8_t fl2 = (!plcm.all_bits) && (plcm.mask | inputs_states) && inputs_states;
				if (fl1 || fl2) {
					HAL_GPIO_WritePin(plc_outputs[i].port, plc_outputs[i].pin, GPIO_PIN_SET);
					plc_outputs_states |= (1 << plc_outputs[i].index);
				}
				else
				{
					HAL_GPIO_WritePin(plc_outputs[i].port, plc_outputs[i].pin, GPIO_PIN_RESET);
					plc_outputs_states &= ~(1 << plc_outputs[i].index);
				}
			}
			printByte("plc_outputs_states", plc_outputs_states);
		}
		//
		osDelay(10);
	}
}

void initPlcTasks()
{
	char buf[255] = {0};
	//
	xPlcQueue = xQueueCreate( 1, sizeof(uint8_t) );
	if( xPlcQueue == NULL )
	{
		printf("xPlcQueue creation failed!\n");
	} else {
		printf("xPlcQueue was successfully created!\n");
	}

	if ( xTaskCreate(
			vPlcInputsHahdlerTask,
			buf,
			128,
			NULL,
			osPriorityNormal,
			&xPlcInputsHandlerTask
		) != pdPASS )
	{
		printf("xPlcInputsHahdlerTask creation failed!\n");
	} else {
		printf("xPlcInputsHahdlerTask was successfully created!\n");
	}

	if ( xTaskCreate(
			vPlcOutputsHahdlerTask,
			buf,
			128,
			NULL,
			osPriorityNormal,
			&xPlcOutputsHandlerTask
		) != pdPASS )
	{
		printf("xPlcOutputsHandlerTask creation failed!\n");
	} else {
		printf("xPlcOutputsHandlerTask was successfully created!\n");
	}
}

void plc_init()
{
	initPlcInputs();
	initPlcOutputs();
	initPlcOutputsMasks();
	//
	initPlcTasks();
}
