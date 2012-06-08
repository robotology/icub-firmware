/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
// - doxy
// --------------------------------------------------------------------------------------------------------------------

/* @file       EOMappDataCollector.c
    @brief      This file contains internal implementation for application data Collector
    @author     valentina.gaggero@iit.it
    @date       02/20/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "stdlib.h"     // to see NULL, calloc etc.
#include "string.h"     //memcpy

// abstraction layers
#include "hal.h"
#include "hal_debugPin.h"

//embobj
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"
#include "EOtheBOARDTransceiver.h"
#include "EOtheBOARDTransceiver_hid.h"
#include "EOarray.h"

//embobj-cfg-icub
#include "EoMotionControl.h"
#include "eOcfg_nvsEP_sk.h"   //==> included to clear skin array


// application
#include "EOappEncodersReader.h"
#include "EOappCanServicesProvider.h"
#include "EOappTheNVmapRef.h" //==> included to clear skin array


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "EOMappDataCollector.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "EOMappDataCollector_hid.h" 




// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
#define EVT_CHECK(var, EVTMASK)             (EVTMASK == (var&EVTMASK))



#define TASK_DATACOLLECTOR_PRIO          60   // task piority

//************************ system controller task events **************************************************
#define EVT_GETDATA_START          (1 << 0)
#define EVT_GETDATA_STOP           (1 << 1)


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
extern  uint32_t ena_tx_onrx = 0;


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
//function used to configure eOMtask obj
void s_eom_appDataCollector_taskInit(void *p);
static void s_eom_appDataCollector_taskStartup(EOMtask *p, uint32_t t);
static void s_eom_appDataCollector_taskRun(EOMtask *tsk, uint32_t evtmsgper); 

//function to manage appl run mode
static void s_eom_appDataCollector_taskRun_skinOnly_mode(EOMappDataCollector *p, eOevent_t evt);
static void s_eom_appDataCollector_taskRun_2foc_mode(EOMappDataCollector *p, eOevent_t evt);
static void s_eom_appDataCollector_taskRun_skinAndMc4_mode(EOMappDataCollector *p, eOevent_t evt);

static void s_eom_appDataCollector_readSkin(EOMappDataCollector *p);
static void s_eom_appDataCollector_GetAndProcessEthPkt(EOMappDataCollector *p);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
static const char s_eobj_ownname[] = "EOMappDataCollector";

uint32_t encoders_values[6];
uint8_t motorControllerMod_motorValues[4][8]; 
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern EOMappDataCollector* eom_appDataCollector_New(EOMappDataCollector_cfg_t *cfg)
{
    EOMappDataCollector *retptr = NULL;

    if(NULL == cfg)
    {
        return(retptr);
    }

//    EOethBaseModule_cfg_t eth_mod_cfg = 
//    {
//        EO_INIT(.dtagramQueue_itemNum)  2,
//        EO_INIT(.dtagramQueue_itemSize) 128,
//        EO_INIT(.remaddr)               0x01010102,
//        EO_INIT(.localport)             3334,
//        EO_INIT(.action_onRec)          NULL,
//        EO_INIT(.periodTx)              0
//    };    
    
       
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOMappDataCollector), 1);

    retptr->st = eOm_appDataCollector_st__idle;

    //copy configuration data
    memcpy(&(retptr->cfg), cfg, sizeof(EOMappDataCollector_cfg_t));


    retptr->appl_runMode = applrunMode__default;

    retptr->mytask = eom_task_New(eom_mtask_EventDriven,
                              TASK_DATACOLLECTOR_PRIO ,
                              2*1024,           //stacksize: da rivedere
                              s_eom_appDataCollector_taskStartup, 
                              s_eom_appDataCollector_taskRun,  
                              0,                //message queue size. the task is evt based 
                              eok_reltimeINFINITE,
                              retptr, 
                              s_eom_appDataCollector_taskInit, 
                              "dataCollector" );
    
    return(retptr);
}


extern eOresult_t eom_appDataCollector_Activate(EOMappDataCollector *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

//    eo_ethBaseModule_Activate(p->eth_mod);
    p->st = eOm_appDataCollector_st__active;

     return(eores_OK);
}


extern eOresult_t eom_appDataCollector_Deactivate(EOMappDataCollector *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
//    eo_ethBaseModule_Deactivate(p->eth_mod);
    p->st = eOm_appDataCollector_st__idle;

     return(eores_OK);
}


extern eOresult_t eom_appDataCollector_CollectDataStart(EOMappDataCollector *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

     if(eOm_appDataCollector_st__idle == p->st)
     {
        return(eores_NOK_generic);   
     }
     p->st = eOm_appDataCollector_st__collectingData;
     eom_task_isrSetEvent(p->mytask, EVT_GETDATA_START);//eom_task_SetEvent(p->mytask, EVT_GETDATA_START);
     return(eores_OK);
}

extern eOresult_t eom_appDataCollector_CollectDataStop(EOMappDataCollector *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(eOm_appDataCollector_st__collectingData != p->st)
    {
        return(eores_NOK_generic);   
    }

     p->st = eOm_appDataCollector_st__active;
     eom_task_isrSetEvent(p->mytask, EVT_GETDATA_STOP);
     return(eores_OK);
}

    
extern eOresult_t eom_appDataCollector_SetRunMode(EOMappDataCollector *p, eOmn_appl_runMode_t runmode)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    p->appl_runMode = runmode;
    
    return(eores_OK);
}
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
void s_eom_appDataCollector_taskInit(void *p)
{
    eom_task_Start(p);
}


static void s_eom_appDataCollector_taskStartup(EOMtask *tsk, uint32_t t)
{
    EOMappDataCollector *p = (EOMappDataCollector*)eom_task_GetExternalData(tsk);

    eo_errman_Assert(eo_errman_GetHandle(), NULL != p, s_eobj_ownname, "extdata() is NULL");

}

static void s_eom_appDataCollector_taskRun(EOMtask *tsk, uint32_t evtmsgper)
{
    EOMappDataCollector *p = (EOMappDataCollector*)eom_task_GetExternalData(tsk);

    eOevent_t evt = (eOevent_t)evtmsgper;   

    hal_led_on(hal_led1);
    hal_led_off(hal_led0);

    switch(p->appl_runMode)
    {
        case applrunMode__skinOnly:
        {
            s_eom_appDataCollector_taskRun_skinOnly_mode(p, evt);
            
        }break;
        
        case applrunMode__2foc:
        {
            s_eom_appDataCollector_taskRun_2foc_mode(p, evt);
        }break;
        
        case applrunMode__skinAndMc4:
        {
            s_eom_appDataCollector_taskRun_skinAndMc4_mode(p, evt);
        }break;
        
        case applrunMode__mc4Only:
        {
            ;
        }break;

        default:
        {
            ;
        };

    };
        
}

static void s_eom_appDataCollector_taskRun_skinOnly_mode(EOMappDataCollector *p, eOevent_t evt)
{

    if(eo_common_event_check(evt, EVT_GETDATA_START))
    {
        /* 1) get and process eth pkt */
        s_eom_appDataCollector_GetAndProcessEthPkt(p);
        /* 2) read and process can frame of skin */
        s_eom_appDataCollector_readSkin(p); 
    }
    
    
    if(eo_common_event_check(evt, EVT_GETDATA_STOP))
    {

        hal_led_on(hal_led3);
        //on stop evt nothing to do, only send evt

        if(NULL != p->cfg.sig2appMotorController.fn)
        {
            p->cfg.sig2appMotorController.fn(p->cfg.sig2appMotorController.argoffn);            
        }
    }    

}

