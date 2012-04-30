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

/* @file       eOcfg_nvsEP_mc_any_usr.c
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

#include "EoMotionControl.h"
#include "eOcfg_nvsEP_mc_any_con_jxx.h"
#include "eOcfg_nvsEP_mc_any_con_mxx.h"
#include "eOcfg_nvsEP_mc_any_con_bodypart.h"

#include "EOnv_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_mc.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_mc_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

// in here we put a check vs coherency of eo_cfg_nvsEP_mc_jointNVindex_t and eo_cfg_nvsEP_mc_motorNVindex_t and macros 
// used in joint-motor 


EO_VERIFYproposition(xxx, jointNVindex_jconfig                              == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__pidposition                 == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidposition);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__pidvelocity                 == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidvelocity);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__pidtorque                   == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidtorque);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__impedance                   == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__impedance);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__minpositionofjoint          == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__minpositionofjoint);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__maxpositionofjoint          == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__maxpositionofjoint);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__velocitysetpointtimeout     == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__velocitysetpointtimeout);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__controlmode                 == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__controlmode);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__motionmonitormode           == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__motionmonitormode);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__des02FORjstatuschamaleon04  == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__des02FORjstatuschamaleon04);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__holder01FFU01               == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__holder01FFU01);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__holder01FFU02               == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__holder01FFU02);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__holder02FFU03               == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__holder02FFU03);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__holder02FFU04               == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__holder02FFU04);

EO_VERIFYproposition(xxx, jointNVindex_jstatus                              == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jstatus);
EO_VERIFYproposition(xxx, jointNVindex_jstatus__basic                       == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jstatus__basic);
EO_VERIFYproposition(xxx, jointNVindex_jstatus__ofpid                       == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jstatus__ofpid);
EO_VERIFYproposition(xxx, jointNVindex_jstatus__chamaleon04                 == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jstatus__chamaleon04);

EO_VERIFYproposition(xxx, jointNVindex_jinputs__externallymeasuredtorque    == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jinputs__externallymeasuredtorque);
EO_VERIFYproposition(xxx, jointNVindex_jinputs__holder02FFU01               == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jinputs__holder02FFU01);
EO_VERIFYproposition(xxx, jointNVindex_jinputs__holder04FFU02               == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jinputs__holder04FFU02);

EO_VERIFYproposition(xxx, jointNVindex_jcmmnds__calibration                 == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jcmmnds__calibration);
EO_VERIFYproposition(xxx, jointNVindex_jcmmnds__setpoint                    == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jcmmnds__setpoint);
EO_VERIFYproposition(xxx, jointNVindex_jcmmnds__stoptrajectory              == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jcmmnds__stoptrajectory);
EO_VERIFYproposition(xxx, jointNVindex_jcmmnds__holder01FFU01               == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jcmmnds__holder01FFU01);
EO_VERIFYproposition(xxx, jointNVindex_jcmmnds__holder01FFU02               == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jcmmnds__holder01FFU02);
EO_VERIFYproposition(xxx, jointNVindex_jcmmnds__holder01FFU03               == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jcmmnds__holder01FFU03);

EO_VERIFYproposition(xxx, jointNVindex_TOTALnumber                          == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber);


EO_VERIFYproposition(xxx, motorNVindex_mconfig                              == EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig);
EO_VERIFYproposition(xxx, motorNVindex_mconfig__pidcurrent                  == EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig__pidcurrent);
EO_VERIFYproposition(xxx, motorNVindex_mconfig__maxvelocityofmotor          == EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig__maxvelocityofmotor);
EO_VERIFYproposition(xxx, motorNVindex_mconfig__maxcurrentofmotor           == EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig__maxcurrentofmotor);
EO_VERIFYproposition(xxx, motorNVindex_mconfig__des02FORmstatuschamaleon04  == EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig__des02FORmstatuschamaleon04);

EO_VERIFYproposition(xxx, motorNVindex_mstatus                              == EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mstatus);
EO_VERIFYproposition(xxx, motorNVindex_mstatus__basic                       == EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mstatus__basic);
EO_VERIFYproposition(xxx, motorNVindex_mstatus__chamaleon04                 == EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mstatus__chamaleon04);


EO_VERIFYproposition(xxx, motorNVindex_TOTALnumber                          == EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber);


 
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

// as the maximum aggregation of joint-motor in an endpoint is the bodypart, we use the bodypart functions to get the nvid
// irrespectively of the endpoint

extern eOnvID_t eo_cfg_nvsEP_mc_joint_NVID_Get(eo_cfg_nvsEP_mc_endpoint_t ep, eo_cfg_nvsEP_mc_jointNumber_t j, eo_cfg_nvsEP_mc_jointNVindex_t jnvindex)
{
    return(eo_cfg_nvsEP_mc_any_con_bodypart_NVID_for_joint_var_Get((eo_cfg_nvsEP_mc_any_con_bodypart_jointNumber_t)j, jnvindex));    
}


extern eOnvID_t eo_cfg_nvsEP_mc_motor_NVID_Get(eo_cfg_nvsEP_mc_endpoint_t ep, eo_cfg_nvsEP_mc_motorNumber_t m, eo_cfg_nvsEP_mc_motorNVindex_t mnvindex)
{
    return(eo_cfg_nvsEP_mc_any_con_bodypart_NVID_for_motor_var_Get((eo_cfg_nvsEP_mc_any_con_bodypart_motorNumber_t)m, mnvindex));
} 






// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

__weak extern void eo_cfg_nvsEP_mc_hid_INITIALISE(eOnvEP_t ep, void *loc, void *rem)
{
    eObool_t theOwnershipIsLocal = (NULL == rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;    
}

// jxx-init:
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__pidposition(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__pidvelocity(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__pidtorque(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__impedance(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__minpositionofjoint(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__maxpositionofjoint(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__velocitysetpointtimeout(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__controlmode(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__motionmonitormode(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__des02FORjstatuschamaleon04(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder01FFU01(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder01FFU02(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder02FFU03(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder02FFU04(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus__basic(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus__ofpid(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus__chamaleon04(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jinputs__externallymeasuredtorque(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jinputs__holder02FFU01(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jinputs__holder04FFU02(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__calibration(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__setpoint(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__stoptrajectory(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__holder01FFU01(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__holder01FFU02(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__holder01FFU03(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


// jxx-updt:

__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidposition(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidvelocity(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidtorque(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__impedance(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__minpositionofjoint(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__maxpositionofjoint(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__velocitysetpointtimeout(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__controlmode(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__motionmonitormode(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__des02FORjstatuschamaleon04(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder01FFU01(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder01FFU02(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder02FFU03(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder02FFU04(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus__basic(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus__ofpid(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus__chamaleon04(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jinputs__externallymeasuredtorque(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jinputs__holder02FFU01(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jinputs__holder04FFU02(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__calibration(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__setpoint(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__stoptrajectory(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__holder01FFU01(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__holder01FFU02(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__holder01FFU03(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


// mxx-init:

__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__pidcurrent(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__maxvelocityofmotor(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__maxcurrentofmotor(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__des02FORmstatuschamaleon04(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mstatus(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mstatus__basic(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mstatus__chamaleon04(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

// mxx-updt:


__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__pidcurrent(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__maxvelocityofmotor(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__maxcurrentofmotor(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__des02FORmstatuschamaleon04(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}


__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mstatus(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mstatus__basic(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}

__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mstatus__chamaleon04(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
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
