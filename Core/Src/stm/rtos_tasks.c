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

void tasks_init()
{
	char buf[255] = {0};
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
