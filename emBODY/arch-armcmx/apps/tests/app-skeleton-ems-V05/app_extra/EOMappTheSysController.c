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

/* @file       EOMappTheSysController.c
    @brief      This file contains internal implementation of EOMappTheSysController singleton obj
    @author     valentina.gaggero@iit.it
    @date       01/02/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "stdlib.h"
#include "string.h"

//abslayer
#include "hal.h" 
#include "hal_debugPin.h"

//embobj
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"
#include "EOMmutex.h"
#include "EOaction_hid.h"

//icub
#include "EoMotionControl.h"
#include "EoSkin.h"

//nv
#include "EOtheBOARDtransceiver_hid.h"

//nv-cfg
#include "eOcfg_nvsEP_mc.h"
#include "eOcfg_nvsEP_sk.h" 

//can
#include "EOemsCanNetworkTopology.h"

//appl - srv
#include "EOethBaseModule.h"
#include "EOappTheNVmapRef.h"
#include "EOappCanServicesProvider.h"
#include "EOappEncodersReader.h"
//appl - act
#include "EOMappDataCollector.h"
#include "EOMappDatatransmitter.h"
#include "EOMappMotorController.h"


//#include "eOcfg_EPs_eb4.h"
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "EOMappTheSysController.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "EOMappTheSysController_hid.h" 



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
#define         APP_THESYSCONTROL_TASKPRIO                65   

//system controller task events 
#define         APP_THESYSCONTROL_EVT_DGRAMREC           (1 << 0) 


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
extern EOethBaseModule_cfg_t    eom_appTheSysController_ethmod_cfg;
extern EOethBaseModule_cfg_t    mc_ethmod_cfg;
extern uint16_t                 connectedEncodersMask_cfg;


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
void eom_appTheSysController_taskInit(void *p);
static void s_eom_appTheSysController_startup(EOMtask *p, uint32_t t);
static void s_eom_appTheSysController_run(EOMtask *tsk, uint32_t evtmsgper);

static void s_eom_appTheSysController_recDgram_mng(EOMappTheSysController *p);

static eOresult_t s_eom_appTheSysController_Timers_Init(void);
static void s_timer2_appFreq_cbk(void *p);
static void s_timer3_dataCollect_cbk(void *p);
static void s_timer4_motorCntrlStart_cbk(void *p);
static void s_eom_appTheSysController_Timers_Start(void);
static void s_eom_appTheSysController_Timers_Stop(void);

static void s_eom_appTheSysController_GetRunMode(void);
static void s_eom_appTheSysController_GetCanConnectedStuff(void);
static void s_eom_appTheSysController_PopulateNVmapRef(void);
static void s_eom_appTheSysController_SetSocketRunMode(eOm_appTheSysController_socketRunMode_hid_t sockRunMode);
static void s_eom_appThesysController_ConfigValidityCheck(void);

/* function about init services */
static void s_eom_appTheSysController_services_init(void);
static void s_eom_appTheSysController_srv_ledInit(void);
static void s_eom_appTheSysController_srv_theBoardTransceiver_init(void);
static void s_eom_appTheSysController_srv_CanServicesProvider_init(void);
static void s_eom_appTheSysController_srv_EncodersReader_init(void);
static void s_eom_appTheSysController_srv_ethService_init(void);

/* functions about init actors */
static void s_eom_appTheSysController_actors_init(void);
static void s_eom_appTheSysController_act_appMotorController_init(void);
static void s_eom_appTheSysController_act_appDataTransmitter_init(void);
static void s_eom_appTheSysController_act_appDataCollector_init(void);
static void s_eom_appTheSysController_actors_start(void);

/* fn on cfg */
static eOresult_t s_eom_appTheSysController_ConnectedCanBoards_Configure(void);
static eOresult_t s_eom_appTheSysController_ConnectedCanBoards_Start(void);
static eOresult_t s_eom_appTheSysController_ConnectedCanBoards_Stop(void);
// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
static const char s_eobj_ownname[] = "EOMappTheSysController";

