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
	ConfigIntTimer2(T2_INT_PRIOR_5 & T2_INT_ON);
	WriteTimer2(0);
	OpenTimer2(T2_ON & T2_GATE_OFF & T2_IDLE_STOP & T2_PS_1_64  & T2_SOURCE_INT, match_value);

}

