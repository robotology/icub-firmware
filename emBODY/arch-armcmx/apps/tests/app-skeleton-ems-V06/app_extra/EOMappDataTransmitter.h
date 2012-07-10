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
#ifndef _EOMappDataTransmitter_H_
#define _EOMappDataTransmitter_H_


// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOMappDataTransmitter.h
    @brief      This header file implements public interface to aOMappDataTransmitter object.
    @author     valentina.gaggero@iit.it
    @date       01/02/2012
**/

/** @defgroup eom_appDataTransmitter Object EOMappDataTransmitter
    EOMappDataCollector  is an application object belong to application skeleton Ems.
    Its task is send data to icub can boards and towards pc104. in order to perform this, it employs
    object services'providers: canServicesProvider and Transceiver.
    It's important to note that these object are shared with others application's actors, but 
    actors don't use mutex to acess to them in safe mode, bucause reace condition are avoid by 
    "Round robin like mecchanism" (each actors has a quantum time).    
   
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

//embobj
#include "eOcommon.h"
#include "EOmanagement.h"

#include "EOappCanServicesProvider.h"
#include "EOethBaseModule.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOMappDataTransmitter_hid EOMappDataTransmitter;


typedef struct
{    
    EOappCanSP                  *appCanSP_ptr;
    EOethBaseModule             *eth_mod;
} EOMappDataTransmitter_cfg_t;
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern EOMappDataTransmitter* eom_appDataTransmitter_New(EOMappDataTransmitter_cfg_t *cfg)
    @brief      create a new  EOMappDataTransmitter object
    @return     The pointer to the required object or NULL upon failure.
 **/
extern EOMappDataTransmitter* eom_appDataTransmitter_New(EOMappDataTransmitter_cfg_t *cfg);

 
/** @fn         extern eOresult_t eom_appDataTransmitter_Activate(EOMappDataTransmitter *p)
    @brief      this function make the obj ready to transmit.
    @param      p               Pointer to the object.
    @return     eores_NOK_nullpointyer if p is NULL else eores_OK.
 **/
extern eOresult_t eom_appDataTransmitter_Activate(EOMappDataTransmitter *p);


/** @fn         extern eOresult_t eom_appDataTransmitter_Deactivate(EOMappDataTransmitter *p)
    @brief      this function make the obj not ready to transmit.
    @param      p               Pointer to the object.
    @return     eores_NOK_nullpointyer if p is NULL else eores_OK.
 **/
extern eOresult_t eom_appDataTransmitter_Deactivate(EOMappDataTransmitter *p);


/** @fn         extern eOresult_t eom_appDataTransmitter_SendData(EOMappDataTransmitter *p)
    @brief      the obj start to transmit data.
    @param      p               Pointer to the object.
    @return     eores_NOK_nullpointyer if p is NULL else eores_OK.
    @Warning    not implemented     
 **/
extern eOresult_t eom_appDataTransmitter_SendData(EOMappDataTransmitter *p); 


extern eOresult_t eom_appDataTransmitter_SetRunMode(EOMappDataTransmitter *p, eOmn_appl_runMode_t runmode);


extern eOresult_t eom_appDataTransmitter_SetMotionmonitorMode(EOMappDataTransmitter *p, eOmc_motionmonitormode_t monitormode);

// - doxy end ---------------------------------------------------------------------------------------------------------

/** @}            
    end of group eom_appDataTransmitter  
 **/

 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


