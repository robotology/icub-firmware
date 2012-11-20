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

/* @file       bootloader_sm.c
    @brief      this file implements the sm of the bootloader for boards with dspic33fj128mc802.
    @author     valentina.gaggero@iit.it / marco.accame@iit.it
    @date       08/17/2011
*/

//#warning -> todo: what about using a better state machine defined in eEcommon.c ??

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "string.h"
#include "stdint.h"
#include "eEcommon.h"
#include "hal_can.h"
#include "hal_led.h"

#include "bootloader.h"
#include "bootloader_parser.h"
#include "memory_manager.h"

#include "can_protocol.h"

#include "board_func.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "bootloader_sm.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section 



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define led_red     hal_led1

#define BLSM_GETCANID(src, dst)     ( CAN_MSG_CLASS_LOADER | ( ( (src ) & 0xF ) << 4 ) | ( (dst) & 0xF ) )


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section
 


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



typedef struct
{
    blsm_state_t            state;
    blsm_state_sub_t        substate;
    blsm_event_t            nextevent;
} blsm_sm_t;

typedef struct
{
    uint8_t     enable_log;
    uint32_t    brdadr;
} blsm_options_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_blsm_set_state(blsm_state_t state, blsm_state_sub_t substate);


// parse the received can frame
static void s_blsm_parse_canframe(void);

// can transmission
static void s_blsm_send_can_frame(uint16_t id, uint8_t *data, uint8_t data_len);


// on execution of received commands
static void s_blsm_process_cmd_unknown(const blparser_cmd_any_t* cmdany);
static void s_blsm_process_cmd_setboardaddress(const blparser_cmd_any_t* cmdany);
static void s_blsm_process_cmd_getadditionalinfo(const blparser_cmd_any_t* cmdany);
static void s_blsm_process_cmd_broadcast(const blparser_cmd_any_t* cmdany);
static void s_blsm_process_cmd_board(const blparser_cmd_any_t* cmdany);
static void s_blsm_process_cmd_address(const blparser_cmd_any_t* cmdany);
static void s_blsm_process_cmd_data(const blparser_cmd_any_t* cmdany);
static void s_blsm_process_cmd_start(const blparser_cmd_any_t* cmdany);
static void s_blsm_process_cmd_end(const blparser_cmd_any_t* cmdany);
static void s_blsm_process_cmd_enablelog(const blparser_cmd_any_t* cmdany);

// helper

static void s_blsm_send_can_frame(uint16_t id, uint8_t *data, uint8_t data_len);
static void s_blsm_on_error(const blparser_cmd_any_t* cmdany, uint8_t errorcode);
//static void s_blsm_on_warning(const blparser_cmd_any_t* cmdany, uint8_t warningcode); currently not used
static void s_blsm_sendversion(const blparser_cmd_any_t* cmdany);
static void s_blsm_send_ack(const blparser_cmd_any_t* cmdany);
static void s_blsm_memory_set_address_firstone(const blparser_cmd_any_t* cmdany);
static void s_blsm_memory_set_address_subsequent(const blparser_cmd_any_t* cmdany);
static void s_blsm_memory_set_data(const blparser_cmd_any_t* cmdany);




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static blsm_sm_t s_blsm_thesm =
{
    .state          = blsm_state_idle,
    .substate       = blsm_state_any_sub_none,
    .nextevent      = blsm_evt_none
};


static hal_can_frame_t s_blsm_canframe;

static blsm_options_t s_blsm_options =
{
    .enable_log     = 0,
    .brdadr         = 0
};

static uint8_t s_blsm_txdata[8];

static mm_hex32file_lineinfo_t s_blsm_lineinfo;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern void blsm_init(void)
{
    // parts specifics to the state machine
    s_blsm_set_state(blsm_state_idle, blsm_state_any_sub_none);
    blsm_set_evt(blsm_evt_none);
           
    // whatever is required by the state machine which is not used by the application
    
    // the parser
    blparser_init(NULL); 
    
    // init the memory manager with the partition table of the device
    mm_init(bl_partitiontable_get());
    
    // init some other data structure
    
    s_blsm_options.enable_log   = 0;
    s_blsm_options.brdadr       = bl_address_get();

    // start the timing
    bl_ticktimer_start();
}

extern void blsm_set_evt(blsm_event_t evt)
{
    s_blsm_thesm.nextevent      = evt;
}

