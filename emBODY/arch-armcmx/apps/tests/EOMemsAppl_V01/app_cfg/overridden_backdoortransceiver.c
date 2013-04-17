
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

#include "EOMtheEMSbackdoortransceiver_hid.h" 
#include "stdio.h" 
#include "stdlib.h" 
#include "string.h" 


#include "EOMtheIPnet_hid.h"            // to see eom_ipnet_hid_DEBUG (16 bytes)
#include "EOMtheEMSrunner_hid.h"        // to see eom_emsrunner_hid_DEBUG (40 bytes)
#include "EOMtheEMStransceiver_hid.h"   // to see EOMtheEMStransceiverDEBUG_t (28 bytes)

#include "EOemsController_hid.h" 

#include "EOtheEMSApplBody.h"    //to see EOcanFaultLogDEBUG_t

#include "EOappEncodersReader_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
extern EOcanFaultLogDEBUG_t EOcanFaultLogDEBUG;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

//static void generic_on_rec(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata);

static void on_rec_ipnet_debug(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata);

static void on_rec_runner_debug(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata);

static void on_rec_transceiver_debug(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata);

static void on_rec_emscontroller_debug(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata);

static void on_rec_canFaultLog_debug(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata);

static void on_rec_encoderError_debug(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata);


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
        .var        = eom_ipnet_hid_DEBUG_id,
        .size       = sizeof(eom_ipnet_hid_DEBUG),
        .ptr        = &eom_ipnet_hid_DEBUG,
        .onrec      = on_rec_ipnet_debug
    },
    {
        .var        = eom_emsrunner_hid_DEBUG_id,
        .size       = sizeof(eom_emsrunner_hid_DEBUG),
        .ptr        = &eom_emsrunner_hid_DEBUG,
        .onrec      = on_rec_runner_debug
    },
    {
        .var        = eom_emstransceiver_hid_DEBUG_id,
        .size       = sizeof(eom_emstransceiver_hid_DEBUG),
        .ptr        = &eom_emstransceiver_hid_DEBUG,
        .onrec      = on_rec_transceiver_debug
    },
    {
        .var        = eo_emsController_hid_DEBUG_id,
        .size       = sizeof(eo_emsController_hid_DEBUG),
        .ptr        = &eo_emsController_hid_DEBUG,
        .onrec      = on_rec_emscontroller_debug
    },    
    {
        .var        = eo_canFaultLogDEBUG_id,
        .size       = sizeof(EOcanFaultLogDEBUG_t),
        .ptr        = &EOcanFaultLogDEBUG,
        .onrec      = on_rec_canFaultLog_debug
    },

     {
        .var        = eo_EncoderErrorDEBUG_id,
        .size       = sizeof(EOencoderErrorDEBUG_t),
        .ptr        = &EOencoderErrorDEBUG,
        .onrec      = on_rec_encoderError_debug
    } 
    
};

extern opcprotman_cfg_t opcprotmanCFGv0x1234 =
{
    .databaseversion        = 0x1234,
    .numberofvariables      = 6,
    .arrayofvariablemap     = s_myarray
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static void on_rec_ipnet_debug(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata)
{   // for the ems
    
    switch(opc)
    {
        
        case opcprotman_opc_set:
        {   // someboby has sent us the order of copying recdata into map->ptr 
            
            // we just dont do it ...         
        } break;
        
        case opcprotman_opc_say:    // someboby has replied to a ask we sent
        case opcprotman_opc_sig:    // someboby has spontaneously sent some data
        default:
        {   // other are not managed
        
        } break;
    }       
    
}

static void on_rec_runner_debug(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata)
{   // for the ems
    
    switch(opc)
    {
        
        case opcprotman_opc_set:
        {   // someboby has sent us the order of copying recdata into map->ptr 
            
            // we just dont do it ...         
        } break;
        
        case opcprotman_opc_say:    // someboby has replied to a ask we sent
        case opcprotman_opc_sig:    // someboby has spontaneously sent some data
        default:
        {   // other are not managed
        
        } break;
    }       
    
}

static void on_rec_transceiver_debug(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata)
{   // for the ems
    
    switch(opc)
    {
        
        case opcprotman_opc_set:
        {   // someboby has sent us the order of copying recdata into map->ptr 
            
            // we just dont do it ...         
        } break;
        
        case opcprotman_opc_say:    // someboby has replied to a ask we sent
        case opcprotman_opc_sig:    // someboby has spontaneously sent some data
        default:
        {   // other are not managed
        
        } break;
    }       
    
}

static void on_rec_emscontroller_debug(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata)
{   // for the ems
    
    switch(opc)
    {
        
        case opcprotman_opc_set:
        {   // someboby has sent us the order of copying recdata into map->ptr 
            
            // we just dont do it ...         
        } break;
        
        case opcprotman_opc_say:    // someboby has replied to a ask we sent
        case opcprotman_opc_sig:    // someboby has spontaneously sent some data
        default:
        {   // other are not managed
        
        } break;
    }       
    
}

static void on_rec_canFaultLog_debug(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata)
{   // for the ems
    
    //NOTE: never rec in ems
    switch(opc)
    {
        
        case opcprotman_opc_set:
        {   // someboby has sent us the order of copying recdata into map->ptr 
            
            // we just dont do it ...         
        } break;
        
        case opcprotman_opc_say:    // someboby has replied to a ask we sent
        case opcprotman_opc_sig:    // someboby has spontaneously sent some data
        default:
        {   // other are not managed
        
        } break;
    }       
    
}

static void on_rec_encoderError_debug(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata)
{   // for the ems
    
    //NOTE: never rec in ems
    switch(opc)
    {
        
        case opcprotman_opc_set:
        {   // someboby has sent us the order of copying recdata into map->ptr 
            
            // we just dont do it ...         
        } break;
        
        case opcprotman_opc_say:    // someboby has replied to a ask we sent
        case opcprotman_opc_sig:    // someboby has spontaneously sent some data
        default:
        {   // other are not managed
        
        } break;
    }       
    
}

// static void generic_on_rec(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata)
// {   // the copy in map->ptr is not yet done ...

//     switch(opc)
//     {
//         case opcprotman_opc_say:
//         {   // someboby has replied to a ask we sent: get the recdata and use it as you like
//             recdata = recdata;
//         } break;
//         
//        case opcprotman_opc_sig:
//         {   // someboby has spontaneously sent some data: use it as you like
//             recdata = recdata;
//         } break;        
//         
//         case opcprotman_opc_set:
//         {   // someboby has sent us the order of copying recdata into map->ptr ... do it
//             if((NULL != map->ptr) && (0 != map->size))
//             {
//                 memcpy(map->ptr, recdata, map->size); 
//             }                
//         } break;
//         
//         default:
//         {   // other are not managed
//         
//         } break;
//     }
//  
// }

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern opcprotman_cfg_t* eom_emsbackdoortransceiver_hid_userdef_get_OPCprotocolManager_cfg(void)
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




