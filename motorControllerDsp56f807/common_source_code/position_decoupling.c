#include "position_decoupling.h"
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
	
#if VERSION == 0x0115
	    Int32 temp32 = _position[0];
		_position[0] = _position[0] + _position[1];
		_position[1] = temp32       - _position[1];
#elif VERSION == 0x0215
        Int32 temp32 = _position[2];
		_position[2] = _position[2] + _position[3];
		_position[3] = temp32       - _position[3];		
#elif VERSION == 0x0119
		//_position[1] = _position[1];		//omitted
		_position[2] = _position[1] + _position[2];		
#elif VERSION == 0x0219
		//_position[1] = _position[1];		//omitted
		//_position[2] = _position[2];		//omitted
#elif   VERSION == 0x0140 	
	//_position [0] = _position[0];
	_position[1] = (float) (-_position[0] + _position[1]) * 1.625F;
	
#elif   VERSION == 0x0147
 	//_position [0] = _position[0];
 	//_position [1] = _position[1];

#elif   VERSION == 0x0157 
	_cpl_pos_counter++;
	if (_cpl_pos_counter < timeout_cpl_pos  && (get_error_abs_ssi(0)==ERR_OK))
	{
		// beware of the first cycle when _old has no meaning		
		_position[0] = (((float) _position[0])*0.6153F);  
		_position[0] = _position[0]+ _cpl_pos_prediction[0];
		_position[0] = _position[0]- _cpl_pos_prediction[1];
		
		// |J1| |  1     0    0   |  |E1|     pulley diameter
		// |J2|=|  0     1    0   |* |E2|     with a=40/65 i.e. a=0.6153
		// |J3| |  1    -1    a   |  |E3|
		
		_cpl_pos_prediction[0] = L_add(_cpl_pos_prediction[0], _cpl_pos_delta[0]);
		_cpl_pos_prediction[1] = L_add(_cpl_pos_prediction[1], _cpl_pos_delta[1]);
	}
	else
	{
		//disable the joint if no POSITION data is receveived from the coupled joint
		if (!mode_is_idle(0))
		{
			put_motor_in_fault(0);	
			#ifdef DEBUG_CAN_MSG
			if(count==255)
			{
				can_printf("No cpl pos info");
				count=0;
			}			
			count++;				
			#endif	
		}
	}
#endif
}
