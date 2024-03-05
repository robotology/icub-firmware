
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EOTHEMCAMC_HID_H_
#define _EOTHEMCAMC_HID_H_

//#ifdef __cplusplus
//extern "C" {
//#endif


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"

#include "Controller.h"


#if defined(USE_EMBOT_theServices)
#include "embot_app_eth_theServices.h"
#include "embot_app_eth_Service_legacy.h"
#else 
#include "EOtheServices_hid.h"
#endif

// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheMotionController.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------


typedef struct
{
    EOconstarray*                           jomodescriptors; // points to the jomodescriptor inside EOtheMotionController_hid::service::servconfig etc. 
    MController*                            thecontroller;
} eOmotioncontroller_objs_t;


enum { motioncontrol_maxRegulars = 32 }; // there can be at most 12 jomos. we typically use status of joint and status of motor, thus 24 ... however, i use 32 which is the max value of regulars in a board



struct EOtheMotionController_hid
{
    eOservice_core_t                        service;
    eOservice_diagnostics_t                 diagnostics;
    eOservice_cantools_t                    sharedcan;  
    
    uint8_t                                 numofjomos;    
    eOmotioncontroller_objs_t               ctrlobjs;             
    EOarray*                                id32ofregulars;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


//#ifdef __cplusplus
//}       // closing brace for extern "C"
//#endif 
 
#endif  // include-guard 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



