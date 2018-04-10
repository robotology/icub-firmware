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
	EO_INIT(.config)
    {   
        0
    },
    EO_INIT(.tobedefined)   0
};




const eOcanprot_functions_t s_eo_canprot_functions_pollingMotorControl[] =
{
    {   // 000      ICUBCANPROTO_POL_MC_CMD__NO_MESSAGE
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 001      ICUBCANPROTO_POL_MC_CMD__CONTROLLER_RUN
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__CONTROLLER_RUN,
        EO_INIT(.parser) NULL
    }, 
    {   // 002      ICUBCANPROTO_POL_MC_CMD__CONTROLLER_IDLE
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__CONTROLLER_IDLE,
        EO_INIT(.parser) NULL
    },  
    {   // 003
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 004      ICUBCANPROTO_POL_MC_CMD__CALIBRATE_ENCODER
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__CALIBRATE_ENCODER,
        EO_INIT(.parser) NULL
    },  
    {   // 005      ICUBCANPROTO_POL_MC_CMD__ENABLE_PWM_PAD
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__ENABLE_PWM_PAD,
        EO_INIT(.parser) NULL
    }, 
    {   // 006      ICUBCANPROTO_POL_MC_CMD__DISABLE_PWM_PAD
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__DISABLE_PWM_PAD,
        EO_INIT(.parser) NULL
    },  
    {   // 007      ICUBCANPROTO_POL_MC_CMD__GET_CONTROL_MODE
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__GET_CONTROL_MODE,
        EO_INIT(.parser) eocanprotMCpolling_parser_POL_MC_CMD__GET_CONTROL_MODE
    }, 
    {   // 008      ICUBCANPROTO_POL_MC_CMD__MOTION_DONE
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__MOTION_DONE,
        EO_INIT(.parser) eocanprotMCpolling_parser_POL_MC_CMD__MOTION_DONE
    },  
    {   // 009      ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_CONTROL_MODE,
        EO_INIT(.parser) NULL
    }, 
    {   // 010
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 011
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 012
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 013
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 014      ICUBCANPROTO_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT,
        EO_INIT(.parser) NULL
    },  
    {   // 015      ICUBCANPROTO_POL_MC_CMD__SET_MAX_MOTOR_POS
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_MOTOR_POS,
        EO_INIT(.parser) NULL
    },     
    {   // 016      ICUBCANPROTO_POL_MC_CMD__SET_MIN_MOTOR_POS
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_MIN_MOTOR_POS,
        EO_INIT(.parser) NULL
    },  
    {   // 017
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 018      ICUBCANPROTO_POL_MC_CMD__SET_DEBUG_PARAM
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 019      ICUBCANPROTO_POL_MC_CMD__GET_DEBUG_PARAM
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 020      ICUBCANPROTO_POL_MC_CMD__GET_ENCODER_POSITION
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 021      ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_POSITION
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 22       ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_POSITION
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 023      ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_VELOCITY
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 024      ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_VELOCITY
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 025      ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_ACCELER
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 026      ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_ACCELER
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 027      ICUBCANPROTO_POL_MC_CMD__POSITION_MOVE
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__POSITION_MOVE,
        EO_INIT(.parser) NULL
    }, 
    {   // 028      ICUBCANPROTO_POL_MC_CMD__VELOCITY_MOVE
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__VELOCITY_MOVE,
        EO_INIT(.parser) NULL
    },  
    {   // 029      ICUBCANPROTO_POL_MC_CMD__SET_ENCODER_POSITION
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 30       ICUBCANPROTO_POL_MC_CMD__SET_P_GAIN
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 031      ICUBCANPROTO_POL_MC_CMD__GET_P_GAIN
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },    
    {   // 032      ICUBCANPROTO_POL_MC_CMD__SET_D_GAIN
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 033      ICUBCANPROTO_POL_MC_CMD__GET_D_GAIN     
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 034      ICUBCANPROTO_POL_MC_CMD__SET_I_GAIN
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 035      ICUBCANPROTO_POL_MC_CMD__GET_I_GAIN
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 036      ICUBCANPROTO_POL_MC_CMD__SET_ILIM_GAIN
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 037      ICUBCANPROTO_POL_MC_CMD__GET_ILIM_GAIN
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 038      ICUBCANPROTO_POL_MC_CMD__SET_OFFSET
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 039      ICUBCANPROTO_POL_MC_CMD__GET_OFFSET
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 040      ICUBCANPROTO_POL_MC_CMD__SET_SCALE
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 041      ICUBCANPROTO_POL_MC_CMD__GET_SCALE
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 042      ICUBCANPROTO_POL_MC_CMD__SET_TLIM
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 043      ICUBCANPROTO_POL_MC_CMD__GET_TLIM
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 044      ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_TORQUE
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_DESIRED_TORQUE,
        EO_INIT(.parser) NULL
    },  
    {   // 045      ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_TORQUE
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__GET_DESIRED_TORQUE,
        EO_INIT(.parser) NULL
    }, 
    {   // 046      ICUBCANPROTO_POL_MC_CMD__STOP_TRAJECTORY
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__STOP_TRAJECTORY,
        EO_INIT(.parser) NULL
    },  
    {   // 047      ICUBCANPROTO_POL_MC_CMD__SET_PWM_LIMIT
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_PWM_LIMIT,
        EO_INIT(.parser) NULL
    },     
    {   // 048      ICUBCANPROTO_POL_MC_CMD__GET_PWM_LIMIT
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__GET_PWM_LIMIT,
        EO_INIT(.parser) eocanprotMCpolling_parser_POL_MC_CMD__GET_PWM_LIMIT,
    },  
    {   // 049
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 050      ICUBCANPROTO_POL_MC_CMD__SET_BOARD_ID
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 051      ICUBCANPROTO_POL_MC_CMD__GET_BOARD_ID
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 052      ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_SOURCE
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 053
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 054
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 055      ICUBCANPROTO_POL_MC_CMD__GET_PID_ERROR
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 056      ICUBCANPROTO_POL_MC_CMD__GET_CALIBRATION_STATUS
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 057
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 058
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 059
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 060      ICUBCANPROTO_POL_MC_CMD__GET_ERROR_STATUS
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 061      ICUBCANPROTO_POL_MC_CMD__GET_ENCODER_VELOCITY
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 062      ICUBCANPROTO_POL_MC_CMD__SET_COMMAND_POSITION
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_COMMAND_POSITION,
        EO_INIT(.parser) NULL
    },  
    {   // 063      ICUBCANPROTO_POL_MC_CMD__GET_PID_OUTPUT
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },       
    {   // 064      ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION 
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_MIN_POSITION,
        EO_INIT(.parser) NULL 
    },
    {   // 065      ICUBCANPROTO_POL_MC_CMD__GET_MIN_POSITION
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__GET_MIN_POSITION,
        EO_INIT(.parser) eocanprotMCpolling_parser_POL_MC_CMD__GET_MIN_POSITION
    },
    {   // 066      ICUBCANPROTO_POL_MC_CMD__SET_MAX_POSITION 
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        EO_INIT(.parser) NULL //unsupported
    },
    {   // 067      ICUBCANPROTO_POL_MC_CMD__GET_MAX_POSITION 
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__GET_MAX_POSITION,
        EO_INIT(.parser) eocanprotMCpolling_parser_POL_MC_CMD__GET_MAX_POSITION
    },
    {   // 068      ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY 
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_VELOCITY,
        EO_INIT(.parser) NULL
    },  
    {   // 069      ICUBCANPROTO_POL_MC_CMD__GET_MAX_VELOCITY
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 070      ICUBCANPROTO_POL_MC_CMD__GET_ACTIVE_ENCODER_POSITION
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 071      ICUBCANPROTO_POL_MC_CMD__SET_ACTIVE_ENCODER_POSITION
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 072      ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT 
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_CURRENT_LIMIT,
        EO_INIT(.parser) NULL
    }, 
    {   // 073      ICUBCANPROTO_POL_MC_CMD__SET_BCAST_POLICY
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_BCAST_POLICY,
        EO_INIT(.parser) NULL
    },        
    {   // 074      ICUBCANPROTO_POL_MC_CMD__SET_VEL_SHIFT 
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_VEL_SHIFT,
        EO_INIT(.parser) NULL
    },
    {   // 075      ICUBCANPROTO_POL_MC_CMD__SET_OFFSET_ABS_ENCODER
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 076      ICUBCANPROTO_POL_MC_CMD__GET_OFFSET_ABS_ENCODER      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 077      ICUBCANPROTO_POL_MC_CMD__SET_SMOOTH_PID
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 078      ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PID 
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_TORQUE_PID,
        EO_INIT(.parser) NULL
    },  
    {   // 079      ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PID
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__GET_TORQUE_PID,
        EO_INIT(.parser) eocanprotMCpolling_parser_POL_MC_CMD__GET_TORQUE_PID
    },
    {   // 080      ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PIDLIMITS 
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_TORQUE_PIDLIMITS,
        EO_INIT(.parser) NULL
    }, 
    {   // 081      ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PIDLIMITS
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__GET_TORQUE_PIDLIMITS,
        EO_INIT(.parser) eocanprotMCpolling_parser_POL_MC_CMD__GET_TORQUE_PIDLIMITS
    }, 
    {   // 082      ICUBCANPROTO_POL_MC_CMD__SET_POS_PID 
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_POS_PID,
        EO_INIT(.parser) NULL
    }, 
    {   // 083      ICUBCANPROTO_POL_MC_CMD__GET_POS_PID
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__GET_POS_PID,
        EO_INIT(.parser) eocanprotMCpolling_parser_POL_MC_CMD__GET_POS_PID
    },        
    {   // 084      ICUBCANPROTO_POL_MC_CMD__SET_POS_PIDLIMITS 
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_POS_PIDLIMITS,
        EO_INIT(.parser) NULL
    },
    {   // 085      ICUBCANPROTO_POL_MC_CMD__GET_POS_PIDLIMITS
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__GET_POS_PIDLIMITS,
        EO_INIT(.parser) eocanprotMCpolling_parser_POL_MC_CMD__GET_POS_PIDLIMITS
    },
    {   // 086      ICUBCANPROTO_POL_MC_CMD__SET_VEL_TIMEOUT
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_VEL_TIMEOUT,
        EO_INIT(.parser) NULL
    },
    {   // 087      ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_PARAMS
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_IMPEDANCE_PARAMS,
        EO_INIT(.parser) NULL
    },
    {   // 088      ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_PARAMS 
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__GET_IMPEDANCE_PARAMS,
        EO_INIT(.parser) eocanprotMCpolling_parser_POL_MC_CMD__GET_IMPEDANCE_PARAMS
    },  
    {   // 089      ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_OFFSET 
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_IMPEDANCE_OFFSET,
        EO_INIT(.parser) NULL
    },
    {   // 090      ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_OFFSET 
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__GET_IMPEDANCE_OFFSET,
        EO_INIT(.parser) eocanprotMCpolling_parser_POL_MC_CMD__GET_IMPEDANCE_OFFSET
    }, 
    {   // 091      ICUBCANPROTO_POL_MC_CMD__GET_FIRMWARE_VERSION 
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__GET_FIRMWARE_VERSION,
        EO_INIT(.parser) eocanprotMCpolling_parser_POL_MC_CMD__GET_FIRMWARE_VERSION
    }, 
    {   // 092      ICUBCANPROTO_POL_MC_CMD__SET_OPTICAL_ENC_RATIO 
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 093      ICUBCANPROTO_POL_MC_CMD__SET_POS_STICTION_PARAMS
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_POS_STICTION_PARAMS,
        EO_INIT(.parser) NULL
    },        
    {   // 094      ICUBCANPROTO_POL_MC_CMD__GET_POS_STICTION_PARAMS        
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__GET_POS_STICTION_PARAMS,
        EO_INIT(.parser) eocanprotMCpolling_parser_POL_MC_CMD__GET_POS_STICTION_PARAMS
    },
    {   // 095      ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_STICTION_PARAMS
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_TORQUE_STICTION_PARAMS,
        EO_INIT(.parser) NULL
    },
    {   // 096      ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_STICTION_PARAMS
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__GET_TORQUE_STICTION_PARAMS,
        EO_INIT(.parser) eocanprotMCpolling_parser_POL_MC_CMD__GET_TORQUE_STICTION_PARAMS
    },
    {   // 097      ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_PARAMS 
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 098      ICUBCANPROTO_POL_MC_CMD__GET_MOTOR_PARAMS
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 099      ICUBCANPROTO_POL_MC_CMD__SET_MODEL_PARAMS 
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 100      ICUBCANPROTO_POL_MC_CMD__GET_MODEL_PARAMS
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 101      ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID 
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_CURRENT_PID,
        EO_INIT(.parser) NULL
    }, 
    {   // 102      ICUBCANPROTO_POL_MC_CMD__GET_CURRENT_PID
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 103      ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PIDLIMITS 
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_CURRENT_PIDLIMITS,
        EO_INIT(.parser) NULL
    },        
    {   // 104      ICUBCANPROTO_POL_MC_CMD__GET_CURRENT_PIDLIMITS  
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 105      ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PID
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_VELOCITY_PID,
        EO_INIT(.parser) NULL
    },
    {   // 106      ICUBCANPROTO_POL_MC_CMD__GET_VELOCITY_PID 
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 107      ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PIDLIMITS 
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_VELOCITY_PID,
        EO_INIT(.parser) NULL
    },
    {   // 108      ICUBCANPROTO_POL_MC_CMD__GET_VELOCITY_PIDLIMITS
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 109      ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_CURRENT
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_DESIRED_CURRENT,
        EO_INIT(.parser) NULL
    },
    {   // 110      ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_CURRENT
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 111      ICUBCANPROTO_POL_MC_CMD__SET_PERIODIC_MSG_CONTENTS
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 112      ICUBCANPROTO_POL_MC_CMD__SET_I2T_PARAMS       
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_I2T_PARAMS,
        EO_INIT(.parser) NULL
    }, 
    {   // 113      ICUBCANPROTO_POL_MC_CMD__GET_I2T_PARAMS
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },        
    {   // 114      ICUBCANPROTO_POL_MC_CMD__SET_OPENLOOP_PARAMS 
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_OPENLOOP_PARAMS,
        EO_INIT(.parser) NULL
    },
    {   // 115      ICUBCANPROTO_POL_MC_CMD__GET_OPENLOOP_PARAMS
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__GET_OPENLOOP_PARAMS,
        EO_INIT(.parser) eocanprotMCpolling_parser_POL_MC_CMD__GET_OPENLOOP_PARAMS
    },
    {   // 116      ICUBCANPROTO_POL_MC_CMD__SET_INTERACTION_MODE 
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_INTERACTION_MODE,
        EO_INIT(.parser) NULL
    },
    {   // 117      ICUBCANPROTO_POL_MC_CMD__GET_INTERACTION_MODE 
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 118      ICUBCANPROTO_POL_MC_CMD__SET_TCFILTER_TYPE
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 119      ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_CONFIG 
        EO_INIT(.former) eocanprotMCpolling_former_POL_MC_CMD__SET_MOTOR_CONFIG,
        EO_INIT(.parser) NULL
    },
    {   // 120      ICUBCANPROTO_POL_MC_CMD__GET_TCFILTER_TYPE 
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }
};  EO_VERIFYsizeof(s_eo_canprot_functions_pollingMotorControl, (ICUBCANPROTO_POL_MC_CMD_MAXNUM)*sizeof(eOcanprot_functions_t))



