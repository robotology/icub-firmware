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

// marco.accame@iit.it: revised on sept 2020.
// - removed osal
// - substituted osal with embot::os
// - using embot::os::init() 
// - so far, objects embot::os::theTimerManager and embot::os::theCallbackManager are called w/ their default values
//   and not with those inside eOmtimerman_cfg_t and eOmcallbackman_cfg_t

#warning TODO: use eOmtimerman_cfg_t and eOmcallbackman_cfg_t

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "EoCommon.h"

#include "stdlib.h"
#include "string.h"
#include "stdio.h"


#include "hal.h"

#include "EOtheErrorManager.h"
#include "EOVtheSystem_hid.h" 

#include "embot_core.h"
#include "embot_os.h"
#include "embot_os_rtos.h"
//#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheSystem.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheSystem_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

//#error start from here. embed in here the embot::os ....


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOmempool_alloc_config_t eom_thesystem_mempool_alloc_config_heaposal =
{
    EO_INIT(.pool)
    {
        EO_INIT(.size08)        0,
        EO_INIT(.data08)        NULL,
        EO_INIT(.size16)        0,
        EO_INIT(.data16)        NULL,
        EO_INIT(.size32)        0,
        EO_INIT(.data32)        NULL,
        EO_INIT(.size64)        0,
        EO_INIT(.data64)        NULL
    },
    EO_INIT(.heap)
    {
//        EO_INIT(.allocate)      osal_base_memory_new,
//        EO_INIT(.reallocate)    osal_base_memory_realloc,
//        EO_INIT(.release)       osal_base_memory_del
        EO_INIT(.allocate)      embot::os::rtos::memory_new,
        EO_INIT(.reallocate)    embot::os::rtos::memory_realloc,
        EO_INIT(.release)       embot::os::rtos::memory_delete        
    }
};


const eOmempool_cfg_t eom_thesystem_mempool_cfg_osal = 
{
    EO_INIT(.mode)      eo_mempool_alloc_dynamic, // we use static allcoation
    EO_INIT(.conf)      &eom_thesystem_mempool_alloc_config_heaposal    
};




// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eom_sys_start(eOvoid_fp_void_t userinit_fn);

static void s_eom_thecreation(void);

static EOVtaskDerived* s_eom_gettask(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static const char s_eobj_ownname[] = "EOMtheSystem";

static EOMtheSystem s_eom_system = 
{
    EO_INIT(.thevsys)       NULL,               // thevsys
    EO_INIT(.osalcfg)       NULL,               // osalcfg
    EO_INIT(.tmrmancfg)     NULL,               // tmrmancfg
    EO_INIT(.cbkmancfg)     NULL,               // cbkmancfg
    EO_INIT(.user_init_fn)  NULL                // user_init_fn
};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

void onIdle(embot::os::Thread *t, void* idleparam)
{
    static uint32_t i = 0;
    i++;
}

void initSystem(embot::os::Thread *t, void* initparam)
{
    embot::os::theTimerManager::getInstance().start({});     
    embot::os::theCallbackManager::getInstance().start({});  
    s_eom_thecreation();
}

uint64_t get_now()
{
    return embot::core::now();
}

void set_now(uint64_t t)
{
  #warning TOBESOLVED  
}

uint64_t get_nano()
{
    return 1000*embot::core::now();
}

extern EOMtheSystem * eom_sys_Initialise(const eOmsystem_cfg_t *syscfg, 
                                         const eOmempool_cfg_t *mpoolcfg, 
                                         const eOerrman_cfg_t *errmancfg,
                                         const eOmtimerman_cfg_t *tmrmancfg,
                                         const eOmcallbackman_cfg_t *cbkmancfg)
{
    uint32_t ram08size = 0;
    uint64_t *ram08data = NULL;

    if(NULL != s_eom_system.thevsys) 
    {
        // already initialised
        return(&s_eom_system);
    }


    eo_errman_Assert(eo_errman_GetHandle(), (NULL != syscfg), "eom_sys_Initialise(): NULL syscfg", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);
    // verify that we have a valid halcfg
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != syscfg->halcfg), "eom_sys_Initialise(): NULL halcfg", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);
//    eo_errman_Assert(eo_errman_GetHandle(), (NULL != syscfg->osalcfg), "eom_sys_Initialise(): NULL osalcfg", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);

    // mpoolcfg can be NULL: in such a case we use eo_mempool_alloc_dynamic mode
    // errmancfg can be NULL
    // tmrmancfg can be NULL: in such a case we use default values
    // cbkmancfg can be NULL: in such a case we use default values

    
    // mempool and error manager initialised inside here.
    s_eom_system.thevsys = eov_sys_hid_Initialise(mpoolcfg,
                                                  errmancfg,        // error man 
                                                  (eOres_fp_voidfpvoid_t)s_eom_sys_start, s_eom_gettask, 
                                                  get_now, set_now, 
                                                  get_nano,
                                                  hal_sys_irq_disable);

    s_eom_system.halcfg     = syscfg->halcfg;
    s_eom_system.osalcfg    = syscfg->osalcfg;
    s_eom_system.tmrmancfg  = tmrmancfg;
    s_eom_system.cbkmancfg  = cbkmancfg;

    // the calls to init hal are removed. the hw initialization is done inside embot::os::init()
    // which calls stm32hal_init()
    
    // similar mechanism for the rtos init.


