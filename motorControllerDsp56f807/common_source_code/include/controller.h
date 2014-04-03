//# Copyright: (C) 2007-2010 RobotCub Consortium
//# Authors: Marco Maggiali, Marco Randazzo
//# CopyPolicy: Released under the terms of the GNU GPL v2.0.

///
/// $Id: controller.h,v 1.15 2009/06/26 17:10:49 randaz Exp $
///
///

#include "options.h"
  
#ifndef __controllerh__
#define __controllerh__

#ifndef __ONLY_DEF
#include "dsp56f807.h"
#endif

/* 
 * the purpose of including this file on Linux/Winnt/Qnx is to 
 *     get the definition of messages and params of the dsp controller
 * define __ONLY_DEF before inclusion on Linux/Winnt/Qnx
 */
#include "stdint.h"
#include "iCubCanProto_types.h"
//board_types
#define BOARD_TYPE_DSP    icubCanProto_boardType__dsp
#define BOARD_TYPE_PIC    icubCanProto_boardType__pic
#define BOARD_TYPE_2DC    icubCanProto_boardType__2dc
#define BOARD_TYPE_4DC    icubCanProto_boardType__4dc
#define BOARD_TYPE_BLL    icubCanProto_boardType__bll
#define BOARD_TYPE_2BLLDC icubCanProto_boardType__bll



#define DEFAULT_BOARD_ID    15
#define SMALL_BUFFER_SIZE 	32		/* for serial I/O */
#define CONTROLLER_PERIOD 	1		/* espressed in ms */




#define MODE_IDLE						icubCanProto_controlmode_idle
#define MODE_POSITION 					icubCanProto_controlmode_position
#define MODE_VELOCITY					icubCanProto_controlmode_velocity
#define MODE_TORQUE						icubCanProto_controlmode_torque
#define MODE_IMPEDANCE_POS				icubCanProto_controlmode_impedance_pos
#define MODE_IMPEDANCE_VEL				icubCanProto_controlmode_impedance_vel
#define MODE_CALIB_ABS_POS_SENS			icubCanProto_controlmode_calib_abs_pos_sens
#define MODE_CALIB_HARD_STOPS			icubCanProto_controlmode_calib_hard_stops
#define MODE_HANDLE_HARD_STOPS			icubCanProto_controlmode_handle_hard_stops
#define MODE_MARGIN_REACHED    			icubCanProto_controlmode_margin_reached
#define MODE_CALIB_ABS_AND_INCREMENTAL	icubCanProto_controlmode_calib_abs_and_incremental
#define MODE_OPENLOOP               	icubCanProto_controlmode_openloop

//   Calibration Type Messages 
#define CALIB_HARD_STOPS            icubCanProto_calibration_type0_hard_stops 
#define CALIB_ABS_POS_SENS          icubCanProto_calibration_type1_abs_sens_analog
#define CALIB_HARD_STOPS_DIFF       icubCanProto_calibration_type2_hard_stops_diff 
#define CALIB_ABS_DIGITAL           icubCanProto_calibration_type3_abs_sens_digital
#define CALIB_ABS_AND_INCREMENTAL   icubCanProto_calibration_type4_abs_and_incremental

#define DEFAULT_VELOCITY 10
#define DEFAULT_ACCELERATION 10
#define DEFAULT_MAX_POSITION 5000
#define DEFAULT_MAX_VELOCITY 0x7fff
#define HALL_EFFECT_SENS_ZERO 14760

#ifndef __ONLY_DEF
/* deals with the endianism - byte 4 is the MSB on the Pentium */
#define BYTE_1(x) (byte)(__shr(__extract_h(x), 8))
#define BYTE_2(x) (byte)(__extract_h(x) & 0xff)
#define BYTE_3(x) (byte)(__shr(__extract_l(x), 8))
#define BYTE_4(x) (byte)(__extract_l(x) & 0xff)

/* extracting from a short */
#define BYTE_L(x) (byte)(__shr(x, 8))
#define BYTE_H(x) (byte)(x & 0xff)

/* same as above, deals also with endianism */
dword BYTE_C(byte x4, byte x3, byte x2, byte x1);
	 
#define BYTE_W(x2, x1) (__shl((int)x1,8) | (int)x2)

#if (JN == 2)
	#define INIT_ARRAY(x) {x,x}
#elif (JN == 4)
	#define INIT_ARRAY(x) {x,x,x,x}
#else
	#error unable to init array for JN != 2,4
#endif
    
/* clrRegBit(CANRIER, RXFIE), setRegBit(CANRIER, RXFIE) */

#endif

#endif
