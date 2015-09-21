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
#ifndef _EOTHEBOARDCONFIG_H_
#define _EOTHEBOARDCONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EOtheBoardConfig.h
    @brief      This header file contains the error codes and error strings used by the Ethernet boards in iCub. 
    @author     marco.accame@iit.it
    @date       Nov 10 2014
**/

/** @defgroup eo_EOtheBoardConfigs Error codes and associated strings in ETH protocol for iCub 
    This file contains error codes and error strings which are used in the iCub ETH protocol for iCub.
    Its use is in eOmn_info_properties_t::code.
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOconstvector.h"
#include "EoProtocol.h"
#include "EOappEncodersReader.h"
#include "EOtheInertial.h"

#include "EOtheCANdiscovery2.h"

// - public #define  --------------------------------------------------------------------------------------------------



// - declaration of public user-defined types ------------------------------------------------------------------------- 

enum { eoboardconfig_maxvalueofsupportedentity = 3 };  // with value of 3 we support also eoprot_entity_as_inertial

// - declaration of extern public functions ---------------------------------------------------------------------------

extern const eOcandiscovery_target_t * eoboardconfig_code2mcdiscoverytarget(uint32_t code);

extern const eOcandiscovery_target_t * eoboardconfig_code2maisdiscoverytarget(uint32_t code);

extern const eOcandiscovery_target_t * eoboardconfig_code2straindiscoverytarget(uint32_t code) ;

// of eOcanmap_board_properties_t
// the constvector contains all the boards in can1 and can2. or ... is empty.
extern EOconstvector * eoboardconfig_code2canboards(uint32_t code); 

// of eOcanmap_entitydescriptor_t
// teh constnector contains reference to the boards used to offer service to a given entity.
// or ... is empty in case of no such an entity on teh board or entity being served not by can
// entity cannot be higher than eoboardconfig_maxvalueofsupportedentity. if so then it returns an empty vector
extern EOconstvector * eoboardconfig_code2entitydescriptors(uint32_t code, eOprotEndpoint_t ep, eOprotEntity_t entity); 


extern const eOappEncReader_cfg_t * eoboardconfig_code2encoderconfig(uint32_t code);

// of eOprot_EPcfg_t
extern EOconstvector * eoboardconfig_code2EPcfg(uint32_t code);

//extern eOas_inertialidentifier_t eoboardconfig_code2inertialID(uint32_t code);

extern const eOas_inertial_serviceconfig_t * eoboardconfig_code2inertialCFG(uint32_t code);


/** @}            
    end of group eo_EOtheBoardConfig  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