extern blsm_event_t blsm_get_evt(void)
{
    blsm_event_t evt = s_blsm_thesm.nextevent;
    
    if(blsm_evt_none != evt)
    {
        // reset the internal event
        blsm_set_evt(blsm_evt_none);
        return(evt);
    }
    else
    {
        // the default event. 
        // it can be modified by some states if something happens: a received canframe or an expired timer or ...
        evt = blsm_evt_none;
        
        switch(s_blsm_thesm.state)
        {
            case blsm_state_idle:
            {
                hal_result_t ret = hal_can_get(hal_can_port1, &s_blsm_canframe, NULL);

                if(hal_res_OK == ret)
                {   // if we have just received a valid canframe
                    evt = blsm_evt_validcanmsgreceived;
                }
                else if((hal_res_NOK_nodata == ret) && (ee_res_OK == bl_ticktimer_has_just_expired()))
                {   // if we did not receive any packet but the timer has just expired
                    evt = blsm_evt_tickexpired;
                }
                else if(hal_res_NOK_generic == ret)
                {   // in case of can reception error
                    evt = blsm_evt_goto_error;
                }

            } break;
            
            case blsm_state_error:
            {
                // nothing. but we can add in here any emission of events to enhance error management ... blink a led or else. 
            } break;

            case blsm_state_exec:
            {
                // nothing. 
                // actually we should not come in here because on entering the exec state the application immediately jumps to another e-process 
                // maybe we can add a:
                evt = blsm_evt_goto_error;
            } break;            

            case blsm_state_update:
            {
                hal_result_t ret = hal_can_get(hal_can_port1, &s_blsm_canframe, NULL);

                if(hal_res_OK == ret)
                {   // if we have just received a valid canframe ...
                    evt = blsm_evt_validcanmsgreceived;
                }
                else if(hal_res_NOK_nodata == ret)
                {   // the canframe is traveling and will arrive. just be patient
                    ;
                }
                else if(hal_res_NOK_generic == ret)
                {   // in case of can reception error
                    evt = blsm_evt_goto_error;
                }                
                
                
            } break;
            
            default:
            {   // there are no other states, but still add the following line to drive 
                // the state machine to error in case we add a state and we dont manage it.
                evt = blsm_evt_goto_error;
            } break;
        
        }
        
        return(evt);
    }
    
    return(blsm_evt_none);
}


