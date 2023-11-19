/*
 * stm_utils.c
 *
 *  Created on: Nov 19, 2023
 *      Author: Vasilii Evdokimov
 */

#include "main.h"
#include "stm_utils.h"

int _write(int file, char *ptr, int len)
{
	int i;
	for(i = 0; i < len; i++)
		ITM_SendChar((*ptr++));
	return len;
}

void printByte(char* prefix, uint8_t value)
{
	printf("%s = ", prefix);
	for (int j = sizeof(value) * 8 - 1; j >= 0; j--)
		printf("%d", (value >> j) & 1);
	printf("\n");
}

void printUartBuffer(char* aPrefix, uint8_t* aBuffer)
{
	printf("%s:", aPrefix);
	for (int i = 0; i <= aBuffer[1]; i++)
		printf(" [%d]=%d", i, aBuffer[i]);
	printf("\n");
}
