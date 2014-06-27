#ifndef __options_h__
#define __options_h__

//#define VERSION 0x0161   				/* standard/basic implementation */
//#define VERSION 0x0162   				/* differential joint in the head */

//#define SERIAL_MENU_OPTION    1       /* if on, enables serial menu interface */

//#define DEBUG_CAN_MSG 		1		/* conditional compile for printing can info */
//#define DEBUG_CONTROL_RATE	1 		/* for debugging control cycle rate */
//#define DEBUG_TRAJECTORY 		1		/* print encoder/joint position */
//#define DEBUG_SERIAL			1		/* for debugging through terminal */
//#define DEBUG_CURRENT			1		/* for debugging current through terminal */
#define DEBUG_CALIBRATION		1		/* for calibration debugging through terminal */
//#define EMERGENCY_DISABLED	1		/* emergency fault signal disabled */
//#define   TEMPERATURE_SENSOR    1
//#define ENCODER_SHIFT_11BITS  1
//#define DEBUG_ABS_SENSOR_STATUS 1
//#define DEBUG_TEMP_SENS 		1
//#define DEBUG_FAULT_ON_LIMIT    1
#endif

#define CURRENT_BOARD_TYPE  BOARD_TYPE_2BLLDC
#define JN 					2		/* number of axes */
#define CAN_SYNCHRO_STEPS 5
#define BOARDSTATUSTIME 5000 //the time in ms between a BCAST_STATUS message and another
