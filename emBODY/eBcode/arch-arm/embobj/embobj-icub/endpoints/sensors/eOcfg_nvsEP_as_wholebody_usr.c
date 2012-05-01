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

/* @file       eOcfg_nvsEP_as_wholebody_usr.c
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
#include "eOcfg_nvsEP_mc_hid.h"
#include "eOcfg_nvsEP_as_wholebody_con.h"
#include "eOcfg_nvsEP_as_wholebody_con_hid.h"

#include "EOnv.h"
#include "EOconstvector_hid.h"
#include "eOcfg_nvsEP_as_any_con_sxxdefault.h"
#include "eOcfg_nvsEP_as_any_con_mxxdefault.h"






// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_as_wholebody_usr.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "eOcfg_nvsEP_as_wholebody_usr_hid.h"


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

// the generic declaration
//extern void eo_cfg_nvsEP_as_wholebody_usr_hid_INIT__x1x2x3(const EOnv* nv);
//extern void eo_cfg_nvsEP_as_wholebody_usr_hid_update__x1x2x3(const EOnv* nv, const eOabstime_t time, const uint32_t sign);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


#define SXXMACRO_INIT        wholebody_usr_hid_INIT_Sxx
#define SXXMACRO_UPDT        wholebody_usr_hid_UPDT_Sxx
#define SXXMACRO_PART        _wholebody
#define SXXMACRO_BOARD       _ebx
#define SXXMACRO_SSTR        _s00
#define SXXMACRO_SNUM        0
#include "eOcfg_nvsEP_as_any_usr_sxxmacro.c"


#define SXXMACRO_INIT        wholebody_usr_hid_INIT_Sxx
#define SXXMACRO_UPDT        wholebody_usr_hid_UPDT_Sxx
#define SXXMACRO_PART        _wholebody
#define SXXMACRO_BOARD       _ebx
#define SXXMACRO_SSTR        _s01
#define SXXMACRO_SNUM        1
#include "eOcfg_nvsEP_as_any_usr_sxxmacro.c"

#define SXXMACRO_INIT        wholebody_usr_hid_INIT_Sxx
#define SXXMACRO_UPDT        wholebody_usr_hid_UPDT_Sxx
#define SXXMACRO_PART        _wholebody
#define SXXMACRO_BOARD       _ebx
#define SXXMACRO_SSTR        _s02
#define SXXMACRO_SNUM        2
#include "eOcfg_nvsEP_as_any_usr_sxxmacro.c"

#define SXXMACRO_INIT        wholebody_usr_hid_INIT_Sxx
#define SXXMACRO_UPDT        wholebody_usr_hid_UPDT_Sxx
#define SXXMACRO_PART        _wholebody
#define SXXMACRO_BOARD       _ebx
#define SXXMACRO_SSTR        _s03
#define SXXMACRO_SNUM        3
#include "eOcfg_nvsEP_as_any_usr_sxxmacro.c"


#define MXXMACRO_INIT        wholebody_usr_hid_INIT_Mxx
#define MXXMACRO_UPDT        wholebody_usr_hid_UPDT_Mxx
#define MXXMACRO_PART        _wholebody
#define MXXMACRO_BOARD       _ebx
#define MXXMACRO_MSTR        _m00
#define MXXMACRO_MNUM        0
#include "eOcfg_nvsEP_as_any_usr_mxxmacro.c"


#define MXXMACRO_INIT        wholebody_usr_hid_INIT_Mxx
#define MXXMACRO_UPDT        wholebody_usr_hid_UPDT_Mxx
#define MXXMACRO_PART        _wholebody
#define MXXMACRO_BOARD       _ebx
#define MXXMACRO_MSTR        _m01
#define MXXMACRO_MNUM        1
#include "eOcfg_nvsEP_as_any_usr_mxxmacro.c"



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


static const EOnv_usr_t s_eo_cfg_nvsEP_as_wholebody_usr_array_of_EOnv_usr[] =
{
    {   // s00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s00_ebx_sconfig__mode,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // s00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s00_ebx_sconfig__datarate,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // s00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s00_ebx_sconfig__signaloncefullscale,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // s00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s00_ebx_sstatus__fullscale,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // s00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s00_ebx_sstatus__calibratedvalues,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // s00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s00_ebx_sstatus__uncalibratedvalues,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  

    
    {   // s01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s01_ebx_sconfig__mode,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // s01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s01_ebx_sconfig__datarate,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // s01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s01_ebx_sconfig__signaloncefullscale,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // s01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s01_ebx_sstatus__fullscale,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // s01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s01_ebx_sstatus__calibratedvalues,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // s01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s01_ebx_sstatus__uncalibratedvalues,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  

    
    {   // s02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s02_ebx_sconfig__mode,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // s02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s02_ebx_sconfig__datarate,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // s02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s02_ebx_sconfig__signaloncefullscale,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // s02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s02_ebx_sstatus__fullscale,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // s02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s02_ebx_sstatus__calibratedvalues,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // s02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s02_ebx_sstatus__uncalibratedvalues,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  

    {   // s03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s03_ebx_sconfig__mode,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // s03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s03_ebx_sconfig__datarate,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // s03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s03_ebx_sconfig__signaloncefullscale,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // s03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s03_ebx_sstatus__fullscale,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // s03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s03_ebx_sstatus__calibratedvalues,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // s03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_s03_ebx_sstatus__uncalibratedvalues,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  


    // and in here come the 2 mais     
    
    
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_m00_ebx_mconfig__mode,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_m00_ebx_mconfig__datarate,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_m00_ebx_mconfig__resolution,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_m00_ebx_mstatus__the15values,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 

    
    
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_m01_ebx_mconfig__mode,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_m01_ebx_mconfig__datarate,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_m01_ebx_mconfig__resolution,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_wholebody_m01_ebx_mstatus__the15values,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }
    
};  EO_VERIFYsizeof(s_eo_cfg_nvsEP_as_wholebody_usr_array_of_EOnv_usr, sizeof(EOnv_usr_t)*(varsASwholebody_TOTALnumber)); 


const EOconstvector  s_eo_cfg_nvsEP_as_wholebody_usr_constvector_of_EOnv_usr = 
{
    EO_INIT(.size)              sizeof(s_eo_cfg_nvsEP_as_wholebody_usr_array_of_EOnv_usr)/sizeof(EOnv_usr_t), 
    EO_INIT(.item_size)         sizeof(EOnv_usr_t),
    EO_INIT(.item_array_data)   s_eo_cfg_nvsEP_as_wholebody_usr_array_of_EOnv_usr
};


extern const EOconstvector* const eo_cfg_nvsEP_as_wholebody_usr_constvector_of_EOnv_usr = &s_eo_cfg_nvsEP_as_wholebody_usr_constvector_of_EOnv_usr;

extern eo_cfg_nvsEP_as_wholebody_t* nvsEP_as_wholebody_usr_mem_local    = NULL;
extern eo_cfg_nvsEP_as_wholebody_t* nvsEP_as_wholebody_usr_mem_remote   = NULL;



extern eo_cfg_nvsEP_as_wholebody_t* eo_cfg_nvsEP_as_wholebody_usr_locallyownedmemory            = NULL;

extern eo_cfg_nvsEP_as_wholebody_t* eo_cfg_nvsEP_as_wholebody_usr_remotelyownedmemory_loc       = NULL;
extern eo_cfg_nvsEP_as_wholebody_t* eo_cfg_nvsEP_as_wholebody_usr_remotelyownedmemory_rem       = NULL;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern void eo_cfg_nvsEP_as_wholebody_usr_initialise(eOnvEP_t ep, void* loc, void* rem)
{ 
    eObool_t theOwnershipIsLocal = (NULL == rem) ? eobool_true : eobool_false;
    
    // init the pointer to the ram of the endpoint
    if(eobool_true == theOwnershipIsLocal)
    {   // when used locally
        eo_cfg_nvsEP_as_wholebody_usr_locallyownedmemory      = (eo_cfg_nvsEP_as_wholebody_t*)loc;
    }
    else
    {   // when used remotely
        eo_cfg_nvsEP_as_wholebody_usr_remotelyownedmemory_loc = (eo_cfg_nvsEP_as_wholebody_t*)loc;
        eo_cfg_nvsEP_as_wholebody_usr_remotelyownedmemory_rem = (eo_cfg_nvsEP_as_wholebody_t*)rem;
    }
    
    // launch a specialised initialisation
    eo_cfg_nvsEP_as_wholebody_usr_hid_INITIALISE(ep, loc, rem);

}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


__weak extern void eo_cfg_nvsEP_as_wholebody_usr_hid_INITIALISE(eOnvEP_t ep, void *loc, void *rem)
{
    eObool_t theOwnershipIsLocal = (NULL == rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    //eo_cfg_nvsEP_as_hid_INITIALISE(ep, loc, rem);
}


// sxx-init:
__weak extern void eo_cfg_nvsEP_as_wholebody_usr_hid_INIT_Sxx_sconfig__mode(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    //eo_cfg_nvsEP_as_hid_INIT_Sxx_sconfig__mode((eo_cfg_nvsEP_as_wholebody_con_strainNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_as_wholebody_usr_hid_INIT_Sxx_sconfig__datarate(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    //eo_cfg_nvsEP_as_hid_INIT_Sxx_sconfig__datarate((eo_cfg_nvsEP_as_wholebody_con_strainNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_as_wholebody_usr_hid_INIT_Sxx_sconfig__signaloncefullscale(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    //eo_cfg_nvsEP_as_hid_INIT_Sxx_sconfig__signaloncefullscale((eo_cfg_nvsEP_as_wholebody_con_strainNumber_t)xx, nv);
}


__weak extern void eo_cfg_nvsEP_as_wholebody_usr_hid_INIT_Sxx_sstatus__fullscale(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    //eo_cfg_nvsEP_as_hid_INIT_Sxx_sstatus__fullscale((eo_cfg_nvsEP_as_wholebody_con_strainNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_as_wholebody_usr_hid_INIT_Sxx_sstatus__calibratedvalues(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    //eo_cfg_nvsEP_as_hid_INIT_Sxx_sstatus__calibratedvalues((eo_cfg_nvsEP_as_wholebody_con_strainNumber_t)xx, nv);
}


__weak extern void eo_cfg_nvsEP_as_wholebody_usr_hid_INIT_Sxx_sstatus__uncalibratedvalues(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    //eo_cfg_nvsEP_as_hid_INIT_Sxx_sstatus__uncalibratedvalues((eo_cfg_nvsEP_as_wholebody_con_strainNumber_t)xx, nv);
}



// sxx-updt:

__weak extern void eo_cfg_nvsEP_as_wholebody_usr_hid_UPDT_Sxx_sconfig__mode(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    //eo_cfg_nvsEP_as_hid_UPDT_Sxx_sconfig__mode((eo_cfg_nvsEP_as_wholebody_con_strainNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_as_wholebody_usr_hid_UPDT_Sxx_sconfig__datarate(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    //eo_cfg_nvsEP_as_hid_UPDT_Sxx_sconfig__datarate((eo_cfg_nvsEP_as_wholebody_con_strainNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_as_wholebody_usr_hid_UPDT_Sxx_sconfig__signaloncefullscale(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    //eo_cfg_nvsEP_as_hid_UPDT_Sxx_sconfig__signaloncefullscale((eo_cfg_nvsEP_as_wholebody_con_strainNumber_t)xx, nv, time, sign);
}


__weak extern void eo_cfg_nvsEP_as_wholebody_usr_hid_UPDT_Sxx_sstatus__fullscale(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    //eo_cfg_nvsEP_as_hid_UPDT_Sxx_sstatus__fullscale((eo_cfg_nvsEP_as_wholebody_con_strainNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_as_wholebody_usr_hid_UPDT_Sxx_sstatus__calibratedvalues(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    //eo_cfg_nvsEP_as_hid_UPDT_Sxx_sstatus__calibratedvalues((eo_cfg_nvsEP_as_wholebody_con_strainNumber_t)xx, nv, time, sign);
}


__weak extern void eo_cfg_nvsEP_as_wholebody_usr_hid_UPDT_Sxx_sstatus__uncalibratedvalues(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    //eo_cfg_nvsEP_as_hid_UPDT_Sxx_sstatus__uncalibratedvalues((eo_cfg_nvsEP_as_wholebody_con_strainNumber_t)xx, nv, time, sign);
}



// mxx-init:

__weak extern void eo_cfg_nvsEP_as_wholebody_usr_hid_INIT_Mxx_mconfig__mode(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    //eo_cfg_nvsEP_as_hid_INIT_Mxx_mconfig__mode((eo_cfg_nvsEP_as_wholebody_con_maisNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_as_wholebody_usr_hid_INIT_Mxx_mconfig__datarate(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    //eo_cfg_nvsEP_as_hid_INIT_Mxx_mconfig__datarate((eo_cfg_nvsEP_as_wholebody_con_maisNumber_t)xx, nv);
}


__weak extern void eo_cfg_nvsEP_as_wholebody_usr_hid_INIT_Mxx_mconfig__resolution(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    //eo_cfg_nvsEP_as_hid_INIT_Mxx_mconfig__resolution((eo_cfg_nvsEP_as_wholebody_con_maisNumber_t)xx, nv);
}


__weak extern void eo_cfg_nvsEP_as_wholebody_usr_hid_INIT_Mxx_mstatus__the15values(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    //eo_cfg_nvsEP_as_hid_INIT_Mxx_mstatus__the15values((eo_cfg_nvsEP_as_wholebody_con_maisNumber_t)xx, nv);
}



// mxx-updt:


__weak extern void eo_cfg_nvsEP_as_wholebody_usr_hid_UPDT_Mxx_mconfig__mode(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    //eo_cfg_nvsEP_as_hid_UPDT_Mxx_mconfig__mode((eo_cfg_nvsEP_as_wholebody_con_maisNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_as_wholebody_usr_hid_UPDT_Mxx_mconfig__datarate(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    //eo_cfg_nvsEP_as_hid_UPDT_Mxx_mconfig__datarate((eo_cfg_nvsEP_as_wholebody_con_maisNumber_t)xx, nv, time, sign);
}


__weak extern void eo_cfg_nvsEP_as_wholebody_usr_hid_UPDT_Mxx_mconfig__resolution(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    //eo_cfg_nvsEP_as_hid_UPDT_Mxx_mconfig__resolution((eo_cfg_nvsEP_as_wholebody_con_maisNumber_t)xx, nv, time, sign);
}


__weak extern void eo_cfg_nvsEP_as_wholebody_usr_hid_UPDT_Mxx_mstatus__the15values(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    //eo_cfg_nvsEP_as_hid_UPDT_Mxx_mstatus__the15values((eo_cfg_nvsEP_as_wholebody_con_maisNumber_t)xx, nv, time, sign);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------









// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



