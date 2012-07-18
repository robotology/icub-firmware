#include "dsp56f807.h"
#include "options.h"
#include "pid.h"
#include "trajectory.h"
#include "can_interface.h"
#include "flash_interface.h"
#include "pwm_interface.h"
#include "calibration.h"
#include "encoders_interface.h"
#include "currents_interface.h"
#include "abs_ssi_interface.h"
#include "messages.h"
#include "asc.h"
#include "can1.h"
#include "phase_hall_sens.h"
#include "brushless_comm.h"
#include "strain_board.h"
#include "filters.h"
#include "identification.h"

#ifndef VERSION
#	error "No valid version specified"
#endif

#ifdef IDENTIF
#	include "identification.h"
#endif 

#ifdef USE_NEW_DECOUPLING
#	warning "Using new decoupling"
#else
#   warning "NOT using new decoupling"
#endif

#if CURRENT_BOARD_TYPE != BOARD_TYPE_4DC
	extern sDutyControlBL DutyCycle[2];
	extern sDutyControlBL DutyCycleReq[2];
#endif

//************************************
// CAN bus communication global vars 
//************************************

extern canmsg_t can_fifo[];
extern Int16 write_p;
extern Int16 read_p;

extern UInt8 mais_counter;
canmsg_t _canmsg;					// buffer to prepare messages for send 

byte  _general_board_error = ERROR_NONE;
byte  can_channels = 0; 
dword broadcast_mask[JN]=INIT_ARRAY (0); // specifies which broadcast messages are to be sent 

extern byte	_board_ID;
extern void  bootStart(void);            // Defined in linker.cmd file  
extern char  _additional_info [32];

extern byte _build_number;
extern byte _my_can_protocol_major;
extern byte _my_can_protocol_minor;
extern bool _can_protocol_ack;

extern UInt8 mainLoopOVF;
extern int   _countBoardStatus[2];
extern UInt8 highcurrent[2];

#ifdef TEMPERATURE_SENSOR
	extern UInt8   overtemp[2];
	extern UInt8   errortemp[2];
	extern Int16   TempSens[2];
#endif


/**********************************************************************
  Converts four bytes in a double word
 **********************************************************************/
dword BYTE_C(byte x4, byte x3, byte x2, byte x1)
{
	dword ret;
	word *p = (word *)&ret;
	*p++ = __shl(x3,8) | x4;
	*p++ = __shl(x1,8) | x2;
	return ret;
}

/***************************************************************************/
/**
 * This method inits the CAN interface
 * @param b_type
 ***************************************************************************/
void can_interface_init (byte channels_num)
{
	can_channels = channels_num;
	CAN1_init (_board_ID);
	set_can_masks();
}

/***************************************************************************/
/**
 * This method extracts from the can message the information about the target
 * axis of the board.
 * @return the axis number: 
 * 0-1 for 2DC/2BLL boards,
 * 0-3 for 4DC boards
 ***************************************************************************/
 inline int get_axis (void) 
 {
 	if (can_channels == 2 )
 	 	return ((_canmsg.CAN_data[0] & 0x80) ? 1 : 0) ;
 	
 	if (can_channels == 4 )
 	{
 		if (_board_ID == (_canmsg.CAN_messID & 0x000F)) 
			return ((_canmsg.CAN_data[0] & 0x80) ? 1 : 0) ;
		else if (_board_ID+1 == (_canmsg.CAN_messID & 0x000F)) 
			return ((_canmsg.CAN_data[0] & 0x80) ? 3 : 2) ; 
 	}
 }
 
