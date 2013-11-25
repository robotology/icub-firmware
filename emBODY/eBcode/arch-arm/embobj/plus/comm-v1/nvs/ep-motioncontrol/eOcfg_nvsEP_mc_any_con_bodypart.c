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
#include "eOcfg_nvsEP_mc_any_con_cxx.h"
#include "eOcfg_nvsEP_mc_hid.h"

#include "EOnv_hid.h"


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
// empty-section

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section
 

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern eOnvID_t eo_cfg_nvsEP_mc_any_con_bodypart_joint_NVID_Get(eo_cfg_nvsEP_mc_any_con_bodypart_jointNumber_t j, eOcfg_nvsEP_mc_jointNVindex_t jnvindex)
{
    if((j >= EOK_cfg_nvsEP_mc_any_con_bodypart_maxnumof_joints) || (jnvindex >= EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber))
    {
        return(EOK_uint16dummy);
    }
    return(EO_nv_ID(eo_cfg_nvsEP_mc_any_con_jxx_funtyp[jnvindex], EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, jnvindex)));    
}


extern eOnvID_t eo_cfg_nvsEP_mc_any_con_bodypart_motor_NVID_Get(eo_cfg_nvsEP_mc_any_con_bodypart_motorNumber_t m, eOcfg_nvsEP_mc_motorNVindex_t mnvindex)
{
    if((m >= EOK_cfg_nvsEP_mc_any_con_bodypart_maxnumof_motors) || (mnvindex >= EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber))
    {
        return(EOK_uint16dummy);
    }
    return(EO_nv_ID(eo_cfg_nvsEP_mc_any_con_mxx_funtyp[mnvindex], EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_motor(m, mnvindex)));
} 

extern eOnvID_t eo_cfg_nvsEP_mc_any_con_bodypart_controller_NVID_Get(eOcfg_nvsEP_mc_controllerNVindex_t cnvindex)
{
    if((cnvindex >= EOK_cfg_nvsEP_mc_any_con_cxx_cnvindex_TOTALnumber))
    {
        return(EOK_uint16dummy);
    }
    return(EO_nv_ID(eo_cfg_nvsEP_mc_any_con_cxx_funtyp[cnvindex], EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_controller(cnvindex)));
}




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
