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


/* @file       eo_appSkeletonEms_body.c
    @brief      This file keeps ...
    @author     valentina.gaggero@iit.it
    @date       02/20/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
//sys
#include "stdlib.h"
#include "string.h"
#include "EoCommon.h"

//abs-layer
#include "hal.h"
#include "hal_debugPin.h"

//embObj
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"
#include "EOsocketDatagram.h"
#include "EOtheBOARDtransceiver.h"
#include "EOtheBOARDtransceiver_hid.h"
#include "EOfifoByte.h"

#include "EoMotionControl.h"
#include "EOnvsCfg.h"


//embobj of application
#include "EOappCanServicesProvider.h"
#include "EOappEncodersReader.h"
#include "EOMappDataCollector.h"
#include "EOMappDataTransmitter.h"
#include "EOMappMotorController.h"
#include "EOMappTheSysController.h"
#include "EOappTheNVmapRef.h"

//endpoints-cfg
#include "eOcfg_EPs_eb7.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eom_appSkeletonEms_body.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

//#include "eom_appSkeletonEms_body_hid.h" currently this file not exist, becouase not used.


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
static void s_eom_appSkeletonEms_body_OnError(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info);



extern const ipal_cfg_t    ipal_cfg;


extern const ipal_cfg_t* eom_appSkeletonEms_body_ipal_cfg = &ipal_cfg;


extern const eOerrman_cfg_t  eom_appSkeletonEms_body_errcfg = 
{
    .extfn.usr_on_error = s_eom_appSkeletonEms_body_OnError
};

extern const eOmipnet_cfg_dtgskt_t eom_appSkeletonEms_body_dtgskt_cfg = 
{   
    EO_INIT(.numberofsockets)			4, 
    EO_INIT(.maxdatagramenqueuedintx)   8
};




// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
static void s_eom_appSkeletonEms_body_ledInit(void);

//services
static void s_eom_appSkeletonEms_body_theBoardTransceiver_init(void);
static void s_eom_appSkeletonEms_body_CanServicesProvider_init(void);
static void s_eom_appSkeletonEms_body_EncodersReader_init(void);
static void s_eom_appSkeletonEms_body_NVmapRef_init(void);
static void s_eom_appSkeletonEms_body_ethService_init(void);

//actors
static void s_eom_appSkeletonEms_body_TheSysController_init(void);
static void s_eom_appSkeletonEms_body_appDataCollector_init(void);
static void s_eom_appSkeletonEms_body_appMotorController_init(void);
static void s_eom_appSkeletonEms_body_appDataTransmitter_init(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
/*** services'provider obj ***/
/*Note: these objects are used by one or more actors. in this application, mutex are not employed
to access shared obj in safe modo, becouse aech actor works during a specific quantum time 
and race condition should not verify. if it is not so, then  there is a project error.*/
static EOappCanSP           *s_appCanSP_ptr;
static EOappEncReader       *s_appEncReader_ptr;
static EOethBaseModule      *s_ethMod_ptr;

/*** actors obj ***/ 
static EOMappDataCollector      *s_appDataCollector_ptr;
static EOMappMotorController    *s_appMotorController_ptr;
static EOMappDataTransmitter    *s_appDataTransmitter_ptr;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern eOresult_t eom_appSkeletonEms_body_services_init(void)
{

/* 1) led init and debugPin init*/
    s_eom_appSkeletonEms_body_ledInit();
    hal_debugPin_init();

/* 2) init transceiver */
    s_eom_appSkeletonEms_body_theBoardTransceiver_init();

/* 3) init can module */
   s_eom_appSkeletonEms_body_CanServicesProvider_init();

/* 4) init encoder reader */
    s_eom_appSkeletonEms_body_EncodersReader_init();

/* 5) init motorController-networkVariables map*/
    s_eom_appSkeletonEms_body_NVmapRef_init();

/* 6) init eth services */
   s_eom_appSkeletonEms_body_ethService_init(); 
    return(eores_OK);
}