/***************************************************************************/
/**
 * This method reads and executes the CAN messages.
/***************************************************************************/
byte can_interface (void)
{
	bool done = false;
	byte i=0;
	byte j=0;
	word ikk;
	dword IdTx;
	byte  write_buffer = 0;
	byte strain_num = 0;

	canmsg_t *p;
		
	CAN_DI;
	if (read_p != -1)
	{
		CAN_EI;
		while (!done)
		{
		
		CAN_DI;	
			p = can_fifo + read_p;
			
			/* makes a private copy of the message */
			_canmsg.CAN_data[0] = p->CAN_data[0];
			_canmsg.CAN_data[1] = p->CAN_data[1];
			_canmsg.CAN_data[2] = p->CAN_data[2];
			_canmsg.CAN_data[3] = p->CAN_data[3];
			_canmsg.CAN_data[4] = p->CAN_data[4];
			_canmsg.CAN_data[5] = p->CAN_data[5];
			_canmsg.CAN_data[6] = p->CAN_data[6];
			_canmsg.CAN_data[7] = p->CAN_data[7];
			_canmsg.CAN_messID = p->CAN_messID;
			_canmsg.CAN_ID_class= (unsigned char) (_canmsg.CAN_messID>>8 & 0x7);
			_canmsg.CAN_ID_src=(unsigned char) (_canmsg.CAN_messID>>4 & 0xf);
			_canmsg.CAN_ID_dst=(unsigned char) (_canmsg.CAN_messID  & 0xf);
			_canmsg.CAN_frameType = p->CAN_frameType;
			_canmsg.CAN_frameFormat = p->CAN_frameFormat;
			_canmsg.CAN_length = p->CAN_length;
			if (read_p == write_p)
			{
				read_p = -1;	/* empty */
				done = true;
			}
			else
			{
				read_p ++;
				if (read_p >= CAN_FIFO_LEN)
					read_p = 0;
				//done = true;
			}
    	CAN_EI;
			
			if (write_buffer == 0)
				write_buffer = 2;
			else
				write_buffer = 0;

#define CAN_BUFFER write_buffer
#define CAN_DATA _canmsg.CAN_data
#define CAN_FRAME_TYPE _canmsg.CAN_frameType
#define CAN_FRAME_FMT _canmsg.CAN_frameFormat
#define CAN_LEN _canmsg.CAN_length
#define CAN_ID _canmsg.CAN_messID
#define CAN_SRC _canmsg.CAN_ID_src
#define CAN_TEMP16 ikk

			// special message, not too neat
			// ID 0x0100 (001 0000 0000b) message class = periodic message  
			
#if VERSION == 0x0153 || VERSION==0x0157 || VERSION==0x0150 || VERSION == 0x0351 || VERSION==0x0147 || VERSION==0x0140 || VERSION==0x0250 || VERSION==0x0257

			if (_canmsg.CAN_ID_class == CLASS_PERIODIC_DSP)
			{
				// for messages of class 001, the meaning of data/ID is:
				// AAA BBBB CCCC, with AAA=Message class, BBBB Source address, CCCC message command
				
				// Switch on the message command: 000 0000 1111b
	 			switch (_canmsg.CAN_ID_dst)
				{
					case CAN_BCAST_POSITION:
		 				CAN_SET_ACTIVE_ENCODER_POSITION_HANDLER(0)
						break;

					case CAN_BCAST_PID_VAL:
						CAN_SET_ACTIVE_PID_HANDLER(0)
						break;
		
					case CAN_BCAST_CURRENT:	
						CAN_SET_ACTIVE_ERROR_HANDLER(0)
						break;
						
					default:
						break;
				}
			}
			else
#else
			if (_canmsg.CAN_ID_class == CLASS_PERIODIC_DSP)
			{
			}
			else
#endif

#if VERSION == 0x0119 || VERSION == 0x0150 || VERSION == 0x0151 || VERSION == 0x0152 || VERSION == 0x0154 || VERSION == 0x0157 || VERSION == 0x0140 || VERSION == 0x0147 || VERSION==0x0250 || VERSION == 0x0251 || VERSION==0x0257 || VERSION==0x0252 || VERSION==0x0254  
			if (_canmsg.CAN_ID_class == CLASS_PERIODIC_SENS)
			{
				if 		(_canmsg.CAN_ID_src==CAN_ID_JNT_STRAIN_11)  strain_num=WDT_JNT_STRAIN_11;
				else if (_canmsg.CAN_ID_src==CAN_ID_JNT_STRAIN_12)  strain_num=WDT_JNT_STRAIN_12;
				else if (_canmsg.CAN_ID_src==CAN_ID_6AX_STRAIN_13)  strain_num=WDT_6AX_STRAIN_13;
				else if (_canmsg.CAN_ID_src==CAN_ID_6AX_STRAIN_14)  strain_num=WDT_6AX_STRAIN_14;			
				else    										 
				{
					can_printf("ERR: UNKNOWN STRAIN!");
					strain_num=0;
				}

				switch(_canmsg.CAN_ID_dst)
				{
				
					case 0xa:
					_strain[strain_num][0] = (Int16)(BYTE_W(_canmsg.CAN_data[0],_canmsg.CAN_data[1])-(UInt16)0x8000);
					_strain[strain_num][1] = (Int16)(BYTE_W(_canmsg.CAN_data[2],_canmsg.CAN_data[3])-(UInt16)0x8000);
					_strain[strain_num][2] = (Int16)(BYTE_W(_canmsg.CAN_data[4],_canmsg.CAN_data[5])-(UInt16)0x8000);
					break;
					
					case 0xb:
					_strain[strain_num][3] = (Int16)(BYTE_W(_canmsg.CAN_data[0],_canmsg.CAN_data[1])-(UInt16)0x8000);
					_strain[strain_num][4] = (Int16)(BYTE_W(_canmsg.CAN_data[2],_canmsg.CAN_data[3])-(UInt16)0x8000);
					_strain[strain_num][5] = (Int16)(BYTE_W(_canmsg.CAN_data[4],_canmsg.CAN_data[5])-(UInt16)0x8000);
					break;
				}
				
				_strain_wtd[strain_num] = STRAIN_SAFE;
			}
			else 
#elif (VERSION == 0x0121 || VERSION == 0x0128 || VERSION == 0x0130)

			if (_canmsg.CAN_ID_class == CLASS_PERIODIC_SENS)
			{
			
				switch (_canmsg.CAN_ID_src)
				{
					case MAIS_CAN_ID:
					
						mais_counter=0;
						CAN_SET_ACTIVE_ENCODER_POSITION_HANDLER(0)
						break;
					default:
						break;
				}
			}
			else 
#endif

			// special message for the can loader 
			// ID 0x0700 (111 0000 0000b) message class = broadcast message 
			if (_canmsg.CAN_ID_class == CLASS_CANLOADER)
			{
				// if canloader message is for me or is for all...
				if (_canmsg.CAN_ID_dst == _board_ID ||
				    _canmsg.CAN_ID_dst == 0x0f)
					{
		 				IdTx = (_canmsg.CAN_messID & 0x0700);			
						IdTx |= (L_deposit_l (_board_ID) << 4);
						IdTx |= ((_canmsg.CAN_messID >> 4) & 0x000F);
						
				    	switch (_canmsg.CAN_data[0]) 
				    	{
				    		//request for board type and firmware versione
							case 0xFF:
								_canmsg.CAN_data[0] = 0xFF;
								_canmsg.CAN_data[1] = CURRENT_BOARD_TYPE;  
								_canmsg.CAN_data[2] = (_version & 0xff00) >> 8;  // firmware version.	
								_canmsg.CAN_data[3] = (unsigned char) (_version & 0x00ff); 		 // firmware revision.
								_canmsg.CAN_data[4] = _build_number;             // build number
								CAN1_send(IdTx, DATA_FRAME, 5, _canmsg.CAN_data);

								
								// disable PWM
								for (i=0; i<JN; i++)
								{
									PWM_outputPadDisable(i); 								
									_pad_enabled[i] = false; 
									_general_board_error = ERROR_NONE; 								
								}
#ifdef DEBUG_CAN_MSG							
								can_printf("CANLOADER DIS");
#endif	
								break;
						
							// ping command	
						    case 4:
			    				if (_board_ID == _canmsg.CAN_ID_dst) 
			    				{
									_canmsg.CAN_data[0] = 4;
									_canmsg.CAN_data[1] = 1; 
									CAN1_send (IdTx, DATA_FRAME, 2, _canmsg.CAN_data);
								}	
								break;
							
							// restart bootloader	
						    case 0:
			    				if (_board_ID == _canmsg.CAN_ID_dst)
			    					asm(jsr bootStart);	/// check whether this has to be a JMP rather than a JSR.
			    				break;
			    				
			    			// set additional info					
				    		case CAN_SET_ADDITIONAL_INFO:
				    			if (_board_ID == _canmsg.CAN_ID_dst) 
			    				{
					    			can_receive_additional_info(); 
									writeToFlash ( (unsigned int)_flash_addr); 
			    				}
			    			break;
			    			
			    			// get additional info	
				    		case CAN_GET_ADDITIONAL_INFO:
				    			if (_board_ID == _canmsg.CAN_ID_dst) 
			    				{
					    			CAN_ID >>= 4; 
									CAN_ID &= 0xffffff0f; 
									CAN_ID |= (_board_ID << 4); 
					    			CAN_ID |= 0x700;
					    			can_send_additional_info(); 
			    				}
				    		break;
			    		}	
					}
			}
			
			// polling message 
			// ID 0x0000 (000 xxxx xxxxb) message class = polling message 
			else if (_canmsg.CAN_ID_class == CLASS_POLLING_DSP)
			{
				int axis = get_axis();
				BEGIN_MSG_TABLE (_canmsg.CAN_data[0]) 
				
				//High priority (most frequent) messages. 
				//The 'switch/break' statements ensures that the other conditions are not evaluated.
				HANDLE_MSG (CAN_POSITION_MOVE, CAN_POSITION_MOVE_HANDLER)
				HANDLE_MSG (CAN_VELOCITY_MOVE, CAN_VELOCITY_MOVE_HANDLER)
				
				//Low priority (occasional) messages.
				HANDLE_MSG (CAN_NO_MESSAGE, CAN_NO_MESSAGE_HANDLER)
				HANDLE_MSG (CAN_CONTROLLER_RUN, CAN_CONTROLLER_RUN_HANDLER)
				HANDLE_MSG (CAN_CONTROLLER_IDLE, CAN_CONTROLLER_IDLE_HANDLER)
				HANDLE_MSG (CAN_TOGGLE_VERBOSE, CAN_TOGGLE_VERBOSE_HANDLER)
				HANDLE_MSG (CAN_CALIBRATE_ENCODER, CAN_CALIBRATE_ENCODER_HANDLER)
				HANDLE_MSG (CAN_ENABLE_PWM_PAD, CAN_ENABLE_PWM_PAD_HANDLER)
				HANDLE_MSG (CAN_DISABLE_PWM_PAD, CAN_DISABLE_PWM_PAD_HANDLER)
				HANDLE_MSG (CAN_GET_CONTROL_MODE, CAN_GET_CONTROL_MODE_HANDLER)
				HANDLE_MSG (CAN_SET_CONTROL_MODE, CAN_SET_CONTROL_MODE_HANDLER)
				HANDLE_MSG (CAN_MOTION_DONE, CAN_MOTION_DONE_HANDLER)
			
				HANDLE_MSG (CAN_WRITE_FLASH_MEM, CAN_WRITE_FLASH_MEM_HANDLER)
				HANDLE_MSG (CAN_READ_FLASH_MEM, CAN_READ_FLASH_MEM_HANDLER)
				HANDLE_MSG (CAN_GET_ADDITIONAL_INFO, CAN_GET_ADDITIONAL_INFO_HANDLER)
				HANDLE_MSG (CAN_SET_ADDITIONAL_INFO, CAN_SET_ADDITIONAL_INFO_HANDLER)
			
				HANDLE_MSG (CAN_GET_ENCODER_POSITION, CAN_GET_ENCODER_POSITION_HANDLER)
				HANDLE_MSG (CAN_SET_DESIRED_POSITION, CAN_SET_DESIRED_POSITION_HANDLER)
				HANDLE_MSG (CAN_GET_DESIRED_POSITION, CAN_GET_DESIRED_POSITION_HANDLER)
				HANDLE_MSG (CAN_SET_DESIRED_VELOCITY, CAN_SET_DESIRED_VELOCITY_HANDLER)
				HANDLE_MSG (CAN_GET_DESIRED_VELOCITY, CAN_GET_DESIRED_VELOCITY_HANDLER)
				HANDLE_MSG (CAN_SET_DESIRED_ACCELER, CAN_SET_DESIRED_ACCELER_HANDLER)
				HANDLE_MSG (CAN_GET_DESIRED_ACCELER, CAN_GET_DESIRED_ACCELER_HANDLER)
				HANDLE_MSG (CAN_SET_DESIRED_TORQUE, CAN_SET_DESIRED_TORQUE_HANDLER)
				HANDLE_MSG (CAN_GET_DESIRED_TORQUE, CAN_GET_DESIRED_TORQUE_HANDLER)
				
				HANDLE_MSG (CAN_SET_DEBUG_PARAM, CAN_SET_DEBUG_PARAM_HANDLER)
				HANDLE_MSG (CAN_GET_DEBUG_PARAM, CAN_GET_DEBUG_PARAM_HANDLER)
				HANDLE_MSG (CAN_SET_SPEED_ESTIM_SHIFT, CAN_SET_SPEED_ESTIM_SHIFT_HANDLER) 

				HANDLE_MSG (CAN_SET_ENCODER_POSITION, CAN_SET_ENCODER_POSITION_HANDLER)
				HANDLE_MSG (CAN_GET_ENCODER_VELOCITY, CAN_GET_ENCODER_VELOCITY_HANDLER)
				HANDLE_MSG (CAN_SET_COMMAND_POSITION, CAN_SET_COMMAND_POSITION_HANDLER)
				
				HANDLE_MSG (CAN_SET_P_GAIN, CAN_SET_P_GAIN_HANDLER)
				HANDLE_MSG (CAN_GET_P_GAIN, CAN_GET_P_GAIN_HANDLER)
				HANDLE_MSG (CAN_SET_D_GAIN, CAN_SET_D_GAIN_HANDLER)
				HANDLE_MSG (CAN_GET_D_GAIN, CAN_GET_D_GAIN_HANDLER)
				HANDLE_MSG (CAN_SET_I_GAIN, CAN_SET_I_GAIN_HANDLER)
				HANDLE_MSG (CAN_GET_I_GAIN, CAN_GET_I_GAIN_HANDLER)
				HANDLE_MSG (CAN_SET_ILIM_GAIN, CAN_SET_ILIM_GAIN_HANDLER)
				HANDLE_MSG (CAN_GET_ILIM_GAIN, CAN_GET_ILIM_GAIN_HANDLER)
				HANDLE_MSG (CAN_SET_OFFSET, CAN_SET_OFFSET_HANDLER)
				HANDLE_MSG (CAN_GET_OFFSET, CAN_GET_OFFSET_HANDLER)
				HANDLE_MSG (CAN_SET_SCALE, CAN_SET_SCALE_HANDLER)
				HANDLE_MSG (CAN_GET_SCALE, CAN_GET_SCALE_HANDLER)
			
				HANDLE_MSG (CAN_GET_PID_OUTPUT, CAN_GET_PID_OUTPUT_HANDLER)
				HANDLE_MSG (CAN_GET_PID_ERROR, CAN_GET_PID_ERROR_HANDLER)
				
				HANDLE_MSG (CAN_SET_MIN_POSITION, CAN_SET_MIN_POSITION_HANDLER)
				HANDLE_MSG (CAN_GET_MIN_POSITION, CAN_GET_MIN_POSITION_HANDLER)
				HANDLE_MSG (CAN_SET_MAX_POSITION, CAN_SET_MAX_POSITION_HANDLER)
				HANDLE_MSG (CAN_GET_MAX_POSITION, CAN_GET_MAX_POSITION_HANDLER)
				HANDLE_MSG (CAN_SET_OFFSET_ABS_ENCODER, CAN_SET_OFFSET_ABS_ENCODER_HANDLER)
				HANDLE_MSG (CAN_GET_OFFSET_ABS_ENCODER, CAN_GET_OFFSET_ABS_ENCODER_HANDLER)
				HANDLE_MSG (CAN_SET_MAX_VELOCITY, CAN_SET_MAX_VELOCITY_HANDLER)
				HANDLE_MSG (CAN_GET_MAX_VELOCITY, CAN_GET_MAX_VELOCITY_HANDLER)
			
				HANDLE_MSG (CAN_SET_TLIM, CAN_SET_TLIM_HANDLER)
				HANDLE_MSG (CAN_GET_TLIM, CAN_GET_TLIM_HANDLER)
			
				HANDLE_MSG (CAN_SET_BOARD_ID, CAN_SET_BOARD_ID_HANDLER)
				HANDLE_MSG (CAN_GET_BOARD_ID, CAN_GET_BOARD_ID_HANDLER)
				
				HANDLE_MSG (CAN_SET_CURRENT_LIMIT, CAN_SET_CURRENT_LIMIT_HANDLER)
				HANDLE_MSG (CAN_SET_BCAST_POLICY, CAN_SET_BCAST_POLICY_HANDLER)
				HANDLE_MSG (CAN_GET_ERROR_STATUS, CAN_GET_ERROR_STATUS_HANDLER)
				HANDLE_MSG (CAN_SET_VEL_SHIFT, CAN_SET_VEL_SHIFT_HANDLER)
				HANDLE_MSG (CAN_SET_VEL_TIMEOUT, CAN_SET_VEL_TIMEOUT_HANDLER)
				HANDLE_MSG (CAN_STOP_TRAJECTORY, CAN_STOP_TRAJECTORY_HANDLER)
				
#ifdef SMOOTH_PID_CTRL				
				HANDLE_MSG (CAN_SET_SMOOTH_PID, CAN_SET_SMOOTH_PID_HANDLER)
#endif				
				HANDLE_MSG (CAN_SET_TORQUE_PID, CAN_SET_TORQUE_PID_HANDLER)
				HANDLE_MSG (CAN_GET_TORQUE_PID, CAN_GET_TORQUE_PID_HANDLER)
				HANDLE_MSG (CAN_SET_TORQUE_PIDLIMITS, CAN_SET_TORQUE_PIDLIMITS_HANDLER)
				HANDLE_MSG (CAN_GET_TORQUE_PIDLIMITS, CAN_GET_TORQUE_PIDLIMITS_HANDLER)
				HANDLE_MSG (CAN_SET_POS_PID, CAN_SET_POS_PID_HANDLER)
				HANDLE_MSG (CAN_GET_POS_PID, CAN_GET_POS_PID_HANDLER)
				HANDLE_MSG (CAN_SET_POS_PIDLIMITS, CAN_SET_POS_PIDLIMITS_HANDLER)
				HANDLE_MSG (CAN_GET_POS_PIDLIMITS, CAN_GET_POS_PIDLIMITS_HANDLER)
				HANDLE_MSG (CAN_SET_IMPEDANCE_PARAMS, CAN_SET_IMPEDANCE_PARAMS_HANDLER)
				HANDLE_MSG (CAN_GET_IMPEDANCE_PARAMS, CAN_GET_IMPEDANCE_PARAMS_HANDLER)
				HANDLE_MSG (CAN_SET_IMPEDANCE_OFFSET, CAN_SET_IMPEDANCE_OFFSET_HANDLER)
				HANDLE_MSG (CAN_GET_IMPEDANCE_OFFSET, CAN_GET_IMPEDANCE_OFFSET_HANDLER)
				
				HANDLE_MSG (CAN_GET_FIRMWARE_VERSION, CAN_GET_FIRMWARE_VERSION_HANDLER)
										
	//			HANDLE_MSG (CAN_GET_ACTIVE_ENCODER_POSITION, CAN_GET_ACTIVE_ENCODER_POSITION_HANDLER)
			
				END_MSG_TABLE		
			}
			else 
			{
				// UNKNOWN _canmsg.CAN_ID_class
			}

		} /* end of while() */
		
	} /* end of if () */
	else
	{
		CAN_EI;
	}
			
	return ERR_OK;
}

