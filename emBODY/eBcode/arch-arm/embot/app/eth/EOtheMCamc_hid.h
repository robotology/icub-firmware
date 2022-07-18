
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

//#include "Controller.h"
//#include "EOtheEncoderReader.h"

//#include "EOtheMAIS.h"
//#include "EOthePSC.h"
//#include "EOtheMC4boards.h"
//#include "EOtheVirtualStrain.h"
//#include "hal_motor.h"
//#include "hal_adc.h"



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
    
//    // for mc4can only
//    EOtheMC4boards*                         themc4boards; 
//    
//    // for mc4can and mc4plus-mais
//    eOmn_serv_configuration_t               servconfigmais;
//    EOtheMAIS*                              themais;     
    
//    // for mc2plus-psc
//    eOmn_serv_configuration_t               servconfigpsc;
//    EOthePSC*                               thepsc;     
    
//    // for mc4plus-faps & mc4plus-pmc
//    eOmn_serv_configuration_t               servconfigpos;
//    EOthePOS*                               thepos;  
    
    // for everything apart mc4can
    EOconstarray*                           jomodescriptors; // points to the jomodescriptor inside EOtheMotionController_hid::service::servconfig etc. 
//    MController*                            thecontroller;
//    EOtheEncoderReader*                     theencoderreader;   
        
    
//    // for mc4plus, mc2plus, mc4plus-mais, mc2pluspsc, mc4plus-faps
//    int16_t                                 pwmvalue[hal_motors_number];    // at most i can manage 4 motors
//    hal_motor_t                             pwmport[hal_motors_number];
//    hal_dma_current_t                       currents[hal_motors_number];    // in ma
//    int16_t                                 voltage;                        // in mv       
} eOmotioncontroller_objs_t;


enum { motioncontrol_maxRegulars = 32 }; // there can be at most 12 jomos. we typically use status of joint and status of motor, thus 24 ... however, i use 32 which is the max value of regulars in a board


//typedef struct
//{
//    EOtimer *tmr;
//    eOmc_motor_config_t *mc;
//    uint8_t num;    
//} motorDelayer_t;


struct EOtheMotionController_hid
{
    eOservice_core_t                        service;
    eOservice_diagnostics_t                 diagnostics;
    eOservice_cantools_t                    sharedcan;  
    
    uint8_t                                 numofjomos;    
    eOmotioncontroller_objs_t               ctrlobjs;             
    EOarray*                                id32ofregulars;
    
//    motorDelayer_t                          motor_delayer[eo_motcon_standardJOMOs];
//    eOflags08_t                             motor_delayer_flags;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


//#ifdef __cplusplus
//}       // closing brace for extern "C"
//#endif 
 
#endif  // include-guard 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



