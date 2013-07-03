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


static uint16_t s_eoprot_ep_mc_rom_epid2index_of_folded_descriptors(eOprotID_t id);

static uint16_t s_eoprot_ep_mc_rom_joint_ramoffset(uint16_t tag);
static uint16_t s_eoprot_ep_mc_rom_motor_ramoffset(uint16_t tag);
static uint16_t s_eoprot_ep_mc_rom_controller_ramoffset(uint16_t tag);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// - default value of a joint

#if 0
const eOmc_joint_t eoprot_ep_mc_rom_joint_defaultvalue = { 0 };
#else
const eOmc_joint_t eoprot_ep_mc_rom_joint_defaultvalue =
{
    EO_INIT(.config)             
    {
        EO_INIT(.pidposition)
        {
            EO_INIT(.kp)                    0x6666,
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
            EO_INIT(.kp)                    0x7777,
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
            EO_INIT(.position)              0x99999999,
            EO_INIT(.velocity)              0x88888888,       
            EO_INIT(.acceleration)          0x77777777,     
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
        EO_INIT(.chamaleon04)               {0x0C, 0x01, 0x0A, 0x00} 
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
#endif

// - default value of a motor

#if 1
const eOmc_motor_t eoprot_ep_mc_rom_motor_defaultvalue = { 0 };
#else
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
#endif

// - default value of a controller

#if 1
const eOmc_controller_t eoprot_ep_mc_rom_controller_defaultvalue = { 0 };
#else
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
#endif


// - descriptors for the variables of a joint

EOnv_rom_t eoprot_ep_mc_rom_descriptor_joint_wholeitem =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_joint_wholeitem,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_joint_wholeitem,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_joint_wholeitem
};


EOnv_rom_t eoprot_ep_mc_rom_descriptor_joint_config =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_joint_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.config,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_joint_config,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_joint_config
};


EOnv_rom_t eoprot_ep_mc_rom_descriptor_joint_config_pidposition =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config.pidposition),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_joint_config_pidposition,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.config.pidposition,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_joint_config_pidposition,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_joint_config_pidposition
};


EOnv_rom_t eoprot_ep_mc_rom_descriptor_joint_config_pidvelocity =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config.pidvelocity),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_joint_config_pidvelocity,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.config.pidvelocity,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_joint_config_pidvelocity,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_joint_config_pidvelocity
};


EOnv_rom_t eoprot_ep_mc_rom_descriptor_joint_config_pidtorque =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config.pidtorque),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_joint_config_pidtorque,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.config.pidtorque,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_joint_config_pidtorque,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_joint_config_pidtorque
};


EOnv_rom_t eoprot_ep_mc_rom_descriptor_joint_config_impedance =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config.impedance),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_joint_config_impedance,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.config.impedance,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_joint_config_impedance,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_joint_config_impedance
}; 


EOnv_rom_t eoprot_ep_mc_rom_descriptor_joint_config_minpositionofjoint =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config.minpositionofjoint),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_joint_config_minpositionofjoint,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.config.minpositionofjoint,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_joint_config_minpositionofjoint,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_joint_config_minpositionofjoint
}; 


EOnv_rom_t eoprot_ep_mc_rom_descriptor_joint_config_maxpositionofjoint =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config.maxpositionofjoint),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_joint_config_maxpositionofjoint,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.config.maxpositionofjoint,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_joint_config_maxpositionofjoint,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_joint_config_maxpositionofjoint
}; 


EOnv_rom_t eoprot_ep_mc_rom_descriptor_joint_config_velocitysetpointtimeout =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config.velocitysetpointtimeout),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_joint_config_velocitysetpointtimeout,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.config.velocitysetpointtimeout,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_joint_config_velocitysetpointtimeout,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_joint_config_velocitysetpointtimeout
}; 


EOnv_rom_t eoprot_ep_mc_rom_descriptor_joint_config_motionmonitormode =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config.motionmonitormode),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_joint_config_motionmonitormode,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.config.motionmonitormode,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_joint_config_motionmonitormode,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_joint_config_motionmonitormode
}; 


EOnv_rom_t eoprot_ep_mc_rom_descriptor_joint_config_encoderconversionfactor =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config.encoderconversionfactor),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_joint_config_encoderconversionfactor,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.config.encoderconversionfactor,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_joint_config_encoderconversionfactor,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_joint_config_encoderconversionfactor
}; 


EOnv_rom_t eoprot_ep_mc_rom_descriptor_joint_config_encoderconversionoffset =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.config.encoderconversionoffset),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_joint_config_encoderconversionoffset,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.config.encoderconversionoffset,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_joint_config_encoderconversionoffset,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_joint_config_encoderconversionoffset
}; 


