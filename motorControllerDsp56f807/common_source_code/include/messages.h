/*
 * messages.h
 *	macros for CAN bus message handling.
 *
 */
 
#ifndef __messagesh__
#define __messagesh__

/* error status values */
#define ERROR_NONE					0			/* no error, all ok */
#define ERROR_UNSPECIFIED			1			/* generic error */
#define ERROR_MODE					2			/* mode error, can't apply command in current mode */
#define ERROR_FMT					3			/* format error, command in wrong format */
#define ERROR_SEND					4			/* can't send answer back */

/**
 * it takes the existing header, swaps src and dest
 * leaves the channel number in place and the message
 * type. Doesn't change the priority either (3msb of 
 * the ID).
 */
#define PREPARE_HEADER\
{ \
	CAN_ID >>= 4; \
	CAN_ID &= 0xffffff0f; \
	if (axis <= 1) CAN_ID |= (_board_ID << 4); \
	else		   CAN_ID |= ((_board_ID+1) << 4); \
}


//-------------------------------------------------------------------
#define CAN_NO_MESSAGE_HANDLER(x) \
{ \
}

//-------------------------------------------------------------------
#define CAN_TOGGLE_VERBOSE_HANDLER(x) \
{ \
	_verbose = !_verbose; \
}

//-------------------------------------------------------------------
#define CAN_CALIBRATE_ENCODER_HANDLER(x) \
{ \
	calibrate (axis, CAN_DATA[1], BYTE_W(CAN_DATA[2], CAN_DATA[3]), \
								  BYTE_W(CAN_DATA[4], CAN_DATA[5]), \
   								  BYTE_W(CAN_DATA[6], CAN_DATA[7])); \
}

#define USE_NEW_CONTROL_MODES  //USE NEW CONTROL MODES!!
#ifndef USE_NEW_CONTROL_MODES 
//-------------------------------------------------------------------
#define CAN_GET_CONTROL_MODE_HANDLER(x) \
{ \
	get_control_mode(axis); \
}

//-------------------------------------------------------------------
#define CAN_SET_CONTROL_MODE_HANDLER(x) \
{ \
	set_control_mode(axis, CAN_DATA[1]); \
}
#else
//-------------------------------------------------------------------
#define CAN_GET_CONTROL_MODE_HANDLER(x) \
{ \
	get_control_mode(axis); \
}

//-------------------------------------------------------------------
#define CAN_SET_CONTROL_MODE_HANDLER(x) \
{ \
	set_control_mode(axis); \
}
#endif

//-------------------------------------------------------------------
#define CAN_GET_INTERACTION_MODE_HANDLER(x) \
{ \
	get_interaction_mode(axis); \
}

//-------------------------------------------------------------------
#define CAN_SET_INTERACTION_MODE_HANDLER(x) \
{ \
	set_interaction_mode(axis); \
}

//-------------------------------------------------------------------
#define CAN_SET_VEL_SHIFT_HANDLER(x) \
{ \
	byte value = 0; \
	if (CAN_LEN == 3) \
	{ \
		value = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
		if (value>=0 && value <=16) _vel_shift[axis] = value; \
	} \
}

//-------------------------------------------------------------------
#define CAN_SET_VEL_TIMEOUT_HANDLER(x) \
{ \
	byte value = 0; \
	if (CAN_LEN == 3) \
	{ \
		value = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
		if (value>=0) _vel_timeout[axis] = value; \
	} \
}

