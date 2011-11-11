#include<p30f4011.h>
#include "PIN.h"

extern void Wait(unsigned int value);

void PINs_Init()
{
	PWMCON1=0;
	PWMCON2=0;
	PTPER=0;
	OVDCON=0;
	PTCON=0;
	
	// the DCDC converter is enabled
	DC_EN=0;
	SD=1;
	
	TRISE=1;
	LED1=0;	
}

void PINs_test()
{
	LED1=1;
	SD=1;
	Wait(30000);
	LED1=0;
	SD=0; 
	Wait(30000);
}
void LED1_test()
{
	LED1=1;
	Wait(30000);
	LED1=0;
	Wait(30000);
}

void LED1_toggle()
{
	LED1=~LED1;
}

	