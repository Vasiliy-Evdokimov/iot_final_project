/*
 * rtos_tasks.h
 *
 *  Created on: Nov 19, 2023
 *      Author: Vasilii Evdokimov
 */

#ifndef SRC_STM_RTOS_TASKS_H_
#define SRC_STM_RTOS_TASKS_H_

#include "main.h"
#include "cmsis_os.h"

void vAddUartTransmitterTask(UartBuffer aUartBuffer);

void tasks_init();

#endif /* SRC_STM_RTOS_TASKS_H_ */
