
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

#include "EOMtheEMSdiscoverytransceiver_hid.h" 
#include "stdio.h" 
#include "stdlib.h" 
#include "string.h" 

extern uint32_t lamiavariabileU32 = 0x12345678;
extern uint16_t lamiavariabileU16 = 0xabcd;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

static void generic_on_rec(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata);


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
        .var        = 1,
        .size       = 4,
        .ptr        = &lamiavariabileU32,
        .onrec      = generic_on_rec
    },
    {
        .var        = 2,
        .size       = 2,
        .ptr        = &lamiavariabileU16,
        .onrec      = generic_on_rec
    }
};

extern opcprotman_cfg_t opcprotmanCFGv0x1234 =
{
    .databaseversion        = 0x1234,
    .numberofvariables      = 2,
    .arrayofvariablemap     = s_myarray
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void generic_on_rec(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata)
{   // the copy in map->ptr is not yet done ...

    switch(opc)
    {
        case opcprotman_opc_say:
        {   // someboby has replied to a ask we sent: get the recdata and use it as you like
            recdata = recdata;
        } break;
        
       case opcprotman_opc_sig:
        {   // someboby has spontaneously sent some data: use it as you like
            recdata = recdata;
        } break;        
        
        case opcprotman_opc_set:
        {   // someboby has sent us the order of copying recdata into map->ptr ... do it
            if((NULL != map->ptr) && (0 != map->size))
            {
                memcpy(map->ptr, recdata, map->size); 
            }                
        } break;
        
        default:
        {   // other are not managed
        
        } break;
    }
 
}

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern opcprotman_cfg_t* eom_emsdiscoverytransceiver_hid_userdef_get_OPCprotocolManager_cfg(void)
{
    return(&opcprotmanCFGv0x1234);
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




