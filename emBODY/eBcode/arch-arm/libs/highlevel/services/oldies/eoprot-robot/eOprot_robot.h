/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _EOPROT_ROBOT_H_
#define _EOPROT_ROBOT_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       eOprot_robot.h          
	@brief      This header file contains protocol definition for an eth-based robot
	@author     marco.accame@iit.it
	@date       04/14/2013
**/

/** @defgroup doxy_eOprot_robot Configuration of protocol for an eth-based robot
    It contains protocol personalisation for all eth-boards of a robot 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOnvSet.h"




// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section



// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern eOresult_t eoprot_robot_Initialise(void)
    @brief      Initialises the general stuff for the eth protocol for the robot. for example: it overrides possible callbacks, or it will instantiate
                the arrays internal to EoProtocol.c which depend upon the number of boards ... (?) ... not yet implemented as now there is a fixed max 
                number of 16 boards
    @return     eores_OK if successful or eores_NOK_generic upon failure.
 **/
extern eOresult_t eoprot_robot_Initialise(void);


/** @fn         extern uint32_t eoprot_robot_id_get(void)
    @brief      retrieves a unique id of the robot
    @return     the ID.
 **/
extern uint32_t eoprot_robot_id_get(void);


/** @fn         extern uint32_t eoprot_robot_family_get(void)
    @brief      retrieves a family descriptor of the robot
    @return     the family descriptor.
 **/
extern uint32_t eoprot_robot_family_get(void);


/** @fn         extern const char * eoprot_robot_name_get(void)
    @brief      retrieves a '/0' terminated string with the name of the robot
    @return     the nane.
 **/
extern const char * eoprot_robot_name_get(void);


/** @fn         extern uint8_t eoprot_robot_DEVcfg_numberof
    @brief      retrieves the number of boards supported by the robot
    @return     the number of boards.
 **/
extern uint8_t eoprot_robot_DEVcfg_numberof(void);


/** @fn         extern const eOnvset_DEVcfg_t* eoprot_robot_DEVcfg_get(uint8_t n)
    @brief      retrieves the a pointer to the eOnvset_DEVcfg_t required to init the transceiver
                for a given board.
    @param      n       the board number in range [0, eoprot_robot_DEVcfg_numberof() - 1]
    @return     the eOnvset_DEVcfg_t pointer or NULL if n is out of range
 **/
extern const eOnvset_DEVcfg_t* eoprot_robot_DEVcfg_get(uint8_t n);



/** @}            
    end of group doxy_eOprot_robot  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