extern eOresult_t eom_appSkeletonEms_body_actors_init(void)
{
/*PAY ATTENTION  AT THE ORDER OF INITIALIZATION OBJ: SOMEONE DEPEND ON OTHER ONE!!!! */


/* 1) create DataTransmitter */
   s_eom_appSkeletonEms_body_appDataTransmitter_init();

/* 2) create Mortorcontroller */
    s_eom_appSkeletonEms_body_appMotorController_init();

/* 3) create DataCollector */
   s_eom_appSkeletonEms_body_appDataCollector_init();

/* 4) initilise system controller */
    s_eom_appSkeletonEms_body_TheSysController_init();

    return(eores_OK);

}


extern eOresult_t eom_appSkeletonEms_body_actors_start(void)
{

/* 1) activate Mortorcontroller */
    eom_appMotorController_Activate(s_appMotorController_ptr);

/* 2) activate DataCollector */
    eom_appDataCollector_Activate(s_appDataCollector_ptr);

/* 3) activate DataTransmitter */
    eom_appDataTransmitter_Activate(s_appDataTransmitter_ptr);

/* 4) start TheSysController */
    eom_appTheSysController_Start(eom_appTheSysController_GetHandle());

    return(eores_OK);
}



extern void* eom_appSkeletonEms_body_services_can_getHandle(void)
{
    return((void*)s_appCanSP_ptr);   
}

