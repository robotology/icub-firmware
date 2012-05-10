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

/* @file       eOcfg_nvsEP_as_onemais_usr.c
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
#include "eOcfg_nvsEP_as_hid.h"
#include "eOcfg_nvsEP_as_onemais_con.h"
#include "eOcfg_nvsEP_as_onemais_con_hid.h"

#include "EOnv.h"
#include "EOconstvector_hid.h"
#include "eOcfg_nvsEP_as_any_con_sxxdefault.h"
#include "eOcfg_nvsEP_as_any_con_mxxdefault.h"






// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_as_onemais_usr.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "eOcfg_nvsEP_as_onemais_usr_hid.h"


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



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

#if 0
#define SXXMACRO_INIT        onemais_usr_hid_INIT_Sxx
#define SXXMACRO_UPDT        onemais_usr_hid_UPDT_Sxx
#define SXXMACRO_PART        _onemais
#define SXXMACRO_BOARD       _ebx
#define SXXMACRO_SSTR        _s00
#define SXXMACRO_SNUM        0
#include "eOcfg_nvsEP_as_any_usr_sxxmacro.c"
#endif

#if 1
#define MXXMACRO_INIT        onemais_usr_hid_INIT_Mxx
#define MXXMACRO_UPDT        onemais_usr_hid_UPDT_Mxx
#define MXXMACRO_PART        _onemais
#define MXXMACRO_BOARD       _ebx
#define MXXMACRO_MSTR        _m00
#define MXXMACRO_MNUM        0
#include "eOcfg_nvsEP_as_any_usr_mxxmacro.c"
#endif




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


static const EOnv_usr_t s_eo_cfg_nvsEP_as_onemais_usr_array_of_EOnv_usr[] =
{
#if 0
    {   // s00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_onemais_s00_ebx_sconfig__mode,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // s00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_onemais_s00_ebx_sconfig__datarate,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // s00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_onemais_s00_ebx_sconfig__signaloncefullscale,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // s00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_onemais_s00_ebx_sstatus__fullscale,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // s00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_onemais_s00_ebx_sstatus__calibratedvalues,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // s00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_onemais_s00_ebx_sstatus__uncalibratedvalues,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }
#endif    

#if 1    

    // and in here come the  mais     
    
    
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_onemais_m00_ebx_mconfig__mode,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_onemais_m00_ebx_mconfig__datarate,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_onemais_m00_ebx_mconfig__resolution,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_as_onemais_m00_ebx_mstatus__the15values,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }
#endif    
    
};  EO_VERIFYsizeof(s_eo_cfg_nvsEP_as_onemais_usr_array_of_EOnv_usr, sizeof(EOnv_usr_t)*(varsASonemais_TOTALnumber)); 


const EOconstvector  s_eo_cfg_nvsEP_as_onemais_usr_constvector_of_EOnv_usr = 
{
    EO_INIT(.size)              sizeof(s_eo_cfg_nvsEP_as_onemais_usr_array_of_EOnv_usr)/sizeof(EOnv_usr_t), 
    EO_INIT(.item_size)         sizeof(EOnv_usr_t),
    EO_INIT(.item_array_data)   s_eo_cfg_nvsEP_as_onemais_usr_array_of_EOnv_usr
};


extern const EOconstvector* const eo_cfg_nvsEP_as_onemais_usr_constvector_of_EOnv_usr = &s_eo_cfg_nvsEP_as_onemais_usr_constvector_of_EOnv_usr;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern void eo_cfg_nvsEP_as_onemais_usr_initialise(eOnvEP_t ep, void* loc, void* rem)
{ 
//    eObool_t theOwnershipIsLocal = (NULL == rem) ? eobool_true : eobool_false;
//    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_as_onemais_t* p = NULL;
    uint8_t i;
   
    // copy default values
    if(NULL != loc)
    {
        p = (eo_cfg_nvsEP_as_onemais_t*) loc;
        for(i=0; i<maisOneMais_TOTALnumber; i++)
        {
            memcpy(&p->maises[i], eo_cfg_nvsEP_as_mais_defaultvalue, sizeof(eo_cfg_nvsEP_as_onemais_t));
        }
    }    
    if(NULL != rem)
    {   
        p = (eo_cfg_nvsEP_as_onemais_t*) rem;
        for(i=0; i<maisOneMais_TOTALnumber; i++)
        {
            memcpy(&p->maises[i], eo_cfg_nvsEP_as_mais_defaultvalue, sizeof(eo_cfg_nvsEP_as_onemais_t));
        }

    }        
    
    // launch a specialised initialisation
    eo_cfg_nvsEP_as_onemais_usr_hid_INITIALISE(ep, loc, rem);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


__weak extern void eo_cfg_nvsEP_as_onemais_usr_hid_INITIALISE(eOnvEP_t ep, void *loc, void *rem)
{
    eObool_t theOwnershipIsLocal = (NULL == rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_as_hid_INITIALISE(ep, loc, rem);
}


// // sxx-init:
// __weak extern void eo_cfg_nvsEP_as_onemais_usr_hid_INIT_Sxx_sconfig__mode(uint16_t xx, const EOnv* nv)
// {
//     eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
//     theOwnershipIsLocal = theOwnershipIsLocal;
//     eo_cfg_nvsEP_as_hid_INIT_Sxx_sconfig__mode((eOcfg_nvsEP_as_strainNumber_t)xx, nv);
// }

// __weak extern void eo_cfg_nvsEP_as_onemais_usr_hid_INIT_Sxx_sconfig__datarate(uint16_t xx, const EOnv* nv)
// {
//     eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
//     theOwnershipIsLocal = theOwnershipIsLocal;
//     eo_cfg_nvsEP_as_hid_INIT_Sxx_sconfig__datarate((eOcfg_nvsEP_as_strainNumber_t)xx, nv);
// }

// __weak extern void eo_cfg_nvsEP_as_onemais_usr_hid_INIT_Sxx_sconfig__signaloncefullscale(uint16_t xx, const EOnv* nv)
// {
//     eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
//     theOwnershipIsLocal = theOwnershipIsLocal;
//     eo_cfg_nvsEP_as_hid_INIT_Sxx_sconfig__signaloncefullscale((eOcfg_nvsEP_as_strainNumber_t)xx, nv);
// }


// __weak extern void eo_cfg_nvsEP_as_onemais_usr_hid_INIT_Sxx_sstatus__fullscale(uint16_t xx, const EOnv* nv)
// {
//     eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
//     theOwnershipIsLocal = theOwnershipIsLocal;
//     eo_cfg_nvsEP_as_hid_INIT_Sxx_sstatus__fullscale((eOcfg_nvsEP_as_strainNumber_t)xx, nv);
// }

// __weak extern void eo_cfg_nvsEP_as_onemais_usr_hid_INIT_Sxx_sstatus__calibratedvalues(uint16_t xx, const EOnv* nv)
// {
//     eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
//     theOwnershipIsLocal = theOwnershipIsLocal;
//     eo_cfg_nvsEP_as_hid_INIT_Sxx_sstatus__calibratedvalues((eOcfg_nvsEP_as_strainNumber_t)xx, nv);
// }


// __weak extern void eo_cfg_nvsEP_as_onemais_usr_hid_INIT_Sxx_sstatus__uncalibratedvalues(uint16_t xx, const EOnv* nv)
// {
//     eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
//     theOwnershipIsLocal = theOwnershipIsLocal;
//     eo_cfg_nvsEP_as_hid_INIT_Sxx_sstatus__uncalibratedvalues((eOcfg_nvsEP_as_strainNumber_t)xx, nv);
// }



// // sxx-updt:

// __weak extern void eo_cfg_nvsEP_as_onemais_usr_hid_UPDT_Sxx_sconfig__mode(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
// {
//     eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
//     theOwnershipIsLocal = theOwnershipIsLocal;
//     eo_cfg_nvsEP_as_hid_UPDT_Sxx_sconfig__mode((eOcfg_nvsEP_as_strainNumber_t)xx, nv, time, sign);
// }

// __weak extern void eo_cfg_nvsEP_as_onemais_usr_hid_UPDT_Sxx_sconfig__datarate(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
// {
//     eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
//     theOwnershipIsLocal = theOwnershipIsLocal;
//     eo_cfg_nvsEP_as_hid_UPDT_Sxx_sconfig__datarate((eOcfg_nvsEP_as_strainNumber_t)xx, nv, time, sign);
// }

// __weak extern void eo_cfg_nvsEP_as_onemais_usr_hid_UPDT_Sxx_sconfig__signaloncefullscale(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
// {
//     eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
//     theOwnershipIsLocal = theOwnershipIsLocal;
//     eo_cfg_nvsEP_as_hid_UPDT_Sxx_sconfig__signaloncefullscale((eOcfg_nvsEP_as_strainNumber_t)xx, nv, time, sign);
// }


// __weak extern void eo_cfg_nvsEP_as_onemais_usr_hid_UPDT_Sxx_sstatus__fullscale(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
// {
//     eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
//     theOwnershipIsLocal = theOwnershipIsLocal;
//     eo_cfg_nvsEP_as_hid_UPDT_Sxx_sstatus__fullscale((eOcfg_nvsEP_as_strainNumber_t)xx, nv, time, sign);
// }

// __weak extern void eo_cfg_nvsEP_as_onemais_usr_hid_UPDT_Sxx_sstatus__calibratedvalues(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
// {
//     eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
//     theOwnershipIsLocal = theOwnershipIsLocal;
//     eo_cfg_nvsEP_as_hid_UPDT_Sxx_sstatus__calibratedvalues((eOcfg_nvsEP_as_strainNumber_t)xx, nv, time, sign);
// }


// __weak extern void eo_cfg_nvsEP_as_onemais_usr_hid_UPDT_Sxx_sstatus__uncalibratedvalues(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
// {
//     eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
//     theOwnershipIsLocal = theOwnershipIsLocal;
//     eo_cfg_nvsEP_as_hid_UPDT_Sxx_sstatus__uncalibratedvalues((eOcfg_nvsEP_as_strainNumber_t)xx, nv, time, sign);
// }



// mxx-init:

__weak extern void eo_cfg_nvsEP_as_onemais_usr_hid_INIT_Mxx_mconfig__mode(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_as_hid_INIT_Mxx_mconfig__mode((eOcfg_nvsEP_as_maisNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_as_onemais_usr_hid_INIT_Mxx_mconfig__datarate(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_as_hid_INIT_Mxx_mconfig__datarate((eOcfg_nvsEP_as_maisNumber_t)xx, nv);
}


__weak extern void eo_cfg_nvsEP_as_onemais_usr_hid_INIT_Mxx_mconfig__resolution(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_as_hid_INIT_Mxx_mconfig__resolution((eOcfg_nvsEP_as_maisNumber_t)xx, nv);
}


__weak extern void eo_cfg_nvsEP_as_onemais_usr_hid_INIT_Mxx_mstatus__the15values(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_as_hid_INIT_Mxx_mstatus__the15values((eOcfg_nvsEP_as_maisNumber_t)xx, nv);
}



// mxx-updt:


__weak extern void eo_cfg_nvsEP_as_onemais_usr_hid_UPDT_Mxx_mconfig__mode(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_as_hid_UPDT_Mxx_mconfig__mode((eOcfg_nvsEP_as_maisNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_as_onemais_usr_hid_UPDT_Mxx_mconfig__datarate(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_as_hid_UPDT_Mxx_mconfig__datarate((eOcfg_nvsEP_as_maisNumber_t)xx, nv, time, sign);
}


__weak extern void eo_cfg_nvsEP_as_onemais_usr_hid_UPDT_Mxx_mconfig__resolution(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_as_hid_UPDT_Mxx_mconfig__resolution((eOcfg_nvsEP_as_maisNumber_t)xx, nv, time, sign);
}


__weak extern void eo_cfg_nvsEP_as_onemais_usr_hid_UPDT_Mxx_mstatus__the15values(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_as_hid_UPDT_Mxx_mstatus__the15values((eOcfg_nvsEP_as_maisNumber_t)xx, nv, time, sign);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------









// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



