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
#ifndef _EOTHEVIRTUALSTRAIN_H_
#define _EOTHEVIRTUALSTRAIN_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheVirtualStrain.h
    @brief      this object implements what is required for motion done.                
    @author     marco.accame@iit.it
    @date       05/28/2015
**/

/** @defgroup eo_EOtheVirtualStrain Object EOtheVirtualStrain
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheVirtualStrain_hid EOtheVirtualStrain;


enum { eo_virtualstrain_canaddress = 12 };
   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOtheVirtualStrain* eo_virtualstrain_Initialise(void);

extern EOtheVirtualStrain* eo_virtualstrain_GetHandle(void);

extern eOresult_t eo_virtualstrain_SetTorque(EOtheVirtualStrain *p, eOprotIndex_t jindex, icubCanProto_torque_t value);

// so far only the tick is implemented. it is to be called in the control loop in the do phase.
extern eOresult_t eo_virtualstrain_Tick(EOtheVirtualStrain *p);




/** @}            
    end of group eo_EOtheVirtualStrain
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