extern void blsm_process_evt(blsm_event_t evt)
{
    if(blsm_evt_none == evt)
    {
        return;
    }
    
    // reset the internal event. it as reset already in blsm_get_evt(), but better do it again
    blsm_set_evt(blsm_evt_none);
    
    // events to be managed by any state ...
    switch(evt)
    {
        case blsm_evt_goto_error:
        {
             s_blsm_set_state(blsm_state_error, blsm_state_any_sub_none);
            
            // -- ON-ENTRY:
            
            // i may clean everything.
            // if the timer is running ... if the can is active ... the action depends on where the error is issued: in idle or in update state?
            // maybe now i can do something such as:
            // s_bl_errorhandler_start();
            // and later on execute a function inside the state error
            // s_bl_errorhandler_tick();
            
            return;
        } break; 

        case blsm_evt_goto_exec:
        {
            s_blsm_set_state(blsm_state_exec, blsm_state_any_sub_none);
            
            // -- ON-ENTRY:        

            // run application
            bl_run_application();
            
            // i should never be in here ... if i go in here i emit a goto_error
            blsm_set_evt(blsm_evt_goto_error);

            return;
        } break;  

        case blsm_evt_goto_update_connected:
        {
            s_blsm_set_state(blsm_state_update, blsm_state_update_sub_connected);
            
            // -- ON-ENTRY:        

            return;
        } break; 
        
        default:
        {
        } break;

    }
    
    // events to be managed only by some states ...
   
    switch(s_blsm_thesm.state)
    {
        case blsm_state_idle:
        {
            switch(evt)
            {
                case blsm_evt_tickexpired:    
                {
                    uint8_t lastexpiry = 0;
                    bl_on_ticktimer_expired(&lastexpiry);

#if defined(UPDTBL)
#else                    
                    if(1 == lastexpiry)
                    {
                        blsm_set_evt(blsm_evt_goto_exec);
                    }
#endif
                    
                } break;
                
                case blsm_evt_validcanmsgreceived:
                {
                     
                    s_blsm_parse_canframe();
                   
                } break;

#if 0
                case blsm_evt_goto_update:
                {   
                    s_blsm_set_state(blsm_state_update, blsm_state_any_sub_none);
                    
                    // and we issue an event to go to the substate connected
                    blsm_set_evt(blsm_evt_goto_update_connected);
                  
                    // --- ON-ENTRY:
                    // nothing so far.
                    
                } break; 
#endif                
                case blsm_evt_goto_update_readytorecbin:
                {   // dont pass through update_connected state and go directly to update_readytorecbin
                    s_blsm_set_state(blsm_state_update, blsm_state_update_sub_readytorecbin);
                    
                    // --- ON-ENTRY:                 
                    // do something ??                
                } break;
                default:
                {
                    // any other event is ignored
                } break;                
        
            };
            
            return;
            
        } break;
        
        case blsm_state_error:
        {
            // nothing. but we could blink a led or tick an handler
            // s_bl_errorhandler_tick();
        } break;

        case blsm_state_exec:
        {
            // we should never be in here because the jump is done on transition. i put the following just in case ...
            blsm_set_evt(blsm_evt_goto_error);
        } break;            

        case blsm_state_update:
        {
        
            switch(evt)
            {
                case blsm_evt_validcanmsgreceived:
                {
                    // evaluate the canmsg with the parser. pass the pointer to my state machine to teh parser so that it can
                    // do something. someone may also set the nextevent ... to blsm_evt_goto_update_* or blsm_evt_goto_exec or even blsm_evt_goto_error
                    //#warning -> add parsing of the canframe s_blsm_canframe
                    // 1. call the parser_parse() and retrieve the feedback.
                    // 2. if what is requested is ok then do it. else ignore it or send an error message                    
                    
                    s_blsm_parse_canframe();
                    
                } break; 
                
                case blsm_evt_goto_update_connected:
                {
                    s_blsm_set_state(blsm_state_update, blsm_evt_goto_update_connected);
                    
                    // --- ON-ENTRY: 
                    // do something ??
                } break;
                
                case blsm_evt_goto_update_readytorecbin:
                {
                    s_blsm_set_state(blsm_state_update, blsm_state_update_sub_readytorecbin);
                    
                    // --- ON-ENTRY:                 
                    // do something ??                
                } break;
                
                case blsm_evt_goto_update_writingbin:
                {
                    s_blsm_set_state(blsm_state_update, blsm_state_update_sub_writingbin);
                    
                    // --- ON-ENTRY:                 
                    // do something ??                  

                } break;
                
                case blsm_evt_goto_update_finishtowrite:
                {
                    s_blsm_set_state(blsm_state_update, blsm_state_update_sub_finishtowrite);
                    
                    // --- ON-ENTRY:                 
                    // do something ??                 
                } break;
                
                case blsm_evt_goto_update_end:
                {
                    s_blsm_set_state(blsm_state_update, blsm_state_update_sub_end);
                    
                    // --- ON-ENTRY:  
                    // do parser command                     
                    blsm_set_evt(blsm_evt_goto_exec);                     
                } break;    
                
                default:
                {
                    // ignored
                } break;                
        
            };        

        } break;
        
        default:
        {
        } break;
    
    }
    

}

#if 0
extern void blsm_get_state_substate(blsm_state_t* state, blsm_state_sub_t* sub)
{
    *state = s_blsm_thesm.state;
    *sub   = s_blsm_thesm.substate;
}
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_blsm_set_state(blsm_state_t state, blsm_state_sub_t substate)
{
    s_blsm_thesm.state          = state;
    s_blsm_thesm.substate       = substate;
}

static void s_blsm_parse_canframe(void)
{
    const blparser_cmd_any_t *cmdany = NULL;
    
    blparser_parse_canframe(&s_blsm_canframe, &cmdany);
    
    if(NULL == cmdany)
    {
        return;
    }
        
    switch(cmdany->opc)
    {
        case blparser_opc_unknown:       
        {   
            s_blsm_process_cmd_unknown(cmdany);
        } break;
        
        case blparser_opc_setboardaddress:       
        {   
            s_blsm_process_cmd_setboardaddress(cmdany);
        } break;     
        
        case blparser_opc_getadditionalinfo:       
        {   
            s_blsm_process_cmd_getadditionalinfo(cmdany);
        } break;     
        
        case blparser_opc_broadcast:       
        {   
            s_blsm_process_cmd_broadcast(cmdany);
        } break;  
        
        case blparser_opc_board:       
        {   
            s_blsm_process_cmd_board(cmdany);
        } break;   
        
        case blparser_opc_address:       
        {   
            s_blsm_process_cmd_address(cmdany);
        } break;
        
        case blparser_opc_data:       
        {   
            s_blsm_process_cmd_data(cmdany);
        } break;  
        
        case blparser_opc_start:       
        {   
            s_blsm_process_cmd_start(cmdany);
        } break;     
        
        case blparser_opc_end:       
        {   
            s_blsm_process_cmd_end(cmdany);
        } break;  
        
        case blparser_opc_enablelog:       
        {   
            s_blsm_process_cmd_enablelog(cmdany);
        } break;     

        default:
        {
        } break;
    }

}