static const uint8_t s_eo_canprot_functions_pollingMotorControl_maxnumber = sizeof(s_eo_canprot_functions_pollingMotorControl) / sizeof(eOcanprot_functions_t);



const eOcanprot_functions_t s_eo_canprot_functions_periodicMotorControl[] =
{
    {   // 000      ICUBCANPROTO_PER_MC_MSG__2FOC
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotMCperiodic_parser_PER_MC_MSG__2FOC
    },
    {   // 001      ICUBCANPROTO_PER_MC_MSG__POSITION
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotMCperiodic_parser_PER_MC_MSG__POSITION
    }, 
    {   // 002      ICUBCANPROTO_PER_MC_MSG__PID_VAL
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotMCperiodic_parser_PER_MC_MSG__PID_VAL
    },
    {   // 003      ICUBCANPROTO_PER_MC_MSG__STATUS
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotMCperiodic_parser_PER_MC_MSG__STATUS
    },
    {   // 004      ICUBCANPROTO_PER_MC_MSG__CURRENT
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotMCperiodic_parser_PER_MC_MSG__CURRENT
    },
    {   // 005      ICUBCANPROTO_PER_MC_MSG__OVERFLOW
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 006      ICUBCANPROTO_PER_MC_MSG__PRINT
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotMCperiodic_parser_PER_MC_MSG__PRINT
    },
    {   // 007      ICUBCANPROTO_PER_MC_MSG__VELOCITY
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotMCperiodic_parser_PER_MC_MSG__VELOCITY
    },
    {   // 008      ICUBCANPROTO_PER_MC_MSG__PID_ERROR
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotMCperiodic_parser_PER_MC_MSG__PID_ERROR
    },
    {   // 009      ICUBCANPROTO_PER_MC_MSG__DEBUG
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotMCperiodic_parser_PER_MC_MSG__DEBUG
    },
    {   // 010      ICUBCANPROTO_PER_MC_MSG__MOTOR_POSITION
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotMCperiodic_parser_PER_MC_MSG__MOTOR_POSITION
    },
    {   // 011      ICUBCANPROTO_PER_MC_MSG__MOTOR_SPEED
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 012      ICUBCANPROTO_PER_MC_MSG__ADDITIONAL_STATUS
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotMCperiodic_parser_PER_MC_MSG__ADDITIONAL_STATUS
    },
    {   // 013
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 014
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 015      ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT
        EO_INIT(.former) eocanprotMCperiodic_former_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT,
        EO_INIT(.parser) NULL
    }    
};  EO_VERIFYsizeof(s_eo_canprot_functions_periodicMotorControl, ((eocanprot_classperiodic_msgtypes_maxnumberof))*sizeof(eOcanprot_functions_t))


