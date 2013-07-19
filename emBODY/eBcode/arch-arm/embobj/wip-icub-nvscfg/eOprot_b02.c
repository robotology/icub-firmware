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

/* @file       eOprot_b02.c
    @brief      This file keeps ...
    @author     marco.accame@iit.it
    @date       06/06/2013
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"


#include "EOconstvector_hid.h"


#include "EoProtocolAS.h"
#include "EoProtocolMC.h"
#include "EoProtocolMN.h"
#include "EoProtocolSK.h"





// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOprot_b02.h"


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

EO_VERIFYproposition(eoprot_b02_gasdfe, eoprot_boards_maxnumberof > eoprot_b02_boardnumber);



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_eoprot_b02_ep2index(eOnvEP8_t ep);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// if an endpoint XX is present in this array, then the relevant epXXpos must be defined equal to its position, else 0xff 
enum { epMNpos = 0, epMCpos = 1, epASpos = 2, epSKpos = 3 };

static const eOnvset_EPcfg_t s_eoprot_b02_theEPcfgs[] =
{  
    {   // management
        EO_INIT(.endpoint)                          eoprot_endpoint_management,
        EO_INIT(.dummy)                             0,
        EO_INIT(.epram_sizeof)                      sizeof(eOprot_b02_management_t),
        EO_INIT(.fptr_ram_initialise)               eoprot_fun_INITIALISE_mn,
        EO_INIT(.protif)                            (eOnvset_protocol_interface_t*)&eoprot_eonvset_interface_mn        
    },        

    {   // motion-control         
        EO_INIT(.endpoint)                          eoprot_endpoint_motioncontrol,
        EO_INIT(.dummy)                             0,
        EO_INIT(.epram_sizeof)                      sizeof(eOprot_b02_motioncontrol_t),
        EO_INIT(.fptr_ram_initialise)               eoprot_fun_INITIALISE_mc,
        EO_INIT(.protif)                            (eOnvset_protocol_interface_t*)&eoprot_eonvset_interface_mc        
    },
 
    {   // analog-sensors         
        EO_INIT(.endpoint)                          eoprot_endpoint_analogsensors,
        EO_INIT(.dummy)                             0,
        EO_INIT(.epram_sizeof)                      sizeof(eOprot_b02_analogsensors_t),
        EO_INIT(.fptr_ram_initialise)               eoprot_fun_INITIALISE_as,
        EO_INIT(.protif)                            (eOnvset_protocol_interface_t*)&eoprot_eonvset_interface_as        
    },

    {   // skin         
        EO_INIT(.endpoint)                          eoprot_endpoint_skin,
        EO_INIT(.dummy)                             0,
        EO_INIT(.epram_sizeof)                      sizeof(eOprot_b02_skin_t),
        EO_INIT(.fptr_ram_initialise)               eoprot_fun_INITIALISE_sk,
        EO_INIT(.protif)                            (eOnvset_protocol_interface_t*)&eoprot_eonvset_interface_sk             
    }     
    
};  EO_VERIFYsizeof(s_eoprot_b02_theEPcfgs, sizeof(eOnvset_EPcfg_t)*(eoprot_b02_endpoints_numberof));



static const EOconstvector s_eoprot_b02_constvectofEPcfg = 
{
    EO_INIT(.size)                  sizeof(s_eoprot_b02_theEPcfgs)/sizeof(const eOnvset_EPcfg_t),
    EO_INIT(.item_size)             sizeof(eOnvset_EPcfg_t),
    EO_INIT(.item_array_data)       s_eoprot_b02_theEPcfgs
};



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

const eOnvset_DEVcfg_t eoprot_b02_nvsetDEVcfg =
{
    EO_INIT(.boardnum)                  eoprot_b02_boardnumber,
    EO_INIT(.dummy)                     {0, 0, 0},
    EO_INIT(.fptr_device_initialise)    eoprot_b02_Initialise,     
    EO_INIT(.vectorof_epcfg)            &s_eoprot_b02_constvectofEPcfg,
    EO_INIT(.fptr_ep2indexofepcfg)      s_eoprot_b02_ep2index
};



const uint8_t eoprot_b02_mn_entities_numberofeach[eomn_entities_numberof] = 
{ 
    eoprot_b02_mn_comms_numberof, 
    eoprot_b02_mn_appls_numberof
};

const uint8_t eoprot_b02_mc_entities_numberofeach[eomc_entities_numberof] = 
{ 
    eoprot_b02_mc_joints_numberof, 
    eoprot_b02_mc_motors_numberof, 
    eoprot_b02_mc_controllers_numberof
};

const uint8_t eoprot_b02_as_entities_numberofeach[eoas_entities_numberof] = 
{ 
    eoprot_b02_as_strains_numberof, 
    eoprot_b02_as_maises_numberof
};


const uint8_t eoprot_b02_sk_entities_numberofeach[eosk_entities_numberof] = 
{ 
    eoprot_b02_sk_skins_numberof
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern eOresult_t eoprot_b02_Initialise(eObool_t islocal)
{
    // must initialise the mc, the mn, the ...
    
    eoprot_config_endpoint_entities(eoprot_b02_boardnumber, eoprot_endpoint_management, eoprot_b02_mn_entities_numberofeach);
    eoprot_config_endpoint_entities(eoprot_b02_boardnumber, eoprot_endpoint_motioncontrol, eoprot_b02_mc_entities_numberofeach);
    eoprot_config_endpoint_entities(eoprot_b02_boardnumber, eoprot_endpoint_analogsensors, eoprot_b02_as_entities_numberofeach);
    eoprot_config_endpoint_entities(eoprot_b02_boardnumber, eoprot_endpoint_skin, eoprot_b02_sk_entities_numberofeach);
    
    if(eobool_true == islocal)
    {
        eoprot_config_board_local(eoprot_b02_boardnumber);
    }
    
    return(eores_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

EO_VERIFYproposition(s_eoprot_b02_mn_val, 0x01 == eoprot_endpoint_management);
EO_VERIFYproposition(s_eoprot_b02_mc_val, 0x11 == eoprot_endpoint_motioncontrol);
EO_VERIFYproposition(s_eoprot_b02_as_val, 0x21 == eoprot_endpoint_analogsensors);
EO_VERIFYproposition(s_eoprot_b02_sk_val, 0x31 == eoprot_endpoint_skin);

// VERY IMPORTANT INFO about how to fill the hashtable:
// the table has all entries equal to 0xff except for the entries in positions p = ep&0x3f (the 6 less significant 
// bits of the endpoint) which shall contain the index be applied to s_eoprot_b02_theEPcfgs[] which
// allows to retrieve the eOnvset_EPcfg_t of the endpoint ep.
static const uint8_t s_eoprot_b02_hashtable[64] = 
{   
    // 00-15: eoprot_endpoint_management has p=0x01=1 and its EPcfg is in epcfgindex = 0 -> hashtable[1] = 0
    0xff, epMNpos, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    // 16-31: eoprot_endpoint_motioncontrol has p=0x11=17 and its EPcfg is in epcfgindex = 1 -> hashtable[17] = 1
    0xff, epMCpos, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    // 32-47: eoprot_endpoint_analogsensors has p=0x21=33 and its EPcfg is in epcfgindex = 2 -> hashtable[33] = 2
    0xff, epASpos, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    // 48-63: SK not present -> all row is 0xff.
    0xff, epSKpos, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff     
};  

static uint16_t s_eoprot_b02_ep2index(eOnvEP8_t ep)
{    
    if(ep > sizeof(s_eoprot_b02_hashtable))
    {
        return(EOK_uint16dummy);
    }
    
    uint8_t r = s_eoprot_b02_hashtable[ep];
    
    if(r < eoprot_b02_endpoints_numberof)
    {
        return(r);
    }
    return(EOK_uint16dummy);
}





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