/*********************************************************** 
 * send broadcast messages FAST VERSION FOR IDENTIFICATION
 ***********************************************************/
void can_send_broadcast_identification(byte j)
{
#ifdef IDENTIF
	if (broadcast_mask[0] == 0) return;
	if (j==0 && sine_ampl[0] == 0) return;
	if (j==1 && sine_ampl[1] == 0) return;
	
		_canmsg.CAN_messID = 0x100;
		_canmsg.CAN_messID |= (_board_ID) << 4;
		_canmsg.CAN_messID |= 10; //special broadcast value
		_canmsg.CAN_data[0] = BYTE_H((Int16)_position[j]); //WARNING: _position casted from 32 to 16 bit
		_canmsg.CAN_data[1] = BYTE_L((Int16)_position[j]);
		_canmsg.CAN_data[2] = BYTE_H(_speed[j]);
		_canmsg.CAN_data[3] = BYTE_L(_speed[j]);
		_canmsg.CAN_data[4] = BYTE_H(_pid[j]);
		_canmsg.CAN_data[5] = BYTE_L(_pid[j]);
		_canmsg.CAN_data[6] = BYTE_H(_strain_val[j]);
		_canmsg.CAN_data[7] = BYTE_L(_strain_val[j]);			
		_canmsg.CAN_length = 8;
		_canmsg.CAN_frameType = DATA_FRAME;
		CAN1_send (_canmsg.CAN_messID, _canmsg.CAN_frameType, _canmsg.CAN_length, _canmsg.CAN_data);

		_canmsg.CAN_messID = 0x100;
		_canmsg.CAN_messID |= (_board_ID) << 4;
		_canmsg.CAN_messID |= 11; //special broadcast value
		_canmsg.CAN_data[0] = BYTE_H((Int16)sine_freq[j]*10); 
		_canmsg.CAN_data[1] = BYTE_L((Int16)sine_freq[j]*10); 
		_canmsg.CAN_data[2] = BYTE_H((Int16)sine_ampl[j]); 
		_canmsg.CAN_data[3] = BYTE_L((Int16)sine_ampl[j]); 
		_canmsg.CAN_data[4] = 0;
		_canmsg.CAN_data[5] = 0;
		if (DutyCycle[j].Dir==0)
		{
		    _canmsg.CAN_data[4] = BYTE_H((Int16)DutyCycle[j].Duty);
			_canmsg.CAN_data[5] = BYTE_L((Int16)DutyCycle[j].Duty);		
	    }
		else
		{
			_canmsg.CAN_data[4] = BYTE_H(-1*(Int16)DutyCycle[j].Duty);
			_canmsg.CAN_data[5] = BYTE_L(-1*(Int16)DutyCycle[j].Duty);
		}
		_canmsg.CAN_data[6] = 0;
		_canmsg.CAN_data[7] = 0;			
		_canmsg.CAN_length = 8;
		_canmsg.CAN_frameType = DATA_FRAME;
		CAN1_send (_canmsg.CAN_messID, _canmsg.CAN_frameType, _canmsg.CAN_length, _canmsg.CAN_data);

#endif 
}