static EOMappTheSysController s_theSysController = 
{
    EO_INIT(.st)    eOm_appTheSysController_st__NOTinited
};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern EOMappTheSysController* eom_appTheSysController_Initialise(EOMappTheSysController_cfg_t *cfg)
{
    EOMappTheSysController *retptr = NULL;

    if(NULL == cfg)
    {
        return(retptr);
    }

    retptr = &s_theSysController;

    retptr->st = eOm_appTheSysController_st__NOTinited;

    //save in the singleton its configuration
    memcpy(&retptr->cfg, cfg, sizeof(EOMappTheSysController_cfg_t));

    retptr->mytask = NULL;


    // 1) initialize service providers : if an arror accurs error manager is invoked
    s_eom_appTheSysController_services_init();

    // 2) get joints,motors, sensors and skin connected to this ems board
    s_eom_appTheSysController_GetCanConnectedStuff();

    // 3) verify nv configuation is consistent with ems can network topology configuration. 
    s_eom_appThesysController_ConfigValidityCheck();

    // 3) populate networkVariables map
    s_eom_appTheSysController_PopulateNVmapRef();

    // 4) get run mode
    s_eom_appTheSysController_GetRunMode();

    // 5) initialise actors
    s_eom_appTheSysController_actors_init();

    //6) init myself

    // change its status
    retptr->st = eOm_appTheSysController_st__inited;

    return(retptr);    
}



extern EOMappTheSysController* eom_appTheSysController_GetHandle(void)
{
    return((eOm_appTheSysController_st__NOTinited != s_theSysController.st) ? &s_theSysController : NULL);
}

extern eOresult_t eom_appTheSysController_Start(EOMappTheSysController *p)
{

    p->mytask = eom_task_New(eom_mtask_EventDriven,
                                      APP_THESYSCONTROL_TASKPRIO,
                                      3*1024, //stacksize: da rivedere
                                      s_eom_appTheSysController_startup, 
                                      s_eom_appTheSysController_run,  
                                      0, //message queue size. the task is evt based
                                      eok_reltimeINFINITE,
                                      p, 
                                      eom_appTheSysController_taskInit, 
                                      "sysController");
    
    return(eores_OK);
}

//this function is used only in this file, but it must not defined static for debug task viewer.
void eom_appTheSysController_taskInit(void *p)
{
    eom_task_Start(p);
}

extern eOresult_t eom_appTheSysController_Go2ConfigState(EOMappTheSysController *p)
{
    eOresult_t res;
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eOm_appTheSysController_st__started != p->st)
    {
        return(eores_NOK_generic);
    }    
    // 1) set ethbase mod based on evt
    s_eom_appTheSysController_SetSocketRunMode(eOm_appTheSysController_sockRunMode__onEvent);

    // 2) configure connected can boards
    res = s_eom_appTheSysController_ConnectedCanBoards_Configure();
    if(eores_OK != res)
    {
        return(res);
    }


    // 3) initialise its timers
    res = s_eom_appTheSysController_Timers_Init();
    if(eores_OK != res)
    {
        return(res);
    }

    s_theSysController.appl_state = applstate_config;

    return(eores_OK);

}


extern eOresult_t eom_appTheSysController_Go2RunState(EOMappTheSysController *p)
{
    eOresult_t res; 

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eOm_appTheSysController_st__started != p->st)
    {
        return(eores_NOK_generic);
    }    
    // 1) set ethbase mod based on evt
    s_eom_appTheSysController_SetSocketRunMode(eOm_appTheSysController_runMode__onDemand);

    // 2) start all can boards
    res = s_eom_appTheSysController_ConnectedCanBoards_Start();
        if(eores_OK != res)
    {
        return(res);
    }

    // 3) start all actors
    s_eom_appTheSysController_actors_start();

    // 4) start timers
    s_eom_appTheSysController_Timers_Start();
    

    s_theSysController.appl_state = applstate_running;

    return(eores_OK);

}


extern void* eom_appTheSysController_Services_Can_GetHandle(EOMappTheSysController *p)
{

    eo_errman_Assert(eo_errman_GetHandle(), NULL != p, s_eobj_ownname, "get canSP and thSysContr isn't init");    

    return((void*)p->srv.appCanSP_ptr);
}


extern eOresult_t eom_appTheSysController_ExitFromRunState(EOMappTheSysController *p)
{
    s_eom_appTheSysController_Timers_Stop();
    s_eom_appTheSysController_ConnectedCanBoards_Stop();
    return(eores_OK);   
}