//    hal_core_init(syscfg->halcfg);
//    hal_core_start();

//    if(0 != syscfg->codespaceoffset)
//    {
//        hal_sys_vectortable_relocate(syscfg->codespaceoffset);
//    }    

 
//    // initialise osal
//    osal_base_memory_getsize(s_eom_system.osalcfg, &ram08size);
//    if(0 != ram08size)
//    {
//        ram08data = (uint64_t*) eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_64bit, ram08size, 1);
//    }

//    osal_base_initialise(s_eom_system.osalcfg, ram08data);

    constexpr embot::os::InitThread::Config initcfg = { 4*1024, initSystem, nullptr };
    constexpr embot::os::IdleThread::Config idlecfg = { 512, nullptr, nullptr, onIdle };
    constexpr embot::core::Callback onOSerror = { };
    constexpr embot::os::Config osconfig {embot::core::time1millisec, initcfg, idlecfg, onOSerror};
    
    // embot::os::init() internally calls embot::hw::bsp::init() which also calls embot::core::init()
    embot::os::init(osconfig);
    
    return(&s_eom_system);   
}


extern EOMtheSystem* eom_sys_GetHandle(void)
{
    if(NULL == s_eom_system.thevsys)
    {
        return(NULL);
    }

    return(&s_eom_system);
}    


extern void eom_sys_Start(EOMtheSystem *p, eOvoid_fp_void_t userinit_fn)
{
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != p), "eom_sys_Start(): NULL handle", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);
    s_eom_sys_start(userinit_fn);
}

extern uint32_t eom_sys_GetHeapSize(EOMtheSystem *p)
{
    if(NULL == s_eom_system.thevsys)
    {
        return(0);
    }
	return(s_eom_system.halcfg->syscfg.heapsize);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eom_sys_start(eOvoid_fp_void_t userinit_fn)
{
    // start osal
    s_eom_system.user_init_fn = userinit_fn;
    //osal_system_start(s_eom_thecreation);
    
    embot::os::start();
 
    return(eores_OK);
}


static void s_eom_thecreation(void)
{
    char str[96];
    
    snprintf(str, sizeof(str), "OSAL::tskinit: just started");
    eo_errman_Trace(eo_errman_GetHandle(), str, s_eobj_ownname);
    
    snprintf(str, sizeof(str), "OSAL::tskinit: about to init EOMtheTimerManager + EOMtheCallbackManager");
    eo_errman_Trace(eo_errman_GetHandle(), str, s_eobj_ownname);

    // start the services offered by embobj: timer manager and callback manager

    eom_timerman_Initialise(s_eom_system.tmrmancfg);
    eom_callbackman_Initialise(s_eom_system.cbkmancfg);
    
            
    snprintf(str, sizeof(str), "OSAL::tskinit: calling user-defined function");
    eo_errman_Trace(eo_errman_GetHandle(), str, s_eobj_ownname);
    
    // run user defined initialisation ...
    if(NULL != s_eom_system.user_init_fn)
    {
        s_eom_system.user_init_fn();
    }
    
    snprintf(str, sizeof(str), "OSAL::tskinit: has finished user-defined function");
    eo_errman_Trace(eo_errman_GetHandle(), str, s_eobj_ownname);
    
    
    snprintf(str, sizeof(str), "OSAL::tskinit: about to stop itself and start scheduling");
    eo_errman_Trace(eo_errman_GetHandle(), str, s_eobj_ownname);
}


static EOVtaskDerived* s_eom_gettask(void)
{
//    osal_task_t *p;
//    
//    p = osal_task_get(osal_callerTSK);

//    if(NULL == p)
//    {
//        return(NULL);
//    }

//    return(osal_task_extdata_get(p));
    #warning TOBESOLVED
    return nullptr;
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

