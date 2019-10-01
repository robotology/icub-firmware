/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdlib.h"
#include "EoCommon.h"

#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"



#include "EOMtask.h"

	#include "EOMtheIPnet.h"

	#include "EOMmutex.h"
#include "EOaction.h"
#include "string.h"
#include "EOMtheEMSdiscoverytransceiver.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSdiscoverylistener.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSdiscoverylistener_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOemsdiscoverylistener_cfg_t eom_emsdiscoverylistener_DefaultCfg = 
{
    EO_INIT(.taskpriority)              41, 
    EO_INIT(.taskstacksize)             512,
    EO_INIT(.inpdatagramnumber)         2, 
    EO_INIT(.outdatagramnumber)         1, 
    EO_INIT(.inpdatagramsizeof)         64, 
    EO_INIT(.outdatagramsizeof)         64,
    EO_INIT(.localport)                 3333, 
    EO_INIT(.remoteport)                3333,
    EO_INIT(.remoteipaddr)              EO_COMMON_IPV4ADDR_LOCALHOST,
    EO_INIT(.usemutex)                  eobool_true,
    EO_INIT(.discoveryprotocol)         eodiscovery_protocol_ethloader_reduced    
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef enum
{   // must be single bits: 1, 2, 4, 8, 16, 32, 64 etc.
    emsdiscoverylistener_evt_packet_received = 1,
    emsdiscoverylistener_evt_packet_2transmit = 2
} emsdiscoverylistener_event_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

extern void tskEMScfg(void *p);

static void s_eom_emsdiscoverylistener_task_startup(EOMtask *p, uint32_t t);
static void s_eom_emsdiscoverylistener_task_run(EOMtask *p, uint32_t t);

static eOresult_t s_eom_emsdiscoverylistener_Transmit(EOMtheEMSdiscoverylistener* p, EOpacket* txpkt);
static eOresult_t s_eom_emsdiscoverylistener_Connect(EOMtheEMSdiscoverylistener* p, eOipv4addr_t remaddr);  


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOMtheEMSdiscoverylistener";

 
static EOMtheEMSdiscoverylistener s_emsdiscoverylistener_singleton = 
{
    EO_INIT(.task)                          NULL,
    EO_INIT(.socket)                        NULL,
    EO_INIT(.rxpkt)                         NULL,
    EO_INIT(.replypkt)                         NULL,
    EO_INIT(.hostaddress)                   EO_COMMON_IPV4ADDR_LOCALHOST,
    EO_INIT(.connected2host)                eobool_false,
    EO_INIT(.cfg)       
    {
        0
    }    
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOMtheEMSdiscoverylistener * eom_emsdiscoverylistener_Initialise(const eOemsdiscoverylistener_cfg_t *cfg)
{
    if(NULL != s_emsdiscoverylistener_singleton.task)
    {
        return(&s_emsdiscoverylistener_singleton);
    }
    
    if(NULL == cfg)
    {
        cfg = &eom_emsdiscoverylistener_DefaultCfg;
    }
    
    memcpy(&s_emsdiscoverylistener_singleton.cfg, cfg, sizeof(eOemsdiscoverylistener_cfg_t));
    
    
    if(NULL == eom_ipnet_GetHandle())
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "eom_emsdiscoverylistener_Initialise(): EOMtheIPnet not started yet", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);
    }

    // create the socket    
    s_emsdiscoverylistener_singleton.socket = eo_socketdtg_New(cfg->inpdatagramnumber, cfg->inpdatagramsizeof, (eobool_true == cfg->usemutex) ? (eom_mutex_New()) : (NULL), 
                                                               cfg->outdatagramnumber, cfg->outdatagramsizeof, (eobool_true == cfg->usemutex) ? (eom_mutex_New()) : (NULL)
                                                              );    
    // create the rx packet
    s_emsdiscoverylistener_singleton.rxpkt = eo_packet_New(cfg->inpdatagramsizeof);
    
    
    // initialise the transceiver
    eOemsdiscoverytransceiver_cfg_t dtrcfg;
    dtrcfg.hostipv4addr  = cfg->remoteipaddr;
    dtrcfg.hostipv4port  = cfg->remoteport;
    dtrcfg.txpktcapacity = cfg->outdatagramsizeof;
    dtrcfg.discoveryprotocol = cfg->discoveryprotocol;
    dtrcfg.dbgshutdowntime = 0; // if 0 the shutdown timer is not started. otherwise it forces a got to updater. 3*60*eok_reltime1sec;
    eom_emsdiscoverytransceiver_Initialise(&dtrcfg);
    
    
    // create the task
    s_emsdiscoverylistener_singleton.task = eom_task_New(eom_mtask_EventDriven, cfg->taskpriority, cfg->taskstacksize, 
                                                    s_eom_emsdiscoverylistener_task_startup, s_eom_emsdiscoverylistener_task_run,  
                                                    (eOevent_t)0, eok_reltimeINFINITE, NULL, 
                                                    tskEMScfg, "tskEMSlis");
 
                                                   
    
    return(&s_emsdiscoverylistener_singleton);
}


