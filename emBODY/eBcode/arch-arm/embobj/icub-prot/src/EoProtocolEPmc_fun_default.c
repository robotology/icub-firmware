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
__weak extern void eoprot_ep_mc_fun_INITIALISE(eOnvEP_t ep, void *loc, void *rem) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_jconfig)
__weak extern void eoprot_ep_mc_fun_INIT_joint_jconfig(const EOnv* nv, uint16_t index) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_jconfig)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_jconfig(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_jconfig__pidposition)
__weak extern void eoprot_ep_mc_fun_INIT_joint_jconfig__pidposition(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_jconfig__pidposition)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_jconfig__pidposition(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_jconfig__pidvelocity)
__weak extern void eoprot_ep_mc_fun_INIT_joint_jconfig__pidvelocity(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_jconfig__pidvelocity)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_jconfig__pidvelocity(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif
   

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_jconfig__pidtorque)
__weak extern void eoprot_ep_mc_fun_INIT_joint_jconfig__pidtorque(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_jconfig__pidtorque)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_jconfig__pidtorque(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_jconfig__impedance)
__weak extern void eoprot_ep_mc_fun_INIT_joint_jconfig__impedance(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_jconfig__impedance)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_jconfig__impedance(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_jconfig__minpositionofjoint)
__weak extern void eoprot_ep_mc_fun_INIT_joint_jconfig__minpositionofjoint(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_jconfig__minpositionofjoint)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_jconfig__minpositionofjoint(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif
   

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_jconfig__maxpositionofjoint)
__weak extern void eoprot_ep_mc_fun_INIT_joint_jconfig__maxpositionofjoint(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_jconfig__maxpositionofjoint)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_jconfig__maxpositionofjoint(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_jconfig__velocitysetpointtimeout)
__weak extern void eoprot_ep_mc_fun_INIT_joint_jconfig__velocitysetpointtimeout(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_jconfig__velocitysetpointtimeout)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_jconfig__velocitysetpointtimeout(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif
  

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_jconfig__motionmonitormode)
__weak extern void eoprot_ep_mc_fun_INIT_joint_jconfig__motionmonitormode(const EOnv* nv, uint16_t index) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_jconfig__motionmonitormode)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_jconfig__motionmonitormode(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_jconfig__encoderconversionfactor)
__weak extern void eoprot_ep_mc_fun_INIT_joint_jconfig__encoderconversionfactor(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_jconfig__encoderconversionfactor)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_jconfig__encoderconversionfactor(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_jconfig__encoderconversionoffset)
__weak extern void eoprot_ep_mc_fun_INIT_joint_jconfig__encoderconversionoffset(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_jconfig__encoderconversionoffset)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_jconfig__encoderconversionoffset(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_jconfig__des02FORjstatuschamaleon04)
__weak extern void eoprot_ep_mc_fun_INIT_joint_jconfig__des02FORjstatuschamaleon04(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_jconfig__des02FORjstatuschamaleon04)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_jconfig__des02FORjstatuschamaleon04(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif

  

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_jstatus)
__weak extern void eoprot_ep_mc_fun_INIT_joint_jstatus(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_jstatus)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_jstatus(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_jstatus__basic)
__weak extern void eoprot_ep_mc_fun_INIT_joint_jstatus__basic(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_jstatus__basic)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_jstatus__basic(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_jstatus__ofpid)
__weak extern void eoprot_ep_mc_fun_INIT_joint_jstatus__ofpid(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_jstatus__ofpid)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_jstatus__ofpid(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_jstatus__chamaleon04)
__weak extern void eoprot_ep_mc_fun_INIT_joint_jstatus__chamaleon04(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_jstatus__chamaleon04_updt)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_jstatus__chamaleon04(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif



#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_jinputs)
__weak extern void eoprot_ep_mc_fun_INIT_joint_jinputs(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_jinputs)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_jinputs(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_jinputs__externallymeasuredtorque)
__weak extern void eoprot_ep_mc_fun_INIT_joint_jinputs__externallymeasuredtorque(const EOnv* nv, uint16_t index) {}
#endif   

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_jinputs__externallymeasuredtorque)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_jinputs__externallymeasuredtorque(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif



#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_jcmmnds__calibration)
__weak extern void eoprot_ep_mc_fun_INIT_joint_jcmmnds__calibration(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_jcmmnds__calibration_UDPT)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_jcmmnds__calibration(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_jcmmnds__setpoint)
__weak extern void eoprot_ep_mc_fun_INIT_joint_jcmmnds__setpoint(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_jcmmnds__setpoint)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_jcmmnds__setpoint(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_jcmmnds__stoptrajectory)
__weak extern void eoprot_ep_mc_fun_INIT_joint_jcmmnds__stoptrajectory(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_jcmmnds__stoptrajectory)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_jcmmnds__stoptrajectory(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_jcmmnds__controlmode)
__weak extern void eoprot_ep_mc_fun_INIT_joint_jcmmnds__controlmode(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_jcmmnds__controlmode)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_jcmmnds__controlmode(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif
  

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_mconfig)
__weak extern void eoprot_ep_mc_fun_INIT_motor_mconfig(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_mconfig)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_mconfig(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_mconfig__pidcurrent)
__weak extern void eoprot_ep_mc_fun_INIT_motor_mconfig__pidcurrent(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_mconfig__pidcurrent)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_mconfig__pidcurrent(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_mconfig__maxvelocityofmotor)
__weak extern void eoprot_ep_mc_fun_INIT_motor_mconfig__maxvelocityofmotor(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_mconfig__maxvelocityofmotor)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_mconfig__maxvelocityofmotor(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_mconfig__maxcurrentofmotor)
__weak extern void eoprot_ep_mc_fun_INIT_motor_mconfig__maxcurrentofmotor(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_mconfig__maxcurrentofmotor)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_mconfig__maxcurrentofmotor(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_mconfig__des02FORmstatuschamaleon04)
__weak extern void eoprot_ep_mc_fun_INIT_motor_mconfig__des02FORmstatuschamaleon04(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_mconfig__des02FORmstatuschamaleon04)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_mconfig__des02FORmstatuschamaleon04(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_mstatus)
__weak extern void eoprot_ep_mc_fun_INIT_motor_mstatus(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_mstatus)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_mstatus(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_mstatus__basic)
__weak extern void eoprot_ep_mc_fun_INIT_motor_mstatus__basic(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_mstatus__basic)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_mstatus__basic(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_mstatus__chamaleon04)
__weak extern void eoprot_ep_mc_fun_INIT_motor_mstatus__chamaleon04(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_mstatus__chamaleon04)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_mstatus__chamaleon04(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif   


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_controller_cconfig)
__weak extern void eoprot_ep_mc_fun_INIT_controller_cconfig(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_controller_cconfig)
__weak extern void eoprot_ep_mc_fun_UPDT_controller_cconfig(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_controller_cconfig__durationofctrlloop)
__weak extern void eoprot_ep_mc_fun_INIT_controller_cconfig__durationofctrlloop(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_controller_cconfig__durationofctrlloop)
__weak extern void eoprot_ep_mc_fun_UPDT_controller_cconfig__durationofctrlloop(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_controller_cstatus)
__weak extern void eoprot_ep_mc_fun_INIT_controller_cstatus(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_controller_cstatus)
__weak extern void eoprot_ep_mc_fun_UPDT_controller_cstatus(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_controller_cstatus__alljomoinitted)
__weak extern void eoprot_ep_mc_fun_INIT_controller_cstatus__alljomoinitted(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_controller_cstatus__alljomoinitted)
__weak extern void eoprot_ep_mc_fun_UPDT_controller_cstatus__alljomoinitted(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_controller_ccmmnds__go2stateofcontroller)
__weak extern void eoprot_ep_mc_fun_INIT_controller_ccmmnds__go2stateofcontroller(const EOnv* nv, uint16_t index) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_controller_ccmmnds__go2stateofcontroller)
__weak extern void eoprot_ep_mc_fun_UPDT_controller_ccmmnds__go2stateofcontroller(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index) {}
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

