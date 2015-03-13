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

#include "EoCANprotMCpolling.h" 
#include "EoCANprotMCperiodic.h" 

#include "EoCANprotASpolling.h" 
#include "EoCANprotASperiodic.h" 

#include "EoCANprotSKperiodic.h" 

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
    {   // 
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MIN_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MIN_POSITION
    },
    {
        .former     = eocanprotMCpolling_former_POL_MC_CMD__GET_MIN_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__GET_MIN_POSITION
    },
    {
        .former     = eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION,
        .parser     = eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION
    } 
};



const eOcanprot_functions_t s_eo_canprot_functions_periodicMotorControl[] =
{
    {
        .former     = eocanprotMCperiodic_former_00,
        .parser     = eocanprotMCperiodic_parser_00
    },
    {
        .former     = eocanprotMCperiodic_former_01,
        .parser     = eocanprotMCperiodic_parser_01
    } 
};



const eOcanprot_functions_t s_eo_canprot_functions_pollingAnalogSensor[] =
{
    {
        .former     = eocanprotASpolling_former_00,
        .parser     = eocanprotASpolling_parser_00
    },
    {
        .former     = eocanprotASpolling_former_01,
        .parser     = eocanprotASpolling_parser_01
    },
    {
        .former     = eocanprotASpolling_former_02,
        .parser     = eocanprotASpolling_parser_02
    }    
};



const eOcanprot_functions_t s_eo_canprot_functions_periodicAnalogSensor[] =
{
    {
        .former     = eocanprotASperiodic_former_00,
        .parser     = eocanprotASperiodic_parser_00
    }
};




const eOcanprot_functions_t s_eo_canprot_functions_periodicSkin[] =
{
    {
        .former     = eocanprotSKperiodic_former_00,
        .parser     = eocanprotSKperiodic_parser_00
    }
};



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
    
    #warning marco.accame: TODO the rule which maps the pair {port, frame} into the relevant parser function 
    
    return(res);
}


extern eOresult_t eo_canprot_Form(EOtheCANprotocol *p, eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    eOresult_t res = eores_OK;
    
    #warning marco.accame: TODO the rule which maps the descriptor into the relevant former function which fills the frame
    
    return(res);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





