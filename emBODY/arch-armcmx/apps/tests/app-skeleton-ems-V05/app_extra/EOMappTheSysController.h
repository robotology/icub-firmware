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
#ifndef _EOMAPPTHESYSCONTROLLER_H_
#define _EOMAPPTHESYSCONTROLLER_H_

/** @file       EOMappTheSysController.h
    @brief      This header file implements public interface to EOMappTheSysController singleton obj
    @author     valentina.gaggero@iit.it
    @date       01/02/2012
**/

/** @defgroup eom_appTheSysController Object EOMappTheSysController
    The EOMappTheSysController is a singleton object; it is an "actor" of application ems Skeleton.
    It receives pc104's commands about application status; currently possible status are: 
    configuration state and realTime-running state.
    When it receives a command it pilots others application's "actor" and 
    forces them to move in the state in according with received command.
    Note: here the "system" word means the entire application.
    when the "system" is in realTime-running state, eom_appTheSysController manage
    quantum time for each actor: dataCollector, motor controller, transmitter.
     
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------
// embobj
#include "EoCommon.h"
#include "EOconstvector.h"
//nv
#include "EOnv.h"

//appl
#include "EOethBaseModule.h"




// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 
typedef struct EOMappTheSysController_hid   EOMappTheSysController;


typedef struct
{

    EOethBaseModule_cfg_t    *ethmod_cfg_ptr;
    uint16_t                 connectedEncodersMask_cfg; 
    const EOconstvector*     vectorof_endpoint_cfg;
    eOuint16_fp_uint16_t     hashfunction_ep2index;
    eOnvEP_t                 mc_endpoint;  /**<  motion control endopoint managed by the application    */
    eOnvEP_t                 as_endpoint;  /**<  analog sensor endopoint managed by the application    */
    eOnvEP_t                 sk_endpoint;  /**<  analog sensor endopoint managed by the application    */
} EOMappTheSysController_cfg_t;
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         EOMappTheSysController* eom_sysController_New(EOMappTheSysController_cfg_t *cfg)
    @brief      initialise the singleton EOMappTheSysController object. 
    @return     The pointer to the singleton object.
 **/
extern EOMappTheSysController* eom_appTheSysController_Initialise(EOMappTheSysController_cfg_t *cfg);



/** @fn         extern eOresult_t eom_sysController_Start(EOMappTheSysController *p)
    @brief      makes the EOMappTheSysController ready to receive commnads.
    @return     eores_OK.
 **/
extern eOresult_t eom_appTheSysController_Start(EOMappTheSysController *p);



/** @fn         extern eOresult_t eom_sysController_Start(EOMappTheSysController *p)
    @brief      makes the EOMappTheSysController ready to receive commnads.
    @return     The pointer to the singleton object.
 **/
extern EOMappTheSysController* eom_appTheSysController_GetHandle(void);


/** @fn         extern void* eom_appTheSysController_services_can_getHandle(void)
    @brief      This function returns hadle to object that provides services about can.
    @return     a poiter.
 **/
extern void* eom_appTheSysController_Services_Can_GetHandle(EOMappTheSysController *p);


extern eOresult_t eom_appTheSysController_Go2ConfigState(EOMappTheSysController *p);
extern eOresult_t eom_appTheSysController_Go2RunState(EOMappTheSysController *p);
extern eOresult_t eom_appTheSysController_ExitFromRunState(EOMappTheSysController *p);
extern eOresult_t eom_appTheSysController_Go2ErrorState(EOMappTheSysController *p);


/** @}            
    end of group eom_appTheSysController 
 **/


#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


