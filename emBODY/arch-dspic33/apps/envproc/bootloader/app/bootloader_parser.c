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

/* @file       bootloader_parser.c
    @brief      this file implements internals of the parser module. It verifies if a received message is correct and 
                fills a cmd data structure for external processing.
    @author     valentina.gaggero@iit.it / marco.accame@iit.it
    @date       10/03/2011
**/



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <string.h>

#include "eEcommon.h"
#include "bootloader_sm.h"

#include "can_protocol.h"





// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "bootloader_parser.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

#define acemor_removed


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    blparser_cfg_t          cfg;
} blparser_datastructure_t;




// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static eEresult_t s_parser_polling_msg(hal_can_frame_t *msg);
static eEresult_t s_parser_loader_msg(hal_can_frame_t *msg);

static void s_parser_reccmd_clear(void);
static void s_parser_reccmd_set_sender(hal_can_frame_t *frame);
static void s_parser_reccmd_set_opc_unknown(hal_can_frame_t *frame);
static void s_parser_reccmd_set_opc_setboardaddress(hal_can_frame_t *frame);
static void s_parser_reccmd_set_opc_getadditionalinfo(hal_can_frame_t *frame);
static void s_parser_reccmd_set_opc_broadcast(hal_can_frame_t *frame);
static void s_parser_reccmd_set_opc_board(hal_can_frame_t *frame);
static void s_parser_reccmd_set_opc_address(hal_can_frame_t *frame);
static void s_parser_reccmd_set_opc_data(hal_can_frame_t *frame);
static void s_parser_reccmd_set_opc_end(hal_can_frame_t *frame);
static void s_parser_reccmd_set_opc_start(hal_can_frame_t *frame);
static void s_parser_reccmd_set_opc_enablelog(hal_can_frame_t *frame);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

const blparser_cfg_t blparser_cfg_default =
{
    .dummy  = 0
};


static blparser_datastructure_t s_parser_data = { .cfg = { .dummy = 0 } };


static blparser_cmd_any_t s_parser_received_cmd = { .sender = {0}, .opc = blparser_opc_unknown, .mem = {0} };



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern eEresult_t blparser_init(const blparser_cfg_t *cfg)
{
    if(NULL == cfg)
    {
        cfg = &blparser_cfg_default;
    }
    
    memset(&s_parser_data, 0, sizeof(blparser_datastructure_t));
    
    memcpy(&s_parser_data.cfg, cfg, sizeof(blparser_cfg_t));
    
    s_parser_reccmd_clear();
    
    return(ee_res_OK);
}


