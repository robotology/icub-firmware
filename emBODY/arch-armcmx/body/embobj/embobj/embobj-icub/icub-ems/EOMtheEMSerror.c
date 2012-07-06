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


#include "EOMtheEMStransceiver.h"
#include "EOMtheEMSsocket.h"
#include "EOMtask.h"

#include "stdio.h"
#include "hal_trace.h"

#include "EOVtheSystem.h"

//#include "EOsm.h"

#include "EOMtheEMSappl.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSerror.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSerror_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOemserror_cfg_t eom_emserror_DefaultCfg = 
{
    EO_INIT(.taskpriority)             40, 
    EO_INIT(.taskstacksize)            512
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

extern void tskEMSerr(void *p);

static void s_eom_emserror_task_startup(EOMtask *p, uint32_t t);
static void s_eom_emserror_task_run(EOMtask *p, uint32_t t);

static void s_eom_emserror_OnError(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOMtheEMSerror";

 
static EOMtheEMSerror s_emserror_singleton = 
{
    EO_INIT(.task)              NULL
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOMtheEMSerror * eom_emserror_Initialise(const eOemserror_cfg_t *cfg)
{
    if(NULL != s_emserror_singleton.task)
    {
        return(&s_emserror_singleton);
    }
    
    if(NULL == cfg)
    {
        cfg = &eom_emserror_DefaultCfg;
    }
    
    s_emserror_singleton.task = eom_task_New(eom_mtask_EventDriven, cfg->taskpriority, cfg->taskstacksize, 
                                                    s_eom_emserror_task_startup, s_eom_emserror_task_run,  
                                                    (eOevent_t)0, eok_reltimeINFINITE, NULL, 
                                                    tskEMSerr, "tskEMSerr");
 
    eo_errman_SetOnErrorHandler(eo_errman_GetHandle(), s_eom_emserror_OnError);                                               
    
    return(&s_emserror_singleton);
}


extern EOMtheEMSerror* eom_emserror_GetHandle(void) 
{
    if(NULL != s_emserror_singleton.task)
    {
        return(&s_emserror_singleton);
    }
    else
    {
        return(NULL);
    }
}

extern EOMtask * eom_emserror_GetTask(EOMtheEMSerror *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    return(s_emserror_singleton.task);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern void tskEMSerr(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start(p);
} 


__weak extern void eom_emserror_hid_userdef_DoJustAfterPacketReceived(EOMtheEMSerror *p, EOpacket *rxpkt)
{

} 


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_eom_emserror_task_startup(EOMtask *p, uint32_t t)
{
    // dont do anything. the ems-socket, ems-transceiver etc have been already initted by the EOMtheEMSappl
}



static void s_eom_emserror_task_run(EOMtask *p, uint32_t t)
{
    // the event that we have received
    eOevent_t evt = (eOevent_t)t;
    EOpacket* rxpkt = NULL;
    eOsizecntnr_t remainingrxpkts = 0;
    eOresult_t res;
    
    if(eobool_true == eo_common_event_check(evt, emssocket_evt_packet_received))
    {   // process the reception of a packet. it must contain a ropframe and nothing else
    
        // 1. get the packet. we need passing just a pointer because the storage is inside the EOMtheEMSsocket       
        res = eom_emssocket_Receive(eom_emssocket_GetHandle(), &rxpkt, &remainingrxpkts);

        
        if(eores_OK == res)
        {
            // perform an user-defined function
            eom_emserror_hid_userdef_DoJustAfterPacketReceived(&s_emserror_singleton, rxpkt);
        }
        
        // 5. if another packet is in the rx fifo, send a new event to process its retrieval again        
        if(remainingrxpkts > 0)
        {
            eom_task_SetEvent(p, emssocket_evt_packet_received); 
        }     
    }
    
    if(eobool_true == eo_common_event_check(evt, emserror_evt_error))
    {
        #warning ---> manage start of error....
        eom_emsappl_ProcessEvent(eom_emsappl_GetHandle(), eo_sm_emsappl_EVgo2err);
        //eo_sm_ProcessEvent(eom_emsappl_GetStateMachine(eom_emsappl_GetHandle()), eo_sm_emsappl_EVgo2err); 
    }
}


static void s_eom_emserror_OnError(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info)
{
  
   
    const char err[4][16] = {"info", "warning", "weak error", "fatal error"};
    char str[192];

    snprintf(str, sizeof(str)-1, "EOMtheEMSerror: [eobj: %s, tsk: %d] %s: %s", eobjstr, taskid, err[(uint8_t)errtype], info);
    hal_trace_puts(str);

    if(errtype <= eo_errortype_warning)
    {
        return;
    }
    
    //eov_sys_Stop(eov_sys_GetHandle());
    
     
    eom_task_PrioritySet(s_emserror_singleton.task, eom_mtask_prio_max);
    
    eom_task_SetEvent(s_emserror_singleton.task, emserror_evt_error); 
    
    for(;;);    
}





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





