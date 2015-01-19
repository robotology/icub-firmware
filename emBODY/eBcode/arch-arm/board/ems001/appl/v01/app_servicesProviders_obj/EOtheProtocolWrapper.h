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
#ifndef _EOTHEPROTOCOLWRAPPER_H_
#define _EOTHEPROTOCOLWRAPPER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheProtocolWrapper.h
    @brief      This file gives direct access to the RAM used in protocol. That can be done directly by calling
                function eoprot_entity_ramof_get() but i also write this object because originally this service was
                inside another object.                
    @author     marco.accame@iit.it
    @date       01/09/2015
**/

/** @defgroup eo_EOtheProtocolWrapper Object EOtheProtocolWrapper
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "eOcommon.h"
#include "EoProtocol.h"
#include "EoMotionControl.h"
#include "EoAnalogSensors.h"
#include "EoSkin.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheProtocolWrapper_hid EOtheProtocolWrapper;


   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOtheProtocolWrapper* eo_protocolwrapper_Initialise(void);

extern EOtheProtocolWrapper* eo_protocolwrapper_GetHandle(void);


// null if does not exists. the real pointer (use it with care!) if everything is ok

extern eOmc_joint_config_t * eo_protocolwrapper_GetJointConfig(EOtheProtocolWrapper *p, eOmc_jointId_t id);
extern eOmc_joint_status_t * eo_protocolwrapper_GetJointStatus(EOtheProtocolWrapper *p, eOmc_jointId_t id);

extern eOmc_motor_status_t * eo_protocolwrapper_GetMotorStatus(EOtheProtocolWrapper *p, eOmc_motorId_t id);

extern eOas_mais_config_t * eo_protocolwrapper_GetMaisConfig(EOtheProtocolWrapper *p, eOas_maisId_t id);
extern eOas_mais_status_t * eo_protocolwrapper_GetMaisStatus(EOtheProtocolWrapper *p, eOas_maisId_t id);


extern eOas_strain_config_t * eo_protocolwrapper_GetStrainConfig(EOtheProtocolWrapper *p, eOas_strainId_t id);
extern eOas_strain_status_t * eo_protocolwrapper_GetStrainStatus(EOtheProtocolWrapper *p, eOas_strainId_t id);

extern EOarray_of_10canframes * eo_protocolwrapper_GetSkinStatusArray(EOtheProtocolWrapper *p, eOsk_skinId_t id);






/** @}            
    end of group eo_EOtheProtocolWrapper
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


