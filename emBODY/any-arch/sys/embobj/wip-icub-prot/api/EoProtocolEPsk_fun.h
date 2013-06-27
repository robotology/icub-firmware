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

#ifndef _EOPROTOCOLEPSK_FUN_H_
#define _EOPROTOCOLEPSK_FUN_H_




/** @file       EoProtocolEPsk_fun.h
	@brief      This header file contains the declarations of the functions which can be ovverridden in the skin of skin 
	@author     marco.accame@iit.it
	@date       05/29/2013
**/

/** @defgroup eo_EoProtocolEPsk_fun Functions used in skin endpoint
    Here are the functions used in skin endpoint. They can be overridden as they are weakly defined in the .c file.
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EOnv.h"
#include "EOrop.h"


 

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

// all of them are weakly-defined or/and overridable by means of a specific include file

extern void eoprot_ep_sk_fun_INITIALISE(eOnvEP_t ep, void *ram);


extern void eoprot_ep_sk_fun_INIT_skin_config__sigmode(const EOnv* nv);
extern void eoprot_ep_sk_fun_UPDT_skin_config__sigmode(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_ep_sk_fun_INIT_skin_status__arrayof10canframes(const EOnv* nv);
extern void eoprot_ep_sk_fun_UPDT_skin_status__arrayof10canframes(const EOnv* nv, const eOropdescriptor_t* rd);



/** @}            
    end of group eo_EoProtocolEPsk_fun  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

