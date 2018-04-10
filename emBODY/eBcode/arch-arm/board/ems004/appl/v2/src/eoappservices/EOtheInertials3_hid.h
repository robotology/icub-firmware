/*
 * Copyright (C) 2018 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _EOTHEINERTIALS3_HID_H_
#define _EOTHEINERTIALS3_HID_H_

#ifdef __cplusplus
extern "C" {
#endif


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EOtheServices_hid.h"

#include "EOtheMEMS.h"


// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheInertials3.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

enum { inertials3_maxRegulars = 1 }; // there cannot be more than 1 inertials object, and typically not more than 1 signalled variable.

enum { inertials3_mems_gyro = 0, inertials3_mems_accel = 1 , inertials3_mems_numberofthem = 2 };

enum { eo_inertials3_fifocapacity = 32 };


enum { eo_inertials3_maxsensorsONboard = 4 };
enum { eo_inertials3_posof_mtb = 0, eo_inertials3_posof_mtb4 = 1, eo_inertials3_posof_strain2 = 2, eo_inertials3_maxTypesOfBoard = 3 };

struct EOtheInertials_hid
{
    eOservice_core_t                        service;
    eOservice_diagnostics_t                 diagnostics;
    eOservice_cantools_t                    sharedcan;
    
    eObool_t                                configured;
    
    uint8_t                                 numofcanboards[eo_inertials3_maxTypesOfBoard];
    
    // now the old ones
    eOas_inertial3_config_t                 sensorsconfig;    
    uint16_t                                canmap_brd_active[2];
    uint8_t                                 ethmap_mems_active;
    uint16_t                                frommems2id[inertials3_mems_numberofthem];
    uint8_t                                 memsparam[inertials3_mems_numberofthem];    
    eOmems_sensor_cfg_t                     memsconfig[inertials3_mems_numberofthem];
    
    EOvector*                               fifoofinertial3data;    
    
//    uint8_t                                fromcan2id[2][16][eo_inertials3_maxsensorsONboard];   // 2 ports, 15 addresses (0->14), several kinds on same board (at most 4)
    
    
#if 0    
    uint16_t                                canmap_mtb_accel_int[2];
    uint16_t                                canmap_mtb_accel_ext[2];
    uint16_t                                canmap_mtb_gyros_ext[2];
    
    
    

    
#endif  
  
    // the inertial at the end
    eOas_inertial3_t*                       inertial3;  
    EOarray*                                id32ofregulars;
    eOas_inertial3_setof_boardinfos_t       setofboardinfos;
    EOarray*                                arrayofsensordescriptors;
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



