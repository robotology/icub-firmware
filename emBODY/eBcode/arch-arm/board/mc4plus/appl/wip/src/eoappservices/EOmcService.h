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
#ifndef _EOMCSERVICE_H_
#define _EOMCSERVICE_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOmcService.h
    @brief      This file ...                
    @author     marco.accame@iit.it
    @date       01/09/2015
**/

/** @defgroup eo_EOmcService Object EOmcService
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "eOcommon.h"
#include "EoProtocol.h"
#include "EoMotionControl.h"
#include "EoAnalogSensors.h"
#include "EoSkin.h"

#include "EoMCConfigurations.h"
#include "EOemsController.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOmcService_hid EOmcService;

   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOmcService* eo_mcserv_Initialise(eOmcconfig_cfg_t *cfg);

extern EOmcService* eo_mcserv_GetHandle(void);



extern uint8_t eo_mcserv_NumberOfJoints(EOmcService *p);

extern eOmc_joint_t* eo_mcserv_GetJoint(EOmcService *p, uint8_t index);

extern uint8_t eo_mcserv_NumberOfMotors(EOmcService *p);

extern eOmc_motor_t* eo_mcserv_GetMotor(EOmcService *p, uint8_t index);

extern eOmcconfig_cfg_t* eo_mcserv_GetMotionControlConfig (EOmcService *p);



// used to verify if all required resources are available: can boards (mc4 or 2foc or mais discovery, etc.)
// it may start a discovery process (e.g., the serach for all mc4-mais or 2foc boards).
extern eOresult_t eo_mcserv_CheckResources(EOmcService *p);

// tells if all resources are ready.
// use it when robotinterface attempt to start the run mode.
extern eObool_t eo_mcserv_AreResourcesReady(EOmcService *p, uint32_t *readyresourcesdescriptor);


// call it in on-entry of run mode
extern eOresult_t eo_mcserv_Start(EOmcService *p);
// call it in on-exit or run mode
extern eOresult_t eo_mcserv_Stop(EOmcService *p);

// call it inside the protocol callbacks ... to be refined along the way.
extern eOresult_t eo_mcserv_Set(EOmcService *p, uint8_t jomo, uint32_t whatisrequired);

extern eOresult_t eo_mcserv_EnableMotor(EOmcService *p, uint8_t joint_index);

extern eOresult_t eo_mcserv_EnableFaultDetection(EOmcService *p);

extern eObool_t eo_mcserv_AreMotorsExtFaulted(EOmcService *p);

extern eOresult_t eo_mcserv_SetMotorFaultMask(EOmcService *p, uint8_t motor, uint8_t* fault_mask);

extern uint16_t eo_mcserv_GetMotorFaultMask(EOmcService *p, uint8_t motor);

extern int16_t eo_mcserv_GetMotorCurrent(EOmcService *p, uint8_t joint);

extern uint32_t eo_mcserv_GetMotorPositionRaw(EOmcService *p, uint8_t joint);

// call it inside the DO step of run mode
// it performs differently according to its eOmcserv_type_t type.
// basically: it reads encoders, compute target pwm, send pwm to actuators (pwm peripherals or put inside can tx buffer 2foc boards), update jomo status
extern eOresult_t eo_mcserv_Actuate(EOmcService *p);




/** @}            
    end of group eo_EOmcService
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


