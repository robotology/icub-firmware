/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdlib.h"
#include "EoCommon.h"

#include "EOtheCANprotocol_functions.h" 

#include "EOtheErrorManager.h"
#include "EoError.h"

#include "EOtheCANmapping.h"
#include "EoProtocol.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheCANprotocol.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheCANprotocol_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


const eOcanprot_cfg_t eo_canprot_DefaultCfg = 
{
    0
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eObool_t s_eo_canprot_filter_sourceaddress(eOcanframe_t *frame);

static eOresult_t s_eo_canprot_parse0length(eOcanframe_t *frame, eOcanport_t port);

//static eOresult_t s_eo_canprot_search4exceptions(eOcanframe_t *frame, eOcanport_t port);

static eObool_t s_eo_canprot_isit_exception(eOcanframe_t *frame, eOcanport_t port, eOresult_t *res);


static eOresult_t s_eo_canprot_get_indices(eOcanframe_t *frame, eOcanprot_descriptor_t *des, uint8_t *index0, uint8_t *index1);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOtheCANprotocol";

 
static EOtheCANprotocol s_eo_canprot_singleton = 
{
	.config             =
    {   
        0
    },
    .tobedefined        = 0
};




const eOcanprot_functions_t s_eo_canprot_functions_pollingMotorControl[] =
{
    {   // 000      ICUBCANPROTO_POL_MC_CMD__NO_MESSAGE
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 001      ICUBCANPROTO_POL_MC_CMD__CONTROLLER_RUN
        .former     = eocanprotMCpolling_former_POL_MC_CMD__CONTROLLER_RUN,
        .parser     = NULL
    }, 
    {   // 002      ICUBCANPROTO_POL_MC_CMD__CONTROLLER_IDLE
        .former     = eocanprotMCpolling_former_POL_MC_CMD__CONTROLLER_IDLE,
        .parser     = NULL
    },  
    {   // 003
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 004      ICUBCANPROTO_POL_MC_CMD__CALIBRATE_ENCODER
        .former     = eocanprotMCpolling_former_POL_MC_CMD__CALIBRATE_ENCODER,
        .parser     = NULL
    },  
    {   // 005      ICUBCANPROTO_POL_MC_CMD__ENABLE_PWM_PAD
        .former     = eocanprotMCpolling_former_POL_MC_CMD__ENABLE_PWM_PAD,
        .parser     = NULL
    }, 
    {   // 006      ICUBCANPROTO_POL_MC_CMD__DISABLE_PWM_PAD
        .former     = eocanprotMCpolling_former_POL_MC_CMD__DISABLE_PWM_PAD,
        .parser     = NULL
    },  
    {   // 007      ICUBCANPROTO_POL_MC_CMD__GET_CONTROL_MODE
        .former     = eocanprotMCpolling_former_POL_MC_CMD__GET_CONTROL_MODE,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__GET_CONTROL_MODE
    }, 
    {   // 008      ICUBCANPROTO_POL_MC_CMD__MOTION_DONE
        .former     = eocanprotMCpolling_former_POL_MC_CMD__MOTION_DONE,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__MOTION_DONE
    },  
    {   // 009      ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_CONTROL_MODE,
        .parser     = NULL
    }, 
    {   // 010
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 011
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 012
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 013
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 014      ICUBCANPROTO_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT,
        .parser     = NULL
    },  
    {   // 015      ICUBCANPROTO_POL_MC_CMD__SET_MAX_MOTOR_POS
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_MOTOR_POS,
        .parser     = NULL
    },     
    {   // 016      ICUBCANPROTO_POL_MC_CMD__SET_MIN_MOTOR_POS
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MIN_MOTOR_POS,
        .parser     = NULL
    },  
    {   // 017
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 018      ICUBCANPROTO_POL_MC_CMD__SET_DEBUG_PARAM
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 019      ICUBCANPROTO_POL_MC_CMD__GET_DEBUG_PARAM
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 020      ICUBCANPROTO_POL_MC_CMD__GET_ENCODER_POSITION
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 021      ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_POSITION
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 22       ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_POSITION
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 023      ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_VELOCITY
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 024      ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_VELOCITY
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 025      ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_ACCELER
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 026      ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_ACCELER
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 027      ICUBCANPROTO_POL_MC_CMD__POSITION_MOVE
        .former     = eocanprotMCpolling_former_POL_MC_CMD__POSITION_MOVE,
        .parser     = NULL
    }, 
    {   // 028      ICUBCANPROTO_POL_MC_CMD__VELOCITY_MOVE
        .former     = eocanprotMCpolling_former_POL_MC_CMD__VELOCITY_MOVE,
        .parser     = NULL
    },  
    {   // 029      ICUBCANPROTO_POL_MC_CMD__SET_ENCODER_POSITION
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 30       ICUBCANPROTO_POL_MC_CMD__SET_P_GAIN
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 031      ICUBCANPROTO_POL_MC_CMD__GET_P_GAIN
        .former     = NULL,
        .parser     = NULL
    },    
    {   // 032      ICUBCANPROTO_POL_MC_CMD__SET_D_GAIN
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 033      ICUBCANPROTO_POL_MC_CMD__GET_D_GAIN     
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 034      ICUBCANPROTO_POL_MC_CMD__SET_I_GAIN
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 035      ICUBCANPROTO_POL_MC_CMD__GET_I_GAIN
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 036      ICUBCANPROTO_POL_MC_CMD__SET_ILIM_GAIN
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 037      ICUBCANPROTO_POL_MC_CMD__GET_ILIM_GAIN
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 038      ICUBCANPROTO_POL_MC_CMD__SET_OFFSET
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 039      ICUBCANPROTO_POL_MC_CMD__GET_OFFSET
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 040      ICUBCANPROTO_POL_MC_CMD__SET_SCALE
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 041      ICUBCANPROTO_POL_MC_CMD__GET_SCALE
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 042      ICUBCANPROTO_POL_MC_CMD__SET_TLIM
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 043      ICUBCANPROTO_POL_MC_CMD__GET_TLIM
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 044      ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_TORQUE
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_DESIRED_TORQUE,
        .parser     = NULL
    },  
    {   // 045      ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_TORQUE
        .former     = eocanprotMCpolling_former_POL_MC_CMD__GET_DESIRED_TORQUE,
        .parser     = NULL
    }, 
    {   // 046      ICUBCANPROTO_POL_MC_CMD__STOP_TRAJECTORY
        .former     = eocanprotMCpolling_former_POL_MC_CMD__STOP_TRAJECTORY,
        .parser     = NULL
    },  
    {   // 047      ICUBCANPROTO_POL_MC_CMD__SET_PWM_LIMIT
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_PWM_LIMIT,
        .parser     = NULL
    },     
    {   // 048      ICUBCANPROTO_POL_MC_CMD__GET_PWM_LIMIT
        .former     = eocanprotMCpolling_former_POL_MC_CMD__GET_PWM_LIMIT,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__GET_PWM_LIMIT,
    },  
    {   // 049
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 050      ICUBCANPROTO_POL_MC_CMD__SET_BOARD_ID
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 051      ICUBCANPROTO_POL_MC_CMD__GET_BOARD_ID
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 052      ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_SOURCE
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 053
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 054
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 055      ICUBCANPROTO_POL_MC_CMD__GET_PID_ERROR
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 056      ICUBCANPROTO_POL_MC_CMD__GET_CALIBRATION_STATUS
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 057
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 058
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 059
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 060      ICUBCANPROTO_POL_MC_CMD__GET_ERROR_STATUS
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 061      ICUBCANPROTO_POL_MC_CMD__GET_ENCODER_VELOCITY
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 062      ICUBCANPROTO_POL_MC_CMD__SET_COMMAND_POSITION
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_COMMAND_POSITION,
        .parser     = NULL
    },  
    {   // 063      ICUBCANPROTO_POL_MC_CMD__GET_PID_OUTPUT
        .former     = NULL,
        .parser     = NULL
    },       
    {   // 064      ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MIN_POSITION,
        .parser     = NULL 
    },
    {   // 065      ICUBCANPROTO_POL_MC_CMD__GET_MIN_POSITION
        .former     = eocanprotMCpolling_former_POL_MC_CMD__GET_MIN_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__GET_MIN_POSITION
    },
    {   // 066      ICUBCANPROTO_POL_MC_CMD__SET_MAX_POSITION 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = NULL //unsupported
    },
    {   // 067      ICUBCANPROTO_POL_MC_CMD__GET_MAX_POSITION 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__GET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__GET_MAX_POSITION
    },
    {   // 068      ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_VELOCITY,
        .parser     = NULL
    },  
    {   // 069      ICUBCANPROTO_POL_MC_CMD__GET_MAX_VELOCITY
        .former     = NULL,
        .parser     = NULL
    },
    {   // 070      ICUBCANPROTO_POL_MC_CMD__GET_ACTIVE_ENCODER_POSITION
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 071      ICUBCANPROTO_POL_MC_CMD__SET_ACTIVE_ENCODER_POSITION
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 072      ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_CURRENT_LIMIT,
        .parser     = NULL
    }, 
    {   // 073      ICUBCANPROTO_POL_MC_CMD__SET_BCAST_POLICY
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_BCAST_POLICY,
        .parser     = NULL
    },        
    {   // 074      ICUBCANPROTO_POL_MC_CMD__SET_VEL_SHIFT 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_VEL_SHIFT,
        .parser     = NULL
    },
    {   // 075      ICUBCANPROTO_POL_MC_CMD__SET_OFFSET_ABS_ENCODER
        .former     = NULL,
        .parser     = NULL
    },
    {   // 076      ICUBCANPROTO_POL_MC_CMD__GET_OFFSET_ABS_ENCODER      
        .former     = NULL,
        .parser     = NULL
    },
    {   // 077      ICUBCANPROTO_POL_MC_CMD__SET_SMOOTH_PID
        .former     = NULL,
        .parser     = NULL
    },
    {   // 078      ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PID 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_TORQUE_PID,
        .parser     = NULL
    },  
    {   // 079      ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PID
        .former     = eocanprotMCpolling_former_POL_MC_CMD__GET_TORQUE_PID,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__GET_TORQUE_PID
    },
    {   // 080      ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PIDLIMITS 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_TORQUE_PIDLIMITS,
        .parser     = NULL
    }, 
    {   // 081      ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PIDLIMITS
        .former     = eocanprotMCpolling_former_POL_MC_CMD__GET_TORQUE_PIDLIMITS,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__GET_TORQUE_PIDLIMITS
    }, 
    {   // 082      ICUBCANPROTO_POL_MC_CMD__SET_POS_PID 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_POS_PID,
        .parser     = NULL
    }, 
    {   // 083      ICUBCANPROTO_POL_MC_CMD__GET_POS_PID
        .former     = eocanprotMCpolling_former_POL_MC_CMD__GET_POS_PID,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__GET_POS_PID
    },        
    {   // 084      ICUBCANPROTO_POL_MC_CMD__SET_POS_PIDLIMITS 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_POS_PIDLIMITS,
        .parser     = NULL
    },
    {   // 085      ICUBCANPROTO_POL_MC_CMD__GET_POS_PIDLIMITS
        .former     = eocanprotMCpolling_former_POL_MC_CMD__GET_POS_PIDLIMITS,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__GET_POS_PIDLIMITS
    },
    {   // 086      ICUBCANPROTO_POL_MC_CMD__SET_VEL_TIMEOUT
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_VEL_TIMEOUT,
        .parser     = NULL
    },
    {   // 087      ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_PARAMS
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_IMPEDANCE_PARAMS,
        .parser     = NULL
    },
    {   // 088      ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_PARAMS 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__GET_IMPEDANCE_PARAMS,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__GET_IMPEDANCE_PARAMS
    },  
    {   // 089      ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_OFFSET 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_IMPEDANCE_OFFSET,
        .parser     = NULL
    },
    {   // 090      ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_OFFSET 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__GET_IMPEDANCE_OFFSET,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__GET_IMPEDANCE_OFFSET
    }, 
    {   // 091      ICUBCANPROTO_POL_MC_CMD__GET_FIRMWARE_VERSION 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__GET_FIRMWARE_VERSION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__GET_FIRMWARE_VERSION
    }, 
    {   // 092      ICUBCANPROTO_POL_MC_CMD__SET_OPTICAL_ENC_RATIO 
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 093      ICUBCANPROTO_POL_MC_CMD__SET_POS_STICTION_PARAMS
        .former     = NULL,
        .parser     = NULL
    },        
    {   // 094      ICUBCANPROTO_POL_MC_CMD__GET_POS_STICTION_PARAMS        
        .former     = NULL,
        .parser     = NULL
    },
    {   // 095      ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_STICTION_PARAMS
        .former     = NULL,
        .parser     = NULL
    },
    {   // 096      ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_STICTION_PARAMS
        .former     = NULL,
        .parser     = NULL
    },
    {   // 097      ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_PARAMS 
        .former     = NULL,
        .parser     = NULL
    },
    {   // 098      ICUBCANPROTO_POL_MC_CMD__GET_MOTOR_PARAMS
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 099      ICUBCANPROTO_POL_MC_CMD__SET_MODEL_PARAMS 
        .former     = NULL,
        .parser     = NULL
    },
    {   // 100      ICUBCANPROTO_POL_MC_CMD__GET_MODEL_PARAMS
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 101      ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_CURRENT_PID,
        .parser     = NULL
    }, 
    {   // 102      ICUBCANPROTO_POL_MC_CMD__GET_CURRENT_PID
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 103      ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PIDLIMITS 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_CURRENT_PIDLIMITS,
        .parser     = NULL
    },        
    {   // 104      ICUBCANPROTO_POL_MC_CMD__GET_CURRENT_PIDLIMITS  
        .former     = NULL,
        .parser     = NULL
    },
    {   // 105      ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PID
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_VELOCITY_PID,
        .parser     = NULL
    },
    {   // 106      ICUBCANPROTO_POL_MC_CMD__GET_VELOCITY_PID 
        .former     = NULL,
        .parser     = NULL
    },
    {   // 107      ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PIDLIMITS 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_VELOCITY_PID,
        .parser     = NULL
    },
    {   // 108      ICUBCANPROTO_POL_MC_CMD__GET_VELOCITY_PIDLIMITS
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 109      ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_CURRENT
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_DESIRED_CURRENT,
        .parser     = NULL
    },
    {   // 110      ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_CURRENT
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 111      ICUBCANPROTO_POL_MC_CMD__SET_PERIODIC_MSG_CONTENTS
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 112      ICUBCANPROTO_POL_MC_CMD__SET_I2T_PARAMS       
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_I2T_PARAMS,
        .parser     = NULL
    }, 
    {   // 113      ICUBCANPROTO_POL_MC_CMD__GET_I2T_PARAMS
        .former     = NULL,
        .parser     = NULL
    },        
    {   // 114      ICUBCANPROTO_POL_MC_CMD__SET_OPENLOOP_PARAMS 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_OPENLOOP_PARAMS,
        .parser     = NULL
    },
    {   // 115      ICUBCANPROTO_POL_MC_CMD__GET_OPENLOOP_PARAMS
        .former     = eocanprotMCpolling_former_POL_MC_CMD__GET_OPENLOOP_PARAMS,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__GET_OPENLOOP_PARAMS
    },
    {   // 116      ICUBCANPROTO_POL_MC_CMD__SET_INTERACTION_MODE 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_INTERACTION_MODE,
        .parser     = NULL
    },
    {   // 117      ICUBCANPROTO_POL_MC_CMD__GET_INTERACTION_MODE 
        .former     = NULL,
        .parser     = NULL
    },
    {   // 118      ICUBCANPROTO_POL_MC_CMD__SET_TCFILTER_TYPE
        .former     = NULL,
        .parser     = NULL
    },
    {   // 119      ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_CONFIG 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MOTOR_CONFIG,
        .parser     = NULL
    },
    {   // 120      ICUBCANPROTO_POL_MC_CMD__GET_TCFILTER_TYPE 
        .former     = NULL,
        .parser     = NULL
    }
};  EO_VERIFYsizeof(s_eo_canprot_functions_pollingMotorControl, (ICUBCANPROTO_POL_MC_CMD_MAXNUM)*sizeof(eOcanprot_functions_t));



static const uint8_t s_eo_canprot_functions_pollingMotorControl_maxnumber = sizeof(s_eo_canprot_functions_pollingMotorControl) / sizeof(eOcanprot_functions_t);



const eOcanprot_functions_t s_eo_canprot_functions_periodicMotorControl[] =
{
    {   // 000      ICUBCANPROTO_PER_MC_MSG__2FOC
        .former     = NULL,
        .parser     = eocanprotMCperiodic_parser_PER_MC_MSG__2FOC
    },
    {   // 001      ICUBCANPROTO_PER_MC_MSG__POSITION
        .former     = NULL,
        .parser     = eocanprotMCperiodic_parser_PER_MC_MSG__POSITION
    }, 
    {   // 002      ICUBCANPROTO_PER_MC_MSG__PID_VAL
        .former     = NULL,
        .parser     = eocanprotMCperiodic_parser_PER_MC_MSG__PID_VAL
    },
    {   // 003      ICUBCANPROTO_PER_MC_MSG__STATUS
        .former     = NULL,
        .parser     = eocanprotMCperiodic_parser_PER_MC_MSG__STATUS
    },
    {   // 004      ICUBCANPROTO_PER_MC_MSG__CURRENT
        .former     = NULL,
        .parser     = eocanprotMCperiodic_parser_PER_MC_MSG__CURRENT
    },
    {   // 005      ICUBCANPROTO_PER_MC_MSG__OVERFLOW
        .former     = NULL,
        .parser     = NULL
    },
    {   // 006      ICUBCANPROTO_PER_MC_MSG__PRINT
        .former     = NULL,
        .parser     = eocanprotMCperiodic_parser_PER_MC_MSG__PRINT
    },
    {   // 007      ICUBCANPROTO_PER_MC_MSG__VELOCITY
        .former     = NULL,
        .parser     = eocanprotMCperiodic_parser_PER_MC_MSG__VELOCITY
    },
    {   // 008      ICUBCANPROTO_PER_MC_MSG__PID_ERROR
        .former     = NULL,
        .parser     = eocanprotMCperiodic_parser_PER_MC_MSG__PID_ERROR
    },
    {   // 009      ICUBCANPROTO_PER_MC_MSG__DEBUG
        .former     = NULL,
        .parser     = NULL
    },
    {   // 010      ICUBCANPROTO_PER_MC_MSG__MOTOR_POSITION
        .former     = NULL,
        .parser     = NULL
    },
    {   // 011      ICUBCANPROTO_PER_MC_MSG__MOTOR_SPEED
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 012      ICUBCANPROTO_PER_MC_MSG__ADDITIONAL_STATUS
        .former     = NULL,
        .parser     = eocanprotMCperiodic_parser_PER_MC_MSG__ADDITIONAL_STATUS
    },
    {   // 013
        .former     = NULL,
        .parser     = NULL
    },
    {   // 014
        .former     = NULL,
        .parser     = NULL
    },
    {   // 015      ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT
        .former     = eocanprotMCperiodic_former_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT,
        .parser     = NULL
    }    
};  EO_VERIFYsizeof(s_eo_canprot_functions_periodicMotorControl, ((eocanprot_classperiodic_msgtypes_maxnumberof))*sizeof(eOcanprot_functions_t));


static const uint8_t s_eo_canprot_functions_periodicMotorControl_maxnumber = sizeof(s_eo_canprot_functions_periodicMotorControl) / sizeof(eOcanprot_functions_t);



const eOcanprot_functions_t s_eo_canprot_functions_pollingAnalogSensor[] =
{
    {   // 000      ICUBCANPROTO_POL_AS_CMD__NONE
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 001      ICUBCANPROTO_POL_AS_CMD__SET_IIR
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 002      
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 003      ICUBCANPROTO_POL_AS_CMD__SET_MATRIX_RC
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 004      ICUBCANPROTO_POL_AS_CMD__SET_CH_DAC
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 005      ICUBCANPROTO_POL_AS_CMD__SELECT_ACTIVE_CH
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 006      ICUBCANPROTO_POL_AS_CMD__CALIBRATE_OFFSET
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 007      ICUBCANPROTO_POL_AS_CMD__SET_TXMODE
        .former     = eocanprotASpolling_former_POL_AS_CMD__SET_TXMODE,
        .parser     = NULL
    }, 
    {   // 008      ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE
        .former     = eocanprotASpolling_former_POL_AS_CMD__SET_CANDATARATE,
        .parser     = NULL
    },  
    {   // 009      ICUBCANPROTO_POL_AS_CMD__SAVE2EE
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 010      ICUBCANPROTO_POL_AS_CMD__GET_MATRIX_RC
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 011      ICUBCANPROTO_POL_AS_CMD__GET_CH_DAC
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 012      ICUBCANPROTO_POL_AS_CMD__GET_CH_ADC
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 013      ICUBCANPROTO_POL_AS_CMD__FILTER_EN
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 014      ICUBCANPROTO_POL_AS_CMD__MUX_EN
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 015      ICUBCANPROTO_POL_AS_CMD__MUX_NUM
        .former     = NULL,
        .parser     = NULL
    },     
    {   // 016      ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION
        .former     = eocanprotASpolling_former_POL_AS_CMD__SET_RESOLUTION,
        .parser     = NULL
    },  
    {   // 017      ICUBCANPROTO_POL_AS_CMD__SET_MATRIX_G
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 018      ICUBCANPROTO_POL_AS_CMD__GET_MATRIX_G
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 019      ICUBCANPROTO_POL_AS_CMD__SET_CALIB_TARE
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 020      ICUBCANPROTO_POL_AS_CMD__GET_CALIB_TARE
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 021      ICUBCANPROTO_POL_AS_CMD__SET_CURR_TARE
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 22       ICUBCANPROTO_POL_AS_CMD__GET_CURR_TARE
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 023      ICUBCANPROTO_POL_AS_CMD__SET_FULL_SCALES
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 024      ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES
        .former     = eocanprotASpolling_former_POL_AS_CMD__GET_FULL_SCALES,
        .parser     = eocanprotASpolling_parser_POL_AS_CMD__GET_FULL_SCALES
    },  
    {   // 025      ICUBCANPROTO_POL_AS_CMD__SET_SERIAL_NO
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 026      ICUBCANPROTO_POL_AS_CMD__GET_SERIAL_NO
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 027      ICUBCANPROTO_POL_AS_CMD__GET_EEPROM_STATUS
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 028      ICUBCANPROTO_POL_AS_CMD__GET_FW_VERSION
        .former     = eocanprotASpolling_former_POL_AS_CMD__GET_FIRMWARE_VERSION,
        .parser     = eocanprotASpolling_parser_POL_AS_CMD__GET_FIRMWARE_VERSION
    },  
    {   // 029      
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 30       
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 031      
        .former     = NULL,
        .parser     = NULL
    },    
    {   // 032      
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 033           
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 034      
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 035      
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 036      
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 037      
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 038      
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 039      
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 040      
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 041      
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 042      
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 043      
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 044      
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 045      
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 046      
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 047
        .former     = NULL,
        .parser     = NULL
    },     
    {   // 048
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 049
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 050      ICUBCANPROTO_POL_AS_CMD__SET_BOARD_ADX
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 051      
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 052      
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 053
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 054
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 055      
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 056      
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 057
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 058
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 059
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 060      
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 061      
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 062      
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 063      
        .former     = NULL,
        .parser     = NULL
    },       
    {   // 064       
        .former     = NULL,
        .parser     = NULL 
    },
    {   // 065      
        .former     = NULL,
        .parser     = NULL
    },
    {   // 066       
        .former     = NULL,
        .parser     = NULL 
    },
    {   // 067       
        .former     = NULL,
        .parser     = NULL
    },
    {   // 068       
        .former     = NULL,
        .parser     = NULL
    },  
    {   // 069      
        .former     = NULL,
        .parser     = NULL
    },
    {   // 070      
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 071      
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 072       
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 073      
        .former     = NULL,
        .parser     = NULL
    },        
    {   // 074       
        .former     = NULL,
        .parser     = NULL
    },
    {   // 075      
        .former     = NULL,
        .parser     = NULL
    },
    {   // 076      ICUBCANPROTO_POL_SK_CMD__TACT_SETUP      
        .former     = eocanprotASpolling_former_POL_SK_CMD__TACT_SETUP,
        .parser     = NULL
    },
    {   // 077      ICUBCANPROTO_POL_SK_CMD__SET_BRD_CFG
        .former     = eocanprotASpolling_former_POL_SK_CMD__SET_BRD_CFG,
        .parser     = NULL
    },
    {   // 078      ICUBCANPROTO_POL_SK_CMD__TACT_SETUP2 
        .former     = eocanprotASpolling_former_POL_SK_CMD__TACT_SETUP2,
        .parser     = NULL
    },  
    {   // 079      ICUBCANPROTO_POL_SK_CMD__ACC_GYRO_SETUP
        .former     = eocanprotASpolling_former_POL_SK_CMD__ACC_GYRO_SETUP,
        .parser     = NULL
    },
    {   // 080      ICUBCANPROTO_POL_SK_CMD__SET_TRIANG_CFG 
        .former     = eocanprotASpolling_former_POL_SK_CMD__SET_TRIANG_CFG,
        .parser     = NULL
    }, 
    {   // 081      
        .former     = NULL,
        .parser     = NULL
    }
};  EO_VERIFYsizeof(s_eo_canprot_functions_pollingAnalogSensor, (82)*sizeof(eOcanprot_functions_t));



//const eOcanprot_functions_t s_eo_canprot_functions_pollingAnalogSensor[] =
//{
//    {   // 000 - > ICUBCANPROTO_POL_AS_CMD__SET_TXMODE = 0x07
//        .former     = eocanprotASpolling_former_00,
//        .parser     = eocanprotASpolling_parser_00
//    },
//    {   // 001 -> ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE = 0x08
//        .former     = eocanprotASpolling_former_01,
//        .parser     = eocanprotASpolling_parser_01
//    },
//    {   // 002 -> ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION = 0x10
//        .former     = eocanprotASpolling_former_02,
//        .parser     = eocanprotASpolling_parser_02
//    },
//    {   // 003 -> ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES = 0x18
//        .former     = eocanprotASpolling_former_02,
//        .parser     = eocanprotASpolling_parser_02        
//    },
//    {   // 004 -> ICUBCANPROTO_POL_SK_CMD__TACT_SETUP = 0x4c
//        .former     = eocanprotASpolling_former_02,
//        .parser     = eocanprotASpolling_parser_02        
//    }, 
//    {   // 005 -> ICUBCANPROTO_POL_SK_CMD__SET_BRD_CFG = 0x4d
//        .former     = eocanprotASpolling_former_02,
//        .parser     = eocanprotASpolling_parser_02        
//    }, 
//    {   // 006 -> ICUBCANPROTO_POL_SK_CMD__TACT_SETUP2 = 0x4e
//        .former     = eocanprotASpolling_former_02,
//        .parser     = eocanprotASpolling_parser_02        
//    }, 
//    {   // 007 -> ICUBCANPROTO_POL_SK_CMD__ACC_GYRO_SETUP = 0x4f
//        .former     = eocanprotASpolling_former_02,
//        .parser     = eocanprotASpolling_parser_02        
//    },   
//    {   // 008 -> ICUBCANPROTO_POL_SK_CMD__SET_TRIANG_CFG = 0x50
//        .former     = eocanprotASpolling_former_02,
//        .parser     = eocanprotASpolling_parser_02        
//    }     
//};  EO_VERIFYsizeof(s_eo_canprot_functions_pollingAnalogSensor, ((9))*sizeof(eOcanprot_functions_t));

static const uint8_t s_eo_canprot_functions_pollingAnalogSensor_maxnumber = sizeof(s_eo_canprot_functions_pollingAnalogSensor) / sizeof(eOcanprot_functions_t);


const eOcanprot_functions_t s_eo_canprot_functions_periodicAnalogSensor[] =
{
    {   // 000      ICUBCANPROTO_PER_AS_MSG__DIGITAL_GYROSCOPE
        .former     = NULL,
        .parser     = eocanprotASperiodic_parser_PER_AS_MSG__DIGITAL_GYROSCOPE
    },
    {   // 001      ICUBCANPROTO_PER_AS_MSG__DIGITAL_ACCELEROMETER
        .former     = NULL,
        .parser     = eocanprotASperiodic_parser_PER_AS_MSG__DIGITAL_ACCELEROMETER
    }, 
    {   // 002
        .former     = NULL,
        .parser     = NULL
    },
    {   // 003
        .former     = NULL,
        .parser     = NULL
    },
    {   // 004
        .former     = NULL,
        .parser     = NULL
    },
    {   // 005
        .former     = NULL,
        .parser     = NULL
    },
    {   // 006
        .former     = NULL,
        .parser     = NULL
    },
    {   // 007
        .former     = NULL,
        .parser     = NULL
    },
    {   // 008      ICUBCANPROTO_PER_AS_MSG__UNCALIBFORCE_VECTOR_DEBUGMODE
        .former     = NULL,
        .parser     = eocanprotASperiodic_parser_PER_AS_MSG__UNCALIBFORCE_VECTOR_DEBUGMODE
    },
    {   // 009      ICUBCANPROTO_PER_AS_MSG__UNCALIBTORQUE_VECTOR_DEBUGMODE
        .former     = NULL,
        .parser     = eocanprotASperiodic_parser_PER_AS_MSG__UNCALIBTORQUE_VECTOR_DEBUGMODE
    },
    {   // 010      ICUBCANPROTO_PER_AS_MSG__FORCE_VECTOR
        .former     = eocanprotASperiodic_former_PER_AS_MSG__FORCE_VECTOR,
        .parser     = eocanprotASperiodic_parser_PER_AS_MSG__FORCE_VECTOR
    },
    {   // 011      ICUBCANPROTO_PER_AS_MSG__TORQUE_VECTOR
        .former     = eocanprotASperiodic_former_PER_AS_MSG__TORQUE_VECTOR,
        .parser     = eocanprotASperiodic_parser_PER_AS_MSG__TORQUE_VECTOR
    }, 
    {   // 012      ICUBCANPROTO_PER_AS_MSG__HES0TO6
        .former     = NULL,
        .parser     = eocanprotASperiodic_parser_PER_AS_MSG__HES0TO6
    },
    {   // 013      ICUBCANPROTO_PER_AS_MSG__HES7TO14
        .former     = NULL,
        .parser     = eocanprotASperiodic_parser_PER_AS_MSG__HES7TO14
    },
    {   // 014
        .former     = NULL,
        .parser     = NULL
    },
    {   // 015
        .former     = NULL,
        .parser     = NULL
    }   
    
};   EO_VERIFYsizeof(s_eo_canprot_functions_periodicAnalogSensor, ((eocanprot_classperiodic_msgtypes_maxnumberof))*sizeof(eOcanprot_functions_t));

static const uint8_t s_eo_canprot_functions_periodicAnalogSensor_maxnumber = sizeof(s_eo_canprot_functions_periodicAnalogSensor) / sizeof(eOcanprot_functions_t);



const eOcanprot_functions_t s_eo_canprot_functions_periodicSkin[] =
{   // so far we manage every skin messages with only one parser function ...
    {
        .former     = NULL,
        .parser     = eocanprotSKperiodic_parser_ANY_PERIODIC_SKIN_MSG
    }
};   EO_VERIFYsizeof(s_eo_canprot_functions_periodicSkin, ((1))*sizeof(eOcanprot_functions_t));

static const uint8_t s_eo_canprot_functions_periodicSkin_maxnumber = sizeof(s_eo_canprot_functions_periodicSkin) / sizeof(eOcanprot_functions_t);



const eOcanprot_functions_t s_eo_canprot_functions_periodicInertialSensor[] =
{
    {   // 000      ICUBCANPROTO_PER_IS_MSG__DIGITAL_GYROSCOPE
        .former     = NULL,
        .parser     = eocanprotINperiodic_parser_PER_IS_MSG__DIGITAL_GYROSCOPE
    },
    {   // 001      ICUBCANPROTO_PER_IS_MSG__DIGITAL_ACCELEROMETER
        .former     = NULL,
        .parser     = eocanprotINperiodic_parser_PER_IS_MSG__DIGITAL_ACCELEROMETER
    }, 
    {   // 002      ICUBCANPROTO_PER_IS_MSG__ANALOG_ACCELEROMETER
        .former     = NULL,
        .parser     = NULL, //eocanprotINperiodic_parser_PER_IS_MSG__ANALOG_ACCELEROMETER
    },
    {   // 003
        .former     = NULL,
        .parser     = NULL
    },
    {   // 004
        .former     = NULL,
        .parser     = NULL
    },
    {   // 005
        .former     = NULL,
        .parser     = NULL
    },
    {   // 006
        .former     = NULL,
        .parser     = NULL
    },
    {   // 007
        .former     = NULL,
        .parser     = NULL
    },
    {   // 008      
        .former     = NULL,
        .parser     = NULL
    },
    {   // 009      
        .former     = NULL,
        .parser     = NULL
    },
    {   // 010      
        .former     = NULL,
        .parser     = NULL
    },
    {   // 011      
        .former     = NULL,
        .parser     = NULL
    }, 
    {   // 012      
        .former     = NULL,
        .parser     = NULL
    },
    {   // 013      
        .former     = NULL,
        .parser     = NULL
    },
    {   // 014
        .former     = NULL,
        .parser     = NULL
    },
    {   // 015
        .former     = NULL,
        .parser     = NULL
    } 
};   EO_VERIFYsizeof(s_eo_canprot_functions_periodicInertialSensor, ((eocanprot_classperiodic_msgtypes_maxnumberof))*sizeof(eOcanprot_functions_t));

static const uint8_t s_eo_canprot_functions_periodicInertialSensor_maxnumber = sizeof(s_eo_canprot_functions_periodicInertialSensor) / sizeof(eOcanprot_functions_t);




const eOcanprot_functions_t * const eo_canprot_lut_functions[] =
{   
    (const eOcanprot_functions_t *)&s_eo_canprot_functions_pollingMotorControl,   
    (const eOcanprot_functions_t *)&s_eo_canprot_functions_periodicMotorControl,        
    (const eOcanprot_functions_t *)&s_eo_canprot_functions_pollingAnalogSensor,
    (const eOcanprot_functions_t *)&s_eo_canprot_functions_periodicAnalogSensor,
    (const eOcanprot_functions_t *)&s_eo_canprot_functions_periodicSkin,
    (const eOcanprot_functions_t *)&s_eo_canprot_functions_periodicInertialSensor
};  EO_VERIFYsizeof(eo_canprot_lut_functions, (eocanprot_msgclass_maxvalue+1)*sizeof(const eOcanprot_functions_t *));  



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheCANprotocol * eo_canprot_Initialise(const eOcanprot_cfg_t *canprotcfg)
{
    if(NULL == canprotcfg)
    {
        canprotcfg = &eo_canprot_DefaultCfg;    
    }

    memcpy(&s_eo_canprot_singleton.config, canprotcfg, sizeof(eOcanprot_cfg_t));

    return(&s_eo_canprot_singleton);
}


extern EOtheCANprotocol* eo_canprot_GetHandle(void) 
{
    return(&s_eo_canprot_singleton);
}


extern eOresult_t eo_canprot_Parse(EOtheCANprotocol *p, eOcanframe_t *frame, eOcanport_t port)
{
    eOresult_t res = eores_OK;
    eOcanprot_fp_parser_t parserfunction = NULL;
//    uint8_t msgindex = 0;
//    uint8_t msgclass = 0;
    
    uint8_t index0 = 0;
    uint8_t index1 = 0;
    
    // 0. verify we dont have NULL data
    if((NULL == p) || (NULL == frame))
    {
        return(eores_NOK_nullpointer);
    }
    
    // discard messages from some addresses
    if(eobool_true == s_eo_canprot_filter_sourceaddress(frame))
    {
        return(eores_OK);
    }     
    
    if(eobool_true == s_eo_canprot_isit_exception(frame, port, &res))
    {   // in here we manage for instance the mais error.
        return(res);
    }    
    
    // verify the length is ok.
    if(frame->size > 8)
    {
        return(eores_NOK_generic);
    }
    else if(0 == frame->size)
    {
        return(s_eo_canprot_parse0length(frame, port));
    }
          
    
    if(eores_OK != s_eo_canprot_get_indices(frame, NULL, &index0, &index1))
    {
        return(eores_NOK_generic);
    }
    
    parserfunction = eo_canprot_lut_functions[index0][index1].parser;
    
    if(NULL == parserfunction)
    {
        return(eores_NOK_generic);
    }
    
    res = parserfunction(frame, port); 
    
//    if(eores_OK != res)
//    {   // search in here cases such as mais error etc. maybe better to move it up just 
//        res = s_eo_canprot_search4exceptions(frame, port);
//    }
    
    return(res);
}
 
    

extern eOresult_t eo_canprot_Form(EOtheCANprotocol *p, eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    eOresult_t res = eores_OK;
    
    uint8_t index0 = 0;
    uint8_t index1 = 0;
    
    eOcanprot_fp_former_t formerfunction = NULL; 


    if(eores_OK != s_eo_canprot_get_indices(NULL, descriptor, &index0, &index1))
    {
        return(eores_NOK_generic);
    }    
    
    formerfunction = eo_canprot_lut_functions[index0][index1].former;
    
    if(NULL == formerfunction)
    {
        return(eores_NOK_generic);
    }
    
    res = formerfunction(descriptor, frame); 
    
    return(res);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eo_canprot_parse0length(eOcanframe_t *frame, eOcanport_t port)
{
    // the only zero-length can frames are those coming as ack sent by strain or mais for a polling message.
    // we just check that the message is class eocanprot_msgclass_pollingAnalogSensor and is for address 0 (the EMS).
    // we avoid doing the extra check that the sender address is from a mais/strain to save computations
    if((eocanprot_msgclass_pollingAnalogSensor == EOCANPROT_FRAME_GET_CLASS(frame)) && (0 == EOCANPROT_FRAME_POLLING_GET_DESTINATION(frame)))
    {
        eOcanmap_location_t bloc = {0};
        bloc.port = port;
        bloc.addr = EOCANPROT_FRAME_GET_SOURCE(frame);
        //loc.insideindex = eocanmap_insideindex_none;
        const eOcanmap_board_extended_t *board = eo_canmap_GetBoard(eo_canmap_GetHandle(), bloc);
        if((eobrd_cantype_strain == board->board.props.type) || (eobrd_cantype_mais == board->board.props.type))
        {
            return(eores_OK);
        }
    }

    return(eores_NOK_generic);
}

//static eOresult_t s_eo_canprot_search4exceptions(eOcanframe_t *frame, eOcanport_t port)
//{
//    // in here is put ok only if we find an exception    
//     #warning -> marco.accame: in eo_canprot_Parse() u must consider also the case of mais error. it is an illegal frame, thus maybe we do it only if the following cannot be done.
//    
//    // if we find an exception, then found is 
//    return(eores_OK);
//}


static eObool_t s_eo_canprot_filter_sourceaddress(eOcanframe_t *frame)
{    
    if(0 == EOCANPROT_FRAME_GET_SOURCE(frame))
    {
        return(eobool_true);
    }

    return(eobool_false);   
}


static eObool_t s_eo_canprot_isit_exception(eOcanframe_t *frame, eOcanport_t port, eOresult_t *res)
{
    // in here is put ok only if we find an exception
    
    // mais bug
    // get location of mais 
    static eObool_t mais_is_present = eobool_false;
    static eObool_t mais_is_searched = eobool_false;    
    static eOcanmap_location_t loc = {0};
    if(eobool_false == mais_is_searched)
    {   // search mais only at first execution of the function
        mais_is_searched = eobool_true;
        eOprotID32_t id32  = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_mais, 0, eoprot_tag_none);
        if(eores_OK == eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), id32, &loc, NULL, NULL))
        {   // have found a mais
            mais_is_present = eobool_true;
        }
    }
    
    if(eobool_false == mais_is_present)
    {
        return(eobool_false);
    } 

    if((loc.port == port) && (EOCANPROT_FRAME_GET_SOURCE(frame) == loc.addr))
    {   // ok, it is from mais
        // it is a mais bug only if class is either motor polling or motor periodic
        uint8_t rxclass = EOCANPROT_FRAME_GET_CLASS(frame);
        uint8_t classismotorpolling  = (rxclass == ICUBCANPROTO_CLASS_POLLING_MOTORCONTROL) ? 1 : 0;
        uint8_t classismotorperiodic = (rxclass == ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL) ? 1 : 0;

        if(classismotorpolling || classismotorperiodic) 
        {
            eOerrmanDescriptor_t des = {0};
            des.code = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_rxmaisbug);
            des.par16 = (frame->id & 0x0fff) | ((frame->size & 0x000f) << 12);
            des.par64 = eo_common_canframe_data2u64(frame);
            des.sourcedevice = (eOcanport1 == port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
            des.sourceaddress = EOCANPROT_FRAME_GET_SOURCE(frame);

            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &des);
            
            *res = eores_OK; 
            return(eobool_true);
        }                
    }
    
         
    // if we find an exception, then we return true and we put the relevant result in *res 
    return(eobool_false);
}

