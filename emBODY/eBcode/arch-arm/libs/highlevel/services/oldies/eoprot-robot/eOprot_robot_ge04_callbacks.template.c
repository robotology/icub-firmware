/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       eOprot_robot_ge04_fakecallbacks.c
    @brief      This file keeps ...
    @author     marco.accame@iit.it
    @date       04/14/2014
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"


#include "EoProtocol.h"
#include "EoProtocolAS.h"
#include "EoProtocolMC.h"
#include "EoProtocolMN.h"
#include "EoProtocolSK.h"


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
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

#if     defined(EOPROT_CFG_OVERRIDE_CALLBACKS_IN_RUNTIME)    

extern void eoprot_fun_UPDT_mc_joint_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
}
    
 
extern void eoprot_fun_UPDT_mc_joint_config_pidposition(const EOnv* nv, const eOropdescriptor_t* rd)
{
}
 
extern void eoprot_fun_UPDT_mc_joint_config_impedance(const EOnv* nv, const eOropdescriptor_t* rd)
{
}

 
extern void eoprot_fun_UPDT_mc_joint_config_pidtorque(const EOnv* nv, const eOropdescriptor_t* rd)
{
}  
    

extern void eoprot_fun_UPDT_mc_joint_status(const EOnv* nv, const eOropdescriptor_t* rd)
{
}
        

extern void eoprot_fun_UPDT_mc_joint_status_basic(const EOnv* nv, const eOropdescriptor_t* rd)
{
}
 

extern void eoprot_fun_UPDT_mc_joint_cmmnds_setpoint(const EOnv* nv, const eOropdescriptor_t* rd)
{
}


extern void eoprot_fun_UPDT_mc_joint_config_limitsofjoint(const EOnv* nv, const eOropdescriptor_t* rd)
{
}   
    

extern void eoprot_fun_UPDT_mc_motor_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
}  
    

extern void eoprot_fun_UPDT_mc_motor_config_maxcurrentofmotor(const EOnv* nv, const eOropdescriptor_t* rd)
{
}
 

extern void eoprot_fun_UPDT_mc_motor_status_basic(const EOnv* nv, const eOropdescriptor_t* rd)
{
}
        

extern void eoprot_fun_UPDT_as_strain_status_calibratedvalues(const EOnv* nv, const eOropdescriptor_t* rd)
{
} 


extern void eoprot_fun_UPDT_as_strain_status_uncalibratedvalues(const EOnv* nv, const eOropdescriptor_t* rd)
{
} 


extern void eoprot_fun_UPDT_as_mais_status_the15values(const EOnv* nv, const eOropdescriptor_t* rd)
{
} 
    

extern void eoprot_fun_UPDT_sk_skin_status_arrayof10canframes(const EOnv* nv, const eOropdescriptor_t* rd)
{
} 

#endif//defined(EOPROT_CFG_OVERRIDE_CALLBACKS_IN_RUNTIME)    

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




