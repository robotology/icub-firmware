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


#include "stdio.h" 
#include "stdlib.h" 
#include "string.h" 


#include "EOMtheIPnet_hid.h"            // to see eom_ipnet_hid_DEBUG (16 bytes)
#include "EOMtheEMSrunner_hid.h"        // to see eom_emsrunner_hid_DEBUG (40 bytes)
#include "EOMtheEMStransceiver_hid.h"   // to see EOMtheEMStransceiverDEBUG_t (28 bytes)


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------


extern void on_rec_ipnet_debug(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata);

extern void on_rec_runner_debug(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata);

extern void on_rec_transceiver_debug(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata);

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


extern EOMtheIPnetDEBUG_t eom_ipnet_hid_DEBUG_of_ems =
{
    .datagrams_failed_to_go_in_rxfifo               = 0,
    .datagrams_failed_to_go_in_txosalqueue          = 0,
    .datagrams_failed_to_be_retrieved_from_txfifo   = 0,
    .datagrams_failed_to_be_sent_by_ipal            = 0    
};

extern EOMtheEMSrunnerDEBUG_t eom_emsrunner_hid_DEBUG_of_ems =
{
    .numberofperiods                            = 0,
    .cumulativeabsoluteerrorinperiod            = 0,
    .meanofabsoluteerrorinperiod                = 0,
    .movingmeanofabsoluteerrorinperiod          = 0,
    .maxabsoluteerrorinperiod                   = 0,
    .minabsoluteerrorinperiod                   = 1000000000,
    .executionoverflows                         = {0, 0, 0},
    .datagrams_failed_to_go_in_txsocket         = 0  
};

extern EOMtheEMStransceiverDEBUG_t eom_emstransceiver_hid_DEBUG_of_ems =
{
    .rxinvalidropframes                     = 0,
    .errorsinsequencenumber                 = 0,
    .lostreplies                            = 0,
    .failuresinloadofreplyropframe          = 0,
    .txropframeistoobigforthepacket         = 0,
    .cannotloadropinregulars                = 0,
    .cannotloadropinoccasionals             = 0    
};

static opcprotman_var_map_t s_myarray[] = 
{
    {
        .var        = 1,
        .size       = sizeof(EOMtheIPnetDEBUG_t),
        .ptr        = &eom_ipnet_hid_DEBUG_of_ems,
        .onrec      = on_rec_ipnet_debug
    },
    {
        .var        = 2,
        .size       = sizeof(EOMtheEMSrunnerDEBUG_t),
        .ptr        = &eom_emsrunner_hid_DEBUG_of_ems,
        .onrec      = on_rec_runner_debug
    },
    {
        .var        = 3,
        .size       = sizeof(EOMtheEMStransceiverDEBUG_t),
        .ptr        = &eom_emstransceiver_hid_DEBUG_of_ems,
        .onrec      = on_rec_transceiver_debug
    },
    
};

extern opcprotman_cfg_t opcprotmanCFGv0x1234 =
{
    .databaseversion        = 0x1234,
    .numberofvariables      = 3,
    .arrayofvariablemap     = s_myarray
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


extern void on_rec_ipnet_debug(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata)
{   // for the host

    EOMtheIPnetDEBUG_t* data = (EOMtheIPnetDEBUG_t*)recdata;
    
    switch(opc)
    {
        
        default:
        case opcprotman_opc_set:
        {   // nobody can order that to us           
            // we just dont do it ...         
        } break;
        
        case opcprotman_opc_say:    // someboby has replied to a ask we sent
        case opcprotman_opc_sig:    // someboby has spontaneously sent some data
        {   
        
            printf("received data of ipnet");
        
        } break;
    }       
    
}

extern void on_rec_runner_debug(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata)
{   // for the host

    eom_emsrunner_hid_DEBUG_of_ems* data = (eom_emsrunner_hid_DEBUG_of_ems*)recdata;
    
    switch(opc)
    {
        
        default:
        case opcprotman_opc_set:
        {   // nobody can order that to us           
            // we just dont do it ...         
        } break;
        
        case opcprotman_opc_say:    // someboby has replied to a ask we sent
        case opcprotman_opc_sig:    // someboby has spontaneously sent some data
        {   
        
            printf("received data of runner");
        
        } break;
    }       
    
}

extern void on_rec_transceiver_debug(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata)
{   // for the host
    
    switch(opc)
    {
        
        default:
        case opcprotman_opc_set:
        {   // nobody can order that to us           
            // we just dont do it ...         
        } break;
        
        case opcprotman_opc_say:    // someboby has replied to a ask we sent
        case opcprotman_opc_sig:    // someboby has spontaneously sent some data
        {   
        
            printf("received data of transceiver");
        
        } break;
    }       
    
}



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern opcprotman_cfg_t* example_cfg_host_get_OPCprotocolManager_cfg(void)
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





