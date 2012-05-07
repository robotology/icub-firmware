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

/* @file       eOcfg_nvsEP_mc_leg_usr_pippo.c
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
#include "eOcfg_nvsEP_mc_leg_usr_hid.h"

#include "eOcfg_nvsEP_mc_leg_con.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------



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



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
#warning --> OK BUT NEED TO KNOW WHICH EP WE ARE INITIALISING.
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INITIALISE(eOnvEP_t ep, void *loc, void *rem)
{
    eObool_t theOwnershipIsLocal = (NULL == rem) ? eobool_true : eobool_false;
    
    uint8_t j, m;
    eo_cfg_nvsEP_mc_leg_t *lloc = (eo_cfg_nvsEP_mc_leg_t*)loc;
    eo_cfg_nvsEP_mc_leg_t *rrem = (eo_cfg_nvsEP_mc_leg_t*)rem;    
 
    // in here we initailise the ram allocated by the EOnvscfg object: 
    // 1. at least put it at its default value. 
    // 2. if you need to initialise other peripherals or objects linked to the values do it.
    //    HOWEVER: initialisation of NVs is done on specific functions 
    
    
    // 1. assign default values to loc ..

    for(j=0; j<jointLeg_TOTALnumber; j++)
    {
        memcpy(&lloc->joints[j], eo_cfg_nvsEP_mc_leg_joint_defaultvalue, sizeof(eOmc_joint_t)); 
    }
    for(m=0; m<motorLeg_TOTALnumber; m++)
    {
        memcpy(&lloc->motors[m], &eo_cfg_nvsEP_mc_leg_motor_defaultvalue, sizeof(eOmc_motor_t)); 
    }    
    
    
    // 2. init other peripherals ...
    // i dont do it
    
}


// jxx-init:
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig(uint16_t xx, const EOnv* nv)
{
    // xx is the joint number
    
}

extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jstatus(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    // xx is the joint number
}








// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



