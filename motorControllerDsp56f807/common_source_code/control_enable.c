#include "control_enable.h"
#include "pid.h"
#include "phase_hall_sens.h"
#include "brushless_comm.h"
#include "can1.h"
#include "pwm_interface.h"
#include "can_interface.h"
#include "trajectory.h"
#include "filters.h"
#include "messages.h"

extern canmsg_t _canmsg;
extern  byte	_board_ID ;
#define CAN_DATA _canmsg.CAN_data
#define CAN_FRAME_TYPE _canmsg.CAN_frameType
#define CAN_FRAME_FMT _canmsg.CAN_frameFormat
#define CAN_LEN _canmsg.CAN_length
#define CAN_ID _canmsg.CAN_messID
#define CAN_SRC _canmsg.CAN_ID_src

//private functions
void enable_motor_pwm      (byte axis);
void disable_motor_pwm     (byte axis);

void helper_set_control_mode      (byte axis, byte value);	
void helper_enable_single_motor   (byte j);
void helper_enable_coupled_motor  (byte j1, byte j2);
void helper_disable_single_motor  (byte j);
void helper_disable_coupled_motor (byte j1, byte j2);

//-----------------------------------------------------------------------------------------------------------------------
byte helper_controlmode_api_to_fw(byte mode)
{
	switch (mode)
	{
		case icubCanProto_controlmode_idle:          return MODE_IDLE;
		case icubCanProto_controlmode_position:      return MODE_POSITION;
		case icubCanProto_controlmode_mixed:         return MODE_MIXED;
		case icubCanProto_controlmode_direct:        return MODE_DIRECT;
		case icubCanProto_controlmode_velocity:      return MODE_VELOCITY;
		case icubCanProto_controlmode_torque:        return MODE_TORQUE;
		case icubCanProto_controlmode_impedance_pos: return MODE_IMPEDANCE_POS;
		case icubCanProto_controlmode_impedance_vel: return MODE_IMPEDANCE_VEL;
        case icubCanProto_controlmode_openloop:      return MODE_OPENLOOP;
		case icubCanProto_controlmode_hwFault:       return MODE_HW_FAULT;
	}
	
	return MODE_UNKNOWN_ERROR;
}

//-----------------------------------------------------------------------------------------------------------------------
byte helper_controlmode_fw_to_api(byte mode)
{
	switch (mode)
	{
		case MODE_IDLE:						 return icubCanProto_controlmode_idle;
		case MODE_POSITION:			   	     return icubCanProto_controlmode_position;
		case MODE_MIXED:			   	     return icubCanProto_controlmode_mixed;
		case MODE_DIRECT:			   	     return icubCanProto_controlmode_direct;
		case MODE_VELOCITY:					 return icubCanProto_controlmode_velocity;
		case MODE_TORQUE:					 return icubCanProto_controlmode_torque;
		case MODE_IMPEDANCE_POS:			 return icubCanProto_controlmode_impedance_pos;
		case MODE_IMPEDANCE_VEL:			 return icubCanProto_controlmode_impedance_vel;
		case MODE_CALIB_ABS_POS_SENS:		 return icubCanProto_controlmode_calibration;
		case MODE_CALIB_HARD_STOPS:      	 return icubCanProto_controlmode_calibration;
		case MODE_HANDLE_HARD_STOPS:		 return icubCanProto_controlmode_calibration;
		case MODE_MARGIN_REACHED:   		 return icubCanProto_controlmode_calibration;
		case MODE_CALIB_ABS_AND_INCREMENTAL: return icubCanProto_controlmode_calibration;
		case MODE_OPENLOOP:                  return icubCanProto_controlmode_openloop;	
		case MODE_HW_FAULT:                  return icubCanProto_controlmode_hwFault;
	}
	
	return icubCanProto_controlmode_unknownError;
}

//=======================================================================================================================
void disable_motor_pwm (byte axis) 
{
#if VERSION == 0x0152 || VERSION == 0x0162 || VERSION==0x0252 
//this is for waist coupling
	{  
		helper_disable_coupled_motor(0,1);
	}
#elif VERSION == 0x0215 || VERSION == 0x0115
//this is for eyes coupling
	{  
		if ((axis==2) || (axis==3)) 
		{ 
			helper_disable_coupled_motor(2,3);
		} 
		else 
		{ 
			helper_disable_single_motor(axis);
		} 
    }
#elif VERSION == 0x0219  || VERSION == 0x0119
//this is for wrist coupling
	{  
		if ((axis==1) || (axis==2)) 
		{ 
			helper_disable_coupled_motor(1,2);
		} 
		else 
		{ 
			helper_disable_single_motor(axis);
		} 
	}
#elif VERSION == 0x0351 
//ikart firmware
	{  
		if (_board_ID==1)  
		{  
			helper_disable_coupled_motor(0,1);
		} 
		else 
		if (_board_ID==2) 
		{ 
			helper_disable_single_motor(0);
		} 
	}
#else 
//standard version
	{ 
		helper_disable_single_motor(axis);
	}
#endif
}

