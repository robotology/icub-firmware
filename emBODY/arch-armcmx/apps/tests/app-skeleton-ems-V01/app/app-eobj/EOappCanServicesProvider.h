/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
#ifndef _EOAPPCANSERVICESPROVIDER_H_
#define _EOAPPCANSERVICESPROVIDER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOappCanServicesProvider.h
    @brief      This file provides CAN services to ems application.
                These services pertain to communication with icub-can-board by CAN bus.
                Therefore, these object is tied to icub-can-protocol and icub-world in general
    @author     valentina.gaggero@iit.it
    @date       02/14/2012
**/

/** @defgroup eo_app_canServicesProvider Object eOappCanSP
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------
#include "EoCommon.h"
#include "EoMotionControl.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 
typedef struct EOappCanSP_hid   EOappCanSP;

typedef struct
{
    uint32_t dummy;
} eOappCanSP_cfg_t;
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
extern EOappCanSP* eo_appCanSP_New(eOappCanSP_cfg_t *cfg);


extern eOresult_t eo_appCanSP_ConfigMotor(EOappCanSP *p, eOmc_motorUniqueId_t mUniqueId, eOmc_motor_config_t *cfg);
extern eOresult_t eo_appCanSP_ConfigJoint(EOappCanSP *p, eOmc_jointUniqueId_t jUniqueId, eOmc_joint_config_t *cfg);

//extern eOresult_t eo_appCanSP_ConfigJoint_4IndipendentBoard(EOappCanSP *p, eOmc_jointUniqueId_t jUniqueId, eOmc_joint_config_t *cfg);
//extern eOresult_t eo_appCanSP_ConfigJoint_4SlaveBoard(EOappCanSP *p, eOmc_jointUniqueId_t jUniqueId, eOmc_joint_config_t *cfg);
extern eOresult_t eo_appCanSP_SendSetPoint(EOappCanSP *p, eOmc_jointUniqueId_t jUniqueId, eOmc_setpoint_t *setPoint);

extern eOresult_t eo_appCanSP_read(EOappCanSP *p); //test porpouse

/** @}            
    end of group eo_app_canServicesProvider
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


