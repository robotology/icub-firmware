/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _EOCFG_EPS_EB2_H_
#define _EOCFG_EPS_EB2_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       eOcfg_EPs_eb2.h          
	@brief      This header file contains a const configuration for the endpoints of board EB2 (left arm, lower)
	@author     marco.accame@iit.it
	@date       06/06/2013
**/

/** @defgroup eo_eOcfg_EPs_eb2 Configuration of the NVs for management of the ems board eb2
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOconstvector.h"
#include "EOnvSet.h"

#include "EoManagement.h"
#include "EoMotionControl.h"
#include "EoAnalogSensors.h"
#include "EoSkin.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

// - declaration of public user-defined types ------------------------------------------------------------------------- 

enum {  eocfg_EPs_eb2_boardnumber = 1   }; 

enum {  eocfg_EPs_eb2_numofendpoints = 4 };


// - management

enum {  eocfg_EPs_eb2_mn_numberof_comms = 1, eocfg_EPs_eb2_mn_numberof_appls = 1 };
            

/** @typedef    typedef struct eocfg_EPs_eb2_mn_t;
    @brief      It is the container of comms, apps in the management endpoint of board eb2.
 **/
typedef struct                  // 124+24+0 = 128              
{
    eOmn_comm_t                 communication; 
    eOmn_appl_t                 application;
} eocfg_EPs_eb2_mn_t;           EO_VERIFYsizeof(eocfg_EPs_eb2_mn_t, 128); 


// - motion control

enum {  eocfg_EPs_eb2_mc_numberof_joints = 12, eocfg_EPs_eb2_mc_numberof_motors = 12, eocfg_EPs_eb2_mc_numberof_controllers = 1 };
            
/** @typedef    typedef struct eocfg_EPs_eb2_mc_t;
    @brief      It is the container of joints, motors, controllers in the motion control endpoint of board eb2.
 **/
typedef struct                  // 152*12+40*12+24 = 2328              
{
    eOmc_joint_t                joints[eocfg_EPs_eb2_mc_numberof_joints]; 
    eOmc_motor_t                motors[eocfg_EPs_eb2_mc_numberof_motors];
    eOmc_controller_t           thecontroller;
} eocfg_EPs_eb2_mc_t;           EO_VERIFYsizeof(eocfg_EPs_eb2_mc_t, 2328);      


// - analog sensors

enum {  eocfg_EPs_eb2_as_numberof_strains = 0, eocfg_EPs_eb2_as_numberof_maises = 1 };
            

/** @typedef    typedef struct eocfg_EPs_eb2_as_t;
    @brief      It is the container of strain, mais in the analog sensors endpoint of board eb2.
 **/
typedef struct                  // 48+0 = 48              
{
    eOas_mais_t                 mais; 
} eocfg_EPs_eb2_as_t;           EO_VERIFYsizeof(eocfg_EPs_eb2_as_t, 48); 
  

  
// - skin

enum {  eocfg_EPs_eb2_sk_numberof_skins = 1 };
            

/** @typedef    typedef struct eocfg_EPs_eb2_sk_t;
    @brief      It is the container of skins in the skin endpoint of board eb2.
 **/
typedef struct                  // 176+0 = 176              
{
    eOsk_skin_t                 skin; 
} eocfg_EPs_eb2_sk_t;           EO_VERIFYsizeof(eocfg_EPs_eb2_sk_t, 176); 
    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOnvset_DEVcfg_t eocfg_EPs_eb2_object;


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern eOresult_t eocfg_nvsEP_eb2_Initialise(void)
    @brief      Initialises the endpoints of this board by loading the number of entities for each of them in the
                related endpoint file. As a result of that, the function eoprot_ep_mc_variables_numberof_Get(eocfg_EPs_eb2_boardnumber)
                will return the correct value.
    @return     eores_OK if successful or eores_NOK_generic upon failure.
 **/
extern eOresult_t eocfg_EPs_eb2_Initialise(void);


/** @fn         extern void* eocfg_EPs_eb2_Get_RAM(eOnvEP8_t ep)
    @brief      Retrieves all the RAM of the endpoint specified by @e ep.
                This ram can be used with a proper cast to the struct representing the endpoint on the board (e.g., for 
                the endpoint eoprot_endpoint_motioncontrol in this board, the cast is to be done with eOprot_ep_mc_eb2_t).
    @param      ep              The endpoint.
    @return     The pointer to the memory or NULL if the memory is not initialised yet or if the @e ep is not recognised
 **/
extern void* eocfg_EPs_eb2_Get_RAM(eOnvEP8_t ep);




/** @}            
    end of group eo_eOcfg_EPs_eb2  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




