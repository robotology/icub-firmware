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

#include "eOcfg_nvsEP_mc_upperarm_con.h"
#include "eOcfg_nvsEP_mc_lowerarm_con.h"
#include "eOcfg_nvsEP_mc_torso_con.h"
#include "eOcfg_nvsEP_mc_upperleg_con.h"
#include "eOcfg_nvsEP_mc_lowerleg_con.h"


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

//#define EOCFG_NVSEP_MC_OVERRIDE

#if defined(EOCFG_NVSEP_MC_OVERRIDE)
#include "eOcfg_nvsEP_mc_overridden.h"
#endif




// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// in here we put a check vs coherency of eOcfg_nvsEP_mc_jointNVindex_t and eOcfg_nvsEP_mc_motorNVindex_t and macros 
// used in joint-motor 


EO_VERIFYproposition(xxx, jointNVindex_jconfig                              == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__pidposition                 == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidposition);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__pidvelocity                 == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidvelocity);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__pidtorque                   == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidtorque);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__impedance                   == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__impedance);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__minpositionofjoint          == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__minpositionofjoint);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__maxpositionofjoint          == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__maxpositionofjoint);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__velocitysetpointtimeout     == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__velocitysetpointtimeout);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__holder01FFU00                 == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__holder01FFU00);
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
EO_VERIFYproposition(xxx, jointNVindex_jcmmnds__controlmode               == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jcmmnds__controlmode);
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


extern uint16_t eo_cfg_nvsEP_mc_joint_numbermax_Get(eOcfg_nvsEP_mc_endpoint_t ep)
{
    static const uint8_t jmax[9] =
    {
        jointUpperArm_TOTALnumber, jointLowerArm_TOTALnumber,           // left arm
        jointUpperArm_TOTALnumber, jointLowerArm_TOTALnumber,           // right arm
        jointTorso_TOTALnumber,                                         // torso
        jointUpperLeg_TOTALnumber, jointLowerLeg_TOTALnumber,           // left leg
        jointUpperLeg_TOTALnumber, jointLowerLeg_TOTALnumber            // right leg       
    };  EO_VERIFYsizeof(jmax, 9*sizeof(uint8_t));
    
    uint16_t max = 0; 
    uint16_t i = (uint16_t)ep - (uint16_t)endpoint_mc_leftupperarm;
    if(i < 9)
    {
        max = jmax[i];
    }
    
    return(max);   
}

extern uint16_t eo_cfg_nvsEP_mc_motor_numbermax_Get(eOcfg_nvsEP_mc_endpoint_t ep)
{
    return(eo_cfg_nvsEP_mc_joint_numbermax_Get(ep));
}

// as the maximum aggregation of joint-motor in an endpoint is the bodypart, we use the bodypart functions to get the nvid
// irrespectively of the endpoint

extern eOnvID_t eo_cfg_nvsEP_mc_joint_NVID_Get(eOcfg_nvsEP_mc_endpoint_t ep, eOcfg_nvsEP_mc_jointNumber_t j, eOcfg_nvsEP_mc_jointNVindex_t jnvindex)
{
    return(eo_cfg_nvsEP_mc_any_con_bodypart_joint_NVID_Get((eo_cfg_nvsEP_mc_any_con_bodypart_jointNumber_t)j, jnvindex));    
}


extern eOnvID_t eo_cfg_nvsEP_mc_motor_NVID_Get(eOcfg_nvsEP_mc_endpoint_t ep, eOcfg_nvsEP_mc_motorNumber_t m, eOcfg_nvsEP_mc_motorNVindex_t mnvindex)
{
    return(eo_cfg_nvsEP_mc_any_con_bodypart_motor_NVID_Get((eo_cfg_nvsEP_mc_any_con_bodypart_motorNumber_t)m, mnvindex));
} 


extern eOnvID_t eo_cfg_nvsEP_mc_controller_NVID_Get(eOcfg_nvsEP_mc_endpoint_t ep, eOcfg_nvsEP_mc_controllerNVindex_t cnvindex)
{
    return(eo_cfg_nvsEP_mc_any_con_bodypart_controller_NVID_Get(cnvindex));
}






// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INITIALISE)
__weak extern void eo_cfg_nvsEP_mc_hid_INITIALISE(eOnvEP_t ep, void *loc, void *rem)
{
    eObool_t theOwnershipIsLocal = (NULL == rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;    
}
#endif


// jxx-init:

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__pidposition)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__pidposition(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__pidvelocity)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__pidvelocity(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__pidtorque)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__pidtorque(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__impedance)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__impedance(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__minpositionofjoint)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__minpositionofjoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__maxpositionofjoint)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__maxpositionofjoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__velocitysetpointtimeout)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__velocitysetpointtimeout(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder01FFU00)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder01FFU00(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__motionmonitormode)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__motionmonitormode(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__encoderconversionfactor)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__encoderconversionfactor(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__encoderconversionoffset)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__encoderconversionoffset(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif


#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__des02FORjstatuschamaleon04)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__des02FORjstatuschamaleon04(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder01FFU01)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder01FFU01(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder01FFU02)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder01FFU02(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder02FFU03)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder02FFU03(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder02FFU04)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder02FFU04(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus__basic)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus__basic(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus__ofpid)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus__ofpid(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus__chamaleon04)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus__chamaleon04(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jinputs__externallymeasuredtorque)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jinputs__externallymeasuredtorque(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jinputs__holder02FFU01)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jinputs__holder02FFU01(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jinputs__holder04FFU02)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jinputs__holder04FFU02(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__calibration)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__calibration(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__setpoint)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__setpoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__stoptrajectory)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__stoptrajectory(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__controlmode)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__controlmode(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__holder01FFU02)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__holder01FFU02(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__holder01FFU03)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__holder01FFU03(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif


// jxx-updt:

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidposition)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidposition(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidvelocity)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidvelocity(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidtorque)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidtorque(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__impedance)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__impedance(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__minpositionofjoint)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__minpositionofjoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__maxpositionofjoint)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__maxpositionofjoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__velocitysetpointtimeout)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__velocitysetpointtimeout(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder01FFU00)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder01FFU00(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__motionmonitormode)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__motionmonitormode(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__encoderconversionfactor)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__encoderconversionfactor(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__encoderconversionoffset)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__encoderconversionoffset(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__des02FORjstatuschamaleon04)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__des02FORjstatuschamaleon04(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder01FFU01)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder01FFU01(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder01FFU02)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder01FFU02(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder02FFU03)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder02FFU03(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder02FFU04)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder02FFU04(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus__basic)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus__basic(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus__ofpid)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus__ofpid(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus__chamaleon04)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus__chamaleon04(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jinputs__externallymeasuredtorque)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jinputs__externallymeasuredtorque(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jinputs__holder02FFU01)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jinputs__holder02FFU01(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jinputs__holder04FFU02)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jinputs__holder04FFU02(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__calibration)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__calibration(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__setpoint)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__setpoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__stoptrajectory)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__stoptrajectory(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__controlmode)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__controlmode(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__holder01FFU02)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__holder01FFU02(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__holder01FFU03)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__holder01FFU03(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

// mxx-init:


#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__pidcurrent)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__pidcurrent(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__maxvelocityofmotor)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__maxvelocityofmotor(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__maxcurrentofmotor)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__maxcurrentofmotor(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__des02FORmstatuschamaleon04)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__des02FORmstatuschamaleon04(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Mxx_mstatus)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mstatus(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Mxx_mstatus__basic)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mstatus__basic(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Mxx_mstatus__chamaleon04)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mstatus__chamaleon04(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

// mxx-updt:

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__pidcurrent)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__pidcurrent(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__maxvelocityofmotor)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__maxvelocityofmotor(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__maxcurrentofmotor)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__maxcurrentofmotor(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__des02FORmstatuschamaleon04)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__des02FORmstatuschamaleon04(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mstatus)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mstatus(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mstatus__basic)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mstatus__basic(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mstatus__chamaleon04)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mstatus__chamaleon04(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif


// c00-init:    xx is 0
#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Cxx_cconfig)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Cxx_cconfig(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Cxx_cconfig__durationofctrlloop)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Cxx_cconfig__durationofctrlloop(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Cxx_cstatus)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Cxx_cstatus(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_Cxx_cstatus__alljomoinitted)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Cxx_cstatus__alljomoinitted(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_INIT_Cxx_ccmmnds__go2stateofcontroller)
__weak extern void eo_cfg_nvsEP_mc_hid_INIT_Cxx_ccmmnds__go2stateofcontroller(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

// c00-updt:    xx is 0
#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Cxx_cconfig)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Cxx_cconfig(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Cxx_cconfig__durationofctrlloop)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Cxx_cconfig__durationofctrlloop(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Cxx_cstatus)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Cxx_cstatus(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_UPDT_Cxx_cstatus__alljomoinitted)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Cxx_cstatus__alljomoinitted(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif

#if !defined(OVERRIDE_eo_cfg_nvsEP_mc_hid_Cxx_ccmmnds__go2stateofcontroller)
__weak extern void eo_cfg_nvsEP_mc_hid_UPDT_Cxx_ccmmnds__go2stateofcontroller(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
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
