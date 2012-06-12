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
#include "EoCommon.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"
#include "EOsocketDatagram.h"
#include "EOconstvector.h"
#include "EOnv.h"
#include "EOnvsCfg.h"

//in order to get ep number
#include "eOcfg_nvsEP_mc.h"
#include "eOcfg_nvsEP_as.h"
#include "eOcfg_nvsEP_sk.h"



//embobj of application
#include "EOMappTheSysController.h"
//#include "EOappCanServicesProvider.h"
//#include "EOappEncodersReader.h"
//#include "EOMappDataCollector.h"
//#include "EOMappDataTransmitter.h"
//#include "EOMappMotorController.h"
//
//#include "EOappTheNVmapRef.h"
//
//endpoints-cfg
#if     defined(EP_EB4) 
#include "eOcfg_EPs_eb4.h"
#elif   defined(EP_EB2) //left lower arm
    #include "eOcfg_EPs_eb2.h"
#endif

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
static void s_eom_appSkeletonEms_body_t5_blinkgreenled_cbk(void *arg);
static void s_eom_appSkeletonEms_body_t5_blinkredled_cbk(void *arg);
extern const ipal_cfg_t    ipal_cfg;


extern const ipal_cfg_t* eom_appSkeletonEms_body_ipal_cfg = &ipal_cfg;


extern const eOerrman_cfg_t  eom_appSkeletonEms_body_errcfg = 
{
    EO_INIT(.extfn.usr_on_error)     s_eom_appSkeletonEms_body_OnError
};

extern const eOmipnet_cfg_dtgskt_t eom_appSkeletonEms_body_dtgskt_cfg = 
{   
    EO_INIT(.numberofsockets)			1, 
    EO_INIT(.maxdatagramenqueuedintx)   4
};

//extern var for application config
extern EOethBaseModule_cfg_t    *ethmod_cfg_ptr;
extern uint16_t                 connectedEncodersMask_cfg; 


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern void eom_appSkeletonEms_body_application_init(void)
{
    //this timer is used to signal "application is alive"
    hal_timer_cfg_t t5_cfg = 
    {
        .prescaler          = hal_timer_prescalerAUTO,         
        .countdown          = 500000,
        .priority           = hal_int_priority15,
        .mode               = hal_timer_mode_periodic,
        .callback_on_exp    = s_eom_appSkeletonEms_body_t5_blinkgreenled_cbk,
        .arg                = NULL
    };

    hal_timer_init(hal_timer5, &t5_cfg, NULL);
    hal_timer_start(hal_timer5);

#if     defined(EP_EB4)     //right lower arm
    EOMappTheSysController_cfg_t cfg =
    {
        EO_INIT(.ethmod_cfg_ptr)                ethmod_cfg_ptr,
        EO_INIT(.connectedEncodersMask_cfg)     connectedEncodersMask_cfg,
        EO_INIT(.vectorof_endpoint_cfg)         eo_cfg_EPs_vectorof_eb4,
        EO_INIT(.hashfunction_ep2index)         eo_cfg_nvsEP_eb4_fptr_hashfunction_ep2index,
        EO_INIT(.mc_endpoint)                   endpoint_mc_rightlowerarm,       
        EO_INIT(.as_endpoint)                   0, 
        EO_INIT(.sk_endpoint)                   endpoint_sk_emsboard_rightlowerarm 
    };


#elif   defined(EP_EB2) //left lower arm
    
    EOMappTheSysController_cfg_t cfg =
    {
        EO_INIT(.ethmod_cfg_ptr)                ethmod_cfg_ptr,
        EO_INIT(.connectedEncodersMask_cfg)     connectedEncodersMask_cfg,
        EO_INIT(.vectorof_endpoint_cfg)         eo_cfg_EPs_vectorof_eb2,
        EO_INIT(.hashfunction_ep2index)         eo_cfg_nvsEP_eb2_fptr_hashfunction_ep2index,
        EO_INIT(.mc_endpoint)                   endpoint_mc_leftlowerarm,       
        EO_INIT(.as_endpoint)                   0, 
        EO_INIT(.sk_endpoint)                   endpoint_sk_emsboard_leftlowerarm 
    };

#else
        #error choose endpoint!!!
#endif
    
    eom_appTheSysController_Initialise(&cfg);
}

extern void eom_appSkeletonEms_body_application_start(void)
{
    eom_appTheSysController_Start(eom_appTheSysController_GetHandle());
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
    hal_timer_cfg_t t5_cfg = 
    {
        .prescaler          = hal_timer_prescalerAUTO,         
        .countdown          = 100000,
        .priority           = hal_int_priority15,
        .mode               = hal_timer_mode_periodic,
        .callback_on_exp    = s_eom_appSkeletonEms_body_t5_blinkredled_cbk,
        .arg                = NULL
    };
    
    if(errtype <= eo_errortype_warning)
    {
        return;
    }

    hal_timer_stop(hal_timer5);
    hal_timer_init(hal_timer5, &t5_cfg, NULL);
    hal_timer_start(hal_timer5);

    for(;;);
}