/********************************************************************* 
 * Send broadcast debug messages
 * you can (optionally) call this function from the main() in order
 * to output the _debug[j] variables every <rate> milliseconds.
 *********************************************************************/
void can_send_broadcast_debug(byte j, int rate)
{
	static byte count = 0;
	if (count==rate)
	{
		_canmsg.CAN_messID = 0x100;
		_canmsg.CAN_messID |= (_board_ID) << 4;
		_canmsg.CAN_messID |= CAN_BCAST_DEBUG;

		_canmsg.CAN_data[0] = BYTE_H(_debug_out1[j]);
		_canmsg.CAN_data[1] = BYTE_L(_debug_out1[j]);
		_canmsg.CAN_data[2] = BYTE_H(_debug_out2[j]);
		_canmsg.CAN_data[3] = BYTE_L(_debug_out2[j]);
		
		_canmsg.CAN_data[4] = BYTE_H(_debug_out3[j]);
		_canmsg.CAN_data[5] = BYTE_L(_debug_out3[j]);
		_canmsg.CAN_data[6] = BYTE_H(_debug_out4[j]);
		_canmsg.CAN_data[7] = BYTE_L(_debug_out4[j]);
			
		_canmsg.CAN_length = 8;
		_canmsg.CAN_frameType = DATA_FRAME;
		CAN1_send (_canmsg.CAN_messID, _canmsg.CAN_frameType, _canmsg.CAN_length, _canmsg.CAN_data);		
		count =0;
	}
	count++;
}

/*********************************************************** 
 * send broadcast messages according to mask 
 ***********************************************************/
