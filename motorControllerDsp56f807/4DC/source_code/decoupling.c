#include "decoupling.h"
#include "abs_ssi_interface.h"
#include "pwm_interface.h"
#include "pid.h"
#include "can1.h" 
#include "control_enable.h"

/***************************************************************************/
/**
 * this function decouple encoder readings.
 ***************************************************************************/
void decouple_positions(void)
{				
#if VERSION == 0x0115
		//_position[0] = _position[0] - _position[1];
		//_position[1] = _position[0] + 2*_position[1];
		//vergence / version inverted
		_position[0] = _position[0] + _position[1];
		_position[1] = _position[0] - 2*_position[1];
#elif VERSION == 0x0215

		_position[2] = _position[2] + _position[3];
		_position[3] = _position[2] - 2*_position[3];	
			
#elif VERSION == 0x0119
		//_position[1] = _position[1];		//omitted
		_position[2] = _position[1] + _position[2];		
#elif VERSION == 0x0219
		//_position[1] = _position[1];		//omitted
		//_position[2] = _position[2];		//omitted
#endif
}


/***************************************************************************/
/**
 * this function decouples PWM.
 ***************************************************************************/
void decouple_dutycycle(Int32 *pwm)
{
	float tempf = 0;
	Int32 temp32 = 0;
	static UInt8 count=0;

#if VERSION == 0x0115 
		if ( ! ((_control_mode[0] == MODE_CALIB_HARD_STOPS ) ||
			    (_control_mode[1] == MODE_CALIB_HARD_STOPS ) ) )
		{
			/*
			pwm[0] = (pwm[0] + pwm[1]) >> 1;
			pwm[1] = pwm[0] - pwm[1];	
			pwm[1] = -pwm[1];
			*/
			temp32 	 = pwm[0];
			pwm[0] = (pwm[0] + pwm[1]) >> 1;
			pwm[1] = (temp32    - pwm[1]) >> 1;	
			
			if (mode_is_idle(0) || mode_is_idle(1))
			{
				pwm[0] = 0;
				pwm[1] = 0;
			}
			/*
			_pd[0] = (_pd[0] + _pd[1]) >> 1;
			_pd[1] = _pd[0] - _pd[1];	
			_pd[1] = -_pd[1];
			*/
			temp32 = _pd[0];
			_pd[0] = (_pd[0] 	+ _pd[1]) >> 1;
			_pd[1] = (temp32 - _pd[1]) >> 1;		
		}		

		if (_control_mode[0] == MODE_CALIB_HARD_STOPS) pwm[1] = 0;
		if (_control_mode[1] == MODE_CALIB_HARD_STOPS) pwm[0] = 0;	
#endif
//-----------------------------------------------------------------------------------
#if VERSION == 0x0215 

		if ( ! ((_control_mode[2] == MODE_CALIB_HARD_STOPS ) ||
			    (_control_mode[3] == MODE_CALIB_HARD_STOPS ) ) )
		{
		
			temp32 	 = pwm[2];
			pwm[2] = (pwm[2] + pwm[3]) >> 1;
			pwm[3] = (temp32    - pwm[3]) >> 1;			
			if (mode_is_idle(2) || mode_is_idle(3))
			{
				pwm[2] = 0;
				pwm[3] = 0;
			}	
			temp32 = _pd[2];
			_pd[2] = (_pd[2] 	+ _pd[3]) >> 1;
			_pd[3] = (temp32 - _pd[3]) >> 1;		
		}		

		if (_control_mode[2] == MODE_CALIB_HARD_STOPS) pwm[3] = 0;
		if (_control_mode[3] == MODE_CALIB_HARD_STOPS) pwm[2] = 0;	
#endif
//-----------------------------------------------------------------------------------
#if VERSION == 0x0119 || VERSION == 0x0219 
			//pwm[1] = pwm[1];	//omitted
			pwm[2] = (pwm[2] - pwm[1]);
			if (mode_is_idle(1) || mode_is_idle(2))
			{
				pwm[1] = 0;
	 			pwm[2] = 0;
			}
			//_pd[1] = _pd[1];				//omitted
			_pd[2] = _pd[2] - _pd[1];
			
			if ((_control_mode[1] == MODE_CALIB_HARD_STOPS) || (_control_mode[2] == MODE_CALIB_HARD_STOPS))
			{
				if 	(pwm[1]> _pwm_calibration[1])
				  	pwm[1] = _pwm_calibration[1];
				if 	(pwm[1] < -_pwm_calibration[1])	
					pwm[1] = -_pwm_calibration[1];
				if 	(pwm[2]> _pwm_calibration[2])
				  	pwm[2] = _pwm_calibration[2];
				if 	(pwm[2] < -_pwm_calibration[2])	
					pwm[2] = -_pwm_calibration[2];		
			}	
#endif 
//-----------------------------------------------------------------------------------
#if   VERSION == 0x0228
	//decoupling of the thumb abduction vs thumb proximal
	
	if (_kp[0] > 0)
	{
		//right hand
		//pwm[0] = pwm[0];
   		pwm[1] = pwm[1] + pwm[0];
    	//pwm[2] = pwm[2];
		//_pd[0] = _pd[0];
    	_pd[1] = _pd[1] + _pd[0];
    	//_pd[2] = _pd[2];	
	}
	else
	{
		//left hand
		//pwm[0] = pwm[0];
	    pwm[1] = pwm[1] - pwm[0];
	    //pwm[2] = pwm[2];
		//_pd[0] = _pd[0];
	    _pd[1] = _pd[1] - _pd[0];
	    //_pd[2] = _pd[2];		
	}
#endif
//-----------------------------------------------------------------------------------
#if   VERSION == 0x0230
	//index dist (0), middle prox (1) and dist (2), ring and pinky (3)
	
	//This is currently disabled because it is not so important (better to save cpu time).
	//tempf= pwm[1];
	//tempf= tempf * 0.75;
	//pwm[2] = pwm[2] + tempf;
#endif
}



