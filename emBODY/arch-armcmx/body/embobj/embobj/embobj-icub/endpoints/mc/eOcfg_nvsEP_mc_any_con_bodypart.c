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

/* @file       eOcfg_nvsEP_mc_any_con_bodypart.c
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


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_mc_any_con_bodypart.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_mc_any_con_bodypart_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

// in here we put a check vs inconsistent values between joint-motor and bodypart

EO_VERIFYproposition(xxx, jointNVindex_jconfig                              == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__pidposition                 == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidposition);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__pidvelocity                 == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidvelocity);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__pidtorque                   == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__pidtorque);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__minpositionofjoint          == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__minpositionofjoint);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__maxpositionofjoint          == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__maxpositionofjoint);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__velocitysetpointtimeout     == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__velocitysetpointtimeout);
EO_VERIFYproposition(xxx, jointNVindex_jconfig__upto02descrforchameleon02   == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_jconfig__upto02descrforchameleon02);
EO_VERIFYproposition(xxx, jointNVindex__jstatus                             == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex__jstatus);
EO_VERIFYproposition(xxx, jointNVindex__calibrator                          == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex__calibrator);
EO_VERIFYproposition(xxx, jointNVindex__setpoint                            == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex__setpoint);
EO_VERIFYproposition(xxx, jointNVindex__controlmode                         == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex__controlmode);
EO_VERIFYproposition(xxx, jointNVindex__signalwhenmotionisdone              == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex__signalwhenmotionisdone);
EO_VERIFYproposition(xxx, jointNVindex__motionisdone                        == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex__motionisdone);
//EO_VERIFYproposition(xxx, jointNVindex__measuredtorque                      == );

EO_VERIFYproposition(xxx, jointNVindex_TOTALnumber                          == EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber);

EO_VERIFYproposition(xxx, motorNVindex_mconfig                              == EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig);
EO_VERIFYproposition(xxx, motorNVindex_mconfig__pidcurrent                  == EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig__pidcurrent);
EO_VERIFYproposition(xxx, motorNVindex_mconfig__maxvelocityofmotor          == EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig__maxvelocityofmotor);
EO_VERIFYproposition(xxx, motorNVindex_mconfig__maxcurrentofmotor           == EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig__maxcurrentofmotor);
EO_VERIFYproposition(xxx, motorNVindex_mconfig__upto02descrforchameleon06   == EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_mconfig__upto02descrforchameleon06);
EO_VERIFYproposition(xxx, motorNVindex__mstatus                             == EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex__mstatus);

EO_VERIFYproposition(xxx, motorNVindex_TOTALnumber                          == EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber);

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const uint8_t s_eo_cfg_nvsEP_mc_any_con_bodypart_jxxfuntyp[] =
{
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidposition,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidvelocity,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidtorque,                           
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__minpositionofjoint,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__maxpositionofjoint,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__velocitysetpointtimeout,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__upto02descrforchameleon02,

    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__jstatus,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__calibrator,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__setpoint,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__controlmode,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__signalwhenmotionisdone,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__motionisdone
};  EO_VERIFYsizeof(s_eo_cfg_nvsEP_mc_any_con_bodypart_jxxfuntyp, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber);


static const uint8_t s_eo_cfg_nvsEP_mc_any_con_bodypart_mxxfuntyp[] =
{
    EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig,
    EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__pidcurrent,
    EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__maxvelocityofmotor,
    EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__maxcurrentofmotor,
    EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__upto02descrforchameleon06,

    EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP__mstatus
};  EO_VERIFYsizeof(s_eo_cfg_nvsEP_mc_any_con_bodypart_mxxfuntyp, EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern eOnvID_t eo_cfg_nvsEP_mc_any_con_bodypart_NVID_for_joint_var_Get(eo_cfg_nvsEP_mc_any_con_bodypart_jointNumber_t j, eo_cfg_nvsEP_mc_any_con_bodypart_jointNVindex_t jnvindex)
{
    if((j >= EOK_cfg_nvsEP_mc_any_con_bodypart_maxnumof_joints) || (jnvindex >= EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber))
    {
        return(EOK_uint16dummy);
    }
    return(EO_nv_ID(s_eo_cfg_nvsEP_mc_any_con_bodypart_jxxfuntyp[jnvindex], EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, jnvindex)));    
}


extern eOnvID_t eo_cfg_nvsEP_mc_any_con_bodypart_NVID_for_motor_var_Get(eo_cfg_nvsEP_mc_any_con_bodypart_motorNumber_t m, eo_cfg_nvsEP_mc_any_con_bodypart_motorNVindex_t mnvindex)
{
    if((m >= EOK_cfg_nvsEP_mc_any_con_bodypart_maxnumof_motors) || (mnvindex >= EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber))
    {
        return(EOK_uint16dummy);
    }
    return(EO_nv_ID(s_eo_cfg_nvsEP_mc_any_con_bodypart_mxxfuntyp[mnvindex], EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_motor(m, mnvindex)));
} 


#if 0

extern eOnvIDfuntyp_t eo_cfg_nvsEP_mc_any_con_bodypart_NVIDfuntyp_jxx_Get(eo_cfg_nvsEP_mc_any_con_bodypart_jointNVindex_t jnvindex);

extern eOnvIDfuntyp_t eo_cfg_nvsEP_mc_any_con_bodypart_NVIDfuntyp_mxx_Get(eo_cfg_nvsEP_mc_any_con_bodypart_motorNVindex_t mnvindex);

extern eOnvIDfuntyp_t eo_cfg_nvsEP_mc_any_con_bodypart_NVIDfuntyp_jxx_Get(eo_cfg_nvsEP_mc_any_con_bodypart_jointNVindex_t jnvindex)
{
    if(jnvindex > EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber)
    {
        return(EOK_uint08dummy);
    }
    return(s_eo_cfg_nvsEP_mc_any_con_bodypart_jxxfuntyp[jnvindex]);
}  


extern eOnvIDfuntyp_t eo_cfg_nvsEP_mc_any_con_bodypart_NVIDfuntyp_mxx_Get(eo_cfg_nvsEP_mc_any_con_bodypart_motorNVindex_t mnvindex)
{
    if(mnvindex > EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber)
    {
        return(EOK_uint08dummy);
    }
    return(s_eo_cfg_nvsEP_mc_any_con_bodypart_mxxfuntyp[mnvindex]);
} 
    
#endif   



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
