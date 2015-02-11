/*
 * uart_lib.c
 *
 *  Created on: 18 дек. 2014 г.
 *      Author: Vadim
 */

#include "uart_lib.h"

#include "inc/tm4c123gh6pm.h"

#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"


#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"


void uart_printf(char *p)
{
	while(*p)
		UARTCharPut(UART0_BASE,*(p++));

}

void uart_print_num(int i)
{
	if (i < 0) {
		UARTCharPut(UART0_BASE,'0');
		i *= -1;
	} else if (i == 0) {
		UARTCharPut(UART0_BASE,'+');
		UARTCharPut(UART0_BASE,'0');
	} else
		UARTCharPut(UART0_BASE,'+');
	int count = 0;
	unsigned char b[3];
	b[0]=b[1]=b[2]=0;
	while (i) {
		b[count++] = i % 10;
		i /= 10;
	}
	for (count = 2; count >= 0; count--)
		UARTCharPut(UART0_BASE,(b[count] + '0'));
}

void uart_init(int baund)
{

  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

        GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);

	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTClockSourceSet(UART0_BASE,UART_CLOCK_PIOSC );

	UARTConfigSetExpClk(UART0_BASE, 16000000, baund,
						(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
						 UART_CONFIG_PAR_NONE));
	IntEnable(INT_UART0);
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);


}
