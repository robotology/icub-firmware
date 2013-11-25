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
#include "EOtheErrormanager.h"

#include "EOMtheEMSappl.h"


#include "EOMtheEMStransceiver.h"
#include "EOMtheEMSsocket.h"
#include "EOMtask.h"

#include "eventviewer.h"
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSconfigurator.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSconfigurator_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
#if defined(EVIEWER_ENABLED)
#define EVIEWER_userDef_IDbase             (ev_ID_first_usrdef+1)
//#define EVIEWER_userDef_RUNRecRopframe     (EVIEWER_userDef_IDbase +1) see definition in EOMtheEMSrunner.c
#define EVIEWER_userDef_CFGRecRopframe     (EVIEWER_userDef_IDbase +2)
#endif


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOemsconfigurator_cfg_t eom_emsconfigurator_DefaultCfg = 
{
    EO_INIT(.taskpriority)             50, 
    EO_INIT(.taskstacksize)            1024
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

extern void tskEMScfg(void *p);

static void s_eom_emsconfigurator_task_startup(EOMtask *p, uint32_t t);
static void s_eom_emsconfigurator_task_run(EOMtask *p, uint32_t t);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOMtheEMSconfigurator";

 
static EOMtheEMSconfigurator s_emsconfigurator_singleton = 
{
    EO_INIT(.task)              NULL,
    EO_INIT(.numofrxrops)       0
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOMtheEMSconfigurator * eom_emsconfigurator_Initialise(const eOemsconfigurator_cfg_t *cfg)
{
    if(NULL != s_emsconfigurator_singleton.task)
    {
        return(&s_emsconfigurator_singleton);
    }
    
    if(NULL == cfg)
    {
        cfg = &eom_emsconfigurator_DefaultCfg;
    }
    
    s_emsconfigurator_singleton.task = eom_task_New(eom_mtask_EventDriven, cfg->taskpriority, cfg->taskstacksize, 
                                                    s_eom_emsconfigurator_task_startup, s_eom_emsconfigurator_task_run,  
                                                    (eOevent_t)0, eok_reltimeINFINITE, NULL, 
                                                    tskEMScfg, "tskEMScfg");
 
                                                   
    #if defined(EVIEWER_ENABLED)    
    eventviewer_load(EVIEWER_userDef_CFGRecRopframe, usrDef_CFGRecRopframe);
    #endif    

    return(&s_emsconfigurator_singleton);
}


extern EOMtheEMSconfigurator* eom_emsconfigurator_GetHandle(void) 
{
    if(NULL != s_emsconfigurator_singleton.task)
    {
        return(&s_emsconfigurator_singleton);
    }
    else
    {
        return(NULL);
    }
}

extern EOMtask * eom_emsconfigurator_GetTask(EOMtheEMSconfigurator *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    return(s_emsconfigurator_singleton.task);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern void tskEMScfg(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start(p);
} 


__weak extern void eom_emsconfigurator_hid_userdef_DoJustAfterPacketParsing(EOMtheEMSconfigurator *p)
{

} 

__weak extern void eom_emsconfigurator_hid_userdef_ProcessUserdefEvent(EOMtheEMSconfigurator* p)
{

} 

__weak extern void eom_emsconfigurator_hid_userdef_onemstransceivererror(EOMtheEMStransceiver* p)
{

}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_eom_emsconfigurator_task_startup(EOMtask *p, uint32_t t)
{
    // dont do anything. the ems-socket, ems-transceiver etc have been already initted by the EOMtheEMSappl
}



static void s_eom_emsconfigurator_task_run(EOMtask *p, uint32_t t)
{
    // the event that we have received
    eOevent_t evt = (eOevent_t)t;
    EOpacket* rxpkt = NULL;
    EOpacket* txpkt = NULL;
    eOsizecntnr_t remainingrxpkts = 0;
    uint16_t numberofrxrops = 0;
    uint16_t numberoftxrops = 0;
    eOabstime_t txtimeofrxropframe = 0;
    eOresult_t res;
    

    if(eobool_true == eo_common_event_check(evt, emsconfigurator_evt_go2runner))
    {
        eom_emsappl_ProcessEvent(eom_emsappl_GetHandle(), eo_sm_emsappl_EVgo2run);
        // no other event is managed anymore. a possible received packet shall be managed by teh runner
        return;
    }
     
    if(eobool_true == eo_common_event_check(evt, emsconfigurator_evt_ropframeTx))
    {
        // 1. call the former to retrieve a tx packet (even if it is an empty ropframe)        
        res = eom_emstransceiver_Form(eom_emstransceiver_GetHandle(), &txpkt, &numberoftxrops);
        
        // 2.  send a packet back. but only if the former gave us a good one.
        if(eores_OK == res)
        {
            res = eom_emssocket_Transmit(eom_emssocket_GetHandle(), txpkt);
        }
        else
        {
            eom_emsconfigurator_hid_userdef_onemstransceivererror(eom_emstransceiver_GetHandle());
        }
    
    }
 
    if(eobool_true == eo_common_event_check(evt, emssocket_evt_packet_received))
    {   // process the reception of a packet. it must contain a ropframe and nothing else
        
        s_emsconfigurator_singleton.numofrxrops = 0;
    
        // 1. get the packet. we need passing just a pointer because the storage is inside the EOMtheEMSsocket       
        res = eom_emssocket_Receive(eom_emssocket_GetHandle(), &rxpkt, &remainingrxpkts);
                
        // 2. process the packet with the transceiver
        if(eores_OK == res)
        {

            #if defined(EVIEWER_ENABLED)    
            evEntityId_t prev = eventviewer_switch_to(EVIEWER_userDef_CFGRecRopframe);
            #endif
            res = eom_emstransceiver_Parse(eom_emstransceiver_GetHandle(), rxpkt, &numberofrxrops, &txtimeofrxropframe);
            #if defined(EVIEWER_ENABLED)    
            eventviewer_switch_to(prev);
            #endif  
            
            if(eores_OK == res)
            {
                s_emsconfigurator_singleton.numofrxrops = numberofrxrops;
            }
            else
            {
               eom_emsconfigurator_hid_userdef_onemstransceivererror(eom_emstransceiver_GetHandle());
            }
            
        }
        

        
        // perform an user-defined function
        eom_emsconfigurator_hid_userdef_DoJustAfterPacketParsing(&s_emsconfigurator_singleton);
        
        // 3. send ropframe-tx evt to myself in order to send a ropframe
        eom_task_SetEvent(p, emsconfigurator_evt_ropframeTx); 
        
        // // 3. call the former to retrieve a tx packet (even if it is an empty ropframe)        
        // res = eom_emstransceiver_Form(eom_emstransceiver_GetHandle(), &txpkt, &numberoftxrops);
        
        // // 4.  send a packet back. but only if the former gave us a good one.
        // if(eores_OK == res)
        // {
            // res = eom_emssocket_Transmit(eom_emssocket_GetHandle(), txpkt);
        // }
        
               
        // 5. if another packet is in the rx fifo, send a new event to process its retrieval again        
        if(remainingrxpkts > 0)
        {
            eom_task_SetEvent(p, emssocket_evt_packet_received); 
        }
     
    }
    
    if(eobool_true == eo_common_event_check(evt, emsconfigurator_evt_userdef))
    {
        eom_emsconfigurator_hid_userdef_ProcessUserdefEvent(&s_emsconfigurator_singleton);
    }
    


}

#if defined(EVIEWER_ENABLED)
void usrDef_CFGRecRopframe(void){}
#endif





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





