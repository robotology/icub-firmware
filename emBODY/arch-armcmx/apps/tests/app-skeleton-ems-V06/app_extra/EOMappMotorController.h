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
#ifndef _EOMAPPMOTORCONTROLLER_H_
#define _EOMAPPMOTORCONTROLLER_H_


// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOMappMotorController.h
    @brief      This header file implements public interface appMotorController Obj
    @author     valentina.gaggero@iit.it
    @date       01/02/2012
**/

/** @defgroup eom_appMotorController Object EOMappMotorController
    the task of this obj is to perfoms motor controller functions.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

//embobj
#include "Eocommon.h"
#include "EOmanagement.h"
#include "EOappEncodersReader.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOMappMotorController_hid EOMappMotorController;


typedef struct
{
    EOappEncReader              *encReader;
    EOappCanSP                  *appCanSP_ptr;
    eOcallbackData_t            sig2appDataTransmitter;
} EOMappMotorController_cfg_t;
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern EOMappMotorController* eom_appMotorController_New(EOMappMotorController_cfg_t *cfg)
    @brief      Creates a new object.
    @return     The pointer to the required object or NULL upon failure.
 **/
extern EOMappMotorController* eom_appMotorController_New(EOMappMotorController_cfg_t *cfg);

 
/** @fn         extern eOresult_t eom_appMotorController_Activate(EOMappMotorController *p)
    @brief      activete the obj. Now the obj can perform calculation. 
    @param      p               Pointer to the object.
    @return     eores_OK
 **/
extern eOresult_t eom_appMotorController_Activate(EOMappMotorController *p);


/** @fn         extern eOresult_t eom_appMotorController_Activate(EOMappMotorController *p)
    @brief      make obj not active. Now the obj can not perform calculation. 
    @param      p               Pointer to the object.
    @return     eores_OK
 **/
extern eOresult_t eom_appMotorController_Deactivate(EOMappMotorController *p);


/** @fn         extern eOresult_t eom_appMotorController_Satrt2Calculate(EOMappMotorController *p)
    @brief      this fn is provided by obj to be informed that now it can start to perform motor controller
                the obj starts to do it, only if it has all data ready. (this data are collected by
                EOMappDataCollector object.)
    @return     eores_NOK_nullpointer if p is NULL, eores_NOK_generic if the obj is in wrong state 
                (it has not been activate), else eores_OK
 **/
extern eOresult_t eom_appMotorController_Satrt2Calculate(EOMappMotorController *p);


/** @fn         extern eOresult_t eom_appMotorController_AllDataAreReady(EOMappMotorController *p)
    @brief      this fn is provided by obj to be informed that all necessary data are ready.
                (this data are collected by EOMappDataCollector object.)
                This function starts the obj to perform motor controller if it has received the command
                to start to calculate (by eom_appMotorController_Satrt2Calculate function) 
    @return     eores_NOK_nullpointer if p is NULL, eores_NOK_generic if the obj is in wrong state 
                (it has not been activate), else eores_OK
 **/
extern eOresult_t eom_appMotorController_AllDataAreReady(EOMappMotorController *p);


extern eOresult_t eom_appMotorController_SetRunMode(EOMappMotorController *p, eOmn_appl_runMode_t runmode);


// - doxy end ---------------------------------------------------------------------------------------------------------

/** @}            
    end of group eom_appMotorController  
 **/

 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


