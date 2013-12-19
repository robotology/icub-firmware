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
#ifndef _EOPROT_B06_H_
#define _EOPROT_B06_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       eOprot_b06.h          
	@brief      This header file contains protocol personalisation for board eb6 (left leg, upper) and its interface
                towards EOnvset.
	@author     marco.accame@iit.it
	@date       06/06/2013
**/

/** @defgroup doxy_eOprot_b06 Configuration of protocol for board eb6 (left upper leg)
    It contains protocol personalisation for board eb6 (left leg, upper) and its interface towards EOnvset 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOnvSet.h"

#include "EoProtocol.h"
#include "EoProtocolAS.h"
#include "EoProtocolMC.h"
#include "EoProtocolMN.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

// - declaration of public user-defined types ------------------------------------------------------------------------- 

enum { eoprot_b06_boardnumber = 5 }; 


enum { eoprot_b06_endpoints_numberof = 3 };


// - management

enum { eoprot_b06_mn_comms_numberof = 1, eoprot_b06_mn_appls_numberof = 1 };


/** @typedef    typedef struct eOprot_b06_management_t;
    @brief      It is the container of entities comm and app in the management endpoint of board b06.
 **/
typedef struct                  // 124+24+0 = 128              
{
    eOmn_comm_t                 communication; 
    eOmn_appl_t                 application;
} eOprot_b06_management_t;      //EO_VERIFYsizeof(eOprot_b06_management_t, 128); 


// - motion control

enum { eoprot_b06_mc_joints_numberof = 4, eoprot_b06_mc_motors_numberof = 4, eoprot_b06_mc_controllers_numberof = 1 };

 
/** @typedef    typedef struct eOprot_b06_motioncontrol_t;
    @brief      It is the container of joints, motors, controllers in the motion control endpoint of board eb1.
 **/
typedef struct                  // 152*4+40*4+16 = 784              
{
    eOmc_joint_t                joints[eoprot_b06_mc_joints_numberof]; 
    eOmc_motor_t                motors[eoprot_b06_mc_motors_numberof];
    eOmc_controller_t           thecontroller;
} eOprot_b06_motioncontrol_t;   //EO_VERIFYsizeof(eOprot_b06_motioncontrol_t, 784);      



// - analog sensors

enum { eoprot_b06_as_strains_numberof = 1, eoprot_b06_as_maises_numberof = 0, eoprot_b06_as_extorque_numberof = 4 };
         

/** @typedef    typedef struct eOprot_b06_analogsensors_t;
    @brief      It is the container of strain, mais in the analog sensors endpoint of board eb1.
 **/
typedef struct                  // 56+4*8+0 = 88              
{
    eOas_strain_t               strain; 
    eOas_extorque_t             extorque[eoprot_b06_as_extorque_numberof];
} eOprot_b06_analogsensors_t;   //EO_VERIFYsizeof(eOprot_b06_analogsensors_t, 88); 


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

// the configuration for the EOnvset object for protocol management
extern const eOnvset_DEVcfg_t eoprot_b06_nvsetDEVcfg;

extern const uint8_t eoprot_b06_mn_entities_numberofeach[]; // = { eoprot_b06_mn_comms_numberof, eoprot_b06_mn_appls_numberof };
extern const uint8_t eoprot_b06_mc_entities_numberofeach[]; // = { eoprot_b06_mc_joints_numberof, eoprot_b06_mc_motors_numberof, eoprot_b06_mc_controllers_numberof };
extern const uint8_t eoprot_b06_as_entities_numberofeach[]; // = { eoprot_b06_as_strains_numberof, eoprot_b06_as_maises_numberof, eoprot_b06_as_extorque_numberof };


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern eOresult_t eoprot_b06_Initialise(eObool_t islocal)
    @brief      Initialises the endpoints of this board by loading the number of 
                entities for each of them in the related endpoint file. As a result of that, 
                the functions which require a brd argument will return the correct value if called 
                with brd = eoprot_b06_boardnumber.
                This function is called by the EOnvset because the eOnvset_DEVcfg_t contains a 
                pointer to it.  However, it is made public so that it can be called independently 
                from the use of EOnvset.
    @return     eores_OK if successful or eores_NOK_generic upon failure.
 **/
extern eOresult_t eoprot_b06_Initialise(eObool_t islocal);


/** @fn         extern eObool_t eoprot_b06_isvariableproxied(eOnvID32_t id)
    @brief      Tells if a variable is proxied.
                This function is called by the EOnvset because the eOnvset_DEVcfg_t contains a 
                pointer to it.  However, it is made public so that it can be called independently 
                from the use of EOnvset.
    @return     eobool_true if the variable described by ID is proxied, eobool_false if it is fully local.
 **/
extern eObool_t eoprot_b06_isvariableproxied(eOnvID32_t id);

/** @}            
    end of group doxy_eOprot_b06  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




