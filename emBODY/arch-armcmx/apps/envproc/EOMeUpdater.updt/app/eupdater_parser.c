/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Alessandro Scalzo
 * email:   marco.accame@iit.it, alessandro.scalzo@iit.it
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

/* @file       eupdater-parser.c
	@brief      This file keeps the vcececew
	@author     marco.accame@iit.it
    @date       01/11/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "eEcommon.h"
#include "eEmemorymap.h"
#include "EOVtheSystem.h"

#include "stdlib.h"

#include "updater-core.h"

#include "osal_system.h"
#include "EOtimer.h"
#include "EOaction.h"
#include "shalPART.h"
#include "shalBASE.h"
#include "EOMtheCallbackmanager.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eupdater_parser.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    eOnanotime_t    client_tx_time;
    eOnanotime_t    server_tx_time;
    uint32_t        client_id;
} pkt_payload_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eObool_t s_eupdater_parser_process_rop_dummy(EOpacket *rxpkt, EOpacket *txpkt);
static eObool_t s_eupdater_parser_process_rop_alessandro(EOpacket *rxpkt, EOpacket *txpkt);

static void s_eupdater_jump2app(void *par);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

EOtimer* s_timer_jump2app = NULL;
const eOreltime_t s_countdown = 5 * EOK_reltime1sec;
EOaction *s_action_jump2app = NULL;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern void eupdater_parser_init(void)
{
    eOreltime_t time2stay = 0;
    eEprocess_t pr = ee_procNone;
    
    // if the loader has set the goto and it is updater ... then we stay in here forever
    if(ee_res_OK == shalbase_ipc_gotoproc_get(&pr))
    {
        shalbase_ipc_gotoproc_clr();
        
        if(ee_procUpdater == pr)
        {   // stay in here forever
            return;
        }
    }    
    
    // then we retrieve how long we have to stay in here   
    shalpart_proc_startuptimeinupdater_get(&time2stay);
    
    if(EOK_reltimeZERO == time2stay)
    {
        // if zero time ... then we jump to default application
        s_eupdater_jump2app(NULL);
    }
    else if(EOK_reltimeINFINITE == time2stay)
    {
        return;
    }
    else
    {
        // otherwise start a timer
        s_timer_jump2app = eo_timer_New();
        
        s_action_jump2app = eo_action_New();
        eo_action_SetCallback(s_action_jump2app, 
                              s_eupdater_jump2app, NULL,
                              eom_callbackman_GetTask(eom_callbackman_GetHandle())
                             );
        
        eo_timer_Start(s_timer_jump2app, eok_abstimeNOW, time2stay, eo_tmrmode_ONESHOT, s_action_jump2app);
    }

}



// return true if there is a pkt to transmit back
extern eObool_t eupdater_parser_process_rop(EOpacket *rxpkt, EOpacket *txpkt)
{
    //return(s_eupdater_parser_process_rop_dummy(rxpkt, txpkt));
    return(s_eupdater_parser_process_rop_alessandro(rxpkt, txpkt));
}


extern eObool_t eupdater_parser_process_data(EOpacket *rxpkt, EOpacket *txpkt)
{

    return(eobool_false);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static eObool_t s_eupdater_parser_process_rop_alessandro(EOpacket *rxpkt, EOpacket *txpkt)
{
    uint8_t *datarx;
    uint8_t *datatx;
    uint16_t sizerx;
    uint16_t sizetx;

    eOipv4addr_t remaddr = 0;
    eOipv4port_t remport = 0;
    
    // if we receive any packet from the correct socket, then we stop the countdown
    if(eo_tmrstat_Running == eo_timer_GetStatus(s_timer_jump2app))
    {
        eo_timer_Stop(s_timer_jump2app);
    }

    eo_packet_Payload_Get(rxpkt, &datarx, &sizerx);
    eo_packet_Payload_Get(txpkt, &datatx, &sizetx);

    if(1 == upd_core_manage_cmd(datarx, datatx, &sizetx))
    {
        eo_packet_Destination_Get(rxpkt, &remaddr, &remport);

        eo_packet_Payload_Set(txpkt, (uint8_t*)datatx, sizetx);
        //eo_packet_Destination_Set(txpkt, remaddr, remport);
        eo_packet_Destination_Set(txpkt, remaddr, 3333);

        return(eobool_true);
    }

    return(eobool_false);

}

static eObool_t s_eupdater_parser_process_rop_dummy(EOpacket *rxpkt, EOpacket *txpkt)
{
    pkt_payload_t *pktdata = NULL;
    uint8_t *data;
    uint16_t size;
    eOnanotime_t nano = 0;

    eOipv4addr_t remaddr = 0;
    eOipv4port_t remport = 0;

    eo_packet_Destination_Get(rxpkt, &remaddr, &remport);


    eo_packet_Payload_Get(rxpkt, &data, &size);
    //ipaddr = ((uint8_t*)&remaddr);
    pktdata = (pkt_payload_t*)data;


    // prepare tx packet
    pktdata->client_id = 0x10101010;
    eov_sys_NanoTimeGet(eov_sys_GetHandle(), &nano); 

    pktdata->client_tx_time = nano;

    eo_packet_Payload_Set(txpkt, (uint8_t*)pktdata, sizeof(pkt_payload_t));

    eo_packet_Destination_Set(txpkt, remaddr, 3333);

    return(eobool_true);
}

static void s_eupdater_jump2app(void *par)
{
    // retrieve the default process
    eEprocess_t defproc = ee_procNone;
    
    // nobody else can avoid this ...
    osal_system_scheduling_suspend();

    if(ee_res_OK == shalpart_proc_def2run_get(&defproc))
    {
        // we go to the default process only if it is not the updater ... else we stay in here
        if(ee_procUpdater != defproc)
        {
            shalbase_ipc_gotoproc_set(defproc);
            shalbase_system_restart();
        }
    }

    // if there is not a default process ... then stay in updater
    osal_system_scheduling_restart();
  
}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



