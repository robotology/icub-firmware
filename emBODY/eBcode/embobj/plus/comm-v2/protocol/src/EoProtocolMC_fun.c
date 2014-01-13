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

/* @file       EoProtocolMC_fun.c
    @brief      This file keeps constant configuration for ...
    @author     marco.accame@iit.it
    @date       06/06/2013
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EOnv_hid.h"
#include "EoProtocolMC_rom.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolMC.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define EOPROT_EP_MC_FUN_OVERRIDE

#if defined(EOPROT_EP_MC_FUN_OVERRIDE)
#include "EoProtocolMC_overridden_fun.h"
#endif




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
// empty-section




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

#if !defined(OVERRIDE_eoprot_mc_fun_INITIALISE)
__weak extern void eoprot_fun_INITIALISE_mc(eOprotIP_t ip, void *ram) {}
#endif

// -- joint
    
#if !defined(OVERRIDE_eoprot_fun_INIT_mc_joint_wholeitem)
__weak extern void eoprot_fun_INIT_mc_joint_wholeitem(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_joint_wholeitem)
__weak extern void eoprot_fun_UPDT_mc_joint_wholeitem(const EOnv* nv, const eOropdescriptor_t* rd)  {}
#endif
      
#if !defined(OVERRIDE_eoprot_fun_INIT_mc_joint_config)
__weak extern void eoprot_fun_INIT_mc_joint_config(const EOnv* nv)  {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_joint_config)
__weak extern void eoprot_fun_UPDT_mc_joint_config(const EOnv* nv, const eOropdescriptor_t* rd)  {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_mc_joint_config_pidposition)
__weak extern void eoprot_fun_INIT_mc_joint_config_pidposition(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_joint_config_pidposition)
__weak extern void eoprot_fun_UPDT_mc_joint_config_pidposition(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_mc_joint_config_pidvelocity)
__weak extern void eoprot_fun_INIT_mc_joint_config_pidvelocity(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_joint_config_pidvelocity)
__weak extern void eoprot_fun_UPDT_mc_joint_config_pidvelocity(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_mc_joint_config_pidtorque)
__weak extern void eoprot_fun_INIT_mc_joint_config_pidtorque(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_joint_config_pidtorque)
__weak extern void eoprot_fun_UPDT_mc_joint_config_pidtorque(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif
   
#if !defined(OVERRIDE_eoprot_fun_INIT_mc_joint_config_limitsofjoint)
__weak extern void eoprot_fun_INIT_mc_joint_config_limitsofjoint(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_joint_config_limitsofjoint)
__weak extern void eoprot_fun_UPDT_mc_joint_config_limitsofjoint(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_mc_joint_config_impedance)
__weak extern void eoprot_fun_INIT_mc_joint_config_impedance(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_joint_config_impedance)
__weak extern void eoprot_fun_UPDT_mc_joint_config_impedance(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_mc_joint_config_motionmonitormode)
__weak extern void eoprot_fun_INIT_mc_joint_config_motionmonitormode(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_joint_config_motionmonitormode)
__weak extern void eoprot_fun_UPDT_mc_joint_config_motionmonitormode(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_mc_joint_status)
__weak extern void eoprot_fun_INIT_mc_joint_status(const EOnv* nv)  {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_joint_status)
__weak extern void eoprot_fun_UPDT_mc_joint_status(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_mc_joint_status_basic)
__weak extern void eoprot_fun_INIT_mc_joint_status_basic(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_joint_status_basic)
__weak extern void eoprot_fun_UPDT_mc_joint_status_basic(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_mc_joint_status_ofpid)
__weak extern void eoprot_fun_INIT_mc_joint_status_ofpid(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_joint_status_ofpid)
__weak extern void eoprot_fun_UPDT_mc_joint_status_ofpid(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_mc_joint_inputs)
__weak extern void eoprot_fun_INIT_mc_joint_inputs(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_joint_inputs)
__weak extern void eoprot_fun_UPDT_mc_joint_inputs(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_mc_joint_inputs_externallymeasuredtorque)
__weak extern void eoprot_fun_INIT_mc_joint_inputs_externallymeasuredtorque(const EOnv* nv) {}
#endif   
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_joint_inputs_externallymeasuredtorque)
__weak extern void eoprot_fun_UPDT_mc_joint_inputs_externallymeasuredtorque(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_mc_joint_cmmnds_calibration)
__weak extern void eoprot_fun_INIT_mc_joint_cmmnds_calibration(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_joint_cmmnds_calibration)
__weak extern void eoprot_fun_UPDT_mc_joint_cmmnds_calibration(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_mc_joint_cmmnds_setpoint)
__weak extern void eoprot_fun_INIT_mc_joint_cmmnds_setpoint(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_joint_cmmnds_setpoint)
__weak extern void eoprot_fun_UPDT_mc_joint_cmmnds_setpoint(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_mc_joint_cmmnds_stoptrajectory)
__weak extern void eoprot_fun_INIT_mc_joint_cmmnds_stoptrajectory(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_joint_cmmnds_stoptrajectory)
__weak extern void eoprot_fun_UPDT_mc_joint_cmmnds_stoptrajectory(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_mc_joint_cmmnds_controlmode)
__weak extern void eoprot_fun_INIT_mc_joint_cmmnds_controlmode(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_joint_cmmnds_controlmode)
__weak extern void eoprot_fun_UPDT_mc_joint_cmmnds_controlmode(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif
 
// -- motor    

#if !defined(OVERRIDE_eoprot_fun_INIT_mc_motor_wholeitem)
__weak extern void eoprot_fun_INIT_mc_motor_wholeitem(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_motor_wholeitem)
__weak extern void eoprot_fun_UPDT_mc_motor_wholeitem(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif  
    
#if !defined(OVERRIDE_eoprot_fun_INIT_mc_motor_config)
__weak extern void eoprot_fun_INIT_mc_motor_config(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_motor_config)
__weak extern void eoprot_fun_UPDT_mc_motor_config(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_mc_motor_config_maxcurrentofmotor)
__weak extern void eoprot_fun_INIT_mc_motor_config_maxcurrentofmotor(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_motor_config_maxcurrentofmotor)
__weak extern void eoprot_fun_UPDT_mc_motor_config_maxcurrentofmotor(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_mc_motor_status)
__weak extern void eoprot_fun_INIT_mc_motor_status(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_motor_status)
__weak extern void eoprot_fun_UPDT_mc_motor_status(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_mc_motor_status_basic)
__weak extern void eoprot_fun_INIT_mc_motor_status_basic(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_motor_status_basic)
__weak extern void eoprot_fun_UPDT_mc_motor_status_basic(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

// -- controller
    
#if !defined(OVERRIDE_eoprot_fun_INIT_mc_controller_wholeitem)
__weak extern void eoprot_fun_INIT_mc_controller_wholeitem(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_controller_wholeitem)
__weak extern void eoprot_fun_UPDT_mc_controller_wholeitem(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif    
    
#if !defined(OVERRIDE_eoprot_fun_INIT_mc_controller_config)
__weak extern void eoprot_fun_INIT_mc_controller_config(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_controller_config)
__weak extern void eoprot_fun_UPDT_mc_controller_config(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_mc_controller_status)
__weak extern void eoprot_fun_INIT_mc_controller_status(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_mc_controller_status)
__weak extern void eoprot_fun_UPDT_mc_controller_status(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