extern eOresult_t eom_appTheSysController_Go2ErrorState(EOMappTheSysController *p)
{
/*this is an horrible error state.... but i use this func only for test!!! :)) */
    while(1)
    {;}

}
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
/*
    3 timers are employed by the SysContyroller:
        - timer 2: period = 1000 us. (application_frequency). The application must finish its task in 1000 us. 
        - timer 3: period = 500 us.  (data_collect)time for data collection(from eth, can and encoders))the data collector must finish before this timeout 
        - timer 4: period = 600 us   (motor control start)
*/

static eOresult_t s_eom_appTheSysController_Timers_Init(void)
{
    hal_result_t res;

    #warning VALE-->configure timers'period depending on application run mode

//timer1 is not supported by hal.
    hal_timer_cfg_t t2_appFreq_cfg = 
    {
        .prescaler          = hal_timer_prescalerAUTO,         
        .countdown          = 1000,
        .priority           = hal_int_priority02,
        .mode               = hal_timer_mode_periodic,
        .callback_on_exp    = s_timer2_appFreq_cbk,
        .arg                = (void*)s_theSysController.act.appDataCollector_ptr
    };

    hal_timer_cfg_t t3_dataCollect_cfg = 
    {
        .prescaler          = hal_timer_prescalerAUTO,         
        .countdown          = 500,
        .priority           = hal_int_priority02,
        .mode               = hal_timer_mode_oneshot,
        .callback_on_exp    = s_timer3_dataCollect_cbk,
        .arg                = (void*)s_theSysController.act.appDataCollector_ptr
    };

    hal_timer_cfg_t t4_motorCntrlStart_cfg = 
    {
        .prescaler          = hal_timer_prescalerAUTO,         
        .countdown          = 600,
        .priority           = hal_int_priority02,
        .mode               = hal_timer_mode_oneshot,
        .callback_on_exp    = s_timer4_motorCntrlStart_cbk,
        .arg                = (void*)s_theSysController.act.appMotorController_ptr
    };

    res = hal_timer_init(hal_timer2, &t2_appFreq_cfg, NULL);
    if(hal_res_OK != res)
    {
        return((eOresult_t)res);
    }
    hal_timer_init(hal_timer3, &t3_dataCollect_cfg, NULL);
    if(hal_res_OK != res)
    {
        return((eOresult_t)res);
    }
    hal_timer_init(hal_timer4, &t4_motorCntrlStart_cfg, NULL);
    if(hal_res_OK != res)
    {
        return((eOresult_t)res);
    }
    return(eores_OK);
}

static void s_timer2_appFreq_cbk(void *p)
{

        DEBUG_PIN_ON;

        eom_appDataCollector_CollectDataStart((EOMappDataCollector*)p);

        hal_timer_start(hal_timer3);
        hal_timer_start(hal_timer4);

}


static void s_timer3_dataCollect_cbk(void *p)
{

//    if(NULL != s_theSysController.cfg.sig2appDataCollector_stop.fn)
//    {
//        s_theSysController.cfg.sig2appDataCollector_stop.fn(s_theSysController.cfg.sig2appDataCollector_stop.argoffn);
//    }
       eom_appDataCollector_CollectDataStop((EOMappDataCollector*)p);
}


static void s_timer4_motorCntrlStart_cbk(void *p)
{

//    if(NULL != s_theSysController.cfg.sig2appMotorController.fn)
//    {
//        s_theSysController.cfg.sig2appMotorController.fn(s_theSysController.cfg.sig2appMotorController.argoffn);
//    }

    
    eom_appMotorController_Satrt2Calculate((EOMappMotorController*)p);

}
static void s_eom_appTheSysController_Timers_Start(void)
{
    hal_timer_start(hal_timer2);
}


static void s_eom_appTheSysController_Timers_Stop(void)
{
    hal_timer_stop(hal_timer2);
}


static void s_eom_appTheSysController_startup(EOMtask *tsk, uint32_t t)
{
    EOMappTheSysController *p = (EOMappTheSysController*)eom_task_GetExternalData(tsk);
    eOresult_t res;

    eo_errman_Assert(eo_errman_GetHandle(), NULL != p, s_eobj_ownname, "extdata() is NULL");


    p->st = eOm_appTheSysController_st__started;

    res = eom_appTheSysController_Go2ConfigState(p);
    eo_ethBaseModule_Activate(p->srv.ethMod_ptr);
    #warning VALE --> gestisci errore in go2new state
    res = res;

} 


