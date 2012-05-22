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

/* @file       eOcfg_nvsEP_sk_emsboard_usr.c
    @brief      This file keeps the user-defined local ...
    @author     marco.accame@iit.it
    @date       09/06/2011
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EoCommon.h"
#include "eOcfg_nvsEP_sk_emsboard_con.h"

#include "EOnv_hid.h"
#include "EOconstvector_hid.h"

#include "eOcfg_nvsEP_sk_hid.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_sk_emsboard_usr.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "eOcfg_nvsEP_sk_emsboard_usr_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eo_cfg_nvsEP_sk_emsboard_INIT_sconfig__sigmode(const EOnv* nv);
static void s_eo_cfg_nvsEP_sk_emsboard_INIT_sstatus__arrayof10canframe(const EOnv* nv);

static void s_eo_cfg_nvsEP_sk_emsboard_UPDT_sconfig__sigmode(const EOnv* nv, const eOabstime_t time, const uint32_t sign);
static void s_eo_cfg_nvsEP_sk_emsboard_UPDT_sstatus__arrayof10canframe(const EOnv* nv, const eOabstime_t time, const uint32_t sign);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static const eOnv_fn_peripheral_t s_eo_cfg_nvsEP_sk_emsboard_ebx_sconfig__sigmode =
{
    EO_INIT(.init)      s_eo_cfg_nvsEP_sk_emsboard_INIT_sconfig__sigmode,
    EO_INIT(.update)    s_eo_cfg_nvsEP_sk_emsboard_UPDT_sconfig__sigmode
};

static const eOnv_fn_peripheral_t s_eo_cfg_nvsEP_sk_emsboard_ebx_sstatus__arrayof10canframe =
{
    EO_INIT(.init)      s_eo_cfg_nvsEP_sk_emsboard_INIT_sstatus__arrayof10canframe,
    EO_INIT(.update)    s_eo_cfg_nvsEP_sk_emsboard_UPDT_sstatus__arrayof10canframe
};


static const EOnv_usr_t s_eo_cfg_nvsEP_sk_emsboard_array_of_EOnv_usr[] =
{
    {   // 00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_sk_emsboard_ebx_sconfig__sigmode,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // 01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_sk_emsboard_ebx_sstatus__arrayof10canframe,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }
    
};  EO_VERIFYsizeof(s_eo_cfg_nvsEP_sk_emsboard_array_of_EOnv_usr, sizeof(EOnv_usr_t)*(varsSKemsboard_TOTALnumber)); 




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


const EOconstvector  s_eo_cfg_nvsEP_sk_emsboard_usr_constvector_of_EOnv_usr = 
{
    EO_INIT(.size)              sizeof(s_eo_cfg_nvsEP_sk_emsboard_array_of_EOnv_usr)/sizeof(EOnv_usr_t), 
    EO_INIT(.item_size)         sizeof(EOnv_usr_t),
    EO_INIT(.item_array_data)   s_eo_cfg_nvsEP_sk_emsboard_array_of_EOnv_usr
};


const EOconstvector* const eo_cfg_nvsEP_sk_emsboard_usr_constvector_of_EOnv_usr = &s_eo_cfg_nvsEP_sk_emsboard_usr_constvector_of_EOnv_usr;



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern void eo_cfg_nvsEP_sk_emsboard_usr_initialise(eOnvEP_t ep, void* loc, void* rem)
{
    eo_cfg_nvsEP_sk_emsboard_t *sk_ptr;
    eObool_t theOwnershipIsLocal = (NULL == rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
	
    // copy default values
    if(NULL != loc)
    {
	sk_ptr = (eo_cfg_nvsEP_sk_emsboard_t*)loc;     
        memcpy(sk_ptr, eo_cfg_nvsEP_sk_emsboard_defaultvalues, sizeof(eo_cfg_nvsEP_sk_emsboard_t));
    }    
   
    
    
    // launch a specialised initialisation
    eo_cfg_nvsEP_sk_emsboard_usr_hid_INITIALISE(ep, loc, rem);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// -- the following functions are called by a local board if rem is NULL, otherwise are called by the remote host (pc104).
// -- if we want to simulate on the same process (or teh same board) both local and remote behaviour, the same function is 
//    called by both local and remote transceiver, with different ram. if rem is not NULL, then it called by the remote host 

__weak extern void eo_cfg_nvsEP_sk_emsboard_usr_hid_INITIALISE(eOnvEP_t ep, void *loc, void *rem)
{
    eObool_t theOwnershipIsLocal = (NULL == rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_sk_hid_INITIALISE(ep, loc, rem);
}

// init:
__weak extern void eo_cfg_nvsEP_sk_emsboard_usr_hid_INIT_sconfig__sigmode(uint16_t n, const EOnv* nv)
{   // n is always 0
     eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
     theOwnershipIsLocal = theOwnershipIsLocal;
     eo_cfg_nvsEP_sk_hid_INIT_sconfig__sigmode(n, nv);
}

__weak extern void eo_cfg_nvsEP_sk_emsboard_usr_hid_INIT_sstatus__arrayof10canframe(uint16_t n, const EOnv* nv)
{   // n is always 0
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_sk_hid_INIT_sstatus__arrayof10canframe(n, nv);    
}


// updt:
__weak extern void eo_cfg_nvsEP_sk_emsboard_usr_hid_UPDT_sconfig__sigmode(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{   // n is always 0
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_sk_hid_UPDT_sconfig__sigmode(n, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_sk_emsboard_usr_hid_UPDT_sstatus__arrayof10canframe(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{   // n is always 0
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_sk_hid_UPDT_sstatus__arrayof10canframe(n, nv, time, sign);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------



static void s_eo_cfg_nvsEP_sk_emsboard_INIT_sconfig__sigmode(const EOnv* nv)
{   
    eo_cfg_nvsEP_sk_emsboard_usr_hid_INIT_sconfig__sigmode(0, nv);
}

static void s_eo_cfg_nvsEP_sk_emsboard_INIT_sstatus__arrayof10canframe(const EOnv* nv)
{   
    eo_cfg_nvsEP_sk_emsboard_usr_hid_INIT_sstatus__arrayof10canframe(0, nv);
}


static void s_eo_cfg_nvsEP_sk_emsboard_UPDT_sconfig__sigmode(const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{   
    eo_cfg_nvsEP_sk_emsboard_usr_hid_UPDT_sconfig__sigmode(0, nv, time, sign);
}

static void s_eo_cfg_nvsEP_sk_emsboard_UPDT_sstatus__arrayof10canframe(const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{   
    eo_cfg_nvsEP_sk_emsboard_usr_hid_UPDT_sstatus__arrayof10canframe(0, nv, time, sign);
}





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



