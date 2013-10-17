
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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "string.h"
#include "OPCprotocolManager_Cfg.h" 
#include "EOMtheEMSbackdoortransceiver_hid.h"
#include "OPCprotocolManager.h"

#include "EOtheEMSapplDiagnostics.h"
#include "hal_trace.h"
#include "stdio.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

//static void generic_on_rec(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata);

static void on_rec_dgn_cmds(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata);

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
#define VERIFY_ROP_SETIMPEDANCE
#define VERIFY_ROP_SETPOSITIONRAW

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------




static void on_rec_dgn_cmds(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata)
{   // for the ems
    
    eOdgn_commands_t* cmd_ptr = (eOdgn_commands_t*)recdata;
    
    switch(opc)
    {

        case opcprotman_opc_say:    // someboby has replied to a ask we sent
        case opcprotman_opc_sig:    // someboby has spontaneously sent some data
        {
             
        }
        case opcprotman_opc_set:
        default:
        {   
            char str[50];
            snprintf(str, sizeof(str)-1, "diagnostics cmd=%d, extFault=%d ", cmd_ptr->enable, cmd_ptr->signalExtFault);
            hal_trace_puts(str);
            memcpy(&eo_dgn_cmds, (eOdgn_commands_t*)recdata, sizeof(eOdgn_commands_t));
        } break;
    }       
    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern opcprotman_res_t opcprotman_personalize_database(OPCprotocolManager *p)
{
    opcprotman_res_t res = opcprotman_OK;
    

/* personalize eodgn_nvidbdoor_cmds var*/
	res = opcprotman_personalize_var(   p, 
                                        eodgn_nvidbdoor_cmds,
                                        (uint8_t*) &eo_dgn_cmds, 
                                        on_rec_dgn_cmds);

    if(opcprotman_OK != res)
    {
        return(res);
    }
    
/* personalize eodgn_nvidbdoor_errorlog var*/
	res = opcprotman_personalize_var(   p, 
                                        eodgn_nvidbdoor_errorlog,
                                        (uint8_t*) &eo_dgn_errorlog, 
                                        NULL);

    if(opcprotman_OK != res)
    {
        return(res);
    }
    
/* personalize eodgn_nvidbdoor_emsperiph var*/
	res = opcprotman_personalize_var(   p, 
                                        eodgn_nvidbdoor_emsperiph,
                                        (uint8_t*)&eo_dgn_emsperiph, 
                                        NULL); //on ems i don't receive this data

    if(opcprotman_OK != res)
    {
        return(res);
    }
    
/* personalize eodgn_nvidbdoor_emsapplcore var*/
	res = opcprotman_personalize_var(   p, 
                                        eodgn_nvidbdoor_emsapplcommon,
                                        (uint8_t*)&eo_dgn_emsapplcore, 
                                        NULL); //on ems i don't receive this data

    if(opcprotman_OK != res)
    {
        return(res);
    }
    
/* personalize eodgn_nvidbdoor_emsapplmc var*/
	res = opcprotman_personalize_var(   p, 
                                        eodgn_nvidbdoor_emsapplmc,
                                        (uint8_t*)&eo_dgn_emsappmc, 
                                        NULL); //on ems i don't receive this data

    if(opcprotman_OK != res)
    {
        return(res);
    }
  

/* personalize eodgn_nvidbdoor_motorstatus var*/
	res = opcprotman_personalize_var(   p, 
                                        eodgn_nvidbdoor_motorstatus,
                                        (uint8_t*)&eo_dgn_motorstflag, 
                                        NULL); //on ems i don't receive this data

    if(opcprotman_OK != res)
    {
        return(res);
    }

#if defined(VERIFY_ROP_SETIMPEDANCE) | defined(VERIFY_ROP_SETPOSITIONRAW)

    /* personalize eodgn_nvidbdoor_rxcheckSetpoints var*/
	res = opcprotman_personalize_var(   p, 
                                        eodgn_nvidbdoor_rxcheckSetpoints,
                                        (uint8_t*)&eo_dgn_rxchecksepoints, 
                                         NULL); //on ems i don't receive this data

    if(opcprotman_OK != res)
    {
        return(res);
    }
    
#endif   
    return(res);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




