/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EO_ICUBCANPROTOCOL_SPECIFICATION_H_
#define _EO_ICUBCANPROTOCOL_SPECIFICATION_H_


#ifdef __cplusplus
extern "C" {
#endif

// - doxy begin -------------------------------------------------------------------------------------------------------

/* @file       eo_ICUBCANPROTOcol_messages.h
    @brief      This file provides interface to ICUBCANPROTOcol
    @author    valentina.gaggero@iit.it
    @date       14/02/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------
// empty-section

// - public #define  --------------------------------------------------------------------------------------------------

/****************************************************************************/
// ICUBCANPROTO_POL_MB_CMD_ messages class
/****************************************************************************/
#define ICUBCANPROTO_CLASS_POLLING_MOTORBOARD    			0x00
#define ICUBCANPROTO_CLASS_PERIODIC_MOTORBOARD   			0x01
#define ICUBCANPROTO_CLASS_POLLING_SENSORBOARD     			0x02
#define ICUBCANPROTO_CLASS_PERIODIC_SENSORBOARD    			0x03
#define ICUBCANPROTO_CLASS_LOADER             			    0x07

#define ICUBCANPROTO_CLASS_MAXNUM             			    0x05



/***************************************************************************/
// command messages of class ICUBCANPROTO_CLASS_PERIODIC_MOTORBOARD 
/***************************************************************************/
#define ICUBCANPROTO_PER_MB_CMD_2FOC				        0
#define ICUBCANPROTO_PER_MB_CMD_POSITION			        1
#define ICUBCANPROTO_PER_MB_CMD_PID_VAL			        2
#define ICUBCANPROTO_PER_MB_CMD_STATUS		        	3
#define ICUBCANPROTO_PER_MB_CMD_CURRENT			        4
#define ICUBCANPROTO_PER_MB_CMD_OVERFLOW			        5
#define ICUBCANPROTO_PER_MB_CMD_PRINT				6
#define ICUBCANPROTO_PER_MB_CMD_VELOCITY			        7
#define ICUBCANPROTO_PER_MB_CMD_PID_ERROR			        8
#define ICUBCANPROTO_PER_MB_CMD_DEBUG				9

#define ICUBCANPROTO_PER_MB_CMD_MAXNUM			            ICUBCANPROTO_PER_MB_CMD_DEBUG + 1


/***************************************************************************/
// command messages of class ICUBCANPROTO_CLASS_POLLING_MOTORBOARD 
/***************************************************************************/
/*
ICUBCANPROTO_PER_MB_CMD_
ICUBCANPROTO_POL_SB_CMD_
ICUBCANPROTO_PER_SB_CMD_
*/


#define ICUBCANPROTO_POL_MB_CMD__NO_MESSAGE				0
#define ICUBCANPROTO_POL_MB_CMD__CONTROLLER_RUN		 	1
#define ICUBCANPROTO_POL_MB_CMD__CONTROLLER_IDLE			2
#define ICUBCANPROTO_POL_MB_CMD__TOGGLE_VERBOSE			3
#define ICUBCANPROTO_POL_MB_CMD__CALIBRATE_ENCODER			4
#define ICUBCANPROTO_POL_MB_CMD__ENABLE_PWM_PAD			5
#define ICUBCANPROTO_POL_MB_CMD__DISABLE_PWM_PAD			6
#define ICUBCANPROTO_POL_MB_CMD__GET_CONTROL_MODE			7
#define ICUBCANPROTO_POL_MB_CMD__MOTION_DONE				8
#define ICUBCANPROTO_POL_MB_CMD__SET_CONTROL_MODE			9
#define ICUBCANPROTO_POL_MB_CMD__WRITE_FLASH_MEM			10
#define ICUBCANPROTO_POL_MB_CMD__READ_FLASH_MEM			11
#define ICUBCANPROTO_POL_MB_CMD__GET_ADDITIONAL_INFO		12
#define ICUBCANPROTO_POL_MB_CMD__SET_ADDITIONAL_INFO		13
#define ICUBCANPROTO_POL_MB_CMD__SET_SPEED_ESTIM_SHIFT		14
#define ICUBCANPROTO_POL_MB_CMD__SET_DEBUG_PARAM		        18
#define ICUBCANPROTO_POL_MB_CMD__GET_DEBUG_PARAM		        19
#define ICUBCANPROTO_POL_MB_CMD__GET_ENCODER_POSITION		20
#define ICUBCANPROTO_POL_MB_CMD__SET_DESIRED_POSITION		21
#define ICUBCANPROTO_POL_MB_CMD__GET_DESIRED_POSITION		22
#define ICUBCANPROTO_POL_MB_CMD__SET_DESIRED_VELOCITY		23
#define ICUBCANPROTO_POL_MB_CMD__GET_DESIRED_VELOCITY		24
#define ICUBCANPROTO_POL_MB_CMD__SET_DESIRED_ACCELER		25
#define ICUBCANPROTO_POL_MB_CMD__GET_DESIRED_ACCELER		26
#define ICUBCANPROTO_POL_MB_CMD__POSITION_MOVE			27
#define ICUBCANPROTO_POL_MB_CMD__VELOCITY_MOVE			28
#define ICUBCANPROTO_POL_MB_CMD__SET_ENCODER_POSITION		29
#define ICUBCANPROTO_POL_MB_CMD__SET_P_GAIN				30
#define ICUBCANPROTO_POL_MB_CMD__GET_P_GAIN				31
#define ICUBCANPROTO_POL_MB_CMD__SET_D_GAIN				32
#define ICUBCANPROTO_POL_MB_CMD__GET_D_GAIN				33
#define ICUBCANPROTO_POL_MB_CMD__SET_I_GAIN				34
#define ICUBCANPROTO_POL_MB_CMD__GET_I_GAIN				35
#define ICUBCANPROTO_POL_MB_CMD__SET_ILIM_GAIN			        36
#define ICUBCANPROTO_POL_MB_CMD__GET_ILIM_GAIN			        37
#define ICUBCANPROTO_POL_MB_CMD__SET_OFFSET				38
#define ICUBCANPROTO_POL_MB_CMD__GET_OFFSET				39
#define ICUBCANPROTO_POL_MB_CMD__SET_SCALE				40
#define ICUBCANPROTO_POL_MB_CMD__GET_SCALE				41
#define ICUBCANPROTO_POL_MB_CMD__SET_TLIM				        42
#define ICUBCANPROTO_POL_MB_CMD__GET_TLIM				        43
#define ICUBCANPROTO_POL_MB_CMD__SET_DESIRED_TORQUE 	        44
#define ICUBCANPROTO_POL_MB_CMD__GET_DESIRED_TORQUE              45
#define ICUBCANPROTO_POL_MB_CMD__STOP_TRAJECTORY 		        46
#define ICUBCANPROTO_POL_MB_CMD__SET_BOARD_ID			        50
#define ICUBCANPROTO_POL_MB_CMD__GET_BOARD_ID			        51
#define ICUBCANPROTO_POL_MB_CMD__GET_PID_ERROR		        55
#define ICUBCANPROTO_POL_MB_CMD__GET_ERROR_STATUS			60
#define ICUBCANPROTO_POL_MB_CMD__GET_ENCODER_VELOCITY		61
#define ICUBCANPROTO_POL_MB_CMD__SET_COMMAND_POSITION		62
#define ICUBCANPROTO_POL_MB_CMD__GET_PID_OUTPUT			63
#define ICUBCANPROTO_POL_MB_CMD__SET_MIN_POSITION			64
#define ICUBCANPROTO_POL_MB_CMD__GET_MIN_POSITION			65
#define ICUBCANPROTO_POL_MB_CMD__SET_MAX_POSITION			66
#define ICUBCANPROTO_POL_MB_CMD__GET_MAX_POSITION			67
#define ICUBCANPROTO_POL_MB_CMD__SET_MAX_VELOCITY			68
#define ICUBCANPROTO_POL_MB_CMD__GET_MAX_VELOCITY			69
#define ICUBCANPROTO_POL_MB_CMD__GET_ACTIVE_ENCODER_POSITION  70
#define ICUBCANPROTO_POL_MB_CMD__SET_ACTIVE_ENCODER_POSITION  71
#define ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_LIMIT			72
#define ICUBCANPROTO_POL_MB_CMD__SET_BCAST_POLICY			73
#define ICUBCANPROTO_POL_MB_CMD__SET_VEL_SHIFT			74
#define ICUBCANPROTO_POL_MB_CMD__SET_OFFSET_ABS_ENCODER       75
#define ICUBCANPROTO_POL_MB_CMD__GET_OFFSET_ABS_ENCODER       76
#define ICUBCANPROTO_POL_MB_CMD__SET_SMOOTH_PID               77
#define ICUBCANPROTO_POL_MB_CMD__SET_TORQUE_PID               78
#define ICUBCANPROTO_POL_MB_CMD__GET_TORQUE_PID               79
#define ICUBCANPROTO_POL_MB_CMD__SET_TORQUE_PIDLIMITS         80
#define ICUBCANPROTO_POL_MB_CMD__GET_TORQUE_PIDLIMITS         81
#define ICUBCANPROTO_POL_MB_CMD__SET_POS_PID                  82
#define ICUBCANPROTO_POL_MB_CMD__GET_POS_PID                  83
#define ICUBCANPROTO_POL_MB_CMD__SET_POS_PIDLIMITS            84
#define ICUBCANPROTO_POL_MB_CMD__GET_POS_PIDLIMITS   	   85
#define ICUBCANPROTO_POL_MB_CMD__SET_VEL_TIMEOUT		   86
#define ICUBCANPROTO_POL_MB_CMD__SET_IMPEDANCE_PARAMS         87
#define ICUBCANPROTO_POL_MB_CMD__GET_IMPEDANCE_PARAMS         88
#define ICUBCANPROTO_POL_MB_CMD__SET_IMPEDANCE_OFFSET         89
#define ICUBCANPROTO_POL_MB_CMD__GET_IMPEDANCE_OFFSET         90
#define ICUBCANPROTO_POL_MB_CMD__GET_FIRMWARE_VERSION         91
#define ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_PID       		 92
#define ICUBCANPROTO_POL_MB_CMD__GET_CURRENT_PID 		     93
#define ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_PIDLIMITS       94
#define ICUBCANPROTO_POL_MB_CMD__GET_CURRENT_PIDLIMITS 		 95
#define ICUBCANPROTO_POL_MB_CMD__SET_VELOCITY_PID            96
#define ICUBCANPROTO_POL_MB_CMD__GET_VELOCITY_PID            97
#define ICUBCANPROTO_POL_MB_CMD__SET_VELOCITY_PIDLIMITS      98
#define ICUBCANPROTO_POL_MB_CMD__GET_VELOCITY_PIDLIMITS      99
#define ICUBCANPROTO_POL_MB_CMD__SET_DISIRED_CURRENT		100
#define ICUBCANPROTO_POL_MB_CMD__GET_DISIRED_CURRENT		101
#define ICUBCANPROTO_POL_MB_CMD__SET_PERIODIC_MSG_CONTENTS 	102
#define ICUBCANPROTO_POL_MB_CMD__SET_I2T_PARAMS			    103
#define ICUBCANPROTO_POL_MB_CMD__GET_I2T_PARAMS			    104

#define ICUBCANPROTO_POL_MB_CMD_MAXNUM                     ICUBCANPROTO_POL_MB_CMD__GET_I2T_PARAMS + 1 





/***************************************************************************/
// command messages of class ICUBCANPROTO_CLASS_POLLING_SENSORBOARD 
/***************************************************************************/
#define ICUBCANPROTO_POL_SB_CMD__NONE              0x0
#define ICUBCANPROTO_POL_SB_CMD__SET_IIR           0x1  // select IIR filters parameters 
#define ICUBCANPROTO_POL_SB_CMD__SET_MATRIX_RC     0x3  // Set SG to TF trasformation matrix 
#define ICUBCANPROTO_POL_SB_CMD__SET_CH_DAC        0x4  // set DAC for channel x
#define ICUBCANPROTO_POL_SB_CMD__SELECT_ACTIVE_CH  0x5  // set active channels (activation mask) only active channels are transmitted
#define ICUBCANPROTO_POL_SB_CMD__CALIBRATE_OFFSET  0x6  // set the calibration offset
#define ICUBCANPROTO_POL_SB_CMD__SET_TXMODE        0x7  // set continuous/on demand transmission mode
#define ICUBCANPROTO_POL_SB_CMD__SET_CANDATARATE   0x8  // set board CAN speed in milliseconds minimum, datarate 210ms  
#define ICUBCANPROTO_POL_SB_CMD__SAVE2EE           0x9  // save Config to EE
#define ICUBCANPROTO_POL_SB_CMD__GET_MATRIX_RC     0xA  // Get TF trasformation matrix 
#define ICUBCANPROTO_POL_SB_CMD__GET_CH_DAC        0xB  // Get DAC for channel x
#define ICUBCANPROTO_POL_SB_CMD__GET_CH_ADC        0xC  // Get ADC for channel x
#define ICUBCANPROTO_POL_SB_CMD__FILTER_EN         0xD  // ENABLE/DISABLES FILTER
#define ICUBCANPROTO_POL_SB_CMD__MUX_EN            0xE  // ENABLE/DISABLES MUX
#define ICUBCANPROTO_POL_SB_CMD__MUX_NUM           0xF
#define ICUBCANPROTO_POL_SB_CMD__SET_RESOLUTION    0x10 //set data resolution
#define ICUBCANPROTO_POL_SB_CMD__SET_MATRIX_G      0x11 //set matrix gain
#define ICUBCANPROTO_POL_SB_CMD__GET_MATRIX_G      0x12 //get matrix gain
#define ICUBCANPROTO_POL_SB_CMD__SET_CALIB_TARE    0x13 //set/reset calibration tare
#define ICUBCANPROTO_POL_SB_CMD__GET_CALIB_TARE    0x14 //get calibration tare
#define ICUBCANPROTO_POL_SB_CMD__SET_CURR_TARE     0x15 //set/reset current tare
#define ICUBCANPROTO_POL_SB_CMD__GET_CURR_TARE     0x16 //get current tare
#define ICUBCANPROTO_POL_SB_CMD__SET_FULL_SCALES   0x17 //set/reset current tare
#define ICUBCANPROTO_POL_SB_CMD__GET_FULL_SCALES   0x18 //get current tare
#define ICUBCANPROTO_POL_SB_CMD__SET_SERIAL_NO     0x19 //set/reset current tare
#define ICUBCANPROTO_POL_SB_CMD__GET_SERIAL_NO     0x1A //get current tare
#define ICUBCANPROTO_POL_SB_CMD__GET_EEPROM_STATUS 0x1B //get eeprom status (saved/not saved)
#define ICUBCANPROTO_POL_SB_CMD__GET_FW_VERSION	   0X1C //get fw version and check can protocol compatibility
#define ICUBCANPROTO_POL_SB_CMD__SET_BOARD_ADX     0x32 // Set board CAN address


//since CUBCANPROTO_POL_SB_CMD__SET_BOARD_ADX hasn't continuous number it will manged by exception function and
//not by lookup bable
#define ICUBCANPROTO_POL_SB_CMD__CMD_MAXNUM                 ICUBCANPROTO_POL_SB_CMD__GET_FW_VERSION +1

/***************************************************************************/
// command messages of class ICUBCANPROTO_CLASS_PERIODIC_SENSORBOARD 
/***************************************************************************/
#define ICUBCANPROTO_PER_SB_CMD__FORCE_VECTOR      0xA // Transmit Torque values t1 t1 t2 t2 t3 t3
#define ICUBCANPROTO_PER_SB_CMD__TORQUE_VECTOR     0xB // Transmit Force  values f1 f1 f2 f2 f3 f3
#define ICUBCANPROTO_PER_SB_CMD__HES0TO6           0xC //hall effect sensors from 0  to 6  *8bits
#define ICUBCANPROTO_PER_SB_CMD__HES7TO14          0xD //hall effect sensors from 7  to 14  *8bits


//the following msg are unused!!!
//#define ICUBCANPROTO_PER_SB_CMD__HES0TO3           0xC // hall effect sensors from 0  to 3  *16bits
//#define ICUBCANPROTO_PER_SB_CMD__HES4TO7           0xD // hall effect sensors from 4  to 7  *16bits
//#define ICUBCANPROTO_PER_SB_CMD__HES8TO11          0xE // hall effect sensors from 8  to 11 *16bits
//#define ICUBCANPROTO_PER_SB_CMD__HES12TO14         0xF // hall effect sensors from 12 to 14 *16bits
//there there aren't skin's messages!!!

#define ICUBCANPROTO_PER_SB_CMD__CMD_MAXNUM         6




/***************************************************************************/
// command messages of class 4 ==> SKIN  
/***************************************************************************/
#define ICUBCANPROTO_POL_SK_CMD__TACT_SETUP         76


 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef     enum             eo_icubCanProto_boardType_t
    @brief      contains board type can be mounted on robot.
                Important: its values are defined in according with icubInterface 
                (see downloader.h in canLoader module)
                VALE: utile qui in interfaccia o da mettere in hid???
 **/
typedef enum
{
    eo_icubCanProto_boardType__dsp     = 0,
    eo_icubCanProto_boardType__pic     = 1,
    eo_icubCanProto_boardType__2dc     = 2,
    eo_icubCanProto_boardType__4dc     = 3,
    eo_icubCanProto_boardType__bll     = 4,
    eo_icubCanProto_boardType__skin    = 5,
    eo_icubCanProto_boardType__strain  = 6,
    eo_icubCanProto_boardType__mais    = 7,
    eo_icubCanProto_boardType__2foc    = 8,
    eo_icubCanProto_boardType__6sg     = 9,
    eo_icubCanProto_boardType__unknown = 255
} eo_icubCanProto_boardType_t;


/** @typedef    typedef     enum             eo_icubCanProto_motorAxis_t
    @brief      each CAN board can manage two axises.
                Note: a can board is different from a physical board: the seccond can combine
                two CAN board in a single board object. a Can board ca be identified by unique 
                CAN address on the same CAN bus.
                Each CAN board can manage two axis at most, 
                but a physical board can manage 4 axises at most.
 **/
typedef enum
{
    eo_icubCanProto_mAxis_0 = 0,
    eo_icubCanProto_mAxis_1 = 1
} eo_icubCanProto_motorAxis_t;


/** @typedef    typedef     uint8_t             eo_icubCanProto_canBoardAddress_t
    @brief      contains can address of board. 
                the value must be in the range [1, E] 
    @todo       VALE: fare enum?????
 **/
typedef     uint8_t             eo_icubCanProto_canBoardAddress_t;



/** @typedef    typedef     struct             eo_icubCanProto_msgDestination_t
    @brief      The destination of message belong to icub can protocol is a 
                particuar axis managed by a board.
 **/
typedef struct
{
    uint32_t axis:4;          // usa eo_icubCanProto_motorAxis_t
    uint32_t canAddr:4;       //eo_icubCanProto_canBoardAddress_t;
} eo_icubCanProto_msgDestination_t;
                                     
#define ICUBCANPROTO_MSGDEST_CREATE(axis, canAddr)   ((eo_icubCanProto_msgDestination_t)(axis<<4 |canAddr))





/** @typedef    typedef     uint8_t             eo_icubCanProto_command_t
    @brief      contains command belong to icib can protocol.
                its values are defined in eo_icubCanProtocol_messages.h 
 **/
typedef     uint8_t             eo_icubCanProto_msgCommand_cmdId_t;



/** @typedef    typedef     uint8_t             eo_icubCanProto_msgCommand_class_t
    @brief      contains command belong to icib can protocol.
                its values are defined in eo_icubCanProtocol_messages.h 
                VALE: e' utile messo qui o va messo nel _hid??
 **/
typedef enum
{
    eo_icubCanProto_msgCmdClass_pollingMotorBoard      = 0,
    eo_icubCanProto_msgCmdClass_periodicMotorBoard     = 1,
    eo_icubCanProto_msgCmdClass_pollingSensorBoard     = 2,
    eo_icubCanProto_msgCmdClass_periodicSensorBoard    = 3,
    eo_icubCanProto_msgCmdClass_skinBoard              = 4, //ATTENTION: currently it is not used by other fw. Insert here to manage skin message like other calss messages.
    eo_icubCanProto_msgCmdClass_loader                 = 7    
}eo_icubCanProto_msgCommand_class_t;


/** @typedef    typedef     uint8_t             eo_icubCanProto_msgCommand_t
    @brief      contains command belong to icib can protocol.
                its values are defined in eo_icubCanProtocol_messages.h 
                VALE: e' utile messo qui o va messo nel _hid??
 **/
typedef struct
{
    uint16_t class:8;          //eo_icubCanProto_msgCommand_class_t
    uint16_t cmdId:8;         //eo_icubCanProto_msgCommand_id_t;
} eo_icubCanProto_msgCommand_t;
#define ICUBCANPROTO_MSGCOMMAND_CREATE(class, cmdId)   ((eo_icubCanProto_msgCommand_t)(class<<8 |cmdId))


    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


