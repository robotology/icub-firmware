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

#ifndef _EOTHEENCODERREADER_HID_H_
#define _EOTHEENCODERREADER_HID_H_

#ifdef __cplusplus
extern "C" {
#endif


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

#include "EOappEncodersReader.h"

#include "EOtimer.h"
#include "EOtheErrorManager.h"


#include "EOtheServices_hid.h"


// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheEncoderReader.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct                          
{   
    eOarray_head_t                      head;
    eOmc_jomo_descriptor_t              data[eo_encoderreader_max_jomos];
} eOencoderreader_jomodescriptors_t;   

struct EOtheEncoderReader_hid
{
    eOservice_core_t                        service;
    eOservice_diagnostics_t                 diagnostics;
    
    eOencoderreader_jomodescriptors_t       arrayofjomodes; // contains items of type eOmc_jomo_descriptor_t
    EOtimer*                                waitreadtimer;
    uint8_t                                 numofjomos;
    uint8_t                                 numofencoders;
    uint8_t                                 failuremask2[2];
    eOencoderreader_errortype_t             errors2[2][eOappEncReader_jomos_maxnumberof];
    EOappEncReader*                         reader;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



