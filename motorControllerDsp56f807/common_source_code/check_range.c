#include "check_range.h"
#include "control_enable.h"
#include "controller.h"
#include "pid.h"
#include "trajectory.h"
#include "can1.h"

//#define DEBUG_CHECK_RANGE
static UInt8 count=0;

/***************************************************************************/
/**
 * This function checks if the joint is in range during openloop control mode
 ***************************************************************************/
void check_range(byte i, Int16 band, Int32 *PWM)
{
	
	if (band<0) 
	{
		#ifdef DEBUG_CHECK_RANGE
		can_printf("BAND SIGN j:%d", i);	
		#endif
		band=0; //the band must be positive
	}
	
	if (_control_mode[i] == MODE_OPENLOOP && _ko_openloop[i]!=0)
	{		
 		if  (_position[i] > (_max_position[i]-band))   
		{	
			can_printf("WARN: OPENLOOP out limits on j:%d",i);
			_ko_openloop[i]=0;   
		} 
		else
		if  (_position[i] < (_min_position[i]+band))
		{
			can_printf("WARN: OPENLOOP out limits on j:%d",i);
			_ko_openloop[i]=0;   					
		}
	}
}


/***************************************************************************/
/**
 * This function checks if the joint is in range during torque control mode
 ***************************************************************************/
void check_range_torque(byte i, Int16 band, Int32 *PWM)
{
	static UInt32 TrqLimitCount =0;
 	char   signKp = (_kp[i]>0?1:-1);
 	char   signPWM = (PWM[i]>0?1:-1);
 	int    PWMb   = PWM[i];
 	Int32  PWMc   = 0;
 	if (_control_mode[i] == MODE_TORQUE ||
	  	mode_is_impedance_position(i) ||
	  	mode_is_impedance_velocity(i) )
 		{
	 		if  (_position[i] > _max_position[i])
	 		{
	 			if (signKp*signPWM >0 )
				{
					PWMc = ((Int32) (_max_position[i]-_position[i]) * ((Int32)_kp[i]));
					if (PWMc>=0) {PWMc = PWMc >> _kr[i];} 
					else 	 	 {PWMc = -(-PWMc >> _kr[i]);}
					PWM[i] = PWMc;	
					_integral[i] = 0;
				}
				TrqLimitCount++;	 
				if (TrqLimitCount>=200)
				{
					#ifdef DEBUG_CONTROL_MODE
						can_printf("TORQUE LIMITS kp:%d B:%d A:%f", signKp, PWMb, PWM[i]);	
						TrqLimitCount=0;
				    #endif 
				}			
	 		}
	 		if  (_position[i] < _min_position[i])   
	 		{
	 			if (signKp*signPWM <0 )
				{
					PWMc = ((Int32) (_min_position[i]-_position[i]) * ((Int32)_kp[i]));
					if (PWMc>=0) {PWMc = PWMc >> _kr[i];} 
					else 	 	 {PWMc = -(-PWMc >> _kr[i]);}
					PWM[i] = PWMc;		
					_integral[i] = 0;	
				}				
				TrqLimitCount++;
				if (TrqLimitCount>=200)
				{
					#ifdef DEBUG_CONTROL_MODE
						can_printf("TORQUE LIMITS kp:%d B:%d A:%f", signKp, PWMb, PWM[i]);	
						TrqLimitCount=0;
				    #endif 
				}
	 		} 				
 		}
}

/***************************************************************** 
 * Sets back the desired position to a legal one
 * in case joint limits have been exceeded.
 *****************************************************************/
void check_desired_within_limits(byte i, Int32 previous_desired)
{

#if (CURRENT_BOARD_TYPE  == BOARD_TYPE_4DC)
	Int32 _min_position_coupled = 0;
 	Int32 _max_position_coupled = 0;
 	
 	
 	
#if VERSION == 0x0113
float tmp;
	if (i == 0 && _control_mode[i]!=MODE_CALIB_ABS_POS_SENS)
	{		
		tmp = (((float) _adjustment[0])*0.2683);
		_min_position_coupled = _min_position[i] + (Int32) tmp; 
		if (_desired[i] < _min_position_coupled && (_desired[i] - previous_desired) <= 0) 
		{
			_desired[i] = _min_position_coupled;
			if (_control_mode[i] == MODE_VELOCITY ||
			    _control_mode[i] == MODE_MIXED)
				_set_vel[i] = 0;
#ifdef DEBUG_CONTROL_MODE
			can_printf("WARN: OUT of MIN LIMITS");	
#endif
		}
		
		_max_position_coupled = _max_position[i] + (Int32) tmp; 
		if (_desired[i] > _max_position_coupled && (_desired[i] - previous_desired) >= 0)
		{
			_desired[i] = _max_position_coupled;
			if (_control_mode[i] == MODE_VELOCITY ||
			    _control_mode[i] == MODE_MIXED)
				_set_vel[i] = 0;
#ifdef DEBUG_CONTROL_MODE
			can_printf("WARN: OUT of MAX LIMITS");	
#endif
		}
	}
#endif
	if (_control_mode[i]!=MODE_CALIB_ABS_POS_SENS && _control_mode[i]!=MODE_CALIB_HARD_STOPS)
	{
		if (_desired[i] < _min_position[i] && (_desired[i] - previous_desired) < 0) 
		{
			//_desired[i] = _min_position[i];
			if (_control_mode[i] == MODE_POSITION)
			{
				_set_point[i] = _min_position[i];
				init_trajectory (i, _desired[i], _set_point[i], _set_vel[i]);
			}
			else
			{
				if (_control_mode[i] == MODE_VELOCITY ||
			        _control_mode[i] == MODE_MIXED)
					_set_vel[i] = 0;
				else
					_desired[i] = _min_position[i];
			}
		}
		if (_desired[i] > _max_position[i] && (_desired[i] - previous_desired) > 0)
		{
			//_desired[i] = _max_position[i];
			if (_control_mode[i] == MODE_POSITION)
			{
				_set_point[i] = _max_position[i];
				init_trajectory (i, _desired[i], _set_point[i], _set_vel[i]);
			}			
			else 
			{
				if (_control_mode[i] == MODE_VELOCITY ||
			        _control_mode[i] == MODE_MIXED)
					_set_vel[i] = 0;
				else
					_desired[i] = _max_position[i];
			}
		}
	}
#else //(CURRENT_BOARD_TYPE  == BOARD_TYPE_4DC)
	if (_control_mode[i]!=MODE_CALIB_ABS_POS_SENS)
	{
		if (_desired[i] < _min_position[i] && (_desired[i] - previous_desired) < 0) 
		{
			_desired[i] = _min_position[i];
			if (_control_mode[i] == MODE_VELOCITY  ||
			    _control_mode[i] == MODE_MIXED)
				_set_vel[i] = 0;
#ifdef DEBUG_CONTROL_MODE
			can_printf("WARN: OUT of MIN LIMITS");	
#endif
		}
		if (_desired[i] > _max_position[i] && (_desired[i] - previous_desired) > 0)
		{
			_desired[i] = _max_position[i];
			if (_control_mode[i] == MODE_VELOCITY  ||
			    _control_mode[i] == MODE_MIXED)
				_set_vel[i] = 0;
#ifdef DEBUG_CONTROL_MODE
			can_printf("WARN: OUT of MAX LIMITS");	
#endif
		}
	}
#endif //(CURRENT_BOARD_TYPE  == BOARD_TYPE_4DC)
}
