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
#ifndef _EOAPPTHEMCNVMAP_H_
#define _EOAPPTHEMCNVMAP_H_


/** @file       EOappTheMCNVmap.h
    @brief      This file keeps ...
    @author     valentina.gaggero@iit.it
    @date       04/17/2012
**/

/** @defgroup eo_app_theMCNVmap Singleton EOappTheMCNVmap
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


/** @typedef    eOappTheMCNVmap_cfg_t
    @brief      eOappTheMCNVmap_cfg_t contains .....
 **/  
typedef struct
{
    EOfifoByte  *jointsList;  /**<  List of joints used by the application  */
    EOfifoByte  *motorsList;  /**<  List of motors used by the application  */
    EOfifoByte  *sensorsList; /**<  List of sensors used by the application */
    EOnvsCfg    *nvsCfg;      /**<  Network Variable configuration          */
    eOnvEP_t    mc_endpoint;  /**<  motion control endopoint managed by the application    */
    eOnvEP_t    as_endpoint;  /**<  analog sensor endopoint managed by the application    */
} eOappTheMCNVmap_cfg_t;


/** @typedef    typedef struct EOappTheMCNVmap_hid EOappTheMCNVmap
    @brief      ...... 
 **/  
typedef struct EOappTheMCNVmap_hid EOappTheMCNVmap;

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

//default cfg does not exist.


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern EOappTheMCNVmap* eo_appTheMCNVmap_Initialise(eOappTheMCNVmap_cfg_t *cfg)

    @brief      Initialise the singleton EOappTheMCNVmap.
    @param      cfg         The configuration.
    @return     NULL is case of error, otherwise a handle to the singleton.
 
 **/
extern EOappTheMCNVmap* eo_appTheMCNVmap_Initialise(eOappTheMCNVmap_cfg_t *cfg);



extern EOappTheMCNVmap* eo_appTheMCNVmap_GetHandle(void);


/** @fn         extern eOresult_t eo_appTheMCNVmap_GetJointNVMemoryRef(EOappTheMCNVmap* p, eOmc_jointId_t jUiniqueId, uint8_t nvindex, void**memRef);
    @brief      given joint id and network variable index about joint return a poiter to ram where there is nv's value.
    @param      
    @return     
    @waring:   use eo_cfg_nvsEP_mc_any_con_bodypart_jointNVindex_t for nvindex
 **/
extern eOresult_t eo_appTheMCNVmap_GetJointNVMemoryRef(EOappTheMCNVmap* p, eOmc_jointId_t jUiniqueId, uint8_t nvindex, void**memRef);


extern eOresult_t eo_appTheMCNVmap_GetJointNVMemoryRef_test(EOappTheMCNVmap* p, eOmc_jointId_t jUiniqueId, uint8_t nvindex, void**memRef);


extern eOresult_t eo_appTheMCNVmap_GetMotorNVMemoryRef(EOappTheMCNVmap* p, eOmc_motorId_t mUiniqueId, uint8_t nvindex, void**memRef);

/** @}            
    end of group eo_app_theMCNVmap  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