static void s_eom_appTheSysController_run(EOMtask *tsk, uint32_t evtmsgper)
{
    eOevent_t evt;

    EOMappTheSysController *p = (EOMappTheSysController*)eom_task_GetExternalData(tsk);

    evt = (eOevent_t)evtmsgper;

    
    if(eo_common_event_check(evt, APP_THESYSCONTROL_EVT_DGRAMREC))
    {
        s_eom_appTheSysController_recDgram_mng(p); //received dtagagram manager
    }

}



static void s_eom_appTheSysController_recDgram_mng(EOMappTheSysController *p)
{
    EOpacket *pkt_ptr;
    uint16_t numberofrops;
    eOabstime_t txtime;
    eOresult_t res;

    res = eo_ethBaseModule_GetPacket(p->srv.ethMod_ptr, &pkt_ptr);
    if(eores_OK == res)
    {
        eo_transceiver_Receive(p->srv.transceiver, pkt_ptr, &numberofrops, &txtime);
    }

}





static void s_eom_appTheSysController_services_init(void)
{

    hal_debugPin_init();



/* 1) led init and debugPin init*/
    s_eom_appTheSysController_srv_ledInit();

/* 2) init transceiver */
    s_eom_appTheSysController_srv_theBoardTransceiver_init();

/* 3) init can module */
    s_eom_appTheSysController_srv_CanServicesProvider_init();

/* 4) init encoder reader */
    s_eom_appTheSysController_srv_EncodersReader_init();

/* 5) init eth services */
    s_eom_appTheSysController_srv_ethService_init(); 
    
}

static void s_eom_appTheSysController_srv_ledInit(void)
{
    hal_led_cfg_t cfg = {.dummy=0};

    hal_led_init(hal_led0, &cfg);
    hal_led_off(hal_led0);
    hal_led_init(hal_led1, &cfg);
    hal_led_off(hal_led1);
    hal_led_init(hal_led2, &cfg);
    hal_led_off(hal_led2);
    hal_led_init(hal_led3, &cfg);
    hal_led_off(hal_led3);
}

static void s_eom_appTheSysController_srv_theBoardTransceiver_init(void)
{
    eOboardtransceiver_cfg_t boardtxrxcfg = 
    {
        .vectorof_endpoint_cfg          = s_theSysController.cfg.vectorof_endpoint_cfg, //eo_cfg_EPs_vectorof_eb4, 
        .hashfunction_ep2index          = s_theSysController.cfg.hashfunction_ep2index, //eo_cfg_nvsEP_eb4_fptr_hashfunction_ep2index, 
        .remotehostipv4addr             = s_theSysController.cfg.ethmod_cfg_ptr->remaddr,
        .remotehostipv4port             = s_theSysController.cfg.ethmod_cfg_ptr->remport
    };
    
    s_theSysController.srv.transceiver = eo_boardtransceiver_Initialise(&boardtxrxcfg);
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != s_theSysController.srv.transceiver), 
                     s_eobj_ownname, "error in transceiver");
}


static void s_eom_appTheSysController_srv_CanServicesProvider_init(void)
{
    /* currently its configuration is empty, so useless*/
    s_theSysController.srv.appCanSP_ptr = eo_appCanSP_New(NULL);

    eo_errman_Assert(eo_errman_GetHandle(), (NULL != s_theSysController.srv.appCanSP_ptr), 
                     s_eobj_ownname, "error in appCanSP_New");
}

static void s_eom_appTheSysController_srv_EncodersReader_init(void)
{

    eOappEncReader_cfg_t  cfg = 
    {
        EO_INIT(.connectedEncodersMask)     s_theSysController.cfg.connectedEncodersMask_cfg,
        EO_INIT(.callbackOnLastRead)        NULL
    };

    s_theSysController.srv.appEncReader_ptr = eo_appEncReader_New(&cfg);

    eo_errman_Assert(eo_errman_GetHandle(), (NULL != s_theSysController.srv.appEncReader_ptr), 
                     s_eobj_ownname, "error in appEncReader_New");

}