void can_send_broadcast(void)
{
	int iretval,k; 
	
	static byte old_control_mode[JN] = INIT_ARRAY (MODE_IDLE);
	
	bool sendA      = false;
	bool sendB      = false;
	unsigned char FAULT0     = 0;
	unsigned char  FAULT1     = 0;
	unsigned char  FAULT2     = 0;
	unsigned char  FAULT3     = 0;
	unsigned char  FAULT_ABS0 = 0;
	unsigned char  FAULT_ABS1 = 0;
	unsigned char  FAULT_HLL0 = 0;
	unsigned char  FAULT_HLL1 = 0;
	unsigned char  FAULT_OPTICAL0 = 0;
	unsigned char  FAULT_OPTICAL1 = 0;
	
	// Absolute encoder errors			
	#if (CURRENT_BOARD_TYPE == BOARD_TYPE_BLL) || (CURRENT_BOARD_TYPE == BOARD_TYPE_2BLLDC)
	#ifdef USE_ABS_SSI	

		if (get_error_abs_ssi(0)==ERR_ABS_SSI) FAULT_ABS0 = 1;
		if (get_error_abs_ssi(1)==ERR_ABS_SSI) FAULT_ABS1 = 1;
	#endif		
	#endif

	// Hall effect errors
	#if (CURRENT_BOARD_TYPE == BOARD_TYPE_BLL)	
	#ifdef USE_HALL_SENS
		if (getHallStatus(0))
		{
			FAULT_HLL0 |= ((getHallStatus(0) &0x1)<<4);
			FAULT_HLL0 |= ((getHallStatus(0) &0x2)<<5);
			FAULT_HLL0 |= ((getHallStatus(0) &0x4)<<5);
			setHallStatus(0,0);
		}	
		if (getHallStatus(1))
		{
			FAULT_HLL1 |= ((getHallStatus(1) &0x1)<<4);
			FAULT_HLL1 |= ((getHallStatus(1) &0x2)<<5);
			FAULT_HLL1 |= ((getHallStatus(1) &0x4)<<5);
			setHallStatus(1,0);
		}
	#endif
	#endif
	
	// External, Overload, Undervoltage Faults
	_fault[0] = getReg (PWMA_PMFSA);	
	setReg(PWMA_PMFSA,	0x55);
	_fault[1] = getReg (PWMB_PMFSA);
	setReg(PWMB_PMFSA,	0x55);
	#if (CURRENT_BOARD_TYPE != BOARD_TYPE_4DC) 
		FAULT0 = (unsigned char )((_fault[0]>>8) & 0x01)  | ((_fault[0]>>9) & 0x02) | ((_fault[0]>>10) & 0x04) | ((_fault[0]>>12) & 0x04);				
		FAULT1 = (unsigned char )((_fault[1]>>8) & 0x01)  | ((_fault[1]>>9) & 0x02) | ((_fault[1]>>10) & 0x04) | ((_fault[1]>>12) & 0x04);

	#else
		FAULT0 = (unsigned char )((_fault[0]>>8) & 0x01)  | ((_fault[0]>>9)  & 0x02) | ((_fault[0]>>12) & 0x04);	
		FAULT1 = (unsigned char )((_fault[0]>>8) & 0x01)  | ((_fault[0]>>11) & 0x02) | ((_fault[0]>>12) & 0x04);
		FAULT2 = (unsigned char )((_fault[1]>>8) & 0x01)  | ((_fault[1]>>9)  & 0x02) | ((_fault[1]>>12) & 0x04);						
		FAULT3 = (unsigned char )((_fault[1]>>8) & 0x01)  | ((_fault[1]>>11) & 0x02) | ((_fault[1]>>12) & 0x04);
	#endif

	// Updates						
	_countBoardStatus[0]++;
	_countBoardStatus[1]++;			
				
	if (!broadcast_mask[0])
		return;
	
	// **************************************************************************/
	// * 						    BROADCAST POSITION                          */
	// **************************************************************************/
	
	if ((broadcast_mask[0] & (1<<(CAN_BCAST_POSITION-1))) && _counter == 0)
	{		
		_canmsg.CAN_messID = 0x100;
		_canmsg.CAN_messID |= (_board_ID) << 4;
		_canmsg.CAN_messID |= CAN_BCAST_POSITION;

		_canmsg.CAN_data[0] = BYTE_4(_position[0]);
		_canmsg.CAN_data[1] = BYTE_3(_position[0]);
		_canmsg.CAN_data[2] = BYTE_2(_position[0]);
		_canmsg.CAN_data[3] = BYTE_1(_position[0]);
			
		_canmsg.CAN_data[4] = BYTE_4(_position[1]);
		_canmsg.CAN_data[5] = BYTE_3(_position[1]);
		_canmsg.CAN_data[6] = BYTE_2(_position[1]);
		_canmsg.CAN_data[7] = BYTE_1(_position[1]);	
		
		_canmsg.CAN_length = 8;
		_canmsg.CAN_frameType = DATA_FRAME;

#if (VERSION ==0x0154 || VERSION ==0x0155 || VERSION ==0x0158 || VERSION == 0x0351 || VERSION ==0x0151 || VERSION ==0x0251 || VERSION==0x0254 )
		if(FAULT_ABS0 == 0)
			CAN1_send (_canmsg.CAN_messID, _canmsg.CAN_frameType, _canmsg.CAN_length, _canmsg.CAN_data);
#elif (CURRENT_BOARD_TYPE == BOARD_TYPE_BLL) || (CURRENT_BOARD_TYPE == BOARD_TYPE_2BLLDC)
		#ifdef USE_ABS_SSI
		if(FAULT_ABS0 == 0 && FAULT_ABS1 == 0)
			CAN1_send (_canmsg.CAN_messID, _canmsg.CAN_frameType, _canmsg.CAN_length, _canmsg.CAN_data);
		#endif
#else
			CAN1_send (_canmsg.CAN_messID, _canmsg.CAN_frameType, _canmsg.CAN_length, _canmsg.CAN_data);
#endif
	
	}
	if (JN==4 && (broadcast_mask[1] & (1<<(CAN_BCAST_POSITION-1))) && _counter == 0)
	{
			
			_canmsg.CAN_messID = 0x100;
			_canmsg.CAN_messID |= (_board_ID+1) << 4;
			_canmsg.CAN_messID |= CAN_BCAST_POSITION;

			_canmsg.CAN_data[0] = BYTE_4(_position[2]);
			_canmsg.CAN_data[1] = BYTE_3(_position[2]);
			_canmsg.CAN_data[2] = BYTE_2(_position[2]);
			_canmsg.CAN_data[3] = BYTE_1(_position[2]);
			_canmsg.CAN_data[4] = BYTE_4(_position[3]);
			_canmsg.CAN_data[5] = BYTE_3(_position[3]);
			_canmsg.CAN_data[6] = BYTE_2(_position[3]);
			_canmsg.CAN_data[7] = BYTE_1(_position[3]);
				
			_canmsg.CAN_length = 8;
			_canmsg.CAN_frameType = DATA_FRAME;

			CAN1_send(_canmsg.CAN_messID, _canmsg.CAN_frameType, _canmsg.CAN_length, _canmsg.CAN_data);

			
	}
	
	// **************************************************************************/
	// * 						    BROADCAST PID OUTPUT                        */
	// **************************************************************************/
	if ((broadcast_mask[0] & (1<<(CAN_BCAST_PID_VAL-1)) )  && _counter == 1)
	{
		/* CHANGED: send PID (control) value */
		_canmsg.CAN_messID = 0x100;
		_canmsg.CAN_messID |= (_board_ID) << 4;
		_canmsg.CAN_messID |= CAN_BCAST_PID_VAL;

		_canmsg.CAN_data[0] = BYTE_H(_pid[0]);
		_canmsg.CAN_data[1] = BYTE_L(_pid[0]);
		_canmsg.CAN_data[2] = BYTE_H(_pid[1]);
		_canmsg.CAN_data[3] = BYTE_L(_pid[1]);
		_canmsg.CAN_data[4] = 0;
		_canmsg.CAN_data[5] = 0;
		_canmsg.CAN_data[6] = 0;
		_canmsg.CAN_data[7] = 0;

#ifdef USE_NEW_DECOUPLING
		_canmsg.CAN_data[4] = BYTE_H((Int16)(_bfc_PWMoutput[0]));
		_canmsg.CAN_data[5] = BYTE_L((Int16)(_bfc_PWMoutput[0]));
		_canmsg.CAN_data[6] = BYTE_H((Int16)(_bfc_PWMoutput[1]));
		_canmsg.CAN_data[7] = BYTE_L((Int16)(_bfc_PWMoutput[1]));	
#else	
#if (CURRENT_BOARD_TYPE != BOARD_TYPE_4DC)		
		if (DutyCycle[0].Dir==0)
		{
		    _canmsg.CAN_data[4] = BYTE_H((Int16)DutyCycle[0].Duty);
			_canmsg.CAN_data[5] = BYTE_L((Int16)DutyCycle[0].Duty);
	    }
		else
		{
			_canmsg.CAN_data[4] = BYTE_H(-1*(Int16)DutyCycle[0].Duty);
			_canmsg.CAN_data[5] = BYTE_L(-1*(Int16)DutyCycle[0].Duty);
		}
		if (DutyCycle[1].Dir==0)
		{
		    _canmsg.CAN_data[6] = BYTE_H((Int16)DutyCycle[1].Duty);
			_canmsg.CAN_data[7] = BYTE_L((Int16)DutyCycle[1].Duty);	
	    }
		else
		{
			_canmsg.CAN_data[6] = BYTE_H(-1*(Int16)DutyCycle[1].Duty);
			_canmsg.CAN_data[7] = BYTE_L(-1*(Int16)DutyCycle[1].Duty);
		}
#endif
#endif

		_canmsg.CAN_length = 8;
		_canmsg.CAN_frameType = DATA_FRAME;
		CAN1_send (_canmsg.CAN_messID, _canmsg.CAN_frameType, _canmsg.CAN_length, _canmsg.CAN_data);
	
	}
	if (JN==4 && (broadcast_mask[1] & (1<<(CAN_BCAST_PID_VAL-1))) && _counter == 1)
	{	
			_canmsg.CAN_messID = 0x100;
			_canmsg.CAN_messID |= (_board_ID+1) << 4;
			_canmsg.CAN_messID |= CAN_BCAST_PID_VAL;

			_canmsg.CAN_data[0] = BYTE_H(_pid[2]);
			_canmsg.CAN_data[1] = BYTE_L(_pid[2]);
			_canmsg.CAN_data[2] = BYTE_H(_pid[3]);
			_canmsg.CAN_data[3] = BYTE_L(_pid[3]);
			_canmsg.CAN_data[4] = 0;
			_canmsg.CAN_data[5] = 0;
			_canmsg.CAN_data[6] = 0;
			_canmsg.CAN_data[7] = 0;	
					
			_canmsg.CAN_length = 8;
			_canmsg.CAN_frameType = DATA_FRAME;

		CAN1_send(_canmsg.CAN_messID, _canmsg.CAN_frameType, _canmsg.CAN_length, _canmsg.CAN_data);
		
			
	}	
	
	// **************************************************************************/
	// * 						    BROADCAST FAULTS/STATUS                     */
	// **************************************************************************/
	
	if ((broadcast_mask[0] & (1<<(CAN_BCAST_STATUS-1))) && _counter == 2)
	{
		for (k=0;k<8;k++)
		{
			_canmsg.CAN_data[k] = 0;
		}
		/* if a new fault is detected then sends a message */
		sendA = false;
		
		//  --- undervoltage overload external fault axis 0 ---
		_canmsg.CAN_data[0]= FAULT0;

	   	//  --- HIGH CURRENT CH 0---
		if (highcurrent[0])
		{
			_canmsg.CAN_data[0] |=highcurrent[0]<<3;
			highcurrent[0]=false;
			sendA=true;
		}

		// Hall fault
		if (FAULT_HLL0)
		{
			_canmsg.CAN_data[0] |= FAULT_HLL0;
			sendA=true;
		}
		
		//Absolute Encoder 0 Reading Error
		if (FAULT_ABS0)
		{
			_canmsg.CAN_data[0] |=0x20;	
			sendA=true;
		}
	
		//Optical Encoder drift detected
		if (FAULT_OPTICAL0)
		{
			_canmsg.CAN_data[0] |=0x40;
			sendA=true;
		}
	
		//  --- Control Mode axes 0 ---
		_canmsg.CAN_data[1]= _control_mode[0];	
		if (_control_mode[0] != old_control_mode[0])
		{
			old_control_mode[0]=_control_mode[0];
			sendA = true;
		}
		
		//  --- undervoltage overload external fault axis 1 ---		
		_canmsg.CAN_data[2]= FAULT1;

		//  --- HIGH CURRENT CH 1---
	#if (VERSION !=0x0154) && (VERSION !=0x0155) && (VERSION !=0x0158) && (VERSION !=0x0254)
		if (highcurrent[1])
		{
			_canmsg.CAN_data[2] |=highcurrent[1]<<3;
			highcurrent[1]=false;
			sendA=true;
		}
		// Hall fault
		if (FAULT_HLL1)
		{
			_canmsg.CAN_data[2] |= FAULT_HLL1;
			sendA=true;
		}
		
		//Absolute Encoder 1 Reading Error
		if (FAULT_ABS1)
		{
			_canmsg.CAN_data[2] |=0x20;	
			sendA=true;
		}
		
		//Optical Encoder drift detected
		if (FAULT_OPTICAL1)
		{
			_canmsg.CAN_data[2] |=0x40;
			sendA=true;
		}
		
	#endif
		//  --- Control Mode axes 1 ---
		_canmsg.CAN_data[3]= _control_mode[1]; 		
		if (_control_mode[1] != old_control_mode[1])
		{
			old_control_mode[1]=_control_mode[1];
			sendA = true;
		}
				
		//CANBUS ERRORS
		_canmsg.CAN_data[4] = getCanStatus();
		//MAIN LOOP overflow
		_canmsg.CAN_data[5] |=  mainLoopOVF;
		// TEMPERATURE
		#ifdef TEMPERATURE_SENSOR
			_canmsg.CAN_data[5] |= overtemp[0]<<1;
			_canmsg.CAN_data[5] |= overtemp[1]<<2;
			_canmsg.CAN_data[5] |= errortemp[0]<<3;
			_canmsg.CAN_data[5] |= errortemp[1]<<4;
			
			//Temperature Measurement 1 
		//	_canmsg.CAN_data[6]=BYTE_L(TempSens[0]);
			//Temperature Measurement 2
		//	_canmsg.CAN_data[7]=BYTE_L(TempSens[1]);		
		#endif
		
		if ((_canmsg.CAN_data[0] &0x7) ) //the first 3 bits
		{
			#ifdef DEBUG_CAN_MSG	
			can_printf("AXIS 0 ERROR %d", (UInt8) _canmsg.CAN_data[0]);	
			#endif
			sendA = true;
		}
		if ((_canmsg.CAN_data[2] &0x7) ) //the first 3 bits
		{
			#ifdef DEBUG_CAN_MSG	
			can_printf("AXIS 1 ERROR %d", (UInt8) _canmsg.CAN_data[2]);	
			#endif
			sendA = true;
		}
		if (_canmsg.CAN_data[4] & 0xFD)
		{
			setCanStatus(0);
			#ifdef DEBUG_CAN_MSG	
			can_printf("CANR%d", (UInt8)_canmsg.CAN_data[4]);	
			#endif
			sendA=true;	
		} 
		if ((_canmsg.CAN_data[5] & 0x7)) //the first 3 bits
		{	
			if (mainLoopOVF) mainLoopOVF=0;
			sendA=true;
		}

		// TO BE SEND EVERY BStatusTime  ms
		if (_countBoardStatus[0]>=BOARDSTATUSTIME) 		
		{
			_countBoardStatus[0]=0;
			sendA=true;
			_canmsg.CAN_data[6]=getTxError();
   			_canmsg.CAN_data[7]=getRxError();
			setTxError();
			setRxError();
		}
		
		// if new fault, send message.		
		if (sendA)
		{
			_canmsg.CAN_messID = 0x100;
			_canmsg.CAN_messID |= (_board_ID) << 4;
			_canmsg.CAN_messID |= CAN_BCAST_STATUS;	
			_canmsg.CAN_length = 8;
			_canmsg.CAN_frameType = DATA_FRAME;
			CAN1_send (_canmsg.CAN_messID, _canmsg.CAN_frameType, _canmsg.CAN_length, _canmsg.CAN_data);
		}
	}
	if ((JN == 4) && (broadcast_mask[1] & (1<<(CAN_BCAST_STATUS-1))) && _counter == 2)
	{
		/* if a new fault is detected then sends a message */
		sendB = false;
		for (k=0;k<8;k++)
		{
			_canmsg.CAN_data[k] = 0;
		}
		
		//  --- undervoltage overload external fault axis 2 e 3 ---
		_canmsg.CAN_data[0]= FAULT2;						
		_canmsg.CAN_data[2]= FAULT3;

		//  --- HIGH CURRENT CH 2 ---
		if (highcurrent[2])
		{
			_canmsg.CAN_data[0] |=highcurrent[2]<<3;
			sendB=true;
		} 
		//  --- HIGH CURRENT CH 3 ---
		if (highcurrent[3])
		{
			_canmsg.CAN_data[2] |=highcurrent[3]<<3;
			sendB=true;
		}
		//  --- Control Mode axes 2 ---
		_canmsg.CAN_data[1]= _control_mode[2];	
		if (_control_mode[2] != old_control_mode[2])
		{
			sendB = true;
			old_control_mode[2]=_control_mode[2];
		}
		
		//  --- Control Mode axes 3 ---
		_canmsg.CAN_data[3]= _control_mode[3]; 		
		if (_control_mode[3] != old_control_mode[3])
		{
			sendB = true;
			old_control_mode[3]=_control_mode[3];
		}

		//CANBUS ERRORS
		_canmsg.CAN_data[4] = getCanStatus();
		//MAIN LOOP overflow
		_canmsg.CAN_data[5] |= mainLoopOVF;
		
		if ((_canmsg.CAN_data[0] &0x7) ) //the first 3 bits
		{
			#ifdef DEBUG_CAN_MSG	
			can_printf("AXIS 0 ERROR %d", (UInt8) _canmsg.CAN_data[0]);	
			#endif
			sendB = true;
		}
		if ((_canmsg.CAN_data[2] &0x7) ) //the first 3 bits
		{
			#ifdef DEBUG_CAN_MSG	
			can_printf("AXIS 1 ERROR %d", (UInt8) _canmsg.CAN_data[2]);	
			#endif
			sendB = true;
		}
		if (_canmsg.CAN_data[4])
		{
			setCanStatus(0); //reset of the can tx status
			#ifdef DEBUG_CAN_MSG	
			can_printf("CAN_ERR %d", (UInt8)_canmsg.CAN_data[4]);	
			#endif
			sendB =true;	
		} 
		if (_canmsg.CAN_data[5] & 0x7) //the first 3 bits
		{	
			#ifdef DEBUG_CAN_MSG	
			can_printf("BOARD STATUS ERROR %d", (UInt8)_canmsg.CAN_data[5]);	
			#endif	
			if (mainLoopOVF) mainLoopOVF=0;
			sendB =true;
		}
		// TO BE SEND EVERY BStatusTime  ms
		if (_countBoardStatus[1]>=BOARDSTATUSTIME) 		
		{
			_countBoardStatus[1]=0;
			_canmsg.CAN_data[6]=getTxError();
	    	_canmsg.CAN_data[7]=getRxError();
			setTxError();
			setRxError();	
			sendB=true;   
		}
		
		if (sendB)
		{
			_canmsg.CAN_messID = 0x100;
			_canmsg.CAN_messID |= (_board_ID+1) << 4;
			_canmsg.CAN_messID |= CAN_BCAST_STATUS;
			
			_canmsg.CAN_length = 8;
			_canmsg.CAN_frameType = DATA_FRAME;
			CAN1_send(_canmsg.CAN_messID, _canmsg.CAN_frameType, _canmsg.CAN_length, _canmsg.CAN_data);

		}
	}	
	
	// **************************************************************************/
	// * 						    BROADCAST CURRENT                           */
	// **************************************************************************/
	
	if ((broadcast_mask[0] & (1<<(CAN_BCAST_CURRENT-1)))  && _counter == 3)
	{
		_canmsg.CAN_messID = 0x100;
		_canmsg.CAN_messID |= (_board_ID) << 4;
		_canmsg.CAN_messID |= CAN_BCAST_CURRENT;
 
		_canmsg.CAN_data[0] = BYTE_H(get_current(0));
		_canmsg.CAN_data[1] = BYTE_L(get_current(0));
		_canmsg.CAN_data[2] = BYTE_H(get_current(1));
		_canmsg.CAN_data[3] = BYTE_L(get_current(1));
	
		_canmsg.CAN_data[4] = BYTE_H(0);
		_canmsg.CAN_data[5] = BYTE_L(0);
		_canmsg.CAN_data[6] = BYTE_H(0);
		_canmsg.CAN_data[7] = BYTE_L(0);
	
		_canmsg.CAN_length = 8;
		_canmsg.CAN_frameType = DATA_FRAME;
		CAN1_send (_canmsg.CAN_messID, _canmsg.CAN_frameType, _canmsg.CAN_length, _canmsg.CAN_data);	
	}
	if (JN== 4 && (broadcast_mask[1] & (1<<(CAN_BCAST_CURRENT-1))) && _counter == 3)
	{
		_canmsg.CAN_messID = 0x100;
		_canmsg.CAN_messID |= (_board_ID+1) << 4;
		_canmsg.CAN_messID |= CAN_BCAST_CURRENT;

		_canmsg.CAN_data[0] = BYTE_H(get_current(2));
		_canmsg.CAN_data[1] = BYTE_L(get_current(2));
		_canmsg.CAN_data[2] = BYTE_H(get_current(3));
		_canmsg.CAN_data[3] = BYTE_L(get_current(3));
		
		_canmsg.CAN_data[4] = BYTE_H(0);
		_canmsg.CAN_data[5] = BYTE_L(0);
		_canmsg.CAN_data[6] = BYTE_H(0);
		_canmsg.CAN_data[7] = BYTE_L(0);
			
		_canmsg.CAN_length = 8;
		_canmsg.CAN_frameType = DATA_FRAME;

		CAN1_send(_canmsg.CAN_messID, _canmsg.CAN_frameType, _canmsg.CAN_length, _canmsg.CAN_data);
	}

	// **************************************************************************/
	// * 						    BROADCAST PID_ERROR                         */
	// **************************************************************************/
	
	if ((broadcast_mask[0] & (1<<(CAN_BCAST_PID_ERROR-1)))  && _counter == 3) //same counter of CAN_BCAST_CURRENT
	{
		_canmsg.CAN_messID = 0x100;
		_canmsg.CAN_messID |= (_board_ID) << 4;
		_canmsg.CAN_messID |= CAN_BCAST_PID_ERROR;
 
		_canmsg.CAN_data[0] = BYTE_H(_error_position[0]);
		_canmsg.CAN_data[1] = BYTE_L(_error_position[0]);
		_canmsg.CAN_data[2] = BYTE_H(_error_position[1]);
		_canmsg.CAN_data[3] = BYTE_L(_error_position[1]);
		
#if (VERSION ==0x0351)
		//iKart only
		_canmsg.CAN_data[0] = BYTE_H(_error_speed[0]);
		_canmsg.CAN_data[1] = BYTE_L(_error_speed[0]);
		_canmsg.CAN_data[2] = BYTE_H(_error_speed[1]);
		_canmsg.CAN_data[3] = BYTE_L(_error_speed[1]);
#endif

		_canmsg.CAN_data[4] = BYTE_H(_error_torque[0]);
		_canmsg.CAN_data[5] = BYTE_L(_error_torque[0]);
		_canmsg.CAN_data[6] = BYTE_H(_error_torque[1]);
		_canmsg.CAN_data[7] = BYTE_L(_error_torque[1]);

		//_canmsg.CAN_data[0] = BYTE_H(_cpl_pid_prediction[0]);  //$$$ DEBUG ONLY!!!
		//_canmsg.CAN_data[1] = BYTE_L(_cpl_pid_prediction[0]);  //$$$ DEBUG ONLY!!!
		//_canmsg.CAN_data[2] = BYTE_H(_cpl_pid_prediction[1]);  //$$$ DEBUG ONLY!!!
		//_canmsg.CAN_data[3] = BYTE_L(_cpl_pid_prediction[1]);  //$$$ DEBUG ONLY!!!
		//_canmsg.CAN_data[4] = BYTE_H((Int16)(_bfc_PWMoutput[0]));  //$$$ DEBUG ONLY!!!
		//_canmsg.CAN_data[5] = BYTE_L((Int16)(_bfc_PWMoutput[0]));  //$$$ DEBUG ONLY!!!
		//_canmsg.CAN_data[6] = BYTE_H((Int16)(_bfc_PWMoutput[1]));  //$$$ DEBUG ONLY!!!
		//_canmsg.CAN_data[7] = BYTE_L((Int16)(_bfc_PWMoutput[1]));  //$$$ DEBUG ONLY!!!	
						
		_canmsg.CAN_length = 8;
		_canmsg.CAN_frameType = DATA_FRAME;
		CAN1_send (_canmsg.CAN_messID, _canmsg.CAN_frameType, _canmsg.CAN_length, _canmsg.CAN_data);	
	}
	if (JN==4 && (broadcast_mask[1] & (1<<(CAN_BCAST_PID_ERROR-1))) && _counter == 3) //same counter of CAN_BCAST_CURRENT
	{
			_canmsg.CAN_messID = 0x100;
			_canmsg.CAN_messID |= (_board_ID+1) << 4;
			_canmsg.CAN_messID |= CAN_BCAST_PID_ERROR;

			_canmsg.CAN_data[0] = BYTE_H(_error_position[2]);
			_canmsg.CAN_data[1] = BYTE_L(_error_position[2]);
			_canmsg.CAN_data[2] = BYTE_H(_error_position[3]);
			_canmsg.CAN_data[3] = BYTE_L(_error_position[3]);
			
			_canmsg.CAN_data[4] = BYTE_H(_error_torque[2]);
			_canmsg.CAN_data[5] = BYTE_L(_error_torque[2]);
			_canmsg.CAN_data[6] = BYTE_H(_error_torque[3]);
			_canmsg.CAN_data[7] = BYTE_L(_error_torque[3]);
				
			_canmsg.CAN_length = 8;
			_canmsg.CAN_frameType = DATA_FRAME;

			CAN1_send(_canmsg.CAN_messID, _canmsg.CAN_frameType, _canmsg.CAN_length, _canmsg.CAN_data);
	
	}

	// **************************************************************************/
	// * 						    BROADCAST VELOCITY                          */
	// **************************************************************************/
	
//  @@@RANDAZ: important: CAN_SYNCHRO_STEP has been reduced from 5 to 4 because of CAN_SET_ACTIVE_PID_HANDLER()	
//	if ((broadcast_mask[0] & (1<<(CAN_BCAST_VELOCITY-1))) && _counter == 4) 
	if ((broadcast_mask[0] & (1<<(CAN_BCAST_VELOCITY-1))) && _counter == 3)
	{
		_canmsg.CAN_messID = 0x100;
		_canmsg.CAN_messID |= (_board_ID) << 4;
		_canmsg.CAN_messID |= CAN_BCAST_VELOCITY;

		_canmsg.CAN_data[0] = BYTE_H(_speed[0]);
		_canmsg.CAN_data[1] = BYTE_L(_speed[0]);
		_canmsg.CAN_data[2] = BYTE_H(_speed[1]);
		_canmsg.CAN_data[3] = BYTE_L(_speed[1]);
		
		_canmsg.CAN_data[4] = BYTE_H(_accel[0]);
		_canmsg.CAN_data[5] = BYTE_L(_accel[0]);
		_canmsg.CAN_data[6] = BYTE_H(_accel[1]);
		_canmsg.CAN_data[7] = BYTE_L(_accel[1]);
			
		_canmsg.CAN_length = 8;
		_canmsg.CAN_frameType = DATA_FRAME;
		CAN1_send (_canmsg.CAN_messID, _canmsg.CAN_frameType, _canmsg.CAN_length, _canmsg.CAN_data);	

	}
	if (JN == 4 && (broadcast_mask[1] & (1<<(CAN_BCAST_VELOCITY-1))) && _counter == 4)
	{
		_canmsg.CAN_messID = 0x100;
		_canmsg.CAN_messID |= (_board_ID+1) << 4;
		_canmsg.CAN_messID |= CAN_BCAST_VELOCITY;

		_canmsg.CAN_data[0] = BYTE_H(_speed[2]);
		_canmsg.CAN_data[1] = BYTE_L(_speed[2]);
		_canmsg.CAN_data[2] = BYTE_H(_speed[3]);
		_canmsg.CAN_data[3] = BYTE_L(_speed[3]);
		
		_canmsg.CAN_data[4] = BYTE_H(_accel[2]);
		_canmsg.CAN_data[5] = BYTE_L(_accel[2]);
		_canmsg.CAN_data[6] = BYTE_H(_accel[3]);
		_canmsg.CAN_data[7] = BYTE_L(_accel[3]);
			
		_canmsg.CAN_length = 8;
		_canmsg.CAN_frameType = DATA_FRAME;
		CAN1_sendFrame (1, _canmsg.CAN_messID, _canmsg.CAN_frameType, _canmsg.CAN_length, _canmsg.CAN_data);
		
	}
	
	// **************************************************************************/
	// * 						    BROADCAST MOTOR POSITION                    */
	// **************************************************************************/
	
//  @@@RANDAZ: important: CAN_SYNCHRO_STEP has been reduced from 5 to 4 because of CAN_SET_ACTIVE_PID_HANDLER()	
 
	if ((broadcast_mask[0] & (1<<(CAN_BCAST_MOTOR_POSITION-1))) && _counter == 3)
	{
		_canmsg.CAN_messID = 0x100;
		_canmsg.CAN_messID |= (_board_ID) << 4;
		_canmsg.CAN_messID |= CAN_BCAST_MOTOR_POSITION;

		_canmsg.CAN_data[0] = BYTE_4(_motor_position[0]);
		_canmsg.CAN_data[1] = BYTE_3(_motor_position[0]);
		_canmsg.CAN_data[2] = BYTE_2(_motor_position[0]);
		_canmsg.CAN_data[3] = BYTE_1(_motor_position[0]);
		
		_canmsg.CAN_data[4] = BYTE_4(_motor_position[1]);
		_canmsg.CAN_data[5] = BYTE_3(_motor_position[1]);
		_canmsg.CAN_data[6] = BYTE_2(_motor_position[1]);
		_canmsg.CAN_data[7] = BYTE_1(_motor_position[1]);
		
			
		_canmsg.CAN_length = 8;
		_canmsg.CAN_frameType = DATA_FRAME;
		CAN1_send (_canmsg.CAN_messID, _canmsg.CAN_frameType, _canmsg.CAN_length, _canmsg.CAN_data);	

	}
	if (JN == 4 && (broadcast_mask[1] & (1<<(CAN_BCAST_MOTOR_POSITION-1))) && _counter == 4)
	{
		_canmsg.CAN_messID = 0x100;
		_canmsg.CAN_messID |= (_board_ID+1) << 4;
		_canmsg.CAN_messID |= CAN_BCAST_MOTOR_POSITION;

		_canmsg.CAN_data[0] = BYTE_4(_motor_position[0]);
		_canmsg.CAN_data[1] = BYTE_3(_motor_position[0]);
		_canmsg.CAN_data[2] = BYTE_2(_motor_position[0]);
		_canmsg.CAN_data[3] = BYTE_1(_motor_position[0]);
		
		_canmsg.CAN_data[4] = BYTE_4(_motor_position[1]);
		_canmsg.CAN_data[5] = BYTE_3(_motor_position[1]);
		_canmsg.CAN_data[6] = BYTE_2(_motor_position[1]);
		_canmsg.CAN_data[7] = BYTE_1(_motor_position[1]);
		
			
		_canmsg.CAN_length = 8;
		_canmsg.CAN_frameType = DATA_FRAME;
		CAN1_sendFrame (1, _canmsg.CAN_messID, _canmsg.CAN_frameType, _canmsg.CAN_length, _canmsg.CAN_data);
		
	}
		
	// **************************************************************************/
	// * 						    BROADCAST MOTOR SPEED                       */
	// **************************************************************************/
	
//  @@@RANDAZ: important: CAN_SYNCHRO_STEP has been reduced from 5 to 4 because of CAN_SET_ACTIVE_PID_HANDLER()	
 
	if ((broadcast_mask[0] & (1<<(CAN_BCAST_MOTOR_SPEED-1))) && _counter == 3)
	{
		Int16 casted_motor_speed0 = (Int16)(_motor_speed[0]);
		Int16 casted_motor_speed1 = (Int16)(_motor_speed[1]);
		_canmsg.CAN_messID = 0x100;
		_canmsg.CAN_messID |= (_board_ID) << 4;
		_canmsg.CAN_messID |= CAN_BCAST_MOTOR_SPEED;

		_canmsg.CAN_data[0] = BYTE_H(casted_motor_speed0);
		_canmsg.CAN_data[1] = BYTE_L(casted_motor_speed0);
		_canmsg.CAN_data[2] = BYTE_H(casted_motor_speed1);
		_canmsg.CAN_data[3] = BYTE_L(casted_motor_speed1);
		_canmsg.CAN_data[4] = BYTE_H(0);
		_canmsg.CAN_data[5] = BYTE_L(0);
		_canmsg.CAN_data[6] = BYTE_H(0);
		_canmsg.CAN_data[7] = BYTE_L(0);				
			
		_canmsg.CAN_length = 8;
		_canmsg.CAN_frameType = DATA_FRAME;
		CAN1_send (_canmsg.CAN_messID, _canmsg.CAN_frameType, _canmsg.CAN_length, _canmsg.CAN_data);	

	}
	if (JN == 4 && (broadcast_mask[1] & (1<<(CAN_BCAST_MOTOR_SPEED-1))) && _counter == 4)
	{
		Int16 casted_motor_speed2 = (Int16)(_motor_speed[2]);
		Int16 casted_motor_speed3 = (Int16)(_motor_speed[3]);
		_canmsg.CAN_messID = 0x100;
		_canmsg.CAN_messID |= (_board_ID+1) << 4;
		_canmsg.CAN_messID |= CAN_BCAST_MOTOR_SPEED;

		_canmsg.CAN_data[0] = BYTE_H(casted_motor_speed2);
		_canmsg.CAN_data[1] = BYTE_L(casted_motor_speed2);
		_canmsg.CAN_data[2] = BYTE_H(casted_motor_speed3);
		_canmsg.CAN_data[3] = BYTE_L(casted_motor_speed3);
		_canmsg.CAN_data[4] = BYTE_H(0);
		_canmsg.CAN_data[5] = BYTE_L(0);
		_canmsg.CAN_data[6] = BYTE_H(0);
		_canmsg.CAN_data[7] = BYTE_L(0);		
			
		_canmsg.CAN_length = 8;
		_canmsg.CAN_frameType = DATA_FRAME;
		CAN1_sendFrame (1, _canmsg.CAN_messID, _canmsg.CAN_frameType, _canmsg.CAN_length, _canmsg.CAN_data);
		
	}
}


