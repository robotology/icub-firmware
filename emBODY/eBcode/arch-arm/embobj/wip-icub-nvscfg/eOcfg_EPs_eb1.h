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
#ifndef _EOCFG_EPS_EB1_H_
#define _EOCFG_EPS_EB1_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       eOcfg_EPs_eb1.h          
	@brief      This header file contains a const configuration for the endpoints of board EB1 (left arm, upper)
	@author     marco.accame@iit.it
	@date       06/06/2013
**/

/** @defgroup eo_eOcfg_EPs_eb1 Configuration of the endpoints for management of the ems board eb1
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



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

// - declaration of public user-defined types ------------------------------------------------------------------------- 

enum {  eocfg_EPs_eb1_boardnumber = 0   }; 

enum {  eocfg_EPs_eb1_numofendpoints = 3 };


// - management

enum {  eocfg_EPs_eb1_mn_numberof_comms = 1, eocfg_EPs_eb1_mn_numberof_appls = 1 };
            

/** @typedef    typedef struct eocfg_EPs_eb1_mn_t;
    @brief      It is the container of comms, apps in the management endpoint of board eb1.
 **/
typedef struct                  // 136+24+0 = 160              
{
    eOmn_comm_t                 communication; 
    eOmn_appl_t                 application;
} eocfg_EPs_eb1_mn_t;           EO_VERIFYsizeof(eocfg_EPs_eb1_mn_t, 160); 


// - motion control

enum {  eocfg_EPs_eb1_mc_numberof_joints = 4, eocfg_EPs_eb1_mc_numberof_motors = 4, eocfg_EPs_eb1_mc_numberof_controllers = 1 };
            
/** @typedef    typedef struct eocfg_EPs_eb1_mc_t;
    @brief      It is the container of joints, motors, controllers in the motion control endpoint of board eb1.
 **/
typedef struct                  // 152*4+40*4+24 = 792              
{
    eOmc_joint_t                joints[eocfg_EPs_eb1_mc_numberof_joints]; 
    eOmc_motor_t                motors[eocfg_EPs_eb1_mc_numberof_motors];
    eOmc_controller_t           thecontroller;
} eocfg_EPs_eb1_mc_t;           EO_VERIFYsizeof(eocfg_EPs_eb1_mc_t, 792);      


// - analog sensors

enum {  eocfg_EPs_eb1_as_numberof_strains = 1, eocfg_EPs_eb1_as_numberof_maises = 0 };
            

/** @typedef    typedef struct eocfg_EPs_eb1_as_t;
    @brief      It is the container of strain, mais in the analog sensors endpoint of board eb1.
 **/
typedef struct                  // 56+0 = 56              
{
    eOas_strain_t               strain; 
} eocfg_EPs_eb1_as_t;           EO_VERIFYsizeof(eocfg_EPs_eb1_as_t, 56); 
  

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOnvset_DEVcfg_t eocfg_EPs_eb1_object;


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern eOresult_t eocfg_nvsEP_eb1_Initialise(void)
    @brief      Initialises the endpoints of this board by loading the number of entities for each of them in the
                related endpoint file. As a result of that, the function eoprot_ep_mc_variables_numberof_Get(eocfg_EPs_eb1_boardnumber)
                will return the correct value.
    @return     eores_OK if successful or eores_NOK_generic upon failure.
 **/
extern eOresult_t eocfg_EPs_eb1_Initialise(void);


/** @fn         extern void* eocfg_EPs_eb1_Get_RAM(eOnvEP_t ep)
    @brief      Retrieves all the RAM of the endpoint specified by @e ep.
                This ram can be used with a proper cast to the struct representing the endpoint on the board (e.g., for 
                the endpoint eoprot_endpoint_motioncontrol in this board, the cast is to be done with eOprot_ep_mc_eb1_t).
    @param      ep              The endpoint.
    @return     The pointer to the memory or NULL if the memory is not initialised yet or if the @e ep is not recognised
 **/
extern void* eocfg_EPs_eb1_Get_RAM(eOnvEP_t ep);


/** @}            
    end of group eo_eOcfg_EPs_eb1  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




