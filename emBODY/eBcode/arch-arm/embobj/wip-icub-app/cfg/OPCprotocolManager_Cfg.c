/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
#include "stdio.h"
#include "OPCprotocolManager_Cfg.h" 
#include "OPCprotocolManager.h"

#include "EoDiagnostics.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

static opcprotman_var_map_t s_myarray[] = 
{
    {
        .var        = eodgn_nvidbdoor_cmds,
        .size       = sizeof(eOdgn_commands_t),
        .ptr        = NULL,
        .onrec      = NULL
    },
    {
        .var        = eodgn_nvidbdoor_errorlog,
        .size       = sizeof(eOdgn_errorlog_t),
        .ptr        = NULL,
        .onrec      = NULL
    },
    {
        .var        = eodgn_nvidbdoor_emsperiph,
        .size       = sizeof(eOdgn_emsperipheralstatus_t),
        .ptr        = NULL,
        .onrec      = NULL
    },
    {
        .var        = eodgn_nvidbdoor_emsapplcommon,
        .size       = sizeof(eOdgn_emsapplication_common_t),
        .ptr        = NULL,
        .onrec      = NULL
    },
    {
        .var        = eodgn_nvidbdoor_emsapplmc,
        .size       = sizeof(eOdgn_emsapplication_emswithmc_t),
        .ptr        = NULL,
        .onrec      = NULL
    },
    {
        .var        = eodgn_nvidbdoor_motorstatus,
        .size       = sizeof(eOdgn_motorstatusflags_t),
        .ptr        = NULL,
        .onrec      = NULL
    }
};

opcprotman_cfg_t opcprotmanCFGv0x1234 =
{
    .databaseversion        = 0x1235,
    .numberofvariables      = opcCfg_nvid_bdoor_user_num + eodgn_nvidbdoor_max,
    .arrayofvariablemap     = s_myarray
};

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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern opcprotman_cfg_t* OPCprotocolManager_Cfg_getconfig(void)
{
    return(&opcprotmanCFGv0x1234);
}


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




