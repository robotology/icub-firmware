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

#include "EoSkin.h"
#include "EOnv_hid.h"

#include "eOcfg_nvsEP_sk_emsboard_con.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_sk.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_sk_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



//#define EOCFG_NVSEP_SK_OVERRIDE

#if defined(EOCFG_NVSEP_SK_OVERRIDE)
#include "eOcfg_nvsEP_sk_overridden.h"
#endif


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

// in here we put a check vs coherency of ....


EO_VERIFYproposition(xxx, skinNVindex_sconfig__sigmode                      == skinEMSboardNVindex_sconfig__sigmode);
EO_VERIFYproposition(xxx, skinNVindex_sstatus__arrayof10canframe            == skinEMSboardNVindex_sstatus__arrayof10canframe);

EO_VERIFYproposition(xxx, skinNVindex_TOTALnumber                           == skinEMSboardNVindex_TOTALnumber);



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

extern const eo_cfg_nvsEP_sk_emsboard_t eo_cfg_nvsEP_sk_emsboard_default;

extern const eOskin_someskin_t* eo_cfg_nvsEP_sk_someskin_defaultvalue = &eo_cfg_nvsEP_sk_emsboard_default.someskin;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern eOnvID_t eo_cfg_nvsEP_sk_NVID_Get(eOcfg_nvsEP_sk_endpoint_t ep, eOcfg_nvsEP_sk_skinNumber_t s, eOcfg_nvsEP_sk_skinNVindex_t snvindex)
{
    return(eo_cfg_nvsEP_sk_emsboard_NVID_Get((eo_cfg_nvsEP_sk_emsboard_con_skinNVindex_t) snvindex));
}





// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


__weak extern void eo_cfg_nvsEP_sk_hid_INITIALISE(eOnvEP_t ep, void *loc, void *rem)
{
    eObool_t theOwnershipIsLocal = (NULL == rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
}

// init:
__weak extern void eo_cfg_nvsEP_sk_hid_INIT_sconfig__sigmode(uint16_t n, const EOnv* nv)
{   // n is always 0
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_sk_hid_INIT_sstatus__arrayof10canframe(uint16_t n, const EOnv* nv)
{   // n is always 0
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


// updt:
__weak extern void eo_cfg_nvsEP_sk_hid_UPDT_sconfig__sigmode(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{   // n is always 0
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_sk_hid_UPDT_sstatus__arrayof10canframe(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{   // n is always 0
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
