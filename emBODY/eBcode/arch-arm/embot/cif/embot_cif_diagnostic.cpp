
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_cif_diagnostic.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_core.h"
#include "embot_prot_eth_rop.h"
#include "embot_prot_eth_diagnostic.h"


#include "EOMtheEMSDiagnostic.h"

#include "EOMtheEMSapplCfg.h"
#include "hal_trace.h"
#include "EoError.h" 
#include "EOtheInfoDispatcher.h"

#include "EoProtocolMN.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


// #define DIAGNOSTIC2_send_to_yarprobotinterface
// #define DIAGNOSTIC2_send_to_daemon
// #define DIAGNOSTIC2_receive_from_daemon

#if !defined(DIAGNOSTIC2_enabled)
    #warning INFO: diagnostic2 is disabled 
#else
    #warning INFO: diagnostic2 is enabled

    #if defined(DIAGNOSTIC2_send_to_yarprobotinterface)
        #warning INFO: diagnostic2 sends to yarprobotinterface
    #endif
    #if defined(DIAGNOSTIC2_send_to_daemon)
        #warning INFO: diagnostic2 sends to diagnostic-daemon
    #endif
    #if defined(DIAGNOSTIC2_receive_from_daemon)
        #warning INFO: diagnostic2 receives from diagnostic-daemon
    #endif
    
    #if defined(DIAGNOSTIC2_enabled) & defined(DIAGNOSTIC2_send_to_yarprobotinterface) & defined(DIAGNOSTIC2_send_to_daemon) & defined(DIAGNOSTIC2_receive_from_daemon)  
        #warning CAVEAT: this DIAGNOSTIC2 configuration is too much memory hungry. it may not run. 
        #error: this DIAGNOSTIC2 configuration is too much memory hungry. it does nor run.
    #endif 
#endif


static void s_manage_haltrace(const eOerrmanErrorType_t errtype, const char *info, const eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *edes);
static void s_manage_dispatch(const eOerrmanErrorType_t errtype, const char *info, const eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des);
static void s_manage_fatal(const char *info, const eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des);


static osal_mutex_t* onerrormutex {nullptr};
static osal_semaphore_t* blockingsemaphore {nullptr};

extern void embot_cif_diagnostic_Init()
{
    onerrormutex = osal_mutex_new();
    blockingsemaphore = osal_semaphore_new(2, 0);
    EOMtheEMSDiagnostic::Params param;
    EOMtheEMSDiagnostic::instance().initialise(param);    
}


extern void embot_cif_diagnostic_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *errdes)
{
    if(eobool_true == eom_emsapplcfg_GetHandle()->errmng_haltrace_enabled)
    {   
        // any errtype, if haltrace is enabled, is also managed in here for print onto local trace port        
        s_manage_haltrace(errtype, info, caller, errdes);
    }
    
    if(eo_errortype_trace == errtype)
    {   // we dont transmit the trace
        return;
    }
    
    // ok, now we dispatch all but trace 
    s_manage_dispatch(errtype, info, caller, errdes);
        
    // if fatal we manage it in a particular way
    if(eo_errortype_fatal == errtype)
    {
        s_manage_fatal(info, caller, errdes);
    }
    
}
       
