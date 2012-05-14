#ifndef __options_h__
#define __options_h__

//#define VERSION 0x0251                /* it reads absolute and incremental encoder for two motors */
                                        /* commutation with incremental encoder
//#define VERSION 0x0255                /* it reads absolute and incremental encoder for one motor */
//#define VERSION 0x0258                /* commutation with incremental encoder

//#define SERIAL_MENU_OPTION    1       /* if on, enables serial menu interface */

//#define DEBUG_CAN_MSG 			1		/* conditional compile for printing can info */
//#define DEBUG_CONTROL_RATE		1 		/* for debugging control cycle rate */
//#define DEBUG_TRAJECTORY 			1		/* print encoder/joint position */
//#define DEBUG_SERIAL				1		/* for debugging through terminal */
//#define DEBUG_CURRENT				1		/* for debugging current through terminal */
//#define DEBUG_CALIBRATION			1		/* for calibration debugging through terminal */
//#define DEBUG_CPL_BOARD	    	1		/* for coupled board debug */
//#define EMERGENCY_DISABLED		1		/* emergency fault signal disabled */
//#define SMOOTH_PID_CTRL			1		/* for debugging current spikes */
//#define TEMPERATURE_SENSOR    	1
//#define ENCODER_SHIFT_11BITS  	1
//#define DEBUG_ABS_SENSOR_STATUS 	1
//#define DEBUG_CONTROL_MODE    	1

#define CURRENT_BOARD_TYPE  BOARD_TYPE_BLL
#define JN 					2		/* number of axes */
#define CAN_SYNCHRO_STEPS 4  //@@@RANDAZ: CAN_SYNCHRO_STEP reduced from 5 to 4 because of CAN_SET_ACTIVE_PID_HANDLER() 
#define BOARDSTATUSTIME 5000 //the time in ms between a BCAST_STATUS message and another
#define USE_HALL_SENS   1
#define USE_ABS_SSI     1
//#define IDENTIF         1
#endif
  