static void s_eom_appSkeletonEms_body_t5_blinkgreenled_cbk(void *arg)
{
//     hal_led_off(hal_led0);
//     hal_led_off(hal_led2);
//     hal_led_off(hal_led3);
    hal_led_toggle(hal_led1);
}


static void s_eom_appSkeletonEms_body_t5_blinkredled_cbk(void *arg)
{
    hal_led_off(hal_led1);
    hal_led_off(hal_led2);
    hal_led_off(hal_led3);

    hal_led_toggle(hal_led0); //led red
}

//static void s_eom_appSkeletonEms_body_sig2appDataCollector_start(void *arg)
//{
//    eom_appDataCollector_CollectDataStart(((EOMappDataCollector*)(arg)));
//}
//
//static void s_eom_appSkeletonEms_body_sig2appDataCollector_stop(void *arg)
//{
//    eom_appDataCollector_CollectDataStop(((EOMappDataCollector*)arg));
//}
//
//static void s_eom_appSkeletonEms_body_sig2appMotorController(void *arg)
//{
//    eom_appMotorController_Satrt2Calculate(((EOMappMotorController*)arg));
//}
//
//static void s_eom_appSkeletonEms_body_TheSysController_init(void)
//{
//    EOMappTheSysController_cfg_t cfg =
//    {
//        EO_INIT(.ethmod_cfg)                    {0}, //configured with memcpy
//
//        EO_INIT(.canSP_ptr)                     s_appCanSP_ptr, 
//
//        EO_INIT(.sig2appDataCollector_start)
//        {                                      
//            EO_INIT(.fn)                        s_eom_appSkeletonEms_body_sig2appDataCollector_start, 
//            EO_INIT(.argoffn)                   s_appDataCollector_ptr
//        },
//
//        EO_INIT(.sig2appDataCollector_stop)
//        {                                      
//            EO_INIT(.fn)                        s_eom_appSkeletonEms_body_sig2appDataCollector_stop, 
//            EO_INIT(.argoffn)                   s_appDataCollector_ptr
//        },  
//
//        EO_INIT(.sig2appMotorController)
//        {                                      
//            EO_INIT(.fn)                        s_eom_appSkeletonEms_body_sig2appMotorController, 
//            EO_INIT(.argoffn)                   s_appMotorController_ptr
//        }  
//
//    };
//
//    memcpy(&cfg.ethmod_cfg, &eom_appTheSysController_ethmod_cfg, sizeof(EOethBaseModule_cfg_t));
//
//    eom_appTheSysController_Initialise(&cfg);
//}
//
//static void s_eom_appSkeletonEms_body_sig2appMotorController_byAppDataCollector(void *arg)
//{
//    eom_appMotorController_AllDataAreReady(((EOMappMotorController*)arg));
//}
//
//static void s_eom_appSkeletonEms_body_appDataCollector_init(void)
//{
//    EOMappDataCollector_cfg_t cfg =
//    {
//
//        EO_INIT(.sig2appMotorController)
//        {                                      
//            EO_INIT(.fn)                    s_eom_appSkeletonEms_body_sig2appMotorController_byAppDataCollector, 
//            EO_INIT(.argoffn)               s_appMotorController_ptr
//        },  
//        EO_INIT(.encReader_ptr)             s_appEncReader_ptr,
//        EO_INIT(.canSP_ptr)                 s_appCanSP_ptr,
//        EO_INIT(.eth_mod)                   s_ethMod_ptr
//
//    };
//    s_appDataCollector_ptr = eom_appDataCollector_New(&cfg);
//}
//
//
//static void s_eom_appSkeletonEms_body_sig2appDataTransmitter(void *arg)
//{
//    eom_appDataTransmitter_SendData(((EOMappDataTransmitter*)arg));
//}
//
//
//static void s_eom_appSkeletonEms_body_appMotorController_init(void)
//{
//    EOMappMotorController_cfg_t cfg =
//    {
//        EO_INIT(.encReader)                 s_appEncReader_ptr,
//        EO_INIT(.sig2appDataTransmitter)
//        {                                      
//            EO_INIT(.fn)                    s_eom_appSkeletonEms_body_sig2appDataTransmitter,
//            EO_INIT(.argoffn)               s_appDataTransmitter_ptr
//        }
//    };
//
//    s_appMotorController_ptr = eom_appMotorController_New(&cfg);
//}
//
//
//static void s_eom_appSkeletonEms_body_appDataTransmitter_init(void)
//{
//
//    EOMappDataTransmitter_cfg_t cfg = 
//    {
//        EO_INIT(.appCanSP_ptr)     s_appCanSP_ptr,
//        EO_INIT(.eth_mod)          s_ethMod_ptr     
//    };
//
//    s_appDataTransmitter_ptr = eom_appDataTransmitter_New(&cfg);
//}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



