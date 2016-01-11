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
#include "string.h"
#include "EOtheMemoryPool.h"

#include "EOtheErrorManager.h"

#include "EOVtask_hid.h"
#include "EOMmutex.h"

#include "osal.h"

#include "stdio.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtask.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtask_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define _EOMTASK_USE_FAST_IMPL_

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eom_task_msg_driven_loop(void *param);
static void s_eom_task_cbk_driven_loop(void *param);
static void s_eom_task_anyevt_driven_loop(void *param);
static void s_eom_task_allevt_driven_loop(void *param);
static void s_eom_task_periodic_loop(void *param);
static void s_eom_task_userdefined(void *param);

// virtual
static eOresult_t s_eom_task_isr_set_event(EOVtaskDerived *t, eOevent_t evt);
// virtual
static eOresult_t s_eom_task_tsk_set_event(EOVtaskDerived *t, eOevent_t evt);
// virtual
static eOresult_t s_eom_task_isr_send_message(EOVtaskDerived *t, eOmessage_t msg);
// virtual
extern eOresult_t s_eom_task_tsk_send_message(EOVtaskDerived *t, eOmessage_t msg, eOreltime_t tout);
// virtual
static eOresult_t s_eom_task_isr_exec_callback(EOVtaskDerived *t, eOcallback_t cbk, void *arg);
// virtual
static eOresult_t s_eom_task_tsk_exec_callback(EOVtaskDerived *t, eOcallback_t cbk, void *arg, eOreltime_t tout);
// virtual
static eOid08_t s_eom_task_get_id(EOVtaskDerived *t);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOMtask";

static char s_eom_task_count = 0;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

#undef PRINT_TASK_ADDITIONAL_INFO

extern EOMtask * eom_task_New(eOmtaskType_t type, uint8_t priority, uint16_t stacksize,
                                       void (*startup_fn)(EOMtask *tsk, uint32_t zero),
                                       void (*run_fn)(EOMtask *tsk, uint32_t evtmsgper), 
                                       uint32_t queuesizeORalleventsmask, eOreltime_t timeoutORperiod,
									   void *extdata,
                                       void (*nameofthetask_fn)(void *tsk),
                                       const char *name
									   )