static void s_eom_appTheSysController_PopulateNVmapRef(void)
{

//EOnvsCfg *nvscfg_ptr = (EOnvsCfg *)eo_boardtransceiver_hid_GetNvsCfg();  
 
    eOappTheNVmapRef_cfg_t cfg =
    {
        EO_INIT(.jointsList)    s_theSysController.jointsList,
        EO_INIT(.motorsList)    s_theSysController.motorsList,
        EO_INIT(.sensorsList)   s_theSysController.sensorsList,
        EO_INIT(.skinList)      s_theSysController.skinList,
        EO_INIT(.nvsCfg)        eo_boardtransceiver_hid_GetNvsCfg(),
        EO_INIT(.mc_endpoint)   s_theSysController.cfg.mc_endpoint, //(0x0014), //right lower arm //(0x0017)- endpoint_mc_leftlowerleg
        EO_INIT(.sk_endpoint)   s_theSysController.cfg.sk_endpoint//(0x0034) //endpoint_sk_emsboard_rightlowerarm
    };


    s_theSysController.srv.appTheNVmapRef_ptr = eo_appTheNVmapRef_Initialise(&cfg);

    eo_errman_Assert(eo_errman_GetHandle(), (NULL != s_theSysController.srv.appTheNVmapRef_ptr), 
                     s_eobj_ownname, "error in appTheNVmapRef_Initialise");

}


static void s_eom_appTheSysController_srv_ethService_init(void)
{

    s_theSysController.srv.ethMod_ptr = eo_ethBaseModule_New(s_theSysController.cfg.ethmod_cfg_ptr);

    eo_errman_Assert(eo_errman_GetHandle(), (NULL != s_theSysController.srv.ethMod_ptr), 
                     s_eobj_ownname, "error in ethBaseModule_New");


//    res = eo_ethBaseModule_Activate(s_theSysController.srv.ethMod_ptr);
//
//    eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), 
//                    s_eobj_ownname, "error in ethBaseModule_Activate");
//
}

/*********** actors ****************************/

static void s_eom_appTheSysController_actors_init(void)
{
/*PAY ATTENTION  AT THE ORDER OF INITIALIZATION OBJ: SOMEONE DEPEND ON OTHER ONE!!!! */

/* 1) create DataTransmitter */
   s_eom_appTheSysController_act_appDataTransmitter_init();

/* 2) create Mortorcontroller */
    s_eom_appTheSysController_act_appMotorController_init();

/* 3) create DataCollector */
   s_eom_appTheSysController_act_appDataCollector_init();

}

static void s_eom_appTheSysController_act_appDataTransmitter_init(void)
{

    EOMappDataTransmitter_cfg_t cfg = 
    {
        EO_INIT(.appCanSP_ptr)     s_theSysController.srv.appCanSP_ptr,
        EO_INIT(.eth_mod)          s_theSysController.srv.ethMod_ptr     
    };

    s_theSysController.act.appDataTransmitter_ptr = eom_appDataTransmitter_New(&cfg);

    eo_errman_Assert(eo_errman_GetHandle(), (NULL != s_theSysController.act.appDataTransmitter_ptr), 
                     s_eobj_ownname, "error in appDataTransmitter_New");
}




static void s_eom_appTheSysController_act_sig2appDataTransmitter(void *arg)
{
    eom_appDataTransmitter_SendData(((EOMappDataTransmitter*)arg));
}

static void s_eom_appTheSysController_act_appMotorController_init(void)
{
    EOMappMotorController_cfg_t cfg =
    {
        EO_INIT(.encReader)                 s_theSysController.srv.appEncReader_ptr,
        EO_INIT(.sig2appDataTransmitter)
        {                                      
            EO_INIT(.fn)                    s_eom_appTheSysController_act_sig2appDataTransmitter,
            EO_INIT(.argoffn)               s_theSysController.act.appDataTransmitter_ptr
        }
    };

    s_theSysController.act.appMotorController_ptr = eom_appMotorController_New(&cfg);

    eo_errman_Assert(eo_errman_GetHandle(), (NULL != s_theSysController.act.appMotorController_ptr), 
                     s_eobj_ownname, "error in appMotorController_New");

}


static void s_eom_appTheSysController_act_sig2appMotorController_byAppDataCollector(void *arg)
{
    eom_appMotorController_AllDataAreReady(((EOMappMotorController*)arg));
}