//---------------------------------------------------------------------------------------------------
void helper_disable_coupled_motor(byte j1, byte j2)
{
	if (_control_mode[j1] != MODE_IDLE && 
	    _control_mode[j1] != MODE_HW_FAULT)
	{
		_control_mode[j1] = MODE_IDLE;
	}
	if (_control_mode[j2] != MODE_IDLE && 
	    _control_mode[j2] != MODE_HW_FAULT)
	{
		_control_mode[j2] = MODE_IDLE;
	}
	
	PWM_outputPadDisable(j1);
	PWM_outputPadDisable(j2);	 
	can_printf("PWM ENA COUPLED:%d & %d",j1,j2);  
}

//---------------------------------------------------------------------------------------------------
void helper_disable_single_motor(byte j)
{
	if (_control_mode[j] != MODE_IDLE && 
	    _control_mode[j] != MODE_HW_FAULT)
	{
		_control_mode[j] = MODE_IDLE;
	}
	
	PWM_outputPadDisable(j); 
	can_printf("PWM DIS:%d",j);
}

//---------------------------------------------------------------------------------------------------
void helper_enable_single_motor(byte j)
{
	if (_control_mode[j]== MODE_HW_FAULT) return;
	
	if (_can_protocol_ack == false)
	{
		can_printf("can protocol NOT ack");
		_control_mode[j] = MODE_HW_FAULT;
		return;
	}
	if (!(_received_pid[j].rec_pid==0x7F))
	{
		can_printf("PID IS NOT SET %d", j);
		_control_mode[j] = MODE_HW_FAULT;
		return;
	}
	#if (CURRENT_BOARD_TYPE  != BOARD_TYPE_4DC)
	if (_calibrated[j] == false)
	{
		can_printf("calib failed %d",j);  
		_control_mode[j] = MODE_HW_FAULT;		
	}
	#endif
	PWM_outputPadEnable(j); 
	_control_mode[j] = MODE_POSITION;  
	can_printf("PWM ENA:%d",j);
}

//---------------------------------------------------------------------------------------------------
void helper_enable_coupled_motor(byte j1, byte j2)
{
	if (_control_mode[j1]== MODE_HW_FAULT) return;
	if (_control_mode[j2]== MODE_HW_FAULT) return;
	
	if (_can_protocol_ack == false)
	{
		can_printf("can protocol NOT ack %d & %d", j1, j2);
		_control_mode[j1] = MODE_HW_FAULT;
		_control_mode[j2] = MODE_HW_FAULT;
		return;
	}
	if (!(_received_pid[j1].rec_pid==0x7F) ||
	    !(_received_pid[j2].rec_pid==0x7F))
	{
		can_printf("WARNING:PID IS NOT SET %d & %d", j1, j2);
		_control_mode[j1] = MODE_HW_FAULT;
		_control_mode[j2] = MODE_HW_FAULT; 		
		return;	
	}	
	#if (CURRENT_BOARD_TYPE  != BOARD_TYPE_4DC)
	if (_calibrated[j1] == false ||
	    _calibrated[j2] == false)
	{
		can_printf("calib failed %d & %d",j1,j2);  
		_control_mode[j1] = MODE_HW_FAULT;
		_control_mode[j2] = MODE_HW_FAULT; 		
		return;			
	}
	#endif
	PWM_outputPadEnable(j1);   
	PWM_outputPadEnable(j2);  
	_control_mode[j1] = MODE_POSITION;
	_control_mode[j2] = MODE_POSITION; 	 
	can_printf("PWM ENA COUPLED:%d & %d",j1,j2);  
}

//---------------------------------------------------------------------------------------------------
void enable_motor_pwm (byte axis) 
{		
#if VERSION == 0x0152 || VERSION == 0x0162 || VERSION==0x0252 
//this is for waist coupling
	{   
		helper_enable_coupled_motor(0,1);
	}
#elif VERSION == 0x0215 || VERSION == 0x0115
//this is for eyes coupling
	{   
		if ((axis==2) || (axis==3))  
		{  
			helper_enable_coupled_motor(2,3);
		}  
		else  
		{  
			helper_enable_single_motor(axis);
		}  
	} 
#elif VERSION == 0x0219 || VERSION == 0x0119
//this is for wrist coupling
	{   
		if ((axis==1) || (axis==2))  
		{  
			helper_enable_coupled_motor(1,2);
		}  
		else  
		{  
			helper_enable_single_motor(axis);
		}  
	}
#elif VERSION == 0x0351 
//ikart 
	{   
		if (_board_ID==1)   
		{
			helper_enable_coupled_motor(0,1);		
		}
		else   
		if (_board_ID==2)   
		{   
			helper_enable_single_motor(0);
		}   
	}
#else
// standard firmware
	{
		helper_enable_single_motor(axis);
	}
#endif
}

//---------------------------------------------------------------------------------------------------------------------------
void get_control_mode(byte axis)
{
    byte m=helper_controlmode_fw_to_api(_control_mode[axis]);

	PREPARE_HEADER; 
	CAN_LEN = 3; 
	CAN_DATA[1] = m; 
	CAN_DATA[2] = 0; 
	CAN1_send( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); 
    
}

