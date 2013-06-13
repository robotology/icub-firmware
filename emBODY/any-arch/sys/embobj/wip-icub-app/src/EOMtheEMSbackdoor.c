/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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
#include "EOtheErrormanager.h"
#include "osal_semaphore.h"



#include "EOMtask.h"

#include "EOMtheIPnet.h"

#include "EOMmutex.h"
#include "EOaction_hid.h"
#include "string.h"
#include "EOMtheEMSbackdoortransceiver.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSbackdoor.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSbackdoor_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define _MAXTOKENS_TXSEM_ 2 // but as it is used as a binary semaphore, then 1 would be enough 


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOemsbackdoor_cfg_t eom_emsbackdoor_DefaultCfg = 
{
    EO_INIT(.taskpriority)              42, 
    EO_INIT(.taskstacksize)             512,
    EO_INIT(.inpdatagramnumber)         2, 
    EO_INIT(.outdatagramnumber)         2, 
    EO_INIT(.inpdatagramsizeof)         64, 
    EO_INIT(.outdatagramsizeof)         64,
    EO_INIT(.localport)                 4444, 
    EO_INIT(.remoteport)                4444,
    EO_INIT(.remoteipaddr)              EO_COMMON_IPV4ADDR_LOCALHOST,
    EO_INIT(.usemutex)                  eobool_true
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef enum
{   // must be single bits: 1, 2, 4, 8, 16, 32, 64 etc.
    emsbackdoor_evt_packet_received     = 1,
    emsbackdoor_evt_packet_2transmit    = 2
} emsbackdoor_event_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

extern void tskEMScfg(void *p);

static void s_eom_emsbackdoor_task_startup(EOMtask *p, uint32_t t);
static void s_eom_emsbackdoor_task_run(EOMtask *p, uint32_t t);

static eOresult_t s_eom_emsbackdoor_Transmit(EOMtheEMSbackdoor* p, EOpacket* txpkt);
static eOresult_t s_eom_emsbackdoor_Connect(EOMtheEMSbackdoor* p, eOipv4addr_t remaddr);  


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOMtheEMSbackdoor";

 
static EOMtheEMSbackdoor s_emsbackdoor_singleton = 
{
    EO_INIT(.task)                          NULL,
    EO_INIT(.socket)                        NULL,
    EO_INIT(.rxpkt)                         NULL,
    EO_INIT(.txpkt)                         NULL,
    EO_INIT(.hostaddress)                   EO_COMMON_IPV4ADDR_LOCALHOST,
    EO_INIT(.connected2host)                eobool_false,
    EO_INIT(.cfg)       
    {
        0
    },
    EO_INIT(.txsemaphore)                   NULL  
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOMtheEMSbackdoor * eom_emsbackdoor_Initialise(const eOemsbackdoor_cfg_t *cfg)
{
    if(NULL != s_emsbackdoor_singleton.task)
    {
        return(&s_emsbackdoor_singleton);
    }
    
    if(NULL == cfg)
    {
        cfg = &eom_emsbackdoor_DefaultCfg;
    }
    
    memcpy(&s_emsbackdoor_singleton.cfg, cfg, sizeof(eOemsbackdoor_cfg_t));
    
    
    if(NULL == eom_ipnet_GetHandle())
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "the EOMtheIPnet has not started yet");
    }

    // create the socket    
    s_emsbackdoor_singleton.socket = eo_socketdtg_New(cfg->inpdatagramnumber, cfg->inpdatagramsizeof, (eobool_true == cfg->usemutex) ? (eom_mutex_New()) : (NULL), 
                                                      cfg->outdatagramnumber, cfg->outdatagramsizeof, (eobool_true == cfg->usemutex) ? (eom_mutex_New()) : (NULL)
                                                     );    
    // create the rx packet
    s_emsbackdoor_singleton.rxpkt = eo_packet_New(cfg->inpdatagramsizeof);
    
    // create the tx packet
    s_emsbackdoor_singleton.txpkt = eo_packet_New(cfg->outdatagramsizeof);
    s_emsbackdoor_singleton.txsemaphore = osal_semaphore_new(_MAXTOKENS_TXSEM_, 1);
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != s_emsbackdoor_singleton.txsemaphore), s_eobj_ownname, "semaphore is NULL");
    
    
    // initialise the transceiver
   eOemsbackdoortransceiver_cfg_t dtrcfg;
   dtrcfg.hostipv4port      = cfg->remoteport;
   dtrcfg.replypktcapacity  = cfg->outdatagramsizeof;
   dtrcfg.backdoorprotocol  = eobackdoor_protocol_userdef_opc;
   eom_emsbackdoortransceiver_Initialise(&dtrcfg);
    
    
    // create the task
    s_emsbackdoor_singleton.task = eom_task_New(eom_mtask_EventDriven, cfg->taskpriority, cfg->taskstacksize, 
                                                    s_eom_emsbackdoor_task_startup, s_eom_emsbackdoor_task_run,  
                                                    (eOevent_t)0, eok_reltimeINFINITE, NULL, 
                                                    tskEMScfg, "tskEMSbackdoor");
 
 
 
    s_emsbackdoor_singleton.hostaddress     = cfg->remoteipaddr;
    s_emsbackdoor_singleton.connected2host  = eobool_false;
    s_eom_emsbackdoor_Connect(&s_emsbackdoor_singleton, cfg->remoteipaddr);
    
    return(&s_emsbackdoor_singleton);
}


