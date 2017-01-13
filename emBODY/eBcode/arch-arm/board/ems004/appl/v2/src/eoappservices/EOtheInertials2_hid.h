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

#ifdef __cplusplus
extern "C" {
#endif


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EOtheServices_hid.h"

#include "EOtheMEMS.h"


// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheInertials2.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

enum { inertials_maxRegulars = 2 }; // there cannot be more than 1 inertials object, and typically not more than 1 signalled variable.

enum { mems_gyro = 0, mems_accel = 1 , mems_numberofthem = 2 };

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
    uint8_t                                 ethmap_mems_active;
    
    uint16_t                                fromcan2id[2][16][3];   // 2 ports, 15 addresses (0->14), 3 kinds on mtb ... use mtb-eoas_inertial_accel_mtb_int
    uint16_t                                frommems2id[mems_numberofthem];
    uint8_t                                 memsparam[mems_numberofthem];    
    eOmems_sensor_cfg_t                     memsconfig[mems_numberofthem];
    // the inertial at the end
    eOas_inertial_t*                        inertial2;  
    EOarray*                                id32ofregulars;
    EOarray*                                arrayofsensors;
    uint16_t                                not_heardof_target[2];
    uint16_t                                not_heardof_status[2];
    uint32_t                                not_heardof_counter;
    eObool_t                                transmissionisactive;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



