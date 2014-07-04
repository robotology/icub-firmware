#include "strain_board.h"
#include "control_enable.h"
#include "pid.h"
#include "ad.h"
#include "can1.h" 

Int16 _strain[STRAIN_MAX][6];
Int16 _strain_init[STRAIN_MAX][6];
Int16 _strain_old[STRAIN_MAX][6];

byte  _strain_wtd[STRAIN_MAX]   = INIT_ARRAY (STRAIN_SAFE);;

byte  _selected_strain_id[JN]   = INIT_ARRAY (-1);
byte  _selected_strain_chan[JN] = INIT_ARRAY (-1);

extern byte _board_ID;

/*************************************************************************** 
 * Initialization
 ***************************************************************************/
void start_strain (word can_address)
{
	//start the can transmission. CAN must be already intialized.
	canmsg_t canmsg;
	canmsg.CAN_messID = can_address;
	canmsg.CAN_data[0] = 0x07;
	canmsg.CAN_data[1] = 0x00;
	canmsg.CAN_length = 2;
	canmsg.CAN_frameType = DATA_FRAME;
	CAN1_send (canmsg.CAN_messID, canmsg.CAN_frameType, canmsg.CAN_length, canmsg.CAN_data);	
}

/*************************************************************************** 
 * Initialization
 ***************************************************************************/
void stop_strain (word can_address)
{
	//start the can transmission. CAN must be already intialized.
	canmsg_t canmsg;
	canmsg.CAN_messID = can_address;
	canmsg.CAN_data[0] = 0x07;
	canmsg.CAN_data[1] = 0x01;
	canmsg.CAN_length = 2;
	canmsg.CAN_frameType = DATA_FRAME;
	CAN1_send (canmsg.CAN_messID, canmsg.CAN_frameType, canmsg.CAN_length, canmsg.CAN_data);	
}

/*************************************************************************** 
 * Initialization
 ***************************************************************************/
