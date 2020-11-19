/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
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
#include "string.h"

#include "EoCommon.h"
#include "EOtheErrorManager.h"
#include "EoError.h"
#include "EOVtheSystem.h"
#include "hal_trace.h"
#include "hal_gyroscope.h"

#include "EOtheSharedHW.h"
#include "EOaction.h"

#include "EOtheServices_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheMEMS.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheMEMS_hid.h"


#if defined(EOTHESERVICES_disable_theMEMs)

    // provide empty implementation, so that we dont need to change the caller of the API

    extern EOtheMEMS* eo_mems_Initialise(const eOmems_cfg_t *cfg)
    {
        return NULL;
    }

    extern EOtheMEMS* eo_mems_GetHandle(void)
    {
        return NULL;
    }

    extern eObool_t eo_mems_IsSensorSupported(EOtheMEMS *p, eOmems_sensor_t sensor)
    {
        return eobool_false;
    }

    extern eOresult_t eo_mems_Config(EOtheMEMS *p, eOmems_sensor_cfg_t *cfg)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_mems_Start(EOtheMEMS *p)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_mems_SetAlert(EOtheMEMS *p, EOMtask *task, eOevent_t event)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_mems_Get(EOtheMEMS *p, eOas_inertial_data_t* data, eOreltime_t timeout, eOmems_sensor_t *sensor, uint16_t* remaining)
    {
        return eores_NOK_generic;
    }
    
    extern eOresult_t eo_mems_Stop(EOtheMEMS *p)
    {
        return eores_NOK_generic;
    }

#elif !defined(EOTHESERVICES_disable_theMEMs)

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------

const eOmems_cfg_t eo_mems_DefaultCfg = 
{
    EO_INIT(.priority)          11,
    EO_INIT(.stacksize)         1024
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef enum
{
    mems_evt_gyro_init      = 0x00000001,
    mems_evt_gyro_read      = 0x00000002,
    mems_evt_gyro_deinit    = 0x00000004    
} eo_mems_evt_t;




// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eo_mems_taskworker_startup(EOMtask *rt, uint32_t n);

static void s_eo_mems_taskworker_run(EOMtask *rt, uint32_t n);

static eObool_t s_eo_mems_init_gyro(void); 
static eObool_t s_eo_mems_read_gyro(eOas_inertial_data_t *data);
static eObool_t s_eo_mems_deinit_gyro(void);




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheMEMS s_eo_themems = 
{
    EO_INIT(.fifoofinertialdata)    NULL,
    EO_INIT(.semaphorefifo)         NULL,
    EO_INIT(.initted)               eobool_false,
    EO_INIT(.enabled)               eobool_false,
    EO_INIT(.newresultsavailable)   eobool_false,
    EO_INIT(.taskworker)            NULL,
    EO_INIT(.ticktimer)             NULL,
    EO_INIT(.config) 
    {
        EO_INIT(.sensor)                        mems_gyroscope_l3g4200,
        EO_INIT(.properties.gyroscope.range)    hal_gyroscope_range_500dps,
        EO_INIT(.acquisitionrate)               50
    },
    EO_INIT(.sensorisinitted)       eobool_false
};

static const char s_eobj_ownname[] = "EOtheMEMS";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheMEMS* eo_mems_Initialise(const eOmems_cfg_t *cfg)
{
    EOtheMEMS* p = &s_eo_themems;
    
    if(eobool_true == p->initted)
    {
        return(p);
    }
    
    if(NULL == cfg)
    {
        cfg = &eo_mems_DefaultCfg;       
    }
    
    // create all synch data (semaphore, results, etc)
    
    eo_sharedhw_Initialise(NULL);
    
    p->semaphorefifo = osal_semaphore_new(1, 1);

    // start the evt-based task
    
    const eOreltime_t timeout = 100*EOK_reltime1ms;
    
    p->taskworker = eom_task_New(   eom_mtask_EventDriven, cfg->priority, cfg->stacksize,
                                                    s_eo_mems_taskworker_startup, s_eo_mems_taskworker_run,
                                                    0, timeout,
                                                    NULL, 
                                                    eo_mems,
                                                    "mems"
                                                    );
    
    
    p->ticktimer = eo_timer_New();
    
    p->initted = eobool_false;
    p->newresultsavailable = eobool_false;
        
    p->semaphorefifo = osal_semaphore_new(1, 1);

    p->fifoofinertialdata = eo_vector_New(sizeof(eOas_inertial_data_t), 8, NULL, 0, NULL, NULL);
    
    p->initted = eobool_true;
    
    return(&s_eo_themems);   
}


extern EOtheMEMS* eo_mems_GetHandle(void)
{
    if(eobool_true == s_eo_themems.initted)
    {
        return(&s_eo_themems);
    }
    return(NULL);
}


extern eObool_t eo_mems_IsSensorSupported(EOtheMEMS *p, eOmems_sensor_t sensor)
{
    if(NULL == p)
    {
        return(eobool_false);
    }  

    if(mems_gyroscope_l3g4200 != sensor)
    {
        return(eobool_false);
    }
    
    return(hal_gyroscope_supported_is(hal_gyroscope1));    
}


extern eOresult_t eo_mems_Config(EOtheMEMS *p, eOmems_sensor_cfg_t *cfg)
{
    if((NULL == p) || (NULL == cfg))
    {
        return(eores_NOK_nullpointer);
    }
    
    // at least store params so that one can later on start the timer
    
    memcpy(&p->config, cfg, sizeof(eOmems_sensor_cfg_t));
    
    p->sensorisinitted = eobool_false;
    
    eom_task_SetEvent(p->taskworker, mems_evt_gyro_init);


    return(eores_OK);
}


extern eOresult_t eo_mems_Get(EOtheMEMS *p, eOas_inertial_data_t* data, eOreltime_t timeout, eOmems_sensor_t *sensor, uint16_t* remaining)
{
    if((NULL == p) || (NULL == data))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->enabled)
    {   // nothing to do because it is not enabled
        return(eores_NOK_generic);
    }
        

    data->id = eoas_inertial_none;
    eOresult_t res = eores_NOK_generic;

    // i try to lock: i get the semaphore.    
    if(osal_res_OK == osal_semaphore_decrement(p->semaphorefifo, timeout))
    {  
        if(eobool_true != eo_vector_Empty(p->fifoofinertialdata))
        {
            eOas_inertial_data_t *front = (eOas_inertial_data_t*) eo_vector_Front(p->fifoofinertialdata);
            if(NULL != front)
            {
                memcpy(data, front, sizeof(eOas_inertial_data_t));
                eo_vector_PopFront(p->fifoofinertialdata);
                res = eores_OK;
                if(NULL != sensor)
                {
                    *sensor = (mems_gyroscope_l3g4200 == data->id) ? (mems_gyroscope_l3g4200) : (mems_gyroscope_l3g4200);
                }
                if(NULL != remaining)
                {
                    *remaining = eo_vector_Size(p->fifoofinertialdata);
                }
            }            
        }
//        else
//        {
//            res = eores_NOK_generic;
//            data->id = eoas_inertial_none;
//        }
            
        osal_semaphore_increment(p->semaphorefifo, osal_callerAUTOdetect);
    }
//    else
//    {
//        data->id = eoas_inertial_none;
//        res = eores_NOK_generic;       
//    }    

    return(res);
}



