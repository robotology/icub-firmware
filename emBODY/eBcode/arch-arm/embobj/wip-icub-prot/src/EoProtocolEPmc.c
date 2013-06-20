/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       EoProtocolEPmc.c
    @brief      This file keeps ...
    @author     marco.accame@iit.it
    @date       06/05/2013
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EoProtocolEPmc_rom.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolEPmc.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

// - guard on max number of entities
EO_VERIFYproposition(eoprot_ep_mc_dew4, eoprot_ep_mc_entities_numberof <= eoprot_entities_numberof);

// - guard on tags ...
EO_VERIFYproposition(eoprot_ep_mc_tagsnum_jo, eoprot_ep_mc_joint_tags_numberof == eoprot_ep_mc_joint_funtyps_numberof);
EO_VERIFYproposition(eoprot_ep_mc_tagsmax_jo, eoprot_ep_mc_joint_tags_numberof <= eoprot_tags_numberof);

EO_VERIFYproposition(eoprot_ep_mc_tagsnum_mo, eoprot_ep_mc_motor_tags_numberof == eoprot_ep_mc_motor_funtyps_numberof);
EO_VERIFYproposition(eoprot_ep_mc_tagsmax_mo, eoprot_ep_mc_motor_tags_numberof <= eoprot_tags_numberof);

EO_VERIFYproposition(eoprot_ep_mc_tagsnum_co, eoprot_ep_mc_controller_tags_numberof == eoprot_ep_mc_controller_funtyps_numberof);
EO_VERIFYproposition(eoprot_ep_mc_tagsmax_co, eoprot_ep_mc_controller_tags_numberof <= eoprot_tags_numberof);

// - some more constants

typedef enum
{
    eoprot_ep_mc_vars_numberof_leftupperarm   = eoprot_ep_mc_joints_numberof_leftupperarm*eoprot_ep_mc_joint_tags_numberof+
                                                eoprot_ep_mc_motors_numberof_leftupperarm*eoprot_ep_mc_motor_tags_numberof+
                                                eoprot_ep_mc_controllers_numberof_leftupperarm*eoprot_ep_mc_controller_tags_numberof,
    eoprot_ep_mc_vars_numberof_leftlowerarm   = eoprot_ep_mc_joints_numberof_leftlowerarm*eoprot_ep_mc_joint_tags_numberof+
                                                eoprot_ep_mc_motors_numberof_leftlowerarm*eoprot_ep_mc_motor_tags_numberof+
                                                eoprot_ep_mc_controllers_numberof_leftlowerarm*eoprot_ep_mc_controller_tags_numberof,                                                
    eoprot_ep_mc_vars_numberof_rightupperarm  = eoprot_ep_mc_joints_numberof_rightupperarm*eoprot_ep_mc_joint_tags_numberof+
                                                eoprot_ep_mc_motors_numberof_rightupperarm*eoprot_ep_mc_motor_tags_numberof+
                                                eoprot_ep_mc_controllers_numberof_rightupperarm*eoprot_ep_mc_controller_tags_numberof,
    eoprot_ep_mc_vars_numberof_rightlowerarm  = eoprot_ep_mc_joints_numberof_rightlowerarm*eoprot_ep_mc_joint_tags_numberof+
                                                eoprot_ep_mc_motors_numberof_rightlowerarm*eoprot_ep_mc_motor_tags_numberof+
                                                eoprot_ep_mc_controllers_numberof_rightlowerarm*eoprot_ep_mc_controller_tags_numberof, 
    eoprot_ep_mc_vars_numberof_torso          = eoprot_ep_mc_joints_numberof_torso*eoprot_ep_mc_joint_tags_numberof+
                                                eoprot_ep_mc_motors_numberof_torso*eoprot_ep_mc_motor_tags_numberof+
                                                eoprot_ep_mc_controllers_numberof_torso*eoprot_ep_mc_controller_tags_numberof,                                                 
    eoprot_ep_mc_vars_numberof_leftupperleg   = eoprot_ep_mc_joints_numberof_leftupperleg*eoprot_ep_mc_joint_tags_numberof+
                                                eoprot_ep_mc_motors_numberof_leftupperleg*eoprot_ep_mc_motor_tags_numberof+
                                                eoprot_ep_mc_controllers_numberof_leftupperleg*eoprot_ep_mc_controller_tags_numberof,
    eoprot_ep_mc_vars_numberof_leftlowerleg   = eoprot_ep_mc_joints_numberof_leftlowerleg*eoprot_ep_mc_joint_tags_numberof+
                                                eoprot_ep_mc_motors_numberof_leftlowerleg*eoprot_ep_mc_motor_tags_numberof+
                                                eoprot_ep_mc_controllers_numberof_leftlowerleg*eoprot_ep_mc_controller_tags_numberof,                                                
    eoprot_ep_mc_vars_numberof_rightupperleg  = eoprot_ep_mc_joints_numberof_rightupperleg*eoprot_ep_mc_joint_tags_numberof+
                                                eoprot_ep_mc_motors_numberof_rightupperleg*eoprot_ep_mc_motor_tags_numberof+
                                                eoprot_ep_mc_controllers_numberof_rightupperleg*eoprot_ep_mc_controller_tags_numberof,
    eoprot_ep_mc_vars_numberof_rightlowerleg  = eoprot_ep_mc_joints_numberof_rightlowerleg*eoprot_ep_mc_joint_tags_numberof+
                                                eoprot_ep_mc_motors_numberof_rightlowerleg*eoprot_ep_mc_motor_tags_numberof+
                                                eoprot_ep_mc_controllers_numberof_rightlowerleg*eoprot_ep_mc_controller_tags_numberof
} eOprot_ep_mc_vars_numberof_t;
  

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint8_t s_eoprot_ep_mc_ep2arrayindex(eOprotEP_t ep);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const uint8_t s_eoprot_ep_mc_number_of_joints[] = 
{   
    eoprot_ep_mc_joints_numberof_leftupperarm,
    eoprot_ep_mc_joints_numberof_leftlowerarm,
    eoprot_ep_mc_joints_numberof_rightupperarm,   
    eoprot_ep_mc_joints_numberof_rightlowerarm,   
    eoprot_ep_mc_joints_numberof_torso,   
    eoprot_ep_mc_joints_numberof_leftupperleg,  
    eoprot_ep_mc_joints_numberof_leftlowerleg,      
    eoprot_ep_mc_joints_numberof_rightupperleg,   
    eoprot_ep_mc_joints_numberof_rightlowerleg
};    

