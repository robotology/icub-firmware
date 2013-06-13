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


static void s_eoprot_ep_mc_rom_joint_INIT_jconfig(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_jconfig(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_jconfig__pidposition(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_jconfig__pidposition(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_jconfig__pidvelocity(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_jconfig__pidvelocity(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_jconfig__pidtorque(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_jconfig__pidtorque(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_jconfig__impedance(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_jconfig__impedance(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_jconfig__minpositionofjoint(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_jconfig__minpositionofjoint(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_jconfig__maxpositionofjoint(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_jconfig__maxpositionofjoint(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_jconfig__velocitysetpointtimeout(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_jconfig__velocitysetpointtimeout(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_jconfig__motionmonitormode(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_jconfig__motionmonitormode(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_jconfig__encoderconversionfactor(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_jconfig__encoderconversionfactor(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_jconfig__encoderconversionoffset(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_jconfig__encoderconversionoffset(const EOnv* nv, const eOropdescriptor_t* rd);


static void s_eoprot_ep_mc_rom_joint_INIT_jstatus(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_jstatus(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_jstatus__basic(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_jstatus__basic(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_jstatus__ofpid(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_jstatus__ofpid(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_jstatus__chamaleon04(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_jstatus__chamaleon04(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_jinputs(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_jinputs(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_jinputs__externallymeasuredtorque(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_jinputs__externallymeasuredtorque(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_jcmmnds__calibration(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_jcmmnds__calibration(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_jcmmnds__setpoint(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_jcmmnds__setpoint(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_jcmmnds__stoptrajectory(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_jcmmnds__stoptrajectory(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_joint_INIT_jcmmnds__controlmode(const EOnv* nv);
static void s_eoprot_ep_mc_rom_joint_UPDT_jcmmnds__controlmode(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_motor_INIT_mconfig(const EOnv* nv);
static void s_eoprot_ep_mc_rom_motor_UPDT_mconfig(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_motor_INIT_mconfig__pidcurrent(const EOnv* nv);
static void s_eoprot_ep_mc_rom_motor_UPDT_mconfig__pidcurrent(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_motor_INIT_mconfig__maxvelocityofmotor(const EOnv* nv);
static void s_eoprot_ep_mc_rom_motor_UPDT_mconfig__maxvelocityofmotor(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_motor_INIT_mconfig__maxcurrentofmotor(const EOnv* nv);
static void s_eoprot_ep_mc_rom_motor_UPDT_mconfig__maxcurrentofmotor(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_motor_INIT_mstatus(const EOnv* nv);
static void s_eoprot_ep_mc_rom_motor_UPDT_mstatus(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_motor_INIT_mstatus__basic(const EOnv* nv);
static void s_eoprot_ep_mc_rom_motor_UPDT_mstatus__basic(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_motor_INIT_mstatus__chamaleon04(const EOnv* nv);
static void s_eoprot_ep_mc_rom_motor_UPDT_mstatus__chamaleon04(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_controller_INIT_cconfig(const EOnv* nv);
static void s_eoprot_ep_mc_rom_controller_UPDT_cconfig(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_controller_INIT_cconfig__durationofctrlloop(const EOnv* nv);
static void s_eoprot_ep_mc_rom_controller_UPDT_cconfig__durationofctrlloop(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_controller_INIT_cstatus(const EOnv* nv);
static void s_eoprot_ep_mc_rom_controller_UPDT_cstatus(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_controller_INIT_cstatus__alljomoinitted(const EOnv* nv);
static void s_eoprot_ep_mc_rom_controller_UPDT_cstatus__alljomoinitted(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mc_rom_controller_INIT_ccmmnds__go2stateofcontroller(const EOnv* nv);
static void s_eoprot_ep_mc_rom_controller_UPDT_ccmmnds__go2stateofcontroller(const EOnv* nv, const eOropdescriptor_t* rd);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// - default value of a joint

const eOmc_joint_t eoprot_ep_mc_rom_joint_defaultvalue =
{
    EO_INIT(.jconfig)             
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
    EO_INIT(.jstatus)                       
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
    EO_INIT(.jinputs)                       
    {
        EO_INIT(.externallymeasuredtorque)  0,
        EO_INIT(.filler06)                  {0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6} 
    },
    EO_INIT(.jcmmnds)                       
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
    EO_INIT(.mconfig)             
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
    EO_INIT(.mstatus)                       
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
    EO_INIT(.cconfig)             
    {
        EO_INIT(.durationofctrlloop)        EOK_reltime1ms,
        EO_INIT(.filler04)                  {0xf1, 0xf2, 0xf3, 0xf4}
    },
    EO_INIT(.cstatus)                      
    {
        EO_INIT(.alljomoinitted)            eobool_false,
        EO_INIT(.numofjoints)               EOK_uint08dummy,
        EO_INIT(.numofmotors)               EOK_uint08dummy,
        EO_INIT(.stateofcontroller)         eomc_stateofcontroller_config,
        EO_INIT(.flagsinittedjoints)        0x0000,
        EO_INIT(.flagsinittedmotors)        0x0000   
    },
    EO_INIT(.ccmmnds)                      
    {
        EO_INIT(.go2stateofcontroller)      eomc_stateofcontroller_config,
        EO_INIT(.filler07)                  {0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7}   
    }    
};


// - descriptors for the variables of a joint

EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_jconfig =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.jconfig),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_jconfig,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.jconfig,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_jconfig,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_jconfig
};


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_jconfig__pidposition =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.jconfig.pidposition),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_jconfig__pidposition,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.jconfig.pidposition,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_jconfig__pidposition,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_jconfig__pidposition
};


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_jconfig__pidvelocity =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.jconfig.pidvelocity),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_jconfig__pidvelocity,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.jconfig.pidvelocity,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_jconfig__pidvelocity,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_jconfig__pidvelocity
};


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_jconfig__pidtorque =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.jconfig.pidtorque),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_jconfig__pidtorque,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.jconfig.pidtorque,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_jconfig__pidtorque,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_jconfig__pidtorque
};


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_jconfig__impedance =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.jconfig.impedance),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_jconfig__impedance,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.jconfig.impedance,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_jconfig__impedance,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_jconfig__impedance
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_jconfig__minpositionofjoint =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.jconfig.minpositionofjoint),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_jconfig__minpositionofjoint,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.jconfig.minpositionofjoint,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_jconfig__minpositionofjoint,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_jconfig__minpositionofjoint
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_jconfig__maxpositionofjoint =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.jconfig.maxpositionofjoint),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_jconfig__maxpositionofjoint,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.jconfig.maxpositionofjoint,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_jconfig__maxpositionofjoint,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_jconfig__maxpositionofjoint
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_jconfig__velocitysetpointtimeout =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.jconfig.velocitysetpointtimeout),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_jconfig__velocitysetpointtimeout,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.jconfig.velocitysetpointtimeout,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_jconfig__velocitysetpointtimeout,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_jconfig__velocitysetpointtimeout
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_jconfig__motionmonitormode =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.jconfig.motionmonitormode),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_jconfig__motionmonitormode,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.jconfig.motionmonitormode,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_jconfig__motionmonitormode,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_jconfig__motionmonitormode
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_jconfig__encoderconversionfactor =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.jconfig.encoderconversionfactor),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_jconfig__encoderconversionfactor,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.jconfig.encoderconversionfactor,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_jconfig__encoderconversionfactor,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_jconfig__encoderconversionfactor
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_jconfig__encoderconversionoffset =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.jconfig.encoderconversionoffset),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_jconfig__encoderconversionoffset,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.jconfig.encoderconversionoffset,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_jconfig__encoderconversionoffset,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_jconfig__encoderconversionoffset
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_jstatus =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.jstatus),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_jstatus,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.jstatus,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_jstatus,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_jstatus
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_jstatus__basic =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.jstatus.basic),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_jstatus__basic,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.jstatus.basic,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_jstatus__basic,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_jstatus__basic
};


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_jstatus__ofpid =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.jstatus.ofpid),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_jstatus__ofpid,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.jstatus.ofpid,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_jstatus__ofpid,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_jstatus__ofpid
};


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_jstatus__chamaleon04 =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.jstatus.chamaleon04),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_jstatus__chamaleon04,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.jstatus.chamaleon04,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_jstatus__chamaleon04,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_jstatus__chamaleon04
};


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_jinputs =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.jinputs),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_jinputs,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.jinputs,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_jinputs,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_jinputs
};


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_jinputs__externallymeasuredtorque =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.jinputs.externallymeasuredtorque),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_jinputs__externallymeasuredtorque,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.jinputs.externallymeasuredtorque,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_jinputs__externallymeasuredtorque,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_jinputs__externallymeasuredtorque
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_jcmmnds__calibration =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.jcmmnds.calibration),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_jcmmnds__calibration,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.jcmmnds.calibration,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_jcmmnds__calibration,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_jcmmnds__calibration
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_jcmmnds__setpoint =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.jcmmnds.setpoint),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_jcmmnds__setpoint,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.jcmmnds.setpoint,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_jcmmnds__setpoint,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_jcmmnds__setpoint
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_jcmmnds__stoptrajectory =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.jcmmnds.stoptrajectory),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_jcmmnds__stoptrajectory,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.jcmmnds.stoptrajectory,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_jcmmnds__stoptrajectory,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_jcmmnds__stoptrajectory
}; 


EOnv_rom_t eoprot_ep_mc_rom_joint_descriptor_jcmmnds__controlmode =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.jcmmnds.controlmode),
    EO_INIT(.funtyp)    eoprot_ep_mc_joint_funtyp_jcmmnds__controlmode,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.jcmmnds.controlmode,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_joint_INIT_jcmmnds__controlmode,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_joint_UPDT_jcmmnds__controlmode
}; 



// - descriptors for the variables of a motor 

EOnv_rom_t eoprot_ep_mc_rom_motor_descriptor_mconfig =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_motor_defaultvalue.mconfig),
    EO_INIT(.funtyp)    eoprot_ep_mc_motor_funtyp_mconfig,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_motor_defaultvalue.mconfig,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_motor_INIT_mconfig,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_motor_UPDT_mconfig
};


EOnv_rom_t eoprot_ep_mc_rom_motor_descriptor_mconfig__pidcurrent =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_motor_defaultvalue.mconfig.pidcurrent),
    EO_INIT(.funtyp)    eoprot_ep_mc_motor_funtyp_mconfig__pidcurrent,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_motor_defaultvalue.mconfig.pidcurrent,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_motor_INIT_mconfig__pidcurrent,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_motor_UPDT_mconfig__pidcurrent
};


EOnv_rom_t eoprot_ep_mc_rom_motor_descriptor_mconfig__maxvelocityofmotor =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_motor_defaultvalue.mconfig.maxvelocityofmotor),
    EO_INIT(.funtyp)    eoprot_ep_mc_motor_funtyp_mconfig__maxvelocityofmotor,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_motor_defaultvalue.mconfig.maxvelocityofmotor,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_motor_INIT_mconfig__maxvelocityofmotor,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_motor_UPDT_mconfig__maxvelocityofmotor
};


EOnv_rom_t eoprot_ep_mc_rom_motor_descriptor_mconfig__maxcurrentofmotor =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_motor_defaultvalue.mconfig.maxcurrentofmotor),
    EO_INIT(.funtyp)    eoprot_ep_mc_motor_funtyp_mconfig__maxcurrentofmotor,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_motor_defaultvalue.mconfig.maxcurrentofmotor,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_motor_INIT_mconfig__maxcurrentofmotor,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_motor_UPDT_mconfig__maxcurrentofmotor
};



EOnv_rom_t eoprot_ep_mc_rom_motor_descriptor_mstatus =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_motor_defaultvalue.mstatus),
    EO_INIT(.funtyp)    eoprot_ep_mc_motor_funtyp_mstatus,
    EO_INIT(.dummy)     0,   
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_motor_defaultvalue.mstatus,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_motor_INIT_mstatus,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_motor_UPDT_mstatus
};


EOnv_rom_t eoprot_ep_mc_rom_motor_descriptor_mstatus__basic =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_motor_defaultvalue.mstatus.basic),
    EO_INIT(.funtyp)    eoprot_ep_mc_motor_funtyp_mstatus__basic,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_motor_defaultvalue.mstatus.basic,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_motor_INIT_mstatus__basic,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_motor_UPDT_mstatus__basic
};

EOnv_rom_t eoprot_ep_mc_rom_motor_descriptor_mstatus__chamaleon04 =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_motor_defaultvalue.mstatus.chamaleon04),
    EO_INIT(.funtyp)    eoprot_ep_mc_motor_funtyp_mstatus__chamaleon04,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_motor_defaultvalue.mstatus.chamaleon04,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_motor_INIT_mstatus__chamaleon04,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_motor_UPDT_mstatus__chamaleon04
};


// - descriptors for the variables of a controller


EOnv_rom_t eoprot_ep_mc_rom_controller_descriptor_cconfig =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_controller_defaultvalue.cconfig),
    EO_INIT(.funtyp)    eoprot_ep_mc_controller_funtyp_cconfig,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_controller_defaultvalue.cconfig,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_controller_INIT_cconfig,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_controller_UPDT_cconfig
};


EOnv_rom_t eoprot_ep_mc_rom_controller_descriptor_cconfig__durationofctrlloop =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_controller_defaultvalue.cconfig.durationofctrlloop),
    EO_INIT(.funtyp)    eoprot_ep_mc_controller_funtyp_cconfig__durationofctrlloop,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_controller_defaultvalue.cconfig.durationofctrlloop,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_controller_INIT_cconfig__durationofctrlloop,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_controller_UPDT_cconfig__durationofctrlloop
};


EOnv_rom_t eoprot_ep_mc_rom_controller_descriptor_cstatus =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_controller_defaultvalue.cstatus),
    EO_INIT(.funtyp)    eoprot_ep_mc_controller_funtyp_cstatus,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_controller_defaultvalue.cstatus,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_controller_INIT_cstatus,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_controller_UPDT_cstatus
};


