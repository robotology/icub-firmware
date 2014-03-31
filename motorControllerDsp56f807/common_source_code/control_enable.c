#include "control_enable.h"
#include "pid.h"
#include "phase_hall_sens.h"
#include "brushless_comm.h"
#include "can1.h"
#include "pwm_interface.h"
#include "can_interface.h"
#include "trajectory.h"
#include "filters.h"

extern canmsg_t _canmsg;
#define CAN_DATA _canmsg.CAN_data
#define CAN_FRAME_TYPE _canmsg.CAN_frameType
#define CAN_FRAME_FMT _canmsg.CAN_frameFormat
#define CAN_LEN _canmsg.CAN_length
#define CAN_ID _canmsg.CAN_messID
#define CAN_SRC _canmsg.CAN_ID_src

void disable_motor_pwm (char axis) 
{
#if VERSION == 0x0152 || VERSION == 0x0162 || VERSION==0x0252 
//this is for waist coupling
	{  
		PWM_outputPadDisable(0);  
		PWM_outputPadDisable(1);  
		_pad_enabled[0] = false;  
		_pad_enabled[1] = false;  
		can_printf("PWM DIS COUPLED:0 & 1"); 
	}
#elif VERSION == 0x0215 || VERSION == 0x0115
//this is for eyes coupling
	{  
		if ((axis==2) || (axis==3)) 
		{ 
			PWM_outputPadDisable(2);  
			PWM_outputPadDisable(3);  
			_pad_enabled[2] = false;  
			_pad_enabled[3] = false;  
			can_printf("PWM DIS COUPLED:2 & 3"); 
		} 
		else 
		{ 
			PWM_outputPadDisable(axis);  
			_pad_enabled[axis] = false;  
			can_printf("PWM DIS:%d",axis); 
		} 
    }
#elif VERSION == 0x0219  || VERSION == 0x0119
//this is for wrist coupling
	{  
		if ((axis>0) && (axis<3)) 
		{ 
			PWM_outputPadDisable(1);  
			PWM_outputPadDisable(2);  
			_pad_enabled[1] = false;  
			_pad_enabled[2] = false;   
			can_printf("PWM DIS COUPLED:1 & 2"); 
		} 
		else 
		{ 
			PWM_outputPadDisable(axis);  
			_pad_enabled[axis] = false;   
			can_printf("PWM DIS:%d",axis); 
		} 
	}
#elif VERSION == 0x0351 
	{  
		if (_board_ID==1)  
		{  
			PWM_outputPadDisable(0);  
			PWM_outputPadDisable(1);  
			_pad_enabled[0] = false;  
			_pad_enabled[1] = false;   
			can_printf("PWM DIS COUPLED:012"); 
			if (CAN_SRC==0)  
				{  
				  CAN_ID = (_board_ID << 4) ;  
				  CAN_ID |=  2;  
				  CAN1_send( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); 
				}  
		} 
		else 
		if (_board_ID==2) 
		{ 
			PWM_outputPadDisable(0); 
			PWM_outputPadDisable(1); 
			_pad_enabled[0] = false; 
			_pad_enabled[1] = false; 
			can_printf("PWM ENA COUPLED:012");
			if (CAN_SRC==0) 
				{ 
				  CAN_ID = (_board_ID << 4) ; 
				  CAN_ID |=  1; 
				  CAN1_send( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); 
				} 
			CAN_LEN = 3; 
			CAN_DATA[1] = _control_mode[axis];
			CAN_DATA[2] = 0;
			CAN1_send( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); 
		} 
	}
#else 
	{ 
		PWM_outputPadDisable(axis); 
		_pad_enabled[axis] = false; 
		can_printf("PWM DIS:%d",axis);
	}
#endif
}

