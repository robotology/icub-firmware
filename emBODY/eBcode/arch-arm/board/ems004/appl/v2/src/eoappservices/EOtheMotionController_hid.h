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




// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"

#include "Controller.h"
#include "EOtheEncoderReader.h"

#include "EOtheMAIS.h"
#include "EOtheMC4boards.h"
#include "EOtheVirtualStrain.h"
#include "hal_motor.h"
#include "hal_adc.h"


#include "EOtheServices_hid.h"

// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheMotionController.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------


//typedef struct
//{
//    MController*                            thecontroller;
//    EOtheEncoderReader*                     theencoderreader;    
//} eOmotioncontroller_objs_foc_t;

//typedef struct
//{
//    eOmn_serv_configuration_t               servconfigmais;
//    EOtheMAIS*                              themais;  
//    EOtheMC4boards*                         themc4boards;    
//} eOmotioncontroller_objs_mc4_t;

//typedef struct
//{
//    MController*                            thecontroller;
//    EOtheEncoderReader*                     theencoderreader; 
//    int16_t                                 pwmvalue[hal_motors_number];    // at most i can manage 4 motors
//    hal_motor_t                             pwmport[hal_motors_number];
//    hal_dma_current_t                       currents[hal_motors_number];    // in ma
//    int16_t                                 voltage;                        // in mv   
//} eOmotioncontroller_objs_mc4plus_t;



typedef struct
{
    
    // for mc4can only
    EOtheMC4boards*                         themc4boards; 
    
    // for mc4can and mc4plus-mais
    eOmn_serv_configuration_t               servconfigmais;
    EOtheMAIS*                              themais;     
    
    // for everything apart mc4can
    EOconstarray*                           jomodescriptors; // points to service.servconfig.data.mc.xxx_based.arrayofjomodescriptors, where xxx is: foc, mc4plus, mc4plusmais
    MController*                            thecontroller;
    EOtheEncoderReader*                     theencoderreader;   
        
    
    // for mc4plus and mc4plus-mais only
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
         
//    // they are for foc-based mc only
//    eOmotioncontroller_objs_foc_t           mcfoc;    
//    // they are for mc4-based mc only
//    eOmotioncontroller_objs_mc4_t           mcmc4; 
//    // they are for mc4plus only
//    eOmotioncontroller_objs_mc4plus_t       mcmc4plus;
    
    EOarray*                                id32ofregulars;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



