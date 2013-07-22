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

/* @file       EoProtocolMC_rom.c
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

#include "EoProtocolMC.h"
#include "EoMotionControl.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolMC_rom.h"


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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// - default value of a joint
static const eOmc_joint_t eoprot_mc_rom_joint_defaultvalue = { 0 };

// - default value of a motor
static const eOmc_motor_t eoprot_mc_rom_motor_defaultvalue = { 0 };

// - default value of a controller
static const eOmc_controller_t eoprot_mc_rom_controller_defaultvalue = { 0 };


// - descriptors for the variables of a joint

static EOnv_rom_t eoprot_mc_rom_descriptor_joint_wholeitem =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_joint_defaultvalue),
    EO_INIT(.rwmode)    eoprot_rwm_mc_joint_wholeitem,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_joint_defaultvalue,
    EO_INIT(.init)      eoprot_fun_INIT_mc_joint_wholeitem,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_joint_wholeitem
};


static EOnv_rom_t eoprot_mc_rom_descriptor_joint_config =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_joint_defaultvalue.config),
    EO_INIT(.rwmode)    eoprot_rwm_mc_joint_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_joint_defaultvalue.config,
    EO_INIT(.init)      eoprot_fun_INIT_mc_joint_config,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_joint_config
};


static EOnv_rom_t eoprot_mc_rom_descriptor_joint_config_pidposition =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_joint_defaultvalue.config.pidposition),
    EO_INIT(.rwmode)    eoprot_rwm_mc_joint_config_pidposition,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_joint_defaultvalue.config.pidposition,
    EO_INIT(.init)      eoprot_fun_INIT_mc_joint_config_pidposition,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_joint_config_pidposition
};


static EOnv_rom_t eoprot_mc_rom_descriptor_joint_config_pidvelocity =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_joint_defaultvalue.config.pidvelocity),
    EO_INIT(.rwmode)    eoprot_rwm_mc_joint_config_pidvelocity,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_joint_defaultvalue.config.pidvelocity,
    EO_INIT(.init)      eoprot_fun_INIT_mc_joint_config_pidvelocity,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_joint_config_pidvelocity
};


static EOnv_rom_t eoprot_mc_rom_descriptor_joint_config_pidtorque =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_joint_defaultvalue.config.pidtorque),
    EO_INIT(.rwmode)    eoprot_rwm_mc_joint_config_pidtorque,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_joint_defaultvalue.config.pidtorque,
    EO_INIT(.init)      eoprot_fun_INIT_mc_joint_config_pidtorque,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_joint_config_pidtorque
};


static EOnv_rom_t eoprot_mc_rom_descriptor_joint_config_impedance =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_joint_defaultvalue.config.impedance),
    EO_INIT(.rwmode)    eoprot_rwm_mc_joint_config_impedance,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_joint_defaultvalue.config.impedance,
    EO_INIT(.init)      eoprot_fun_INIT_mc_joint_config_impedance,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_joint_config_impedance
}; 


static EOnv_rom_t eoprot_mc_rom_descriptor_joint_config_minpositionofjoint =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_joint_defaultvalue.config.minpositionofjoint),
    EO_INIT(.rwmode)    eoprot_rwm_mc_joint_config_minpositionofjoint,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_joint_defaultvalue.config.minpositionofjoint,
    EO_INIT(.init)      eoprot_fun_INIT_mc_joint_config_minpositionofjoint,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_joint_config_minpositionofjoint
}; 


static EOnv_rom_t eoprot_mc_rom_descriptor_joint_config_maxpositionofjoint =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_joint_defaultvalue.config.maxpositionofjoint),
    EO_INIT(.rwmode)    eoprot_rwm_mc_joint_config_maxpositionofjoint,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_joint_defaultvalue.config.maxpositionofjoint,
    EO_INIT(.init)      eoprot_fun_INIT_mc_joint_config_maxpositionofjoint,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_joint_config_maxpositionofjoint
}; 


static EOnv_rom_t eoprot_mc_rom_descriptor_joint_config_velocitysetpointtimeout =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_joint_defaultvalue.config.velocitysetpointtimeout),
    EO_INIT(.rwmode)    eoprot_rwm_mc_joint_config_velocitysetpointtimeout,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_joint_defaultvalue.config.velocitysetpointtimeout,
    EO_INIT(.init)      eoprot_fun_INIT_mc_joint_config_velocitysetpointtimeout,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_joint_config_velocitysetpointtimeout
}; 


static EOnv_rom_t eoprot_mc_rom_descriptor_joint_config_motionmonitormode =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_joint_defaultvalue.config.motionmonitormode),
    EO_INIT(.rwmode)    eoprot_rwm_mc_joint_config_motionmonitormode,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_joint_defaultvalue.config.motionmonitormode,
    EO_INIT(.init)      eoprot_fun_INIT_mc_joint_config_motionmonitormode,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_joint_config_motionmonitormode
}; 


static EOnv_rom_t eoprot_mc_rom_descriptor_joint_config_encoderconversionfactor =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_joint_defaultvalue.config.encoderconversionfactor),
    EO_INIT(.rwmode)    eoprot_rwm_mc_joint_config_encoderconversionfactor,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_joint_defaultvalue.config.encoderconversionfactor,
    EO_INIT(.init)      eoprot_fun_INIT_mc_joint_config_encoderconversionfactor,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_joint_config_encoderconversionfactor
}; 


static EOnv_rom_t eoprot_mc_rom_descriptor_joint_config_encoderconversionoffset =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_joint_defaultvalue.config.encoderconversionoffset),
    EO_INIT(.rwmode)    eoprot_rwm_mc_joint_config_encoderconversionoffset,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_joint_defaultvalue.config.encoderconversionoffset,
    EO_INIT(.init)      eoprot_fun_INIT_mc_joint_config_encoderconversionoffset,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_joint_config_encoderconversionoffset
}; 


static EOnv_rom_t eoprot_mc_rom_descriptor_joint_status =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_joint_defaultvalue.status),
    EO_INIT(.rwmode)    eoprot_rwm_mc_joint_status,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_joint_defaultvalue.status,
    EO_INIT(.init)      eoprot_fun_INIT_mc_joint_status,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_joint_status
}; 


static EOnv_rom_t eoprot_mc_rom_descriptor_joint_status_basic =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_joint_defaultvalue.status.basic),
    EO_INIT(.rwmode)    eoprot_rwm_mc_joint_status_basic,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_joint_defaultvalue.status.basic,
    EO_INIT(.init)      eoprot_fun_INIT_mc_joint_status_basic,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_joint_status_basic
};


static EOnv_rom_t eoprot_mc_rom_descriptor_joint_status_ofpid =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_joint_defaultvalue.status.ofpid),
    EO_INIT(.rwmode)    eoprot_rwm_mc_joint_status_ofpid,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_joint_defaultvalue.status.ofpid,
    EO_INIT(.init)      eoprot_fun_INIT_mc_joint_status_ofpid,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_joint_status_ofpid
};


static EOnv_rom_t eoprot_mc_rom_descriptor_joint_status_chamaleon04 =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_joint_defaultvalue.status.chamaleon04),
    EO_INIT(.rwmode)    eoprot_rwm_mc_joint_status_chamaleon04,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_joint_defaultvalue.status.chamaleon04,
    EO_INIT(.init)      eoprot_fun_INIT_mc_joint_status_chamaleon04,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_joint_status_chamaleon04
};


static EOnv_rom_t eoprot_mc_rom_descriptor_joint_inputs =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_joint_defaultvalue.inputs),
    EO_INIT(.rwmode)    eoprot_rwm_mc_joint_inputs,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_joint_defaultvalue.inputs,
    EO_INIT(.init)      eoprot_fun_INIT_mc_joint_inputs,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_joint_inputs
};


static EOnv_rom_t eoprot_mc_rom_descriptor_joint_inputs_externallymeasuredtorque =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_joint_defaultvalue.inputs.externallymeasuredtorque),
    EO_INIT(.rwmode)    eoprot_rwm_mc_joint_inputs_externallymeasuredtorque,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_joint_defaultvalue.inputs.externallymeasuredtorque,
    EO_INIT(.init)      eoprot_fun_INIT_mc_joint_inputs_externallymeasuredtorque,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_joint_inputs_externallymeasuredtorque
}; 


static EOnv_rom_t eoprot_mc_rom_descriptor_joint_cmmnds_calibration =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_joint_defaultvalue.cmmnds.calibration),
    EO_INIT(.rwmode)    eoprot_rwm_mc_joint_cmmnds_calibration,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_joint_defaultvalue.cmmnds.calibration,
    EO_INIT(.init)      eoprot_fun_INIT_mc_joint_cmmnds_calibration,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_joint_cmmnds_calibration
}; 


static EOnv_rom_t eoprot_mc_rom_descriptor_joint_cmmnds_setpoint =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_joint_defaultvalue.cmmnds.setpoint),
    EO_INIT(.rwmode)    eoprot_rwm_mc_joint_cmmnds_setpoint,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_joint_defaultvalue.cmmnds.setpoint,
    EO_INIT(.init)      eoprot_fun_INIT_mc_joint_cmmnds_setpoint,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_joint_cmmnds_setpoint
}; 


static EOnv_rom_t eoprot_mc_rom_descriptor_joint_cmmnds_stoptrajectory =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_joint_defaultvalue.cmmnds.stoptrajectory),
    EO_INIT(.rwmode)    eoprot_rwm_mc_joint_cmmnds_stoptrajectory,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_joint_defaultvalue.cmmnds.stoptrajectory,
    EO_INIT(.init)      eoprot_fun_INIT_mc_joint_cmmnds_stoptrajectory,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_joint_cmmnds_stoptrajectory
}; 


static EOnv_rom_t eoprot_mc_rom_descriptor_joint_cmmnds_controlmode =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_joint_defaultvalue.cmmnds.controlmode),
    EO_INIT(.rwmode)    eoprot_rwm_mc_joint_cmmnds_controlmode,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_joint_defaultvalue.cmmnds.controlmode,
    EO_INIT(.init)      eoprot_fun_INIT_mc_joint_cmmnds_controlmode,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_joint_cmmnds_controlmode
}; 




// - descriptors for the variables of a motor 


static EOnv_rom_t eoprot_mc_rom_descriptor_motor_wholeitem =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_motor_defaultvalue),
    EO_INIT(.rwmode)    eoprot_rwm_mc_motor_wholeitem,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_motor_defaultvalue,
    EO_INIT(.init)      eoprot_fun_INIT_mc_motor_wholeitem,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_motor_wholeitem
};


static EOnv_rom_t eoprot_mc_rom_descriptor_motor_config =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_motor_defaultvalue.config),
    EO_INIT(.rwmode)    eoprot_rwm_mc_motor_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_motor_defaultvalue.config,
    EO_INIT(.init)      eoprot_fun_INIT_mc_motor_config,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_motor_config
};


static EOnv_rom_t eoprot_mc_rom_descriptor_motor_config_pidcurrent =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_motor_defaultvalue.config.pidcurrent),
    EO_INIT(.rwmode)    eoprot_rwm_mc_motor_config_pidcurrent,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_motor_defaultvalue.config.pidcurrent,
    EO_INIT(.init)      eoprot_fun_INIT_mc_motor_config_pidcurrent,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_motor_config_pidcurrent
};


static EOnv_rom_t eoprot_mc_rom_descriptor_motor_config_maxvelocityofmotor =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_motor_defaultvalue.config.maxvelocityofmotor),
    EO_INIT(.rwmode)    eoprot_rwm_mc_motor_config_maxvelocityofmotor,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_motor_defaultvalue.config.maxvelocityofmotor,
    EO_INIT(.init)      eoprot_fun_INIT_mc_motor_config_maxvelocityofmotor,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_motor_config_maxvelocityofmotor
};


static EOnv_rom_t eoprot_mc_rom_descriptor_motor_config_maxcurrentofmotor =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_motor_defaultvalue.config.maxcurrentofmotor),
    EO_INIT(.rwmode)    eoprot_rwm_mc_motor_config_maxcurrentofmotor,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_motor_defaultvalue.config.maxcurrentofmotor,
    EO_INIT(.init)      eoprot_fun_INIT_mc_motor_config_maxcurrentofmotor,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_motor_config_maxcurrentofmotor
};


static EOnv_rom_t eoprot_mc_rom_descriptor_motor_status =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_motor_defaultvalue.status),
    EO_INIT(.rwmode)    eoprot_rwm_mc_motor_status,
    EO_INIT(.dummy)     0,   
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_motor_defaultvalue.status,
    EO_INIT(.init)      eoprot_fun_INIT_mc_motor_status,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_motor_status
};


static EOnv_rom_t eoprot_mc_rom_descriptor_motor_status_basic =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_motor_defaultvalue.status.basic),
    EO_INIT(.rwmode)    eoprot_rwm_mc_motor_status_basic,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_motor_defaultvalue.status.basic,
    EO_INIT(.init)      eoprot_fun_INIT_mc_motor_status_basic,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_motor_status_basic
};


static EOnv_rom_t eoprot_mc_rom_descriptor_motor_status_chamaleon04 =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_motor_defaultvalue.status.chamaleon04),
    EO_INIT(.rwmode)    eoprot_rwm_mc_motor_status_chamaleon04,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_motor_defaultvalue.status.chamaleon04,
    EO_INIT(.init)      eoprot_fun_INIT_mc_motor_status_chamaleon04,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_motor_status_chamaleon04
};


// - descriptors for the variables of a controller

static EOnv_rom_t eoprot_mc_rom_descriptor_controller_wholeitem =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_controller_defaultvalue),
    EO_INIT(.rwmode)    eoprot_rwm_mc_controller_wholeitem,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_controller_defaultvalue,
    EO_INIT(.init)      eoprot_fun_INIT_mc_controller_wholeitem,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_controller_wholeitem
};


static EOnv_rom_t eoprot_mc_rom_descriptor_controller_config =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_controller_defaultvalue.config),
    EO_INIT(.rwmode)    eoprot_rwm_mc_controller_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_controller_defaultvalue.config,
    EO_INIT(.init)      eoprot_fun_INIT_mc_controller_config,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_controller_config
};


static EOnv_rom_t eoprot_mc_rom_descriptor_controller_config_durationofctrlloop =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_controller_defaultvalue.config.durationofctrlloop),
    EO_INIT(.rwmode)    eoprot_rwm_mc_controller_config_durationofctrlloop,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_controller_defaultvalue.config.durationofctrlloop,
    EO_INIT(.init)      eoprot_fun_INIT_mc_controller_config_durationofctrlloop,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_controller_config_durationofctrlloop
};


static EOnv_rom_t eoprot_mc_rom_descriptor_controller_status =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_controller_defaultvalue.status),
    EO_INIT(.rwmode)    eoprot_rwm_mc_controller_status,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_controller_defaultvalue.status,
    EO_INIT(.init)      eoprot_fun_INIT_mc_controller_status,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_controller_status
};


static EOnv_rom_t eoprot_mc_rom_descriptor_controller_status_alljomoinitted =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_controller_defaultvalue.status.alljomoinitted),
    EO_INIT(.rwmode)    eoprot_rwm_mc_controller_status_alljomoinitted,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_controller_defaultvalue.status.alljomoinitted,
    EO_INIT(.init)      eoprot_fun_INIT_mc_controller_status_alljomoinitted,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_controller_status_alljomoinitted
};


static EOnv_rom_t eoprot_mc_rom_descriptor_controller_cmmnds_go2stateofcontroller =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mc_rom_controller_defaultvalue.cmmnds.go2stateofcontroller),
    EO_INIT(.rwmode)    eoprot_rwm_mc_controller_cmmnds_go2stateofcontroller,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mc_rom_controller_defaultvalue.cmmnds.go2stateofcontroller,
    EO_INIT(.init)      eoprot_fun_INIT_mc_controller_cmmnds_go2stateofcontroller,
    EO_INIT(.update)    eoprot_fun_UPDT_mc_controller_cmmnds_go2stateofcontroller
};


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

// -- the folded array of descriptors: to be changed if any new tag is added


const EOnv_rom_t * const eoprot_mc_rom_folded_descriptors[] =
{
    // here are eoprot_mc_tag_joints_numberof descriptors for the joints (equal for every joint)
    &eoprot_mc_rom_descriptor_joint_wholeitem,
    &eoprot_mc_rom_descriptor_joint_config,
    &eoprot_mc_rom_descriptor_joint_config_pidposition,
    &eoprot_mc_rom_descriptor_joint_config_pidvelocity,
    &eoprot_mc_rom_descriptor_joint_config_pidtorque,
    &eoprot_mc_rom_descriptor_joint_config_impedance,
    &eoprot_mc_rom_descriptor_joint_config_minpositionofjoint,
    &eoprot_mc_rom_descriptor_joint_config_maxpositionofjoint,
    &eoprot_mc_rom_descriptor_joint_config_velocitysetpointtimeout,
    &eoprot_mc_rom_descriptor_joint_config_motionmonitormode,
    &eoprot_mc_rom_descriptor_joint_config_encoderconversionfactor,
    &eoprot_mc_rom_descriptor_joint_config_encoderconversionoffset,
    &eoprot_mc_rom_descriptor_joint_status,
    &eoprot_mc_rom_descriptor_joint_status_basic,
    &eoprot_mc_rom_descriptor_joint_status_ofpid,
    &eoprot_mc_rom_descriptor_joint_status_chamaleon04,
    &eoprot_mc_rom_descriptor_joint_inputs,
    &eoprot_mc_rom_descriptor_joint_inputs_externallymeasuredtorque,
    &eoprot_mc_rom_descriptor_joint_cmmnds_calibration,
    &eoprot_mc_rom_descriptor_joint_cmmnds_setpoint,
    &eoprot_mc_rom_descriptor_joint_cmmnds_stoptrajectory,
    &eoprot_mc_rom_descriptor_joint_cmmnds_controlmode,
    
    // here are eoprot_tags_mc_motor_numberof descriptors for the motors (equal for every motor)
    &eoprot_mc_rom_descriptor_motor_wholeitem,
    &eoprot_mc_rom_descriptor_motor_config,
    &eoprot_mc_rom_descriptor_motor_config_pidcurrent,
    &eoprot_mc_rom_descriptor_motor_config_maxvelocityofmotor,
    &eoprot_mc_rom_descriptor_motor_config_maxcurrentofmotor,
    &eoprot_mc_rom_descriptor_motor_status,
    &eoprot_mc_rom_descriptor_motor_status_basic,
    &eoprot_mc_rom_descriptor_motor_status_chamaleon04,

    // here are eoprot_tags_mc_controller_numberof descriptors for the single controller
    &eoprot_mc_rom_descriptor_controller_wholeitem,
    &eoprot_mc_rom_descriptor_controller_config,
    &eoprot_mc_rom_descriptor_controller_config_durationofctrlloop,
    &eoprot_mc_rom_descriptor_controller_status,
    &eoprot_mc_rom_descriptor_controller_status_alljomoinitted,
    &eoprot_mc_rom_descriptor_controller_cmmnds_go2stateofcontroller
         
};  EO_VERIFYsizeof(eoprot_mc_rom_folded_descriptors, sizeof(EOnv_rom_t*)*(eoprot_tags_mc_joint_numberof+eoprot_tags_mc_motor_numberof+eoprot_tags_mc_controller_numberof));


// the other constants: to be changed when a new entity is added

const uint8_t eoprot_mc_rom_tags_numberof[] = 
{
    eoprot_tags_mc_joint_numberof, 
    eoprot_tags_mc_motor_numberof, 
    eoprot_tags_mc_controller_numberof
};  EO_VERIFYsizeof(eoprot_mc_rom_tags_numberof, eoprot_entities_mc_numberof*sizeof(uint8_t)); 


const uint16_t eoprot_mc_rom_entities_sizeof[] = 
{
    sizeof(eOmc_joint_t), 
    sizeof(eOmc_motor_t), 
    sizeof(eOmc_controller_t)
};  EO_VERIFYsizeof(eoprot_mc_rom_entities_sizeof, eoprot_entities_mc_numberof*sizeof(uint16_t)); 


const uint32_t* eoprot_mc_rom_entities_defval[] = 
{
    (const uint32_t*)&eoprot_mc_rom_joint_defaultvalue, 
    (const uint32_t*)&eoprot_mc_rom_motor_defaultvalue,
	(const uint32_t*)&eoprot_mc_rom_controller_defaultvalue
};  EO_VERIFYsizeof(eoprot_mc_rom_entities_defval, eoprot_entities_mc_numberof*sizeof(uint32_t*)); 




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section

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



