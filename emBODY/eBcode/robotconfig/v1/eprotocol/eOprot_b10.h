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
#ifndef _EOPROT_B10_H_
#define _EOPROT_B10_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       eOprot_b10.h          
	@brief      This header file contains protocol personalisation for board eb10 (left leg, skin) and its interface
                towards EOnvset.
	@author     marco.accame@iit.it
	@date       04/12/2014
**/

/** @defgroup doxy_eOprot_b10 Configuration of protocol for board eeb10 (left leg, skin) 
    It contains protocol personalisation for board eb10 (left leg, skin)  and its interface towards EOnvset 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOnvSet.h"

#include "EoProtocol.h"
#include "EoProtocolMN.h"
#include "EoProtocolSK.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

// - declaration of public user-defined types ------------------------------------------------------------------------- 

enum { eoprot_b10_boardnumber = 9 }; 


enum { eoprot_b10_endpoints_numberof = 2 };


// - management

enum { eoprot_b10_mn_comms_numberof = 1, eoprot_b10_mn_appls_numberof = 1 };


/** @typedef    typedef struct eOprot_b10_management_t;
    @brief      It is the container of entities comm and app in the management endpoint of board b10.
 **/
typedef struct                  // 124+24+0 = 128              
{
    eOmn_comm_t                 communication; 
    eOmn_appl_t                 application;
} eOprot_b10_management_t;      //EO_VERIFYsizeof(eOprot_b10_management_t, 128); 


// - motion control: none

// - analog sensors: none


// - skin

enum { eoprot_b10_sk_skins_numberof = 2 };
            

/** @typedef    typedef struct eOprot_b10_skin_t;
    @brief      It is the container of skins in the skin endpoint of board eb2.
 **/
typedef struct                  // 176+0 = 176              
{
    eOsk_skin_t                 skin[eoprot_b10_sk_skins_numberof]; 
} eOprot_b10_skin_t;            //EO_VERIFYsizeof(eOprot_b10_skin_t, 176); 


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

// the configuration for the EOnvset object for protocol management
extern const eOnvset_DEVcfg_t eoprot_b10_nvsetDEVcfg;

extern const uint8_t eoprot_b10_mn_entities_numberofeach[]; // = { eoprot_b10_mn_comms_numberof, eoprot_b10_mn_appls_numberof };
extern const uint8_t eoprot_b10_sk_entities_numberofeach[]; // = { eoprot_b10_sk_skins_numberof };


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern eOresult_t eoprot_b10_Initialise(eObool_t islocal)
    @brief      Initialises the endpoints of this board by loading the number of 
                entities for each of them in the related endpoint file. As a result of that, 
                the functions which require a brd argument will return the correct value if called 
                with brd = eoprot_b01_boardnumber.
                This function is called by the EOnvset because the eOnvset_DEVcfg_t contains a 
                pointer to it.  However, it is made public so that it can be called independently 
                from the use of EOnvset.
    @return     eores_OK if successful or eores_NOK_generic upon failure.
 **/
extern eOresult_t eoprot_b10_Initialise(eObool_t islocal);


/** @fn         extern eObool_t eoprot_b10_isvariableproxied(eOnvID32_t id)
    @brief      Tells if a variable is proxied.
                This function is called by the EOnvset because the eOnvset_DEVcfg_t contains a 
                pointer to it.  However, it is made public so that it can be called independently 
                from the use of EOnvset.
    @return     eobool_true if the variable described by ID is proxied, eobool_false if it is fully local.
 **/
extern eObool_t eoprot_b10_isvariableproxied(eOnvID32_t id);




/** @}            
    end of group doxy_eOprot_b10  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




