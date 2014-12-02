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

/* @file       eOcfg_nvsEP_as_any_con_sxx.c
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

#include "EoAnalogSensors.h"
#include "EOnv.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_as_any_con_sxx.h"


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

const uint8_t eo_cfg_nvsEP_as_any_con_sxx_funtyp[] =
{
    EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sconfig,
    EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sconfig__mode,
    EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sconfig__datarate,
    EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sconfig__signaloncefullscale,
    EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sstatus,
    EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sstatus__fullscale,
    EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sstatus__calibratedvalues,
    EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sstatus__uncalibratedvalues    
};  EO_VERIFYsizeof(eo_cfg_nvsEP_as_any_con_sxx_funtyp, EOK_cfg_nvsEP_as_any_con_sxx_snvindex_TOTALnumber);




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