{
    EOMtask *retptr = NULL;
#if defined(PRINT_TASK_ADDITIONAL_INFO)
    char typestr[10];
#endif

    // verify that we have at least a forever-fn
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != run_fn), "eom_task_New(): NULL run_fn", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);

    // verify that we have non-zero stack size
    eo_errman_Assert(eo_errman_GetHandle(), (0 != stacksize), "eom_task_New(): 0 stacksize", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);
    
    // get a new multitask task
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOMtask), 1);

    // get the base task. its vtable shall be initted depending on its kind
    retptr->tsk = eov_task_hid_New();

	retptr->extdata = extdata;

                          
    // initialisations which depend on the type of task
    switch(type)
    {
        case eom_mtask_UserDefined:
        {   // accepts evt but their processing is user-defined

            retptr->type            = eom_mtask_UserDefined;
            retptr->osalrun         = s_eom_task_userdefined;

            retptr->ustime          = 0;
            retptr->waitmask        = 0;
            retptr->maxmessages     = 0;
            retptr->messagequeue    = NULL;
            retptr->argumentqueue   = NULL;

            eov_task_hid_SetVTABLE( retptr->tsk, 
                                    (eOvoid_fp_voidp_uint32_t)startup_fn, (eOvoid_fp_voidp_uint32_t)run_fn,
                                    s_eom_task_isr_set_event, s_eom_task_tsk_set_event,
                                    NULL, NULL,
                                    NULL, NULL,
                                    s_eom_task_get_id
                                  );
#if defined(PRINT_TASK_ADDITIONAL_INFO)
            snprintf(typestr, sizeof(typestr), "%s", "user-def");
#endif
        } break;

        case eom_mtask_EventDriven:
        {   // accepts evt only and their processing is internally defined

            retptr->type            = eom_mtask_EventDriven;
            retptr->osalrun         = s_eom_task_anyevt_driven_loop;

            retptr->ustime          = timeoutORperiod;
            retptr->waitmask        = 0;
            retptr->maxmessages     = 0;
            retptr->messagequeue    = NULL;
            retptr->argumentqueue   = NULL;
            
            eov_task_hid_SetVTABLE( retptr->tsk, 
                                    (eOvoid_fp_voidp_uint32_t)startup_fn, (eOvoid_fp_voidp_uint32_t)run_fn,
                                    s_eom_task_isr_set_event, s_eom_task_tsk_set_event,
                                    NULL, NULL,
                                    NULL, NULL,
                                    s_eom_task_get_id
                                  );
#if defined(PRINT_TASK_ADDITIONAL_INFO)
            snprintf(typestr, sizeof(typestr), "%s", "evt-drvn");
#endif
        } break;

        case eom_mtask_OnAllEventsDriven:
        {   // accepts evt only and their processing is internally defined

            retptr->type            = eom_mtask_OnAllEventsDriven;
            retptr->osalrun         = s_eom_task_allevt_driven_loop;

            retptr->ustime          = timeoutORperiod;
            retptr->waitmask        = queuesizeORalleventsmask;
            retptr->maxmessages     = 0;
            retptr->messagequeue    = NULL;
            retptr->argumentqueue   = NULL;
            
            eov_task_hid_SetVTABLE( retptr->tsk, 
                                    (eOvoid_fp_voidp_uint32_t)startup_fn, (eOvoid_fp_voidp_uint32_t)run_fn,
                                    s_eom_task_isr_set_event, s_eom_task_tsk_set_event,
                                    NULL, NULL,
                                    NULL, NULL,
                                    s_eom_task_get_id
                                  );

#if defined(PRINT_TASK_ADDITIONAL_INFO)
            snprintf(typestr, sizeof(typestr), "%s", "evt-drvn");
#endif
        } break;

        case eom_mtask_MessageDriven:
        {   // accepts msg only and their processing is internally defined

            retptr->type            = eom_mtask_MessageDriven;
            retptr->osalrun         = s_eom_task_msg_driven_loop;

            retptr->ustime          = timeoutORperiod;
            retptr->waitmask        = 0;
            retptr->maxmessages     = (0 == queuesizeORalleventsmask) ? (1) : (queuesizeORalleventsmask);   // cannot be zero.
            retptr->messagequeue    = osal_messagequeue_new(retptr->maxmessages);
            retptr->argumentqueue   = NULL;

            // osal may return NULL
            eo_errman_Assert(eo_errman_GetHandle(), (NULL != retptr->messagequeue), "eom_task_New(): osal gives NULL m-messagequeue", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);

            eov_task_hid_SetVTABLE( retptr->tsk, 
                                    (eOvoid_fp_voidp_uint32_t)startup_fn, (eOvoid_fp_voidp_uint32_t)run_fn,
                                    NULL, NULL,
                                    s_eom_task_isr_send_message, s_eom_task_tsk_send_message,
                                    NULL, NULL,
                                    s_eom_task_get_id
                                  );

#if defined(PRINT_TASK_ADDITIONAL_INFO)
            snprintf(typestr, sizeof(typestr), "%s", "msg-drvn");
#endif
        } break;

        case eom_mtask_CallbackDriven:
        {   // accepts only activity requests (via the message queue) and its execution is internally defined

            retptr->type            = eom_mtask_CallbackDriven;
            retptr->osalrun         = s_eom_task_cbk_driven_loop;

            retptr->ustime          = timeoutORperiod;
            retptr->waitmask        = 0;
            retptr->maxmessages     = (0 == queuesizeORalleventsmask) ? (1) : (queuesizeORalleventsmask);   // cannot be zero.
            retptr->messagequeue    = osal_messagequeue_new(retptr->maxmessages);
            retptr->argumentqueue   = osal_messagequeue_new(retptr->maxmessages);

            // osal may return NULL
            eo_errman_Assert(eo_errman_GetHandle(), (NULL != retptr->messagequeue),   "eom_task_New(): osal gives NULL cb-messagequeue", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);
            eo_errman_Assert(eo_errman_GetHandle(), (NULL != retptr->argumentqueue), "eom_task_New(): osal gives NULL cb-argumentqueue", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);

            eov_task_hid_SetVTABLE( retptr->tsk, 
                                    (eOvoid_fp_voidp_uint32_t)startup_fn, (eOvoid_fp_voidp_uint32_t)run_fn,
                                    NULL, NULL,
                                    NULL, NULL,
                                    s_eom_task_isr_exec_callback, s_eom_task_tsk_exec_callback,
                                    s_eom_task_get_id
                                  );

#if defined(PRINT_TASK_ADDITIONAL_INFO)
            snprintf(typestr, sizeof(typestr), "%s", "actv-drvn");
#endif
        } break;

        case eom_mtask_Periodic:
        {   // does not accepts msg, evt or cbk. it only executes a fixed avtivity at regular interval of time

            retptr->type            = eom_mtask_Periodic;
            retptr->osalrun         = s_eom_task_periodic_loop;

            retptr->ustime          = timeoutORperiod;
            retptr->waitmask        = 0;
            retptr->maxmessages     = 0;
            retptr->messagequeue    = NULL;
            retptr->argumentqueue   = NULL;

            eov_task_hid_SetVTABLE( retptr->tsk, 
                                    (eOvoid_fp_voidp_uint32_t)startup_fn, (eOvoid_fp_voidp_uint32_t)run_fn,
                                    NULL, NULL,
                                    NULL, NULL,
                                    NULL, NULL,
                                    s_eom_task_get_id
                                  );

#if defined(PRINT_TASK_ADDITIONAL_INFO)
            snprintf(typestr, sizeof(typestr), "%s", "periodic");
#endif
        } break;

                
        default:
        {
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "eom_task_New(): unsupported type", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);
        } break;
    }


    
    // now i create the osal task

    // adjust the parameters. with osal there is no need to retrieve stack memory as stack is reserved by osal_task_new()

    if((priority<osal_prio_usrtsk_max) && (priority>osal_prio_usrtsk_min))
    {
        retptr->priority =  priority;   
    }
    else
    {
        retptr->priority = (priority<osal_prio_usrtsk_min) ? (osal_prio_usrtsk_min) : (osal_prio_usrtsk_max);
    }

    stacksize = (stacksize+7)/8;
    stacksize *= 8;



    if(NULL != nameofthetask_fn)
    {
        retptr->osaltask = osal_task_new(nameofthetask_fn, retptr, retptr->priority, stacksize); // fn, arg, prio, stksize
        osal_task_extdata_set(retptr->osaltask, retptr);
    }
    else
    {
        retptr->osaltask = osal_task_new(retptr->osalrun, retptr, retptr->priority, stacksize);
        osal_task_extdata_set(retptr->osaltask, retptr);
    }

    // osaltask must not be NULL, thus i check it.
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != retptr->osaltask), "eom_task_New(): osal gives NULL osaltask", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);

    s_eom_task_count ++;
