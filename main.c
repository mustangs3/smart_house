
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "inc/tm4c123gh6pm.h"
#include "i2c_lib.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/rom.h"
#include "dht11.h"

#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "TFT.h"
#include "Nokia5110.h"
#include "uart_lib.h"
int temp,humidity;


void systick_headler(void)
{
	static int ar=1;


	switch(ar)
	{
	case 1 :{uart_printf("vadim");break;}
	case 2: {uart_printf("eu");break;}
	default: ar=0;

	}
	 ar++;


	}

void uart_headler(void)
{
	 unsigned char a;
	    a=(unsigned char) UARTCharGet(UART0_BASE);
	    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,a);
	 GPIO_PORTF_DATA_R=a;
	 uart_printf("ok");
}
void system_tick(uint32_t value)
{
	 NVIC_ST_CTRL_R = 0; //set interrupt from systick
	 NVIC_ST_RELOAD_R= value ;
	 NVIC_ST_CURRENT_R = 0;
	 NVIC_ST_CTRL_R = 3;
}



int main(void)
{

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);	//Configure the system clock to 40MHz
    uart_init(9600);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    	  GPIO_PORTF_DIR_R = 0b11111111;
    	  GPIO_PORTF_DEN_R = 0b11111111;

        while(1);





}