static void s_eom_appDataCollector_taskRun_2foc_mode(EOMappDataCollector *p, eOevent_t evt)
{

    if(eo_common_event_check(evt, EVT_GETDATA_START))
    {
        /* 1) start encoder reading */
        eo_appEncReader_startRead(p->cfg.encReader_ptr);
 
        /* 2) get and process eth pkt */
        s_eom_appDataCollector_GetAndProcessEthPkt(p);

        /* 3) process received can frame DA VERIFICARE: PER ORA NON TESTATO!!!!!*/
        eo_appCanSP_read(p->cfg.canSP_ptr, eOcanport1, 4, NULL); 


       /* 4) get encoders values*/
       if(eobool_true == eo_appEncReader_isReady(p->cfg.encReader_ptr))
       {
           eo_appEncReader_getValues(p->cfg.encReader_ptr, encoders_values);
       }
       else
       {
           //Soluzione 1: mi invio un evento dedicato alla lettura dell'encoder 
           //nella gestione di questo evento verifico se i valorio sono pronti:
           //  se si ==>  ok
           //  altrimenti mi rinvio l'evento

           //Soluzione 2: faccio una lettura degli encoder su ricezione dell'evt_stop.
           

           //soluzione 3: faccio una get degli encoder letti nel motion control, tanto e' inutile farla qui,
           //copiare i valori e passarli al motion control
           //ho deciso di adottare questa soluzione e quindi ho commentato le letture degli encoder
       }
     
 
    }

    if(eo_common_event_check(evt, EVT_GETDATA_STOP))
    {

        hal_led_on(hal_led3);
       if(eobool_true == eo_appEncReader_isReady(p->cfg.encReader_ptr))
       {
           eo_appEncReader_getValues(p->cfg.encReader_ptr, encoders_values);
       }

        if(NULL != p->cfg.sig2appMotorController.fn)
        {
            p->cfg.sig2appMotorController.fn(p->cfg.sig2appMotorController.argoffn);            
        }
    }    
}
static void s_eom_appDataCollector_taskRun_skinAndMc4_mode(EOMappDataCollector *p, eOevent_t evt)
{
    s_eom_appDataCollector_readSkin(p);
    //something else about mc4

}

static void s_eom_appDataCollector_readSkin(EOMappDataCollector *p)
{
    void *memRef;
    EOarray *sk_array;
    eOresult_t res;
    
    //1) reset nv array
    res = eo_appTheNVmapRef_GetSkinNVMemoryRef(eo_appTheNVmapRef_GetHandle(), 0, skinNVindex_sstatus__arrayof10canframe, &memRef);
    if(eores_OK != res)
    {
        return; //TODO: error management
    }

    sk_array = (EOarray*)memRef;
    eo_array_Reset(sk_array);

    eo_appCanSP_read(p->cfg.canSP_ptr, eOcanport2, 10, NULL); //10 is the max size of sk_array

}

static void s_eom_appDataCollector_GetAndProcessEthPkt(EOMappDataCollector *p)
{
    EOpacket *pkt_ptr;
    uint16_t numberofrops;
    eOabstime_t txtime;
    eOresult_t res;
    
    /* process received eth packet*/
    res = eo_ethBaseModule_GetPacket(p->cfg.eth_mod, &pkt_ptr);
    if(eores_OK == res)
    {
        eo_transceiver_Receive(eo_boardtransceiver_GetHandle(), pkt_ptr, &numberofrops, &txtime);
    }

}
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



