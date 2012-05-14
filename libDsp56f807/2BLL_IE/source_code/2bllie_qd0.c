/**
 * qd0.c
 * implementation of the quadrature decoder interface.
 *
 */

#include "qd0.h"
#include "2bllie_brushless_comm.h"
#include "phase_hall_sens.h"
#include "qd0.h"

#define NUMBER_OF_TICK_MS   1600 // it about 2x the number of tick/ms if the encoder is 8192 tick/ms and the motor speed is 6000rpm 

word oldTimerValue0;
extern volatile byte COMMUTATION_MODE;

/**
 * sets the init registers of the quadrature decoder.
 * @param Position is the 32bit position value.
 * @return ERR_OK always.
 */
byte QD0_setResetPosition (dword Position)
{
	if (COMMUTATION_MODE==HALL )  
	{
	setReg (QD0_UIR, (word)(Position>>16));
	setReg (QD0_LIR, (word)(Position));
	}
	else
	{
		
	}
	return ERR_OK;
}

void QD0_SetCounter(UInt16 Value)
{
  setReg(TMRA0_CNTR,Value);            /* Store given value to the counter register */
}

void QD0_DisableEvent() 
{
   //disable Interrupt
	/*TMRA0_SCR: TCF=0,TCFIE=1,TOF=0,TOFIE=0,IEF=0,IEFIE=0,IPS=0,INPUT=0,Capture_Mode=0,MSTR=0,EEOF=0,VAL=0,FORCE=0,OPS=0,OEN=0 */
	setReg(TMRA0_SCR,0x0000);
	
}
void QD0_EnableEvent() 
{
   //enable Interrupt
	/*TMRA0_SCR: TCF=0,TCFIE=1,TOF=0,TOFIE=0,IEF=0,IEFIE=0,IPS=0,INPUT=0,Capture_Mode=0,MSTR=0,EEOF=0,VAL=0,FORCE=0,OPS=0,OEN=0 */
	setReg(TMRA0_SCR,0x4000);
	
}
/**
 * sets the encoder position by using the init register method.
 * @param Position is the 32bit position value.
 * @return ERR_OK always.
 */
byte QD0_setPosition (dword Position)
{
    if (COMMUTATION_MODE==HALL )  
	{
		setReg(QD0_UIR, (word)(Position>>16));
		setReg(QD0_LIR, (word)(Position));
		setRegBit(QD0_DECCR, SWIP);
	}
	return ERR_OK;
}

#pragma interrupt called
void QD0_GetCounterValue(word *Value)
{
  *Value = getReg(TMRA0_CNTR);         /* Return counter value */
}

/**
 * gets the encoder position value as a 32 bit integer.
 * @param Position is the pointer to the variable holding the value.
 * @return ERR_OK always.
 */
byte QD0_getPosition (dword *Position)
{
//	word TimerValue=getReg(TMRA0_CNTR);
	
  //	if (COMMUTATION_MODE==HALL )  
	{
		*Position = getReg(QD0_LPOS) | ((dword)getReg(QD0_UPOS)<<16);
	}
/*	else
	{
		if ((TimerValue-oldTimerValue0)>0 && (abs_s(TimerValue-oldTimerValue0)<NUMBER_OF_TICK_MS ))  
		{
		*Position += (TimerValue-oldTimerValue0); 	//CW  without timer overflow 
		}
		if ((TimerValue-oldTimerValue0)<=0 && (abs_s(TimerValue-oldTimerValue0)<NUMBER_OF_TICK_MS ))  
		{
			*Position -= (oldTimerValue0-TimerValue); 	//CCW  without timer overflow
		}
		if ((TimerValue-oldTimerValue0)>0 && (abs_s(TimerValue-oldTimerValue0)>NUMBER_OF_TICK_MS )) 
		{
			*Position -= (65535-TimerValue)+oldTimerValue0;	//CCW  with timer overflow
		}
		if ((TimerValue-oldTimerValue0)<=0 && (abs_s(TimerValue-oldTimerValue0)>NUMBER_OF_TICK_MS )) 
		{
			*Position  += (65535-oldTimerValue0)+TimerValue;	//CW  with timer overflow
		}
		oldTimerValue0=TimerValue;
	}
*/
	
	return ERR_OK;
}

/**
 * initializes the quadrature decoder circuitry.
 */
void QD0_init (void)
{
// 	if (COMMUTATION_MODE==ENCODER)  
//	{
//		 /* TMRA0_CTRL: CM=0,PCS=1,SCS=0,ONCE=0,LENGTH=0,DIR=0,Co_INIT=0,OM=0 */
//	  	setReg(TMRA0_CTRL,0x200);               /* Set up mode */
//		/*TMRA0_SCR: TCF=0,TCFIE=1,TOF=0,TOFIE=0,IEF=0,IEFIE=0,IPS=0,INPUT=0,Capture_Mode=0,MSTR=0,EEOF=0,VAL=0,FORCE=0,OPS=0,OEN=0 */
//		setReg(TMRA0_SCR,0x4000);
//		setReg(TMRA0_CNTR,0);                /* Reset counter register */
//	  	setReg(TMRA0_LOAD,0);                /* Reset load register */
//	  	setReg(TMRA0_CMP1,HALF_COMMUTATION_INTERVAL);           /* Set up compare 1 register */
//	  	setReg(TMRA0_CMP2,-HALF_COMMUTATION_INTERVAL-1);     /* Set up compare 2 register */
//
	//	clrRegBits (TMRA0_CTRL, 0xe000);
//		setRegBits (TMRA0_CTRL, 0x8000);	   /* counter on in quadrature mode! */
    
//    	setReg (QD0_FIR, 10);
//		setRegBits (QD0_DECCR, 0);

//	}
//	else
	{	
		setReg (QD0_FIR, 10);
		setRegBits (QD0_DECCR, 0);
	
	}  
   
}


#pragma interrupt saveall                     //it runs only if COMMUTATION_METHOD is ENCODER 
void QD0_Interrupt(void)
{
 //  QD0_OnCompare();              
 clrRegBit (TMRA0_SCR, TCF);            /* Reset interrupt request flag */      

}



/**
 * intiializes the position by setting the SWIP bit.
 * @return ERR_OK always.
 */
byte QD0_ResetPosition (void)
{
	if (COMMUTATION_MODE==HALL)
	{
		setRegBit (QD0_DECCR, SWIP);
	}
	else setReg(TMRA0_CNTR,0);                /* Reset counter register */
	return ERR_OK;
}

/**
 * gets the signals from the quadrature decoder channels.
 * @param Filtered, if TRUE gets the filtered signals, otherwise the raw ones.
 * @param Signals is the pointer to an array of raw/filtered signals containing 
 * home, index, B, and A.
 */
byte QD0_getSignals (bool Filtered, word *Signals)
{
	if (Filtered)
		*Signals = getReg(QD0_IMR) >> 4;
	else
		*Signals = getReg(QD0_IMR) & 15;
	return ERR_OK;
}
