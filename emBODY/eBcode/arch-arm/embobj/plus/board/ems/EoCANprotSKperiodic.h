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
#ifndef _EOCANPROTOCOLSKPERIODIC_H_
#define _EOCANPROTOCOLSKPERIODIC_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EoCANprotocolSKperiodic.h
    @brief      This header file contains declaration of the functions used to parse / form can frames related to 
                skin periodic messages
    @author     marco.accame@iit.it
	@date       12 mar 2015
**/

/** @defgroup eo_EoCANprotocolSKperiodic CAN protocol for SK periodic messages
    This file contains function declarations for parsing / forming can frames related to skin
    periodic message
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOtheCANprotocol.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

  
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

extern eOresult_t eocanprotSKperiodic_parser_TactileSensorOutput8bit(eOcanframe_t *frame, eOcanport_t port);
extern eOresult_t eocanprotSKperiodic_former_00(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);




/** @}            
    end of group eo_EoCANprotocolSKperiodic  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


