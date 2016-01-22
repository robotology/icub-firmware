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

#include "EOemsController.h"
#include "EOtheEncoderReader.h"

#include "EOtheMAIS.h"
#include "EOtheMC4boards.h"
#include "EOtheVirtualStrain.h"


#include "EOtheServices_hid.h"

// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheMotionController.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------


typedef struct
{
    EOemsController*                        thecontroller;
    EOtheEncoderReader*                     theencoderreader;    
} eOmotioncontroller_objs_foc_t;

typedef struct
{
    eOmn_serv_configuration_t               servconfigmais;
    EOtheMAIS*                              themais;  
    EOtheMC4boards*                         themc4boards;    
} eOmotioncontroller_objs_mc4_t;

typedef struct
{
    EOemsController*                        thecontroller;
    EOtheEncoderReader*                     theencoderreader; 
    int16_t                                 pwmvalue[eo_motcon_standardJOMOs]; // at most i can manage 4 joints, hence 4 pwm values
    uint8_t                                 pwmport[eo_motcon_standardJOMOs];
    // anything else? add it in here. or maybe we use the one from foc ... 
} eOmotioncontroller_objs_mc4plus_t;



struct EOtheMotionController_hid
{
    eOservice_core_t                        service;
    eOservice_diagnostics_t                 diagnostics;
    eOservice_cantools_t                    sharedcan;  
    
    uint8_t                                 numofjomos;
         
    // they are for foc-based mc only
    eOmotioncontroller_objs_foc_t           mcfoc;    
    // they are for mc4-based mc only
    eOmotioncontroller_objs_mc4_t           mcmc4; 
    // they are for mc4plus only
    eOmotioncontroller_objs_mc4plus_t       mcmc4plus;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



