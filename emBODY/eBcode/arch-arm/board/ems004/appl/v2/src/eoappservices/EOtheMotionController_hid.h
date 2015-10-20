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


struct EOtheMotionController_hid
{
    eObool_t                                initted;
    eObool_t                                active;
    eObool_t                                started;
    uint8_t                                 numofjomos;
    eOmn_serv_configuration_t               servconfig;
    
    // they are for mc (foc or mc4 boards)
    EOvector*                               canboardproperties;
    EOvector*                               canentitydescriptor;
    eOcandiscovery_target_t                 candiscoverytarget;
    eOmotcon_onendofoperation_fun_t         onverify;
    eObool_t                                activateafterverify;
    eOcandiscovery_onstop_t                 ondiscoverystop;
    
    // they are for foc-based mc only
    EOemsController*                        controller;
    EOtheEncoderReader*                     encoderreader;
    
    // they are for mc4-based mc only
    EOtheMAIS*                              mais;  
    eOmn_serv_configuration_t               servconfigmais;
    EOtheMC4boards*                         mc4boards;
    
    // this part if for error (diagnostics) reporting: if something is OK or NOT OK. we use a timer to repeat for some time the messages
    EOtimer*                                errorReportTimer;
    eOerrmanDescriptor_t                    errorDescriptor;
    eOerrmanErrorType_t                     errorType;
    uint8_t                                 errorCallbackCount;
    uint8_t                                 repetitionOKcase;
    eOreltime_t                             reportPeriod;  
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



