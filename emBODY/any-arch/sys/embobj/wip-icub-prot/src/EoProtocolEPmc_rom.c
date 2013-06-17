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

/* @file       EoProtocolEPmc_rom.c
    @brief      This file keeps ....
    @author     marco.accame@iit.it
    @date       06/06/2013
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EoCommon.h"
#include "EOnv_hid.h"
#include "EOconstvector_hid.h"


#include "EoMotionControl.h"
#include "EoProtocolEPmc.h"
#include "EoProtocolEPmc_fun.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolEPmc_rom.h"


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

static uint16_t s_eoprot_ep_mc_rom_joint_ramoffset(uint16_t tag);
static uint16_t s_eoprot_ep_mc_rom_motor_ramoffset(uint16_t tag);
static uint16_t s_eoprot_ep_mc_rom_controller_ramoffset(uint16_t tag);


static void s_eoprot_ep_mc_rom_joint_INIT_config(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_config(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_config__pidposition(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_config__pidposition(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_config__pidvelocity(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_config__pidvelocity(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_config__pidtorque(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_config__pidtorque(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_config__impedance(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_config__impedance(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_config__minpositionofjoint(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_config__minpositionofjoint(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_config__maxpositionofjoint(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_config__maxpositionofjoint(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_config__velocitysetpointtimeout(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_config__velocitysetpointtimeout(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_config__motionmonitormode(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_config__motionmonitormode(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_config__encoderconversionfactor(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_config__encoderconversionfactor(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_config__encoderconversionoffset(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_config__encoderconversionoffset(const EOnv* nv, const eOropdescriptor_t* rd);


static void s_eoprot_ep_mc_rom_joint_INIT_status(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_status(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_status__basic(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_status__basic(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_status__ofpid(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_status__ofpid(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_status__chamaleon04(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_status__chamaleon04(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_inputs(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_inputs(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_inputs__externallymeasuredtorque(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_inputs__externallymeasuredtorque(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_cmmnds__calibration(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_cmmnds__calibration(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_cmmnds__setpoint(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_cmmnds__setpoint(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_cmmnds__stoptrajectory(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_cmmnds__stoptrajectory(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_cmmnds__controlmode(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_cmmnds__controlmode(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_motor_INIT_config(const EOnv* nv);
static void s_eoprot_ep_mc_rom_motor_UPDT_config(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_motor_INIT_config__pidcurrent(const EOnv* nv);
static void s_eoprot_ep_mc_rom_motor_UPDT_config__pidcurrent(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_motor_INIT_config__maxvelocityofmotor(const EOnv* nv);
static void s_eoprot_ep_mc_rom_motor_UPDT_config__maxvelocityofmotor(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_motor_INIT_config__maxcurrentofmotor(const EOnv* nv);
static void s_eoprot_ep_mc_rom_motor_UPDT_config__maxcurrentofmotor(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_motor_INIT_status(const EOnv* nv);
static void s_eoprot_ep_mc_rom_motor_UPDT_status(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_motor_INIT_status__basic(const EOnv* nv);
static void s_eoprot_ep_mc_rom_motor_UPDT_status__basic(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_motor_INIT_status__chamaleon04(const EOnv* nv);
static void s_eoprot_ep_mc_rom_motor_UPDT_status__chamaleon04(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_controller_INIT_config(const EOnv* nv);
static void s_eoprot_ep_mc_rom_controller_UPDT_config(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_controller_INIT_config__durationofctrlloop(const EOnv* nv);
static void s_eoprot_ep_mc_rom_controller_UPDT_config__durationofctrlloop(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_controller_INIT_status(const EOnv* nv);
static void s_eoprot_ep_mc_rom_controller_UPDT_status(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_controller_INIT_status__alljomoinitted(const EOnv* nv);
static void s_eoprot_ep_mc_rom_controller_UPDT_status__alljomoinitted(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_controller_INIT_cmmnds__go2stateofcontroller(const EOnv* nv);
static void s_eoprot_ep_mc_rom_controller_UPDT_cmmnds__go2stateofcontroller(const EOnv* nv, const eOropdescriptor_t* rd);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// - default value of a joint

const eOmc_joint_t eoprot_ep_mc_rom_joint_defaultvalue =
{
    EO_INIT(.config)             
    {
        EO_INIT(.pidposition)
        {
            EO_INIT(.kp)                    0,
            EO_INIT(.ki)                    0,
            EO_INIT(.kd)                    0,
            EO_INIT(.limitonintegral)       0,
            EO_INIT(.limitonoutput)         0,
            EO_INIT(.offset)                0,
            EO_INIT(.scale)                 0,       
            EO_INIT(.filler03)              {0xf1, 0xf2, 0xf3}
        },
        EO_INIT(.pidvelocity)
        {
            EO_INIT(.kp)                    0,
            EO_INIT(.ki)                    0,
            EO_INIT(.kd)                    0,
            EO_INIT(.limitonintegral)       0,
            EO_INIT(.limitonoutput)         0,
            EO_INIT(.offset)                0,
            EO_INIT(.scale)                 0,       
            EO_INIT(.filler03)              {0xf1, 0xf2, 0xf3}
        },
        EO_INIT(.pidtorque)
        {
            EO_INIT(.kp)                    0,
            EO_INIT(.ki)                    0,
            EO_INIT(.kd)                    0,
            EO_INIT(.limitonintegral)       0,
            EO_INIT(.limitonoutput)         0,
            EO_INIT(.offset)                0,
            EO_INIT(.scale)                 0,       
            EO_INIT(.filler03)              {0xf1, 0xf2, 0xf3}
        }, 
        EO_INIT(.impedance)
        {
            EO_INIT(.stiffness)             0,
            EO_INIT(.damping)               0,
            EO_INIT(.offset)                0,
            EO_INIT(.filler02)              {0xf1, 0xf2}           
        },        
        EO_INIT(.minpositionofjoint)        0,
        EO_INIT(.maxpositionofjoint)        0,
        EO_INIT(.velocitysetpointtimeout)   0,
        EO_INIT(.motionmonitormode)         eomc_motionmonitormode_dontmonitor,
        EO_INIT(.filler01)                  {0xf1},
        EO_INIT(.encoderconversionfactor)   EOUTIL_EMULFLOAT32_ONE,
        EO_INIT(.encoderconversionoffset)   EOUTIL_EMULFLOAT32_ZERO
    },
    EO_INIT(.status)                       
    {
        EO_INIT(.basic)
        {
            EO_INIT(.position)              0,
            EO_INIT(.velocity)              0,       
            EO_INIT(.acceleration)          0,     
            EO_INIT(.torque)                0,
            EO_INIT(.motionmonitorstatus)   eomc_motionmonitorstatus_notmonitored,
            EO_INIT(.controlmodestatus)     eomc_controlmode_idle,
        },
        EO_INIT(.ofpid)                     
        {
            EO_INIT(.reference)             0,
            EO_INIT(.error)                 0,       
            EO_INIT(.output)                0             
        },
        EO_INIT(.chamaleon04)               {0, 0, 0, 0} 
    },
    EO_INIT(.inputs)                       
    {
        EO_INIT(.externallymeasuredtorque)  0,
        EO_INIT(.filler06)                  {0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6} 
    },
    EO_INIT(.cmmnds)                       
    {
        EO_INIT(.calibration)               {0},
        EO_INIT(.setpoint)                  {0},
        EO_INIT(.stoptrajectory)            0,
        EO_INIT(.controlmode)               eomc_controlmode_cmd_switch_everything_off,
        EO_INIT(.filler02)                  {0xf1, 0xf2}
    }
}; 

// - default value of a motor

const eOmc_motor_t eoprot_ep_mc_rom_motor_defaultvalue =
{
    EO_INIT(.config)             
    {
        EO_INIT(.pidcurrent)
        {
            EO_INIT(.kp)                    0,
            EO_INIT(.ki)                    0,
            EO_INIT(.kd)                    0,
            EO_INIT(.limitonintegral)       0,
            EO_INIT(.limitonoutput)         0,
            EO_INIT(.offset)                0,
            EO_INIT(.scale)                 0,            
            EO_INIT(.filler03)              {0xf1, 0xf2, 0xf3}
        },
        EO_INIT(.maxvelocityofmotor)        0,
        EO_INIT(.maxcurrentofmotor)         0,
        EO_INIT(.filler02)                  {0xf1, 0xf2}
    },
    EO_INIT(.status)                       
    {
        EO_INIT(.basic) 
        {
            EO_INIT(.position)              0,
            EO_INIT(.velocity)              0,
            EO_INIT(.current)               0       
        },
        EO_INIT(.chamaleon04)               {0, 0, 0, 0}
    }
};  

// - default value of a controller

const eOmc_controller_t eoprot_ep_mc_rom_controller_defaultvalue =
{
    EO_INIT(.config)             
    {
        EO_INIT(.durationofctrlloop)        EOK_reltime1ms,
        EO_INIT(.filler04)                  {0xf1, 0xf2, 0xf3, 0xf4}
    },
    EO_INIT(.status)                      
    {
        EO_INIT(.alljomoinitted)            eobool_false,
        EO_INIT(.numofjoints)               EOK_uint08dummy,
        EO_INIT(.numofmotors)               EOK_uint08dummy,
        EO_INIT(.stateofcontroller)         eomc_stateofcontroller_config,
        EO_INIT(.flagsinittedjoints)        0x0000,
        EO_INIT(.flagsinittedmotors)        0x0000   
    },
    EO_INIT(.cmmnds)                      
    {
        EO_INIT(.go2stateofcontroller)      eomc_stateofcontroller_config,
        EO_INIT(.filler07)                  {0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7}   
    }    
};


// - descriptors for the variables of a joint

EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_config =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.config,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_config,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_config
};


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_config__pidposition =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config.pidposition),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_config__pidposition,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.config.pidposition,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_config__pidposition,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_config__pidposition
};


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_config__pidvelocity =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config.pidvelocity),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_config__pidvelocity,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.config.pidvelocity,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_config__pidvelocity,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_config__pidvelocity
};


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_config__pidtorque =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config.pidtorque),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_config__pidtorque,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.config.pidtorque,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_config__pidtorque,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_config__pidtorque
};


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_config__impedance =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config.impedance),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_config__impedance,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.config.impedance,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_config__impedance,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_config__impedance
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_config__minpositionofjoint =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config.minpositionofjoint),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_config__minpositionofjoint,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.config.minpositionofjoint,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_config__minpositionofjoint,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_config__minpositionofjoint
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_config__maxpositionofjoint =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config.maxpositionofjoint),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_config__maxpositionofjoint,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.config.maxpositionofjoint,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_config__maxpositionofjoint,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_config__maxpositionofjoint
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_config__velocitysetpointtimeout =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config.velocitysetpointtimeout),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_config__velocitysetpointtimeout,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.config.velocitysetpointtimeout,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_config__velocitysetpointtimeout,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_config__velocitysetpointtimeout
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_config__motionmonitormode =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config.motionmonitormode),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_config__motionmonitormode,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.config.motionmonitormode,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_config__motionmonitormode,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_config__motionmonitormode
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_config__encoderconversionfactor =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config.encoderconversionfactor),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_config__encoderconversionfactor,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.config.encoderconversionfactor,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_config__encoderconversionfactor,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_config__encoderconversionfactor
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_config__encoderconversionoffset =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config.encoderconversionoffset),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_config__encoderconversionoffset,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.config.encoderconversionoffset,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_config__encoderconversionoffset,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_config__encoderconversionoffset
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_status =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.status),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_status,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.status,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_status,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_status
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_status__basic =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.status.basic),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_status__basic,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.status.basic,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_status__basic,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_status__basic
};


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_status__ofpid =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.status.ofpid),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_status__ofpid,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.status.ofpid,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_status__ofpid,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_status__ofpid
};


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_status__chamaleon04 =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.status.chamaleon04),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_status__chamaleon04,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.status.chamaleon04,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_status__chamaleon04,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_status__chamaleon04
};


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_inputs =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.inputs),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_inputs,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.inputs,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_inputs,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_inputs
};


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_inputs__externallymeasuredtorque =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.inputs.externallymeasuredtorque),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_inputs__externallymeasuredtorque,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.inputs.externallymeasuredtorque,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_inputs__externallymeasuredtorque,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_inputs__externallymeasuredtorque
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_cmmnds__calibration =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.cmmnds.calibration),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_cmmnds__calibration,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.cmmnds.calibration,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_cmmnds__calibration,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_cmmnds__calibration
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_cmmnds__setpoint =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.cmmnds.setpoint),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_cmmnds__setpoint,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.cmmnds.setpoint,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_cmmnds__setpoint,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_cmmnds__setpoint
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_cmmnds__stoptrajectory =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.cmmnds.stoptrajectory),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_cmmnds__stoptrajectory,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.cmmnds.stoptrajectory,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_cmmnds__stoptrajectory,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_cmmnds__stoptrajectory
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_cmmnds__controlmode =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.cmmnds.controlmode),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_cmmnds__controlmode,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.cmmnds.controlmode,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_cmmnds__controlmode,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_cmmnds__controlmode
}; 



// - descriptors for the variables of a motor 

EOnv_rom_t eoprot_ep_mc_rom_motor_descriptor_config =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_motor_defaultvalue.config),
    EO_INIT(.funtyp)    eoprot_ep_mc_motor_funtyp_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_motor_defaultvalue.config,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_motor_INIT_config,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_motor_UPDT_config
};


EOnv_rom_t eoprot_ep_mc_rom_motor_descriptor_config__pidcurrent =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_motor_defaultvalue.config.pidcurrent),
    EO_INIT(.funtyp)    eoprot_ep_mc_motor_funtyp_config__pidcurrent,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_motor_defaultvalue.config.pidcurrent,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_motor_INIT_config__pidcurrent,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_motor_UPDT_config__pidcurrent
};


EOnv_rom_t eoprot_ep_mc_rom_motor_descriptor_config__maxvelocityofmotor =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_motor_defaultvalue.config.maxvelocityofmotor),
    EO_INIT(.funtyp)    eoprot_ep_mc_motor_funtyp_config__maxvelocityofmotor,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_motor_defaultvalue.config.maxvelocityofmotor,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_motor_INIT_config__maxvelocityofmotor,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_motor_UPDT_config__maxvelocityofmotor
};


EOnv_rom_t eoprot_ep_mc_rom_motor_descriptor_config__maxcurrentofmotor =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_motor_defaultvalue.config.maxcurrentofmotor),
    EO_INIT(.funtyp)    eoprot_ep_mc_motor_funtyp_config__maxcurrentofmotor,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_motor_defaultvalue.config.maxcurrentofmotor,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_motor_INIT_config__maxcurrentofmotor,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_motor_UPDT_config__maxcurrentofmotor
};



EOnv_rom_t eoprot_ep_mc_rom_motor_descriptor_status =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_motor_defaultvalue.status),
    EO_INIT(.funtyp)    eoprot_ep_mc_motor_funtyp_status,
    EO_INIT(.dummy)     0,   
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_motor_defaultvalue.status,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_motor_INIT_status,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_motor_UPDT_status
};


EOnv_rom_t eoprot_ep_mc_rom_motor_descriptor_status__basic =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_motor_defaultvalue.status.basic),
    EO_INIT(.funtyp)    eoprot_ep_mc_motor_funtyp_status__basic,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_motor_defaultvalue.status.basic,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_motor_INIT_status__basic,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_motor_UPDT_status__basic
};

EOnv_rom_t eoprot_ep_mc_rom_motor_descriptor_status__chamaleon04 =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_motor_defaultvalue.status.chamaleon04),
    EO_INIT(.funtyp)    eoprot_ep_mc_motor_funtyp_status__chamaleon04,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_motor_defaultvalue.status.chamaleon04,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_motor_INIT_status__chamaleon04,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_motor_UPDT_status__chamaleon04
};


// - descriptors for the variables of a controller


EOnv_rom_t eoprot_ep_mc_rom_controller_descriptor_config =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_controller_defaultvalue.config),
    EO_INIT(.funtyp)    eoprot_ep_mc_controller_funtyp_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_controller_defaultvalue.config,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_controller_INIT_config,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_controller_UPDT_config
};


EOnv_rom_t eoprot_ep_mc_rom_controller_descriptor_config__durationofctrlloop =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_controller_defaultvalue.config.durationofctrlloop),
    EO_INIT(.funtyp)    eoprot_ep_mc_controller_funtyp_config__durationofctrlloop,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_controller_defaultvalue.config.durationofctrlloop,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_controller_INIT_config__durationofctrlloop,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_controller_UPDT_config__durationofctrlloop
};


EOnv_rom_t eoprot_ep_mc_rom_controller_descriptor_status =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_controller_defaultvalue.status),
    EO_INIT(.funtyp)    eoprot_ep_mc_controller_funtyp_status,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_controller_defaultvalue.status,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_controller_INIT_status,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_controller_UPDT_status
};


EOnv_rom_t eoprot_ep_mc_rom_controller_descriptor_status__alljomoinitted =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_controller_defaultvalue.status.alljomoinitted),
    EO_INIT(.funtyp)    eoprot_ep_mc_controller_funtyp_status__alljomoinitted,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_controller_defaultvalue.status.alljomoinitted,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_controller_INIT_status__alljomoinitted,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_controller_UPDT_status__alljomoinitted
};


EOnv_rom_t eoprot_ep_mc_rom_controller_descriptor_cmmnds__go2stateofcontroller =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_controller_defaultvalue.cmmnds.go2stateofcontroller),
    EO_INIT(.funtyp)    eoprot_ep_mc_controller_funtyp_cmmnds__go2stateofcontroller,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_controller_defaultvalue.cmmnds.go2stateofcontroller,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_controller_INIT_cmmnds__go2stateofcontroller,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_controller_UPDT_cmmnds__go2stateofcontroller
};




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


const EOnv_rom_t * const eoprot_ep_mc_rom_folded_descriptors[] =
{
    // here are eoprot_ep_mc_joint_tags_numberof descriptors for the joints (equal for every joint)
    &eoprot_ep_mc_rom_joint_descriptor_config,
    &eoprot_ep_mc_rom_joint_descriptor_config__pidposition,
    &eoprot_ep_mc_rom_joint_descriptor_config__pidvelocity,
    &eoprot_ep_mc_rom_joint_descriptor_config__pidtorque,
    &eoprot_ep_mc_rom_joint_descriptor_config__impedance,
    &eoprot_ep_mc_rom_joint_descriptor_config__minpositionofjoint,
    &eoprot_ep_mc_rom_joint_descriptor_config__maxpositionofjoint,
    &eoprot_ep_mc_rom_joint_descriptor_config__velocitysetpointtimeout,
    &eoprot_ep_mc_rom_joint_descriptor_config__motionmonitormode,
    &eoprot_ep_mc_rom_joint_descriptor_config__encoderconversionfactor,
    &eoprot_ep_mc_rom_joint_descriptor_config__encoderconversionoffset,
    &eoprot_ep_mc_rom_joint_descriptor_status,
    &eoprot_ep_mc_rom_joint_descriptor_status__basic,
    &eoprot_ep_mc_rom_joint_descriptor_status__ofpid,
    &eoprot_ep_mc_rom_joint_descriptor_status__chamaleon04,
    &eoprot_ep_mc_rom_joint_descriptor_inputs,
    &eoprot_ep_mc_rom_joint_descriptor_inputs__externallymeasuredtorque,
    &eoprot_ep_mc_rom_joint_descriptor_cmmnds__calibration,
    &eoprot_ep_mc_rom_joint_descriptor_cmmnds__setpoint,
    &eoprot_ep_mc_rom_joint_descriptor_cmmnds__stoptrajectory,
    &eoprot_ep_mc_rom_joint_descriptor_cmmnds__controlmode,
    
    // here are eoprot_ep_mc_motor_tags_numberof descriptors for the motors (equal for every motor)
    &eoprot_ep_mc_rom_motor_descriptor_config,
    &eoprot_ep_mc_rom_motor_descriptor_config__pidcurrent,
    &eoprot_ep_mc_rom_motor_descriptor_config__maxvelocityofmotor,
    &eoprot_ep_mc_rom_motor_descriptor_config__maxcurrentofmotor,
    &eoprot_ep_mc_rom_motor_descriptor_status,
    &eoprot_ep_mc_rom_motor_descriptor_status__basic,
    &eoprot_ep_mc_rom_motor_descriptor_status__chamaleon04,

    // here are eoprot_ep_mc_controller_tags_numberof descriptors for the single controller
    &eoprot_ep_mc_rom_controller_descriptor_config,
    &eoprot_ep_mc_rom_controller_descriptor_config__durationofctrlloop,
    &eoprot_ep_mc_rom_controller_descriptor_status,
    &eoprot_ep_mc_rom_controller_descriptor_status__alljomoinitted,
    &eoprot_ep_mc_rom_controller_descriptor_cmmnds__go2stateofcontroller
         
};  EO_VERIFYsizeof(eoprot_ep_mc_rom_folded_descriptors, sizeof(EOnv_rom_t*)*(eoprot_ep_mc_joint_tags_numberof+eoprot_ep_mc_motor_tags_numberof+eoprot_ep_mc_controller_tags_numberof));





const EOconstvector  eoprot_ep_mc_rom_constvector_of_folded_descriptors_dat = 
{
    EO_INIT(.size)              sizeof(eoprot_ep_mc_rom_folded_descriptors)/sizeof(EOnv_rom_t*), 
    EO_INIT(.item_size)         sizeof(EOnv_rom_t*),
    EO_INIT(.item_array_data)   eoprot_ep_mc_rom_folded_descriptors
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

//const void* eoprot_ep_mc_rom_joint_offsets[] = 
//{   // in position i-th it contains the offset from start of eOmc_joint_t of data related to nvindex with value i 
//    0,
//    4,
//}; EO_VERIFYsizeof(eoprot_ep_mc_rom_joint_offsets, sizeof(uint16_t)*(jointNVs_CURRENTnumberof));

//const uint16_t eoprot_ep_mc_rom_motor_offsets[] = 
//{   // in position i-th it contains the offset from start of eOmc_motor_t of data related to nvindex with value i 
//    0,
//    4,
//}; EO_VERIFYsizeof(eoprot_ep_mc_rom_motor_offsets, sizeof(uint16_t)*(motorNVs_CURRENTnumberof));

//const uint16_t eoprot_ep_mc_rom_controller_offsets[] = 
//{   // in position i-th it contains the offset from start of eOmc_controller_t of data related to nvindex with value i 
//    0,
//}; EO_VERIFYsizeof(eoprot_ep_mc_rom_controller_offsets, sizeof(uint16_t)*(controllerNVs_CURRENTnumberof));


extern uint16_t eoprot_ep_mc_rom_epid2ramoffset(eOprotEP_t ep, eOprotID_t id)
{
    uint16_t offset = 0;
    uint16_t tag = eoprot_ep_variable_ID2tag(ep, id);
    
    if(EOK_uint16dummy == tag)
    {
        return(EOK_uint16dummy);
    }
    
    eOprotEntity_t entity = eoprot_ep_variable_ID2entity(ep, id);
    uint16_t index = eoprot_ep_variable_ID2index(ep, id);
    
    switch(entity)
    {
        case eomc_entity_joint:
        {   // the joints are all displaced at the beginning of the data in every mc endpoint
            offset = index*sizeof(eOmc_joint_t) + s_eoprot_ep_mc_rom_joint_ramoffset(tag); 
        } break;
        
        case eomc_entity_motor:
        {   // the motors are placed after all the joints. the index of the joints depends on teh endpoint
            uint16_t numberofjoints = eoprot_ep_mc_joints_numberof_Get(ep);
            offset = numberofjoints*sizeof(eOmc_joint_t) + index*sizeof(eOmc_motor_t) + s_eoprot_ep_mc_rom_motor_ramoffset(tag);  
        } break;      

        case eomc_entity_controller:
        {   // the controller is placed after all joints and motors. their index depends on the endpoint
            uint16_t numberofjoints = eoprot_ep_mc_joints_numberof_Get(ep);
            uint16_t numberofmotors = eoprot_ep_mc_motors_numberof_Get(ep);
            offset = numberofjoints*sizeof(eOmc_joint_t) + numberofmotors*sizeof(eOmc_motor_t) + index*sizeof(eOmc_controller_t) + s_eoprot_ep_mc_rom_controller_ramoffset(tag);
        } break; 
        
        default:
        {   // error
            offset = EOK_uint16dummy;
        } break;
    
    }
    
    return(offset);  
}

extern void* eoprot_ep_mc_rom_dataepid2nvram(void* data, eOprotEP_t ep, eOprotID_t id)
{
    uint8_t* startofdata = data;
    uint16_t offset = eoprot_ep_mc_rom_epid2ramoffset(ep, id);
    
    if(EOK_uint16dummy == offset)
    {
        return(NULL);
    }   

    return(&startofdata[offset]);
}


extern uint16_t eoprot_ep_mc_rom_epid2index_of_folded_descriptors(eOprotEP_t ep, eOprotID_t id)
{      
    uint16_t tag = eoprot_ep_variable_ID2tag(ep, id);
    
    if(EOK_uint16dummy == tag)
    {
        return(EOK_uint16dummy);
    }
    
    eOprotEntity_t entity = eoprot_ep_variable_ID2entity(ep, id);
    
    switch(entity)
    {
        case eomc_entity_joint:
        {   // nothing to add as the joint vars are in first position
            ; 
        } break;
        
        case eomc_entity_motor:
        {   // must add the number of vars in a joint
            tag += eoprot_ep_mc_joint_tags_numberof; 
        } break;      

        case eomc_entity_controller:
        {   // must add the number of vars in a joint and in a motor
            tag += eoprot_ep_mc_joint_tags_numberof; 
            tag += eoprot_ep_mc_motor_tags_numberof;
        } break; 
        
        default:
        {   // error
            tag = EOK_uint16dummy;
        } break;
    
    }
    
    return(tag);   
}

extern void* eoprot_ep_mc_rom_epid2nvrom(eOprotEP_t ep, eOprotID_t id)
{
    uint16_t indexoffoldeddescriptors = eoprot_ep_mc_rom_epid2index_of_folded_descriptors(ep, id);
    
    if(EOK_uint16dummy == indexoffoldeddescriptors)
    {
        return(NULL);
    }
    
    return((void*)&eoprot_ep_mc_rom_folded_descriptors[indexoffoldeddescriptors]);   
}

extern uint16_t eoprot_ep_mc_rom_epid2progressivenumber(eOprotEP_t ep, eOprotID_t id)
{     
    uint16_t tag = eoprot_ep_variable_ID2tag(ep, id);
    uint16_t index = eoprot_ep_variable_ID2index(ep, id);
    eOprotEntity_t entity = eoprot_ep_variable_ID2entity(ep, id);
    uint16_t prognum = 0;
    
    if((EOK_uint16dummy == tag) || (EOK_uint16dummy == index) || (EOK_uint16dummy == entity))
    {
        return(EOK_uint16dummy);
    }
    
      
    switch(entity)
    {
        case eomc_entity_joint:
        {   // the joints are the first ones   
            prognum = index*eoprot_ep_mc_joint_tags_numberof + tag;
        } break;
        
        case eomc_entity_motor:
        {   // the motors are after all joints
            prognum = eoprot_ep_mc_joints_numberof_Get(ep)*eoprot_ep_mc_joint_tags_numberof + 
                      index*eoprot_ep_mc_motor_tags_numberof + tag; 
        } break;      

        case eomc_entity_controller:
        {   // the controller is after all the joints and all the motors
            prognum = eoprot_ep_mc_joints_numberof_Get(ep)*eoprot_ep_mc_joint_tags_numberof + 
                      eoprot_ep_mc_motors_numberof_Get(ep)*eoprot_ep_mc_motor_tags_numberof +
                      index*eoprot_ep_mc_controller_tags_numberof + tag;
        } break; 
        
        default:
        {   // error
            prognum = EOK_uint16dummy;
        } break;
    
    }
    
    return(prognum); 
}

extern uint16_t eoprot_ep_mc_rom_ep2variablesnumberof(eOprotEP_t ep)
{     
    uint16_t total = eoprot_ep_mc_joints_numberof_Get(ep)*eoprot_ep_mc_joint_tags_numberof + 
                     eoprot_ep_mc_motors_numberof_Get(ep)*eoprot_ep_mc_motor_tags_numberof +
                     eoprot_ep_mc_controllers_numberof_Get(ep)*eoprot_ep_mc_controller_tags_numberof;
    
    return(total); 
}

extern uint16_t eoprot_ep_mc_rom_epid2sizeofvar(eOprotEP_t ep, eOprotID_t id)
{     
    EOnv_rom_t* rom = eoprot_ep_mc_rom_epid2nvrom(ep, id);  
    if(NULL == rom)
    {
        return(0);
    }    
    return(rom->capacity); 
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

// returns the offset form the start of the struct eOmc_joint_t of the variable with a given tag 
static uint16_t s_eoprot_ep_mc_rom_joint_ramoffset(uint16_t tag)
{   
    //return(eoprot_ep_mc_rom_joint_offsets[tag]);
    uint32_t tmp = ((uint32_t) eoprot_ep_mc_rom_folded_descriptors[tag]->resetval) - ((uint32_t) &eoprot_ep_mc_rom_joint_defaultvalue);
    return((uint16_t)tmp); 
}

// returns the offset form the start of the struct eOmc_motor_t of the variable with a given tag 
static uint16_t s_eoprot_ep_mc_rom_motor_ramoffset(uint16_t tag)
{
    //return(eoprot_ep_mc_rom_motor_offsets[tag]);
    uint32_t tmp = ((uint32_t) eoprot_ep_mc_rom_folded_descriptors[tag]->resetval) - ((uint32_t) &eoprot_ep_mc_rom_motor_defaultvalue);
    return((uint16_t)tmp); 
}

// returns the offset form the start of the struct eOmc_controller_t of the variable with a given tag 
static uint16_t s_eoprot_ep_mc_rom_controller_ramoffset(uint16_t tag)
{
    //return(eoprot_ep_mc_rom_controller_offsets[tag]);
    uint32_t tmp = ((uint32_t) eoprot_ep_mc_rom_folded_descriptors[tag]->resetval) - ((uint32_t) &eoprot_ep_mc_rom_controller_defaultvalue);
    return((uint16_t)tmp); 
}


// - the functions called by the descriptors

static void s_eoprot_ep_mc_rom_joint_INIT_config(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_config(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_config(nv, rd, index);
}


static void s_eoprot_ep_mc_rom_joint_INIT_config__pidposition(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_config__pidposition(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_config__pidposition(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_config__pidposition(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_config__pidvelocity(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_config__pidvelocity(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_config__pidvelocity(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_config__pidvelocity(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_config__pidtorque(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_config__pidtorque(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_config__pidtorque(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_config__pidtorque(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_config__impedance(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_config__impedance(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_config__impedance(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_config__impedance(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_config__minpositionofjoint(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_config__minpositionofjoint(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_config__minpositionofjoint(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_config__minpositionofjoint(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_config__maxpositionofjoint(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_config__maxpositionofjoint(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_config__maxpositionofjoint(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_config__maxpositionofjoint(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_config__velocitysetpointtimeout(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_config__velocitysetpointtimeout(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_config__velocitysetpointtimeout(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_config__velocitysetpointtimeout(nv, rd, index);
}


static void s_eoprot_ep_mc_rom_joint_INIT_config__motionmonitormode(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_config__motionmonitormode(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_config__motionmonitormode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_config__motionmonitormode(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_config__encoderconversionfactor(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_config__encoderconversionfactor(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_config__encoderconversionfactor(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_config__encoderconversionfactor(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_config__encoderconversionoffset(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_config__encoderconversionoffset(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_config__encoderconversionoffset(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_config__encoderconversionoffset(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_status(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_status(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_status(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_status(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_status__basic(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_status__basic(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_status__basic(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_status__basic(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_status__ofpid(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_status__ofpid(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_status__ofpid(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_status__ofpid(nv, rd, index);
}


static void s_eoprot_ep_mc_rom_joint_INIT_status__chamaleon04(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_status__chamaleon04(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_status__chamaleon04(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_status__chamaleon04(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_inputs(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_inputs(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_inputs(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_inputs(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_inputs__externallymeasuredtorque(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_inputs__externallymeasuredtorque(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_inputs__externallymeasuredtorque(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_inputs__externallymeasuredtorque(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_cmmnds__calibration(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_cmmnds__calibration(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_cmmnds__calibration(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_cmmnds__calibration(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_cmmnds__setpoint(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_cmmnds__setpoint(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_cmmnds__setpoint(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_cmmnds__setpoint(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_cmmnds__stoptrajectory(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_cmmnds__stoptrajectory(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_cmmnds__stoptrajectory(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_cmmnds__stoptrajectory(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_cmmnds__controlmode(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_cmmnds__controlmode(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_cmmnds__controlmode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_cmmnds__controlmode(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_motor_INIT_config(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_motor_config(nv, index);
}
static void s_eoprot_ep_mc_rom_motor_UPDT_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_motor_config(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_motor_INIT_config__pidcurrent(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_motor_config__pidcurrent(nv, index);
}
static void s_eoprot_ep_mc_rom_motor_UPDT_config__pidcurrent(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_motor_config__pidcurrent(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_motor_INIT_config__maxvelocityofmotor(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_motor_config__maxvelocityofmotor(nv, index);
}
static void s_eoprot_ep_mc_rom_motor_UPDT_config__maxvelocityofmotor(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_motor_config__maxvelocityofmotor(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_motor_INIT_config__maxcurrentofmotor(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_motor_config__maxcurrentofmotor(nv, index);
}
static void s_eoprot_ep_mc_rom_motor_UPDT_config__maxcurrentofmotor(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_motor_config__maxcurrentofmotor(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_motor_INIT_status(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_motor_status(nv, index);
}
static void s_eoprot_ep_mc_rom_motor_UPDT_status(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_motor_status(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_motor_INIT_status__basic(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_motor_status__basic(nv, index);
}
static void s_eoprot_ep_mc_rom_motor_UPDT_status__basic(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_motor_status__basic(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_motor_INIT_status__chamaleon04(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_motor_status__chamaleon04(nv, index);
}
static void s_eoprot_ep_mc_rom_motor_UPDT_status__chamaleon04(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_motor_status__chamaleon04(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_controller_INIT_config(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_controller_config(nv, index);
}
static void s_eoprot_ep_mc_rom_controller_UPDT_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_controller_config(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_controller_INIT_config__durationofctrlloop(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_controller_config__durationofctrlloop(nv, index);
}
static void s_eoprot_ep_mc_rom_controller_UPDT_config__durationofctrlloop(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_controller_config__durationofctrlloop(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_controller_INIT_status(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_controller_status(nv, index);
}
static void s_eoprot_ep_mc_rom_controller_UPDT_status(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_controller_status(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_controller_INIT_status__alljomoinitted(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_controller_status__alljomoinitted(nv, index);
}
static void s_eoprot_ep_mc_rom_controller_UPDT_status__alljomoinitted(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_controller_status__alljomoinitted(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_controller_INIT_cmmnds__go2stateofcontroller(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_controller_cmmnds__go2stateofcontroller(nv, index);
}
static void s_eoprot_ep_mc_rom_controller_UPDT_cmmnds__go2stateofcontroller(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_controller_cmmnds__go2stateofcontroller(nv, rd, index);
}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