static void s_eom_appTheSysController_act_appDataCollector_init(void)
{
    EOMappDataCollector_cfg_t cfg =
    {

        EO_INIT(.sig2appMotorController)
        {                                      
            EO_INIT(.fn)                    s_eom_appTheSysController_act_sig2appMotorController_byAppDataCollector, 
            EO_INIT(.argoffn)               s_theSysController.act.appMotorController_ptr
        },  
        EO_INIT(.encReader_ptr)             s_theSysController.srv.appEncReader_ptr,
        EO_INIT(.canSP_ptr)                 s_theSysController.srv.appCanSP_ptr,
        EO_INIT(.eth_mod)                   s_theSysController.srv.ethMod_ptr

    };
    s_theSysController.act.appDataCollector_ptr = eom_appDataCollector_New(&cfg);
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != s_theSysController.act.appDataCollector_ptr), 
                     s_eobj_ownname, "error in appDataCollector_New");
}



static void s_eom_appTheSysController_actors_start(void)
{

/* 1) activate Mortorcontroller */
    eom_appMotorController_Activate(s_theSysController.act.appMotorController_ptr);

/* 2) activate DataCollector */
    eom_appDataCollector_Activate(s_theSysController.act.appDataCollector_ptr);

/* 3) activate DataTransmitter */
    eom_appDataTransmitter_Activate(s_theSysController.act.appDataTransmitter_ptr);

}


static void s_eom_appThesysController_ConfigValidityCheck(void)
{
    //TODO: campare size of constvector of emscannettopo with size opf joint in used enpont
    return;
}


static void s_eom_appTheSysController_GetCanConnectedStuff(void)
{
    uint16_t jointMaxNumber = eo_cfg_nvsEP_mc_joint_numbermax_Get((eOcfg_nvsEP_mc_endpoint_t)s_theSysController.cfg.mc_endpoint);
    uint16_t motorMaxNumber = eo_cfg_nvsEP_mc_motor_numbermax_Get((eOcfg_nvsEP_mc_endpoint_t)s_theSysController.cfg.mc_endpoint);
//    uint16_t sensorMaxNumber = eo_cfg_nvsEP_mc_joint_numbermax_Get(eOcfg_nvsEP_mc_endpoint_t ep);
    uint16_t skinMaxNumber = eo_cfg_nvsEP_sk_sknumbermax_Get((eOcfg_nvsEP_sk_endpoint_t)s_theSysController.cfg.sk_endpoint);

   //get joints, motors, sensors and skin from can configuration
    s_theSysController.jointsList =  eo_array_New(jointMaxNumber, sizeof(eOmc_jointId_t), NULL);
    s_theSysController.motorsList =  eo_array_New(motorMaxNumber, sizeof(eOmc_motorId_t), NULL); 
    s_theSysController.skinList =  eo_array_New(skinMaxNumber, sizeof(eOsk_skinId_t), NULL); 
    s_theSysController.sensorsList =  NULL;
    
    eo_appCanSP_GetConnectedJoints(s_theSysController.srv.appCanSP_ptr, s_theSysController.jointsList);
    eo_appCanSP_GetConnectedMotors(s_theSysController.srv.appCanSP_ptr, s_theSysController.motorsList);
    eo_appCanSP_GetConnectedSkin(s_theSysController.srv.appCanSP_ptr, s_theSysController.skinList);
}


static void s_eom_appTheSysController_GetRunMode(void)
{

    eOsizecntnr_t size;
    eOmc_jointId_t *jid_ptr;
    eo_appCanSP_canLocation canLoc;
    eObrd_types_t motor_boardType;
    eObool_t skin_isConnected = eobool_false;
    eObool_t mc4_isConnected = eobool_false;
    eObool_t foc_isConnected = eobool_false;

    
    size = eo_array_Size(s_theSysController.jointsList);
//    res =  eo_fifoword_Size(s_theSysController.jointsList, &size, 0);
//    eo_errman_Assert(eo_errman_GetHandle(), (eores_OK != res), 
//                     s_eobj_ownname, "error in get size of s_theSysController.jointsList");

    if(size > 0)
    {
        jid_ptr = (eOmc_jointId_t*)eo_array_At(s_theSysController.jointsList, 0);
        eo_appCanSP_GetJointCanLocation(s_theSysController.srv.appCanSP_ptr, *jid_ptr,
                                        &canLoc, &motor_boardType);
        switch(motor_boardType)
        {
            case eobrd_mc4:
            {
                mc4_isConnected = eobool_true;    
            }break;

            case eobrd_1foc:
            {
                foc_isConnected = eobool_true;    
            }break;
        }
    }


    size = eo_array_Size(s_theSysController.skinList);

    if(size > 0)
    {
        skin_isConnected = eobool_true;
    }




    if(mc4_isConnected)
    {
        if(skin_isConnected)
        {
            s_theSysController.appl_runMode = eOm_appTheSysController_runMode__skinAndMc4;
            return;    
        }
        else
        {
            s_theSysController.appl_runMode = eOm_appTheSysController_runMode__mc4Only;
            return;
        }    
    }

    if((foc_isConnected) && (!skin_isConnected))
    {
        s_theSysController.appl_runMode = eOm_appTheSysController_runMode__2foc;
        return;
    }

    if((skin_isConnected) && (!mc4_isConnected) && (!foc_isConnected))
    {
        s_theSysController.appl_runMode = eOm_appTheSysController_runMode__skinOnly;
        return;    
    }

    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "fault in getting appl_runMode");

}