extern EOMtheEMSbackdoor* eom_emsbackdoor_GetHandle(void) 
{
    if(NULL != s_emsbackdoor_singleton.task)
    {
        return(&s_emsbackdoor_singleton);
    }
    else
    {
        return(NULL);
    }
}

extern EOMtask * eom_emsbackdoor_GetTask(EOMtheEMSbackdoor *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    return(s_emsbackdoor_singleton.task);
}


extern eOresult_t eom_emsbackdoor_Transmit(EOMtheEMSbackdoor *p, uint8_t* data, uint16_t size, eOreltime_t timeout)
{
    if((NULL == p) || (NULL == data) || (0 == size))
    {
        return(eores_NOK_nullpointer);
    }
    
    // metti data nel txpkt ... ma solo se il semaforo lo consente e poi manda un evento
    
    osal_result_t res = osal_semaphore_decrement(s_emsbackdoor_singleton.txsemaphore, timeout);
    
    if(osal_res_OK != res)
    {
        return(eores_NOK_generic);
    }
    
    eo_packet_Payload_Set(s_emsbackdoor_singleton.txpkt, data, size);
    eo_packet_Addressing_Set(s_emsbackdoor_singleton.txpkt, s_emsbackdoor_singleton.hostaddress, s_emsbackdoor_singleton.cfg.remoteport);
          
    eom_task_SetEvent(p->task, emsbackdoor_evt_packet_2transmit); 
    
    
    return(eores_OK);
}


