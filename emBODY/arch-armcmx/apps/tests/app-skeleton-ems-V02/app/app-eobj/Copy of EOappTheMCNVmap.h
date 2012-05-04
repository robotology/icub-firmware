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
    eOnvEP_t    endpoint;     /**<  Endopoint managed by the application    */
} eOappTheMCNVmap_cfg_t;


/** @typedef    typedef struct EOappTheMCNVmap_hid EOappTheMCNVmap
    @brief      ...... 
 **/  
typedef struct EOappTheMCNVmap_hid EOappTheMCNVmap;

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

//default cfg does not exist.


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern EONtheSystem * eo_appTheMCNV_Initialise(eOappTheMCNVmap_cfg_t *cfg)

    @brief      TODOInitialise the singleton EONtheSystem and all the other entities/objects that are required by the system:
                the memory pool, the error manager. The timer manager is started separately.
    @param      syscfg          The configuration.
    @return     A not NULL handle to the singleton.  In case of errors it is called the EOtheErrorManager
 
 **/
extern EOappTheMCNVmap* eo_appTheMCNVmap_Initialise(eOappTheMCNVmap_cfg_t *cfg);



extern EOappTheMCNVmap* eo_appTheMCNVmap_GetHandle(void);


extern eOresult_t eo_appTheMCNVmap_GetJointNVMemoryRef(EOappTheMCNVmap* p, eOmc_jointUniqueId_t jUiniqueId, uint8_t nvindex, void**memRef);


extern eOresult_t eo_appTheMCNVmap_GetJointNVMemoryRef_test(EOappTheMCNVmap* p, eOmc_jointUniqueId_t jUiniqueId, uint8_t nvindex, void**memRef);


/** @}            
    end of group eo_app_theMCNVmap  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

