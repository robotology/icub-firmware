/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero
 * email:   valentina.gaggero@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/


#ifndef __ICUB_PROTO_MESSAGES_H
#define __ICUB_PROTO_MESSAGES_H


/****************************************************************************/
// can messages class
/****************************************************************************/
#define ICUBPROTO_CLASS_POLLING_MOTORBOARD    			0x00
#define ICUBPROTO_CLASS_PERIODIC_MOTORBOARD   			0x01
#define ICUBPROTO_CLASS_POLLING_SENSBOARD     			0x02
#define ICUBPROTO_CLASS_PERIODIC_SENSBOARD    			0x03
#define ICUBPROTO_CLASS_CANLOADER             			0x07

/***************************************************************************/
// command messages of class ICUBPROTO_CLASS_PERIODIC_MOTORBOARD 
/***************************************************************************/
#define ICUBPROTO_PERIODICCMD_2FOC				        0
#define ICUBPROTO_PERIODICCMD_POSITION			        1
#define ICUBPROTO_PERIODICCMD_PID_VAL			        2
#define ICUBPROTO_PERIODICCMD_STATUS		        	3
#define ICUBPROTO_PERIODICCMD_CURRENT			        4
#define ICUBPROTO_PERIODICCMD_OVERFLOW			        5
#define ICUBPROTO_PERIODICCMD_PRINT				        6
#define ICUBPROTO_PERIODICCMD_VELOCITY			        7
#define ICUBPROTO_PERIODICCMD_EMSTO2FOC_DESIRED_CURRENT 15

//#define ICUBPROTO_PERIODIC_EMSTO2FOC_SETPOINT_CURRENT   D
//#define ICUBPROTO_PERIODIC_EMSTO2FOC_SETPOINT_VELOCITY  E


/***************************************************************************/
// command messages of class ICUBPROTO_CLASS_POLLING_MOTORBOARD 
/***************************************************************************/
#define ICUBPROTO_POLLINGCMD_CONTROLLER_RUN		 		1
#define ICUBPROTO_POLLINGCMD_CONTROLLER_IDLE			2

#define ICUBPROTO_POLLINGCMD_ENABLE_PWM_PAD				5
#define ICUBPROTO_POLLINGCMD_DISABLE_PWM_PAD			6
#define ICUBPROTO_POLLINGCMD_GET_CONTROL_MODE			7
#define ICUBPROTO_POLLINGCMD_SET_CONTROL_MODE			9
#define ICUBPROTO_POLLINGCMD_WRITE_FLASH_MEM			10

#define ICUBPROTO_POLLINGCMD_GET_ADDITIONAL_INFO		12
#define ICUBPROTO_POLLINGCMD_SET_ADDITIONAL_INFO		13

#define ICUBPROTO_POLLINGCMD_SET_DESIRED_VELOCITY		23
#define ICUBPROTO_POLLINGCMD_GET_DESIRED_VELOCITY		24

#define ICUBPROTO_POLLINGCMD_SET_ENCODER_POSITION		29

#define ICUBPROTO_POLLINGCMD_SET_BOARD_ID				50 // =0x32
#define ICUBPROTO_POLLINGCMD_GET_BOARD_ID				51

#define ICUBPROTO_POLLINGCMD_SET_MAX_VELOCITY			68
#define ICUBPROTO_POLLINGCMD_GET_MAX_VELOCITY			69

#define ICUBPROTO_POLLINGCMD_SET_CURRENT_LIMIT			72

#define ICUBPROTO_POLLINGCMD_GET_FIRMWARE_VERSION    	91

//#define ICUBPROTO_POLLINGCMD_SET_CURRENT_PID_PARAM 		92 //=0x5C
//#define ICUBPROTO_POLLINGCMD_GET_CURRENT_PID_PARAM 		93
//#define ICUBPROTO_POLLINGCMD_SET_VELOCITY_PID_PARAM 	94
//#define ICUBPROTO_POLLINGCMD_GET_VELOCITY_PID_PARAM 	95
//#define ICUBPROTO_POLLINGCMD_SET_DESIRED_CURRENT		96 //=0x60
//#define ICUBPROTO_POLLINGCMD_GET_DESIRED_CURRENT		97
//#define ICUBPROTO_POLLINGCMD_SET_PERIODIC_MSG_CONTENTS  98
//#define ICUBPROTO_POLLINGCMD_SET_I2T_PARAMS				99
//#define ICUBPROTO_POLLINGCMD_GET_I2T_PARAMS				100 //=0x64



#define ICUBPROTO_POLLINGCMD_SET_CURRENT_PID       		 92
#define ICUBPROTO_POLLINGCMD_GET_CURRENT_PID 		     93
#define ICUBPROTO_POLLINGCMD_SET_CURRENT_PIDLIMITS       94
#define ICUBPROTO_POLLINGCMD_GET_CURRENT_PIDLIMITS 		 95
#define ICUBPROTO_POLLINGCMD_SET_VELOCITY_PID            96
#define ICUBPROTO_POLLINGCMD_GET_VELOCITY_PID            97
#define ICUBPROTO_POLLINGCMD_SET_VELOCITY_PIDLIMITS      98
#define ICUBPROTO_POLLINGCMD_GET_VELOCITY_PIDLIMITS      99
#define ICUBPROTO_POLLINGCMD_SET_DISIRED_CURRENT		100
#define ICUBPROTO_POLLINGCMD_GET_DISIRED_CURRENT		101
#define ICUBPROTO_POLLINGCMD_SET_PERIODIC_MSG_CONTENTS 	102
#define ICUBPROTO_POLLINGCMD_SET_I2T_PARAMS			    103
#define ICUBPROTO_POLLINGCMD_GET_I2T_PARAMS			    104



//control mode (ICUBPROTO_POLLINGCMD_SET_CONTROL_MODE and ICUBPROTO_POLLINGCMD_GET_CONTROL_MODE)
#define ICUBPROTO_CONTROLMODE_IDLE						0x00 //default mode
#define ICUBPROTO_CONTROLMODE_POSITION 					0x01 //not implemented
#define ICUBPROTO_CONTROLMODE_VELOCITY					0x02
#define ICUBPROTO_CONTROLMODE_TORQUE					0x03
#define ICUBPROTO_CONTROLMODE_IMPEDANCE_POS				0x04
#define ICUBPROTO_CONTROLMODE_IMPEDANCE_VEL				0x05
#define ICUBPROTO_CONTROLMODE_CURRENT					0x06
#define ICUBPROTO_CONTROLMODE_CALIB_ABS_POS_SENS		0x10
#define ICUBPROTO_CONTROLMODE_CALIB_HARD_STOPS			0x20
#define ICUBPROTO_CONTROLMODE_HANDLE_HARD_STOPS			0x30
#define ICUBPROTO_CONTROLMODE_MARGIN_REACHED   			0x40
#define ICUBPROTO_CONTROLMODE_CALIB_ABS_AND_INC			0x41
#define ICUBPROTO_CONTROLMODE_OPENLOOP					0x50


/***************************************************************************/
// command messages of class ICUBPROTO_CLASS_CANLOADER 
/***************************************************************************/
#define CMD_BOARD                                       0x00
#define CMD_GET_ADDITIONAL_INFO                         0x0C
#define CMD_SET_ADDITIONAL_INFO	                        0x0D
#define CMD_BROADCAST                                   0xFF

//Type of board. it is necessary in response of CMD_BROADCAST
#define BOARD_TYPE_2FOC	   				                0x08

#endif

