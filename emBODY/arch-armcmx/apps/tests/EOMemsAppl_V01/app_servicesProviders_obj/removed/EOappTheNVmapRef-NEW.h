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
#include "EoSkin.h"
#include "EoSensors.h"
#include "EOarray.h"
#include "EOnvsCfg.h"
#include "EOnv.h"



// - public #define  --------------------------------------------------------------------------------------------------


// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    eOappTheNVmapRef_cfg_t
    @brief      eOappTheNVmapRef_cfg_t contains .....
 **/  
typedef struct
{
    EOnvsCfg    *nvsCfg;      /**<  Network Variable configuration          */
} eOappTheNVmapRef_cfg_t;


/** @typedef    typedef struct EOappTheNVmapRef_hid EOappTheNVmapRef
    @brief      ...... 
 **/  
typedef struct EOappTheNVmapRef_hid EOappTheNVmapRef;



typedef enum
{
    eOappNVmapRef_jointNVindex_jconfig                               = 0,
    eOappNVmapRef_jointNVindex_jstaus                                = 1,
    eOappNVmapRef_jointNVindex_jinputs__externallymeasuredtorque     = 2,
    eOappNVmapRef_jointNVindex_jcmmnds__calibration                  = 3,
    eOappNVmapRef_jointNVindex_jcmmnds__setpoint                     = 4,
    eOappNVmapRef_jointNVindex_jcmmnds__stoptrajectory               = 5,
} eOappNVmapRef_jointNVindex_t;
enum{ eOappNVmapRef_jointNVindex_TOTALnumber = eOappNVmapRef_jointNVindex_jcmmnds__stoptrajectory +1 };


typedef enum
{
    eOappNVmapRef_motorNVindex_mconfig                               = 0,
    eOappNVmapRef_motorNVindex_mstaus                                = 1
} eOappNVmapRef_motorNVindex_t;

enum{ eOappNVmapRef_motorNVindex_TOTALnumber = eOappNVmapRef_motorNVindex_mstaus +1 };


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
    @return    returns:
               - eores_NOK_nullpointer if p is NULL
               - eores_NOK_generic is jId or Nvindex are out of range
               - eores_NOK_nodata if joint identified by @e jId is not managed, by this board
               - eores_OK on success
 **/
extern eOresult_t eo_appTheNVmapRef_GetJointNVMemoryRef(EOappTheNVmapRef* p, eOmc_jointId_t jId, eOappNVmapRef_jointNVindex_t index, void**memRef);


/** @fn         extern eOresult_t eo_appTheNVmapRef_GetJointNVMemoryRef(EOappTheNVmapRef* p, eOmc_jointId_t jUiniqueId, uint8_t nvindex, void**memRef);
    @brief      given joint id and network variable index about joint return a poiter to ram where there is nv's value.
    @param      
    @return    returns:
               - eores_NOK_nullpointer if p is NULL
               - eores_NOK_generic is jId or Nvindex are out of range
               - eores_NOK_nodata if joint identified by @e jId is not managed, by this board
               - eores_OK on success
 **/
extern eOresult_t eo_appTheNVmapRef_GetMotorNVMemoryRef(EOappTheNVmapRef* p, eOmc_motorId_t jId, eOappNVmapRef_motorNVindex_t index, void**memRef);

/** @}            
    end of group eo_app_TheNVmapRef  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

