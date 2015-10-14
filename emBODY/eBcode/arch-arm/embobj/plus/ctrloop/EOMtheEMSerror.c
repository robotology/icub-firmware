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


#include "EoCommon.h"

#include "EOtheMemoryPool.h"
#include "EOtheErrormanager.h"


#include "EOMtheEMStransceiver.h"
#include "EOMtheEMSsocket.h"
#include "EOMtask.h"

#include "stdio.h"
#include "hal_trace.h"

#include "EOVtheSystem.h"


#include "EOMtheEMSappl.h"
#include "EOMtheEMSapplCfg.h"

#include "EOtheInfoDispatcher.h"


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

//static void s_eom_emserror_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOMtheEMSerror";

 
static EOMtheEMSerror s_emserror_singleton = 
{
    EO_INIT(.task)                      NULL,
    EO_INIT(.timer)                     NULL,
    EO_INIT(.numberoffatalerrors)       0,
    EO_INIT(.errordescriptor)           {0},
    EO_INIT(.latesterrordesc)           {0}
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
    
    const eOreltime_t timeout = eok_reltime10ms; //eok_reltimeINFINITE;
    
    s_emserror_singleton.task = eom_task_New(eom_mtask_EventDriven, cfg->taskpriority, cfg->taskstacksize, 
                                                    s_eom_emserror_task_startup, s_eom_emserror_task_run,  
                                                    (eOevent_t)0, timeout, NULL, 
                                                    tskEMSerr, "tskEMSerr");
    
    s_emserror_singleton.timer = eo_timer_New();
        
    memcpy(&s_emserror_singleton.errordescriptor, &eo_errman_DescrRunningHappily, sizeof(eOerrmanDescriptor_t));  
    memcpy(&s_emserror_singleton.latesterrordesc, &eo_errman_DescrRunningHappily, sizeof(eOerrmanDescriptor_t)); 

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


extern eOresult_t eom_emserror_SetFatalError(EOMtheEMSerror *p, const eOerrmanDescriptor_t* fatalerror) 
{
    if((NULL == p) || (NULL == fatalerror))
    {
        return(eores_NOK_nullpointer);
    } 
    
    s_emserror_singleton.numberoffatalerrors++;

    memcpy(&s_emserror_singleton.latesterrordesc, fatalerror, sizeof(eOerrmanDescriptor_t));        
    memcpy(&s_emserror_singleton.errordescriptor, &eo_errman_DescrRuntimeFatalErrorState, sizeof(eOerrmanDescriptor_t));
    
    s_emserror_singleton.errordescriptor.par16 = s_emserror_singleton.numberoffatalerrors;
    s_emserror_singleton.errordescriptor.par64 = 0;
    //#warning marco.accame: we could use a par64 to send to robotinterface some more info about the fatal error. think of it
    // send fatal error event.
   
    return(eores_OK);    
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

extern void eom_emserror_OnError_userdefined_call(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des)
{   
    // marco.accame:  
    // very important info: in november 2014 i rewrote the error handling mechanism and i put the overridable error
    // funtion of EOtheErrorManager inside the object EOMtheEMSappl. the new function sends ROPs inside the standard socket.                     
    // the previous overridable function was this eom_emserror_OnError_userdefined_call().
    // thus, i have removed the __weak keyword so that if anybody has defined it elsewhere the linker gives error.
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
    EOpacket* txpkt = NULL;
    eOsizecntnr_t remainingrxpkts = 0;
    uint16_t numberofrxrops = 0;
    uint16_t numberoftxrops = 0;
    eOabstime_t txtimeofrxropframe = 0;
    eOresult_t res;
    static uint32_t num = 0;
    
    
    if(eobool_true == eo_common_event_check(evt, emserror_evt_tick))
    {
        num++;  
        eOerrmanErrorType_t errortype = (eo_errman_code_sys_runninghappily == s_emserror_singleton.errordescriptor.code) ? (eo_errortype_info) : (eo_errortype_error);
        eo_errman_Error(eo_errman_GetHandle(), errortype, NULL, s_eobj_ownname, &s_emserror_singleton.errordescriptor);
    }
    
    
    if(eobool_true == eo_common_event_check(evt, emserror_evt_ropframeTx))
    {
        // 1. call the former to retrieve a tx packet (even if it is an empty ropframe)        
        res = eom_emstransceiver_Form(eom_emstransceiver_GetHandle(), &txpkt, &numberoftxrops, NULL);
        
        // 2.  send a packet back. but only if the former gave us a good one and there are rops inside
        if(eores_OK == res)
        {
            // we must transmit only if there are rops inside. the sequence number is incremented only if there are any rops.
            if(numberoftxrops > 0)
            { 
                res = eom_emssocket_Transmit(eom_emssocket_GetHandle(), txpkt);
            }
        }
    
    }

    
    if(eobool_true == eo_common_event_check(evt, emssocket_evt_packet_received))
    {   // process the reception of a packet. it must contain a ropframe and nothing else
        
        uint16_t replies = 0;
        uint16_t occasionals = 0;
        uint16_t regulars = 0;
    
        // 1. get the packet. we need passing just a pointer because the storage is inside the EOMtheEMSsocket       
        res = eom_emssocket_Receive(eom_emssocket_GetHandle(), &rxpkt, &remainingrxpkts);
        
        
        if(eores_OK == res)
        {
            res = eom_emstransceiver_Parse(eom_emstransceiver_GetHandle(), rxpkt, &numberofrxrops, &txtimeofrxropframe); 
            // perform a user-defined function
            eom_emserror_hid_userdef_DoJustAfterPacketReceived(&s_emserror_singleton, rxpkt);            
        }        

        eom_emstransceiver_NumberofOutROPs(eom_emstransceiver_GetHandle(), &replies, &occasionals, &regulars);
        
        if((replies + occasionals) > 0)
        {
            // 3. send ropframe-tx evt to myself in order to send a ropframe.
            // marco.accame on 25 nov 2014: when we receive a packet we may need to send imemdiately one packet back 
            //                              in case there are any say<> rops. we prefer to send the evt_ropframeTx
            //                              anyway so that the remote host can monitor the existence of the board. 
            eom_task_SetEvent(p, emserror_evt_ropframeTx); 
        }        
        
        // 5. if another packet is in the rx fifo, send a new event to process its retrieval again        
        if(remainingrxpkts > 0)
        {
            eom_task_SetEvent(p, emssocket_evt_packet_received); 
        }     
    }
    
    // so far noboby sends an event emserror_evt_error and transition to error state is done elsewhere
    if(eobool_true == eo_common_event_check(evt, emserror_evt_fatalerror))
    {
        // if not started yet, start the timer ...
        if(eo_tmrstat_Running != eo_timer_GetStatus(s_emserror_singleton.timer))
        {
            // start it
            EOaction_strg astg = {0};
            EOaction *act = (EOaction*)&astg;
            
            eo_action_SetEvent(act, emserror_evt_tick, s_emserror_singleton.task);
            eo_timer_Start(s_emserror_singleton.timer, eok_abstimeNOW, 1*eok_reltime1sec, eo_tmrmode_FOREVER, act);         
        }
    }
}


//static void s_eom_emserror_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des)
//{
//  
//    const char empty[] = "EO?";
//    const char *err = eo_errman_ErrorStringGet(eo_errman_GetHandle(), errtype);
//    const char *eobjstr = (NULL == caller) ? (empty) : (caller->eobjstr);
//    const uint32_t taskid = (NULL == caller) ? (0) : (caller->taskid);
//    char str[256];

//    EOMtheEMSapplCfg *emsapplcfg = eom_emsapplcfg_GetHandle();

//    if(emsapplcfg->errmng_haltrace_enabled)
//    {
//        char strdes[64] = {0};
//        if(NULL != des)
//        {
//            snprintf(strdes, sizeof(strdes), "code = 0x%x, param = %d, dev = %d, adr = %d", des->code, des->param, des->sourcedevice, des->sourceaddress);
//        }
//        snprintf(str, sizeof(str), "EOMtheEMSerror: [eobj: %s, tsk: %d] %s %s: %s", eobjstr, taskid, err, strdes, info);        
//        hal_trace_puts(str);
//    }
//    
//    // now i use the info-dispatcher
//    
//    eOmn_info_type_t type = (eOmn_info_type_t)errtype;
//    eOmn_info_source_t source = (NULL != des) ? ((eOmn_info_source_t)des->sourcedevice) : (eomn_info_source_board);
//    uint8_t  address = (NULL != des) ? (des->sourceaddress) : (0);
//    eOmn_info_extraformat_t extraformat = (NULL == info) ? (eomn_info_extraformat_none) : (eomn_info_extraformat_verbal);    
//    
//    eOmn_info_properties_t props = {0};
//    props.code          = (NULL != des) ? (des->code) : (0);
//    props.param         = (NULL != des) ? (des->param) : (0);
//    
//    EOMN_INFO_PROPERTIES_FLAGS_set_type(props.flags, type);
//    EOMN_INFO_PROPERTIES_FLAGS_set_source(props.flags, source);
//    EOMN_INFO_PROPERTIES_FLAGS_set_address(props.flags, address);
//    EOMN_INFO_PROPERTIES_FLAGS_set_extraformat(props.flags, extraformat);
//    EOMN_INFO_PROPERTIES_FLAGS_set_futureuse(props.flags, 0);
//    
//    
//    eo_infodispatcher_Put(eo_infodispatcher_GetHandle(), &props, info);
//    #warning --> eo_infodispatcher_Put() requires also that someone triggers the transmission of a packet .... think of it 
//    
//    eom_emserror_OnError_userdefined_call(errtype, info, caller, des);
//    
//    if(errtype <= eo_errortype_error)
//    {
//        return;
//    }
//    
//    //eov_sys_Stop(eov_sys_GetHandle());
//       

//    eom_task_PrioritySet(s_emserror_singleton.task, eom_mtask_prio_max);
//    
//    eom_task_SetEvent(s_emserror_singleton.task, emserror_evt_error); 
//    
//    //for(;;);
//    eom_mutex_Take(s_emserror_singleton.mutex, eok_reltimeINFINITE);
//}
//
//#warning --> marco.accame: bisogna pensare a come fermare il sistema nel caso di errore fatale .... lo si manda o no in task di errore?





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