//---------------------------------------------------------------------------------------------------
void enable_motor_pwm (char axis) 
{
#if VERSION == 0x0152 || VERSION == 0x0162 || VERSION==0x0252 
//this is for waist coupling
	#define CAN_ENABLE_PWM_PAD_HANDLER(x)   
	{   
		if (_can_protocol_ack == false)   
		{   
			can_printf("can protocol NOT ack");   
			return;   
		}   
		if (_calibrated[0] == true && _calibrated[1] == true)   
		{   
			PWM_outputPadEnable(0);   
			PWM_outputPadEnable(1);   
			_control_mode[0] = MODE_IDLE;   
			_control_mode[1] = MODE_IDLE;   
			can_printf("PWM ENA COUPLED:0 & 1");  
		}   
		else   
		{   
			can_printf("calib failed 0&1");   
		}   
	}
#elif VERSION == 0x0215 || VERSION == 0x0115
//this is for eyes coupling
	#define CAN_ENABLE_PWM_PAD_HANDLER(x)   
	{   
		if (_can_protocol_ack == false)   
		{   
			can_printf("can protocol NOT ack");   
			return;   
		}   
		if ((axis==2) || (axis==3))  
		{  
			if (_pad_enabled[2] == false &&	_pad_enabled[3] == false)   
			{   
				PWM_outputPadEnable(2);   
				PWM_outputPadEnable(3);   
				can_printf("PWM ENA COUPLED:2 & 3");  
			}   
		}  
		else  
		{  
			PWM_outputPadEnable(axis);  
			_control_mode[axis] = MODE_IDLE;   
			can_printf("PWM ENA:%d",axis);  
		}  
	} 
#elif VERSION == 0x0219 || VERSION == 0x0119
//this is for wrist coupling
	#define CAN_ENABLE_PWM_PAD_HANDLER(x)   
	{   
		if (_can_protocol_ack == false)   
		{   
			can_printf("can protocol NOT ack");   
			return;   
		}   
		if ((axis>0) && (axis<3))  
		{  
			if (_pad_enabled[1] == false &&	_pad_enabled[2] == false)   
			{   
				PWM_outputPadEnable(1);   
				PWM_outputPadEnable(2);   
				can_printf("PWM ENA COUPLED:1 & 2");  
			}   
		}  
		else  
		{  
			PWM_outputPadEnable(axis);  
			_control_mode[axis] = MODE_IDLE;   
			can_printf("PWM ENA:%d",axis);  
		}  
	}
#elif VERSION == 0x0351  //ikart 
	{   
		if (_can_protocol_ack == false)   
		{   
			can_printf("can protocol NOT ack");   
			return;   
		}   
		if (_board_ID==1)   
		{   
			if (_pad_enabled[0] == false &&	_pad_enabled[1] == false)   
			{   
				PWM_outputPadEnable(0);   
				PWM_outputPadEnable(1);   
				can_printf("PWM ENA COUPLED:012");  
				if (CAN_SRC==0)   
				{   
				  CAN_ID = (_board_ID << 4) ;   
				  CAN_ID |=  2;   
				  CAN1_send( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA);   
				}   
			}   
		}   
		else   
		if (_board_ID==2)   
		{   
			if (_pad_enabled[0] == false)   
			{   
				PWM_outputPadEnable(0);   
				can_printf("PWM ENA COUPLED:012");  
				if (CAN_SRC==0)   
				{   
				  CAN_ID = (_board_ID << 4) ;   
				  CAN_ID |=  1;   
				  CAN1_send( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA);   
				}   
			}   
		}   
	}
#else
	#if (CURRENT_BOARD_TYPE  == BOARD_TYPE_4DC)
		#define CAN_ENABLE_PWM_PAD_HANDLER(x)   
		{   
			if (_can_protocol_ack == false)   
			{   
				can_printf("can protocol NOT ack");   
				return;   
			}   
			PWM_outputPadEnable(axis);   
			_control_mode[axis] = MODE_IDLE;   
			can_printf("PWM ENA:%d",axis);  
		}
	#else //(CURRENT_BOARD_TYPE  == BOARD_TYPE_4DC)
		#define CAN_ENABLE_PWM_PAD_HANDLER(x)   
		{   
			if (_can_protocol_ack == false)   
			{   
				can_printf("can protocol NOT ack");   
				return;   
			}   
			if (_calibrated[axis] == true)   
			{   
				PWM_outputPadEnable(axis);   
				_control_mode[axis] = MODE_IDLE;    
				can_printf("PWM ENA:%d",axis);  
			}   
			else   
			{   
				can_printf("calib failed:%d",axis);   
			}   
		} 
	#endif //(CURRENT_BOARD_TYPE  == BOARD_TYPE_4DC)
#endif
}

//---------------------------------------------------------------------------------------------------------------------------