#if defined(PRINT_TASK_ADDITIONAL_INFO)
    char str[128] = {0};
    osal_task_id_t id = 0;
    osal_task_id_get(retptr->osaltask, &id);
    snprintf(str, sizeof(str), "#%d: %s, id %d, pr %d, %s", s_eom_task_count, name, id, priority, typestr);
    eo_errman_Info(eo_errman_GetHandle(), str, s_eobj_ownname, NULL);
#endif
    // ok. everything is done. when the rtos will start, then the run function of the task will be executed
    
    return(retptr);
}


extern void* eom_task_GetExternalData(EOMtask *p)
{
    if(NULL == p) 
    {
        return(NULL);
    }

    return(p->extdata);
}

extern eOresult_t eom_task_SetEvent(EOMtask *p, eOevent_t evt) 
{
#if 0 // its ok, but better using the base call as it already built with every control at creation of the EOMtask
    if(NULL == p) 
    {
        return(eores_NOK_nullpointer);
    }

    if((eom_mtask_UserDefined != p->type) && (eom_mtask_EventDriven != p->type))
    {
        return(eores_NOK_generic);
    }

    // can be only _ok as osaltask is never null.
    return((eOresult_t)osal_eventflag_set(evt, p->osaltask, osal_callerTSK));
#else
    return(eov_task_tskSetEvent(p, evt));
#endif
}


