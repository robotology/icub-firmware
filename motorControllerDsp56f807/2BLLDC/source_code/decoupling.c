#include "decoupling.h"
#include "abs_ssi_interface.h"
#include "pwm_interface.h"
#include "pid.h"
#include "can1.h" 
#include "control_enable.h"

/***************************************************************************/
/**
 * this function decouples encoder readings.
 ***************************************************************************/
void decouple_positions(void)
{
	byte timeout_cpl_pos = 100;
#ifdef DEBUG_CAN_MSG
	static UInt8 count=0;
#endif			
	
}

#define a_coeff 1.6455F
#define b_coeff 1.6455F
#define t_coeff 0.6077F

#ifndef USE_NEW_DECOUPLING

#error USE_NEW_DECOUPLING MUST BE TRUE

#else  //ifndef USE_NEW_DECOUPLING

/***************************************************************************/
/**
 * this function decouples PWM (new version joint version).
 ***************************************************************************/
 
void decouple_dutycycle_new_joint(Int32 *pwm)
{
	float tempf = 0;
	Int32 temp32 = 0;
	static UInt8 count=0;
	byte timeout_cpl_pid = 100;
	
	Int32 pd_out [2]  = {0,0};
	Int32 pwm_out [2] = {0,0};
	   
	pd_out[0]=_pd[0];
	pd_out[1]=_pd[1];
	pwm_out[0]=pwm[0];
	pwm_out[1]=pwm[1];


#if VERSION == 0x0162
	/*  Neck Differential coupling 
		|Me1| |  1    -1 |  |Je1|
		|Me2|=|  1     1 |* |Je2|    */
	
	pwm_out[0] = (-pwm[0] + pwm[1])>>1;
	pwm_out[1] = ( pwm[0] + pwm[1])>>1;	

	pd_out[0] = (_pd[0] - _pd[1])>>1;
	pd_out[1] = (_pd[0] + _pd[1])>>1;
					
	if (mode_is_idle(0) || mode_is_idle(1))
	{
		pwm_out[0] = 0;
		pwm_out[1] = 0;
	}

#endif

#ifdef DEBUG_CPL_BOARD
	if(count==255)
	{
		// before decoupling
		can_printf("Pid:(%d,%d)", (Int16) pwm[0], (Int16) pwm[1]);
		// after decoupling
		can_printf("cplPid:(%d,%d)", (Int16) pwm_out[0], (Int16) pwm_out[1]);
		count=0;
	}			
	count++;
#endif	

	//*** WRITE THE OUTPUT ***
	pwm[0]=pwm_out[0];
	pwm[1]=pwm_out[1];
	_pd[0]=pd_out[0];
	_pd[1]=pd_out[1];
		
}

#endif //USE_NEW_DECOUPLING