void enable_control (char axis)
{
#if VERSION == 0x0152 || VERSION == 0x0162 || VERSION==0x0252 //this is for waist coupling
	{ 
		if ((_pad_enabled[0]==false) || (_pad_enabled[1]==false))
			can_printf("WARNING: RUN called before AMP");
		if ((_control_mode[0] == MODE_IDLE) || (_control_mode[1] == MODE_IDLE)) 
		{ 
		 	if ((_received_pid[0].rec_pid==0x7F) || (_received_pid[1].rec_pid==0x7F))   
			{ 
				_control_mode[0] = MODE_POSITION; 
				_control_mode[1] = MODE_POSITION; 
				_desired[0] = _position[0]; 
				_desired[1] = _position[1]; 
				_integral[0] = 0; 
				_integral[1] = 0; 
				_ko_imp[0] = 0; 
				_ko_imp[1] = 0; 
				_set_point[0] = _position[0]; 
				_set_point[1] = _position[1]; 
				init_trajectory (0, _position[0], _position[0], 1); 
				init_trajectory (1, _position[1], _position[1], 1); 
			} 
			else 
			{ 
			  can_printf("WARNING:PID IS NOT SET");
			} 
		} 
	}
#elif VERSION == 0x0215 || VERSION == 0x0115 //this is for eyes coupling
	{ 
		if ((axis==2) || (axis==3))
		{
			if ((_pad_enabled[2]==false) || (_pad_enabled[3]==false))
				can_printf("WARNING: RUN called before AMP");
			if (((_control_mode[2] == MODE_IDLE) || (_control_mode[3] == MODE_IDLE)))  
			{ 
			 	if ((_received_pid[2].rec_pid==0x7F) || (_received_pid[3].rec_pid==0x7F))   
				{ 
					_control_mode[2] = MODE_POSITION; 
					_control_mode[3] = MODE_POSITION; 
					_desired[2] = _position[2]; 
					_desired[3] = _position[3]; 
					_integral[2] = 0; 
					_integral[3] = 0; 
					_ko_imp[2] = 0; 
					_ko_imp[3] = 0; 
					_set_point[2] = _position[2]; 
					_set_point[3] = _position[3]; 
					init_trajectory (2, _position[2], _position[2], 1); 
					init_trajectory (3, _position[3], _position[3], 1); 
				} 
				else 
				{ 
				  can_printf("WARNING:PID IS NOT SET");
				} 
			} 
		}
		else
		{
			if (_pad_enabled[axis]==false)
			can_printf("WARNING: RUN called before AMP");
			if (_control_mode[axis] == MODE_IDLE) 
			{ 
				if (_received_pid[axis].rec_pid==0x7F)
				{
				_control_mode[axis] = MODE_POSITION; 
				_desired[axis] = _position[axis]; 
				_desired_vel[axis] = 0; 
				_integral[axis] = 0; 
				_ko_imp[axis] = 0; 
				_set_point[axis] = _position[axis]; 
				init_trajectory (axis, _position[axis], _position[axis], 1); 
				}
				else
				{ 
					can_printf("WARNING:PID IS NOT SET"); 
				} 
			} 
		}
	}
#elif VERSION == 0x0351 //ikart
	{ 
		if (_board_ID==1) 
		{ 
			if ((_pad_enabled[0]==false) || (_pad_enabled[1]==false)) can_printf("WARNING: RUN called before AMP");
			else if (((_control_mode[0] == MODE_IDLE) || (_control_mode[1] == MODE_IDLE)))  
			{ 
				_control_mode[0] = MODE_POSITION; 
				_control_mode[1] = MODE_POSITION; 
				_desired[0] = _position[0]; 
				_desired[1] = _position[1]; 
				_integral[0] = 0; 
				_integral[1] = 0; 
				_set_point[0] = _position[0]; 
				_set_point[1] = _position[1]; 
				if (CAN_SRC==0) 
				{ 
				  CAN_ID = (_board_ID << 4) ; 
				  CAN_ID |=  2; 
				  CAN1_send( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); 
				} 
			} 
		} 
		else 
		if (_board_ID==2) 
		{ 
			if (_pad_enabled[0]==false) can_printf("WARNING: RUN called before AMP");
			else if (((_control_mode[0] == MODE_IDLE) || (_control_mode[1] == MODE_IDLE)))  
			{ 
				_control_mode[0] = MODE_POSITION; 
				_desired[0] = _position[0]; 
				_integral[0] = 0; 
				_set_point[0] = _position[0]; 
				if (CAN_SRC==0) 
				{ 
				  CAN_ID = (_board_ID << 4) ; 
				  CAN_ID |=  1; 
				  CAN1_send( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); 
				} 
			} 
		} 
	}
#elif VERSION == 0x0219 || VERSION == 0x0119 //this is for wrist coupling
	{ 
		if ((axis>0) && (axis<3))
		{
			if ((_pad_enabled[1]==false) || (_pad_enabled[2]==false))
				can_printf("WARNING: RUN called before AMP");
			if (((_control_mode[1] == MODE_IDLE) || (_control_mode[2] == MODE_IDLE)))  
			{ 
			 	if ((_received_pid[1].rec_pid==0x7F) || (_received_pid[2].rec_pid==0x7F))   
				{ 
					_control_mode[1] = MODE_POSITION; 
					_control_mode[2] = MODE_POSITION; 
					_desired[1] = _position[1]; 
					_desired[2] = _position[2]; 
					_integral[1] = 0; 
					_integral[2] = 0; 
					_ko_imp[1] = 0; 
					_ko_imp[2] = 0; 
					_set_point[1] = _position[1]; 
					_set_point[2] = _position[2]; 
					init_trajectory (1, _position[1], _position[1], 1); 
					init_trajectory (2, _position[2], _position[2], 1); 
				} 
				else 
				{ 
				  can_printf("WARNING:PID IS NOT SET");
				} 
			} 
		}
		else
		{
			if (_pad_enabled[axis]==false)
			can_printf("WARNING: RUN called before AMP");
			if (_control_mode[axis] == MODE_IDLE) 
			{ 
				if (_received_pid[axis].rec_pid==0x7F)
				{
					_control_mode[axis] = MODE_POSITION; 
					_desired[axis] = _position[axis]; 
					_desired_vel[axis] = 0; 
					_integral[axis] = 0; 
					_ko_imp[axis] = 0; 
					_set_point[axis] = _position[axis]; 
					init_trajectory (axis, _position[axis], _position[axis], 1); 
				}
				else
				{ 
					can_printf("WARNING:PID IS NOT SET"); 
				} 
			} 
		}
	}
#else //normal boards
	{ 
		if (_pad_enabled[axis]==false)
			can_printf("WARNING: RUN called before AMP");
		else if (_control_mode[axis] == MODE_IDLE) 
		{ 
			if (_received_pid[axis].rec_pid==0x7F) 
			{ 
				_control_mode[axis] = MODE_POSITION; 
				_desired[axis] = _position[axis]; 
				_integral[axis] = 0; 
				_ko_imp[axis] = 0; 
				_set_point[axis] = _position[axis]; 
				init_trajectory (axis, _position[axis], _position[axis], 1); 
			} 
			else 
			{ 
				can_printf("WARNING:PID IS NOT SET"); 
			} 
		} 
	}
#endif
}

