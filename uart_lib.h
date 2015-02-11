/*
 * uart_lib.h
 *
 *  Created on: 18 дек. 2014 г.
 *      Author: Vadim
 */


#ifndef _uart_lib_H_
#define _uart_lib_H_

#include <stdint.h>
#include <stdbool.h>





void uart_printf(char *p);


void uart_print_num(int i);


void uart_init(int baund);


#endif
