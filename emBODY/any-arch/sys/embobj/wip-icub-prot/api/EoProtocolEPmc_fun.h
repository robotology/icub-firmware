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

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EOPROTOCOLEPMC_FUN_H_
#define _EOPROTOCOLEPMC_FUN_H_




/** @file       EoProtocolEPmc_fun.h
	@brief      This header file contains the declarations of the functions which can be ovverridden in the management of motion control 
	@author     marco.accame@iit.it
	@date       05/29/2013
**/

/** @defgroup eo_asfdgr12345 Functions used in motion control endpoint
    Here are the functions used in motion control endpoint. They can be overridden as they are weakly defined in the .c file.
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EOnv.h"
#include "EOrop.h"


 

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

// all of them are weakly-defined or/and overridable by means of a specific include file

extern void eoprot_ep_mc_fun_INITIALISE(eOnvEP_t ep, void *loc, void *rem);


extern void eoprot_ep_mc_fun_INIT_joint_jconfig(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_joint_jconfig(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_joint_jconfig__pidposition(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_joint_jconfig__pidposition(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_joint_jconfig__pidvelocity(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_joint_jconfig__pidvelocity(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);
   
extern void eoprot_ep_mc_fun_INIT_joint_jconfig__pidtorque(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_joint_jconfig__pidtorque(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_joint_jconfig__impedance(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_joint_jconfig__impedance(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_joint_jconfig__minpositionofjoint(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_joint_jconfig__minpositionofjoint(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);
   
extern void eoprot_ep_mc_fun_INIT_joint_jconfig__maxpositionofjoint(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_joint_jconfig__maxpositionofjoint(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_joint_jconfig__velocitysetpointtimeout(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_joint_jconfig__velocitysetpointtimeout(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_joint_jconfig__motionmonitormode(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_joint_jconfig__motionmonitormode(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_joint_jconfig__encoderconversionfactor(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_joint_jconfig__encoderconversionfactor(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_joint_jconfig__encoderconversionoffset(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_joint_jconfig__encoderconversionoffset(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_joint_jconfig__des02FORjstatuschamaleon04(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_joint_jconfig__des02FORjstatuschamaleon04(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);


extern void eoprot_ep_mc_fun_INIT_joint_jstatus(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_joint_jstatus(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_joint_jstatus__basic(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_joint_jstatus__basic(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_joint_jstatus__ofpid(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_joint_jstatus__ofpid(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_joint_jstatus__chamaleon04(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_joint_jstatus__chamaleon04(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);


extern void eoprot_ep_mc_fun_INIT_joint_jinputs(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_joint_jinputs(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_joint_jinputs__externallymeasuredtorque(const EOnv* nv, uint16_t index);   
extern void eoprot_ep_mc_fun_UPDT_joint_jinputs__externallymeasuredtorque(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);


extern void eoprot_ep_mc_fun_INIT_joint_jcmmnds__calibration(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_joint_jcmmnds__calibration(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_joint_jcmmnds__setpoint(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_joint_jcmmnds__setpoint(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_joint_jcmmnds__stoptrajectory(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_joint_jcmmnds__stoptrajectory(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_joint_jcmmnds__controlmode(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_joint_jcmmnds__controlmode(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);


extern void eoprot_ep_mc_fun_INIT_motor_mconfig(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_motor_mconfig(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_motor_mconfig__pidcurrent(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_motor_mconfig__pidcurrent(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_motor_mconfig__maxvelocityofmotor(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_motor_mconfig__maxvelocityofmotor(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_motor_mconfig__maxcurrentofmotor(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_motor_mconfig__maxcurrentofmotor(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_motor_mconfig__des02FORmstatuschamaleon04(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_motor_mconfig__des02FORmstatuschamaleon04(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_motor_mstatus(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_motor_mstatus(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_motor_mstatus__basic(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_motor_mstatus__basic(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_motor_mstatus__chamaleon04(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_motor_mstatus__chamaleon04(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);
   
extern void eoprot_ep_mc_fun_INIT_controller_cconfig(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_controller_cconfig(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_controller_cconfig__durationofctrlloop(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_controller_cconfig__durationofctrlloop(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_controller_cstatus(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_controller_cstatus(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_controller_cstatus__alljomoinitted(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_controller_cstatus__alljomoinitted(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);

extern void eoprot_ep_mc_fun_INIT_controller_ccmmnds__go2stateofcontroller(const EOnv* nv, uint16_t index);
extern void eoprot_ep_mc_fun_UPDT_controller_ccmmnds__go2stateofcontroller(const EOnv* nv, const eOropdescriptor_t* rd, uint16_t index);



/** @}            
    end of group eo_asfdgr12345  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