extern void* eom_appSkeletonEms_body_services_TheNVmapRef_getHandle(void)
{
    return((void*)s_appCanSP_ptr);   
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static void s_eom_appSkeletonEms_body_OnError(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info)
{
    if(errtype <= eo_errortype_warning)
    {
        return;
    }

    for(;;);
}


static void s_eom_appSkeletonEms_body_ledInit(void)
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

static void s_eom_appSkeletonEms_body_theBoardTransceiver_init(void)
{
    eOboardtransceiver_cfg_t boardtxrxcfg = 
    {
        .vectorof_endpoint_cfg          = eo_cfg_EPs_vectorof_eb7,
        .hashfunction_ep2index          = eo_cfg_nvsEP_eb7_fptr_hashfunction_ep2index,
        .remotehostipv4addr             = 0x01010101,
        .remotehostipv4port             = 3334
    };
    
    eo_boardtransceiver_Initialise(&boardtxrxcfg);



}


static void s_eom_appSkeletonEms_body_CanServicesProvider_init(void)
{
    /* currently its configuration is empty, so useless*/
    s_appCanSP_ptr = eo_appCanSP_New(NULL);

}

static void s_eom_appSkeletonEms_body_EncodersReader_init(void)
{

    eOappEncReader_cfg_t  cfg = 
    {
        EO_INIT(.connectedEncodersMask)     0xFF,
        EO_INIT(.callbackOnLastRead)        NULL
    };


    s_appEncReader_ptr = eo_appEncReader_New(&cfg);

}

static void s_eom_appSkeletonEms_body_NVmapRef_init(void)
{

    EOfifoByte *sList_ptr;

    eOappTheNVmapRef_cfg_t cfg =
    {
        EO_INIT(.jointsList)    eo_fifobyte_New(20 /*jointNumber_TOTALnumber*/, NULL),
        EO_INIT(.motorsList)    eo_fifobyte_New(20 /*motorNumber_TOTALnumber*/, NULL),
        EO_INIT(.sensorsList)   sList_ptr,
        EO_INIT(.nvsCfg)        eo_boardtransceiver_hid_GetNvsCfg(),
        EO_INIT(.mc_endpoint)      (0x0017) //endpoint_mc_leftlowerleg
    };

    //from can configuration I get information about joint connected to ems
    //this let me no waste memory
//   jList_ptr =  eo_fifobyte_New(eOmc_maxnumof_joints_perBodypart, NULL); 

    eo_appCanSP_GetConnectedJoints(s_appCanSP_ptr, cfg.jointsList/*jList_ptr*/);
    eo_appCanSP_GetConnectedJoints(s_appCanSP_ptr, cfg.motorsList/*jList_ptr*/);
#warning VALE--> fai GetConnectedmotors func!!!!
    eo_appTheNVmapRef_Initialise(&cfg);

}


static void s_eom_appSkeletonEms_body_ethService_init(void)
{
    EOethBaseModule_cfg_t eth_mod_cfg = 
    {
        EO_INIT(.dtagramQueue_itemNum)  2,
        EO_INIT(.dtagramQueue_itemSize) 512,
        EO_INIT(.remaddr)               0x01010101,
        EO_INIT(.remport)               3334,
        EO_INIT(.localport)             3334,
        EO_INIT(.action_onRec)          NULL,
        EO_INIT(.periodTx)              0
    }; 

    s_ethMod_ptr = eo_ethBaseModule_New(&eth_mod_cfg);
    eo_ethBaseModule_Activate(s_ethMod_ptr);
}


static void s_eom_appSkeletonEms_body_sig2appDataCollector_start(void *arg)
{
    eom_appDataCollector_CollectDataStart(((EOMappDataCollector*)(arg)));
}

static void s_eom_appSkeletonEms_body_sig2appDataCollector_stop(void *arg)
{
    eom_appDataCollector_CollectDataStop(((EOMappDataCollector*)arg));
}

static void s_eom_appSkeletonEms_body_sig2appMotorController(void *arg)
{
    eom_appMotorController_Satrt2Calculate(((EOMappMotorController*)arg));
}

static void s_eom_appSkeletonEms_body_TheSysController_init(void)
{
    EOMappTheSysController_cfg_t cfg =
    {
        EO_INIT(.ethmod_cfg)
        {
            EO_INIT(.dtagramQueue_itemNum)      2,
            EO_INIT(.dtagramQueue_itemSize)     64,
            EO_INIT(.remaddr)                   0x01010101,
            EO_INIT(.remport)                   3333,
            EO_INIT(.localport)                 3333,
            EO_INIT(.action_onRec)              NULL,
            EO_INIT(.periodTx)                  0
        },

        EO_INIT(.sig2appDataCollector_start)
        {                                      
            EO_INIT(.fn)                        s_eom_appSkeletonEms_body_sig2appDataCollector_start, 
            EO_INIT(.argoffn)                   s_appDataCollector_ptr
        },

        EO_INIT(.sig2appDataCollector_stop)
        {                                      
            EO_INIT(.fn)                        s_eom_appSkeletonEms_body_sig2appDataCollector_stop, 
            EO_INIT(.argoffn)                   s_appDataCollector_ptr
        },  

        EO_INIT(.sig2appMotorController)
        {                                      
            EO_INIT(.fn)                        s_eom_appSkeletonEms_body_sig2appMotorController, 
            EO_INIT(.argoffn)                   s_appMotorController_ptr
        }  

    };
    eom_appTheSysController_Initialise(&cfg);
}

static void s_eom_appSkeletonEms_body_sig2appMotorController_byAppDataCollector(void *arg)
{
    eom_appMotorController_AllDataAreReady(((EOMappMotorController*)arg));
}

static void s_eom_appSkeletonEms_body_appDataCollector_init(void)
{
    EOMappDataCollector_cfg_t cfg =
    {

        EO_INIT(.sig2appMotorController)
        {                                      
            EO_INIT(.fn)                    s_eom_appSkeletonEms_body_sig2appMotorController_byAppDataCollector, 
            EO_INIT(.argoffn)               s_appMotorController_ptr
        },  
        EO_INIT(.encReader_ptr)             s_appEncReader_ptr,
        EO_INIT(.canSP_ptr)                 s_appCanSP_ptr,
        EO_INIT(.eth_mod)                   s_ethMod_ptr

    };
    s_appDataCollector_ptr = eom_appDataCollector_New(&cfg);
}


static void s_eom_appSkeletonEms_body_sig2appDataTransmitter(void *arg)
{
    eom_appDataTransmitter_SendData(((EOMappDataTransmitter*)arg));
}


static void s_eom_appSkeletonEms_body_appMotorController_init(void)
{
    EOMappMotorController_cfg_t cfg =
    {
        EO_INIT(.sig2appDataTransmitter)
        {                                      
            EO_INIT(.fn)                    s_eom_appSkeletonEms_body_sig2appDataTransmitter,
            EO_INIT(.argoffn)               s_appDataTransmitter_ptr
        }
    };

    s_appMotorController_ptr = eom_appMotorController_New(&cfg);
}


static void s_eom_appSkeletonEms_body_appDataTransmitter_init(void)
{

    EOMappDataTransmitter_cfg_t cfg = 
    {
        EO_INIT(.appCanSP_ptr)     s_appCanSP_ptr,
        EO_INIT(.eth_mod)          s_ethMod_ptr     
    };

    s_appDataTransmitter_ptr = eom_appDataTransmitter_New(&cfg);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



