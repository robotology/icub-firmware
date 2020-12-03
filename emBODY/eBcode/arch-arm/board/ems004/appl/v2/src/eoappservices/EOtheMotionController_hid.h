/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EOTHEMOTIONCONTROLLER_HID_H_
#define _EOTHEMOTIONCONTROLLER_HID_H_

#ifdef __cplusplus
extern "C" {
#endif


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"

#include "Controller.h"
#include "EOtheEncoderReader.h"

#include "EOtheMAIS.h"
#include "EOthePSC.h"
#include "EOtheMC4boards.h"
#include "EOtheVirtualStrain.h"
#include "hal_motor.h"
#include "hal_adc.h"


#include "EOtheServices_hid.h"

// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheMotionController.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------



typedef struct
{
    
    // for mc4can only
    EOtheMC4boards*                         themc4boards; 
    
    // for mc4can and mc4plus-mais
    eOmn_serv_configuration_t               servconfigmais;
    EOtheMAIS*                              themais;     
    
    // for mc2plus-psc
    eOmn_serv_configuration_t               servconfigpsc;
    EOthePSC*                               thepsc;     
    
    // for mc4plus-faps & mc4plus-pmc
    eOmn_serv_configuration_t               servconfigpos;
    EOthePOS*                               thepos;  
    
    // for everything apart mc4can
    EOconstarray*                           jomodescriptors; // points to service.servconfig.data.mc.xxx_based.arrayofjomodescriptors, where xxx is: foc, mc4plus, mc4plusmais
    MController*                            thecontroller;
    EOtheEncoderReader*                     theencoderreader;   
        
    
    // for mc4plus, mc2plus, mc4plus-mais, mc2pluspsc, mc4plus-faps
    int16_t                                 pwmvalue[hal_motors_number];    // at most i can manage 4 motors
    hal_motor_t                             pwmport[hal_motors_number];
    hal_dma_current_t                       currents[hal_motors_number];    // in ma
    int16_t                                 voltage;                        // in mv       
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


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