EOnv_rom_t eoprot_ep_mc_rom_descriptor_joint_status =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.status),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_joint_status,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.status,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_joint_status,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_joint_status
}; 


EOnv_rom_t eoprot_ep_mc_rom_descriptor_joint_status_basic =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.status.basic),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_joint_status_basic,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.status.basic,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_joint_status_basic,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_joint_status_basic
};


EOnv_rom_t eoprot_ep_mc_rom_descriptor_joint_status_ofpid =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.status.ofpid),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_joint_status_ofpid,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.status.ofpid,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_joint_status_ofpid,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_joint_status_ofpid
};


EOnv_rom_t eoprot_ep_mc_rom_descriptor_joint_status_chamaleon04 =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.status.chamaleon04),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_joint_status_chamaleon04,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.status.chamaleon04,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_joint_status_chamaleon04,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_joint_status_chamaleon04
};


EOnv_rom_t eoprot_ep_mc_rom_descriptor_joint_inputs =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.inputs),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_joint_inputs,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.inputs,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_joint_inputs,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_joint_inputs
};


EOnv_rom_t eoprot_ep_mc_rom_descriptor_joint_inputs_externallymeasuredtorque =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.inputs.externallymeasuredtorque),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_joint_inputs_externallymeasuredtorque,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.inputs.externallymeasuredtorque,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_joint_inputs_externallymeasuredtorque,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_joint_inputs_externallymeasuredtorque
}; 


EOnv_rom_t eoprot_ep_mc_rom_descriptor_joint_cmmnds_calibration =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.cmmnds.calibration),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_joint_cmmnds_calibration,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.cmmnds.calibration,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_joint_cmmnds_calibration,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_joint_cmmnds_calibration
}; 


EOnv_rom_t eoprot_ep_mc_rom_descriptor_joint_cmmnds_setpoint =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.cmmnds.setpoint),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_joint_cmmnds_setpoint,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.cmmnds.setpoint,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_joint_cmmnds_setpoint,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_joint_cmmnds_setpoint
}; 


EOnv_rom_t eoprot_ep_mc_rom_descriptor_joint_cmmnds_stoptrajectory =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.cmmnds.stoptrajectory),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_joint_cmmnds_stoptrajectory,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.cmmnds.stoptrajectory,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_joint_cmmnds_stoptrajectory,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_joint_cmmnds_stoptrajectory
}; 


EOnv_rom_t eoprot_ep_mc_rom_descriptor_joint_cmmnds_controlmode =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_joint_defaultvalue.cmmnds.controlmode),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_joint_cmmnds_controlmode,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_joint_defaultvalue.cmmnds.controlmode,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_joint_cmmnds_controlmode,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_joint_cmmnds_controlmode
}; 




// - descriptors for the variables of a motor 


EOnv_rom_t eoprot_ep_mc_rom_descriptor_motor_wholeitem =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_motor_defaultvalue),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_motor_wholeitem,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_motor_defaultvalue,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_motor_wholeitem,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_motor_wholeitem
};


EOnv_rom_t eoprot_ep_mc_rom_descriptor_motor_config =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_motor_defaultvalue.config),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_motor_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_motor_defaultvalue.config,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_motor_config,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_motor_config
};


EOnv_rom_t eoprot_ep_mc_rom_descriptor_motor_config_pidcurrent =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_motor_defaultvalue.config.pidcurrent),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_motor_config_pidcurrent,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_motor_defaultvalue.config.pidcurrent,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_motor_config_pidcurrent,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_motor_config_pidcurrent
};


EOnv_rom_t eoprot_ep_mc_rom_descriptor_motor_config_maxvelocityofmotor =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_motor_defaultvalue.config.maxvelocityofmotor),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_motor_config_maxvelocityofmotor,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_motor_defaultvalue.config.maxvelocityofmotor,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_motor_config_maxvelocityofmotor,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_motor_config_maxvelocityofmotor
};


EOnv_rom_t eoprot_ep_mc_rom_descriptor_motor_config_maxcurrentofmotor =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_motor_defaultvalue.config.maxcurrentofmotor),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_motor_config_maxcurrentofmotor,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_motor_defaultvalue.config.maxcurrentofmotor,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_motor_config_maxcurrentofmotor,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_motor_config_maxcurrentofmotor
};


EOnv_rom_t eoprot_ep_mc_rom_descriptor_motor_status =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_motor_defaultvalue.status),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_motor_status,
    EO_INIT(.dummy)     0,   
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_motor_defaultvalue.status,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_motor_status,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_motor_status
};


EOnv_rom_t eoprot_ep_mc_rom_descriptor_motor_status_basic =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_motor_defaultvalue.status.basic),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_motor_status_basic,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_motor_defaultvalue.status.basic,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_motor_status_basic,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_motor_status_basic
};


