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

/* @file       eOcfg_nvsEP_as_any_usr.c
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

#include "EoSensors.h"
#include "eOcfg_nvsEP_as_any_con_sxx.h"
#include "eOcfg_nvsEP_as_any_con_mxx.h"
#include "eOcfg_nvsEP_as_any_con_sxxdefault.h"
#include "eOcfg_nvsEP_as_any_con_mxxdefault.h"

#include "eOcfg_nvsEP_as_any_con_body.h"

#include "eOcfg_nvsEP_as_onestrain_con.h"
#include "eOcfg_nvsEP_as_onemais_con.h"

#include "EOnv_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_as.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_as_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

// in here we put a check vs coherency of eo_cfg_nvsEP_as_strainNVindex_t and eo_cfg_nvsEP_as_maisNVindex_t and macros 
// used in strain-mais 



EO_VERIFYproposition(xxx, strainNVindex_sconfig__mode                       == EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sconfig__mode);
EO_VERIFYproposition(xxx, strainNVindex_sconfig__datarate                   == EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sconfig__datarate);
EO_VERIFYproposition(xxx, strainNVindex_sconfig__signaloncefullscale        == EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sconfig__signaloncefullscale);
EO_VERIFYproposition(xxx, strainNVindex_sstatus__fullscale                  == EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sstatus__fullscale);
EO_VERIFYproposition(xxx, strainNVindex_sstatus__calibratedvalues           == EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sstatus__calibratedvalues);
EO_VERIFYproposition(xxx, strainNVindex_sstatus__uncalibratedvalues         == EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sstatus__uncalibratedvalues);

EO_VERIFYproposition(xxx, strainNVindex_TOTALnumber                         == EOK_cfg_nvsEP_as_any_con_sxx_snvindex_TOTALnumber);


EO_VERIFYproposition(xxx, maisNVindex_mconfig__mode                         == EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_mconfig__mode);
EO_VERIFYproposition(xxx, maisNVindex_mconfig__datarate                     == EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_mconfig__datarate);
EO_VERIFYproposition(xxx, maisNVindex_mconfig__resolution                   == EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_mconfig__resolution);
EO_VERIFYproposition(xxx, maisNVindex_mstatus__the15values                  == EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_mstatus__the15values);


EO_VERIFYproposition(xxx, maisNVindex_TOTALnumber                           == EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_TOTALnumber);


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

extern const eOsnsr_strain_t* eo_cfg_nvsEP_as_strain_defaultvalue = &eo_cfg_nvsEP_as_any_con_sxxdefault_defaultvalue;
extern const eOsnsr_mais_t* eo_cfg_nvsEP_as_mais_defaultvalue = &eo_cfg_nvsEP_as_any_con_mxxdefault_defaultvalue;




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern uint16_t eo_cfg_nvsEP_as_strain_numbermax_Get(eo_cfg_nvsEP_as_endpoint_t ep)
{
    static const uint8_t smax[9] =
    {
        strainOneStrain_TOTALnumber, 0,                                 // left arm
        strainOneStrain_TOTALnumber, 0,                                 // right arm
        0,                                                              // torso
        strainOneStrain_TOTALnumber, 0,                                 // left leg
        strainOneStrain_TOTALnumber, 0                                  // right leg       
    };  EO_VERIFYsizeof(smax, 9*sizeof(uint8_t));   
    
    uint16_t max = 0; 
    uint16_t i = (uint16_t)ep - (uint16_t)endpoint_as_leftupperarm;
    if(i < 9)
    {
        max = smax[i];
    }
    
    return(max);       
}


extern uint16_t eo_cfg_nvsEP_as_mais_numbermax_Get(eo_cfg_nvsEP_as_endpoint_t ep)
{
    static const uint8_t mmax[9] =
    {
        0, maisOneMais_TOTALnumber,                                 // left arm
        0, maisOneMais_TOTALnumber,                                 // right arm
        0,                                                          // torso
        0, 0,                                                       // left leg
        0, 0                                                        // right leg       
    };  EO_VERIFYsizeof(mmax, 9*sizeof(uint8_t));   
    
    uint16_t max = 0; 
    uint16_t i = (uint16_t)ep - (uint16_t)endpoint_as_leftupperarm;
    if(i < 9)
    {
        max = mmax[i];
    }
    
    return(max);        
    
}


// as the maximum aggregation in an endpoint is the bodypart, we use the bodypart functions to get the nvid
// irrespectively of the endpoint

extern eOnvID_t eo_cfg_nvsEP_as_strain_NVID_Get(eo_cfg_nvsEP_as_endpoint_t ep, eo_cfg_nvsEP_as_strainNumber_t s, eo_cfg_nvsEP_as_strainNVindex_t snvindex)
{
    return(eo_cfg_nvsEP_as_any_con_body_strain_NVID_Get((eo_cfg_nvsEP_as_any_con_body_strainNumber_t)s, snvindex));    
}


extern eOnvID_t eo_cfg_nvsEP_as_mais_NVID_Get(eo_cfg_nvsEP_as_endpoint_t ep, eo_cfg_nvsEP_as_maisNumber_t m, eo_cfg_nvsEP_as_maisNVindex_t mnvindex)
{
    return(eo_cfg_nvsEP_as_any_con_body_mais_NVID_Get((eo_cfg_nvsEP_as_any_con_body_maisNumber_t)m, mnvindex));
} 






// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



__weak extern void eo_cfg_nvsEP_as_hid_INITIALISE(eOnvEP_t ep, void *loc, void *rem)
{
    eObool_t theOwnershipIsLocal = (NULL == rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;    
}

// sxx-init:
__weak extern void eo_cfg_nvsEP_as_hid_INIT_Sxx_sconfig__mode(eo_cfg_nvsEP_as_strainNumber_t sxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_as_hid_INIT_Sxx_sconfig__datarate(eo_cfg_nvsEP_as_strainNumber_t sxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_as_hid_INIT_Sxx_sconfig__signaloncefullscale(eo_cfg_nvsEP_as_strainNumber_t sxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_as_hid_INIT_Sxx_sstatus__fullscale(eo_cfg_nvsEP_as_strainNumber_t sxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_as_hid_INIT_Sxx_sstatus__calibratedvalues(eo_cfg_nvsEP_as_strainNumber_t sxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_as_hid_INIT_Sxx_sstatus__uncalibratedvalues(eo_cfg_nvsEP_as_strainNumber_t sxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

// sxx-updt:

__weak extern void eo_cfg_nvsEP_as_hid_UPDT_Sxx_sconfig__mode(eo_cfg_nvsEP_as_strainNumber_t sxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_as_hid_UPDT_Sxx_sconfig__datarate(eo_cfg_nvsEP_as_strainNumber_t sxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_as_hid_UPDT_Sxx_sconfig__signaloncefullscale(eo_cfg_nvsEP_as_strainNumber_t sxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_as_hid_UPDT_Sxx_sstatus__fullscale(eo_cfg_nvsEP_as_strainNumber_t sxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_as_hid_UPDT_Sxx_sstatus__calibratedvalues(eo_cfg_nvsEP_as_strainNumber_t sxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_as_hid_UPDT_Sxx_sstatus__uncalibratedvalues(eo_cfg_nvsEP_as_strainNumber_t sxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

// mxx-init:

__weak extern void eo_cfg_nvsEP_as_hid_INIT_Mxx_mconfig__mode(eo_cfg_nvsEP_as_maisNumber_t mxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_as_hid_INIT_Mxx_mconfig__datarate(eo_cfg_nvsEP_as_maisNumber_t mxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_as_hid_INIT_Mxx_mconfig__resolution(eo_cfg_nvsEP_as_maisNumber_t mxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_as_hid_INIT_Mxx_mstatus__the15values(eo_cfg_nvsEP_as_maisNumber_t mxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

// mxx-updt:

__weak extern void eo_cfg_nvsEP_as_hid_UPDT_Mxx_mconfig__mode(eo_cfg_nvsEP_as_maisNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_as_hid_UPDT_Mxx_mconfig__datarate(eo_cfg_nvsEP_as_maisNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_as_hid_UPDT_Mxx_mconfig__resolution(eo_cfg_nvsEP_as_maisNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_as_hid_UPDT_Mxx_mstatus__the15values(eo_cfg_nvsEP_as_maisNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
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
