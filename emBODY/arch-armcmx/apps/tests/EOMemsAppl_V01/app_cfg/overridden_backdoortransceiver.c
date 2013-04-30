
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

#include "OPCprotocolManager_Cfg.h" 
#include "EOMtheEMSbackdoortransceiver_hid.h"
#include "OPCprotocolManager.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



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

//the following vars are declare in somewhere in the code and they contain value to send or the received value.
extern EOMtheIPnetDEBUG_t eom_ipnet_hid_DEBUG;
extern EOMtheEMSrunnerDEBUG_t eom_emsrunner_hid_DEBUG;
extern EOMtheEMStransceiverDEBUG_t eom_emstransceiver_hid_DEBUG;
extern EOemsControllerDEBUG_t eo_emsController_hid_DEBUG;
extern EOcanFaultLogDEBUG_t EOcanFaultLogDEBUG;
extern EOencoderErrorDEBUG_t EOencoderErrorDEBUG;




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


extern opcprotman_res_t opcprotman_personalize_database(OPCprotocolManager *p)
{
    opcprotman_res_t res = opcprotman_OK;
    

/* personalize eom_ipnet_hid_DEBUG_id var*/
	res = opcprotman_personalize_var(   p, 
                                        eom_ipnet_hid_DEBUG_id,
                                        (uint8_t*) &eom_ipnet_hid_DEBUG, 
                                        on_rec_ipnet_debug);

    if(opcprotman_OK != res)
    {
        return(res);
    }
/* personalize eom_emsrunner_hid_DEBUG_id var*/
	res = opcprotman_personalize_var(   p, 
                                        eom_emsrunner_hid_DEBUG_id,
                                        (uint8_t*)&eom_emsrunner_hid_DEBUG, 
                                        on_rec_runner_debug);

    if(opcprotman_OK != res)
    {
        return(res);
    }
    
/* personalize eom_emstransceiver_hid_DEBUG_id var*/
	res = opcprotman_personalize_var(   p, 
                                        eom_emstransceiver_hid_DEBUG_id,
                                        (uint8_t*)&eom_emstransceiver_hid_DEBUG, 
                                        on_rec_transceiver_debug);

    if(opcprotman_OK != res)
    {
        return(res);
    }


/* personalize eo_emsController_hid_DEBUG_id var*/
	res = opcprotman_personalize_var(   p, 
                                        eo_emsController_hid_DEBUG_id,
                                        (uint8_t*)&eo_emsController_hid_DEBUG, 
                                        on_rec_emscontroller_debug);

    if(opcprotman_OK != res)
    {
        return(res);
    }


/* personalize eo_emsController_hid_DEBUG_id var*/
	res = opcprotman_personalize_var(   p, 
                                        eo_canFaultLogDEBUG_id,
                                        (uint8_t*)&EOcanFaultLogDEBUG, 
                                        on_rec_canFaultLog_debug);

    if(opcprotman_OK != res)
    {
        return(res);
    }
  
    /* personalize eo_emsController_hid_DEBUG_id var*/
	res = opcprotman_personalize_var(   p, 
                                         eo_EncoderErrorDEBUG_id ,
                                        (uint8_t*)&EOencoderErrorDEBUG, 
                                        on_rec_encoderError_debug);

    if(opcprotman_OK != res)
    {
        return(res);
    }

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




