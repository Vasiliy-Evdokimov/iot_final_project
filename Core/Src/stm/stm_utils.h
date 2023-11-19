/*
 * stm_utils.h
 *
 *  Created on: Nov 19, 2023
 *      Author: Vasilii Evdokimov
 */

#ifndef SRC_STM_STM_UTILS_H_
#define SRC_STM_STM_UTILS_H_

#include <stdio.h>
#include <stdint.h>

int _write(int file, char *ptr, int len);

void printByte(char* prefix, uint8_t value);

void printUartBuffer(char* aPrefix, uint8_t* aArray);

#endif /* SRC_STM_STM_UTILS_H_ */
