/*
 * dht11.c
 *
 *  Created on: 11 дек. 2014 г.
 *      Author: Vadim
 */


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "driverlib/timer.h"
#include "dht11.h"




long WaitUntilPinState(int clockMhz, long pinbase, long pin, long timerbase, long timer, int previousstate, int usec)
{
// Debounce really not needed unless you have huge noise, but just in case (the processor is fast enough)
#define DEBOUNCEMATCHES 5
	long timermax;
	long timerval;
	int matches = 0;
	TimerDisable(timerbase, timer);
	TimerConfigure(timerbase, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_ONE_SHOT);
	TimerLoadSet(timerbase, timer, usec*clockMhz);
	timermax = TimerValueGet(timerbase, timer);
	timerval = timermax;
	TimerEnable(timerbase, timer);

	while(matches < DEBOUNCEMATCHES){ /* Wait for DHT to pull down */
		if( GPIOPinRead(pinbase, pin) > 0 != previousstate > 0) {
			matches++;
		} else {
			matches = 0;
		}
		timerval = TimerValueGet(timerbase, timer);
		if(!timerval) timerval = 1; /* make sure we never return 0 by mistake */
		if (timerval==timermax){
			return 0;
		}
	}
	return timerval;
}



int ReadDHT(int *temp, int *humidity)
{
	// bit buffers & timeout
	char bitcount;
	char byte;
	char bits[5] = {0,0,0,0,0};
	unsigned int bitints[6];
	long timerval;
	int clockMhz;


	clockMhz = SysCtlClockGet() / 1000000;

	/* Configure hardware counter to check how long we are looping */
	SysCtlPeripheralEnable(MY_TIMER_PERIPH);
	TimerConfigure(MY_TIMERBASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_ONE_SHOT);


	// request reading
	SysCtlPeripheralEnable(MY_PIN_PERIPH);

	GPIOPinTypeGPIOOutput(MY_PIN_PORTBASE, MY_PINNR);
	GPIOPinWrite(MY_PIN_PORTBASE, MY_PINNR, 0x0);

	// #loops = loops_per_us*time_in_us = time in us * loops/us = time_in_us *(clockfreq/10000000)
	SysCtlDelay(30000*clockMhz); // -> 30 ms
	/////// SET PIN HIGH (putting it to input will do)
	GPIOPinTypeGPIOInput(MY_PIN_PORTBASE, MY_PINNR);

	SysCtlDelay(10*clockMhz);

	/* Wait for DHT pull down */
	if(!WaitUntilPinState(clockMhz,MY_PIN_PORTBASE,MY_PINNR,MY_TIMERBASE,MY_TIMER,1,50)) return 0;

	/* Wait for dummy up */
	if(!WaitUntilPinState(clockMhz,MY_PIN_PORTBASE,MY_PINNR,MY_TIMERBASE,MY_TIMER,0,90)) return 0;

	/* Wait for dummy down */
	if(!WaitUntilPinState(clockMhz,MY_PIN_PORTBASE,MY_PINNR,MY_TIMERBASE,MY_TIMER,1,90)) return 0;

	// start receiving 40 bits
	char i;
	bitcount = 7;
	byte = 0;
	for (i=0; i < 40; i++)
	{
		/* Wait for pull up */
		if(!WaitUntilPinState(clockMhz,MY_PIN_PORTBASE,MY_PINNR,MY_TIMERBASE,MY_TIMER,0,60)) return 0;

		/* Wait for pull down and count */
		timerval = WaitUntilPinState(clockMhz,MY_PIN_PORTBASE,MY_PINNR,MY_TIMERBASE,MY_TIMER,1,90);

		if(!timerval) return 0;

		if (timerval < 40*clockMhz) bits[byte] |= (1 << bitcount);
		if (bitcount == 0)
		{
			bitcount = 7;
			byte++;
		}else{
			bitcount--;
		}
	}

	// checksum

	/* Little Endian */
	bitints[0] = ((unsigned int) bits[0]  & (0x000000FF));
	bitints[1] = ((unsigned int) bits[1]  & (0x000000FF));
	bitints[2] = ((unsigned int) bits[2]  & (0x000000FF));
	bitints[3] = ((unsigned int) bits[3]  & (0x000000FF));
	bitints[4] = ((unsigned int) bits[4]  & (0x000000FF));
	if(((bitints[0] + bitints[1] + bitints[2] + bitints[3]) & (0x000000FF)) == bitints[4]){
		*temp = bitints[2];
		*humidity =  bitints[0];
		return 1;
	}else{
		return 0; //Erorr
	}

}