extern eOresult_t eo_mems_Start(EOtheMEMS *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_true == p->enabled)
    {   // nothing to do because it is already enabled
        return(eores_OK);
    }
    
    p->enabled = eobool_true;
    
    // must start a timer which sends events to the worker task
    
    eom_task_SetEvent(p->taskworker, mems_evt_gyro_init);
         
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    eo_action_SetEvent(act, mems_evt_gyro_read, p->taskworker);        
        
    eo_timer_Start(p->ticktimer, eok_abstimeNOW, p->config.acquisitionrate, eo_tmrmode_FOREVER, act);             
              
    return(eores_OK);
}



extern eOresult_t eo_mems_Stop(EOtheMEMS *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->enabled)
    {   // nothing to do because it is already stopped
        return(eores_OK);
    }    
    
    // must stop the timer    
    eo_timer_Stop(p->ticktimer);
    
    eom_task_SetEvent(p->taskworker, mems_evt_gyro_deinit);
    
    // the taskworker will do it ...
    //p->enabled = eobool_false;
          
    return(eores_OK);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// name of the task as it is shown in uvision
void eo_mems(void *p)
{
     eom_task_Start((EOMtask*)p);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_eo_mems_taskworker_startup(EOMtask *rt, uint32_t t)
{
    eo_errman_Trace(eo_errman_GetHandle(), "called _taskworker_startup()", s_eobj_ownname);
}

static void s_eo_mems_taskworker_run(EOMtask *rt, uint32_t t)
{
    EOtheMEMS* p = &s_eo_themems;
    
    eOevent_t evt = (eOevent_t)t;
    // in here we look at the n value . if it is an evt of kind gyro_read then ...
    
    // we have a ... timeout event
    if(0 == evt)
    {
        return;
    }
    
    if(eobool_false == p->enabled)
    {   
        // we dont protect this variable vs concurrent access. in here we just read it and we dont really care if we do one less or ne more run of checks.
        return;
    }
        
    if(eobool_true == eo_common_event_check(evt, mems_evt_gyro_init))
    {
        const osal_reltime_t wait = osal_reltimeINFINITE;
        if(eores_OK == eo_sharedhw_Obtain(eo_sharedhw_GetHandle(), eosharedhw_resource_I2C3, wait))
        {   
            //eo_errman_Trace(eo_errman_GetHandle(), "i2c3 is taken for init", s_eobj_ownname);
            
            s_eo_mems_init_gyro();
            
            //eo_errman_Trace(eo_errman_GetHandle(), "i2c3 is released after init", s_eobj_ownname);            
            eo_sharedhw_Release(eo_sharedhw_GetHandle(), eosharedhw_resource_I2C3);
        }        
    }
    
    if(eobool_true == eo_common_event_check(evt, mems_evt_gyro_read))
    {
        // do the reading.
        // wait util the hw resource is available and then ...
        //const osal_reltime_t wait = osal_reltimeZERO;
        const osal_reltime_t wait = osal_reltimeINFINITE;
        
        eOas_inertial_data_t data = {0};
        eObool_t ok = eobool_false;
        
        // begin of activity.
        if(eores_OK == eo_sharedhw_Obtain(eo_sharedhw_GetHandle(), eosharedhw_resource_I2C3, wait))
        {   
            //eo_errman_Trace(eo_errman_GetHandle(), "i2c3 is taken for read", s_eobj_ownname);
            ok = s_eo_mems_read_gyro(&data);
            //eo_errman_Trace(eo_errman_GetHandle(), "i2c3 is released after read", s_eobj_ownname);            
            eo_sharedhw_Release(eo_sharedhw_GetHandle(), eosharedhw_resource_I2C3);
        }
        else
        {
            eo_errman_Trace(eo_errman_GetHandle(), "i2c3 is busy", s_eobj_ownname); 
        }
        
        if(eobool_true == ok)
        {
            // i read the value and i put it inside a fifo of reading values ...
            // the fifo must be protected by a mutex vs concurrent access: the worker task has access as a producer, but also a consumer will access it.

            const osal_reltime_t timeout = osal_reltimeINFINITE;        
            if(osal_res_OK == osal_semaphore_decrement(p->semaphorefifo, timeout))
            {        
                if(eobool_true == eo_vector_Full(p->fifoofinertialdata))
                {
                    eo_vector_PopFront(p->fifoofinertialdata);
                }
                
                eo_vector_PushBack(p->fifoofinertialdata, &data);
                
                osal_semaphore_increment(p->semaphorefifo, osal_callerAUTOdetect);
            }                  
        }
    }  

    if(eobool_true == eo_common_event_check(evt, mems_evt_gyro_deinit))
    {
        const osal_reltime_t wait = osal_reltimeINFINITE;
        if(eores_OK == eo_sharedhw_Obtain(eo_sharedhw_GetHandle(), eosharedhw_resource_I2C3, wait))
        {   
            //eo_errman_Trace(eo_errman_GetHandle(), "i2c3 is taken for deinit", s_eobj_ownname);
            
            s_eo_mems_deinit_gyro();
            
            p->enabled = eobool_false;
            
            //eo_errman_Trace(eo_errman_GetHandle(), "i2c3 is released after init", s_eobj_ownname);            
            eo_sharedhw_Release(eo_sharedhw_GetHandle(), eosharedhw_resource_I2C3);
        }        
    }    
    
}


static eObool_t s_eo_mems_init_gyro(void)
{
    EOtheMEMS* p = &s_eo_themems;
    eObool_t ok = eobool_false;

    hal_gyroscope_cfg_t config;
    config.range = p->config.properties.gyroscope.range;
    
    hal_result_t res = hal_gyroscope_init(hal_gyroscope1, &config);
    
    if(hal_res_OK == res)
    {
        ok = eobool_true;
    }

    return(ok);
}



// the worker thread calls it regularly
static eObool_t s_eo_mems_read_gyro(eOas_inertial_data_t *data)
{
    EOtheMEMS* p = &s_eo_themems;
    eObool_t ok = eobool_false;

    p->newresultsavailable = eobool_false;
    
    int16_t x = 0;
    int16_t y = 0;
    int16_t z = 0;
    
    hal_result_t res = hal_gyroscope_readraw(hal_gyroscope1, &x, &y, &z);
    
    if(hal_res_OK == res)
    {
        data->timestamp = eov_sys_LifeTimeGet(eov_sys_GetHandle());
        data->id = mems_gyroscope_l3g4200;
        data->x = x;
        data->y = y;
        data->z = z;        
        ok = eobool_true;        
    }

    p->newresultsavailable = ok;    
   
    
    return(ok);
}

static eObool_t s_eo_mems_deinit_gyro(void)
{
//    EOtheMEMS* p = &s_eo_themems;
    eObool_t ok = eobool_false;
    
    hal_result_t res = hal_gyroscope_deinit(hal_gyroscope1);
    
    if(hal_res_OK == res)
    {
        ok = eobool_true;
    }

    return(ok);
}

#endif // #elif !defined(EOTHESERVICES_disable_theMEMs)

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



