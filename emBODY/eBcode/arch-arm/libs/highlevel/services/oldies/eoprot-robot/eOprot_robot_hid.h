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
#ifndef _EOPROT_ROBOT_HID_H_
#define _EOPROT_ROBOT_HID_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       eOprot_robot_hid.h          
	@brief      This header file contains hidden types for protocol  definition for an eth-based robot
	@author     marco.accame@iit.it
	@date       04/14/2013
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOconstvector.h"




// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

// - declaration of public user-defined types ------------------------------------------------------------------------- 


typedef struct
{
    uint32_t                id;                     /*< a unique id of the robot */
    uint32_t                family;                 /*< family to which the robot belongs */
    char                    name[32];               /*< a descriptive name of the robot, e.g, "icub-ge04" */
    const EOconstvector*    vectorof_devcfgptr;     /*< a const vector of eOnvset_DEVcfg_t* of size equal to the number of managed boards. */
    // const EOconstvector* vectorof_transizesptr; --> for the future ... put a const-array of eOtransceiver_sizes_t for the host-transceiver, each one linked to the sizes of the ems-transceiver  
} eOprot_ROBOTcfg_t;

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




