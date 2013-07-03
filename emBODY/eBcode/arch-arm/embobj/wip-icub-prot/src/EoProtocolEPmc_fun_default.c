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

   
#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_wholeitem)
__weak extern void eoprot_ep_mc_fun_INIT_joint_wholeitem(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_wholeitem)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_wholeitem(const EOnv* nv, const eOropdescriptor_t* rd)  {}
#endif
    
    
#warning --> when debug is over .... remove the initialisation of joint_config
#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_config)
__weak extern void eoprot_ep_mc_fun_INIT_joint_config(const EOnv* nv) // {}
    { memcpy(nv->ram, &eoprot_ep_mc_rom_joint_defaultvalue.config, sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config)); }
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_config)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_config(const EOnv* nv, const eOropdescriptor_t* rd)  {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_config_pidposition)
__weak extern void eoprot_ep_mc_fun_INIT_joint_config_pidposition(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_config_pidposition)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_config_pidposition(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_config_pidvelocity)
__weak extern void eoprot_ep_mc_fun_INIT_joint_config_pidvelocity(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_config_pidvelocity)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_config_pidvelocity(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif
   

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_config_pidtorque)
__weak extern void eoprot_ep_mc_fun_INIT_joint_config_pidtorque(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_config_pidtorque)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_config_pidtorque(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_config_impedance)
__weak extern void eoprot_ep_mc_fun_INIT_joint_config_impedance(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_config_impedance)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_config_impedance(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_config_minpositionofjoint)
__weak extern void eoprot_ep_mc_fun_INIT_joint_config_minpositionofjoint(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_config_minpositionofjoint)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_config_minpositionofjoint(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif
   

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_config_maxpositionofjoint)
__weak extern void eoprot_ep_mc_fun_INIT_joint_config_maxpositionofjoint(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_config_maxpositionofjoint)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_config_maxpositionofjoint(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_config_velocitysetpointtimeout)
__weak extern void eoprot_ep_mc_fun_INIT_joint_config_velocitysetpointtimeout(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_config_velocitysetpointtimeout)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_config_velocitysetpointtimeout(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif
  

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_config_motionmonitormode)
__weak extern void eoprot_ep_mc_fun_INIT_joint_config_motionmonitormode(const EOnv* nv) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_config_motionmonitormode)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_config_motionmonitormode(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_config_encoderconversionfactor)
__weak extern void eoprot_ep_mc_fun_INIT_joint_config_encoderconversionfactor(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_config_encoderconversionfactor)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_config_encoderconversionfactor(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_config_encoderconversionoffset)
__weak extern void eoprot_ep_mc_fun_INIT_joint_config_encoderconversionoffset(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_config_encoderconversionoffset)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_config_encoderconversionoffset(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

  
#warning --> when debug is over .... remove the initialisation of joint_status
#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_status)
__weak extern void eoprot_ep_mc_fun_INIT_joint_status(const EOnv* nv) // {}
    { memcpy(nv->ram, &eoprot_ep_mc_rom_joint_defaultvalue.status, sizeof(eoprot_ep_mc_rom_joint_defaultvalue.status)); }
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_status)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_status(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_status_basic)
__weak extern void eoprot_ep_mc_fun_INIT_joint_status_basic(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_status_basic)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_status_basic(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_status_ofpid)
__weak extern void eoprot_ep_mc_fun_INIT_joint_status_ofpid(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_status_ofpid)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_status_ofpid(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_status_chamaleon04)
__weak extern void eoprot_ep_mc_fun_INIT_joint_status_chamaleon04(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_status_chamaleon04_updt)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_status_chamaleon04(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif



#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_inputs)
__weak extern void eoprot_ep_mc_fun_INIT_joint_inputs(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_inputs)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_inputs(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_inputs_externallymeasuredtorque)
__weak extern void eoprot_ep_mc_fun_INIT_joint_inputs_externallymeasuredtorque(const EOnv* nv) {}
#endif   

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_inputs_externallymeasuredtorque)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_inputs_externallymeasuredtorque(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif



#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_cmmnds_calibration)
__weak extern void eoprot_ep_mc_fun_INIT_joint_cmmnds_calibration(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_cmmnds_calibration_UDPT)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_cmmnds_calibration(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_cmmnds_setpoint)
__weak extern void eoprot_ep_mc_fun_INIT_joint_cmmnds_setpoint(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_cmmnds_setpoint)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_cmmnds_setpoint(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_cmmnds_stoptrajectory)
__weak extern void eoprot_ep_mc_fun_INIT_joint_cmmnds_stoptrajectory(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_cmmnds_stoptrajectory)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_cmmnds_stoptrajectory(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_joint_cmmnds_controlmode)
__weak extern void eoprot_ep_mc_fun_INIT_joint_cmmnds_controlmode(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_joint_cmmnds_controlmode)
__weak extern void eoprot_ep_mc_fun_UPDT_joint_cmmnds_controlmode(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif
  

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_wholeitem)
__weak extern void eoprot_ep_mc_fun_INIT_motor_wholeitem(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_wholeitem)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_wholeitem(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif
    
    
#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_config)
__weak extern void eoprot_ep_mc_fun_INIT_motor_config(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_config)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_config(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_config_pidcurrent)
__weak extern void eoprot_ep_mc_fun_INIT_motor_config_pidcurrent(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_config_pidcurrent)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_config_pidcurrent(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_config_maxvelocityofmotor)
__weak extern void eoprot_ep_mc_fun_INIT_motor_config_maxvelocityofmotor(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_config_maxvelocityofmotor)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_config_maxvelocityofmotor(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_config_maxcurrentofmotor)
__weak extern void eoprot_ep_mc_fun_INIT_motor_config_maxcurrentofmotor(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_config_maxcurrentofmotor)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_config_maxcurrentofmotor(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_status)
__weak extern void eoprot_ep_mc_fun_INIT_motor_status(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_status)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_status(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_status_basic)
__weak extern void eoprot_ep_mc_fun_INIT_motor_status_basic(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_status_basic)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_status_basic(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_motor_status_chamaleon04)
__weak extern void eoprot_ep_mc_fun_INIT_motor_status_chamaleon04(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_motor_status_chamaleon04)
__weak extern void eoprot_ep_mc_fun_UPDT_motor_status_chamaleon04(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif   



#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_controller_wholeitem)
__weak extern void eoprot_ep_mc_fun_INIT_controller_wholeitem(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_controller_wholeitem)
__weak extern void eoprot_ep_mc_fun_UPDT_controller_wholeitem(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif    
    
#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_controller_config)
__weak extern void eoprot_ep_mc_fun_INIT_controller_config(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_controller_config)
__weak extern void eoprot_ep_mc_fun_UPDT_controller_config(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_controller_config_durationofctrlloop)
__weak extern void eoprot_ep_mc_fun_INIT_controller_config_durationofctrlloop(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_controller_config_durationofctrlloop)
__weak extern void eoprot_ep_mc_fun_UPDT_controller_config_durationofctrlloop(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_controller_status)
__weak extern void eoprot_ep_mc_fun_INIT_controller_status(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_controller_status)
__weak extern void eoprot_ep_mc_fun_UPDT_controller_status(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_controller_status_alljomoinitted)
__weak extern void eoprot_ep_mc_fun_INIT_controller_status_alljomoinitted(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_controller_status_alljomoinitted)
__weak extern void eoprot_ep_mc_fun_UPDT_controller_status_alljomoinitted(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_INIT_controller_cmmnds_go2stateofcontroller)
__weak extern void eoprot_ep_mc_fun_INIT_controller_cmmnds_go2stateofcontroller(const EOnv* nv) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_mc_fun_UPDT_controller_cmmnds_go2stateofcontroller)
__weak extern void eoprot_ep_mc_fun_UPDT_controller_cmmnds_go2stateofcontroller(const EOnv* nv, const eOropdescriptor_t* rd) {}
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

