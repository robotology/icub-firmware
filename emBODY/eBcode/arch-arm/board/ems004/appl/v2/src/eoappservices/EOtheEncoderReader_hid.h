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




// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

#include "EOappEncodersReader.h"

#include "EOtimer.h"
#include "EOtheErrorManager.h"


#include "EOtheServices_hid.h"


// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheEncoderReader.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------


struct EOtheEncoderReader_hid
{
    eOservice_core_t                        service;
    eOservice_diagnostics_t                 diagnostics;
    
    eOmn_serv_arrayof_4jomodescriptors_t    arrayofjomodes;

    EOtimer*                                waitreadtimer;
    uint8_t                                 numofjomos;
    uint8_t                                 numofencoders;
    eOencoderreader_errortype_t             errors[eOappEncReader_jomos_maxnumberof];
    uint8_t                                 failuremask;
    EOappEncReader*                         reader;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



