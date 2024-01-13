/*
 * plc.c
 *
 *  Created on: Nov 19, 2023
 *      Author: Vasilii Evdokimov
 */

#include "stm_utils.h"
#include "utils.hpp"
#include "functions.h"
#include "plc.h"

PlcAddress plc_inputs[PLC_INPUTS_COUNT];
PlcAddress plc_outputs[PLC_OUTPUTS_COUNT];

TaskHandle_t xPlcInputsHandlerTask;
TaskHandle_t xPlcOutputsHandlerTask;
QueueHandle_t xPlcQueue;

TaskHandle_t xPlcDataUpdateTask;
QueueHandle_t xPlcDataUpdateQueue;

void initPlcInputs()
{
	plc_inputs[0].port = BTN_1_GPIO_Port;
	plc_inputs[0].pin = BTN_1_Pin;
	//
	plc_inputs[1].port = BTN_2_GPIO_Port;
	plc_inputs[1].pin = BTN_2_Pin;
	//
	plc_inputs[2].port = BTN_3_GPIO_Port;
	plc_inputs[2].pin = BTN_3_Pin;
	//
	plc_inputs[3].port = BTN_4_GPIO_Port;
	plc_inputs[3].pin = BTN_4_Pin;
}

void initPlcOutputs()
{
	plc_outputs[0].port = PLC_LED_1_GPIO_Port;
	plc_outputs[0].pin = PLC_LED_1_Pin;
	//
	plc_outputs[1].port = PLC_LED_2_GPIO_Port;
	plc_outputs[1].pin = PLC_LED_2_Pin;
	//
	plc_outputs[2].port = PLC_LED_3_GPIO_Port;
	plc_outputs[2].pin = PLC_LED_3_Pin;
	//
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
	uint8_t	input_statuses[PLC_INPUTS_COUNT] = {0};
	uint8_t	input_statuses_p[PLC_INPUTS_COUNT] = {0};
	uint8_t inputs_states = 0, inputs_states_p = 0;
	//
	for(;;)
	{
		for (int i = 0; i < PLC_INPUTS_COUNT; i++)
		{
			input_statuses[i] = HAL_GPIO_ReadPin(plc_inputs[i].port, plc_inputs[i].pin);
			if (input_statuses[i] == input_statuses_p[i])
			{
				if (input_statuses[i])
				{
					inputs_states ^= (1 << i);
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
		osDelay(60);
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
			vApplyPlcMasks(inputs_states);
			vUpdatePlcData();
			printByte("plc_outputs_states", plc_outputs_states);
		}
		//
		osDelay(10);
	}
}

void vApplyPlcMasks(uint8_t inputs_states)
{
	for (int i = 0; i < PLC_OUTPUTS_COUNT; i++)
	{
		PlcMask plcm = plc_outputs_masks[i];
		uint8_t fl1 = (plcm.all_bits) && !(plcm.mask ^ (inputs_states & plcm.mask));
		uint8_t fl2 = (!plcm.all_bits) && (plcm.mask | inputs_states) && inputs_states;
		if (fl1 || fl2)
		{
			HAL_GPIO_WritePin(plc_outputs[i].port, plc_outputs[i].pin, GPIO_PIN_SET);
			plc_outputs_states |= (1 << i);
		}
		else
		{
			HAL_GPIO_WritePin(plc_outputs[i].port, plc_outputs[i].pin, GPIO_PIN_RESET);
			plc_outputs_states &= ~(1 << i);
		}
	}
}

void vPlcDataUpdateTask(void * pvParameters)
{
	uint8_t flag;
	//
	for(;;)
	{
		while (xQueueReceive(xPlcDataUpdateQueue, &flag, portMAX_DELAY))
		{
			uint8_t aTX[BUFFER_SIZE];
			fillTxPlcMasksData(aTX);
			doUartTransmit(aTX);
		}
		//
		osDelay(10);
	}
}

void vUpdatePlcData()
{
	uint8_t flag = 1;
	xQueueSend(xPlcDataUpdateQueue, &flag, portMAX_DELAY);
}

void initPlcTasks()
{
	char buf[255] = {0};
	//
	xPlcQueue = xQueueCreate( 1, sizeof(uint8_t) );
	if( xPlcQueue == NULL )
	{
		printf("xPlcQueue creation failed!\n");
	}
	else
	{
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
	}
	else
	{
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
	}
	else
	{
		printf("xPlcOutputsHandlerTask was successfully created!\n");
	}

	xPlcDataUpdateQueue = xQueueCreate( 32, sizeof(uint8_t) );
	if( xPlcDataUpdateQueue == NULL )
	{
		printf("xPlcDataUpdate creation failed!\n");
	}
	else
	{
		printf("xPlcDataUpdate was successfully created!\n");
	}

	if ( xTaskCreate(
			vPlcDataUpdateTask,
			buf,
			128,
			NULL,
			osPriorityNormal,
			&xPlcDataUpdateTask
		) != pdPASS )
	{
		printf("xPlcDataUpdateTask creation failed!\n");
	}
	else
	{
		printf("xPlcDataUpdateTask was successfully created!\n");
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