extern eOresult_t eom_emsbackdoor_Signal(EOMtheEMSbackdoor *p, uint16_t var , eOreltime_t timeout)
{

    if((NULL == p))
    {
        return(eores_NOK_nullpointer);
    }
    
   
    // metti data nel txpkt ... ma solo se il semaforo lo consente e poi manda un evento
    
    osal_result_t osalres = osal_semaphore_decrement(s_emsbackdoor_singleton.txsemaphore, timeout);
    
    if(osal_res_OK != osalres)
    {
        return(eores_NOK_generic);
    }
    
    // form the packet ... by taking as data the payload of the tx packet
    uint8_t* data = NULL;
    uint16_t size;
    eo_packet_Payload_Get(s_emsbackdoor_singleton.txpkt, &data, &size);
    
    eOresult_t res = eom_emsbackdoortransceiver_FormSignal(eom_emsbackdoortransceiver_GetHandle(), var, data, &size);
            
    // if it fails ... release the semaphore and return
    if(eores_OK != res)
    {
        osal_semaphore_increment(s_emsbackdoor_singleton.txsemaphore, osal_callerTSK);
        return(eores_NOK_generic);
    }
          
    eo_packet_Size_Set(s_emsbackdoor_singleton.txpkt, size);
    
    eo_packet_Addressing_Set(s_emsbackdoor_singleton.txpkt, s_emsbackdoor_singleton.hostaddress, s_emsbackdoor_singleton.cfg.remoteport);
          
    eom_task_SetEvent(p->task, emsbackdoor_evt_packet_2transmit); 
        
    return(eores_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern void tskEMSbackdoor(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start(p);
} 


// __weak extern void eom_emsbackdoor_hid_userdef_DoJustAfterPacketParsing(EOMtheEMSbackdoor *p)
// {

// } 


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_eom_emsbackdoor_task_startup(EOMtask *p, uint32_t t)
{
    // open the socket
    EOaction onrx;
    eo_action_SetEvent(&onrx, emsbackdoor_evt_packet_received, p);
    
    eo_socketdtg_Open(s_emsbackdoor_singleton.socket, s_emsbackdoor_singleton.cfg.localport, eo_sktdir_TXRX, eobool_false, 
                      NULL, &onrx, NULL);   
                                
}



static void s_eom_emsbackdoor_task_run(EOMtask *p, uint32_t t)
{
    // the event that we have received
    eOevent_t evt = (eOevent_t)t;
    EOpacket* rxpkt = s_emsbackdoor_singleton.rxpkt;
    EOpacket* txpkt = s_emsbackdoor_singleton.txpkt;
    EOpacket* replypkt = NULL;
    eOsizecntnr_t remainingrxpkts = 0;    
    
    
    if(eobool_true == eo_common_event_check(evt, emsbackdoor_evt_packet_2transmit))
    {
        s_eom_emsbackdoor_Transmit(&s_emsbackdoor_singleton, txpkt);
        osal_semaphore_increment(s_emsbackdoor_singleton.txsemaphore, osal_callerTSK);
    }

    
    if(eobool_true == eo_common_event_check(evt, emsbackdoor_evt_packet_received))
    {   // process the reception of a packet. it must contain a frame with a particular protocol
        
    
        // 1. get the packet.    
        if(eores_OK == eo_socketdtg_Get(s_emsbackdoor_singleton.socket, rxpkt, eok_reltimeZERO))
        {
                 
            // 2. process the packet with the transceiver
            if(eores_OK == eom_emsbackdoortransceiver_Parse(eom_emsbackdoortransceiver_GetHandle(), rxpkt))
            {
             
                // 3. retrieve the reply                
                if(eores_OK == eom_emsbackdoortransceiver_GetReply(eom_emsbackdoortransceiver_GetHandle(), &replypkt))
                {
                    // 4.  send a packet back. but only if the get_reply gave us a good one.
                    s_eom_emsbackdoor_Transmit(&s_emsbackdoor_singleton, replypkt);
                }
                
            }
        }
        
    
        // 5. if another packet is in the rx fifo, send a new event to process its retrieval again   
        eo_socketdtg_Received_NumberOf(s_emsbackdoor_singleton.socket, &remainingrxpkts);
        
        if(remainingrxpkts > 0)
        {
            eom_task_SetEvent(p, emsbackdoor_evt_packet_received); 
        }
        
    }
     
}



static eOresult_t s_eom_emsbackdoor_Transmit(EOMtheEMSbackdoor* p, EOpacket* txpkt)
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
        res = s_eom_emsbackdoor_Connect(p, remaddr);
        if(eores_OK != res)
        {
            return(res);
        }
    }

    res = eo_socketdtg_Put(p->socket, txpkt);
    
    return(res);
}



static eOresult_t s_eom_emsbackdoor_Connect(EOMtheEMSbackdoor* p, eOipv4addr_t remaddr)
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





