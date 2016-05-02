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

#ifndef _EOTHEINERTIALS2_HID_H_
#define _EOTHEINERTIALS2_HID_H_


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EOtheServices_hid.h"


// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheInertials2.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

enum { inertials_maxRegulars = 2 }; // there cannot be more than 1 inertials object, and typically not more than 1 signalled variable.


struct EOtheInertials_hid
{
    eOservice_core_t                        service;
    eOservice_diagnostics_t                 diagnostics;
    eOservice_cantools_t                    sharedcan;

    uint8_t                                 numofmtbs;
    
    // now the old ones
    eOas_inertial_config_t                  sensorsconfig;
    EOvector*                               fifoofinertialdata;    
    
    eObool_t                                configured;
    
    uint16_t                                canmap_mtb_accel_int[2];
    uint16_t                                canmap_mtb_accel_ext[2];
    uint16_t                                canmap_mtb_gyros_ext[2];
    uint16_t                                canmap_mtb_active[2];
    
    uint16_t                                fromcan2id[2][16][3];   // 2 ports, 15 addresses (0->14), 3 kinds on mtb ... use mtb-eoas_inertial_accel_mtb_int

    // the inertial at the end
    eOas_inertial_t*                        inertial2;  
    EOarray*                                id32ofregulars;
    EOarray*                                arrayofsensors;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