EOnv_rom_t eoprot_ep_mc_rom_descriptor_motor_status_chamaleon04 =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_motor_defaultvalue.status.chamaleon04),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_motor_status_chamaleon04,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_motor_defaultvalue.status.chamaleon04,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_motor_status_chamaleon04,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_motor_status_chamaleon04
};


// - descriptors for the variables of a controller


EOnv_rom_t eoprot_ep_mc_rom_descriptor_controller_wholeitem =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_controller_defaultvalue),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_controller_wholeitem,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_controller_defaultvalue,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_controller_wholeitem,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_controller_wholeitem
};


EOnv_rom_t eoprot_ep_mc_rom_descriptor_controller_config =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_controller_defaultvalue.config),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_controller_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_controller_defaultvalue.config,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_controller_config,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_controller_config
};


EOnv_rom_t eoprot_ep_mc_rom_descriptor_controller_config_durationofctrlloop =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_controller_defaultvalue.config.durationofctrlloop),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_controller_config_durationofctrlloop,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_controller_defaultvalue.config.durationofctrlloop,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_controller_config_durationofctrlloop,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_controller_config_durationofctrlloop
};


EOnv_rom_t eoprot_ep_mc_rom_descriptor_controller_status =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_controller_defaultvalue.status),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_controller_status,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_controller_defaultvalue.status,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_controller_status,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_controller_status
};


EOnv_rom_t eoprot_ep_mc_rom_descriptor_controller_status_alljomoinitted =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_controller_defaultvalue.status.alljomoinitted),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_controller_status_alljomoinitted,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_controller_defaultvalue.status.alljomoinitted,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_controller_status_alljomoinitted,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_controller_status_alljomoinitted
};


EOnv_rom_t eoprot_ep_mc_rom_descriptor_controller_cmmnds_go2stateofcontroller =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mc_rom_controller_defaultvalue.cmmnds.go2stateofcontroller),
    EO_INIT(.rwmode)    eoprot_ep_mc_rwmode_controller_cmmnds_go2stateofcontroller,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mc_rom_controller_defaultvalue.cmmnds.go2stateofcontroller,
    EO_INIT(.init)      eoprot_ep_mc_fun_INIT_controller_cmmnds_go2stateofcontroller,
    EO_INIT(.update)    eoprot_ep_mc_fun_UPDT_controller_cmmnds_go2stateofcontroller
};




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


const EOnv_rom_t * const eoprot_ep_mc_rom_folded_descriptors[] =
{
    // here are eoprot_ep_mc_tag_joints_numberof descriptors for the joints (equal for every joint)
    &eoprot_ep_mc_rom_descriptor_joint_wholeitem,
    &eoprot_ep_mc_rom_descriptor_joint_config,
    &eoprot_ep_mc_rom_descriptor_joint_config_pidposition,
    &eoprot_ep_mc_rom_descriptor_joint_config_pidvelocity,
    &eoprot_ep_mc_rom_descriptor_joint_config_pidtorque,
    &eoprot_ep_mc_rom_descriptor_joint_config_impedance,
    &eoprot_ep_mc_rom_descriptor_joint_config_minpositionofjoint,
    &eoprot_ep_mc_rom_descriptor_joint_config_maxpositionofjoint,
    &eoprot_ep_mc_rom_descriptor_joint_config_velocitysetpointtimeout,
    &eoprot_ep_mc_rom_descriptor_joint_config_motionmonitormode,
    &eoprot_ep_mc_rom_descriptor_joint_config_encoderconversionfactor,
    &eoprot_ep_mc_rom_descriptor_joint_config_encoderconversionoffset,
    &eoprot_ep_mc_rom_descriptor_joint_status,
    &eoprot_ep_mc_rom_descriptor_joint_status_basic,
    &eoprot_ep_mc_rom_descriptor_joint_status_ofpid,
    &eoprot_ep_mc_rom_descriptor_joint_status_chamaleon04,
    &eoprot_ep_mc_rom_descriptor_joint_inputs,
    &eoprot_ep_mc_rom_descriptor_joint_inputs_externallymeasuredtorque,
    &eoprot_ep_mc_rom_descriptor_joint_cmmnds_calibration,
    &eoprot_ep_mc_rom_descriptor_joint_cmmnds_setpoint,
    &eoprot_ep_mc_rom_descriptor_joint_cmmnds_stoptrajectory,
    &eoprot_ep_mc_rom_descriptor_joint_cmmnds_controlmode,
    
    // here are eoprot_ep_mc_tags_motor_numberof descriptors for the motors (equal for every motor)
    &eoprot_ep_mc_rom_descriptor_motor_wholeitem,
    &eoprot_ep_mc_rom_descriptor_motor_config,
    &eoprot_ep_mc_rom_descriptor_motor_config_pidcurrent,
    &eoprot_ep_mc_rom_descriptor_motor_config_maxvelocityofmotor,
    &eoprot_ep_mc_rom_descriptor_motor_config_maxcurrentofmotor,
    &eoprot_ep_mc_rom_descriptor_motor_status,
    &eoprot_ep_mc_rom_descriptor_motor_status_basic,
    &eoprot_ep_mc_rom_descriptor_motor_status_chamaleon04,

    // here are eoprot_ep_mc_tags_controller_numberof descriptors for the single controller
    &eoprot_ep_mc_rom_descriptor_controller_wholeitem,
    &eoprot_ep_mc_rom_descriptor_controller_config,
    &eoprot_ep_mc_rom_descriptor_controller_config_durationofctrlloop,
    &eoprot_ep_mc_rom_descriptor_controller_status,
    &eoprot_ep_mc_rom_descriptor_controller_status_alljomoinitted,
    &eoprot_ep_mc_rom_descriptor_controller_cmmnds_go2stateofcontroller
         
};  EO_VERIFYsizeof(eoprot_ep_mc_rom_folded_descriptors, sizeof(EOnv_rom_t*)*(eoprot_ep_mc_tags_joint_numberof+eoprot_ep_mc_tags_motor_numberof+eoprot_ep_mc_tags_controller_numberof));




