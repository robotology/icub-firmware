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

/* @file       eo_cfg_nvsEP_mc_any_con_mxx.c
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

#include "eOcfg_nvsEP_mc_any_con_mxx.h"


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


extern const uint8_t eo_cfg_nvsEP_mc_any_con_mxx_funtyp[] =
{
    EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig,
    EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__pidcurrent,
    EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__maxvelocityofmotor,
    EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__maxcurrentofmotor,
    EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__des02FORmstatuschamaleon04,

    EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mstatus,
    EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mstatus__basic,
    EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mstatus__chamaleon04
    
};  EO_VERIFYsizeof(eo_cfg_nvsEP_mc_any_con_mxx_funtyp, EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber);




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
