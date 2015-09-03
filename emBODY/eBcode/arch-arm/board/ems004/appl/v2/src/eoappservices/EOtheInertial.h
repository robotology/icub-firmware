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
#ifndef _EOTHEINERTIAL_H_
#define _EOTHEINERTIAL_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheInertial.h
    @brief      this object implements what is required for managing the inertial sensor.                
    @author     marco.accame@iit.it
    @date       08/19/2015
**/

/** @defgroup eo_EOtheInertial Object EOtheInertial
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheInertial_hid EOtheInertial;



//typedef struct
//{
//    eOas_inertial_position_t    canmapofsupportedsensors[2][15];  // contains in pos i,j: eoas_inertial_pos_none if no mtb is at that address, else another value of eOas_inertial_position_t
//} eOinertial_cfg_t;


// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------

extern const eOas_inertial_serviceconfig_t eo_inertial_cfg_eb2;
extern const eOas_inertial_serviceconfig_t eo_inertial_cfg_eb4;
extern const eOas_inertial_serviceconfig_t eo_inertial_cfg_eb10;
extern const eOas_inertial_serviceconfig_t eo_inertial_cfg_eb11;

// - declaration of extern public functions ---------------------------------------------------------------------------



extern EOtheInertial* eo_inertial_Initialise(void);

extern eOresult_t eo_inertial_ServiceConfig(EOtheInertial *p, const eOas_inertial_serviceconfig_t* cfg);

extern EOtheInertial* eo_inertial_Initialise2(const eOas_inertial_serviceconfig_t* cfg);

extern EOtheInertial* eo_inertial_GetHandle(void);


extern eOresult_t eo_inertial_SensorsConfig(EOtheInertial *p, eOas_inertial_sensorsconfig_t* config);

extern eOresult_t eo_inertial_Start(EOtheInertial *p);

extern eOresult_t eo_inertial_Stop(EOtheInertial *p);

extern eOresult_t eo_inertial_Receive(EOtheInertial *p, eOas_inertial_type_t type, eOcanframe_t *frame, eOcanport_t port);

extern eOresult_t eo_inertial_RefreshStatusOfEntity(EOtheInertial *p);


/** @}            
    end of group eo_EOtheInertial
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


