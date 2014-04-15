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
#include "iCubCanProto_types.h"



//board_types
#define BOARD_TYPE_DSP    ICUBCANPROTO_BOARDTYPE__DSP
#define BOARD_TYPE_PIC    ICUBCANPROTO_BOARDTYPE__PIC
#define BOARD_TYPE_2DC    ICUBCANPROTO_BOARDTYPE__2DC
#define BOARD_TYPE_4DC    ICUBCANPROTO_BOARDTYPE__4DC
#define BOARD_TYPE_BLL    ICUBCANPROTO_BOARDTYPE__BLL
#define BOARD_TYPE_2BLLDC ICUBCANPROTO_BOARDTYPE__BLL


#define DEFAULT_BOARD_ID    15
#define SMALL_BUFFER_SIZE 	32		/* for serial I/O */
#define CONTROLLER_PERIOD 	1		/* espressed in ms */



// internal board statuses 
#define MODE_IDLE						0x00
#define MODE_POSITION 					0x01
#define MODE_VELOCITY					0x02
#define MODE_TORQUE						0x03
#define MODE_IMPEDANCE_POS				0x04 //deprecated 
#define MODE_IMPEDANCE_VEL				0x05 //deprecated
#define MODE_CURRENT                    0x06
#define MODE_MIXED                      0x07
#define MODE_DIRECT                     0x08
#define MODE_CALIB_ABS_POS_SENS			0x10 //internal calib status
#define MODE_CALIB_HARD_STOPS			0x20 //internal calib status
#define MODE_HANDLE_HARD_STOPS			0x30 //internal calib status
#define MODE_MARGIN_REACHED    			0x40 //internal calib status
#define MODE_CALIB_ABS_AND_INCREMENTAL	0x41 //internal calib status
#define MODE_OPENLOOP               	0x50
#define MODE_HW_FAULT                   0xA0 //internal status
#define MODE_NOT_CONFIGURED             0xB0 //internal status
#define MODE_CONFIGURED                 0xB1 //internal status
#define MODE_UNKNOWN_ERROR              0xFF //internal status

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
