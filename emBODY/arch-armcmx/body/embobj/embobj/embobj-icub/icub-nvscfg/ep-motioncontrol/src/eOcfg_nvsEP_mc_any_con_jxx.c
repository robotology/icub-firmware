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

/* @file       eOcfg_nvsEP_mc_any_con_jxx.c
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
#include "EOnv.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_mc_any_con_jxx.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------





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

const uint8_t eo_cfg_nvsEP_mc_any_con_jxx_funtyp[] =
{
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidposition,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidvelocity,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidtorque,  
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__impedance,    
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__minpositionofjoint,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__maxpositionofjoint,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__velocitysetpointtimeout,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__controlmode,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__motionmonitormode,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__des02FORjstatuschamaleon04,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__holder01FFU01,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__holder01FFU02,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__holder02FFU03,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__holder02FFU04,
    

    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jstatus,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jstatus__basic,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jstatus__ofpid,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jstatus__chamaleon04,
    
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jinputs__externallymeasuredtorque,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jinputs__holder02FFU01,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jinputs__holder04FFU02,
    
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jcmmnds__calibration,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jcmmnds__setpoint,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jcmmnds__stoptrajectory,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jcmmnds__cmdcontrolxmode,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jcmmnds__holder01FFU02,
    EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jcmmnds__holder01FFU03
    
};  EO_VERIFYsizeof(eo_cfg_nvsEP_mc_any_con_jxx_funtyp, EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber);




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------




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
