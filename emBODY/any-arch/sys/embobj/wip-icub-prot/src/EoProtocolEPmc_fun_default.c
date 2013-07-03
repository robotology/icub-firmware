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

/* @file       EoProtocolEPmc_fun_default.c
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
#include "EoProtocolEPmc_rom.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolEPmc_fun.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define EOPROT_EP_MC_FUN_OVERRIDE

#if defined(EOPROT_EP_MC_FUN_OVERRIDE)
#include "EoProtocolEPmc_fun_overridden.h"
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

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INITIALISE)
__weak extern void eoprot_ep_mc_fun_INITIALISE(eOnvEP_t ep, void *ram) {}
#endif

#warning --> when debug is over .... remove the initialisation of joint_config
#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_config)
__weak extern void eoprot_ep_mc_fun_INIT_joint_config(const EOnv* nv) // {}
    { memcpy(nv->ram, &eoprot_ep_mc_rom_joint_defaultvalue.config, sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config)); }
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_config)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_config(const EOnv* nv, const eOropdescriptor_t* rd)  {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_config__pidposition)
__weak extern void eoprot_ep_mc_fun_INIT_joint_config__pidposition(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_config__pidposition)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_config__pidposition(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_config__pidvelocity)
__weak extern void eoprot_ep_mc_fun_INIT_joint_config__pidvelocity(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_config__pidvelocity)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_config__pidvelocity(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif
   

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_config__pidtorque)
__weak extern void eoprot_ep_mc_fun_INIT_joint_config__pidtorque(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_config__pidtorque)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_config__pidtorque(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_config__impedance)
__weak extern void eoprot_ep_mc_fun_INIT_joint_config__impedance(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_config__impedance)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_config__impedance(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_config__minpositionofjoint)
__weak extern void eoprot_ep_mc_fun_INIT_joint_config__minpositionofjoint(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_config__minpositionofjoint)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_config__minpositionofjoint(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif
   

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_config__maxpositionofjoint)
__weak extern void eoprot_ep_mc_fun_INIT_joint_config__maxpositionofjoint(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_config__maxpositionofjoint)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_config__maxpositionofjoint(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_config__velocitysetpointtimeout)
__weak extern void eoprot_ep_mc_fun_INIT_joint_config__velocitysetpointtimeout(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_config__velocitysetpointtimeout)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_config__velocitysetpointtimeout(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif
  

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_config__motionmonitormode)
__weak extern void eoprot_ep_mc_fun_INIT_joint_config__motionmonitormode(const EOnv* nv) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_config__motionmonitormode)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_config__motionmonitormode(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_config__encoderconversionfactor)
__weak extern void eoprot_ep_mc_fun_INIT_joint_config__encoderconversionfactor(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_config__encoderconversionfactor)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_config__encoderconversionfactor(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_config__encoderconversionoffset)
__weak extern void eoprot_ep_mc_fun_INIT_joint_config__encoderconversionoffset(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_config__encoderconversionoffset)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_config__encoderconversionoffset(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

  
#warning --> when debug is over .... remove the initialisation of joint_status
#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_status)
__weak extern void eoprot_ep_mc_fun_INIT_joint_status(const EOnv* nv) // {}
    { memcpy(nv->ram, &eoprot_ep_mc_rom_joint_defaultvalue.status, sizeof(eoprot_ep_mc_rom_joint_defaultvalue.status)); }
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_status)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_status(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_status__basic)
__weak extern void eoprot_ep_mc_fun_INIT_joint_status__basic(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_status__basic)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_status__basic(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_status__ofpid)
__weak extern void eoprot_ep_mc_fun_INIT_joint_status__ofpid(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_status__ofpid)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_status__ofpid(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_status__chamaleon04)
__weak extern void eoprot_ep_mc_fun_INIT_joint_status__chamaleon04(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_status__chamaleon04_updt)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_status__chamaleon04(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif



#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_inputs)
__weak extern void eoprot_ep_mc_fun_INIT_joint_inputs(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_inputs)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_inputs(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_inputs__externallymeasuredtorque)
__weak extern void eoprot_ep_mc_fun_INIT_joint_inputs__externallymeasuredtorque(const EOnv* nv) {}
#endif   

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_inputs__externallymeasuredtorque)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_inputs__externallymeasuredtorque(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif



#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_cmmnds__calibration)
__weak extern void eoprot_ep_mc_fun_INIT_joint_cmmnds__calibration(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_cmmnds__calibration_UDPT)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_cmmnds__calibration(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_cmmnds__setpoint)
__weak extern void eoprot_ep_mc_fun_INIT_joint_cmmnds__setpoint(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_cmmnds__setpoint)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_cmmnds__setpoint(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_cmmnds__stoptrajectory)
__weak extern void eoprot_ep_mc_fun_INIT_joint_cmmnds__stoptrajectory(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_cmmnds__stoptrajectory)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_cmmnds__stoptrajectory(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_cmmnds__controlmode)
__weak extern void eoprot_ep_mc_fun_INIT_joint_cmmnds__controlmode(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_cmmnds__controlmode)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_cmmnds__controlmode(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif
  

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_config)
__weak extern void eoprot_ep_mc_fun_INIT_motor_config(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_config)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_config(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_config__pidcurrent)
__weak extern void eoprot_ep_mc_fun_INIT_motor_config__pidcurrent(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_config__pidcurrent)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_config__pidcurrent(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_config__maxvelocityofmotor)
__weak extern void eoprot_ep_mc_fun_INIT_motor_config__maxvelocityofmotor(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_config__maxvelocityofmotor)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_config__maxvelocityofmotor(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_config__maxcurrentofmotor)
__weak extern void eoprot_ep_mc_fun_INIT_motor_config__maxcurrentofmotor(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_config__maxcurrentofmotor)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_config__maxcurrentofmotor(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_status)
__weak extern void eoprot_ep_mc_fun_INIT_motor_status(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_status)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_status(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_status__basic)
__weak extern void eoprot_ep_mc_fun_INIT_motor_status__basic(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_status__basic)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_status__basic(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_status__chamaleon04)
__weak extern void eoprot_ep_mc_fun_INIT_motor_status__chamaleon04(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_status__chamaleon04)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_status__chamaleon04(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif   


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_controller_config)
__weak extern void eoprot_ep_mc_fun_INIT_controller_config(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_controller_config)
__weak extern void eoprot_ep_mc_fun_UPDT_controller_config(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_controller_config__durationofctrlloop)
__weak extern void eoprot_ep_mc_fun_INIT_controller_config__durationofctrlloop(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_controller_config__durationofctrlloop)
__weak extern void eoprot_ep_mc_fun_UPDT_controller_config__durationofctrlloop(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_controller_status)
__weak extern void eoprot_ep_mc_fun_INIT_controller_status(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_controller_status)
__weak extern void eoprot_ep_mc_fun_UPDT_controller_status(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_controller_status__alljomoinitted)
__weak extern void eoprot_ep_mc_fun_INIT_controller_status__alljomoinitted(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_controller_status__alljomoinitted)
__weak extern void eoprot_ep_mc_fun_UPDT_controller_status__alljomoinitted(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_controller_cmmnds__go2stateofcontroller)
__weak extern void eoprot_ep_mc_fun_INIT_controller_cmmnds__go2stateofcontroller(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_controller_cmmnds__go2stateofcontroller)
__weak extern void eoprot_ep_mc_fun_UPDT_controller_cmmnds__go2stateofcontroller(const EOnv* nv, const eOropdescriptor_t* rd) {}
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