extern eOresult_t eom_task_isrSetEvent(EOMtask *p, eOevent_t evt) 
{
#if 0 // its ok, but better using the base call as it already built with every control at creation of the EOMtask
    if(NULL == p) 
    {
        return(eores_NOK_nullpointer);
    }

    if((eom_mtask_UserDefined != p->type) && (eom_mtask_EventDriven != p->type))
    {
        return(eores_NOK_generic);
    }

    // can be only _ok as osaltask is never null.
    return((eOresult_t)osal_eventflag_set(evt, p->osaltask, osal_callerISR));
#else
    return(eov_task_isrSetEvent(p, evt));
#endif
}


extern eOresult_t eom_task_SendMessage(EOMtask *p, eOmessage_t msg, eOreltime_t tout) 
{
#if 0 // its ok, but better using the base call as it already built with every control at creation of the EOMtask
    eOresult_t res = eores_NOK_nullpointer;

    if(NULL == p)
    {
         return(eores_NOK_nullpointer);
    }

    if(NULL == p->messagequeue)
    {
        return(eores_NOK_nullpointer);
    }

    if(eom_mtask_MessageDriven != p->type)
    {
        return(eores_NOK_generic);
    }

    res = (eOresult_t) osal_messagequeue_put(p->messagequeue, (osal_message_t)msg, tout, osal_callerTSK);

    return(res);
#else
    return(eov_task_tskSendMessage(p, msg, tout));
#endif
}


extern eOresult_t eom_task_isrSendMessage(EOMtask *p, eOmessage_t msg) 
{
#if 0 // its ok, but better using the base call as it already built with every control at creation of the EOMtask    
    eOresult_t res = eores_NOK_nullpointer;

    if(NULL == p)
    {
         return(eores_NOK_nullpointer);
    }

    if(NULL == p->messagequeue)
    {
        return(eores_NOK_nullpointer);
    }

    if(eom_mtask_MessageDriven != p->type)
    {
        return(eores_NOK_generic);
    }

    res = (eOresult_t) osal_messagequeue_put(p->messagequeue, (osal_message_t)msg, 0, osal_callerISR);

    return(res);
#else
    return(eov_task_isrSendMessage(p, msg));
#endif
}


extern eOresult_t eom_task_ExecCallback(EOMtask *p, eOcallback_t cbk, void *arg, eOreltime_t tout)
{
#if 0 // its ok, but better using the base call as it already built with every control at creation of the EOMtask    
    eOresult_t res = eores_NOK_nullpointer;

    if(NULL == p)
    {
         return(eores_NOK_nullpointer);
    }

    if(NULL == p->messagequeue)
    {
        return(eores_NOK_nullpointer);
    }

    if(eom_mtask_MessageDriven != p->type)
    {
        return(eores_NOK_generic);
    }

    res = (eOresult_t) osal_messagequeue_put(p->messagequeue, (osal_message_t)cbk, tout, osal_callerTSK);

    return(res);
#else
    return(eov_task_tskExecCallback(p, cbk, arg, tout));
#endif
}


extern eOresult_t eom_task_isrExecCallback(EOMtask *p, eOcallback_t cbk, void *arg)
{
#if 0 // its ok, but better using the base call as it already built with every control at creation of the EOMtask    
    eOresult_t res = eores_NOK_nullpointer;

    if(NULL == p)
    {
         return(eores_NOK_nullpointer);
    }

    if(NULL == p->messagequeue)
    {
        return(eores_NOK_nullpointer);
    }

    if(eom_mtask_CallbackDriven != p->type)
    {
        return(eores_NOK_generic);
    }

    res = (eOresult_t) osal_messagequeue_put(p->messagequeue, (osal_message_t)cbk, 0, osal_callerISR);

    return(res);
#else
    return(eov_task_isrExecCallback(p, cbk, arg));
#endif
}


extern eOresult_t eom_task_PrioritySet(EOMtask *p, uint8_t prio)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    p->priority = prio;

    return((eOresult_t)osal_task_priority_set(p->osaltask, prio));
}


extern eOresult_t eom_task_PriorityGet(EOMtask *p, uint8_t *prio)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    *prio = p->priority;

    return(eores_OK);
}


//extern uint16_t eom_task_UnusedStackSize(EOMtask *p)
//{
//    return(ral_TASK_stack_sizeofunused(p->stackdata, p->stacksize));
//}


