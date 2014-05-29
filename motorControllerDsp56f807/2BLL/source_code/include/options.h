#ifndef __options_h__
#define __options_h__

//#define VERSION 0x0150   				/* position and pwm decoupling of the first two joint of the shoulder  */
//#define VERSION 0x0151   				/* standard/basic implementation */
//#define VERSION 0x0152				/* waist deifferential decoupling */
//#define VERSION 0x0153			   	/* decouples the third joint of the shoulder */
//#define VERSION 0x0154		    	/* only one joint */
//#define VERSION 0x0155                /* it reads absolute and incremental encoder for one motot */
//#define VERSION 0x0156		    	/* low level current control */
//#define VERSION 0x0157		    	/* position and pwm decoupling of the third joint of the shoulder */
//#define VERSION 0x0158                /* commutation with incremental encoder
//#define VERSION 0x0170		    	/* Listens for CAN messages from strain boards and computes force control */
//#define VERSION 0x0171		    	/* Use the internal ADC to measure the strain gauges computes force control */
//#define VERSION 0x0172		    	/* like version 0x0170, with optical encoder*/
//#define VERSION 0x0140   				/* similar to 0x150, but no joint decoupling  */
//#define VERSION 0x0147   				/* similar to 0x157, but no joint decoupling  */
//#define VERSION 0x0351		    	/* ikart wheels control*/


//#define SERIAL_MENU_OPTION    1       /* if on, enables serial menu interface */

//#define DEBUG_CAN_MSG 			1		/* conditional compile for printing can info */
//#define DEBUG_CONTROL_RATE		1 		/* for debugging control cycle rate */
//#define DEBUG_TRAJECTORY 			1		/* print encoder/joint position */
//#define DEBUG_SERIAL				1		/* for debugging through terminal */
//#define DEBUG_CURRENT				1		/* for debugging current through terminal */
//#define DEBUG_CALIBRATION			1		/* for calibration debugging through terminal */
//#define DEBUG_CPL_BOARD	    	1		/* for coupled board debug */
//#define EMERGENCY_DISABLED		1		/* emergency fault signal disabled */
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

#endif
 