// --- process commands

static void s_blsm_process_cmd_unknown(const blparser_cmd_any_t* cmdany)
{

}

static void s_blsm_process_cmd_setboardaddress(const blparser_cmd_any_t* cmdany)
{
    blparser_cmd_setboardaddress_t* cmd = (blparser_cmd_setboardaddress_t*)cmdany;
    
    if((1 == cmdany->sender.errorinframe) || (0 == cmd->adr) || (cmd->adr > 15))
    {
        s_blsm_on_error(cmdany, canprotocol_errcode_setboardaddress);
    }
    else
    {
        bl_address_set(cmd->adr);
        bl_canfilters_set(cmd->adr);
        s_blsm_options.brdadr = bl_address_get();
    }
}

static void s_blsm_process_cmd_getadditionalinfo(const blparser_cmd_any_t* cmdany)
{
    blparser_cmd_getadditionalinfo_t* cmd = (blparser_cmd_getadditionalinfo_t*)cmdany;
    cmd = cmd; // to remove warnings
    
    if((1 == cmdany->sender.errorinframe))
    {
        s_blsm_on_error(cmdany, canprotocol_errcode_getadditionalinfo);
    }
    else
    {
        // todo: decide what to do for this command
    }
}


static void s_blsm_process_cmd_broadcast(const blparser_cmd_any_t* cmdany)
{
    blparser_cmd_broadcast_t* cmd = (blparser_cmd_broadcast_t*)cmdany;
    cmd = cmd; // to remove warnings
    
    if((1 == cmdany->sender.errorinframe))
    {
        s_blsm_on_error(cmdany, canprotocol_errcode_broadcast);
    }
    else
    {   // so far i dont put any filter on the state ... but i wonder if that is correct
    
        // send back the version
        s_blsm_sendversion(cmdany);
        
        // change state
        blsm_set_evt(blsm_evt_goto_update_connected);
    }
}


static void s_blsm_process_cmd_board(const blparser_cmd_any_t* cmdany)
{
    blparser_cmd_board_t* cmd = (blparser_cmd_board_t*)cmdany;
    cmd = cmd; // to remove warnings
    
    if((1 == cmdany->sender.errorinframe))
    {
        s_blsm_on_error(cmdany, canprotocol_errcode_board);
    }
    else if( (blsm_state_idle == s_blsm_thesm.state) || 
             ((blsm_state_update == s_blsm_thesm.state) && (blsm_state_update_sub_connected == s_blsm_thesm.substate)) 
           )
    {   // only in states idle or update-connected 
    
        // tell the memory manager if it also there is an update of the eeprom
        mm_update_eeprom(cmd->updateeeprom);
        
        // send ack of the msg
        s_blsm_send_ack(cmdany);

        // change state
        blsm_set_evt(blsm_evt_goto_update_readytorecbin);
    }
}


static void s_blsm_process_cmd_address(const blparser_cmd_any_t* cmdany)
{
    blparser_cmd_address_t* cmd = (blparser_cmd_address_t*)cmdany;
    cmd = cmd; // to remove warnings
    
    if((1 == cmdany->sender.errorinframe))
    {
        s_blsm_on_error(cmdany, canprotocol_errcode_address);
    }
    else if( ((blsm_state_update == s_blsm_thesm.state) && (blsm_state_update_sub_readytorecbin == s_blsm_thesm.substate)) )
    {   // in state update-readytorecbin we write the first address and then we change state to update-writingbin 
    
        // write the first address
        s_blsm_memory_set_address_firstone(cmdany);
        
        // change state
        blsm_set_evt(blsm_evt_goto_update_writingbin);
    }
    else if( ((blsm_state_update == s_blsm_thesm.state) && (blsm_state_update_sub_writingbin == s_blsm_thesm.substate)) )
    {   // in state update-writingbin we write the other addresses
        
        // write a subsequent address
        s_blsm_memory_set_address_subsequent(cmdany);
    }
}


