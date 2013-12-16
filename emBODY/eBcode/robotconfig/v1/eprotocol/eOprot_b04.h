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
#ifndef _EOPROT_B04_H_
#define _EOPROT_B04_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       eOprot_b04.h          
	@brief      This header file contains protocol personalisation for board eb4 (right arm, lower) and its interface
                towards EOnvset.
	@author     marco.accame@iit.it
	@date       06/06/2013
**/

/** @defgroup doxy_eOprot_b04 Configuration of protocol for board eb4 (right arm, lower)
    It contains protocol personalisation for board eb4 (right arm, lower) and its interface towards EOnvset 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOnvSet.h"

#include "EoProtocol.h"
#include "EoProtocolAS.h"
#include "EoProtocolMC.h"
#include "EoProtocolMN.h"
#include "EoProtocolSK.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

// - declaration of public user-defined types ------------------------------------------------------------------------- 

enum { eoprot_b04_boardnumber = 3 }; 


enum { eoprot_b04_endpoints_numberof = 4 };


// - management

enum { eoprot_b04_mn_comms_numberof = 1, eoprot_b04_mn_appls_numberof = 1 };


/** @typedef    typedef struct eOprot_b04_management_t;
    @brief      It is the container of entities comm and app in the management endpoint of board b04.
 **/
typedef struct                  // 124+24+0 = 128              
{
    eOmn_comm_t                 communication; 
    eOmn_appl_t                 application;
} eOprot_b04_management_t;      //EO_VERIFYsizeof(eOprot_b04_management_t, 128); 


// - motion control

enum { eoprot_b04_mc_joints_numberof = 12, eoprot_b04_mc_motors_numberof = 12, eoprot_b04_mc_controllers_numberof = 1 };

 
/** @typedef    typedef struct eOprot_b04_motioncontrol_t;
    @brief      It is the container of joints, motors, controllers in the motion control endpoint of board eb1.
 **/
typedef struct                  // 152*12+40*12+16 = 2320            
{
    eOmc_joint_t                joints[eoprot_b04_mc_joints_numberof]; 
    eOmc_motor_t                motors[eoprot_b04_mc_motors_numberof];
    eOmc_controller_t           thecontroller;
} eOprot_b04_motioncontrol_t;   //EO_VERIFYsizeof(eOprot_b04_motioncontrol_t, 2320);      



// - analog sensors

enum { eoprot_b04_as_strains_numberof = 0, eoprot_b04_as_maises_numberof = 1, eoprot_b04_as_extorque_numberof = 12 };
         

/** @typedef    typedef struct eOprot_b04_analogsensors_t;
    @brief      It is the container of strain, mais in the analog sensors endpoint of board eb1.
 **/
typedef struct                  // 48+12*8+0 = 144             
{
    eOas_mais_t                 mais; 
    eOas_extorque_t             extorque[eoprot_b04_as_extorque_numberof];
} eOprot_b04_analogsensors_t;   //EO_VERIFYsizeof(eOprot_b04_analogsensors_t, 144); 


// - skin

enum { eoprot_b04_sk_skins_numberof = 1 };
            

/** @typedef    typedef struct eOprot_b04_skin_t;
    @brief      It is the container of skins in the skin endpoint of board eb2.
 **/
typedef struct                  // 176+0 = 176              
{
    eOsk_skin_t                 skin; 
} eOprot_b04_skin_t;            //EO_VERIFYsizeof(eOprot_b04_skin_t, 176); 


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

// the configuration for the EOnvset object for protocol management
extern const eOnvset_DEVcfg_t eoprot_b04_nvsetDEVcfg;

extern const uint8_t eoprot_b04_mn_entities_numberofeach[]; // = { eoprot_b04_mn_comms_numberof, eoprot_b04_mn_appls_numberof };
extern const uint8_t eoprot_b04_mc_entities_numberofeach[]; // = { eoprot_b04_mc_joints_numberof, eoprot_b04_mc_motors_numberof, eoprot_b04_mc_controllers_numberof };
extern const uint8_t eoprot_b04_as_entities_numberofeach[]; // = { eoprot_b04_as_strains_numberof, eoprot_b04_as_maises_numberof, eoprot_b04_as_extorque_numberof };
extern const uint8_t eoprot_b04_sk_entities_numberofeach[]; // = { eoprot_b04_sk_skins_numberof };


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern eOresult_t eoprot_b04_Initialise(eObool_t islocal)
    @brief      Initialises the endpoints of this board by loading the number of 
                entities for each of them in the related endpoint file. As a result of that, 
                the functions which require a brd argument will return the correct value if called 
                with brd = eoprot_b01_boardnumber.
                This function is called by the EOnvset because the eOnvset_DEVcfg_t contains a 
                pointer to it.  However, it is made public so that it can be called independently 
                from the use of EOnvset.
    @return     eores_OK if successful or eores_NOK_generic upon failure.
 **/
extern eOresult_t eoprot_b04_Initialise(eObool_t islocal);



/** @}            
    end of group doxy_eOprot_b04  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