extern eEresult_t blparser_parse_canframe(hal_can_frame_t *frame, const blparser_cmd_any_t **cmd)
{
    eEresult_t res = ee_res_NOK_generic;
    
    // pass the feedback as soon as possible
    *cmd = &s_parser_received_cmd;
    
    s_parser_reccmd_clear();
    
    if(0 == frame->size)
    {
        return(ee_res_NOK_generic);  
    }

    // If frame belongs to CAN_MSG_CLASS_POLLING
    if ( ( CAN_MSG_CLASS_POLLING_SENSOR == (frame->id & 0x700)) ||
         ( CAN_MSG_CLASS_POLLING_MOTOR == (frame->id & 0x700))  )
    {
        res = s_parser_polling_msg(frame);
    }
    else
    {
        res = s_parser_loader_msg(frame);
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

static eEresult_t s_parser_polling_msg(hal_can_frame_t *frame)
{
    eEresult_t res = ee_res_OK;
    
    s_parser_reccmd_set_sender(frame);
    
    switch(s_parser_received_cmd.sender.cmd)
    {    
		case CAN_CMD_SET_BOARD_ADX: // set board CAN address 
		{
            s_parser_reccmd_set_opc_setboardaddress(frame);
		} break; 

        case CMD_GET_ADDITIONAL_INFO:
        {
            s_parser_reccmd_set_opc_getadditionalinfo(frame);
        } break;
     
        default:
        {
            s_parser_reccmd_set_opc_unknown(frame);
        } break;
    }
    
    return(res);

}


static eEresult_t s_parser_loader_msg(hal_can_frame_t *frame)
{
   
    s_parser_reccmd_set_sender(frame);

    switch(s_parser_received_cmd.sender.cmd)
    {
    
        case CMD_BROADCAST:
        {
            s_parser_reccmd_set_opc_broadcast(frame);
        } break;
        
        case CMD_BOARD:
        {
            s_parser_reccmd_set_opc_board(frame);
        } break;
        

        case CMD_ADDRESS:
        {
            s_parser_reccmd_set_opc_address(frame);
        } break;

        case CMD_DATA: 
        {

            s_parser_reccmd_set_opc_data(frame);
   
        } break;

        
        case CMD_END:
        {
        
            s_parser_reccmd_set_opc_end(frame);
                  
        } break;

    
        case CMD_START:
        {
        
            s_parser_reccmd_set_opc_start(frame);

        } break;
    
        case CMD_ENABLELOG:
        {
        
            s_parser_reccmd_set_opc_enablelog(frame);
         
     	} break;
    

        default:
        {
            s_parser_reccmd_set_opc_unknown(frame);

        } break;
        
    } //end switch()

	return(ee_res_OK);
}



static void s_parser_reccmd_clear(void)
{
    memset(&s_parser_received_cmd, 0, sizeof(s_parser_received_cmd));
    // as the value of blparser_opc_unknown is zero ... the above memset would be enough.
    s_parser_received_cmd.opc           = blparser_opc_unknown;   
}

static void s_parser_reccmd_set_sender(hal_can_frame_t *frame)
{
    s_parser_received_cmd.sender.canid          = frame->id;
    s_parser_received_cmd.sender.cmd            = frame->data[0];
    s_parser_received_cmd.sender.errorinframe   = 0;
}

static void s_parser_reccmd_set_opc_unknown(hal_can_frame_t *frame)
{
    s_parser_received_cmd.opc           = blparser_opc_unknown;
}

static void s_parser_reccmd_set_opc_setboardaddress(hal_can_frame_t *frame)
{
    blparser_cmd_setboardaddress_t *cmd = (blparser_cmd_setboardaddress_t*)&s_parser_received_cmd;
    
    cmd->opc                        = blparser_opc_setboardaddress;
    cmd->adr                        = frame->data[1];
}

static void s_parser_reccmd_set_opc_getadditionalinfo(hal_can_frame_t *frame)
{
    blparser_cmd_getadditionalinfo_t *cmd = (blparser_cmd_getadditionalinfo_t*)&s_parser_received_cmd;
    
    cmd->opc                        = blparser_opc_getadditionalinfo;
 }

static void s_parser_reccmd_set_opc_broadcast(hal_can_frame_t *frame)
{
    blparser_cmd_broadcast_t *cmd = (blparser_cmd_broadcast_t*)&s_parser_received_cmd;
    
    cmd->opc                        = blparser_opc_broadcast;
}

static void s_parser_reccmd_set_opc_board(hal_can_frame_t *frame)
{
    blparser_cmd_board_t *cmd = (blparser_cmd_board_t*)&s_parser_received_cmd;
    
    if(frame->size < PAYLOAD_CMD_BOARD_LEN)
    {
        cmd->sender.errorinframe = 1;
    }
    
    cmd->opc                        = blparser_opc_board;
    cmd->updateeeprom               = frame->data[1];
}


static void s_parser_reccmd_set_opc_address(hal_can_frame_t *frame)
{
    blparser_cmd_address_t *cmd = (blparser_cmd_address_t*)&s_parser_received_cmd;
    
    if(frame->size < PAYLOAD_CMD_ADDRESS_LEN)
    {
        cmd->sender.errorinframe = 1;
    }    
    
    cmd->opc                        = blparser_opc_address;
    cmd->numbytes                   = frame->data[1];
    cmd->addressval[0]              = frame->data[2];
    cmd->addressval[1]              = frame->data[3];
    cmd->addressval[2]              = frame->data[5];
    cmd->addressval[3]              = frame->data[6];     
}


static void s_parser_reccmd_set_opc_data(hal_can_frame_t *frame)
{
    blparser_cmd_data_t *cmd = (blparser_cmd_data_t*)&s_parser_received_cmd;
    
    // we assume that the payload cnn contain any number of bytes, thus we dont put a control on a minimum payload size
    if(frame->size > 8)
    {
        cmd->sender.errorinframe = 1;
    }     
    
    cmd->opc                        = blparser_opc_data;
    cmd->size                       = frame->size;
    memcpy(cmd->data, frame->data, 8); // i copy the required bytes.    
}

static void s_parser_reccmd_set_opc_end(hal_can_frame_t *frame)
{
    blparser_cmd_end_t *cmd = (blparser_cmd_end_t*)&s_parser_received_cmd;
   
    cmd->opc                        = blparser_opc_end;
}


static void s_parser_reccmd_set_opc_start(hal_can_frame_t *frame)
{
    blparser_cmd_start_t *cmd = (blparser_cmd_start_t*)&s_parser_received_cmd;
   
    cmd->opc                        = blparser_opc_start;
}


static void s_parser_reccmd_set_opc_enablelog(hal_can_frame_t *frame)
{
    blparser_cmd_enablelog_t *cmd = (blparser_cmd_enablelog_t*)&s_parser_received_cmd;
    
    if(frame->size < PAYLOAD_CMD_ENABLELOG_LEN)
    {
        cmd->sender.errorinframe = 1;
    }    
    
    cmd->opc                        = blparser_opc_address;
    cmd->enable                     = (0 == frame->data[1]) ? (0) : (1);
}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



