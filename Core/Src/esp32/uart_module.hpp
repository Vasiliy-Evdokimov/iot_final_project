#ifndef UART_MODULE_H
#define UART_MODULE_H

#include "stdint.h"

#define UART_BAUDRATE 9600
#define RXD2 16
#define TXD2 17

extern uint8_t tx[];
extern uint8_t rx[];

void uart_init();

void uart_transmit();

void print_buffer(uint8_t* aBuf, String aPrefix);

void uart_complete_status();

void uart_handle();

void con_print(String s);

void con_println(String s);

#endif