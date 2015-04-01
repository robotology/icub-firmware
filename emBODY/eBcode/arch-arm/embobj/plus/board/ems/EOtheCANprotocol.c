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

#include "EOtheCANprotocolCfg.h" 

#include "EOtheErrorManager.h"
#include "EoError.h"


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
    {   // keep it first
        .former     = eocanprotMCpolling_former_POL_MC_CMD__exceptions,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__exceptions
    },  
    
    {   // 064  
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MIN_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MIN_POSITION
    },
    {   // 065
        .former     = eocanprotMCpolling_former_POL_MC_CMD__GET_MIN_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__GET_MIN_POSITION
    },
    {   // 066 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },
    {   // 067 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },
    {   // 068 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },  
    {   // 069 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },
    {   // 070 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    }, 
    {   // 071 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    }, 
    {   // 072 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    }, 
    {   // 073 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },        
    {   // 074  
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MIN_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MIN_POSITION
    },
    {   // 075
        .former     = eocanprotMCpolling_former_POL_MC_CMD__GET_MIN_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__GET_MIN_POSITION
    },
    {   // 076 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },
    {   // 077 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },
    {   // 078 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },  
    {   // 079 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },
    {   // 080 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    }, 
    {   // 081 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    }, 
    {   // 082 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    }, 
    {   // 083 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },        
    {   // 084  
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MIN_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MIN_POSITION
    },
    {   // 085
        .former     = eocanprotMCpolling_former_POL_MC_CMD__GET_MIN_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__GET_MIN_POSITION
    },
    {   // 086 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },
    {   // 087 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },
    {   // 088 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },  
    {   // 089 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },
    {   // 090 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    }, 
    {   // 091 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    }, 
    {   // 092 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    }, 
    {   // 093 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },        
    {   // 094  
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MIN_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MIN_POSITION
    },
    {   // 095
        .former     = eocanprotMCpolling_former_POL_MC_CMD__GET_MIN_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__GET_MIN_POSITION
    },
    {   // 096 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },
    {   // 097 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },
    {   // 098 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },  
    {   // 099 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },
    {   // 100 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    }, 
    {   // 101 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    }, 
    {   // 102 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    }, 
    {   // 103 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },        
    {   // 104  
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MIN_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MIN_POSITION
    },
    {   // 105
        .former     = eocanprotMCpolling_former_POL_MC_CMD__GET_MIN_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__GET_MIN_POSITION
    },
    {   // 106 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },
    {   // 107 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },
    {   // 108 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },  
    {   // 109 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },
    {   // 110 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    }, 
    {   // 111 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    }, 
    {   // 112 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    }, 
    {   // 113 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },        
    {   // 114  
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MIN_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MIN_POSITION
    },
    {   // 115
        .former     = eocanprotMCpolling_former_POL_MC_CMD__GET_MIN_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__GET_MIN_POSITION
    },
    {   // 116 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },
    {   // 117 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    },
    {   // 118 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    }

   
};  EO_VERIFYsizeof(s_eo_canprot_functions_pollingMotorControl, ((ICUBCANPROTO_POL_MC_CMD_MAXNUM) - (ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION) + 1)*sizeof(eOcanprot_functions_t));

static const uint8_t s_eo_canprot_functions_pollingMotorControl_maxnumber = sizeof(s_eo_canprot_functions_pollingMotorControl) / sizeof(eOcanprot_functions_t);



const eOcanprot_functions_t s_eo_canprot_functions_periodicMotorControl[] =
{
    {   // 000
        .former     = eocanprotMCperiodic_former_00,
        .parser     = eocanprotMCperiodic_parser_00
    },
    {   // 001
        .former     = eocanprotMCperiodic_former_01,
        .parser     = eocanprotMCperiodic_parser_01
    }, 
    {   // 002
        .former     = eocanprotMCperiodic_former_00,
        .parser     = eocanprotMCperiodic_parser_00
    },
    {   // 003
        .former     = eocanprotMCperiodic_former_01,
        .parser     = eocanprotMCperiodic_parser_01
    },
    {   // 004
        .former     = eocanprotMCperiodic_former_00,
        .parser     = eocanprotMCperiodic_parser_00
    },
    {   // 005
        .former     = eocanprotMCperiodic_former_01,
        .parser     = eocanprotMCperiodic_parser_01
    },
    {   // 006
        .former     = eocanprotMCperiodic_former_00,
        .parser     = eocanprotMCperiodic_parser_00
    },
    {   // 007
        .former     = eocanprotMCperiodic_former_01,
        .parser     = eocanprotMCperiodic_parser_01
    },
    {   // 008
        .former     = eocanprotMCperiodic_former_00,
        .parser     = eocanprotMCperiodic_parser_00
    },
    {   // 009
        .former     = eocanprotMCperiodic_former_01,
        .parser     = eocanprotMCperiodic_parser_01
    },
    {   // 010
        .former     = eocanprotMCperiodic_former_00,
        .parser     = eocanprotMCperiodic_parser_00
    },
    {   // 011
        .former     = eocanprotMCperiodic_former_01,
        .parser     = eocanprotMCperiodic_parser_01
    }, 
    {   // 012
        .former     = eocanprotMCperiodic_former_00,
        .parser     = eocanprotMCperiodic_parser_00
    },
    {   // 013
        .former     = eocanprotMCperiodic_former_01,
        .parser     = eocanprotMCperiodic_parser_01
    },
    {   // 014
        .former     = eocanprotMCperiodic_former_00,
        .parser     = eocanprotMCperiodic_parser_00
    },
    {   // 015
        .former     = eocanprotMCperiodic_former_01,
        .parser     = eocanprotMCperiodic_parser_01
    }    
};  EO_VERIFYsizeof(s_eo_canprot_functions_periodicMotorControl, ((16))*sizeof(eOcanprot_functions_t));

static const uint8_t s_eo_canprot_functions_periodicMotorControl_maxnumber = sizeof(s_eo_canprot_functions_periodicMotorControl) / sizeof(eOcanprot_functions_t);



const eOcanprot_functions_t s_eo_canprot_functions_pollingAnalogSensor[] =
{
    {   // 000 - > ICUBCANPROTO_POL_AS_CMD__SET_TXMODE = 0x07
        .former     = eocanprotASpolling_former_00,
        .parser     = eocanprotASpolling_parser_00
    },
    {   // 001 -> ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE = 0x08
        .former     = eocanprotASpolling_former_01,
        .parser     = eocanprotASpolling_parser_01
    },
    {   // 002 -> ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION = 0x10
        .former     = eocanprotASpolling_former_02,
        .parser     = eocanprotASpolling_parser_02
    },
    {   // 003 -> ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES = 0x18
        .former     = eocanprotASpolling_former_02,
        .parser     = eocanprotASpolling_parser_02        
    },
    {   // 004 -> ICUBCANPROTO_POL_SK_CMD__TACT_SETUP = 0x4c
        .former     = eocanprotASpolling_former_02,
        .parser     = eocanprotASpolling_parser_02        
    }, 
    {   // 005 -> ICUBCANPROTO_POL_SK_CMD__SET_BRD_CFG = 0x4d
        .former     = eocanprotASpolling_former_02,
        .parser     = eocanprotASpolling_parser_02        
    }, 
    {   // 006 -> ICUBCANPROTO_POL_SK_CMD__TACT_SETUP2 = 0x4e
        .former     = eocanprotASpolling_former_02,
        .parser     = eocanprotASpolling_parser_02        
    }, 
    {   // 007 -> ICUBCANPROTO_POL_SK_CMD__ACC_GYRO_SETUP = 0x4f
        .former     = eocanprotASpolling_former_02,
        .parser     = eocanprotASpolling_parser_02        
    },   
    {   // 008 -> ICUBCANPROTO_POL_SK_CMD__SET_TRIANG_CFG = 0x50
        .former     = eocanprotASpolling_former_02,
        .parser     = eocanprotASpolling_parser_02        
    }     
};  EO_VERIFYsizeof(s_eo_canprot_functions_pollingAnalogSensor, ((9))*sizeof(eOcanprot_functions_t));

static const uint8_t s_eo_canprot_functions_pollingAnalogSensor_maxnumber = sizeof(s_eo_canprot_functions_pollingAnalogSensor) / sizeof(eOcanprot_functions_t);


const eOcanprot_functions_t s_eo_canprot_functions_periodicAnalogSensor[] =
{
    {   // 000
        .former     = NULL,
        .parser     = NULL
    },
    {   // 001
        .former     = NULL,
        .parser     = NULL
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
    {   // 008
        .former     = eocanprotASperiodic_former_00,
        .parser     = eocanprotASperiodic_parser_00
    },
    {   // 009
        .former     = eocanprotASperiodic_former_00,
        .parser     = eocanprotASperiodic_parser_00
    },
    {   // 010
        .former     = eocanprotASperiodic_former_00,
        .parser     = eocanprotASperiodic_parser_00
    },
    {   // 011
        .former     = eocanprotASperiodic_former_00,
        .parser     = eocanprotASperiodic_parser_00
    }, 
    {   // 012
        .former     = eocanprotASperiodic_former_00,
        .parser     = eocanprotASperiodic_parser_00
    },
    {   // 013
        .former     = eocanprotASperiodic_former_00,
        .parser     = eocanprotASperiodic_parser_00
    },
    {   // 014
        .former     = NULL,
        .parser     = NULL
    },
    {   // 015
        .former     = NULL,
        .parser     = NULL
    }   
    
};   EO_VERIFYsizeof(s_eo_canprot_functions_periodicAnalogSensor, ((16))*sizeof(eOcanprot_functions_t));

static const uint8_t s_eo_canprot_functions_periodicAnalogSensor_maxnumber = sizeof(s_eo_canprot_functions_periodicAnalogSensor) / sizeof(eOcanprot_functions_t);



const eOcanprot_functions_t s_eo_canprot_functions_periodicSkin[] =
{
    {
        .former     = NULL,
        .parser     = eocanprotSKperiodic_parser_TactileSensorOutput8bit
    }
};   EO_VERIFYsizeof(s_eo_canprot_functions_periodicSkin, ((1))*sizeof(eOcanprot_functions_t));

static const uint8_t s_eo_canprot_functions_periodicSkin_maxnumber = sizeof(s_eo_canprot_functions_periodicSkin) / sizeof(eOcanprot_functions_t);



const eOcanprot_functions_t * const eo_canprot_lut_functions[] =
{   
    (const eOcanprot_functions_t *)&s_eo_canprot_functions_pollingMotorControl,   
    (const eOcanprot_functions_t *)&s_eo_canprot_functions_periodicMotorControl,        
    (const eOcanprot_functions_t *)&s_eo_canprot_functions_pollingAnalogSensor,
    (const eOcanprot_functions_t *)&s_eo_canprot_functions_periodicAnalogSensor,
    (const eOcanprot_functions_t *)&s_eo_canprot_functions_periodicSkin
};  EO_VERIFYsizeof(eo_canprot_lut_functions, eocanprot_msgclass_numberofthem*sizeof(const eOcanprot_functions_t *));  



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
        eOcanmap_entitylocation_t loc = {0};
        loc.port = port;
        loc.addr = EOCANPROT_FRAME_GET_SOURCE(frame);
        loc.insideindex = eocanmap_insideindex_none;
        const eOcanmap_canboard_t *board = eo_canmap_GetBoard(eo_canmap_GetHandle(), loc);
        if((eobrd_cantype_strain == board->board.type) || (eobrd_cantype_mais == board->board.type))
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

static eObool_t s_eo_canprot_isit_exception(eOcanframe_t *frame, eOcanport_t port, eOresult_t *res)
{
    // in here is put ok only if we find an exception
    
    // mais bug
    // get location of mais 
    static eObool_t mais_is_present = eobool_false;
    static eObool_t mais_is_searched = eobool_false;    
    static eOcanmap_entitylocation_t loc = {0};
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
        msgclass = des->msgclass;
    }
    else
    {   // only one between them is non NULL, not both
        return(eores_NOK_generic);
    }
    
    if(msgclass >= eocanprot_msgclass_numberofthem)
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
                msgtype = des->msgtype;
            }
            // marco.accame: the used IDs starts from ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION = 64. 
            //               the exceptions are: ICUBCANPROTO_POL_MC_CMD__MOTION_DONE (8), and ... nothing else. i manage exceptions at position 0. the others from 64 start at position 1 
            
            if(msgtype < ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION)
            {
                *index1 = 0;        // the exceptions are mapped as last.
                res = eores_OK;
            }
            else if(msgtype < (ICUBCANPROTO_POL_MC_CMD_MAXNUM))
            {
                *index1 = msgtype - (ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION - 1); // so that id 64 is mapped at position 1, 65 at position 2 etc.
                res = eores_OK;
            }
            
            if(*index1 >= s_eo_canprot_functions_pollingMotorControl_maxnumber)
            {   // extra check
                res = eores_NOK_generic;
            } 
        } break;
        
        case eocanprot_msgclass_pollingAnalogSensor:
        {
            *index0 = msgclass; 
            if(eobool_true == parsemode)
            {
                msgtype = EOCANPROT_FRAME_POLLING_GET_TYPE(frame); 
            }
            else
            {
                msgtype = des->msgtype;
            }                
            // marco.accame: the used IDs are only 4 out of a maximum of 50 something. so, ... just get the used ids and remaps them into 0, 1, 2, 3
            //               actually there are also those of the skin polling commands. they are 5. i write a big switch-case
            switch(msgtype)
            {
                case ICUBCANPROTO_POL_AS_CMD__SET_TXMODE:           { *index1 = 0; res = eores_OK; } break;
                case ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE:      { *index1 = 1; res = eores_OK; } break;
                case ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION:       { *index1 = 2; res = eores_OK; } break;
                case ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES:      { *index1 = 3; res = eores_OK; } break;
                case ICUBCANPROTO_POL_SK_CMD__TACT_SETUP:           { *index1 = 4; res = eores_OK; } break;
                case ICUBCANPROTO_POL_SK_CMD__SET_BRD_CFG:          { *index1 = 5; res = eores_OK; } break;
                case ICUBCANPROTO_POL_SK_CMD__TACT_SETUP2:          { *index1 = 6; res = eores_OK; } break;
                case ICUBCANPROTO_POL_SK_CMD__ACC_GYRO_SETUP:       { *index1 = 7; res = eores_OK; } break;
                case ICUBCANPROTO_POL_SK_CMD__SET_TRIANG_CFG:       { *index1 = 8; res = eores_OK; } break;
                default: { res = eores_NOK_generic; } break;            
            }
            
            // marco.accame: i have put an extra check ... but PLEASE ... be careful if you change things. the index1 must not go beyond s_eo_canprot_functions_pollingAnalogSensor.
            if(*index1 >= s_eo_canprot_functions_pollingAnalogSensor_maxnumber)
            {   // extra check
                res = eores_NOK_generic;
            }           
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
                msgtype = des->msgtype;
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
                msgtype = des->msgtype;
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
            *index1 = 0;        // there is only one type of message: Tactile Sensor Output at 8 bits resolution.  
            res = eores_OK;
            
            if(*index1 >= s_eo_canprot_functions_periodicSkin_maxnumber)
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





