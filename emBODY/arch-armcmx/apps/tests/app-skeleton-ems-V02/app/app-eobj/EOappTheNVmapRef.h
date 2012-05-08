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
#ifndef _EOAPPTHENVMAPREF_H_
#define _EOAPPTHENVMAPREF_H_


/** @file       EOappTheNVmapRef.h
    @brief      This file keeps ...
    @author     valentina.gaggero@iit.it
    @date       04/17/2012
**/

/** @defgroup eo_app_TheNVmapRef Singleton EOappTheNVmapRef
    ................ 
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoMotionControl.h"
#include "EOfifoByte.h"
#include "EOnvsCfg.h"
#include "EOnv.h"



// - public #define  --------------------------------------------------------------------------------------------------


// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    eOappTheNVmapRef_cfg_t
    @brief      eOappTheNVmapRef_cfg_t contains .....
 **/  
typedef struct
{
    EOfifoByte  *jointsList;  /**<  List of joints used by the application  */
    EOfifoByte  *motorsList;  /**<  List of motors used by the application  */
    EOfifoByte  *sensorsList; /**<  List of sensors used by the application */
    EOnvsCfg    *nvsCfg;      /**<  Network Variable configuration          */
    eOnvEP_t    mc_endpoint;  /**<  motion control endopoint managed by the application    */
    eOnvEP_t    as_endpoint;  /**<  analog sensor endopoint managed by the application    */
} eOappTheNVmapRef_cfg_t;


/** @typedef    typedef struct EOappTheNVmapRef_hid EOappTheNVmapRef
    @brief      ...... 
 **/  
typedef struct EOappTheNVmapRef_hid EOappTheNVmapRef;
    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

//default cfg does not exist.


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern EOappTheNVmapRef* eo_appTheNVmapRef_Initialise(eOappTheNVmapRef_cfg_t *cfg)

    @brief      Initialise the singleton EOappTheNVmapRef.
    @param      cfg         The configuration.
    @return     NULL is case of error, otherwise a handle to the singleton.
 
 **/
extern EOappTheNVmapRef* eo_appTheNVmapRef_Initialise(eOappTheNVmapRef_cfg_t *cfg);



extern EOappTheNVmapRef* eo_appTheNVmapRef_GetHandle(void);


/** @fn         extern eOresult_t eo_appTheNVmapRef_GetJointNVMemoryRef(EOappTheNVmapRef* p, eOmc_jointId_t jUiniqueId, uint8_t nvindex, void**memRef);
    @brief      given joint id and network variable index about joint return a poiter to ram where there is nv's value.
    @param      
    @return     
    @waring:   use eo_cfg_nvsEP_mc_any_con_bodypart_jointNVindex_t for nvindex
 **/
extern eOresult_t eo_appTheNVmapRef_GetJointNVMemoryRef(EOappTheNVmapRef* p, eOmc_jointId_t jUiniqueId, uint8_t nvindex, void**memRef);


extern eOresult_t eo_appTheNVmapRef_GetJointNVMemoryRef_test(EOappTheNVmapRef* p, eOmc_jointId_t jUiniqueId, uint8_t nvindex, void**memRef);


extern eOresult_t eo_appTheNVmapRef_GetMotorNVMemoryRef(EOappTheNVmapRef* p, eOmc_motorId_t mUiniqueId, uint8_t nvindex, void**memRef);

/** @}            
    end of group eo_app_TheNVmapRef  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

