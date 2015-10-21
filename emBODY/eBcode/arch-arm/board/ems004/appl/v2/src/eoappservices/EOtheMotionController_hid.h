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
#include "EOtheCANprotocol.h"
#include "EOtheCANdiscovery2.h"

#include "EOemsController.h"
#include "EOtheEncoderReader.h"

#include "EOtheMAIS.h"
#include "EOtheMC4boards.h"
#include "EOtheMotionDone.h"
#include "EOtheVirtualStrain.h"
#include "EOtimer.h"
#include "EOtheErrorManager.h"

// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheMotionController.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct
{
    eOmn_serv_configuration_t               servconfig; 
    eObool_t                                initted;
    eObool_t                                active;  
    eObool_t                                activateafterverify;
    eObool_t                                running;
    eOmotcon_onendofoperation_fun_t         onverify;
} eOmotioncontroller_objs_service_t;

typedef struct
{
    EOvector*                               boardproperties;
    EOvector*                               entitydescriptor;
    eOcandiscovery_target_t                 discoverytarget;
    eOcandiscovery_onstop_t                 ondiscoverystop;   
    eOcanprot_command_t                     command;
} eOmotioncontroller_objs_sharedcan_t;

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
    // anything else? add it in here. or maybe we use the one from foc ... 
} eOmotioncontroller_objs_mc4plus_t;

typedef struct
{
    EOtimer*                                reportTimer;
    eOreltime_t                             reportPeriod;  
    eOerrmanDescriptor_t                    errorDescriptor;
    eOerrmanErrorType_t                     errorType;
    uint8_t                                 errorCallbackCount;
    uint8_t                                 repetitionOKcase;
} eOmotioncontroller_objs_diagnostics_t;


struct EOtheMotionController_hid
{
    eOmotioncontroller_objs_service_t       service;
    eOmotioncontroller_objs_diagnostics_t   diagnostics;    
    eOmotioncontroller_objs_sharedcan_t     sharedcan;   
    
    uint8_t                                 numofjomos;
         
    // they are for foc-based mc only
    eOmotioncontroller_objs_foc_t           mcfoc;    
    // they are for mc4-based mc only
    eOmotioncontroller_objs_mc4_t           mcmc4;    
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