static void s_eom_appTheSysController_SetSocketRunMode(eOm_appTheSysController_socketRunMode_hid_t sockRunMode)
{
    EOaction a;

    eo_errman_Assert(eo_errman_GetHandle(), NULL != s_theSysController.mytask, s_eobj_ownname, "extdata() is NULL");

    if(eOm_appTheSysController_sockRunMode__onEvent == sockRunMode)
    {
        //configure the action to perform on receive datagram
        eo_action_SetEvent(&a, (eOevent_t)APP_THESYSCONTROL_EVT_DGRAMREC , s_theSysController.mytask);
        eo_ethBaseModule_SetActionOnRec(s_theSysController.srv.ethMod_ptr, &a);
        return;
    }

    if(eOm_appTheSysController_runMode__onDemand == sockRunMode)
    {
        eo_action_Clear(&a);
        eo_ethBaseModule_SetActionOnRec(s_theSysController.srv.ethMod_ptr, &a);
        return;
    }
}


static eOresult_t s_eom_appTheSysController_ConnectedCanBoards_Configure(void)
{
    eOresult_t res = eores_NOK_generic;

    switch(s_theSysController.appl_runMode)
    {
        case eOm_appTheSysController_runMode__skinOnly:
        {
            res = eo_appCanSP_ConfigSkin(s_theSysController.srv.appCanSP_ptr, 0);
        
        }break;

        case eOm_appTheSysController_runMode__mc4Only:
        {
            res = eo_appCanSP_ConfigMc4(s_theSysController.srv.appCanSP_ptr);

        }break;

        case eOm_appTheSysController_runMode__skinAndMc4:
        {
            res = eo_appCanSP_ConfigSkin(s_theSysController.srv.appCanSP_ptr, 0);
            if(eores_OK != res)
            {
                return(res);
            }
            res = eo_appCanSP_ConfigMc4(s_theSysController.srv.appCanSP_ptr);
        }break;

        case eOm_appTheSysController_runMode__2foc:
        {
            res = eo_appCanSP_Config2foc(s_theSysController.srv.appCanSP_ptr);
        }break;
    
    }
    return(res);
}


static eOresult_t s_eom_appTheSysController_ConnectedCanBoards_Start(void)
{
    eOresult_t res = eores_NOK_generic;

    switch(s_theSysController.appl_runMode)
    {
        case eOm_appTheSysController_runMode__skinOnly:
        {
            ; //nothing to do
        
        }break;

        case eOm_appTheSysController_runMode__mc4Only:
        {
            ; // start can boards sending PWM_ENA and RUN cmds.

        }break;

        case eOm_appTheSysController_runMode__skinAndMc4:
        {
           ;
        }break;

        case eOm_appTheSysController_runMode__2foc:
        {
            // start can boards sending PWM_ENA and RUN cmds.;
        }break;
    
    }
    #warning VALE-->oly 4 test
    res = eores_OK;
    return(res);
}

static eOresult_t s_eom_appTheSysController_ConnectedCanBoards_Stop(void)
{
    eOresult_t res = eores_NOK_generic;

    switch(s_theSysController.appl_runMode)
    {
        case eOm_appTheSysController_runMode__skinOnly:
        {
            ;
        
        }break;

        case eOm_appTheSysController_runMode__mc4Only:
        {
            ;

        }break;

        case eOm_appTheSysController_runMode__skinAndMc4:
        {
            ;
        }break;

        case eOm_appTheSysController_runMode__2foc:
        {
            ;
        }break;
    
    }
    return(res);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



