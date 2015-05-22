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
#ifndef _EOCANPROTOCOLASPOLLING_H_
#define _EOCANPROTOCOLASPOLLING_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EoCANprotocolASpolling.h
    @brief      This header file contains declaration of the functions used to parse / form can frames related to 
                analog sensor polling messages
    @author     marco.accame@iit.it
	@date       12 mar 2015
**/

/** @defgroup eo_EoCANprotocolASpolling CAN protocol for AS polling messages
    This file contains function declarations for parsing / forming can frames related to analog sensor
    polling message
    
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

extern eOresult_t eocanprotASpolling_parser_00(eOcanframe_t *frame, eOcanport_t port);
extern eOresult_t eocanprotASpolling_former_00(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);


extern eOresult_t eocanprotASpolling_parser_01(eOcanframe_t *frame, eOcanport_t port);
extern eOresult_t eocanprotASpolling_former_01(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);


extern eOresult_t eocanprotASpolling_parser_02(eOcanframe_t *frame, eOcanport_t port);
extern eOresult_t eocanprotASpolling_former_02(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);




/** @}            
    end of group eo_EoCANprotocolASpolling  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


