/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _EOCANMAPBOARDS_H_
#define _EOCANMAPBOARDS_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EoCANmapBoards.h
	@brief      This header file implements public interface to the .c file which contains definition of a specific layout of can boards to entities
 	@author     marco.accame@iit.it
	@date       13 mar 2015
 **/

/** @defgroup eo_canmapboard cewfcwe
    
    It configures the can boards 
  
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
    
//#include "EoBoards.h"

#include "EOtheCANmapping.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------


// this variable contains the pointers of the canboards which are in the two can buses in each of the [0, 14] address
// example, what do we have in can1 bus at address 3? we look at eo_canmapping_boards[0][3]. if no can board is present 
// then we have NULL, otherwise we have a pointer to the board.  

extern eOcanmap_canboard_t * const * const eo_canmapping_boards[];


// -- entities: joint

// this variable contains the number of joints (which is also the size of array eo_canmapping_joints, and which MUST be
// equal to eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
extern const uint8_t eo_canmapping_joints_numberof;

// this variable contains the pointers of the boards which serve a given joint in number [0, eo_canmapping_joints_numberof-1]
// for example, eo_canmapping_joints[0] gives back the pointer to the board which host the joint 0.
// in case eo_canmapping_joints_numberof is 4, then we cannot use eo_canmapping_joints[4] and beyond. 
extern const eOcanmap_canboard_t * const eo_canmapping_joints[];

// -- entities: motor

// this variable contains the number of motors (which is also the size of array eo_canmapping_motors, and which MUST be
// equal to eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor);
extern const uint8_t eo_canmapping_motors_numberof;

// this variable contains the pointers of the boards which serve a given motor in number [0, eo_canmapping_motors_numberof-1]
// for example, eo_canmapping_motors[0] gives back the pointer to the board which host the motor 0.
// in case eo_canmapping_motors_numberof is 4, then we cannot use eo_canmapping_motors[4] and beyond. 
extern const eOcanmap_canboard_t * const eo_canmapping_motors[];


// -- entities: strain

// this variable contains the number of strains (which is also the size of array eo_canmapping_strains, and which MUST be
// equal to eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_strain);
extern const uint8_t eo_canmapping_strains_numberof;

// this variable contains the pointers of the boards which serve a given strain in number [0, eo_canmapping_strains_numberof-1]
// for example, eo_canmapping_strains[0] gives back the pointer to the board which host the strain 0.
// in case eo_canmapping_strains_numberof is 1/0, then we cannot use eo_canmapping_strains[1/0] and beyond. 
extern const eOcanmap_canboard_t * const eo_canmapping_strains[];


// -- entities: mais

// this variable contains the number of maises (which is also the size of array eo_canmapping_maises, and which MUST be
// equal to eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_mais);
extern const uint8_t eo_canmapping_maises_numberof;

// this variable contains the pointers of the boards which serve a given mais in number [0, eo_canmapping_maises_numberof-1]
// for example, eo_canmapping_maises[0] gives back the pointer to the board which host the mais 0.
// in case eo_canmapping_maises_numberof is 1/0, then we cannot use eo_canmapping_maises[1/0] and beyond. 
extern const eOcanmap_canboard_t * const eo_canmapping_maises[];



// -- entities: skin -- the skin is different from other entities because a skin entity is served by more than one skin board


// this variable contains the number of skins as an entity (which is also the size of array xxxxxxxx?, and which MUST be
// equal to eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_skin, eoprot_entity_sk_skin);
extern const uint8_t eo_canmapping_skins_numberof;


// this array tells for each skin in value [0, eo_canmapping_skins_numberof-1] how many boards are used to implement it
extern const uint8_t eo_canmapping_skins_boardsinside[];

// this variable contains the pointer of each board inside the skin number [0, eo_canmapping_skins_numberof-1].
// to get the pointer of the board we must use two indices, as in the following example:
// eo_canmapping_skins_boards[0][3] contains the pointer to the fourth board of the first skin.  
// that is a valid pointer only if eo_canmapping_skins_numberof > 0, and eo_canmapping_skins_boardsinside[0] > 3.
extern const eOcanmap_canboard_t * const * const eo_canmapping_skins_boards[];


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section


/** @}            
    end of group eo_canmapboard  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

