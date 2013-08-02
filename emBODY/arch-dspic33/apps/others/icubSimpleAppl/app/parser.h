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

#ifndef _PARSER_H_
#define _PARSER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       parser.h
    @brief      this file provides parser module's interface. 
                the parser verifies if a received message is correct and asks the caller to do appropriate action.
    @author     valentina.gaggero@iit.it 
    @date       08/01/2013
**/


/** @defgroup parser parser for semple app of boards with dspic33
    The ... allows ... 
 
    
    
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"

#include "hal_can.h"



// - public #define  --------------------------------------------------------------------------------------------------
//icub can protocol specification
#define CAN_MSG_CLASS_POLLING_MOTOR    			0x00
#define CAN_MSG_CLASS_POLLING_SENSOR     		0x02
#define CAN_MSG_CLASS_LOADER             		0x07
/***************************************************************************/
// command messages of class ICUBPROTO_CLASS_CANLOADER 
/***************************************************************************/
#define CMD_BOARD                                       0x00
#define CMD_GET_ADDITIONAL_INFO                         0x0C
#define CMD_SET_ADDITIONAL_INFO	                        0x0D
#define CMD_BROADCAST                                   0xFF

//Type of board. it is necessary in response of CMD_BROADCAST
#define BOARD_TYPE_2FOC	   				                0x08
#define BOARD_TYPE_6SG     				                0x09
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 



    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------




// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern void parser_init(uint8_t can_addr)
    @brief      initialises the parser module 
    @param      can_addr can address of board 
    @return     
 **/
extern void parser_init(uint8_t can_addr);



/** @fn         uint8_t parser_parse_canframe(hal_can_frame_t *frame);
    @brief      parses a received can frame and send a repley if necessary
    @param      frame               the received CAN frame
    @return     0 in case of error else 1
 **/
extern uint8_t parser_parse_canframe(hal_can_frame_t *frame);


// - doxy end ---------------------------------------------------------------------------------------------------------

/** @}            
    end of group bl_parser 
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