/**************************************************************
Set Can Mask
**************************************************************/
void set_can_masks()
{
	UInt32 filter1=0;
	UInt32 filter2=0;
	UInt32 mask1=0;
	UInt32 mask2=0;
	
	#if (VERSION == 0x0153 || VERSION==0x0157 || VERSION==0x0150 || VERSION==0x0147 || VERSION==0x0140 || VERSION==0x0250 || VERSION==0x0257)
		{	
			create_F_M(&filter1, &mask1,CLASS_POLLING_DSP,0xFF, _board_ID, CLASS_PERIODIC_SENS, 0xFF, 0xFF);   
			create_F_M(&filter2, &mask2,CLASS_CANLOADER,  0x00, 0xFF, 	   CLASS_PERIODIC_DSP, 	CAN_ID_COUPLED_BOARD, 0xFF);  		
		}
	#elif (VERSION==0x0351)
		if (_board_ID == 1)
		{	
			create_F_M(&filter1, &mask1,CLASS_POLLING_DSP,0xFF, _board_ID, CLASS_PERIODIC_SENS, 0xFF, 0xFF);   
			create_F_M(&filter2, &mask2,CLASS_CANLOADER,  0x00, 0xFF, 	   CLASS_PERIODIC_DSP, 	2, 0xFF);  		
		}
		else if (_board_ID == 2)
		{	
			create_F_M(&filter1, &mask1,CLASS_POLLING_DSP,0xFF, _board_ID, CLASS_PERIODIC_SENS, 0xFF, 0xFF);   
			create_F_M(&filter2, &mask2,CLASS_CANLOADER,  0x00, 0xFF, 	   CLASS_PERIODIC_DSP, 	1, 0xFF);  		
		}			
	#elif (VERSION == 0x0119 || VERSION == 0x0121 || VERSION == 0x0128 || VERSION == 0x0130)
		{
			create_F_M(&filter1, &mask1,CLASS_POLLING_DSP,0xFF, _board_ID  , CLASS_PERIODIC_SENS, 0xFF, 0xFF);   
			create_F_M(&filter2, &mask2,CLASS_POLLING_DSP,0xFF, _board_ID+1, CLASS_CANLOADER,     0x00, 0xFF);  
		}
	#else
		{
		#if   (JN == 2)
			create_F_M(&filter1, &mask1,CLASS_POLLING_DSP,0xFF, _board_ID, CLASS_PERIODIC_SENS, 0xFF, 0xFF);   
			create_F_M(&filter2, &mask2,CLASS_CANLOADER,  0x00, 0xFF, 	   CLASS_CANLOADER,  	0x00, 0xFF);  
		#elif (JN == 4)
			create_F_M(&filter1, &mask1,CLASS_POLLING_DSP,0xFF, _board_ID,   CLASS_CANLOADER,   0x00, 0xFF);
			create_F_M(&filter2, &mask2,CLASS_POLLING_DSP,0xFF, _board_ID+1, CLASS_CANLOADER,  	0x00, 0xFF);  
		#endif
		}
	#endif	
		
	setmask(filter1,filter2, mask1, mask2);
}