const EOconstvector  eoprot_ep_mc_rom_constvector_of_folded_descriptors_dat = 
{
    EO_INIT(.size)              sizeof(eoprot_ep_mc_rom_folded_descriptors)/sizeof(EOnv_rom_t*), 
    EO_INIT(.item_size)         sizeof(EOnv_rom_t*),
    EO_INIT(.item_array_data)   eoprot_ep_mc_rom_folded_descriptors
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern uint16_t eoprot_ep_mc_rom_joint_get_offset(eOprotTag_t tag)
{
    return(s_eoprot_ep_mc_rom_joint_ramoffset(tag));
}

extern uint16_t eoprot_ep_mc_rom_motor_get_offset(eOprotTag_t tag)
{
    return(s_eoprot_ep_mc_rom_motor_ramoffset(tag));
}

extern uint16_t eoprot_ep_mc_rom_controller_get_offset(eOprotTag_t tag)
{
    return(s_eoprot_ep_mc_rom_controller_ramoffset(tag));
}

extern void* eoprot_ep_mc_rom_get_nvrom(eOprotID_t id)
{
    uint16_t indexoffoldeddescriptors = s_eoprot_ep_mc_rom_epid2index_of_folded_descriptors(id);
    
    if(EOK_uint16dummy == indexoffoldeddescriptors)
    {
        return(NULL);
    }
    
    return((void*)eoprot_ep_mc_rom_folded_descriptors[indexoffoldeddescriptors]);   
}


extern uint16_t eoprot_ep_mc_rom_get_sizeofvar(eOprotID_t id)
{     
    EOnv_rom_t* rom = eoprot_ep_mc_rom_get_nvrom(id);  
    if(NULL == rom)
    {
        return(0);
    }    
    return(rom->capacity); 
}


extern uint16_t eoprot_ep_mc_rom_get_prognum(eOprotID_t id)
{   // we assume that the variables are inserted in a progressive way without holes. and even if there are a few holes never mind.
    return(eoprot_ep_variable_ID2tag(eoprot_endpoint_motioncontrol, id));
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static uint16_t s_eoprot_ep_mc_rom_epid2index_of_folded_descriptors(eOprotID_t id)
{      
    uint16_t tag = eoprot_ep_variable_ID2tag(eoprot_endpoint_motioncontrol, id);
    
    if(EOK_uint16dummy == tag)
    {
        return(EOK_uint16dummy);
    }
    
    eOprotEntity_t entity = eoprot_ep_variable_ID2entity(eoprot_endpoint_motioncontrol, id);
    
    //#warning -> function s_eoprot_ep_mc_rom_epid2index_of_folded_descriptors() uses tha fact that we dont have holes and that eoprot_ep_mc_tag_joints_numberof is equal to eoprot_ep_mc_tag_joint_nextone
    
    switch(entity)
    {
        case eomc_entity_joint:
        {   // nothing to add as the joint vars are in first position
            ; 
        } break;
        
        case eomc_entity_motor:
        {   // must add the number of vars in a joint
            tag += eoprot_ep_mc_tags_joint_numberof; 
        } break;      

        case eomc_entity_controller:
        {   // must add the number of vars in a joint and in a motor
            tag += eoprot_ep_mc_tags_joint_numberof; 
            tag += eoprot_ep_mc_tags_motor_numberof;
        } break; 
        
        default:
        {   // error
            tag = EOK_uint16dummy;
        } break;
    
    }
    
    return(tag);   
}

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


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



