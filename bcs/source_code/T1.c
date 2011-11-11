#include<p30f4011.h>
#include "T1.h"
#include "timer.h"

/* Set SLEEP to 1 is dsPIC will enter sleep in main loop */
#define SLEEP   1
/*---------------------------------------------------------------------
  Function Name: T1_Init
  Description:   Initialize Timer1 for 1 second intervals
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void T1_Init(unsigned int match_value)
{
	/* declare temp variable for CPU IPL storing */
	int current_cpu_ipl;
	
	/* ensure Timer 1 is in reset state */
	T1CON = 0;
	
	/* reset Timer 1 interrupt flag */
		IFS0bits.T1IF = 0;
		
		/* set Timer1 interrupt priority level to 4 */
	IPC0bits.T1IP = 4;
	
	/* enable Timer 1 interrupt */
		IEC0bits.T1IE = 1;
		  	
	/* set Timer 1 period register */
	PR1 = match_value;
		
	/* select postscaler */
	T1CONbits.TCKPS=1;
		/* select external timer clock */
	T1CONbits.TCS = 1;
	
	/* disable interrupts for unlock sequence below */
	SET_AND_SAVE_CPU_IPL(current_cpu_ipl, 7);	
	
	char a, b, c, *p;
	
	a = 2;
	b = 0x46;
	c = 0x57;
	p = (char *)&OSCCON; 
	
	/* enable 32KHz Oscillator here 
	      low byte unlock sequence and enable LP Oscillator */
		asm volatile ("mov.b %1,[%0] \n"
	                 "mov.b %2,[%0] \n"
	                 "mov.b %3,[%0] \n" : /* no outputs */ : "r"(p), "r"(b), "r"(c),
	                                                         "r"(a));
	
	/* restore CPU IPL value after executing unlock sequence */
	RESTORE_CPU_IPL(current_cpu_ipl);
		 
		/* enable Timer 1 and start the count */ 
	T1CONbits.TON = 1;
	
	
	
	#if SLEEP
	   /* If main loop will enter SLEEP mode then wait here 
	      until Oscillator starts and Timer begins to count */
	while ( TMR1 < 2 );	
	#endif	
}
void T2_Init(unsigned int match_value)
{

	//==================================== INIT TIMER ================================
	ConfigIntTimer2(T2_INT_PRIOR_1 & T2_INT_ON);
	WriteTimer2(0);
	OpenTimer2(T2_ON & T2_GATE_OFF & T2_IDLE_STOP & T2_PS_1_64 & T2_SOURCE_INT, match_value);
	
}
void T3_Init(unsigned int match_value)
{

	//==================================== INIT TIMER ================================
	ConfigIntTimer3(T3_INT_PRIOR_1 & T3_INT_ON);
	WriteTimer3(0);
	OpenTimer3(T3_ON & T3_GATE_OFF & T3_IDLE_STOP & T3_PS_1_64 & T3_SOURCE_INT, match_value);
	
}
