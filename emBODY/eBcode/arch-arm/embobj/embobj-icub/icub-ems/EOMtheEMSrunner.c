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

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSrunner.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSrunner_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOemsrunner_cfg_t eom_emsrunner_DefaultCfg = 
{
    EO_INIT(.taskpriority)             {62,     61,     60},  
    EO_INIT(.taskstacksize)            {1024,   1024,   1024}
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

extern void tskEMSrunRX(void *p);
extern void tskEMSrunDO(void *p);
extern void tskEMSrunTX(void *p);

static void s_eom_emsrunner_taskRX_startup(EOMtask *p, uint32_t t);
static void s_eom_emsrunner_taskRX_run(EOMtask *p, uint32_t t);

static void s_eom_emsrunner_taskDO_startup(EOMtask *p, uint32_t t);
static void s_eom_emsrunner_taskDO_run(EOMtask *p, uint32_t t);

static void s_eom_emsrunner_taskTX_startup(EOMtask *p, uint32_t t);
static void s_eom_emsrunner_taskTX_run(EOMtask *p, uint32_t t);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOMtheEMSrunner";

 
static EOMtheEMSrunner s_theemsrunner = 
{
    EO_INIT(.task)          {NULL, NULL, NULL}
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOMtheEMSrunner * eom_emsrunner_Initialise(const eOemsrunner_cfg_t *cfg)
{
    if(NULL != s_theemsrunner.task[0])
    {
        return(&s_theemsrunner);
    }
    
    if(NULL == cfg)
    {
        cfg = &eom_emsrunner_DefaultCfg;
    }
    
    s_theemsrunner.task[eo_emsrunner_taskid_runRX] = eom_task_New(eom_mtask_OnAllEventsDriven, 
                                                                  cfg->taskpriority[eo_emsrunner_taskid_runRX], 
                                                                  cfg->taskstacksize[eo_emsrunner_taskid_runRX], 
                                                                  s_eom_emsrunner_taskRX_startup, s_eom_emsrunner_taskRX_run,  
                                                                  (eOevent_t)(eo_emsrunner_evt_enable) & (eOevent_t)(eo_emsrunner_evt_execute), 
                                                                  eok_reltimeINFINITE, NULL, 
                                                                  tskEMSrunRX, "tskEMSrunRX");
 
    s_theemsrunner.task[eo_emsrunner_taskid_runDO] = eom_task_New(eom_mtask_OnAllEventsDriven, 
                                                                  cfg->taskpriority[eo_emsrunner_taskid_runDO], 
                                                                  cfg->taskstacksize[eo_emsrunner_taskid_runDO], 
                                                                  s_eom_emsrunner_taskDO_startup, s_eom_emsrunner_taskDO_run,  
                                                                  (eOevent_t)(eo_emsrunner_evt_enable) & (eOevent_t)(eo_emsrunner_evt_execute), 
                                                                  eok_reltimeINFINITE, NULL, 
                                                                  tskEMSrunDO, "tskEMSrunDO"); 
                                                                  
    s_theemsrunner.task[eo_emsrunner_taskid_runTX] = eom_task_New(eom_mtask_OnAllEventsDriven, 
                                                                  cfg->taskpriority[eo_emsrunner_taskid_runTX], 
                                                                  cfg->taskstacksize[eo_emsrunner_taskid_runTX], 
                                                                  s_eom_emsrunner_taskTX_startup, s_eom_emsrunner_taskTX_run,  
                                                                  (eOevent_t)(eo_emsrunner_evt_enable) & (eOevent_t)(eo_emsrunner_evt_execute), 
                                                                  eok_reltimeINFINITE, NULL, 
                                                                  tskEMSrunTX, "tskEMSrunTX");                                                              
                                                   
    
    return(&s_theemsrunner);
}


extern EOMtheEMSrunner* eom_emsrunner_GetHandle(void) 
{
    if(NULL != s_theemsrunner.task)
    {
        return(&s_theemsrunner);
    }
    else
    {
        return(NULL);
    }
}

extern EOMtask * eom_emsrunner_GetTask(EOMtheEMSrunner *p, eOemsrunner_taskid_t id)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    return(s_theemsrunner.task[id]);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern void tskEMSrunRX(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start(p);
} 

extern void tskEMSrunDO(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start(p);
} 

extern void tskEMSrunTX(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start(p);
} 




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_eom_emsrunner_taskRX_startup(EOMtask *p, uint32_t t)
{
    
}



static void s_eom_emsrunner_taskRX_run(EOMtask *p, uint32_t t)
{


}


static void s_eom_emsrunner_taskDO_startup(EOMtask *p, uint32_t t)
{
    
}



static void s_eom_emsrunner_taskDO_run(EOMtask *p, uint32_t t)
{


}


static void s_eom_emsrunner_taskTX_startup(EOMtask *p, uint32_t t)
{
    
}



static void s_eom_emsrunner_taskTX_run(EOMtask *p, uint32_t t)
{


}






// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





