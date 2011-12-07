#include<p30f4011.h>
#include "timers.h"
#include "timer.h"

extern volatile char flag;
extern unsigned int TIMER_VALUE;
void T1_Init(unsigned int match_value)
{
	//==================================== INIT TIMER ================================
	ConfigIntTimer1(T1_INT_PRIOR_1 & T1_INT_ON);
	WriteTimer1(0);
	OpenTimer1(T1_ON & T1_GATE_OFF & T1_IDLE_STOP & T1_PS_1_64 & T1_SYNC_EXT_OFF & T1_SOURCE_INT, match_value);
	
}

void T2_Init(unsigned int match_value)
{
	//==================================== INIT TIMER ================================
	ConfigIntTimer2(T2_INT_PRIOR_1 & T2_INT_ON);
	WriteTimer2(0);
	OpenTimer2(T2_ON & T2_GATE_OFF & T2_IDLE_STOP & T2_PS_1_64  & T2_SOURCE_INT, match_value);

}
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
    flag=1;
    WriteTimer1(0x0);
//    if (CONFIG_TYPE==CONFIG_SINGLE)    ServiceAD7147Isr(CH0);
//    if (CONFIG_TYPE==CONFIG_ALL)  ServiceAD7147Isr_all(CH0);
  							   //      TMR1  = 0;          /* Reset Timer1 to 0x0000 */
    PR1   = TIMER_VALUE;     /* assigning Period to Timer period register */
    _T1IF = 0;

}
