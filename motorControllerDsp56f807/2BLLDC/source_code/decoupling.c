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
#define t_coeff 0,6077F

#ifndef USE_NEW_DECOUPLING
/***************************************************************************/
/**
 * this function decouples PWM.
 ***************************************************************************/
void decouple_dutycycle(Int32 *pwm)
{
	float tempf = 0;
	Int32 temp32 = 0;
	static UInt8 count=0;
	byte timeout_cpl_pid = 100;
			
}

/***************************************************************************/
/**
 * this function decouples PWM (new version).
 ***************************************************************************/
/* */
void decouple_dutycycle_new_motor(Int32 *pwm)
{
	float tempf;
	Int32 temp32 = 0;
	static UInt8 count=0;
	byte timeout_cpl_pid = 100;

#ifdef DEBUG_CPL_BOARD
    // Here the pid value are printed BEFORE decoupling 
	if(count==255)
	{
		can_printf("Pid:(%d,%d)", (Int16) pwm[0], (Int16) pwm[1]);
		count=0;
	}			
	count++;
#endif

#if VERSION == 0x0150
	// ----- JOINT 0 -----	
	if (_control_mode[0] == MODE_POSITION)
	{
		// do nothing, alread decoupled
	}
	else if (_control_mode[0] == MODE_TORQUE ||
	    _control_mode[0] == MODE_IMPEDANCE_POS ||
	    _control_mode[0] == MODE_IMPEDANCE_VEL )
	{	
		_pd[0] = (_pd[0] + _pd[1]);
		pwm[0] = (pwm[0] + pwm[1]);	
	}	
	
	// ----- JOINT 1 -----
	if (_control_mode[1] == MODE_POSITION)
	{	
		tempf = ((float)(_pd[1] - _pd[0]));
		tempf = tempf * a_coeff;
		_pd[1] = (Int32) tempf;

		tempf = ((float)(pwm[1] - pwm[0]));
		tempf = tempf * a_coeff;
	 	pwm[1] = (Int32) tempf;				
	}
	else if (_control_mode[1] == MODE_TORQUE ||
	         _control_mode[1] == MODE_IMPEDANCE_POS ||
	         _control_mode[1] == MODE_IMPEDANCE_VEL )
	{
   		tempf = (float)(_pd[1]);
		tempf = tempf * b_coeff;
		tempf = tempf - (float)(_cpl_pid_prediction[0]);
	    temp32 = (Int32) (tempf);
	    _pd[1] = temp32;
	    
		tempf = (float)(pwm[1]);
		tempf = tempf * b_coeff;
		tempf = tempf - (float)(_cpl_pid_prediction[0]);
		temp32 = (Int32) (tempf);
	    pwm[1] = temp32;	        
	}

    //update the prediction for coupled board duty
    _cpl_pid_prediction[0] = _cpl_pid_prediction[0] + _cpl_pid_delta[0];
    _cpl_pid_prediction[1] = _cpl_pid_prediction[1] + _cpl_pid_delta[1];		

	// watchdog on coupling broadcast: if no message is received, turn off the controller	
	_cpl_pid_counter++;
	if (_cpl_pid_counter >= timeout_cpl_pid)
	{
		_control_mode[1] = MODE_HW_FAULT;	
		_pad_enabled[1] = false;
		PWM_outputPadDisable(0);

		#ifdef DEBUG_CAN_MSG
			if(count==255)
			{
				can_printf("No cpl pid info");
				count=0;
			}			
			count++;				
		#endif			
	}
	
#elif VERSION == 0x0152
	//  Waist Differential coupling 
	//	|Me1| |  1    -1 |  |Je1|
	//	|Me2|=|  1     1 |* |Je2|    
	
	temp32 	     = pwm[0];
	pwm[0] = (pwm[0] - pwm[1])>>1;
	pwm[1] = (temp32         + pwm[1])>>1;	
				
	if (mode_is_idle(0) ||mode_is_idle(1))
	{
		pwm[0] = 0;
		pwm[1] = 0;
	}
	temp32   = _pd[0];
	_pd[0] = (_pd[0] - _pd[1])>>1;
	_pd[1] = (temp32   + _pd[1])>>1;
		
#elif VERSION == 0x0153 || VERSION == 0x0157
	// ----- JOINT 0 ONLY -----
	if (_control_mode[0] == MODE_POSITION)
	{
   		tempf = (float)(_pd[0]);
		tempf = tempf * a_coeff;
	    temp32 = (Int32) _cpl_pid_prediction[1] + (Int32) (tempf);
	    _pd[0] = temp32;
	    
		tempf = (float)(pwm[0]);
		tempf = tempf * a_coeff;
		temp32 = (Int32) _cpl_pid_prediction[1] + (Int32) (tempf);
	    pwm[0] = temp32;	    
	}
	else if (_control_mode[0] == MODE_TORQUE ||
	 		 _control_mode[0] == MODE_IMPEDANCE_POS ||
	 		 _control_mode[0] == MODE_IMPEDANCE_VEL)
	{		
		tempf = (float)(_pd[0]);
		tempf = tempf * b_coeff;
		_pd[0] = (Int32) tempf;
		
		tempf = (float)(pwm[0]);
		tempf = tempf * b_coeff;
		pwm[0] = (Int32) tempf;					
	}

	//update the prediction for coupled board duty
	_cpl_pid_prediction[0] = _cpl_pid_prediction[0] + _cpl_pid_delta[0];
	_cpl_pid_prediction[1] = _cpl_pid_prediction[1] + _cpl_pid_delta[1];
	    
	// watchdog on coupling broadcast: if no message is received, turn off the controller	
	_cpl_pid_counter++;
	if (_cpl_pid_counter >= timeout_cpl_pid)
	{
		_control_mode[0] = MODE_HW_FAULT;	
		_pad_enabled[0] = false;
		PWM_outputPadDisable(0);

		#ifdef DEBUG_CAN_MSG
			if(count==255)
			{
				can_printf("No cpl pid info");
				count=0;
			}			
			count++;				
		#endif			
	}
#endif

#ifdef DEBUG_CPL_BOARD
    // Here the pid value are printed AFTER decoupling 
	if(count==255)
	{
		can_printf("cplPid:(%d,%d)", (Int16) pwm[0], (Int16) pwm[1]);
		count=0;
	}			
	count++;
#endif		
}
*/
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