extern void eom_task_Start(EOMtask *p) 
{
    if(NULL != p)
    {
        p->osalrun(p);
    }
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern eOresult_t eom_task_hid_WaitMessage(EOMtask *p, eOmessage_t *msg, eOreltime_t tout) 
{
    eOresult_t res = eores_NOK_nullpointer;

    uint32_t *rr = NULL;

    if(NULL == p)
    {
         return(eores_NOK_nullpointer);
    }

    if(NULL == p->messagequeue)
    {
        return(eores_NOK_nullpointer);
    }

    res = (eOresult_t) osal_messagequeue_get(p->messagequeue, &rr, tout, osal_callerTSK);
    *msg = (eOmessage_t)rr;

    return(res);
}


extern eOresult_t eom_task_hid_WaitEvent(EOMtask *p, eOevent_t msk, eOevent_t *rxmsk, eOreltime_t tout)
{
    eOresult_t res = eores_NOK_timeout; 
    eOevent_t rx = 0;

    if(NULL == p) 
    {
        *rxmsk = 0;
        return(eores_NOK_nullpointer);
    }

    // can be _ok or _nok_timeout
    res = (eOresult_t) osal_eventflag_get(msk, osal_waitANYflag, &rx, tout);
    *rxmsk = rx;

    return(res);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


// virtual
static eOresult_t s_eom_task_isr_set_event(EOVtaskDerived *t, eOevent_t evt) 
{
    // ok, get back my type.
    EOMtask *mtsk = (EOMtask *)t;
    
//    #warning: we could remove teh check vs argument not NULL, as long as it is called correctly
//    if(NULL == mtsk) 
//    {
//        return(eores_NOK_nullpointer);
//    }

    // returns _OK, as _NOK_nullpointer cannot be (osaltask is never null).
    return((eOresult_t)osal_eventflag_set(evt, mtsk->osaltask, osal_callerISR));
}


// virtual
static eOresult_t s_eom_task_tsk_set_event(EOVtaskDerived *t, eOevent_t evt) 
{
    // ok, get back my type.
    EOMtask *mtsk = (EOMtask *)t;

//    #warning: we could remove teh check vs argument not NULL, as long as it is called correctly    
//    if(NULL == mtsk) 
//    {
//        return(eores_NOK_nullpointer);
//    }

    // returns _OK, as _NOK_nullpointer cannot be (osaltask is never null).    
    return((eOresult_t)osal_eventflag_set(evt, mtsk->osaltask, osal_callerTSK));
}


// virtual
static eOresult_t s_eom_task_isr_send_message(EOVtaskDerived *t, eOmessage_t msg) 
{
    // ok, get back my type.
    EOMtask *mtsk = (EOMtask *)t;

//    #warning: we could remove teh check vs argument not NULL, as long as it is called correctly     
//    if(NULL == mtsk) 
//    {
//        return(eores_NOK_nullpointer);
//    }
    if(NULL == mtsk->messagequeue)
    {
        return(eores_NOK_nullpointer);
    }
    
    return((eOresult_t)osal_messagequeue_put(mtsk->messagequeue, (osal_message_t)msg, 0, osal_callerISR));
}


// virtual
static eOresult_t s_eom_task_tsk_send_message(EOVtaskDerived *t, eOmessage_t msg, eOreltime_t tout) 
{
    // ok, get back my type.
    EOMtask *mtsk = (EOMtask *)t;

//    #warning: we could remove teh check vs argument not NULL, as long as it is called correctly     
//    if(NULL == mtsk) 
//    {
//        return(eores_NOK_nullpointer);
//    }

    if(NULL == mtsk->messagequeue)
    {
        return(eores_NOK_nullpointer);
    }  
     
    return((eOresult_t)osal_messagequeue_put(mtsk->messagequeue, (osal_message_t)msg, tout, osal_callerTSK));
}


// virtual
static eOresult_t s_eom_task_isr_exec_callback(EOVtaskDerived *t, eOcallback_t cbk, void *arg) 
{
    // ok, get back my type.
    EOMtask *mtsk = (EOMtask *)t;

    eOresult_t res = eores_NOK_generic;

//    #warning: we could remove teh check vs argument not NULL, as long as it is called correctly     
//    if(NULL == mtsk) 
//    {
//        return(eores_NOK_nullpointer);
//    }
    if((NULL == mtsk->messagequeue) || (NULL == mtsk->argumentqueue))
    {
        return(eores_NOK_nullpointer);
    }

    if(0 != osal_messagequeue_available(mtsk->argumentqueue, osal_callerISR))
    {
        res = (eOresult_t)osal_messagequeue_put(mtsk->argumentqueue, (osal_message_t)arg, 0, osal_callerISR);
        res = (eOresult_t)osal_messagequeue_put(mtsk->messagequeue, (osal_message_t)cbk, 0, osal_callerISR);
    }

    return(res);
}


// virtual
static eOresult_t s_eom_task_tsk_exec_callback(EOVtaskDerived *t, eOcallback_t cbk, void *arg, eOreltime_t tout) 
{
    // ok, get back my type.
    EOMtask *mtsk = (EOMtask *)t;

    eOresult_t res = eores_NOK_generic;

//    #warning: we could remove teh check vs argument not NULL, as long as it is called correctly     
//    if(NULL == mtsk) 
//    {
//        return(eores_NOK_nullpointer);
//    }

    if((NULL == mtsk->messagequeue) || (NULL == mtsk->argumentqueue))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(0 != osal_messagequeue_available(mtsk->argumentqueue, osal_callerISR))
    {
        res = (eOresult_t)osal_messagequeue_put(mtsk->argumentqueue, (osal_message_t)arg, tout, osal_callerTSK);
        res = (eOresult_t)osal_messagequeue_put(mtsk->messagequeue, (osal_message_t)cbk, tout, osal_callerTSK);
    }      
     
    return(res);
}


// virtual
extern eOid08_t s_eom_task_get_id(EOVtaskDerived *t) 
{
    // ok, get back my type.
    EOMtask *mtsk = (EOMtask *)t;
    osal_task_id_t id = 0;

//    #warning: we could remove the check vs argument not NULL, as long as it is called correctly     
//    if(NULL == mtsk) 
//    {
//        return(eores_NOK_nullpointer);
//    }

    osal_task_id_get(mtsk->osaltask, &id);
       
     
    return(id);
}


static void s_eom_task_userdefined(void *param) 
{
    EOMtask *mtsk = (EOMtask *)param;
    eOvoid_fp_voidp_uint32_t v_startup = (eOvoid_fp_voidp_uint32_t)mtsk->tsk->vtable[VF00_startup];
    eOvoid_fp_voidp_uint32_t v_run = (eOvoid_fp_voidp_uint32_t)mtsk->tsk->vtable[VF01_run];
    
    // run the startup
    if(NULL != v_startup)
    {
        v_startup(mtsk, 0);
    }

    // run the user-defined function
    if(NULL != v_run)
    {
        v_run(mtsk, 0);
    }

}


static void s_eom_task_anyevt_driven_loop(void *param) 
{
    EOMtask *mtsk = (EOMtask *)param;
#if !defined(_EOMTASK_USE_FAST_IMPL_)
    eOevent_t evt = 0;
#endif
    const uint32_t tout = mtsk->ustime;
    eOvoid_fp_voidp_uint32_t v_startup = (eOvoid_fp_voidp_uint32_t)mtsk->tsk->vtable[VF00_startup];
    eOvoid_fp_voidp_uint32_t v_run = (eOvoid_fp_voidp_uint32_t)mtsk->tsk->vtable[VF01_run];

    // run the startup
    if(NULL != v_startup)
    {
        v_startup(mtsk, 0);
    }

#if !defined(_EOMTASK_USE_FAST_IMPL_)
    for(;;)
    {
        if(eores_NOK_timeout != eom_task_hid_WaitEvent(mtsk, 0xFFFFFFFF, &evt, tout))
//        if(eores_NOK_timeout != osal_eventflag_get(0xFFFFFFFF, osal_waitANY, &evt, tout))
        {
            v_run(mtsk, (uint32_t)evt);
        }
        else
        {
            // execute timeout
            v_run(mtsk, 0); // we are in timeout, thus evt is 0
        }
    }
#else
    for(;;)
    {
        v_run(mtsk, osal_eventflag_getany(tout));
    }
#endif
    
}

static void s_eom_task_allevt_driven_loop(void *param) 
{
    EOMtask *mtsk = (EOMtask *)param;
    eOevent_t rxmsk = 0;
    eOevent_t waitmsk = mtsk->waitmask;
    const uint32_t tout = mtsk->ustime;
    eOvoid_fp_voidp_uint32_t v_startup = (eOvoid_fp_voidp_uint32_t)mtsk->tsk->vtable[VF00_startup];
    eOvoid_fp_voidp_uint32_t v_run = (eOvoid_fp_voidp_uint32_t)mtsk->tsk->vtable[VF01_run];

    // run the startup
    if(NULL != v_startup)
    {
        v_startup(mtsk, 0);
    }


    for(;;)
    {
        if(eores_NOK_timeout != osal_eventflag_get(waitmsk, osal_waitALLflags, &rxmsk, tout))
        {
            v_run(mtsk, (uint32_t)rxmsk);
        }
        else
        {
            // execute timeout
            v_run(mtsk, 0); // we are in timeout, thus evt is 0
        }
    }
    
}



static void s_eom_task_msg_driven_loop(void *param) 
{
    EOMtask *mtsk = (EOMtask *)param;
#if !defined(_EOMTASK_USE_FAST_IMPL_)
    eOmessage_t msg = 0;
#endif
    const uint32_t tout = mtsk->ustime;
    uint32_t msg = 0;
    eOvoid_fp_voidp_uint32_t v_startup = (eOvoid_fp_voidp_uint32_t)mtsk->tsk->vtable[VF00_startup];
    eOvoid_fp_voidp_uint32_t v_run = (eOvoid_fp_voidp_uint32_t)mtsk->tsk->vtable[VF01_run];

    
    // run the startup
    if(NULL != v_startup)
    {
        v_startup(mtsk, 0);
    }

#if !defined(_EOMTASK_USE_FAST_IMPL_)
    for(;;)
    {
        // the task has a not-null message-queue, thus result can be _ok or _nok_timeout
        if(eores_NOK_timeout != eom_task_hid_WaitMessage(mtsk, &msg, tout))
        {
            v_run(mtsk, msg);
        }
        else
        {
            // execute timeout
            v_run(mtsk, 0);
        }
    }
#else
    for(;;)
    { 
        msg = (uint32_t) osal_messagequeue_getquick(mtsk->messagequeue, tout, osal_callerTSK);
        v_run(mtsk, msg);
    }
#endif
}


static void s_eom_task_cbk_driven_loop(void *param) 
{
    EOMtask *mtsk = (EOMtask *)param;
    const uint32_t tout = mtsk->ustime;
    eOcallback_t cbk = NULL;
    void * arg = NULL;
    eOvoid_fp_voidp_uint32_t v_startup = (eOvoid_fp_voidp_uint32_t)mtsk->tsk->vtable[VF00_startup];
    eOvoid_fp_voidp_uint32_t v_run = (eOvoid_fp_voidp_uint32_t)mtsk->tsk->vtable[VF01_run];

    
    // run the startup
    if(NULL != v_startup)
    {
        v_startup(mtsk, 0);
    }

    for(;;)
    { 
        cbk = (eOcallback_t) osal_messagequeue_getquick(mtsk->messagequeue, tout, osal_callerTSK);
        arg = (void*) osal_messagequeue_getquick(mtsk->argumentqueue, tout, osal_callerTSK);
        if(NULL != cbk)
        {
            cbk(arg);                      // the callback
        }

        v_run(mtsk, (uint32_t)cbk);     // for any other purpose   
    }
}


static void s_eom_task_periodic_loop(void *param) 
{
    EOMtask *mtsk = (EOMtask *)param;
    uint32_t period = mtsk->ustime;
    eOvoid_fp_voidp_uint32_t v_startup = (eOvoid_fp_voidp_uint32_t)mtsk->tsk->vtable[VF00_startup];
    eOvoid_fp_voidp_uint32_t v_run = (eOvoid_fp_voidp_uint32_t)mtsk->tsk->vtable[VF01_run];
    
    // run the startup
    if(NULL != v_startup)
    {
        v_startup(mtsk, 0);
    }

    osal_task_period_set(period);
    for(;;)
    {
        osal_task_period_wait();
        v_run(mtsk, period);
    }
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

