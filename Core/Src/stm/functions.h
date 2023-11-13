/*
 * functions.h
 *
 *  Created on: Nov 14, 2023
 *      Author: Vasilii Evdokimov
 */

#ifndef SRC_STM_FUNCTIONS_H_
#define SRC_STM_FUNCTIONS_H_

#include "stm32f411xe.h"

void init();

void mainLoop();

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif /* SRC_STM_FUNCTIONS_H_ */