/*************************************************************************************/
/**
 * This method creates two 16 bits filters and two 16 bits masks for the CAN module
 * See CAN protocol.
 *
 * @param _class1 the first 3 bits of the CAN ID.  0xFF for don't care
 * @param _source1 source address in CAN ID.	   0xFF for don't care	
 * @param _dest1 destination address in CAN ID.    0xFF for don't care 
 * @param _class2 the first 3 bits of the CAN ID.  0xFF for don't care
 * @param _source2 source address in CAN ID.       0xFF for don't care
 * @param _dest2 destination address in CAN ID.    0xFF for don't care
 * @return set filter 32 bits filter, and mask 32 bits mask;
 *************************************************************************************/
void create_F_M(UInt32 *filter, UInt32 *mask, byte class1, byte source1, byte dest1, byte class2, byte source2, byte dest2)
{
	UInt16 mask1=0x1F; //the first 5 bits don't care in any case
	UInt16 mask2=0x1F; //the first 5 bits don't care in any case
	UInt16 filter1=0;
	UInt16 filter2=0;
	UInt16 temp=0;
	*filter=0;
	*mask=0;
	if (class1 == 0xFF) mask1 |=0xE000;
	class1 == 0xFF ? mask1 |=0xE000 : 0;
	source1 == 0xFF ? mask1 |=0x1E00 : 0;
	dest1 == 0xFF ? mask1 |=0x01E0 : 0; 
	
	class2 == 0xFF ? mask2 |=0xE000 : 0;
	source2 == 0xFF ? mask2 |=0x1E00 : 0;
	dest2 == 0xFF ? mask2 |=0x01E0 : 0;
	
	// devo mettere byte rovesciati;
	temp=mask1;
	mask1=(mask1<<8 &0xFF00) | (temp>>8 &0x00FF);
	temp=mask2;
	mask2=(mask2<<8 &0xFF00) | (temp>>8 &0x00FF);
	
	filter1  =(dest1 & 0xF)<<5;
	filter1 |=(source1 & 0xF)<<9;
	filter1 |=(class1 & 0x7)<<13;
	temp=filter1;
	filter1=(filter1<<8 &0xFF00) | (temp>>8 &0x00FF);
	
	filter2  =(dest2 & 0xF)<<5;
	filter2 |=(source2 & 0xF)<<9;
	filter2 |=(class2 & 0x7)<<13;
	temp=filter2;
	filter2=(filter2<<8 &0xFF00) | (temp>>8 &0x00FF);
	
	
	*filter=L_deposit_h(filter1) | filter2; //create filter and mask
	*mask=L_deposit_h(mask1) | mask2; 
}

