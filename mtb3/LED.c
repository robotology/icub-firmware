#include<p30f4011.h>
#include "LED.h"
	#include<uart.h>
extern void Wait(unsigned int value);

void LED_Init()
{

	U2MODE=0;
	TRISFbits.TRISF5=0;
	TRISFbits.TRISF5=1;
	TRISEbits.TRISE5=1;
	TRISEbits.TRISE4=1;
	TRISEbits.TRISE3=1;
	TRISEbits.TRISE2=1;
	TRISEbits.TRISE1=1;
	TRISEbits.TRISE0=1;
	led0=1;	
}

void LED_test()
{
	led0=0;
	Wait(100);
	led0=1;
	Wait(100);
}
