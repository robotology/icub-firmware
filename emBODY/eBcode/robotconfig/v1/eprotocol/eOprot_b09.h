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
#ifndef _EOPROT_B09_H_
#define _EOPROT_B09_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       eOprot_b09.h          
	@brief      This header file contains protocol personalisation for board eb9 (right leg, lower) and its interface
                towards EOnvset.
	@author     marco.accame@iit.it
	@date       06/06/2013
**/

/** @defgroup doxy_eOprot_b09 Configuration of protocol for board eb9 (right lower leg)
    It contains protocol personalisation for board eb9 (right leg, lower) and its interface towards EOnvset 
    
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

enum { eoprot_b09_boardnumber = 8 }; 


enum { eoprot_b09_endpoints_numberof = 3 };


// - management

enum { eoprot_b09_mn_comms_numberof = 1, eoprot_b09_mn_appls_numberof = 1 };


/** @typedef    typedef struct eOprot_b09_management_t;
    @brief      It is the container of entities comm and app in the management endpoint of board b09.
 **/
typedef struct                  // 124+24+0 = 128              
{
    eOmn_comm_t                 communication; 
    eOmn_appl_t                 application;
} eOprot_b09_management_t;      //EO_VERIFYsizeof(eOprot_b09_management_t, 128); 


// - motion control

enum { eoprot_b09_mc_joints_numberof = 2, eoprot_b09_mc_motors_numberof = 2, eoprot_b09_mc_controllers_numberof = 1 };

 
/** @typedef    typedef struct eOprot_b09_motioncontrol_t;
    @brief      It is the container of joints, motors, controllers in the motion control endpoint of board eb1.
 **/
typedef struct                  // 152*2+40*2+16 = 400              
{
    eOmc_joint_t                joints[eoprot_b09_mc_joints_numberof]; 
    eOmc_motor_t                motors[eoprot_b09_mc_motors_numberof];
    eOmc_controller_t           thecontroller;
} eOprot_b09_motioncontrol_t;   //EO_VERIFYsizeof(eOprot_b09_motioncontrol_t, 400);      



// - analog sensors

enum { eoprot_b09_as_strains_numberof = 0, eoprot_b09_as_maises_numberof = 0, eoprot_b09_as_extorque_numberof = 2 };
         

/** @typedef    typedef struct eOprot_b09_analogsensors_t;
    @brief      It is the container of strain, mais in the analog sensors endpoint.
 **/
typedef struct                  // 2*8+0 = 16              
{
    eOas_extorque_t             extorque[eoprot_b09_as_extorque_numberof];
} eOprot_b09_analogsensors_t;   //EO_VERIFYsizeof(eOprot_b09_analogsensors_t, 16); 


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

// the configuration for the EOnvset object for protocol management
extern const eOnvset_DEVcfg_t eoprot_b09_nvsetDEVcfg;

extern const uint8_t eoprot_b09_mn_entities_numberofeach[]; // = { eoprot_b09_mn_comms_numberof, eoprot_b09_mn_appls_numberof };
extern const uint8_t eoprot_b09_mc_entities_numberofeach[]; // = { eoprot_b09_mc_joints_numberof, eoprot_b09_mc_motors_numberof, eoprot_b09_mc_controllers_numberof };
extern const uint8_t eoprot_b09_as_entities_numberofeach[]; // = { eoprot_b09_as_strains_numberof, eoprot_b09_as_maises_numberof, eoprot_b09_as_extorque_numberof };


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern eOresult_t eoprot_b09_Initialise(eObool_t islocal)
    @brief      Initialises the endpoints of this board by loading the number of 
                entities for each of them in the related endpoint file. As a result of that, 
                the functions which require a brd argument will return the correct value if called 
                with brd = eoprot_b09_boardnumber.
                This function is called by the EOnvset because the eOnvset_DEVcfg_t contains a 
                pointer to it.  However, it is made public so that it can be called independently 
                from the use of EOnvset.
    @return     eores_OK if successful or eores_NOK_generic upon failure.
 **/
extern eOresult_t eoprot_b09_Initialise(eObool_t islocal);



/** @}            
    end of group doxy_eOprot_b09  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




