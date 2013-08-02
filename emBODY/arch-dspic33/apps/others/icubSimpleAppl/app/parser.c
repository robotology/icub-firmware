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

// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------

/* @file         parser.c
    @brief      this file implements internals of the parser module. It verifies if a received message is correct and 
                fills a cmd data structure for external processing.
    @author     valentina.gaggero@iit.it 
    @date       08/01/2013
**/



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <string.h>

#include "eEcommon.h"






// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "parser.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
# define FW_VERSION_MAJOR   1
# define FW_VERSION_MINOR   0
# define FW_VERSION_BUILD   0   

#ifdef  APPL_BOARD_6SG
    #define BOARD_TYPE_X        BOARD_TYPE_6SG
#elif defined APPL_BOARD_2FOC
    #define BOARD_TYPE_X        BOARD_TYPE_2FOC
#else
    #error "Please select the board the application must use: APPL_BOARD_XXX"
#endif



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint8_t s_parser_polling_msg(hal_can_frame_t *rec_frame, hal_can_frame_t *reply_frame);
static uint8_t s_parser_loader_msg(hal_can_frame_t *rec_frame, hal_can_frame_t *reply_frame);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static uint8_t s_can_addr = 0;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern void parser_init(uint8_t can_addr)
{
   s_can_addr = can_addr; 
}


extern uint8_t parser_parse_canframe(hal_can_frame_t *frame)
{
    uint8_t res = 0;   
    hal_can_frame_t reply_frame;
    if(0 == frame->size)
    {
        return(res);  
    }

    // If frame belongs to CAN_MSG_CLASS_POLLING
    if ( ( CAN_MSG_CLASS_POLLING_SENSOR == (frame->id & 0x700)) ||
         ( CAN_MSG_CLASS_POLLING_MOTOR == (frame->id & 0x700))  )
    {
        res = s_parser_polling_msg(frame, &reply_frame);
    }
    else
    {
		res =  s_parser_loader_msg(frame, &reply_frame);
    }
    
    if(reply_frame.size != 0)
    {
        reply_frame.id = (frame->id & 0x700) | (s_can_addr << 4) | ((frame->id & 0x0F0) >> 4);
        hal_can_put(hal_can_port1, &reply_frame, hal_can_send_normprio_now);
    }
    

    return(res);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static uint8_t s_parser_polling_msg(hal_can_frame_t *rec_frame, hal_can_frame_t *reply_frame)
{
    
    reply_frame->size = 0;
    
 /*   switch(s_parser_received_cmd.sender.cmd)
    {    
		case CAN_CMD_SET_BOARD_ADX: // set board CAN address 
		{
           ;
		} break; 

        case CMD_GET_ADDITIONAL_INFO:
        {
            ;
        } break;
     
        default:
        {
            ;
        } break;
    }
  */  
    return(1);

}


static uint8_t s_parser_loader_msg(hal_can_frame_t *rec_frame, hal_can_frame_t *reply_frame)
{
    uint8_t cmd;

    cmd = rec_frame->data[0];
    reply_frame->size = 0;
	
	
	switch (cmd)
    {
        case CMD_BROADCAST: 
        {
            if(1 != rec_frame->size)
            {   // incorrect number of parameters
                return(0);
            }
			
            reply_frame->size = 5;
            reply_frame->data[0] = cmd;
            reply_frame->data[1] = BOARD_TYPE_X; 
            reply_frame->data[2] = FW_VERSION_MAJOR;  
            reply_frame->data[3] = FW_VERSION_MINOR;  
            reply_frame->data[4] = FW_VERSION_BUILD;             
        } break;
                
    
        case CMD_BOARD:
        {
            asm ("reset");    // Jump to bootloader code      
        } break;
    
        case CMD_GET_ADDITIONAL_INFO:
        {
            ;
        } break;
    
        case CMD_SET_ADDITIONAL_INFO:
        {
            ;
        } break;
        
                
        default:
        {
			//ignore other msg!!
            return(1);
        } break;
        
        
    } //end switch

    return(1);    

}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



