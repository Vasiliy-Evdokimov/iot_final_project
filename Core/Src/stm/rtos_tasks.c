/*
 * rtos_tasks.c
 *
 *  Created on: Nov 19, 2023
 *      Author: Vasilii Evdokimov
 */

#include "stm_utils.h"
#include "functions.h"
#include "rtos_tasks.h"

TaskHandle_t xGetSensorsDataTask;
TaskHandle_t xMainLoopTask;

TaskHandle_t xUartTransmitterTask;
QueueHandle_t xUartTransmitterQueue;

TaskHandle_t xUartReceiverTask;
QueueHandle_t xUartReceiverQueue;

extern UART_HandleTypeDef huart1;

void vGetSensorsDataTask(void * pvParameters)
{
	for(;;)
	{
		getSensorsData();
		//
		osDelay(500);
	}
}

void vMainLoopTask(void * pvParameters)
{
	for(;;)
	{
		mainLoop();
		//
		osDelay(10);
	}
}

void vUartTransmitterTask(void * pvParameters)
{
	UartBuffer uartBuffer;
	for(;;)
	{
		while (xQueueReceive(xUartTransmitterQueue, &uartBuffer, portMAX_DELAY))
		{
			printUartBuffer("TX", uartBuffer.bytes);
			HAL_UART_Transmit(&huart1, uartBuffer.bytes, BUFFER_SIZE, 10000);
			HAL_UART_AbortTransmit(&huart1);
			osDelay(100);
		}
		//
		osDelay(100);
	}
}

void vAddUartTransmitterTask(UartBuffer aUartBuffer)
{
	xQueueSend(xUartTransmitterQueue, &aUartBuffer, portMAX_DELAY);
}

void vUartReceiverTask(void * pvParameters)
{
	UartBuffer uartBuffer;
	for(;;)
	{
		while (xQueueReceive(xUartReceiverQueue, &uartBuffer, portMAX_DELAY))
		{
			printUartBuffer("RX", uartBuffer.bytes);
			handleUART(uartBuffer);
			osDelay(100);
		}
		//
		osDelay(100);
	}
}

void vAddUartReceiverTask(UartBuffer aUartBuffer)
{
	xQueueSend(xUartReceiverQueue, &aUartBuffer, portMAX_DELAY);
}

void tasks_init()
{
	char buf[255] = {0};
	//
	xUartTransmitterQueue = xQueueCreate( 32, sizeof(UartBuffer) );
	if( xUartTransmitterQueue == NULL )
	{
		printf("xUartTransmitterQueue creation failed!\n");
	} else {
		printf("xUartTransmitterQueue was successfully created!\n");
	}
	//
	if ( xTaskCreate(
			vUartTransmitterTask,
			buf,
			128,
			NULL,
			osPriorityNormal,
			&xUartTransmitterTask
		) != pdPASS )
	{
		printf("xUartTransmitterTask creation failed!\n");
	} else {
		printf("xUartTransmitterTask was successfully created!\n");
	}
	//
	//
	xUartReceiverQueue = xQueueCreate( 32, sizeof(UartBuffer) );
	if( xUartReceiverQueue == NULL )
	{
		printf("xUartReceiverQueue creation failed!\n");
	} else {
		printf("xUartReceiverQueue was successfully created!\n");
	}
	//
	if ( xTaskCreate(
			vUartReceiverTask,
			buf,
			128,
			NULL,
			osPriorityNormal,
			&xUartReceiverTask
		) != pdPASS )
	{
		printf("xUartReceiverTask creation failed!\n");
	} else {
		printf("xUartReceiverTask was successfully created!\n");
	}
	//
	//
	if ( xTaskCreate(
			vGetSensorsDataTask,
			buf,
			128,
			NULL,
			osPriorityNormal,
			&xGetSensorsDataTask
		) != pdPASS )
	{
		printf("xGetSensorsDataTask creation failed!\n");
	} else {
		printf("xGetSensorsDataTask was successfully created!\n");
	}
	//
	//
	if ( xTaskCreate(
			vMainLoopTask,
			buf,
			128,
			NULL,
			osPriorityNormal,
			&xMainLoopTask
		) != pdPASS )
	{
		printf("xMainLoopTask creation failed!\n");
	} else {
		printf("xMainLoopTask was successfully created!\n");
	}
}