static const uint8_t s_eoprot_ep_mc_number_of_motors[] = 
{   
    eoprot_ep_mc_motors_numberof_leftupperarm,
    eoprot_ep_mc_motors_numberof_leftlowerarm,
    eoprot_ep_mc_motors_numberof_rightupperarm,   
    eoprot_ep_mc_motors_numberof_rightlowerarm,   
    eoprot_ep_mc_motors_numberof_torso,   
    eoprot_ep_mc_motors_numberof_leftupperleg,  
    eoprot_ep_mc_motors_numberof_leftlowerleg,      
    eoprot_ep_mc_motors_numberof_rightupperleg,   
    eoprot_ep_mc_motors_numberof_rightlowerleg
};  


static const uint16_t s_eoprot_ep_mc_number_of_variables[] = 
{   
    eoprot_ep_mc_vars_numberof_leftupperarm,
    eoprot_ep_mc_vars_numberof_leftlowerarm,
    eoprot_ep_mc_vars_numberof_rightupperarm,   
    eoprot_ep_mc_vars_numberof_rightlowerarm,   
    eoprot_ep_mc_vars_numberof_torso,   
    eoprot_ep_mc_vars_numberof_leftupperleg,  
    eoprot_ep_mc_vars_numberof_leftlowerleg,      
    eoprot_ep_mc_vars_numberof_rightupperleg,   
    eoprot_ep_mc_vars_numberof_rightlowerleg
};  

static const uint16_t s_eoprot_ep_mc_sizeof[] = 
{   
    sizeof(eOprot_ep_mc_upperarm_t),
    sizeof(eOprot_ep_mc_lowerarm_t),
    sizeof(eOprot_ep_mc_upperarm_t),   
    sizeof(eOprot_ep_mc_lowerarm_t),   
    sizeof(eOprot_ep_mc_torso_t),   
    sizeof(eOprot_ep_mc_upperleg_t),  
    sizeof(eOprot_ep_mc_lowerleg_t),      
    sizeof(eOprot_ep_mc_upperleg_t),   
    sizeof(eOprot_ep_mc_lowerleg_t)
}; 

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern uint16_t eoprot_ep_mc_variables_numberof_Get(eOprotEP_t ep)
{
    return(s_eoprot_ep_mc_number_of_variables[s_eoprot_ep_mc_ep2arrayindex(ep)]);
}

extern uint16_t eoprot_ep_mc_variable_progressivenumber_Get(eOprotEP_t ep, eOprotID_t id)
{
    return(eoprot_ep_mc_rom_epid2progressivenumber(ep, id));
}

extern uint16_t eoprot_ep_mc_joints_numberof_Get(eOprotEP_t ep)
{
    return(s_eoprot_ep_mc_number_of_joints[s_eoprot_ep_mc_ep2arrayindex(ep)]);
}

extern uint16_t eoprot_ep_mc_motors_numberof_Get(eOprotEP_t ep)
{
    return(s_eoprot_ep_mc_number_of_motors[s_eoprot_ep_mc_ep2arrayindex(ep)]);
}

extern uint16_t eoprot_ep_mc_controllers_numberof_Get(eOprotEP_t ep)
{
    return(eoprot_ep_mc_controllers_numberof);
}

extern uint16_t eoprot_ep_mc_ram_sizeof_Get(eOprotEP_t ep)
{
    return(s_eoprot_ep_mc_sizeof[s_eoprot_ep_mc_ep2arrayindex(ep)]);
}

extern uint16_t eoprot_ep_mc_variable_ram_sizeof_Get(eOprotEP_t ep, eOprotID_t id)
{
    return(eoprot_ep_mc_rom_epid2sizeofvar(ep, id));
}

extern void* eoprot_ep_mc_variable_ram_Extract(void* epram, eOprotEP_t ep, eOprotID_t id)
{
    return(eoprot_ep_mc_rom_dataepid2nvram(epram, ep, id));
}

extern void* eoprot_ep_mc_variable_rom_Get(eOprotEP_t ep, eOprotID_t id)
{
    return(eoprot_ep_mc_rom_epid2nvrom(ep, id));
}

extern const eOmc_joint_t* eoprot_ep_mc_joint_default_Get(void)
{
    return(&eoprot_ep_mc_rom_joint_defaultvalue);
}

extern const eOmc_motor_t* eoprot_ep_mc_motor_default_Get(void)
{
    return(&eoprot_ep_mc_rom_motor_defaultvalue);
}

extern const eOmc_controller_t* eoprot_ep_mc_controller_default_Get(void)
{
    return(&eoprot_ep_mc_rom_controller_defaultvalue);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

//#warning --> the function s_eoprot_ep_mc_ep2arrayindex() does not make a control upon validity of its argument ...
static uint8_t s_eoprot_ep_mc_ep2arrayindex(eOprotEP_t ep)
{   // we have at most 16 endpoints thus we use 0xf.
    uint8_t arrayindex = ep&0xf;
    return(arrayindex);    
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