static eOresult_t s_eo_canprot_get_indices(eOcanframe_t *frame, eOcanprot_descriptor_t *des, uint8_t *index0, uint8_t *index1)
{
    eOresult_t res = eores_NOK_generic;
    
    uint8_t msgclass = 0;
    uint8_t msgtype = 0;
    eObool_t parsemode = eobool_true;
    
    // i retrieve the msgclass and the id from the frame in case of parsing, and from the descriptor in case of forming
    if(NULL != frame)
    {
        parsemode = eobool_true;
        msgclass = EOCANPROT_FRAME_GET_CLASS(frame);
    }
    else if(NULL != des)
    {
        parsemode = eobool_false;
        msgclass = des->cmd.class;
    }
    else
    {   // only one between them is non NULL, not both
        return(eores_NOK_generic);
    }
    
    if(msgclass > eocanprot_msgclass_maxvalue)
    {
        return(eores_NOK_generic);
    }
    
    *index0 = 0;
    *index1 = 0;
    
    switch(msgclass)
    {
        case eocanprot_msgclass_pollingMotorControl:
        {
            *index0 = msgclass; 
            if(eobool_true == parsemode)
            {
                msgtype = EOCANPROT_FRAME_POLLING_GET_TYPE(frame);  
            }
            else
            {
                msgtype = des->cmd.type;
            }

            // marco.accame: the msgtype is used as an index to a big sparse table. this solution gives quicker access.
            *index1 = msgtype;
            res = eores_OK;
                       
            if(*index1 >= s_eo_canprot_functions_pollingMotorControl_maxnumber)
            {   // extra check
                res = eores_NOK_generic;
            } 
        } break;
        
        case eocanprot_msgclass_pollingAnalogSensor: // or eocanprot_msgclass_pollingSkin, both equal to 2
        {
            *index0 = msgclass; 
            if(eobool_true == parsemode)
            {
                msgtype = EOCANPROT_FRAME_POLLING_GET_TYPE(frame); 
            }
            else
            {
                msgtype = des->cmd.type;
            }  

            // marco.accame: the msgtype is used as an index to a big sparse table. this solution gives quicker access.
            *index1 = msgtype;
            res = eores_OK;
                       
            if(*index1 >= s_eo_canprot_functions_pollingAnalogSensor_maxnumber)
            {   // extra check
                res = eores_NOK_generic;
            } 

            
//            // marco.accame: the used IDs are only 4 out of a maximum of 50 something. so, ... just get the used ids and remaps them into 0, 1, 2, 3
//            //               actually there are also those of the skin polling commands. they are 5. i write a big switch-case
//            switch(msgtype)
//            {
//                case ICUBCANPROTO_POL_AS_CMD__SET_TXMODE:           { *index1 = 0; res = eores_OK; } break;
//                case ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE:      { *index1 = 1; res = eores_OK; } break;
//                case ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION:       { *index1 = 2; res = eores_OK; } break;
//                case ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES:      { *index1 = 3; res = eores_OK; } break;
//                case ICUBCANPROTO_POL_SK_CMD__TACT_SETUP:           { *index1 = 4; res = eores_OK; } break;
//                case ICUBCANPROTO_POL_SK_CMD__SET_BRD_CFG:          { *index1 = 5; res = eores_OK; } break;
//                case ICUBCANPROTO_POL_SK_CMD__TACT_SETUP2:          { *index1 = 6; res = eores_OK; } break;
//                case ICUBCANPROTO_POL_SK_CMD__ACC_GYRO_SETUP:       { *index1 = 7; res = eores_OK; } break;
//                case ICUBCANPROTO_POL_SK_CMD__SET_TRIANG_CFG:       { *index1 = 8; res = eores_OK; } break;
//                default: { res = eores_NOK_generic; } break;            
//            }
//            
//            // marco.accame: i have put an extra check ... but PLEASE ... be careful if you change things. the index1 must not go beyond s_eo_canprot_functions_pollingAnalogSensor.
//            if(*index1 >= s_eo_canprot_functions_pollingAnalogSensor_maxnumber)
//            {   // extra check
//                res = eores_NOK_generic;
//            }           
        } break;

        case eocanprot_msgclass_periodicMotorControl:
        {
            *index0 = msgclass;
            if(eobool_true == parsemode)
            {
                msgtype = EOCANPROT_FRAME_PERIODIC_GET_TYPE(frame); 
            }
            else
            {
                msgtype = des->cmd.type;
            }  
            
            *index1 = msgtype; 
            // marco.accame: at most they are 16 ... we don't remap. if any hole we manage with NULL function pointers. 
            res = eores_OK; 
            
            if(*index1 >= s_eo_canprot_functions_periodicMotorControl_maxnumber)
            {   // extra check
                res = eores_NOK_generic;
            }            
        } break;
        
        case eocanprot_msgclass_periodicAnalogSensor:
        {
            *index0 = msgclass;
            if(eobool_true == parsemode)
            {
                msgtype = EOCANPROT_FRAME_PERIODIC_GET_TYPE(frame); 
            }
            else
            {
                msgtype = des->cmd.type;
            }              
            
            *index1 = msgtype;    
            // marco.accame: at most they are 16 ... we don't remap. if any hole we manage with NULL function pointers. 
            res = eores_OK;   
            if(*index1 >= s_eo_canprot_functions_periodicAnalogSensor_maxnumber)
            {   // extra check
                res = eores_NOK_generic;
            }             
        } break;
        
        case eocanprot_msgclass_periodicSkin:
        {
            *index0 = msgclass;
            *index1 = 0;        // there is only one type of message: or rather ... we use only one funtion to parse.  
            res = eores_OK;
            
            if(*index1 >= s_eo_canprot_functions_periodicSkin_maxnumber)
            {   // extra check
                res = eores_NOK_generic;
            }             
        } break;
        
        case eocanprot_msgclass_periodicInertialSensor:
        {
            *index0 = msgclass;
            if(eobool_true == parsemode)
            {
                msgtype = EOCANPROT_FRAME_PERIODIC_GET_TYPE(frame); 
            }
            else
            {
                msgtype = des->cmd.type;
            }              
            
            *index1 = msgtype;    
            // marco.accame: at most they are 16 ... we don't remap. if any hole we manage with NULL function pointers. 
            res = eores_OK;   
            if(*index1 >= s_eo_canprot_functions_periodicInertialSensor_maxnumber)
            {   // extra check
                res = eores_NOK_generic;
            }             
        } break;
        
        default:
        {
            res = eores_NOK_generic;
        } break;
        
    }
    
    return(res);

}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





