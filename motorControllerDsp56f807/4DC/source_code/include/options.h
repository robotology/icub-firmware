#ifndef __options_h__
#define __options_h__

//#define VERSION 0x0111   				/* standard/basic implementation */
//#define VERSION 0x0115		    	/* coordinated control of the eyes */
//#define VERSION 0x0119                /* Differential Joint M1 M2 (wrist) analog in M3*/
//#define VERSION 0x0128                /* Analog encoder in M0 MAISEncoder in M1 (HES0), M2 (HES1+HES2), M3 (HES3) 
//#define VERSION 0x0130                /* MAISEncoder in M0 (HES4+HES5), M1 (HES6), M2 (HES7+HES8), M3 (HES11+HES12+HES13) 
//#define VERSION 0x0215                /* coordinated control of the eyes + neck yaw and eye tilt with AEA*/
//#define VERSION 0x0219                /* AEA encoders in the wrist  
//#define VERSION 0x0228                /* HAND V2 Analog encoder in M0 MAISEncoder in M1 (HES0), M2 (HES1+HES2), M3 (HES3)  
//#define VERSION 0x0230                /* HAND V2 MAISEncoder in M0 (HES4+HES5), M1 (HES6), M2 (HES7+HES8), M3 (HES11+HES12+HES13) 
//#define VERSION 0x0214                /* TALKING HEAD, M3 AEA encoder LIPS LOW, M0 M1 and M2 incremental encoder.    
//#define SERIAL_MENU_OPTION    1       /* if on, enables serial menu interface */

//#define DEBUG_CAN_MSG 	    1		/* conditional compile for printing can info */
//#define DEBUG_CONTROL_RATE	1 		/* for debugging control cycle rate */
//#define DEBUG_TRAJECTORY 		1		/* print encoder/joint position */
//#define DEBUG_SERIAL			1		/* for debugging through terminal */
//#define DEBUG_CURRENT			1		/* for debugging current through terminal */
//#define DEBUG_CALIBRATION		1		/* for calibration debugging through terminal */
//#define EMERGENCY_DISABLED	1		/* emergency fault signal disabled */
//#define DEBUG_ANA_INC         1       /* for debugging diff between analog and incremental*/


#define AEA3   2   //SPIEN3
#define AEA4   0   //SPIEN1
#define AEA5   0   //SPIEN1
#define AEA6   2   //SPIEN2

#define CURRENT_BOARD_TYPE  BOARD_TYPE_4DC
#define JN					4		/* number of axes */
#define CAN_SYNCHRO_STEPS 5
#define BOARDSTATUSTIME 5000 //the time in ms between a BCAST_STATUS message and another


#endif