static void s_manage_haltrace(const eOerrmanErrorType_t errtype, const char *info, const eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des)
{
    char text[256];
    text[0] = 0; // i put a '0' terminator. just in case 
    
    const char empty[] = "EO?";
    const char *err = eo_errman_ErrorStringGet(eo_errman_GetHandle(), errtype);
    const char *eobjstr = (NULL == caller) ? (empty) : ((NULL == caller->eobjstr) ? (empty) : (caller->eobjstr));
    const uint32_t taskid = (NULL == caller) ? (0) : (caller->taskid);
    
    embot::core::TimeFormatter tf {embot::core::now()};
    
    if(eo_errortype_trace == errtype)
    {
        if(nullptr != info)
        {
            snprintf(text, sizeof(text), "[TRACE] (%s @%s)-> %s.", eobjstr, tf.to_string().c_str(), info); 
        }
        else
        {
            snprintf(text, sizeof(text), "[TRACE] (%s @%s)-> ...", eobjstr, tf.to_string().c_str()); 
        }
            
    }
    else
    {        
        if((NULL == des) && (NULL == info))
        {
            snprintf(text, sizeof(text), "[%s] (%s tsk%d @%s)-> ...", err, eobjstr, taskid, tf.to_string().c_str());            
        }
        else if((NULL != des) && (NULL != info))
        {
            snprintf(text, sizeof(text), "[%s] (%s tsk%d @%s)-> {0x%x p16 0x%04x, p64 0x%016llx, dev %d, adr %d}: %s. INFO = %s", err, eobjstr, taskid, tf.to_string().c_str(), des->code, des->par16, des->par64, des->sourcedevice, des->sourceaddress, eoerror_code2string(des->code), info);  
        }
        else if((NULL != des) && (NULL == info))
        {
            snprintf(text, sizeof(text), "[%s] (%s tsk%d @%s)-> {0x%x, p16 0x%04x, p64 0x%016llx, dev %d, adr %d}: %s.", err, eobjstr, taskid, tf.to_string().c_str(), des->code, des->par16, des->par64, des->sourcedevice, des->sourceaddress, eoerror_code2string(des->code));               
        }
        else if((NULL == des) && (NULL != info))
        {
            snprintf(text, sizeof(text), "[%s] (%s tsk%d @%s)-> {}: ... INFO = %s", err, eobjstr, taskid, tf.to_string().c_str(), info);  
        }
    }   

    hal_trace_puts(text);     
}


static void s_manage_dispatch(const eOerrmanErrorType_t errtype, const char *info, const eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des)
{
   
#if defined(DIAGNOSTIC2_send_to_daemon)
           
//    #warning TODO: manage extended info (add methods + test them)
//    #warning DECIDE: if we want to communicate the caller object via diagnostics [for now we dont do it]
    
    embot::prot::eth::diagnostic::TYP typ = static_cast<embot::prot::eth::diagnostic::TYP>(errtype-1);
    embot::prot::eth::diagnostic::SRC src = static_cast<embot::prot::eth::diagnostic::SRC>(des->sourcedevice);
    embot::prot::eth::diagnostic::ADR adr = static_cast<embot::prot::eth::diagnostic::ADR>(des->sourceaddress);
    embot::prot::eth::diagnostic::EXT ext = (nullptr == info) ? embot::prot::eth::diagnostic::EXT::none : embot::prot::eth::diagnostic::EXT::verbal;
    embot::prot::eth::diagnostic::FFU ffu = embot::prot::eth::diagnostic::FFU::none;
    
//    embot::prot::eth::diagnostic::InfoBasic ib {
//        embot::core::now(),
//        des->code,
//        {typ, src, adr, ext, ffu},
//        des->par16,
//        des->par64        
//    };
    
    embot::prot::eth::diagnostic::Info ii {
        embot::prot::eth::diagnostic::InfoBasic {
            embot::core::now(),
            des->code,
            {typ, src, adr, ext, ffu},
            des->par16,
            des->par64
        },
        info
    };
    
    EOMtheEMSDiagnostic::instance().send(ii, true);
    
#endif
    
#if defined(DIAGNOSTIC2_send_to_yarprobotinterface)

    // old c code used so far
    
    // from eOerrmanErrorType_t to eOmn_info_type_t ...
    uint8_t eee = errtype;
    eee --;
    eOmn_info_type_t type = (eOmn_info_type_t)eee;
    
    eOmn_info_source_t source = (NULL != des) ? ((eOmn_info_source_t)des->sourcedevice) : (eomn_info_source_board);
    uint8_t  address = (NULL != des) ? (des->sourceaddress) : (0);
    eOmn_info_extraformat_t extraformat = (NULL == info) ? (eomn_info_extraformat_none) : (eomn_info_extraformat_verbal);
        
    eOmn_info_properties_t props = {0};
    props.code          = (NULL != des) ? (des->code) : (0);
    props.par16         = (NULL != des) ? (des->par16) : (0);
    props.par64         = (NULL != des) ? (des->par64) : (0);

    EOMN_INFO_PROPERTIES_FLAGS_set_type(props.flags, type);
    EOMN_INFO_PROPERTIES_FLAGS_set_source(props.flags, source);
    EOMN_INFO_PROPERTIES_FLAGS_set_address(props.flags, address);
    EOMN_INFO_PROPERTIES_FLAGS_set_extraformat(props.flags, extraformat);
    EOMN_INFO_PROPERTIES_FLAGS_set_futureuse(props.flags, 0);
    
    // well, i DONT want these functions to be interrupted
    osal_mutex_take(onerrormutex, osal_reltimeINFINITE);
    
    // the call eo_infodispatcher_Put() is only in here, thus we can either protect with a mutex in here or put put a mutex inside. WE USE MUTEX IN HERE
    eo_infodispatcher_Put(eo_infodispatcher_GetHandle(), &props, info); 
    // the call eom_emsappl_SendTXRequest() either does nothing and is reentrant (when in run mode) or sends an event to a task with osal_eventflag_set() which is reentrant.
    // it is called also by the can protocol parser in case of proxy, which however is used in run mode (thus reentrant).
    // i protect both functions in here
    eom_emsappl_SendTXRequest(eom_emsappl_GetHandle());
    
    osal_mutex_release(onerrormutex);    

#endif

}