//---------------------------------------------------------------------------------------------------------------------------
void disable_control(char axis)
{
	if (_control_mode[axis] != MODE_IDLE)
	{
		_control_mode[axis] = MODE_IDLE;
	}
}

//---------------------------------------------------------------------------------------------------------------------------
void set_control_mode(char axis)
{
#if VERSION != 0x0351 //normal boards
	byte value = 0; 
	if (CAN_LEN == 2) 
	{ 
		value = (CAN_DATA[1]);
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
#elif VERSION == 0x0351 //ikart
	byte value = 0;
	if (CAN_LEN == 2)
	{
		if (_board_ID == 1)
		{
			value = (CAN_DATA[1]);
			can_printf("CTRLMODE SET COUPLED 012:%d",value);
			if (value>=0 && value <=0x50)
				{
					_control_mode[0] = value;
					_control_mode[1] = value;
				}
			_desired[0] = _position[0];
			_desired[1] = _position[1];
			_desired_vel[0] = 0;
			_desired_vel[1] = 0;
			_integral[0] = 0;
			_integral[1] = 0;
			_ko_imp[0] = 0;
			_ko_imp[1] = 0;
			_set_point[0] = _position[0];
			_set_point[1] = _position[1];
			_ko_openloop[0] = 0;
		    _ko_openloop[1] = 0;
			if (CAN_SRC==0)
			{ 
				 CAN_ID = (_board_ID << 4) ;
				 CAN_ID |=  2;
				 CAN1_send( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); 
			}
		}
		else
		if (_board_ID == 2)
		{ 
			value = (CAN_DATA[1]); 
			can_printf("CTRLMODE SET COUPLED 012:%d",value);
			if (value>=0 && value <=0x50)
				{ 
					_control_mode[0] = value; 
				} 
			_desired[0] = _position[0]; 
			_desired_vel[0] = 0; 
			_integral[0] = 0; 
			_ko_imp[0] = 0; 
			_set_point[0] = _position[0]; 
			_ko_openloop[0] = 0; 
			if (CAN_SRC==0) 
			{ 
				 CAN_ID = (_board_ID << 4) ; 
				 CAN_ID |=  1; 
				 CAN1_send( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); 
			} 
		} 
#endif
}