//-------------------------------------------------------------------
#define CAN_MOTION_DONE_HANDLER(x) \
{ \
	PREPARE_HEADER; \
	CAN_LEN = 3; \
	/* CAN_DATA[1] untouched */ \
	CAN_DATA[1] = BYTE_H(_in_position[axis]); \
	CAN_DATA[2] = BYTE_L(_in_position[axis]); \
	CAN1_send( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_WRITE_FLASH_MEM_HANDLER(x) \
{ \
	writeToFlash (_flash_addr); \
}

//-------------------------------------------------------------------
#define CAN_READ_FLASH_MEM_HANDLER(x) \
{ \
	readFromFlash (_flash_addr); \
}

//-------------------------------------------------------------------
#if 0
#define CAN_GET_ACTIVE_ENCODER_POSITION_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_ID |= 0x100; \
		CAN_LEN = 8; \
		CAN_DATA[0] = BYTE_4(_position[0]); \
		CAN_DATA[1] = BYTE_3(_position[0]); \
		CAN_DATA[2] = BYTE_2(_position[0]); \
		CAN_DATA[3] = BYTE_1(_position[0]); \
		CAN_DATA[4] = BYTE_4(_position[1]); \
		CAN_DATA[5] = BYTE_3(_position[1]); \
		CAN_DATA[6] = BYTE_2(_position[1]); \
		CAN_DATA[7] = BYTE_1(_position[1]); \
		CAN1_send(CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}
#endif

//-------------------------------------------------------------------
#if VERSION==0x0157 || VERSION==0x0150 | VERSION==0x0147 || VERSION==0x0140 || VERSION==0x351 || VERSION==0x0250 || VERSION==0x0257
#define CAN_SET_ACTIVE_ENCODER_POSITION_HANDLER(x) \
{ \
	long value; \
	if (CAN_LEN == 8) \
	{ \
		\
		_cpl_pos_counter = 0; \
		\
		value = BYTE_C(CAN_DATA[0], CAN_DATA[1], CAN_DATA[2], CAN_DATA[3]); \
		/* shift by 2 is because data is available every 4 control cycles */ \
		_cpl_pos_delta[0] = L_sub (value, _cpl_pos_received[0]) >> 2; \
		_cpl_pos_prediction[0] = value; \
		_cpl_pos_received[0] = value; \
		\
		value = BYTE_C(CAN_DATA[4], CAN_DATA[5], CAN_DATA[6], CAN_DATA[7]); \
		_cpl_pos_delta[1] = L_sub (value, _cpl_pos_received[1]) >> 2; \
		_cpl_pos_prediction[1] = value; \
		_cpl_pos_received[1] = value; \
		\
		_pending_request = false; \
		_timeout = 0; \
	} \
}

#elif ((VERSION == 0x0130 || VERSION==0x0230))
#define CAN_SET_ACTIVE_ENCODER_POSITION_HANDLER(x) \
{ \
	byte HES04,HES05,HES06,HES07,HES08,HES11,HES12,HES13; \
	if (CAN_ID==MAIS_8bit_C_MSG) \
	{ \
		HES04=CAN_DATA[4]; \
		HES05=CAN_DATA[5]; \
		HES06=CAN_DATA[6]; \
		_adjustment[0] = HES04+ HES05; \
		_adjustment[1] = HES06; \
		_pending_request = false; \
		_timeout = 0; \
	} \
	if (CAN_ID==MAIS_8bit_D_MSG) \
	{ \
	    HES07=CAN_DATA[0]; \
	    HES08=CAN_DATA[1]; \
		HES11=CAN_DATA[5]; \
		HES12=CAN_DATA[6]; \
		HES13=CAN_DATA[7]; \
		_adjustment[2] = HES07+HES08; \
		_adjustment[3] = HES11+HES12+HES13; \
		_pending_request = false; \
		_timeout = 0; \
	} \
}
#elif ((VERSION == 0x0128) || (VERSION == 0x0228))
#define CAN_SET_ACTIVE_ENCODER_POSITION_HANDLER(x) \
{ \
	byte HES00,HES01,HES02,HES03; \
	if (CAN_ID==MAIS_8bit_C_MSG) \
	{ \
		HES00=CAN_DATA[0]; \
		HES01=CAN_DATA[1]; \
		HES02=CAN_DATA[2]; \
		HES03=CAN_DATA[3]; \
		_adjustment[1] = HES00; \
		_adjustment[2] = HES01+HES02; \
		_adjustment[3] = HES03; \
		_pending_request = false; \
		_timeout = 0; \
	} \
}
#endif



//-------------------------------------------------------------------
#if VERSION==0x0157 || VERSION==0x0150 || VERSION==0x0147 || VERSION==0x0140 || VERSION==0x0351 || VERSION==0x0250 || VERSION==0x0257
#define CAN_SET_ACTIVE_PID_HANDLER(x) \
{ \
	Int16 value; \
	if (CAN_LEN == 8) \
	{ \
		_cpl_pid_counter = 0; \
		value = BYTE_W(CAN_DATA[4], CAN_DATA[5]); \
		_cpl_pid_delta[0] = (value - _cpl_pid_received[0]) >> 2; \
		_cpl_pid_prediction[0] = value; \
		_cpl_pid_received[0] = value; \
		\
		value = BYTE_W(CAN_DATA[6], CAN_DATA[7]); \
		_cpl_pid_delta[1] = (value - _cpl_pid_received[1]) >> 2; \
		_cpl_pid_prediction[1] = value; \
		_cpl_pid_received[1] = value; \
		\
	} \
}
#endif

//-------------------------------------------------------------------
#if VERSION==0x0147 || VERSION==0x0140 || VERSION==0x0157 || VERSION==0x0150 || VERSION==0x351 || VERSION==0x0250 || VERSION==0x0257
#define CAN_SET_ACTIVE_ERROR_HANDLER(x) \
{ \
	Int16 value; \
	if (CAN_LEN == 8) \
	{ \
		value = BYTE_W(CAN_DATA[4], CAN_DATA[5]); \
		_cpl_err[0] = value; \
		\
		value = BYTE_W(CAN_DATA[6], CAN_DATA[7]); \
		_cpl_err[1] = value; \
		\
	} \
}
#endif

//-------------------------------------------------------------------
#define CAN_GET_ENCODER_POSITION_HANDLER(x) \
{ \
	PREPARE_HEADER; \
	CAN_LEN = 7; \
	CAN_DATA[1] = BYTE_4(_position[axis]); \
	CAN_DATA[2] = BYTE_3(_position[axis]); \
	CAN_DATA[3] = BYTE_2(_position[axis]); \
	CAN_DATA[4] = BYTE_1(_position[axis]); \
	CAN_DATA[5] = BYTE_2(_speed[axis]); \
	CAN_DATA[6] = BYTE_1(_speed[axis]); \
    CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#if (CURRENT_BOARD_TYPE  == BOARD_TYPE_4DC)
	#define CAN_SET_ENCODER_POSITION_HANDLER(x) \
	{ \
		long value; \
		if (CAN_LEN == 5) \
		{ \
			value = BYTE_C(CAN_DATA[1], CAN_DATA[2], CAN_DATA[3], CAN_DATA[4]); \
			set_position_encoder (axis, value); \
			_position[axis] = value; \
			_position_old[axis] = value; \
			_integral[axis] = 0; \
		} \
	}
#else
	#define CAN_SET_ENCODER_POSITION_HANDLER(x) \
	{ \
	}
#endif


//-------------------------------------------------------------------
#define CAN_SET_COMMAND_POSITION_HANDLER(x) \
{ \
	if (CAN_LEN == 5) \
	{ \
		if (_control_mode[axis]==MODE_DIRECT) \
		{ \
			_desired[axis] = BYTE_C(CAN_DATA[1], CAN_DATA[2], CAN_DATA[3], CAN_DATA[4]); \
			abort_trajectory (axis, _desired[axis]); \
		} \
	} \
} 

//-------------------------------------------------------------------
#define CAN_SET_DESIRED_POSITION_HANDLER(x) \
{ \
	if (CAN_LEN == 5) \
	{ \
		if (1) \
		{ \
			_set_point[axis] = BYTE_C(CAN_DATA[1], CAN_DATA[2], CAN_DATA[3], CAN_DATA[4]); \
		} \
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_DESIRED_POSITION_HANDLER(x) \
{ \
	PREPARE_HEADER; \
	CAN_LEN = 5; \
	CAN_DATA[1] = BYTE_4(_desired[axis]); \
	CAN_DATA[2] = BYTE_3(_desired[axis]); \
	CAN_DATA[3] = BYTE_2(_desired[axis]); \
	CAN_DATA[4] = BYTE_1(_desired[axis]); \
	CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_MIN_POSITION_HANDLER(x) \
{ \
	if (CAN_LEN == 5) \
	{ \
		_min_position[axis] = BYTE_C(CAN_DATA[1], CAN_DATA[2], CAN_DATA[3], CAN_DATA[4]); \
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_MIN_POSITION_HANDLER(x) \
{ \
	PREPARE_HEADER; \
	CAN_LEN = 5; \
	CAN_DATA[1] = BYTE_4(_min_position[axis]); \
	CAN_DATA[2] = BYTE_3(_min_position[axis]); \
	CAN_DATA[3] = BYTE_2(_min_position[axis]); \
	CAN_DATA[4] = BYTE_1(_min_position[axis]); \
	CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_MAX_POSITION_HANDLER(x) \
{ \
	if (CAN_LEN == 5) \
	{ \
		_max_position[axis] = BYTE_C(CAN_DATA[1], CAN_DATA[2], CAN_DATA[3], CAN_DATA[4]); \
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_MAX_POSITION_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 5; \
		CAN_DATA[1] = BYTE_4(_max_position[axis]); \
		CAN_DATA[2] = BYTE_3(_max_position[axis]); \
		CAN_DATA[3] = BYTE_2(_max_position[axis]); \
		CAN_DATA[4] = BYTE_1(_max_position[axis]); \
		CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_OFFSET_ABS_ENCODER_HANDLER(x) \
{ \
	if (CAN_LEN == 5) \
	{ \
		Int32  value = BYTE_C(CAN_DATA[1], CAN_DATA[2], CAN_DATA[3], CAN_DATA[4]); \
		if (value >=0 && value <=4095) set_max_position(axis, value); \
	} \
}

//-------------------------------------------------------------------
#define CAN_SET_OPTICAL_ENC_RATIO_HANDLER(x) \
{ \
	if (CAN_LEN == 3) \
	{ \
		_optical_ratio[axis] = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_OFFSET_ABS_ENCODER_HANDLER(x) \
{ \
	Int32  value = get_max_position(axis); \
	PREPARE_HEADER; \
		CAN_LEN = 5; \
		CAN_DATA[1] = BYTE_4(value); \
		CAN_DATA[2] = BYTE_3(value); \
		CAN_DATA[3] = BYTE_2(value); \
		CAN_DATA[4] = BYTE_1(value); \
		CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_MAX_VELOCITY_HANDLER(x) \
{ \
	if (CAN_LEN == 3) \
	{ \
		_max_vel[axis] = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_MAX_VELOCITY_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 3; \
		CAN_DATA[1] = BYTE_H(_max_vel[axis]); \
		CAN_DATA[2] = BYTE_L(_max_vel[axis]); \
		CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_STOP_TRAJECTORY_HANDLER(x) \
{ \
	if (_control_mode[axis] == MODE_IMPEDANCE_POS || \
		_control_mode[axis] == MODE_POSITION || \
		_control_mode[axis] == MODE_IMPEDANCE_VEL || \
		_control_mode[axis] == MODE_VELOCITY || \
		_control_mode[axis] == MODE_MIXED) \
		{ \
		    _set_vel[axis] = 0; \
		    _set_acc[axis] = 0; \
            _desired_vel[axis] = 0; \
			init_trajectory (axis, _position[axis], _position[axis], 1); \
		} \
}

//-------------------------------------------------------------------
#define CAN_POSITION_MOVE_HANDLER(x) \
{ \
    if (mode_is_idle(axis)) \
    { \
    /* do nothing */ \
    } \
    else \
	if (CAN_LEN == 7) \
	{ \
		/* only the following modes can received this command */ \
		if (_control_mode[axis] == MODE_POSITION || \
		    _control_mode[axis] == MODE_IMPEDANCE_POS || \
		    _control_mode[axis] == MODE_MIXED) \
		{ \
			/* compute setpoint */ \
			_set_point[axis] = BYTE_C(CAN_DATA[1], CAN_DATA[2], CAN_DATA[3], CAN_DATA[4]); \
			if (_set_point[axis] < _min_position[axis]) \
				_set_point[axis] = _min_position[axis]; \
			else \
			if (_set_point[axis] > _max_position[axis]) \
				_set_point[axis] = _max_position[axis]; \
			_set_vel[axis] = BYTE_W(CAN_DATA[5], CAN_DATA[6]); \
			/* compute setvel, setacc */ \
			if (_set_vel[axis] < 1) \
				_set_vel[axis] = 1; \
			_set_acc[axis] = 0; \
			/* stiction compensation (XOR condition on PID sign) */ \
			if (_ended[axis]) \
			{ \
			   if ( (_set_point[axis]>_position[axis]) == (_kp[axis]>0)) \
		  		  _kstc[axis] = _kstp[axis]; \
			   else \
				  _kstc[axis] = _kstn[axis]; \
			} \
			/* this turn off velocity in MODE_POSITION, MODE_IMPEDANCE_POSITION*/ \
			if (_control_mode[axis] == MODE_POSITION || \
			    _control_mode[axis] == MODE_IMPEDANCE_POS ) \
			{ \
			   _desired_vel[axis] = 0; \
			} \
			/* starts trjectory generation */ \
			init_trajectory (axis, _desired[axis], _set_point[axis], _set_vel[axis]); \
		} \
	} \
}

//-------------------------------------------------------------------
#define CAN_VELOCITY_MOVE_HANDLER(x) \
{ \
	if (mode_is_idle(axis)) \
    { \
    /* do nothing */ \
    } \
	if (CAN_LEN == 5) \
	{ \
		/* only the following modes can received this command */ \
		if (_control_mode[axis] == MODE_VELOCITY || \
		    _control_mode[axis] == MODE_IMPEDANCE_VEL  || \
		    _control_mode[axis] == MODE_MIXED) \
		{ \
			_vel_counter[axis] = 0; \
            /* compute setpoint, set_vel, set_acc */ \
			_set_point[axis] = _desired[axis]; \
			_set_vel[axis] = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
			_set_acc[axis] = BYTE_W(CAN_DATA[3], CAN_DATA[4]); \
		} \
	} \
}

//-------------------------------------------------------------------
#define CAN_SET_DESIRED_VELOCITY_HANDLER(x) \
{ \
	if (CAN_LEN == 3) \
	{ \
		_set_vel[axis] = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_DESIRED_VELOCITY_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 3; \
		CAN_DATA[1] = BYTE_H(_set_vel[axis]); \
		CAN_DATA[2] = BYTE_L(_set_vel[axis]); \
		CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_GET_ENCODER_VELOCITY_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 3; \
		CAN_DATA[1] = BYTE_H(_speed[axis]); \
		CAN_DATA[2] = BYTE_L(_speed[axis]); \
		CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_DESIRED_ACCELER_HANDLER(x) \
{ \
	if (CAN_LEN == 3) \
	{ \
		_set_acc[axis] = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_DESIRED_ACCELER_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 3; \
		CAN_DATA[1] = BYTE_H(_set_acc[axis]); \
		CAN_DATA[2] = BYTE_L(_set_acc[axis]); \
		CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_DESIRED_TORQUE_HANDLER(x) \
{ \
	if (CAN_LEN == 5) \
	{ \
		_desired_torque[axis] = BYTE_C(CAN_DATA[1], CAN_DATA[2], CAN_DATA[3], CAN_DATA[4]); \
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_DESIRED_TORQUE_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 5; \
		CAN_DATA[1] = BYTE_4(_desired_torque[axis]); \
		CAN_DATA[2] = BYTE_3(_desired_torque[axis]); \
		CAN_DATA[3] = BYTE_2(_desired_torque[axis]); \
		CAN_DATA[4] = BYTE_1(_desired_torque[axis]); \
		CAN1_send(CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_SPEED_ESTIM_SHIFT_HANDLER(x) \
{ \
	_jntVel_est_shift[axis] = CAN_DATA[1]; \
	_jntAcc_est_shift[axis] = CAN_DATA[2]; \
	_motVel_est_shift[axis] = CAN_DATA[3]; \
	_motAcc_est_shift[axis] = CAN_DATA[4]; \
}

//-------------------------------------------------------------------
// GENERIC DEBUG PARAMETER
#define CAN_SET_DEBUG_PARAM_HANDLER(x) \
{ \
	if      (CAN_DATA[1]==0) _debug_in0[axis] = BYTE_W(CAN_DATA[2], CAN_DATA[3]); \
	else if (CAN_DATA[1]==1) _debug_in1[axis] = BYTE_W(CAN_DATA[2], CAN_DATA[3]); \
	else if (CAN_DATA[1]==2) _debug_in2[axis] = BYTE_W(CAN_DATA[2], CAN_DATA[3]); \
	else if (CAN_DATA[1]==3) _debug_in3[axis] = BYTE_W(CAN_DATA[2], CAN_DATA[3]); \
	else if (CAN_DATA[1]==4) _debug_in4[axis] = BYTE_W(CAN_DATA[2], CAN_DATA[3]); \
	else if (CAN_DATA[1]==5) _debug_in5[axis] = BYTE_W(CAN_DATA[2], CAN_DATA[3]); \
	else if (CAN_DATA[1]==6) _debug_in6[axis] = BYTE_W(CAN_DATA[2], CAN_DATA[3]); \
	else if (CAN_DATA[1]==7) _debug_in7[axis] = BYTE_W(CAN_DATA[2], CAN_DATA[3]); \
	\
	else if (CAN_DATA[1]==20) \
	{ \
		_sacc0[axis] = BYTE_W(CAN_DATA[2],CAN_DATA[3]); \
	 	_sacc1[axis] = BYTE_W(CAN_DATA[4],CAN_DATA[5]); \
	 	_sacc2[axis] = BYTE_W(CAN_DATA[6],CAN_DATA[7]); \
	} \
	\
	else if (CAN_DATA[1]==10) _param_a10_coeff = (float) (BYTE_W(CAN_DATA[2], CAN_DATA[3])) / 1000.0F; \
	else if (CAN_DATA[1]==11) _param_a11_coeff = (float) (BYTE_W(CAN_DATA[2], CAN_DATA[3])) / 1000.0F; \
	else if (CAN_DATA[1]==12) _param_a20_coeff = (float) (BYTE_W(CAN_DATA[2], CAN_DATA[3])) / 1000.0F; \
	else if (CAN_DATA[1]==13) _param_a21_coeff = (float) (BYTE_W(CAN_DATA[2], CAN_DATA[3])) / 1000.0F; \
	else if (CAN_DATA[1]==14) _param_a22_coeff = (float) (BYTE_W(CAN_DATA[2], CAN_DATA[3])) / 1000.0F; \
}

/*
// used to change the speed of the main loop
if (CAN_LEN == 4) \
{ \
	word tmp; \
	tmp = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
	_t1c = CAN_DATA[3]; \
	setReg (TMRA3_CMP1, tmp); \
} \
else if (CAN_LEN == 3) \
{ \
	word tmp; \
	tmp = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
	_t1c = 0; \
	setReg (TMRA3_CMP1, tmp); \
} \
else \
{ \
	setReg (TMRA3_CMP1, 39999); \
} \
*/

//-------------------------------------------------------------------
// GENERIC DEBUG PARAMETER
#define CAN_GET_DEBUG_PARAM_HANDLER(x) \
{ \
	byte  index_dbg = CAN_DATA[1]; \
	Int16 val = 0; \
	PREPARE_HEADER; \
		CAN_LEN = 3; \
		if      (index_dbg==0) {CAN_DATA[1] = BYTE_H(_debug_in0[axis]);CAN_DATA[2] = BYTE_L(_debug_in0[axis]);} \
		else if (index_dbg==1) {CAN_DATA[1] = BYTE_H(_debug_in1[axis]);CAN_DATA[2] = BYTE_L(_debug_in1[axis]);} \
		else if (index_dbg==2) {CAN_DATA[1] = BYTE_H(_debug_in2[axis]);CAN_DATA[2] = BYTE_L(_debug_in2[axis]);} \
		else if (index_dbg==3) {CAN_DATA[1] = BYTE_H(_debug_in3[axis]);CAN_DATA[2] = BYTE_L(_debug_in3[axis]);} \
		else if (index_dbg==4) {CAN_DATA[1] = BYTE_H(_debug_in4[axis]);CAN_DATA[2] = BYTE_L(_debug_in4[axis]);} \
		else if (index_dbg==5) {CAN_DATA[1] = BYTE_H(_debug_in5[axis]);CAN_DATA[2] = BYTE_L(_debug_in5[axis]);} \
		else if (index_dbg==6) {CAN_DATA[1] = BYTE_H(_debug_in6[axis]);CAN_DATA[2] = BYTE_L(_debug_in6[axis]);} \
		else if (index_dbg==7) {CAN_DATA[1] = BYTE_H(_debug_in7[axis]);CAN_DATA[2] = BYTE_L(_debug_in7[axis]);} \
		\
		else if (index_dbg==10) {val=(Int16)(_param_a10_coeff*1000.0F); CAN_DATA[1] =BYTE_H(val);CAN_DATA[2] = BYTE_L(val);}\
		else if (index_dbg==11) {val=(Int16)(_param_a11_coeff*1000.0F); CAN_DATA[1] =BYTE_H(val);CAN_DATA[2] = BYTE_L(val);}\
		else if (index_dbg==12) {val=(Int16)(_param_a20_coeff*1000.0F); CAN_DATA[1] =BYTE_H(val);CAN_DATA[2] = BYTE_L(val);}\
		else if (index_dbg==13) {val=(Int16)(_param_a21_coeff*1000.0F); CAN_DATA[1] =BYTE_H(val);CAN_DATA[2] = BYTE_L(val);}\
		else if (index_dbg==14) {val=(Int16)(_param_a22_coeff*1000.0F);CAN_DATA[1] = BYTE_H(val);CAN_DATA[2] = BYTE_L(val);}\
		else if (index_dbg==15) {val=0; CAN_DATA[1] =BYTE_H(val);CAN_DATA[2] = BYTE_L(val);}\
		else if (index_dbg==16) {val=0; CAN_DATA[1] =BYTE_H(val);CAN_DATA[2] = BYTE_L(val);}\
		else if (index_dbg==17) {val=0; CAN_DATA[1] =BYTE_H(val);CAN_DATA[2] = BYTE_L(val);}\
		 \
        else if (index_dbg==20) \
        { \
        	CAN_DATA[1] = BYTE_H(_sacc0[axis]); \
        	CAN_DATA[2] = BYTE_L(_sacc0[axis]); \
        	CAN_DATA[3] = BYTE_H(_sacc1[axis]); \
        	CAN_DATA[4] = BYTE_L(_sacc1[axis]); \
        	CAN_DATA[5] = BYTE_H(_sacc2[axis]); \
        	CAN_DATA[6] = BYTE_L(_sacc2[axis]); \
        } \
		CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_GET_PID_OUTPUT_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 3; \
		CAN_DATA[1] = BYTE_H(_pid[axis]); \
		CAN_DATA[2] = BYTE_L(_pid[axis]); \
		CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_GET_PID_ERROR_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 3; \
		CAN_DATA[1] = BYTE_H(_error_position[axis]); \
		CAN_DATA[2] = BYTE_L(_error_position[axis]); \
		CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_TORQUE_PID_HANDLER(x) \
{ \
	if (CAN_LEN == 8) \
	{ \
		_kp_torque[axis] = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
		_ki_torque[axis] = BYTE_W(CAN_DATA[3], CAN_DATA[4]); \
		_kd_torque[axis] = BYTE_W(CAN_DATA[5], CAN_DATA[6]); \
		_kr_torque[axis] = (CAN_DATA[7]); \
	} \
}
//-------------------------------------------------------------------
#define CAN_GET_TORQUE_PID_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 8; \
		CAN_DATA[1] = BYTE_H(_kp_torque[axis]); \
		CAN_DATA[2] = BYTE_L(_kp_torque[axis]); \
		CAN_DATA[3] = BYTE_H(_ki_torque[axis]); \
		CAN_DATA[4] = BYTE_L(_ki_torque[axis]); \
		CAN_DATA[5] = BYTE_H(_kd_torque[axis]); \
		CAN_DATA[6] = BYTE_L(_kd_torque[axis]); \
		CAN_DATA[7] = BYTE_H(_kr_torque[axis]); \
		CAN1_send(CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_IMPEDANCE_PARAMS_HANDLER(x) \
{ \
	if (CAN_LEN == 8) \
	{ \
		_ks_imp[axis] = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
		_kd_imp[axis] = BYTE_W(CAN_DATA[3], CAN_DATA[4]); \
	} \
}
	//_ko_imp[axis] = BYTE_W(CAN_DATA[5], CAN_DATA[6]); \ //commented out for testing
//-------------------------------------------------------------------
#define CAN_GET_IMPEDANCE_PARAMS_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 8; \
		CAN_DATA[1] = BYTE_H(_ks_imp[axis]); \
		CAN_DATA[2] = BYTE_L(_ks_imp[axis]); \
		CAN_DATA[3] = BYTE_H(_kd_imp[axis]); \
		CAN_DATA[4] = BYTE_L(_kd_imp[axis]); \
		CAN_DATA[5] = BYTE_H(_ko_imp[axis]); \
		CAN_DATA[6] = BYTE_L(_ko_imp[axis]); \
		CAN_DATA[7] = 0; \
		CAN1_send(CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_BACKEMF_PARAMS_HANDLER(x) \
{ \
	if (CAN_LEN == 8) \
	{ \
		_backemf_gain[axis] = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
		_backemf_shift[axis] = CAN_DATA[3]; \
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_BACKEMF_PARAMS_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 8; \
		CAN_DATA[1] = BYTE_H(_backemf_gain[axis]); \
		CAN_DATA[2] = BYTE_L(_backemf_gain[axis]); \
		CAN_DATA[3] = _backemf_shift[axis]; \
		CAN_DATA[4] = 0; \
		CAN_DATA[5] = 0; \
		CAN_DATA[6] = 0; \
		CAN_DATA[7] = 0; \
		CAN1_send(CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_OPENLOOP_PARAMS_HANDLER(x) \
{ \
	if (CAN_LEN == 3) \
	{ \
		_ko_openloop[axis] = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_OPENLOOP_PARAMS_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 3; \
		CAN_DATA[1] = BYTE_H(_ko_openloop[axis]); \
		CAN_DATA[2] = BYTE_L(_ko_openloop[axis]); \
		CAN1_send(CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}
//-------------------------------------------------------------------
#define CAN_SET_MODEL_PARAMS_HANDLER(x) \
{ \
	if (CAN_LEN == 8) \
	{ \
		_kff_torque[axis] = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_MODEL_PARAMS_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 8; \
		CAN_DATA[1] = BYTE_H(_kff_torque[axis]); \
		CAN_DATA[2] = BYTE_L(_kff_torque[axis]); \
		CAN_DATA[3] = 0; \
		CAN_DATA[4] = 0; \
		CAN_DATA[5] = 0; \
		CAN_DATA[6] = 0; \
		CAN_DATA[7] = 0; \
		CAN1_send(CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_IMPEDANCE_OFFSET_HANDLER(x) \
{ \
	if (CAN_LEN == 3) \
	{ \
		_ko_imp[axis] = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
	} \
}
//-------------------------------------------------------------------
#define CAN_GET_IMPEDANCE_OFFSET_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 3; \
		CAN_DATA[1] = BYTE_H(_ko_imp[axis]); \
		CAN_DATA[2] = BYTE_L(_ko_imp[axis]); \
		CAN1_send(CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_TORQUE_PIDLIMITS_HANDLER(x) \
{ \
	if (CAN_LEN == 8) \
	{ \
		_ko_torque[axis] 				= BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
		_pid_limit_torque[axis] 		= BYTE_W(CAN_DATA[3], CAN_DATA[4]); \
		_integral_limit_torque[axis] 	= BYTE_W(CAN_DATA[5], CAN_DATA[6]); \
	} \
}
//-------------------------------------------------------------------
#define CAN_GET_TORQUE_PIDLIMITS_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 8; \
		CAN_DATA[1] = BYTE_H(_ko_torque[axis]); \
		CAN_DATA[2] = BYTE_L(_ko_torque[axis]); \
		CAN_DATA[3] = BYTE_H(_pid_limit_torque[axis]); \
		CAN_DATA[4] = BYTE_L(_pid_limit_torque[axis]); \
		CAN_DATA[5] = BYTE_H(_integral_limit_torque[axis]); \
		CAN_DATA[6] = BYTE_L(_integral_limit_torque[axis]); \
		CAN_DATA[7] = 0; \
		CAN1_send(CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_POS_PID_HANDLER(x) \
{ \
	if (CAN_LEN == 8) \
	{ \
		_kp[axis] = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
		_ki[axis] = BYTE_W(CAN_DATA[3], CAN_DATA[4]); \
		_kd[axis] = BYTE_W(CAN_DATA[5], CAN_DATA[6]); \
		_kr[axis] = (CAN_DATA[7]); \
		_received_pid[axis].rec_pid |=0x4F; \
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_POS_PID_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 8; \
		CAN_DATA[1] = BYTE_H(_kp[axis]); \
		CAN_DATA[2] = BYTE_L(_kp[axis]); \
		CAN_DATA[3] = BYTE_H(_ki[axis]); \
		CAN_DATA[4] = BYTE_L(_ki[axis]); \
		CAN_DATA[5] = BYTE_H(_kd[axis]); \
		CAN_DATA[6] = BYTE_L(_kd[axis]); \
		CAN_DATA[7] = BYTE_H(_kr[axis]); \
		CAN1_send(CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_POS_PIDLIMITS_HANDLER(x) \
{ \
	if (CAN_LEN == 8) \
	{ \
		_ko[axis] 				= BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
		_pid_limit[axis] 		= BYTE_W(CAN_DATA[3], CAN_DATA[4]); \
		_integral_limit[axis] 	= BYTE_W(CAN_DATA[5], CAN_DATA[6]); \
		_received_pid[axis].rec_pid |=0x30; \
	} \
}
//-------------------------------------------------------------------
#define CAN_GET_POS_PIDLIMITS_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 8; \
		CAN_DATA[1] = BYTE_H(_ko[axis]); \
		CAN_DATA[2] = BYTE_L(_ko[axis]); \
		CAN_DATA[3] = BYTE_H(_pid_limit[axis]); \
		CAN_DATA[4] = BYTE_L(_pid_limit[axis]); \
		CAN_DATA[5] = BYTE_H(_integral_limit[axis]); \
		CAN_DATA[6] = BYTE_L(_integral_limit[axis]); \
		CAN_DATA[7] = 0; \
		CAN1_send(CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_P_GAIN_HANDLER(x) \
{ \
	if (CAN_LEN == 3) \
	{ \
		_kp[axis] = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
		_received_pid[axis].rec_pid_bits.kp =true; \
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_P_GAIN_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 3; \
		CAN_DATA[1] = BYTE_H(_kp[axis]); \
		CAN_DATA[2] = BYTE_L(_kp[axis]); \
		CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_D_GAIN_HANDLER(x) \
{ \
	if (CAN_LEN == 3) \
	{ \
		_kd[axis] = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
		_received_pid[axis].rec_pid_bits.kd =true; \
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_D_GAIN_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 3; \
		CAN_DATA[1] = BYTE_H(_kd[axis]); \
		CAN_DATA[2] = BYTE_L(_kd[axis]); \
		CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_I_GAIN_HANDLER(x) \
{ \
	if (CAN_LEN == 3) \
	{ \
		_ki[axis] = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
		_integral[axis] = 0; \
		_received_pid[axis].rec_pid_bits.ki =true; \
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_I_GAIN_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 3; \
		CAN_DATA[1] = BYTE_H(_ki[axis]); \
		CAN_DATA[2] = BYTE_L(_ki[axis]); \
		CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_ILIM_GAIN_HANDLER(x) \
{ \
	if (CAN_LEN == 3) \
	{ \
		_integral_limit[axis] = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
		_received_pid[axis].rec_pid_bits.ilim =true; \
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_ILIM_GAIN_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 3; \
		CAN_DATA[1] = BYTE_H(_integral_limit[axis]); \
		CAN_DATA[2] = BYTE_L(_integral_limit[axis]); \
		CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_OFFSET_HANDLER(x) \
{ \
	if (CAN_LEN == 3) \
	{ \
		_ko[axis] = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
		_received_pid[axis].rec_pid_bits.ko =true; \
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_OFFSET_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 3; \
		CAN_DATA[1] = BYTE_H(_ko[axis]); \
		CAN_DATA[2] = BYTE_L(_ko[axis]); \
		CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_POS_STICTION_PARAMS_HANDLER(x) \
{ \
	if (CAN_LEN == 5) \
	{ \
		_kstp[axis] = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
		_kstn[axis] = BYTE_W(CAN_DATA[3], CAN_DATA[4]); \
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_POS_STICTION_PARAMS_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 5; \
		CAN_DATA[1] = BYTE_H(_kstp[axis]); \
		CAN_DATA[2] = BYTE_L(_kstp[axis]); \
		CAN_DATA[3] = BYTE_H(_kstn[axis]); \
		CAN_DATA[4] = BYTE_L(_kstn[axis]); \
		CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_TORQUE_STICTION_PARAMS_HANDLER(x) \
{ \
	if (CAN_LEN == 5) \
	{ \
		_kstp_torque[axis] = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
		_kstn_torque[axis] = BYTE_W(CAN_DATA[3], CAN_DATA[4]); \
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_TORQUE_STICTION_PARAMS_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 5; \
		CAN_DATA[1] = BYTE_H(_kstp_torque[axis]); \
		CAN_DATA[2] = BYTE_L(_kstp_torque[axis]); \
		CAN_DATA[3] = BYTE_H(_kstn_torque[axis]); \
		CAN_DATA[4] = BYTE_L(_kstn_torque[axis]); \
		CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_SCALE_HANDLER(x) \
{ \
	if (CAN_LEN == 3) \
	{ \
		_kr[axis] = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
		_received_pid[axis].rec_pid_bits.kr =true; \
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_SCALE_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 3; \
		CAN_DATA[1] = BYTE_H(_kr[axis]); \
		CAN_DATA[2] = BYTE_L(_kr[axis]); \
		CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_TLIM_HANDLER(x) \
{ \
	if (CAN_LEN == 3) \
	{ \
		_pid_limit[axis] = BYTE_W(CAN_DATA[1], CAN_DATA[2]); \
		_received_pid[axis].rec_pid_bits.tlim =true; \
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_TLIM_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 3; \
		CAN_DATA[1] = BYTE_H(_pid_limit[axis]); \
		CAN_DATA[2] = BYTE_L(_pid_limit[axis]); \
		CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_CURRENT_LIMIT_HANDLER(x) \
{ \
	if (CAN_LEN == 5) \
	{ \
		dword tmp; \
		tmp = BYTE_C(CAN_DATA[1], CAN_DATA[2], CAN_DATA[3], CAN_DATA[4]); \
		if (tmp<=MAX_CURRENT) _max_allowed_current[axis]=tmp;\
		else can_printf("MAX CURRENT BIGGER THEN MAX:%d",axis);\
	} \
}

//	_conversion_factor[axis] = (tmp * 3.3) / 32760.0f; \

//-------------------------------------------------------------------
#define CAN_GET_ERROR_STATUS_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 3; \
		CAN_DATA[1] = 0x00; \
		CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_TORQUE_SOURCE_HANDLER(x) \
{ \
	if (CAN_LEN == 8) \
	{ \
		_selected_strain_id[axis]=CAN_DATA[1]; \
		_selected_strain_chan[axis]=CAN_DATA[2]; \
	} \
}

//-------------------------------------------------------------------
#define CAN_SET_BOARD_ID_HANDLER(x) \
{ \
	byte value = 0; \
	if (CAN_LEN == 2) \
	{ \
		value = CAN_DATA[1]; \
		if (value>=1 && value <=15) _board_ID = value; \
		CAN1_init (_board_ID); \
		set_can_masks(); \
		writeToFlash (_flash_addr); \
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_BOARD_ID_HANDLER(x) \
{ \
	PREPARE_HEADER; \
		CAN_LEN = 2; \
		CAN_DATA[1] = _board_ID; \
		CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

//-------------------------------------------------------------------
#define CAN_SET_ADDITIONAL_INFO_HANDLER(x) \
{ \
	can_receive_additional_info(); \
	writeToFlash (_flash_addr); \
}

//-------------------------------------------------------------------
#define CAN_GET_ADDITIONAL_INFO_HANDLER(x) \
{ \
	PREPARE_HEADER; \
	can_send_additional_info(); \
}

//-------------------------------------------------------------------
#define CAN_SET_BCAST_POLICY_HANDLER(x) \
{ \
	if (CAN_LEN == 5) \
	{ \
		if (axis<2)\
			broadcast_mask[0] = BYTE_C(CAN_DATA[1], CAN_DATA[2], CAN_DATA[3], CAN_DATA[4]); \
		else\
			broadcast_mask[1] = BYTE_C(CAN_DATA[1], CAN_DATA[2], CAN_DATA[3], CAN_DATA[4]); \
		if (broadcast_mask[0] & (1<<(CAN_BCAST_PRINT-1))) \
			enable_can_print(); \
		else \
			disable_can_print();\
	} \
}

//-------------------------------------------------------------------
#define CAN_GET_FIRMWARE_VERSION_HANDLER(x) \
{ \
	byte server_can_protocol_major = CAN_DATA[1]; \
	byte server_can_protocol_minor = CAN_DATA[2]; \
	PREPARE_HEADER; \
		CAN_LEN = 8; \
		_can_protocol_ack = (_my_can_protocol_major == server_can_protocol_major && \
						     _my_can_protocol_minor >= server_can_protocol_minor); \
		_canmsg.CAN_data[1] = CURRENT_BOARD_TYPE;   \
		_canmsg.CAN_data[2] = (_version & 0xff00) >> 8; \
		_canmsg.CAN_data[3] = _version & 0x00ff; 	    \
		_canmsg.CAN_data[4] = _build_number ;   \
		_canmsg.CAN_data[5] = _my_can_protocol_major; \
		_canmsg.CAN_data[6] = _my_can_protocol_minor; \
		_canmsg.CAN_data[7] = (byte)(_can_protocol_ack); \
		CAN1_send ( CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
}

// end of messages 
#endif

