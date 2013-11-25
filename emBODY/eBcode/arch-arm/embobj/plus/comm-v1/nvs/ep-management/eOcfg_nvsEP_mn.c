/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

/* @file       eOcfg_nvsEP_as.c
    @brief      This file keeps constant configuration for ...
    @author     marco.accame@iit.it
    @date       04/06/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EOnv_hid.h"

#include "eOcfg_nvsEP_mn_comm_con.h"
#include "eOcfg_nvsEP_mn_comm_con_hid.h"

#include "eOcfg_nvsEP_mn_appl_con.h"
#include "eOcfg_nvsEP_mn_appl_con_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_mn.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_mn_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


//#define EOCFG_NVSEP_MN_OVERRIDE

#if defined(EOCFG_NVSEP_MN_OVERRIDE)
#include "eOcfg_nvsEP_mn_overridden.h"
#endif



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

// in here we put a check vs coherency of ....


EO_VERIFYproposition(xxx, commNVindex__ropsigcfgcommand                   == EOK_cfg_nvsEP_mn_comm_con_nvindex__ropsigcfgcommand);

EO_VERIFYproposition(xxx, commNVindex_TOTALnumber                         == EOK_cfg_nvsEP_mn_comm_con_NUMofVARS);



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

//extern const eo_cfg_nvsEP_sk_emsboard_t eo_cfg_nvsEP_sk_emsboard_default;

//extern const eOskin_someskin_t* eo_cfg_nvsEP_sk_someskin_defaultvalue = &eo_cfg_nvsEP_sk_emsboard_default.someskin;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern eOnvID_t eo_cfg_nvsEP_mn_comm_NVID_Get(eOcfg_nvsEP_mn_endpoint_t ep, eOcfg_nvsEP_mn_commNumber_t n, eOcfg_nvsEP_mn_commNVindex_t nvindex)
{
    return(eo_cfg_nvsEP_mn_comm_comm_NVID_Get((eOcfg_nvsEP_mn_commNVindex_t) nvindex));
}


extern eOnvID_t eo_cfg_nvsEP_mn_appl_NVID_Get(eOcfg_nvsEP_mn_endpoint_t ep, eOcfg_nvsEP_mn_applNumber_t n, eOcfg_nvsEP_mn_applNVindex_t nvindex)
{
    return(eo_cfg_nvsEP_mn_appl_appl_NVID_Get((eOcfg_nvsEP_mn_applNVindex_t) nvindex));
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// - comm

#if !defined(OVERRIDE_eo_cfg_nvsEP_mn_comm_hid_INITIALISE)
__weak extern void eo_cfg_nvsEP_mn_comm_hid_INITIALISE(eOnvEP_t ep, void *loc, void *rem)
{
    eObool_t theOwnershipIsLocal = (NULL == rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
}
#endif

// init:    n is not used

#if !defined(OVERRIDE_eo_cfg_nvsEP_mn_comm_hid_INIT__ropsigcfgcommand)
__weak extern void eo_cfg_nvsEP_mn_comm_hid_INIT__ropsigcfgcommand(uint16_t n, const EOnv* nv)
{   // n is always 0
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

// updt:    n is not used


#if !defined(OVERRIDE_eo_cfg_nvsEP_mn_comm_hid_UPDT__ropsigcfgcommand)
__weak extern void eo_cfg_nvsEP_mn_comm_hid_UPDT__ropsigcfgcommand(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{   // n is always 0
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif


// - appl

#if !defined(OVERRIDE_eo_cfg_nvsEP_mn_appl_hid_INITIALISE)
__weak extern void eo_cfg_nvsEP_mn_appl_hid_INITIALISE(eOnvEP_t ep, void *loc, void *rem)
{
    eObool_t theOwnershipIsLocal = (NULL == rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
}
#endif

// init:    n is not used

#if !defined(OVERRIDE_eo_cfg_nvsEP_mn_appl_hid_INIT_config)
__weak extern void eo_cfg_nvsEP_mn_appl_hid_INIT_config(uint16_t n, const EOnv* nv)
{   // n is always 0
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mn_appl_hid_INIT_status)
__weak extern void eo_cfg_nvsEP_mn_appl_hid_INIT_status(uint16_t n, const EOnv* nv)
{   // n is always 0
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mn_appl_hid_INIT_cmmnds__go2state)
__weak extern void eo_cfg_nvsEP_mn_appl_hid_INIT_cmmnds__go2state(uint16_t n, const EOnv* nv)
{   // n is always 0
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif


// updt:    n is not used


#if !defined(OVERRIDE_eo_cfg_nvsEP_mn_appl_hid_UPDT_config)
__weak extern void eo_cfg_nvsEP_mn_appl_hid_UPDT_config(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{   // n is always 0
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif


#if !defined(OVERRIDE_eo_cfg_nvsEP_mn_appl_hid_UPDT_status)
__weak extern void eo_cfg_nvsEP_mn_appl_hid_UPDT_status(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{   // n is always 0
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif


#if !defined(OVERRIDE_eo_cfg_nvsEP_mn_appl_hid_UPDT_cmmnds__go2state)
__weak extern void eo_cfg_nvsEP_mn_appl_hid_UPDT_cmmnds__go2state(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{   // n is always 0
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
