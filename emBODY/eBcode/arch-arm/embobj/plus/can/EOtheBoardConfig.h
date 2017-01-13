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
#include "EoProtocol.h"
#include "EOMtheEMSrunner.h"


// - public #define  --------------------------------------------------------------------------------------------------


// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------


extern const eOemsrunner_timing_t * eoboardconfig_code2ctrlooptiming(uint32_t code);


extern const eOmn_serv_configuration_t * eoboardconfig_code2motion_serv_configuration(uint32_t code);

extern const eOmn_serv_configuration_t * eoboardconfig_code2strain_serv_configuration(uint32_t code);

extern const eOmn_serv_configuration_t * eoboardconfig_code2mais_serv_configuration(uint32_t code);

extern const eOmn_serv_configuration_t * eoboardconfig_code2skin_serv_configuration(uint32_t code);

extern const eOmn_serv_configuration_t * eoboardconfig_code2inertials_serv_configuration(uint32_t code);




/** @}            
    end of group eo_EOtheBoardConfig  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




