#include "check_range.h"
#include "controller.h"
#include "pid.h"
#include "trajectory.h"

/***************************************************************************/
/**
 * This function checks if the joint is in range during position/openloop control mode
 ***************************************************************************/
void check_range(byte i, Int16 band, Int32 *PWM)
{
	if (_control_mode[i] == MODE_POSITION)
	{
 		if  (_position[i] > (_max_position[i]-band) ||  (_position[i] < (_min_position[i]+band)))   
		{			
			_ko[i]=0;   //remove the PWM offset if it is out of limits 
			#ifdef DEBUG_CONTROL_MODE
			can_printf("OUT of LIMITS ax:%d", i);	
			#endif
			
			if (band>0)
			{
				if  ((_position[i] > (_max_position[i]-band)) && (_desired[i]>(_max_position[i]-band)))
				{
					_desired[i]=(_max_position[i]-band); 
					_integral[i] = 0; 
					_set_point[i] = _desired[i];
					init_trajectory (i, _desired[i], _desired[i], 1); 
				}
				
				if  ((_position[i] < (_min_position[i]+band)) && (_desired[i]<(_min_position[i]+band)))
				{
					_desired[i]=(_min_position[i]+band); 
					_integral[i] = 0; 
					_set_point[i] = _desired[i];
					init_trajectory (i, _desired[i], _desired[i], 1); 
				}
			}
			else	
			{
				if  ((_position[i] > (_max_position[i]+band)) && (_desired[i]>(_max_position[i]-band)))
				{
					#ifdef DEBUG_CONTROL_MODE
					can_printf("OUT of LIMITS MAX ax:%d", i);	
					#endif
					_desired[i]=_max_position[i]; 
					_integral[i] = 0; 
					_set_point[i] = _desired[i];
					init_trajectory (i, _desired[i], _desired[i], 1); 
				}
				
				if  ((_position[i] < (_min_position[i]-band)) && (_desired[i]<(_min_position[i]+band)))
				{
					#ifdef DEBUG_CONTROL_MODE
					can_printf("OUT of LIMITS MIN ax:%d", i);	
					#endif
					_desired[i]=_min_position[i]; 
					_integral[i] = 0; 
					_set_point[i] = _desired[i];
					init_trajectory (i, _desired[i], _desired[i], 1); 
				}
				
			}
		} 
	}
	else if (_control_mode[i] == MODE_OPENLOOP)
	{
		if  (_position[i] > (_max_position[i]-band) ||  (_position[i] < (_min_position[i]+band)))   
		{			
			_control_mode[i] = MODE_POSITION; //	
			_ko[i]=0;  //remove the PWM offset if it is out of limits
			if (band>0)
			{
				if  ((_position[i] > (_max_position[i]-band)) && (_desired[i]>(_max_position[i]-band)))
				{
					_desired[i]=(_max_position[i]-band); 
					_integral[i] = 0; 
					_set_point[i] = _desired[i];
					init_trajectory (i, _desired[i], _desired[i], 1); 
				}
				
				if  ((_position[i] < (_min_position[i]+band)) && (_desired[i]<(_min_position[i]+band)))
				{
					_desired[i]=(_min_position[i]+band); 
					_integral[i] = 0; 
					_set_point[i] = _desired[i];
					init_trajectory (i, _desired[i], _desired[i], 1); 
				}
			}
			else	
			{
				if  ((_position[i] > (_max_position[i]+band)) && (_desired[i]>(_max_position[i]-band)))
				{
					#ifdef DEBUG_CONTROL_MODE
					can_printf("OUT of LIMITS MAX ax:%d", i);	
					#endif
					_desired[i]=_max_position[i]; 
					_integral[i] = 0; 
					_set_point[i] = _desired[i];
					init_trajectory (i, _desired[i], _desired[i], 1); 
				}
				
				if  ((_position[i] < (_min_position[i]-band)) && (_desired[i]<(_min_position[i]+band)))
				{
					#ifdef DEBUG_CONTROL_MODE
					can_printf("OUT of LIMITS MIN ax:%d", i);	
					#endif
					_desired[i]=_min_position[i]; 
					_integral[i] = 0; 
					_set_point[i] = _desired[i];
					init_trajectory (i, _desired[i], _desired[i], 1); 
				}
			}
			#ifdef DEBUG_CONTROL_MODE
			can_printf("MODE CHANGED TO POSITION, OUT of LIMITS ax:%d", i);	
		    #endif 
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
	  	_control_mode[i] == MODE_IMPEDANCE_POS ||
	  	_control_mode[i] == MODE_IMPEDANCE_VEL)
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