static const uint8_t s_eo_canprot_functions_periodicMotorControl_maxnumber = sizeof(s_eo_canprot_functions_periodicMotorControl) / sizeof(eOcanprot_functions_t);



const eOcanprot_functions_t s_eo_canprot_functions_pollingAnalogSensor[] =
{
    {   // 000      ICUBCANPROTO_POL_AS_CMD__NONE
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 001      ICUBCANPROTO_POL_AS_CMD__SET_IIR
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 002      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 003      ICUBCANPROTO_POL_AS_CMD__SET_MATRIX_RC
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 004      ICUBCANPROTO_POL_AS_CMD__SET_CH_DAC
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 005      ICUBCANPROTO_POL_AS_CMD__SELECT_ACTIVE_CH
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 006      ICUBCANPROTO_POL_AS_CMD__CALIBRATE_OFFSET
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 007      ICUBCANPROTO_POL_AS_CMD__SET_TXMODE
        EO_INIT(.former) eocanprotASpolling_former_POL_AS_CMD__SET_TXMODE,
        EO_INIT(.parser) NULL
    }, 
    {   // 008      ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE
        EO_INIT(.former) eocanprotASpolling_former_POL_AS_CMD__SET_CANDATARATE,
        EO_INIT(.parser) NULL
    },  
    {   // 009      ICUBCANPROTO_POL_AS_CMD__SAVE2EE
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 010      ICUBCANPROTO_POL_AS_CMD__GET_MATRIX_RC
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 011      ICUBCANPROTO_POL_AS_CMD__GET_CH_DAC
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 012      ICUBCANPROTO_POL_AS_CMD__GET_CH_ADC
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 013      ICUBCANPROTO_POL_AS_CMD__FILTER_EN
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 014      ICUBCANPROTO_POL_AS_CMD__MUX_EN
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 015      ICUBCANPROTO_POL_AS_CMD__MUX_NUM
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },     
    {   // 016      ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION
        EO_INIT(.former) eocanprotASpolling_former_POL_AS_CMD__SET_RESOLUTION,
        EO_INIT(.parser) NULL
    },  
    {   // 017      ICUBCANPROTO_POL_AS_CMD__SET_MATRIX_G
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 018      ICUBCANPROTO_POL_AS_CMD__GET_MATRIX_G
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 019      ICUBCANPROTO_POL_AS_CMD__SET_CALIB_TARE
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 020      ICUBCANPROTO_POL_AS_CMD__GET_CALIB_TARE
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 021      ICUBCANPROTO_POL_AS_CMD__SET_CURR_TARE
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 22       ICUBCANPROTO_POL_AS_CMD__GET_CURR_TARE
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 023      ICUBCANPROTO_POL_AS_CMD__SET_FULL_SCALES
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 024      ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES
        EO_INIT(.former) eocanprotASpolling_former_POL_AS_CMD__GET_FULL_SCALES,
        EO_INIT(.parser) eocanprotASpolling_parser_POL_AS_CMD__GET_FULL_SCALES
    },  
    {   // 025      ICUBCANPROTO_POL_AS_CMD__SET_SERIAL_NO
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 026      ICUBCANPROTO_POL_AS_CMD__GET_SERIAL_NO
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 027      ICUBCANPROTO_POL_AS_CMD__GET_EEPROM_STATUS
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 028      ICUBCANPROTO_POL_AS_CMD__GET_FW_VERSION
        EO_INIT(.former) eocanprotASpolling_former_POL_AS_CMD__GET_FIRMWARE_VERSION,
        EO_INIT(.parser) eocanprotASpolling_parser_POL_AS_CMD__GET_FIRMWARE_VERSION
    },  
    {   // 029      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 30       
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 031      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },    
    {   // 032      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 033           
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 034      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 035      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 036      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 037      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 038      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 039      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 040      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 041      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 042      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 043      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 044      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 045      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 046      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 047
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },     
    {   // 048
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 049
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 050      ICUBCANPROTO_POL_AS_CMD__SET_BOARD_ADX
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 051      ICUBCANPROTO_POL_AS_CMD__IMU_CONFIG_GET      
        EO_INIT(.former) eocanprotASpolling_former_POL_AS_CMD__IMU_CONFIG_GET,
        EO_INIT(.parser) eocanprotASpolling_parser_POL_AS_CMD__IMU_CONFIG_GET
    }, 
    {   // 052      ICUBCANPROTO_POL_AS_CMD__IMU_CONFIG_SET      
        EO_INIT(.former) eocanprotASpolling_former_POL_AS_CMD__IMU_CONFIG_SET,
        EO_INIT(.parser) NULL
    },  
    {   // 053      ICUBCANPROTO_POL_AS_CMD__IMU_TRANSMIT
        EO_INIT(.former) eocanprotASpolling_former_POL_AS_CMD__IMU_TRANSMIT,
        EO_INIT(.parser) NULL
    }, 
    {   // 054
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 055      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 056      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 057
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 058
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 059
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 060      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 061      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 062      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 063      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },       
    {   // 064       
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL 
    },
    {   // 065      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 066       
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL 
    },
    {   // 067       
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 068       
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },  
    {   // 069      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 070      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 071      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 072       
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 073      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },        
    {   // 074       
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 075      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 076      ICUBCANPROTO_POL_SK_CMD__TACT_SETUP      
        EO_INIT(.former) eocanprotASpolling_former_POL_SK_CMD__TACT_SETUP,
        EO_INIT(.parser) NULL
    },
    {   // 077      ICUBCANPROTO_POL_SK_CMD__SET_BRD_CFG
        EO_INIT(.former) eocanprotASpolling_former_POL_SK_CMD__SET_BRD_CFG,
        EO_INIT(.parser) NULL
    },
    {   // 078      ICUBCANPROTO_POL_SK_CMD__TACT_SETUP2 
        EO_INIT(.former) eocanprotASpolling_former_POL_SK_CMD__TACT_SETUP2,
        EO_INIT(.parser) NULL
    },  
    {   // 079      ICUBCANPROTO_POL_SK_CMD__ACC_GYRO_SETUP
        EO_INIT(.former) eocanprotASpolling_former_POL_SK_CMD__ACC_GYRO_SETUP,
        EO_INIT(.parser) NULL
    },
    {   // 080      ICUBCANPROTO_POL_SK_CMD__SET_TRIANG_CFG 
        EO_INIT(.former) eocanprotASpolling_former_POL_SK_CMD__SET_TRIANG_CFG,
        EO_INIT(.parser) NULL
    }, 
    {   // 081      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }
};  EO_VERIFYsizeof(s_eo_canprot_functions_pollingAnalogSensor, (82)*sizeof(eOcanprot_functions_t))



//const eOcanprot_functions_t s_eo_canprot_functions_pollingAnalogSensor[] =
//{
//    {   // 000 - > ICUBCANPROTO_POL_AS_CMD__SET_TXMODE = 0x07
//        EO_INIT(.former) eocanprotASpolling_former_00,
//        EO_INIT(.parser) eocanprotASpolling_parser_00
//    },
//    {   // 001 -> ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE = 0x08
//        EO_INIT(.former) eocanprotASpolling_former_01,
//        EO_INIT(.parser) eocanprotASpolling_parser_01
//    },
//    {   // 002 -> ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION = 0x10
//        EO_INIT(.former) eocanprotASpolling_former_02,
//        EO_INIT(.parser) eocanprotASpolling_parser_02
//    },
//    {   // 003 -> ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES = 0x18
//        EO_INIT(.former) eocanprotASpolling_former_02,
//        EO_INIT(.parser) eocanprotASpolling_parser_02        
//    },
//    {   // 004 -> ICUBCANPROTO_POL_SK_CMD__TACT_SETUP = 0x4c
//        EO_INIT(.former) eocanprotASpolling_former_02,
//        EO_INIT(.parser) eocanprotASpolling_parser_02        
//    }, 
//    {   // 005 -> ICUBCANPROTO_POL_SK_CMD__SET_BRD_CFG = 0x4d
//        EO_INIT(.former) eocanprotASpolling_former_02,
//        EO_INIT(.parser) eocanprotASpolling_parser_02        
//    }, 
//    {   // 006 -> ICUBCANPROTO_POL_SK_CMD__TACT_SETUP2 = 0x4e
//        EO_INIT(.former) eocanprotASpolling_former_02,
//        EO_INIT(.parser) eocanprotASpolling_parser_02        
//    }, 
//    {   // 007 -> ICUBCANPROTO_POL_SK_CMD__ACC_GYRO_SETUP = 0x4f
//        EO_INIT(.former) eocanprotASpolling_former_02,
//        EO_INIT(.parser) eocanprotASpolling_parser_02        
//    },   
//    {   // 008 -> ICUBCANPROTO_POL_SK_CMD__SET_TRIANG_CFG = 0x50
//        EO_INIT(.former) eocanprotASpolling_former_02,
//        EO_INIT(.parser) eocanprotASpolling_parser_02        
//    }     
//};  EO_VERIFYsizeof(s_eo_canprot_functions_pollingAnalogSensor, ((9))*sizeof(eOcanprot_functions_t))

static const uint8_t s_eo_canprot_functions_pollingAnalogSensor_maxnumber = sizeof(s_eo_canprot_functions_pollingAnalogSensor) / sizeof(eOcanprot_functions_t);


const eOcanprot_functions_t s_eo_canprot_functions_periodicAnalogSensor[] =
{
    {   // 000      ICUBCANPROTO_PER_AS_MSG__DIGITAL_GYROSCOPE
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotASperiodic_parser_PER_AS_MSG__DIGITAL_GYROSCOPE
    },
    {   // 001      ICUBCANPROTO_PER_AS_MSG__DIGITAL_ACCELEROMETER
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotASperiodic_parser_PER_AS_MSG__DIGITAL_ACCELEROMETER
    }, 
    {   // 002
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 003
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 004
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 005
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 006
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 007
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 008      ICUBCANPROTO_PER_AS_MSG__UNCALIBFORCE_VECTOR_DEBUGMODE
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotASperiodic_parser_PER_AS_MSG__UNCALIBFORCE_VECTOR_DEBUGMODE
    },
    {   // 009      ICUBCANPROTO_PER_AS_MSG__UNCALIBTORQUE_VECTOR_DEBUGMODE
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotASperiodic_parser_PER_AS_MSG__UNCALIBTORQUE_VECTOR_DEBUGMODE
    },
    {   // 010      ICUBCANPROTO_PER_AS_MSG__FORCE_VECTOR
        EO_INIT(.former) eocanprotASperiodic_former_PER_AS_MSG__FORCE_VECTOR,
        EO_INIT(.parser) eocanprotASperiodic_parser_PER_AS_MSG__FORCE_VECTOR
    },
    {   // 011      ICUBCANPROTO_PER_AS_MSG__TORQUE_VECTOR
        EO_INIT(.former) eocanprotASperiodic_former_PER_AS_MSG__TORQUE_VECTOR,
        EO_INIT(.parser) eocanprotASperiodic_parser_PER_AS_MSG__TORQUE_VECTOR
    }, 
    {   // 012      ICUBCANPROTO_PER_AS_MSG__HES0TO6
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotASperiodic_parser_PER_AS_MSG__HES0TO6
    },
    {   // 013      ICUBCANPROTO_PER_AS_MSG__HES7TO14
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotASperiodic_parser_PER_AS_MSG__HES7TO14
    },
    {   // 014
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 015
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }   
    
};   EO_VERIFYsizeof(s_eo_canprot_functions_periodicAnalogSensor, ((eocanprot_classperiodic_msgtypes_maxnumberof))*sizeof(eOcanprot_functions_t))

static const uint8_t s_eo_canprot_functions_periodicAnalogSensor_maxnumber = sizeof(s_eo_canprot_functions_periodicAnalogSensor) / sizeof(eOcanprot_functions_t);



const eOcanprot_functions_t s_eo_canprot_functions_periodicSkin[] =
{   // so far we manage every skin messages with only one parser function ...
    {
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotSKperiodic_parser_ANY_PERIODIC_SKIN_MSG
    }
};   EO_VERIFYsizeof(s_eo_canprot_functions_periodicSkin, ((1))*sizeof(eOcanprot_functions_t))

static const uint8_t s_eo_canprot_functions_periodicSkin_maxnumber = sizeof(s_eo_canprot_functions_periodicSkin) / sizeof(eOcanprot_functions_t);



const eOcanprot_functions_t s_eo_canprot_functions_periodicInertialSensor[] =
{
    {   // 000      ICUBCANPROTO_PER_IS_MSG__DIGITAL_GYROSCOPE
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotINperiodic_parser_PER_IS_MSG__DIGITAL_GYROSCOPE
    },
    {   // 001      ICUBCANPROTO_PER_IS_MSG__DIGITAL_ACCELEROMETER
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotINperiodic_parser_PER_IS_MSG__DIGITAL_ACCELEROMETER
    }, 
    {   // 002      ICUBCANPROTO_PER_IS_MSG__ANALOG_ACCELEROMETER
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL, //eocanprotINperiodic_parser_PER_IS_MSG__ANALOG_ACCELEROMETER
    },
    {   // 003      ICUBCANPROTO_PER_IS_MSG__IMU_TRIPLE
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotINperiodic_parser_PER_IS_MSG__IMU_TRIPLE
    },
    {   // 004      ICUBCANPROTO_PER_IS_MSG__IMU_QUATERNION
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotINperiodic_parser_PER_IS_MSG__IMU_QUATERNION
    },
    {   // 005      ICUBCANPROTO_PER_IS_MSG__IMU_STATUS
        EO_INIT(.former) NULL,
        EO_INIT(.parser) eocanprotINperiodic_parser_PER_IS_MSG__IMU_STATUS
    },
    {   // 006
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 007
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 008      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 009      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 010      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 011      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    }, 
    {   // 012      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 013      
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 014
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    },
    {   // 015
        EO_INIT(.former) NULL,
        EO_INIT(.parser) NULL
    } 
};   EO_VERIFYsizeof(s_eo_canprot_functions_periodicInertialSensor, ((eocanprot_classperiodic_msgtypes_maxnumberof))*sizeof(eOcanprot_functions_t))

static const uint8_t s_eo_canprot_functions_periodicInertialSensor_maxnumber = sizeof(s_eo_canprot_functions_periodicInertialSensor) / sizeof(eOcanprot_functions_t);




const eOcanprot_functions_t * const eo_canprot_lut_functions[] =
{   
    (const eOcanprot_functions_t *)&s_eo_canprot_functions_pollingMotorControl,   
    (const eOcanprot_functions_t *)&s_eo_canprot_functions_periodicMotorControl,        
    (const eOcanprot_functions_t *)&s_eo_canprot_functions_pollingAnalogSensor,
    (const eOcanprot_functions_t *)&s_eo_canprot_functions_periodicAnalogSensor,
    (const eOcanprot_functions_t *)&s_eo_canprot_functions_periodicSkin,
    (const eOcanprot_functions_t *)&s_eo_canprot_functions_periodicInertialSensor
};  EO_VERIFYsizeof(eo_canprot_lut_functions, (eocanprot_msgclass_maxvalue+1)*sizeof(const eOcanprot_functions_t *))  



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
    // we check that the message is class eocanprot_msgclass_pollingAnalogSensor and is for address 0 (the EMS).
    if((eocanprot_msgclass_pollingAnalogSensor == EOCANPROT_FRAME_GET_CLASS(frame)) && (0 == EOCANPROT_FRAME_POLLING_GET_DESTINATION(frame)))
    {
#if 0   
        // we avoid doing the extra check that the sender address is from a mais/strain to save computations 
        return(eores_OK);
#else 
        // in here we do the check ... however, (case 666) in runtime configuration when we call a eo_strain_Deactivate(), we also call eo_canmap_UnloadBoards()
        // which makes eo_canmap_GetBoard() returning NULL. in such a case, we may have an ack sent by mais or strain bu the board is not managed anymore, and hence
        // we cannot know who the sender is 
        eObrd_canlocation_t bloc = {0}; 
        bloc.port = port;
        bloc.addr = EOCANPROT_FRAME_GET_SOURCE(frame);
        
        eObrd_cantype_t btype = eo_canmap_GetBoardType(eo_canmap_GetHandle(), bloc);
        
        if(eobrd_cantype_unknown == btype)
        {   // board is not in EOtheCANmapping. it may be that we are in (case 666). 
            return(eores_OK);
        }
        if((eobrd_cantype_strain == btype) || (eobrd_cantype_mais == btype))
        {   // we have a ack sent by strain or mais
            return(eores_OK);
        }
#endif        
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
    static eObrd_canlocation_t loc = {0};
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
        msgclass = des->cmd.clas;
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





