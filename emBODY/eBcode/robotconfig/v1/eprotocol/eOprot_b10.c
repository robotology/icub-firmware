/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       eOprot_b10.c
    @brief      This file keeps ...
    @author     marco.accame@iit.it
    @date       04/12/2014
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"


#include "EOconstvector_hid.h"


#include "EoProtocolMN.h"
#include "EoProtocolSK.h"





// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOprot_b10.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

EO_VERIFYproposition(eoprot_b10_gasdfe, eoprot_boards_maxnumberof > eoprot_b10_boardnumber);



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_eoprot_b10_ep2index(eOnvEP8_t ep);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static const eOnvset_EPcfg_t s_eoprot_b10_theEPcfgs[] =
{  
    {   // management
        EO_INIT(.endpoint)                          eoprot_endpoint_management,
        EO_INIT(.dummy)                             0,
        EO_INIT(.epram_sizeof)                      sizeof(eOprot_b10_management_t),
        EO_INIT(.fptr_ram_initialise)               eoprot_fun_INITIALISE_mn,
        EO_INIT(.protif)                            (eOnvset_protocol_Interface_t*)&eoprot_eonvset_Interface        
    },        

    {   // skin         
        EO_INIT(.endpoint)                          eoprot_endpoint_skin,
        EO_INIT(.dummy)                             0,
        EO_INIT(.epram_sizeof)                      sizeof(eOprot_b10_skin_t),
        EO_INIT(.fptr_ram_initialise)               eoprot_fun_INITIALISE_sk,
        EO_INIT(.protif)                            (eOnvset_protocol_Interface_t*)&eoprot_eonvset_Interface             
    }     
    
};  EO_VERIFYsizeof(s_eoprot_b10_theEPcfgs, sizeof(eOnvset_EPcfg_t)*(eoprot_b10_endpoints_numberof));



static const EOconstvector s_eoprot_b10_constvectofEPcfg = 
{
    EO_INIT(.size)                  sizeof(s_eoprot_b10_theEPcfgs)/sizeof(const eOnvset_EPcfg_t),
    EO_INIT(.item_size)             sizeof(eOnvset_EPcfg_t),
    EO_INIT(.item_array_data)       s_eoprot_b10_theEPcfgs
};



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

const eOnvset_DEVcfg_t eoprot_b10_nvsetDEVcfg =
{
    EO_INIT(.boardnum)                  eoprot_b10_boardnumber,
    EO_INIT(.dummy)                     {0, 0, 0},
    EO_INIT(.fptr_device_initialise)    eoprot_b10_Initialise,     
    EO_INIT(.vectorof_epcfg)            &s_eoprot_b10_constvectofEPcfg,
    EO_INIT(.fptr_ep2indexofepcfg)      s_eoprot_b10_ep2index
};



const uint8_t eoprot_b10_mn_entities_numberofeach[eomn_entities_numberof] = 
{ 
    eoprot_b10_mn_comms_numberof, 
    eoprot_b10_mn_appls_numberof
};


const uint8_t eoprot_b10_sk_entities_numberofeach[eosk_entities_numberof] = 
{ 
    eoprot_b10_sk_skins_numberof
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern eOresult_t eoprot_b10_Initialise(eObool_t islocal)
{
    // must initialise the mc, the mn, the ...
    
    eoprot_config_endpoint_entities(eoprot_b10_boardnumber, eoprot_endpoint_management, eoprot_b10_mn_entities_numberofeach);
    eoprot_config_endpoint_entities(eoprot_b10_boardnumber, eoprot_endpoint_skin, eoprot_b10_sk_entities_numberofeach);
    
    eoprot_config_proxied_variables(eoprot_b10_boardnumber, eoprot_b10_isvariableproxied);
    
    if(eobool_true == islocal)
    {
        eoprot_config_board_local(eoprot_b10_boardnumber);
    }
    
    return(eores_OK);
}

extern eObool_t eoprot_b10_isvariableproxied(eOnvID32_t id)
{    
//     eOprotEndpoint_t ep = eoprot_ID2endpoint(id);
//     if(eoprot_endpoint_motioncontrol == ep)
//     {
//         eOprotEntity_t ent = eoprot_ID2entity(id);
//         if((eoprot_entity_mc_joint == ent) || (eoprot_entity_mc_motor == ent))
//         {   // only joints and motors are proxied. all of them
//             return(eobool_true);
//         }        
//     }
//     else if(eoprot_endpoint_analogsensors == ep)
//     {   // what about the mais?
//         return(eobool_false);
//     }
//     
//     // all other variables are not proxied
//     return(eobool_false);    

    //vale: i commented all because currently we don't use proxied variables
     return(eobool_false); 
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


EO_VERIFYproposition(s_eoprot_b10_mn_val, 0 == eoprot_endpoint_management);
EO_VERIFYproposition(s_eoprot_b10_sk_val, 3 == eoprot_endpoint_skin);

static uint16_t s_eoprot_b10_ep2index(eOnvEP8_t ep)
{    
    if(ep < eoprot_b10_endpoints_numberof)
    {
        return(ep);
    }
    return(EOK_uint16dummy);
}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



