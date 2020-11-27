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
#ifndef _EOTHEENTITIES_H_
#define _EOTHEENTITIES_H_

#ifdef __cplusplus
extern "C" {
#endif

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheEntities.h
    @brief      This file gives direct access to the RAM used in protocol. That can be done directly by calling
                function eoprot_entity_ramof_get() but i also write this object because originally this service was
                inside another object.                
    @author     marco.accame@iit.it
    @date       01/09/2015
**/

/** @defgroup eo_EOtheEntities Object EOtheEntities
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EoMotionControl.h"
#include "EoAnalogSensors.h"
#include "EoSkin.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheEntities_hid EOtheEntities;


   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOtheEntities* eo_entities_Initialise(void);

extern EOtheEntities* eo_entities_GetHandle(void);

extern eOresult_t eo_entities_Reset(EOtheEntities *p);


extern eOresult_t eo_entities_SetNumOfJoints(EOtheEntities *p, uint8_t num);
extern uint8_t eo_entities_NumOfJoints(EOtheEntities *p);
extern eOmc_joint_t * eo_entities_GetJoint(EOtheEntities *p, eOprotIndex_t id);
extern eOmc_joint_config_t * eo_entities_GetJointConfig(EOtheEntities *p, eOprotIndex_t id);
extern eOmc_joint_status_t * eo_entities_GetJointStatus(EOtheEntities *p, eOprotIndex_t id);

extern eOresult_t eo_entities_SetNumOfMotors(EOtheEntities *p, uint8_t num);
extern uint8_t eo_entities_NumOfMotors(EOtheEntities *p);
extern eOmc_motor_t * eo_entities_GetMotor(EOtheEntities *p, eOprotIndex_t id);
extern eOmc_motor_status_t * eo_entities_GetMotorStatus(EOtheEntities *p, eOprotIndex_t id);

extern eOresult_t eo_entities_SetNumOfMaises(EOtheEntities *p, uint8_t num);
extern uint8_t eo_entities_NumOfMaises(EOtheEntities *p);
extern eOas_mais_t * eo_entities_GetMais(EOtheEntities *p, eOprotIndex_t id);
extern eOas_mais_config_t * eo_entities_GetMaisConfig(EOtheEntities *p, eOprotIndex_t id);
extern eOas_mais_status_t * eo_entities_GetMaisStatus(EOtheEntities *p, eOprotIndex_t id);

extern eOresult_t eo_entities_SetNumOfTemperatures(EOtheEntities *p, uint8_t num);
extern uint8_t eo_entities_NumOfTemperatures(EOtheEntities *p);
extern eOas_temperature_t * eo_entities_GetTemperature(EOtheEntities *p, eOprotIndex_t id);
extern eOas_temperature_config_t * eo_entities_GetTemperatureConfig(EOtheEntities *p, eOprotIndex_t id);
extern eOas_temperature_status_t * eo_entities_GetTemperatureStatus(EOtheEntities *p, eOprotIndex_t id);

extern eOresult_t eo_entities_SetNumOfInertials(EOtheEntities *p, uint8_t num);
extern uint8_t eo_entities_NumOfInertials(EOtheEntities *p);
extern eOas_inertial_t * eo_entities_GetInertial(EOtheEntities *p, eOprotIndex_t id);
extern eOas_inertial_config_t * eo_entities_GetInertialConfig(EOtheEntities *p, eOprotIndex_t id);
extern eOas_inertial_status_t * eo_entities_GetInertialStatus(EOtheEntities *p, eOprotIndex_t id);

extern eOresult_t eo_entities_SetNumOfInertials3(EOtheEntities *p, uint8_t num);
extern uint8_t eo_entities_NumOfInertials3(EOtheEntities *p);
extern eOas_inertial3_t * eo_entities_GetInertial3(EOtheEntities *p, eOprotIndex_t id);
extern eOas_inertial3_config_t * eo_entities_GetInertial3Config(EOtheEntities *p, eOprotIndex_t id);
extern eOas_inertial3_status_t * eo_entities_GetInertial3Status(EOtheEntities *p, eOprotIndex_t id);

extern eOresult_t eo_entities_SetNumOfStrains(EOtheEntities *p, uint8_t num);
extern uint8_t eo_entities_NumOfStrains(EOtheEntities *p);
extern eOas_strain_t * eo_entities_GetStrain(EOtheEntities *p, eOprotIndex_t id);
extern eOas_strain_config_t * eo_entities_GetStrainConfig(EOtheEntities *p, eOprotIndex_t id);
extern eOas_strain_status_t * eo_entities_GetStrainStatus(EOtheEntities *p, eOprotIndex_t id);

extern eOresult_t eo_entities_SetNumOfPSCs(EOtheEntities *p, uint8_t num);
extern uint8_t eo_entities_NumOfPSCs(EOtheEntities *p);
extern eOas_psc_t * eo_entities_GetPSC(EOtheEntities *p, eOprotIndex_t id);

extern eOresult_t eo_entities_SetNumOfPOSs(EOtheEntities *p, uint8_t num);
extern uint8_t eo_entities_NumOfPOSs(EOtheEntities *p);
extern eOas_pos_t * eo_entities_GetPOS(EOtheEntities *p, eOprotIndex_t id);

extern eOresult_t eo_entities_SetNumOfSkins(EOtheEntities *p, uint8_t num);
extern uint8_t eo_entities_NumOfSkins(EOtheEntities *p);
extern eOsk_skin_t * eo_entities_GetSkin(EOtheEntities *p, eOprotIndex_t id);
extern eOsk_status_t * eo_entities_GetSkinStatus(EOtheEntities *p, eOprotIndex_t id);




/** @}            
    end of group eo_EOtheEntities
 **/


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