/***********************************************************************/
/**
 * This method set the 4 16 bits filters and masks for the CAN module
 * See CAN protocol.
 *
 * @param filter1
 * @param filter2
 * @param mask1
 * @param mask2 
 ***********************************************************************/
void setmask(UInt32 filter1, UInt32 filter2, UInt32 mask1,UInt32 mask2)
{
	CAN1_setAcceptanceMask (mask1,mask2);
	CAN1_setAcceptanceCode (filter1,filter2);	
}

/***********************************************************************/
void can_send_additional_info()
{
	byte j = 0;
	byte write_buffer = 0;
    byte counter = 0;
    byte tout=0;
    for (counter = 0 ; counter < 8; counter++)
    {
		do 
		{
			tout++;
		}
		while (CAN1_getStateTX () == 0 && tout<=2000) ;
		if (tout!=2000 )
		{
			CAN_LEN = 6; 
			CAN_DATA[1]=counter;
			for (j=0; j<4; j++)	CAN_DATA[2+j] = (byte)_additional_info[j+counter*4]; 		
			CAN1_send (CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); 
		} 	
    }		
}
/***********************************************************************/
void can_receive_additional_info()
{
	byte value = 0; 
	byte j = 0;
	
	if (CAN_LEN == 6) 
	{ 
		value = CAN_DATA[1]; 
		
		for (j=0; j<4; j++)
			_additional_info[value*4+j] = (char) CAN_DATA[2+j]; 		
	}
}