static void s_blsm_process_cmd_data(const blparser_cmd_any_t* cmdany)
{
    blparser_cmd_data_t* cmd = (blparser_cmd_data_t*)cmdany;
    cmd = cmd; // to remove warnings
    
    if((1 == cmdany->sender.errorinframe))
    {
        s_blsm_on_error(cmdany, canprotocol_errcode_data);
    }
    else if( ((blsm_state_update == s_blsm_thesm.state) && (blsm_state_update_sub_writingbin == s_blsm_thesm.substate)) )
    {   // only in state update-writingbin we write the received data
        
        // write the data and ... see helper function
        s_blsm_memory_set_data(cmdany);
    }
}


static void s_blsm_process_cmd_start(const blparser_cmd_any_t* cmdany)
{
    blparser_cmd_start_t* cmd = (blparser_cmd_start_t*)cmdany;
    cmd = cmd; // to remove warnings
    
    if((1 == cmdany->sender.errorinframe))
    {
        s_blsm_on_error(cmdany, canprotocol_errcode_start);
    }
    else if( ((blsm_state_update == s_blsm_thesm.state) && (blsm_state_update_sub_writingbin == s_blsm_thesm.substate)) )
    {   // only in state update-writingbin
        
         // be sure that the buffer is flushed and toggle the red led ...
        if(ee_res_OK == mm_write_buffer())
        {
            hal_led_toggle(led_red);
        }
        
        // send ack
        s_blsm_send_ack(cmdany);
        
        // change state
        blsm_set_evt(blsm_evt_goto_update_finishtowrite);
    }
}


static void s_blsm_process_cmd_end(const blparser_cmd_any_t* cmdany)
{
    blparser_cmd_end_t* cmd = (blparser_cmd_end_t*)cmdany;
    cmd = cmd; // to remove warnings
    
    if((1 == cmdany->sender.errorinframe))
    {
        s_blsm_on_error(cmdany, canprotocol_errcode_end);
    }
    else if( ((blsm_state_update == s_blsm_thesm.state) && (blsm_state_update_sub_finishtowrite == s_blsm_thesm.substate)) )
    {   // only in state update-finishtowrite
        
 
        // write the configuration bits for the device.
        mm_write_deviceConfig(); 
               
        // send ack
        s_blsm_send_ack(cmdany);
        
        // change state
        blsm_set_evt(blsm_evt_goto_update_end);
    }
}


static void s_blsm_process_cmd_enablelog(const blparser_cmd_any_t* cmdany)
{
    blparser_cmd_enablelog_t* cmd = (blparser_cmd_enablelog_t*)cmdany;
    cmd = cmd; // to remove warnings
    
    if((1 == cmdany->sender.errorinframe))
    {
        s_blsm_on_error(cmdany, canprotocol_errcode_enablelog);
    }
    else
    {   // in any state
 
        // enable it depending on the content of the cmd.
        s_blsm_options.enable_log = cmd->enable; 
               
        // send ack
        s_blsm_send_ack(cmdany);
    }
}

// --- helper functions

static void s_blsm_send_can_frame(uint16_t id, uint8_t *data, uint8_t data_len)
{

    hal_can_frame_t frame;
    
    frame.id            = id;
    frame.id_type       = hal_can_frameID_std;
    frame.frame_type    = hal_can_frame_data;
    frame.size          = data_len;
    memcpy(&frame.data, data, frame.size);  

    while(hal_res_OK != hal_can_put(hal_can_port1, &frame, hal_can_send_normprio_now));
}


static void s_blsm_on_error(const blparser_cmd_any_t* cmdany, uint8_t errorcode)
{
    uint8_t dest        = CAN_MSG_ID_GET_SOURCE((uint16_t)cmdany->sender.canid);
    uint16_t id         = BLSM_GETCANID(s_blsm_options.brdadr, dest);
    
    if(1 == s_blsm_options.enable_log)
    {
        s_blsm_txdata[0] = CMD_ERRORLOG;
        s_blsm_txdata[1] = cmdany->sender.cmd; // the received command that generated error
        s_blsm_txdata[2] = errorcode;
        
        s_blsm_send_can_frame(id, s_blsm_txdata, 3);
    } 
}
//commented becouse not used currently
//static void s_blsm_on_warning(const blparser_cmd_any_t* cmdany, uint8_t warningcode)
//{
//    ;
//}