void init_strain (void)
{
	byte i=0;
	byte k=0;
	canmsg_t canmsg;
		
	//init reading 
	for (i=0; i<6; i++)
		for (k=0; k<STRAIN_MAX; k++)
		{
			_strain[k][i] = 0;
			_strain_init[k][i] = 0;
			_strain_old[k][i] = 0;			
		}
		
	//init watchdog
	for (k=0; k<STRAIN_MAX; k++)
		{
			_strain_wtd[k]= STRAIN_SAFE;	
		}
	
	//default listening channels for all firmware versions 
	#if   (VERSION == 0x150 || VERSION == 0x140 || VERSION == 0x250 ) //arm
		{
		  	_selected_strain_id[0] = CAN_ID_STRAIN_12;
		    _selected_strain_id[1] = CAN_ID_STRAIN_12;
		  	_selected_strain_chan[0] = 0;
		    _selected_strain_chan[1] = 1;	
		}
	#elif (VERSION == 0x151 || VERSION == 0x251 ) //legs
	  	if (_board_ID==5)     //left leg
	  	{
		  	_selected_strain_id[0] = CAN_ID_STRAIN_12;
		  	_selected_strain_id[1] = CAN_ID_STRAIN_12;
		  	_selected_strain_chan[0] = 2;
		    _selected_strain_chan[1] = 3;	  		
	  	}
	  	else if (_board_ID==6) //left leg
	  	{
	  		_selected_strain_id[0] = CAN_ID_STRAIN_12;
	  		_selected_strain_id[1] = CAN_ID_STRAIN_12;
	  		_selected_strain_chan[0] = 0;
	  		_selected_strain_chan[1] = 1; 		
	  	}
	  	else if (_board_ID==7) //left leg
	  	{
	  		_selected_strain_id[0] = CAN_ID_STRAIN_12;
	  		_selected_strain_id[1] = CAN_ID_STRAIN_12;
	  		_selected_strain_chan[0] = 4;
	  		_selected_strain_chan[1] = 5; 	  		
	  	}
	  	else if (_board_ID==8) //right leg
	  	{
		  	_selected_strain_id[0] = CAN_ID_STRAIN_11;
		  	_selected_strain_id[1] = CAN_ID_STRAIN_11;
		  	_selected_strain_chan[0] = 2;
		    _selected_strain_chan[1] = 3;	  		
	  	}  
	  	else if (_board_ID==9) //right leg
	  	{
	  		_selected_strain_id[0] = CAN_ID_STRAIN_11;
	  		_selected_strain_id[1] = CAN_ID_STRAIN_11;
	  		_selected_strain_chan[0] = 0;
	  		_selected_strain_chan[1] = 1;	  		
	  	}
	  	else if (_board_ID==10) //right leg
	  	{
	  		_selected_strain_id[0] = CAN_ID_STRAIN_11;
	  		_selected_strain_id[1] = CAN_ID_STRAIN_11;
	  		_selected_strain_chan[0] = 4;
	  		_selected_strain_chan[1] = 5;  	  		
	  	}  	  	
	#elif (VERSION == 0x152 || VERSION == 0x252) //torso
	  	{
	  		_selected_strain_id[0] = CAN_ID_STRAIN_12;
	  		_selected_strain_id[1] = CAN_ID_STRAIN_12;
	  		_selected_strain_chan[0] = 1;
	  		_selected_strain_chan[1] = 2; 
	  	}
	#elif (VERSION == 0x154 || VERSION == 0x254) //torso
	  	{
	  		_selected_strain_id[0] = CAN_ID_STRAIN_12;
	  		_selected_strain_chan[0] = 0;
	  	}
	#elif (VERSION == 0x157 || VERSION == 0x147 || VERSION == 0x257) //coupled joint of the arm
	    {
	    	_selected_strain_id[0] = CAN_ID_STRAIN_12;
		  	_selected_strain_id[1] = CAN_ID_STRAIN_12;
		  	_selected_strain_chan[0] = 2;
		    _selected_strain_chan[1] = 3;
	    }
	#elif  (VERSION == 0x0119) 	//arm
		{	
			_selected_strain_id[0] = CAN_ID_STRAIN_12; //wrist pronosupination
	  		_selected_strain_id[1] = CAN_ID_STRAIN_11; //wrist pitch disabled
	  		_selected_strain_id[2] = CAN_ID_STRAIN_11; //wrist yaw   disabled
	  		_selected_strain_id[3] = -1;                //fingers disabled
	  		_selected_strain_chan[0] = 4;
	  		_selected_strain_chan[1] = 0; 
	  		_selected_strain_chan[2] = 1;
	  		_selected_strain_chan[3] = 0; 
		}
	#elif  (VERSION == 0x0219)	 //armV2
		{
			_selected_strain_id[0] = CAN_ID_STRAIN_12; //wrist pronosupination
	  		_selected_strain_id[1] = CAN_ID_STRAIN_11; //@@@TODO differential 1 for icubV2
	  		_selected_strain_id[2] = CAN_ID_STRAIN_11; //@@@TODO differential wrist 2 for icubV2
	  		_selected_strain_id[3] = -1;                //fingers disabled
	  		_selected_strain_chan[0] = 4;
	  		_selected_strain_chan[1] = 0; 
	  		_selected_strain_chan[2] = 1;
	  		_selected_strain_chan[3] = 0; 		
		}
	#endif

}

/*************************************************************************** 
 * strain gauges sampling
 ***************************************************************************/
word read_strain(byte jnt, bool sign)
{
	return 0;
}

/***************************************************************************/
/**
 * this function turns off pwm of joint <jnt> if <strain_num> watchdog is
 * triggered (returns false). Returns true otherwise (all ok).
 * the force value contained in the <strain_channel> is assigned to strain_val
 ***************************************************************************/
bool read_force_data (byte jnt, byte strain_num, byte strain_chan)
{
	if (mode_is_force_controlled(jnt))
	{
		if (strain_num==-1)
		{
			put_motor_in_fault(jnt);	

			#ifdef DEBUG_CAN_MSG					
				can_printf("WARN:force control not allowed jnt:%d",jnt);
			#endif
							
			_strain_val[jnt]=0;
			return false;				
		}
		if (_strain_wtd[strain_num]==0)
		{
			put_motor_in_fault(jnt);	
				
			#ifdef DEBUG_CAN_MSG
				can_printf("WARN:strain watchdog disabling pwm jnt:%d",jnt);				
			#endif	
			
			_strain_val[jnt]=0;
			return false;
		}
		else
		{
			_strain_val[jnt]=_strain[strain_num][strain_chan];
			return true;	
		}	
	}
	else
	{
		//here we are in stiff control mode (i.e. position)
		return true;			
	}	
}

