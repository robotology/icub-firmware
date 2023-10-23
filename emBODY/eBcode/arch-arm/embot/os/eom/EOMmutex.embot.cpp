/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// marco.accame@iit.it: 
// - removed osal
// - substituted osal with embot::os

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_core.h"
#include "embot_os.h"
#include "embot_os_rtos.h"

#include "EOVmutex_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMmutex.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

struct EOMmutex_hid 
{
    // - base object
    EOVmutex *base {nullptr};

    // - other stuff
    embot::os::rtos::mutex_t *mutex {nullptr};    

    EOMmutex_hid() = default;
}; 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eom_mutex_take(void *p, eOreltime_t tout);
static eOresult_t s_eom_mutex_release(void *p);
static eOresult_t s_eom_mutex_delete(void *p);

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOMmutex* eom_mutex_New(void) 
{
    EOMmutex *retptr = NULL;    
    
    retptr = new EOMmutex;
    retptr->mutex = embot::os::rtos::mutex_new();  
    retptr->base = eov_mutex_hid_New();
    eov_mutex_hid_SetVTABLE(retptr->base, s_eom_mutex_take, s_eom_mutex_release, s_eom_mutex_delete); 
    
    return(retptr);    
}


//extern void eom_mutex_Delete(EOMmutex *m) 
//{    
//    if(NULL == m)
//    {
//        return;
//    }
//    
//    if(NULL == m->mutex)
//    {
//        return;
//    }
//    
//    embot::os::rtos::mutex_delete(m->mutex);
//    
//    eov_mutex_hid_Delete(m->base);
//    
//    delete m;   
//}


//extern eOresult_t eom_mutex_Take(EOMmutex *m, eOreltime_t tout)
//{
//    if(NULL == m)
//    {
//        return(eores_NOK_nullpointer);
//    }
//    
//    return (true == embot::os::rtos::mutex_take(m->mutex, tout)) ? eores_OK : eores_NOK_generic;
//}


//extern eOresult_t eom_mutex_Release(EOMmutex *m)
//{
//    if(NULL == m)
//    {
//        return(eores_NOK_nullpointer);
//    }
//    
//     embot::os::rtos::mutex_release(m->mutex);
//    
//    return eores_OK;
//}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static eOresult_t s_eom_mutex_take(void *p, eOreltime_t tout) 
{
    EOMmutex *m = reinterpret_cast<EOMmutex*>(p);    
    return (true == embot::os::rtos::mutex_take(m->mutex, tout)) ? eores_OK : eores_NOK_generic;        
}


static eOresult_t s_eom_mutex_release(void *p) 
{
    EOMmutex *m = reinterpret_cast<EOMmutex*>(p);
    embot::os::rtos::mutex_release(m->mutex);
    return eores_OK;
}

static eOresult_t s_eom_mutex_delete(void *p) 
{
    EOMmutex *m = reinterpret_cast<EOMmutex*>(p);
    
    if(NULL == m)
    {
        return eores_NOK_generic;
    }
    
    if(NULL == m->mutex)
    {
        return eores_NOK_generic;
    }
    
    embot::os::rtos::mutex_delete(m->mutex);
    
    eov_mutex_hid_Delete(m->base);
    
    delete m;  
    
    return(eores_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



