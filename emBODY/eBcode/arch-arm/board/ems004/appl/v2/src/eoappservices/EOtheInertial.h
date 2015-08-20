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


   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOtheInertial* eo_inertial_Initialise(void);

extern EOtheInertial* eo_inertial_GetHandle(void);


extern eOresult_t eo_inertial_Config(EOtheInertial *p, eOas_inertial_config_t* inertialcfg);

extern eOresult_t eo_inertial_Start(EOtheInertial *p);

extern eOresult_t eo_inertial_Stop(EOtheInertial *p);





/** @}            
    end of group eo_EOtheInertial
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


