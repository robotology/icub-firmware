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

/* @file       EoProtocolEPmn_fun_default.c
    @brief      This file keeps constant configuration for ...
    @author     marco.accame@iit.it
    @date       06/06/2013
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolEPmn_fun.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define EOPROT_EP_MN_FUN_OVERRIDE

#if defined(EOPROT_EP_MN_FUN_OVERRIDE)
#include "EoProtocolEPmn_fun_overridden.h"
#endif




// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

#if !defined(OVERRIDE_eoprot_ep_mn_fun_INITIALISE)
__weak extern void eoprot_ep_mn_fun_INITIALISE(eOnvEP_t ep, void *loc, void *rem) {}
#endif



#if !defined(OVERRIDE_eoprot_ep_mn_fun_INIT_comm_cmmnds__ropsigcfg)
__weak extern void eoprot_ep_mn_fun_INIT_comm_cmmnds__ropsigcfg(const EOnv* nv, uint16_t index) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mn_fun_UPDT_comm_cmmnds__ropsigcfg)
__weak extern void eoprot_ep_mn_fun_UPDT_comm_cmmnds__ropsigcfg(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif

  

#if !defined(OVERRIDE_eoprot_ep_mn_fun_INIT_appl_config)
__weak extern void eoprot_ep_mn_fun_INIT_appl_config(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mn_fun_UPDT_appl_config)
__weak extern void eoprot_ep_mn_fun_UPDT_appl_config(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mn_fun_INIT_appl_status)
__weak extern void eoprot_ep_mn_fun_INIT_appl_status(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mn_fun_UPDT_appl_mstatus)
__weak extern void eoprot_ep_mn_fun_UPDT_appl_status(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mn_fun_INIT_appl_ccmmnds__go2state)
__weak extern void eoprot_ep_mn_fun_INIT_appl_cmmnds__go2state(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mn_fun_UPDT_appl_ccmmnds__go2state)
__weak extern void eoprot_ep_mn_fun_UPDT_appl_cmmnds__go2state(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