extern EOMtheEMSdiscoverylistener* eom_emsdiscoverylistener_GetHandle(void) 
{
    if(NULL != s_emsdiscoverylistener_singleton.task)
    {
        return(&s_emsdiscoverylistener_singleton);
    }
    else
    {
        return(NULL);
    }
}

extern EOMtask * eom_emsdiscoverylistener_GetTask(EOMtheEMSdiscoverylistener *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    return(s_emsdiscoverylistener_singleton.task);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern void tskEMSlis(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start((EOMtask*)p);
} 


// EO_weak extern void eom_emsdiscoverylistener_hid_userdef_DoJustAfterPacketParsing(EOMtheEMSdiscoverylistener *p)
// {

// } 


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_eom_emsdiscoverylistener_task_startup(EOMtask *p, uint32_t t)
{
    // open the socket
    EOaction_strg astg = {0};
    EOaction *onrx = (EOaction*)&astg;
    eo_action_SetEvent(onrx, emsdiscoverylistener_evt_packet_received, p);
    
    eo_socketdtg_Open(s_emsdiscoverylistener_singleton.socket, s_emsdiscoverylistener_singleton.cfg.localport, eo_sktdir_TXRX, eobool_false, 
                      NULL, onrx, NULL);   
    
    eo_errman_Trace(eo_errman_GetHandle(), "called _task_startup()", s_eobj_ownname);
                                
}



static void s_eom_emsdiscoverylistener_task_run(EOMtask *p, uint32_t t)
{
    // the event that we have received
    eOevent_t evt = (eOevent_t)t;
    EOpacket* rxpkt = s_emsdiscoverylistener_singleton.rxpkt;
    EOpacket* replypkt = NULL;
    eOsizecntnr_t remainingrxpkts = 0;    

    
    if(eobool_true == eo_common_event_check(evt, emsdiscoverylistener_evt_packet_received))
    {   // process the reception of a packet. it must contain a frame with a particular protocol
        
    
        // 1. get the packet.    
        if(eores_OK == eo_socketdtg_Get(s_emsdiscoverylistener_singleton.socket, rxpkt, eok_reltimeZERO))
        {
                 
            // 2. process the packet with the transceiver
            if(eores_OK == eom_emsdiscoverytransceiver_Parse(eom_emsdiscoverytransceiver_GetHandle(), rxpkt))
            {
             
                // 3. retrieve the reply                
                if(eores_OK == eom_emsdiscoverytransceiver_GetReply(eom_emsdiscoverytransceiver_GetHandle(), &replypkt))
                {
                    // 4.  send a packet back. but only if the get_reply gave us a good one.
                    s_eom_emsdiscoverylistener_Transmit(&s_emsdiscoverylistener_singleton, replypkt);
                }
                
            }
        }
        
    
        // 5. if another packet is in the rx fifo, send a new event to process its retrieval again   
        eo_socketdtg_Received_NumberOf(s_emsdiscoverylistener_singleton.socket, &remainingrxpkts);
        
        if(remainingrxpkts > 0)
        {
            eom_task_SetEvent(p, emsdiscoverylistener_evt_packet_received); 
        }
        
    }
     
}

static eOresult_t s_eom_emsdiscoverylistener_Transmit(EOMtheEMSdiscoverylistener* p, EOpacket* txpkt)
{
    eOresult_t res;
    eOipv4addr_t remaddr;
    eOipv4port_t remport;
    
    if((NULL == p) || (NULL == txpkt))
    {
        return(eores_NOK_nullpointer);
    }
    
    
    eo_packet_Addressing_Get(txpkt, &remaddr, &remport);
    

    if((eobool_false == p->connected2host) || (remaddr != p->hostaddress))
    {
        res = s_eom_emsdiscoverylistener_Connect(p, remaddr);
        if(eores_OK != res)
        {
            return(res);
        }
    }
    
    res = eo_socketdtg_Put(p->socket, txpkt);
    
    return(res);
}



static eOresult_t s_eom_emsdiscoverylistener_Connect(EOMtheEMSdiscoverylistener* p, eOipv4addr_t remaddr)
{
    eOresult_t res = eores_OK;
    

    if((eobool_false == p->connected2host) || (remaddr != p->hostaddress))
    {
        p->connected2host = eobool_false;
        p->hostaddress    = remaddr;
        
        res = eom_ipnet_ResolveIP(eom_ipnet_GetHandle(), remaddr, 5*EOK_reltime1sec);
 
        if(eores_OK != res)
        {
            return(res);
        }
        p->connected2host = eobool_true;        
    }
    
    return(eores_OK);   
}








// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