EOnv_rom_t eoprot_ep_mc_rom_controller_descriptor_cstatus__alljomoinitted =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_controller_defaultvalue.cstatus.alljomoinitted),
    EO_INIT(.funtyp)    eoprot_ep_mc_controller_funtyp_cstatus__alljomoinitted,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_controller_defaultvalue.cstatus.alljomoinitted,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_controller_INIT_cstatus__alljomoinitted,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_controller_UPDT_cstatus__alljomoinitted
};


EOnv_rom_t eoprot_ep_mc_rom_controller_descriptor_ccmmnds__go2stateofcontroller =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_controller_defaultvalue.ccmmnds.go2stateofcontroller),
    EO_INIT(.funtyp)    eoprot_ep_mc_controller_funtyp_ccmmnds__go2stateofcontroller,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_controller_defaultvalue.ccmmnds.go2stateofcontroller,
    EO_INIT(.init)      s_eoprot_ep_mc_rom_controller_INIT_ccmmnds__go2stateofcontroller,
    EO_INIT(.update)    s_eoprot_ep_mc_rom_controller_UPDT_ccmmnds__go2stateofcontroller
};




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


const EOnv_rom_t * const eoprot_ep_mc_rom_folded_descriptors[] =
{
    // here are eoprot_ep_mc_joint_tags_numberof descriptors for the joints (equal for every joint)
    &eoprot_ep_mc_rom_joint_descriptor_jconfig,
    &eoprot_ep_mc_rom_joint_descriptor_jconfig__pidposition,
    &eoprot_ep_mc_rom_joint_descriptor_jconfig__pidvelocity,
    &eoprot_ep_mc_rom_joint_descriptor_jconfig__pidtorque,
    &eoprot_ep_mc_rom_joint_descriptor_jconfig__impedance,
    &eoprot_ep_mc_rom_joint_descriptor_jconfig__minpositionofjoint,
    &eoprot_ep_mc_rom_joint_descriptor_jconfig__maxpositionofjoint,
    &eoprot_ep_mc_rom_joint_descriptor_jconfig__velocitysetpointtimeout,
    &eoprot_ep_mc_rom_joint_descriptor_jconfig__motionmonitormode,
    &eoprot_ep_mc_rom_joint_descriptor_jconfig__encoderconversionfactor,
    &eoprot_ep_mc_rom_joint_descriptor_jconfig__encoderconversionoffset,
    &eoprot_ep_mc_rom_joint_descriptor_jstatus,
    &eoprot_ep_mc_rom_joint_descriptor_jstatus__basic,
    &eoprot_ep_mc_rom_joint_descriptor_jstatus__ofpid,
    &eoprot_ep_mc_rom_joint_descriptor_jstatus__chamaleon04,
    &eoprot_ep_mc_rom_joint_descriptor_jinputs,
    &eoprot_ep_mc_rom_joint_descriptor_jinputs__externallymeasuredtorque,
    &eoprot_ep_mc_rom_joint_descriptor_jcmmnds__calibration,
    &eoprot_ep_mc_rom_joint_descriptor_jcmmnds__setpoint,
    &eoprot_ep_mc_rom_joint_descriptor_jcmmnds__stoptrajectory,
    &eoprot_ep_mc_rom_joint_descriptor_jcmmnds__controlmode,
    
    // here are eoprot_ep_mc_motor_tags_numberof descriptors for the motors (equal for every motor)
    &eoprot_ep_mc_rom_motor_descriptor_mconfig,
    &eoprot_ep_mc_rom_motor_descriptor_mconfig__pidcurrent,
    &eoprot_ep_mc_rom_motor_descriptor_mconfig__maxvelocityofmotor,
    &eoprot_ep_mc_rom_motor_descriptor_mconfig__maxcurrentofmotor,
    &eoprot_ep_mc_rom_motor_descriptor_mstatus,
    &eoprot_ep_mc_rom_motor_descriptor_mstatus__basic,
    &eoprot_ep_mc_rom_motor_descriptor_mstatus__chamaleon04,

    // here are eoprot_ep_mc_controller_tags_numberof descriptors for the single controller
    &eoprot_ep_mc_rom_controller_descriptor_cconfig,
    &eoprot_ep_mc_rom_controller_descriptor_cconfig__durationofctrlloop,
    &eoprot_ep_mc_rom_controller_descriptor_cstatus,
    &eoprot_ep_mc_rom_controller_descriptor_cstatus__alljomoinitted,
    &eoprot_ep_mc_rom_controller_descriptor_ccmmnds__go2stateofcontroller
         
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

static void s_eoprot_ep_mc_rom_joint_INIT_jconfig(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_jconfig(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_jconfig(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_jconfig(nv, rd, index);
}


static void s_eoprot_ep_mc_rom_joint_INIT_jconfig__pidposition(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_jconfig__pidposition(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_jconfig__pidposition(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_jconfig__pidposition(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_jconfig__pidvelocity(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_jconfig__pidvelocity(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_jconfig__pidvelocity(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_jconfig__pidvelocity(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_jconfig__pidtorque(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_jconfig__pidtorque(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_jconfig__pidtorque(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_jconfig__pidtorque(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_jconfig__impedance(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_jconfig__impedance(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_jconfig__impedance(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_jconfig__impedance(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_jconfig__minpositionofjoint(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_jconfig__minpositionofjoint(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_jconfig__minpositionofjoint(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_jconfig__minpositionofjoint(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_jconfig__maxpositionofjoint(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_jconfig__maxpositionofjoint(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_jconfig__maxpositionofjoint(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_jconfig__maxpositionofjoint(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_jconfig__velocitysetpointtimeout(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_jconfig__velocitysetpointtimeout(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_jconfig__velocitysetpointtimeout(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_jconfig__velocitysetpointtimeout(nv, rd, index);
}


static void s_eoprot_ep_mc_rom_joint_INIT_jconfig__motionmonitormode(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_jconfig__motionmonitormode(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_jconfig__motionmonitormode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_jconfig__motionmonitormode(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_jconfig__encoderconversionfactor(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_jconfig__encoderconversionfactor(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_jconfig__encoderconversionfactor(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_jconfig__encoderconversionfactor(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_jconfig__encoderconversionoffset(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_jconfig__encoderconversionoffset(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_jconfig__encoderconversionoffset(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_jconfig__encoderconversionoffset(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_jstatus(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_jstatus(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_jstatus(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_jstatus(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_jstatus__basic(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_jstatus__basic(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_jstatus__basic(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_jstatus__basic(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_jstatus__ofpid(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_jstatus__ofpid(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_jstatus__ofpid(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_jstatus__ofpid(nv, rd, index);
}


static void s_eoprot_ep_mc_rom_joint_INIT_jstatus__chamaleon04(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_jstatus__chamaleon04(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_jstatus__chamaleon04(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_jstatus__chamaleon04(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_jinputs(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_jinputs(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_jinputs(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_jinputs(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_jinputs__externallymeasuredtorque(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_jinputs__externallymeasuredtorque(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_jinputs__externallymeasuredtorque(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_jinputs__externallymeasuredtorque(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_jcmmnds__calibration(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_jcmmnds__calibration(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_jcmmnds__calibration(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_jcmmnds__calibration(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_jcmmnds__setpoint(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_jcmmnds__setpoint(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_jcmmnds__setpoint(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_jcmmnds__setpoint(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_jcmmnds__stoptrajectory(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_jcmmnds__stoptrajectory(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_jcmmnds__stoptrajectory(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_jcmmnds__stoptrajectory(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_joint_INIT_jcmmnds__controlmode(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_joint_jcmmnds__controlmode(nv, index);
}
static void s_eoprot_ep_mc_rom_joint_UPDT_jcmmnds__controlmode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_joint_jcmmnds__controlmode(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_motor_INIT_mconfig(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_motor_mconfig(nv, index);
}
static void s_eoprot_ep_mc_rom_motor_UPDT_mconfig(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_motor_mconfig(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_motor_INIT_mconfig__pidcurrent(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_motor_mconfig__pidcurrent(nv, index);
}
static void s_eoprot_ep_mc_rom_motor_UPDT_mconfig__pidcurrent(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_motor_mconfig__pidcurrent(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_motor_INIT_mconfig__maxvelocityofmotor(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_motor_mconfig__maxvelocityofmotor(nv, index);
}
static void s_eoprot_ep_mc_rom_motor_UPDT_mconfig__maxvelocityofmotor(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_motor_mconfig__maxvelocityofmotor(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_motor_INIT_mconfig__maxcurrentofmotor(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_motor_mconfig__maxcurrentofmotor(nv, index);
}
static void s_eoprot_ep_mc_rom_motor_UPDT_mconfig__maxcurrentofmotor(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_motor_mconfig__maxcurrentofmotor(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_motor_INIT_mstatus(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_motor_mstatus(nv, index);
}
static void s_eoprot_ep_mc_rom_motor_UPDT_mstatus(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_motor_mstatus(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_motor_INIT_mstatus__basic(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_motor_mstatus__basic(nv, index);
}
static void s_eoprot_ep_mc_rom_motor_UPDT_mstatus__basic(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_motor_mstatus__basic(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_motor_INIT_mstatus__chamaleon04(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_motor_mstatus__chamaleon04(nv, index);
}
static void s_eoprot_ep_mc_rom_motor_UPDT_mstatus__chamaleon04(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_motor_mstatus__chamaleon04(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_controller_INIT_cconfig(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_controller_cconfig(nv, index);
}
static void s_eoprot_ep_mc_rom_controller_UPDT_cconfig(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_controller_cconfig(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_controller_INIT_cconfig__durationofctrlloop(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_controller_cconfig__durationofctrlloop(nv, index);
}
static void s_eoprot_ep_mc_rom_controller_UPDT_cconfig__durationofctrlloop(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_controller_cconfig__durationofctrlloop(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_controller_INIT_cstatus(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_controller_cstatus(nv, index);
}
static void s_eoprot_ep_mc_rom_controller_UPDT_cstatus(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_controller_cstatus(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_controller_INIT_cstatus__alljomoinitted(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_controller_cstatus__alljomoinitted(nv, index);
}
static void s_eoprot_ep_mc_rom_controller_UPDT_cstatus__alljomoinitted(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_controller_cstatus__alljomoinitted(nv, rd, index);
}

static void s_eoprot_ep_mc_rom_controller_INIT_ccmmnds__go2stateofcontroller(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_INIT_controller_ccmmnds__go2stateofcontroller(nv, index);
}
static void s_eoprot_ep_mc_rom_controller_UPDT_ccmmnds__go2stateofcontroller(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mc_fun_UPDT_controller_ccmmnds__go2stateofcontroller(nv, rd, index);
}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