/***************************************************************************/
/**
 * the followings are taken from the BLL source and shown here just for reference
 ***************************************************************************/
/*
#if VERSION == 0x0150

	// Version 0x0150 relizes the shoulder coupling (here '_c' denotes 
	// the coupled board variables).The applied coupling is the following:
	//
	// 			[    Jm1,      0,      0]
	// tau_m = 	[ -Jm2/a,  Jm2/a,      0] tau_j
	// 			[ -Jm3/a,  Jm3/a,  Jm3/a]
	//
	// 			[    R1/K1,      0,      0]
	// u_m = 	[        0,  R2/K2,      0] tau_m
	// 			[        0,      0,  R3/K3]
	//
	//			[    R1/K1*Jm1,            0,            0]                    			[ 1         0         0     ]
	// u_m =	[ -R2/K2*Jm2/a,  R2/K2*Jm2/a,            0] tau_j =  1.0e-03 * 0.1519 * [-1.6455    1.6455    0     ]
	//			[ -R3/K3*Jm3/a,  R3/K3*Jm3/a,  R3/K3*Jm3/a]                    			[-1.6455    1.6455    1.6455]
	//
	// where:
	// tau_m 	: torques applied by the motors (tau_m_c[0], tau_m_c[1], tau_m[0])
	// tau_j 	: virtual torque at the joints  (tau_j_c[0], tau_j_c[1], tau_j[0])
	// R		: motor resitance (mean of the three phases) R1=0.8967 VS R2=R3=0.8363
	// K		: motor constant torque K1=0.0500  K2=K3=0.0280
	// Jm		: inertia of the motors Jm1=8.47E-06 Jm2=Jm3=5.15E-06 
	// 
	// This solution follows from the following coupling:
	//		[  1,  0,  0]
	// qj =	[  1,  a,  0] qm = Tjm qm
	//		[  0, -a,  a]
	// where:
	//
	// qm 	: position of the motors (qm_c[0], qm_c[1], qm[0])
	// qj 	: position of the motors (qj_c[0], qj_c[1], qj[0])
	// 
	// and from the assumption that dynamics of motors are not
	// effected by the kinematic coupling (due to high gear boxes):
	//
	//	[  Jm1,    0,    0]
	// 	[    0,  Jm2,    0] d2qm = tau_m
	//	[    0,    0,  Jm3]
	//
	// where:
	//
	// Jm 	: motor inertia
	// tau_m: motor torque 
	//

	
	#ifdef DEBUG_CPL_BOARD
	if(count==255)
	{
		can_printf("Pid:(%d,%d)", (Int16) pwm[0], (Int16) pwm[1]);
		count=0;
	}			
	count++;
	#endif	

	tempf = ((float)(_pd[1] - _pd[0]));
	tempf = tempf * a_coeff;
	_pd[1] = (Int32) tempf;

	tempf = ((float)(pwm[1] - pwm[0]));
	tempf = tempf * a_coeff;
	pwm[1] = (Int32) tempf;
			

	#ifdef DEBUG_CPL_BOARD
	if(count==255)
	{
		can_printf("cplPid:(%d,%d)", (Int16) pwm[0], (Int16) pwm[1]);
		count=0;
	}			
	count++;
	#endif	
#endif				

#if VERSION == 0x0152
	//  Waist Differential coupling 
	//	|Me1| |  1    -1 |  |Je1|
	//	|Me2|=|  1     1 |* |Je2|   
	
	temp32 	     = pwm[0];
	pwm[0] = (pwm[0] - pwm[1])>>1;
	pwm[1] = (temp32         + pwm[1])>>1;	
				
	if (mode_is_idle(0) || mode_is_idle(1))
	{
		pwm[0] = 0;
		pwm[1] = 0;
	}
	temp32   = _pd[0];
	_pd[0] = (_pd[0] - _pd[1])>>1;
	_pd[1] = (temp32   + _pd[1])>>1;
#endif			
*/