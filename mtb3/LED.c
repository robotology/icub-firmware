#include<p30f4011.h>
#include "LED.h"
	#include<uart.h>
extern void Wait(unsigned int value);

void LED_Init()
{

	U2MODE=0;
	TRISFbits.TRISF5=0;
        TRISEbits.TRISE8=0;
	TRISEbits.TRISE5=0;
	TRISEbits.TRISE4=0;
	TRISEbits.TRISE3=0;
	TRISEbits.TRISE2=0;
	TRISEbits.TRISE1=0;
	TRISEbits.TRISE0=0;
        PORTE=0;
	led0=1;	
}

void LED_test()
{
	led0=0;
	Wait(100);
	led0=1;
	Wait(100);
}
