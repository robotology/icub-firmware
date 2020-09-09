/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// marco.accame@iit.it
// this project uses the embobj c objects which wrap the embot c++ objects
// it uses a fake stm32hal 
// the updater works fine if EMBOT_USE_rtos_osal is used, but at dato of 9 sept 2020 does not work with EMBOT_USE_rtos_cmsisos2

#if defined(EMBOT_USE_rtos_cmsisos2)

#warning CAVEAT: this project does not run if EMBOT_USE_rtos_cmsisos2 is defined

#endif

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

// abslayer 
#include "hal.h"
#include "hal_trace.h"
#include "ipal.h"


// embobj  
#include "EoCommon.h"
#include "EOMtheSystem.h"
#include "EOVtheSystem.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrormanager.h"


#if !defined(_MAINTAINER_APPL_)
#include "eupdater-info.h"
#include "eupdater_cangtw.h"
#else
#include "emaintainer-info.h"
#endif





// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eupdater.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static void s_udpnode_errman_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------




static const eOerrman_cfg_t errmancfg = 
{
    s_udpnode_errman_OnError
};

#if !defined(_MAINTAINER_APPL_)
const eOmsystem_cfg_t* psyscfg = &eupdater_syscfg;
#else
 const eOmsystem_cfg_t* psyscfg = &emaintainer_syscfg;
#endif


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

int main(void)
{   
    eom_sys_Initialise( psyscfg,
                        &eom_thesystem_mempool_cfg_osal,    // mempool
                        &errmancfg,                         // errman
                        &eom_timerman_DefaultCfg,
                        &eom_callbackman_DefaultCfg
                      );  

    // the system executes eupdater_init() and then schedules the other threads
    eom_sys_Start(eom_sys_GetHandle(), eupdater_init);

}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_udpnode_errman_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des)
{
    const char empty[] = "EO?";
    const char *eobjstr = (NULL == caller) ? (empty) : ((NULL == caller->eobjstr) ? (empty) : (caller->eobjstr));
    const uint32_t taskid = (NULL == caller) ? (0) : (caller->taskid);
    
    char text[128];
    uint64_t tt = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    uint32_t sec = tt/(1000*1000);
    uint32_t tmp = tt%(1000*1000);
    uint32_t msec = tmp / 1000;
    uint32_t usec = tmp % 1000;    
    
    if(eo_errortype_trace == errtype)
    {   // it is a trace
        
        if(NULL != info)
        {
            snprintf(text, sizeof(text), "[TRACE] (%s @s%dm%du%d)-> %s.", eobjstr, sec, msec, usec, info); 
        }
        else
        {
            snprintf(text, sizeof(text), "[TRACE] (%s @s%dm%du%d)-> ...", eobjstr, sec, msec, usec); 
        }
        hal_trace_puts(text);
        return;            
    }    
    
    snprintf(text, sizeof(text), "[eobj: %s, tsk: %d] %s: %s", eobjstr, taskid, eo_errman_ErrorStringGet(eo_errman_GetHandle(), errtype), info);
    hal_trace_puts(text);

    if(errtype <= eo_errortype_error)
    {
        return;
    }

    for(;;);
}





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



