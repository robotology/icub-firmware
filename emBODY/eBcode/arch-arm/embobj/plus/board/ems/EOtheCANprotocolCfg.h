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
#ifndef _EOTHECANPROTOCOLCFG_H_
#define _EOTHECANPROTOCOLCFG_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EOtheCANprotocolCfg.h
    @brief      This header file contains declaration of the functions used to parse / form can frames
    @author     marco.accame@iit.it
	@date       12 mar 2015
**/

/** @defgroup doxygroup_EOtheCANprotocolCfg CAN protocol parsing/forming functions
    This file contains function declarations for parsing / forming can frames
    
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

// - analog sensors: polling

extern eOresult_t eocanprotASpolling_parser_00(eOcanframe_t *frame, eOcanport_t port);
extern eOresult_t eocanprotASpolling_former_00(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);

extern eOresult_t eocanprotASpolling_parser_01(eOcanframe_t *frame, eOcanport_t port);
extern eOresult_t eocanprotASpolling_former_01(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);

extern eOresult_t eocanprotASpolling_parser_02(eOcanframe_t *frame, eOcanport_t port);
extern eOresult_t eocanprotASpolling_former_02(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);


// - analog sensors: periodic

extern eOresult_t eocanprotASperiodic_parser_00(eOcanframe_t *frame, eOcanport_t port);
extern eOresult_t eocanprotASperiodic_former_00(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);

extern eOresult_t eocanprotASperiodic_parser_01(eOcanframe_t *frame, eOcanport_t port);
extern eOresult_t eocanprotASperiodic_former_01(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);


// - motion control: polling

extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__exceptions(eOcanframe_t *frame, eOcanport_t port);
extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__exceptions(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);

extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__SET_MIN_POSITION(eOcanframe_t *frame, eOcanport_t port);
extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_MIN_POSITION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);

extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_MIN_POSITION(eOcanframe_t *frame, eOcanport_t port);
extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_MIN_POSITION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);

extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION(eOcanframe_t *frame, eOcanport_t port);
extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);


// - motion control: periodic

extern eOresult_t eocanprotMCperiodic_parser_00(eOcanframe_t *frame, eOcanport_t port);
extern eOresult_t eocanprotMCperiodic_former_00(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);

extern eOresult_t eocanprotMCperiodic_parser_01(eOcanframe_t *frame, eOcanport_t port);
extern eOresult_t eocanprotMCperiodic_former_01(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);


// - skin: polling
// nothing

// - skin: periodic

extern eOresult_t eocanprotSKperiodic_parser_TactileSensorOutput8bit(eOcanframe_t *frame, eOcanport_t port);
extern eOresult_t eocanprotSKperiodic_former_00(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);


/** @}            
    end of group doxygroup_EOtheCANprotocolCfg  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