static void s_manage_fatal(const char *info, const eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des)
{
    // manage fatal error (go in error state, start periodic tx of error status)  
    //#warning --> marco.accame: in case of fatal error, shall we: (1) go smoothly to error state, (2) force immediate transition to error state?
    
    // if in here tehre is a serious error. but we dont care about concurrency
    //osal_mutex_take(s_emsappl_singleton.onerrormutex, osal_reltimeINFINITE);    
    
    // i am going to error state, thus i set the correct state in eOmn_appl_status_t variable, which is used by robotInterface
    // to understand the status of the ems: cfg, run, err.
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_appl, 0, eoprot_tag_mn_appl_status);
    eOmn_appl_status_t *status = (eOmn_appl_status_t*)eoprot_variable_ramof_get(eoprot_board_localboard, id32);
    status->currstate = applstate_error;
    
//#if 1        
    // this call forces immediate transition to error state. then we stop the calling task, so that 
    // it does not do anything damaging anymore.
    // it seems a good approach because we should immediately stop upon fatal error. 

    eom_emserror_SetFatalError(eom_emserror_GetHandle(), des);
    eom_emsappl_SM_ProcessEvent(eom_emsappl_GetHandle(), eo_sm_emsappl_EVgo2err);  
    eom_task_SetEvent(eom_emserror_GetTask(eom_emserror_GetHandle()), emserror_evt_fatalerror);    
    

    // if the caller is not the error state, then we block it execution.
    // however, we cannot block the execution of the error state otherwise ... we lose operativity
    // of error task and amongst others we lose communication with the remote host.
    // when we are in error state the remote host must be able to know it.
 
    eOsmStatesEMSappl_t state = eo_sm_emsappl_STcfg;
    eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &state);
    
    if(eo_sm_emsappl_STerr != state)
    {
        // this call makes the calling task wait in here forever.
        osal_semaphore_decrement(blockingsemaphore, OSAL_reltimeINFINITE);
        // the forever loop should not be necessary.
        for(;;);
    }
    
//#else
//    // in this situation, the transition to error state is done by the active task, thus we require that the execution
//    // continues. the good thing is that we exit in a clean way from the state, but the high risk is to execute dangerous instructions.
//    // example: some object inside the runner detects that some pointer that is going to be used is NULL. if we keep on running this
//    // code, then teh NULL pointer is deferenced and teh application crashes....    
//    eom_emsappl_ProcessGo2stateRequest(eom_emsappl_GetHandle(), eo_sm_emsappl_STerr);
//    return;
//#endif    
}


// --------------------------------------------------------------------------------------------------------------------
// - test unit
// --------------------------------------------------------------------------------------------------------------------
// TBD

    

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