//---------------------------------------------------------------------------------------------------------------------------
void set_control_mode(byte axis)
{
    byte api_value = CAN_DATA[1];
    byte value = helper_controlmode_api_to_fw(CAN_DATA[1]);

	//special case, from IDLE you can go anywhere, execept IDLE
	if (_control_mode[axis]==MODE_IDLE)
	{
       if (value!=MODE_IDLE &&
           api_value!=icubCanProto_controlmode_forceIdle)	
       {
	        enable_motor_pwm(axis);
			helper_set_control_mode(axis, value);	
       }
       return;
	}

	//special case, from FAULT you can only go to IDLE
	if (_control_mode[axis]==MODE_HW_FAULT)
	{
		if (api_value==icubCanProto_controlmode_forceIdle)	
		{
			disable_motor_pwm(axis);
			helper_set_control_mode(axis, MODE_IDLE);
		}
		return;
	}

	//here current controlmode != IDLE, != FAULT, (motors are on)
	//if you want to turn off motors do this...
	if (value==MODE_IDLE)
	{
		disable_motor_pwm(axis);
		helper_set_control_mode(axis, value);
		return;
	}
	if (api_value==icubCanProto_controlmode_forceIdle)
	{
		disable_motor_pwm(axis);
		helper_set_control_mode(axis, MODE_IDLE);
		return;		
	}
	
	//otherwise just change control mode.
	{
		helper_set_control_mode(axis, value);
	}
}

//---------------------------------------------------------------------------------------------------------------------------
void helper_set_control_mode(byte axis, byte value)
{
	can_printf("CTRLMODE SET:%d",value);
	if (value>=0 && value <=0x50) _control_mode[axis] = value;
	_desired_torque[axis]=0;
	_desired[axis] = _position[axis];
	_desired_vel[axis] = 0;
	_integral[axis] = 0;
	_ko_imp[axis] = 0;
	_set_point[axis] = _position[axis];
	init_trajectory (axis, _position[axis], _position[axis], 1);
	clear_lpf_ord1_3hz  (axis);
	_ko_openloop[axis] = 0;
}

//---------------------------------------------------------------------------------------------------------------------------
void set_interaction_mode(byte axis)
{
	byte value = 0; 
	value = (CAN_DATA[1]);
	if (value!=_interaction_mode[axis])
	{
		_interaction_mode[axis]=value;
	}
}

//---------------------------------------------------------------------------------------------------------------------------
void get_interaction_mode(byte axis)
{
    byte m=_interaction_mode[axis];
    
	PREPARE_HEADER; 
	CAN_LEN = 3; 
	CAN_DATA[1] = m; 
	CAN_DATA[2] = 0; 
	CAN1_send( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA);	
}

//---------------------------------------------------------------------------------------------------------------------------
bool mode_is_idle(byte axis)
{
	if (_control_mode[axis] == MODE_IDLE) return true;
	if (_control_mode[axis] == MODE_HW_FAULT) return true;
	return false;
}

//---------------------------------------------------------------------------------------------------------------------------
bool mode_is_stiff(byte axis)
{
	if (_control_mode[axis] == MODE_IDLE)     return false;
	if (_control_mode[axis] == MODE_HW_FAULT) return false;
	if (_control_mode[axis] == MODE_IMPEDANCE_POS ) return false;
	if (_control_mode[axis] == MODE_IMPEDANCE_VEL ) return false;
	if (_control_mode[axis] == MODE_TORQUE )        return false;
	if (_control_mode[axis] == MODE_OPENLOOP ) return false;
	
	if (_interaction_mode[axis] == icubCanProto_interactionmode_compliant) return false;
	return true;
}

//---------------------------------------------------------------------------------------------------------------------------
bool mode_is_force_controlled(byte axis)
{
	if (_control_mode[axis] == MODE_IDLE)     return false;
	if (_control_mode[axis] == MODE_HW_FAULT) return false;
	if (_control_mode[axis] == MODE_IMPEDANCE_POS ) return true;
	if (_control_mode[axis] == MODE_IMPEDANCE_VEL ) return true;
	if (_control_mode[axis] == MODE_TORQUE )        return true;
	if (_control_mode[axis] == MODE_OPENLOOP ) return false;
		
	if (_interaction_mode[axis] == icubCanProto_interactionmode_stiff) return false;	
	return true;
}

//---------------------------------------------------------------------------------------------------------------------------
bool mode_is_impedance_position(byte axis)
{
	if (_control_mode[axis] == MODE_IMPEDANCE_POS) return true;
	if (_interaction_mode[axis] == icubCanProto_interactionmode_compliant)
	{
		if (_control_mode[axis] == MODE_POSITION ||
		    _control_mode[axis] == MODE_MIXED    ||
	        _control_mode[axis] == MODE_DIRECT)	
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------------------------------------------------------
bool mode_is_impedance_velocity(byte axis)
{
	if (_control_mode[axis] == MODE_IMPEDANCE_VEL) return true;
	if (_interaction_mode[axis] == icubCanProto_interactionmode_compliant)
	{
		if (_control_mode[axis] == MODE_VELOCITY ||
		    _control_mode[axis] == MODE_MIXED)	
		return true;
	}
	return false;	
}
