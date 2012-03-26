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
// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _CAN_PROTOCOL_H_
#define _CAN_PROTOCOL_H_

// - doxy begin -------------------------------------------------------------------------------------------------------
/*	@file 	    can_protocol.h
	@brief 	    This file defines can protocol commands and other macros about can protocol.
                IMPORTANT:  macro's values are defined in CANLOADER  MANUAL. Check it before changing values.  
                            For more information see http://eris.liralab.it/wiki/CanLoader
	@author     valentina.gaggero@iit.it
	@date	    03/10/2011
*/

// - external dependencies --------------------------------------------------------------------------------------------
// empty-section

// - public #define  --------------------------------------------------------------------------------------------------

//CAN Loader Commands. 

//Boot loader uses commands which belong to the CAN_MSG_CLASS_LOADER message class 
// For messages of class CAN_MSG_CLASS_LOADER the meaning of data/ID is defined as follows:
//  -------------------------- ---------------------
// |           11b            |           8B        |
//  -------  -------  -------  --------------------- 
// | 3b     | 4b     | 4b     |         B[0-7]      |
// |class	| Source | Dest   |        Payload      |
//  -------  -------  -------  --------------------- 

// polling messages
//  -------  -------  -------  -------  ------- 
// | 3b     | 4b     | 4b     |                |
// | class  | Source | Dest   |      ....      |
//  -------  -------  -------  -------  ------- 
// recursive messages
//  -------  -------  -------  -------  ------- 
// | 3b     | 4b     | 4b     |      B[0-7]    |
// |class	| Source | Type   |     Payload    |
//  -------  -------  -------  -------  ------- 

#define CAN_MSG_CLASS_LOADER            0x700
#define CAN_MSG_CLASS_POLLING_SENSOR   0x0200
#define CAN_MSG_CLASS_POLLING_MOTOR    0x0000

//commands polling
#define CAN_CMD_SET_BOARD_ADX           0x32 // Set board CAN address

//commands
#define CMD_BROADCAST                   0xFF
#define CMD_BOARD                       0x00
#define CMD_ADDRESS                     0x01
#define CMD_START                       0x02
#define CMD_DATA                        0x03
#define CMD_END                         0x04
#define CMD_ENABLELOG                   0x06
#define CMD_ERRORLOG                    0x07
#define CMD_GET_ADDITIONAL_INFO         0x0C //ignored by bootloader

//can command length
#define PAYLOAD_CMD_ADDRESS_LEN          0x07   //Data length (in byte) for ADDRESS command
#define PAYLOAD_CMD_START_LEN            0x05   //Data length (in byte) for START command
#define PAYLOAD_CMD_BOARD_LEN            0x02   //Data length (in byte) for BOARD command
#define PAYLOAD_CMD_ENABLELOG_LEN        0x02   //Data length (in byte) for ENABLELOG command




//macro for get class, source and destination from standard id of can msg
#define CAN_MSG_ID_GET_CLASS(MSG_ID)        (((MSG_ID)&0b11100000000) >> 8)
#define CAN_MSG_ID_GET_SOURCE(MSG_ID)       (((MSG_ID)&0b00011110000) >> 4)
#define CAN_MSG_ID_GET_DEST(MSG_ID)         ((MSG_ID)&0b00000001111)

 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

#if 0
typedef enum
{
    err_cmd_board = 1,          // #define CMD_BOARD_ERROR             0x1
    err_cmd_address = 2,        // #define CMD_ADDRESS_ERROR           0x2
    err_cmd_start = 3,          // #define CMD_START_ERROR             0x3
    err_cmd_unvailable = 4,     // #define COMMAND_UNAVAILABLE_WARNING 0x4
//#define CMBUFINDEX_ERROR            0x5 unused
    err_payload_empty = 6,       // #define PAYLOAD_ERROR               0x6 //se payload ha size = 0
    err_cmd_data = 7,           //I added this error for completeness
    err_cmd_enalog = 8,         //I added this error for completeness
    err_cmd_setBoardAddr = 8,   //I added this error for completeness
} can_protocol_error_t;
#endif

/** @typedef    typedef enum can_protocol_error_t
    @brief      contains pobbible parser's error. TODO: think how to manage error and insert them in canLoader document
 **/
typedef enum {
    canprotocol_errcode_unknown                 = 4,
    canprotocol_errcode_setboardaddress         = 9,           /**< */
    canprotocol_errcode_getadditionalinfo       = 32,         /**< */
    canprotocol_errcode_broadcast               = 33,                 /**< */
    canprotocol_errcode_board                   = 1,                     /**< */                    
    canprotocol_errcode_address                 = 2,                   /**< */ 
    canprotocol_errcode_data                    = 7,                      /**< */  
    canprotocol_errcode_start                   = 3,                     /**< */
    canprotocol_errcode_end                     = 34,                       /**< */ 
    canprotocol_errcode_enablelog               = 8                  /**< */
} canprotocol_errorcode_t;
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section

// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