static void s_blsm_sendversion(const blparser_cmd_any_t* cmdany)
{
    uint8_t dest        = CAN_MSG_ID_GET_SOURCE((uint16_t)cmdany->sender.canid);
    uint16_t id         = BLSM_GETCANID(s_blsm_options.brdadr, dest);
    uint8_t major       = 0;
    uint8_t minor       = 0;
    
    bl_boardinfo_getversion(&major, &minor); 
    
    s_blsm_txdata[0] = CMD_BROADCAST;
    s_blsm_txdata[1] = TARGET_BOARD;  
    s_blsm_txdata[2] = major;
    s_blsm_txdata[3] = minor;  
           
    s_blsm_send_can_frame(id, s_blsm_txdata, 4);            
}

static void s_blsm_send_ack(const blparser_cmd_any_t* cmdany)
{
    uint8_t dest        = CAN_MSG_ID_GET_SOURCE((uint16_t)cmdany->sender.canid);
    uint16_t id         = BLSM_GETCANID(s_blsm_options.brdadr, dest);
    
    s_blsm_txdata[0] = cmdany->sender.cmd;
    s_blsm_txdata[1] = 0x01;

    s_blsm_send_can_frame(id, s_blsm_txdata, 2);
}


static void s_blsm_memory_set_address_firstone(const blparser_cmd_any_t* cmdany)
{
    blparser_cmd_address_t* cmd = (blparser_cmd_address_t*)cmdany; 

    s_blsm_lineinfo.num_bytes      = cmd->numbytes;
    s_blsm_lineinfo.address.val[0] = cmd->addressval[0];
    s_blsm_lineinfo.address.val[1] = cmd->addressval[1];
    s_blsm_lineinfo.address.val[2] = cmd->addressval[2];
    s_blsm_lineinfo.address.val[3] = cmd->addressval[3];
    
    // the address in hex format must be divided by 2
    s_blsm_lineinfo.address.val32 = s_blsm_lineinfo.address.val32 >> 1;
    
    // as it is the first time that an address arrives we erase memory. we erase also the eeprom (application storage)
    // if the board command contained the field updateeeprom equal to one.
    // acemor-20nov2012: removed from here and moved just before the first writing into flash ....
    //mm_erase();
    
    // we start the write mode. we dont actually write in flash yet. it returns error if the contained address does not belong to the device
    if(ee_res_OK != mm_start_write(&s_blsm_lineinfo))
    {
        s_blsm_on_error(cmdany, canprotocol_errcode_address);
    }    
}

static void s_blsm_memory_set_address_subsequent(const blparser_cmd_any_t* cmdany)
{
    blparser_cmd_address_t* cmd = (blparser_cmd_address_t*)cmdany; 

    s_blsm_lineinfo.num_bytes      = cmd->numbytes;
    s_blsm_lineinfo.address.val[0] = cmd->addressval[0];
    s_blsm_lineinfo.address.val[1] = cmd->addressval[1];
    s_blsm_lineinfo.address.val[2] = cmd->addressval[2];
    s_blsm_lineinfo.address.val[3] = cmd->addressval[3];
    
    // the address in hex format must be divided by 2
    s_blsm_lineinfo.address.val32 = s_blsm_lineinfo.address.val32 >> 1;
    
    // we manage the change of address by writing internal buffers etc ...
    if(ee_res_OK != mm_check_new_fileLineInfo(&s_blsm_lineinfo))
    {
        s_blsm_on_error(cmdany, canprotocol_errcode_address);
    }    
}


static void s_blsm_memory_set_data(const blparser_cmd_any_t* cmdany)
{
    blparser_cmd_data_t* cmd = (blparser_cmd_data_t*)cmdany; 
    
    // we write the received data in flash memory
    if(ee_res_OK != mm_store_data(cmd->data, cmd->size, &s_blsm_lineinfo))
    {
        s_blsm_on_error(cmdany, canprotocol_errcode_data);
    }  

    // if we have finished a line we send back an ack
    if(0 == s_blsm_lineinfo.num_bytes)
    {
        s_blsm_send_ack(cmdany);
    }    
}





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

