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

#ifndef _EOTHETEMPERATURES_HID_H_
#define _EOTHETEMPERATURES_HID_H_

#ifdef __cplusplus
extern "C" {
#endif


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EOtheServices_hid.h"

#include "EOtheMEMS.h"


// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheTemperatures.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

enum { temperature_maxRegulars = 1 }; // there cannot be more than 1 object and typically not more than 1 signalled variable.


enum { eo_temperatures_fifocapacity = 16 };


enum { eo_temperatures_maxsensorsONboard = 1 };
enum { eo_temperatures_posof_mtb4 = 0, eo_temperatures_posof_strain2 = 1, eo_temperatures_maxTypesOfBoard = 2 };

struct EOtheTemperatures_hid
{
    eOservice_core_t                        service;
    eOservice_diagnostics_t                 diagnostics;
    eOservice_cantools_t                    sharedcan;
    
    eObool_t                                configured;
    
    uint8_t                                 numofcanboards[eo_temperatures_maxTypesOfBoard];
    
    // now the old ones
    eOas_temperature_config_t               sensorsconfig;    
    uint16_t                                canmap_brd_active[2];
    
    EOvector*                               fifooftemperaturedata;    
    
    // the sensor at the end
    eOas_temperature_t*                     temperature;  
    EOarray*                                id32ofregulars;
    eOas_temperature_setof_boardinfos_t     setofboardinfos;
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



