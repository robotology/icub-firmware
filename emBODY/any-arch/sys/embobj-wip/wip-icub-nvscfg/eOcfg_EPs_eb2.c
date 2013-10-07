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

/* @file       eOcfg_EPs_eb2.c
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


#include "EoProtocolEP.h"
#include "EoProtocolEPmn.h"
#include "EoProtocolEPmn_fun.h"
#include "EoProtocolEPmc.h"
#include "EoProtocolEPmc_fun.h"
#include "EoProtocolEPas.h"
#include "EoProtocolEPas_fun.h"
#include "EoProtocolEPsk.h"
#include "EoProtocolEPsk_fun.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_EPs_eb2.h"


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



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_eocfg_eps_eb2_ep2index(eOnvEP8_t ep);

// endpoint mn
static void s_eocfg_eps_eb2_mn_ram_initialise(void *ram);

// endpoint mc
static void s_eocfg_eps_eb2_mc_ram_initialise(void *ram);

// endpoint as
static void s_eocfg_eps_eb2_as_ram_initialise(void *ram);


// endpoint sk
static void s_eocfg_eps_eb2_sk_ram_initialise(void *ram);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// typedef enum
// {   
//     eoprot_ep_as_strains_numberof_leftupperarm          = 1,
//     eoprot_ep_as_strains_numberof_leftlowerarm          = 0,
//     eoprot_ep_as_strains_numberof_rightupperarm         = 1,
//     eoprot_ep_as_strains_numberof_rightlowerarm         = 0,
//     eoprot_ep_as_strains_numberof_leftupperleg          = 1,
//     eoprot_ep_as_strains_numberof_rightupperleg         = 1
// } eOprot_ep_as_strains_numberof_t;

// typedef enum
// {   
//     eoprot_ep_as_maises_numberof_leftupperarm          = 0,
//     eoprot_ep_as_maises_numberof_leftlowerarm          = 1,
//     eoprot_ep_as_maises_numberof_rightupperarm         = 0,
//     eoprot_ep_as_maises_numberof_rightlowerarm         = 1,
//     eoprot_ep_as_maises_numberof_leftupperleg          = 0,
//     eoprot_ep_as_maises_numberof_rightupperleg         = 0
// } eOprot_ep_as_maises_numberof_t;

enum { epMNpos = 0, epMCpos = 1, epASpos = 2, epSKpos = 3 };

static const eOnvset_EPcfg_t s_eocfg_EPs_vectorof_eb2_data[] =
{  
     {   // management
        EO_INIT(.endpoint)                          eoprot_endpoint_management,
        EO_INIT(.dummy)                             0,
        EO_INIT(.epram_sizeof)                      sizeof(eocfg_EPs_eb2_mn_t),
        EO_INIT(.fptr_getnvsnumberof)               eoprot_ep_mn_variables_numberof_Get,
        EO_INIT(.fptr_isidsupported)                eoprot_ep_mn_variables_id_isvalid,
        EO_INIT(.fptr_getid)                        eoprot_ep_mn_variable_idfromprognumber_Get,
        EO_INIT(.fptr_getnvprognumber)              eoprot_ep_mn_variable_progressivenumber_Get,
        EO_INIT(.fptr_getnvrom)                     eoprot_ep_mn_variable_rom_Get,
        EO_INIT(.fptr_getnvram)                     eoprot_ep_mn_variable_ram_Extract, 
        EO_INIT(.fptr_initialise)                   s_eocfg_eps_eb2_mn_ram_initialise
    },        

    {   // motion-control
        EO_INIT(.endpoint)                          eoprot_endpoint_motioncontrol,
        EO_INIT(.dummy)                             0,
        EO_INIT(.epram_sizeof)                      sizeof(eocfg_EPs_eb2_mc_t),
        EO_INIT(.fptr_getnvsnumberof)               eoprot_ep_mc_variables_numberof_Get,
        EO_INIT(.fptr_isidsupported)                eoprot_ep_mc_variables_id_isvalid,
        EO_INIT(.fptr_getid)                        eoprot_ep_mc_variable_idfromprognumber_Get,
        EO_INIT(.fptr_getnvprognumber)              eoprot_ep_mc_variable_progressivenumber_Get,
        EO_INIT(.fptr_getnvrom)                     eoprot_ep_mc_variable_rom_Get,
        EO_INIT(.fptr_getnvram)                     eoprot_ep_mc_variable_ram_Extract, 
        EO_INIT(.fptr_initialise)                   s_eocfg_eps_eb2_mc_ram_initialise
    },
 
    {   // analog-sensors
        EO_INIT(.endpoint)                          eoprot_endpoint_analogsensors,
        EO_INIT(.dummy)                             0,
        EO_INIT(.epram_sizeof)                      sizeof(eocfg_EPs_eb2_as_t),
        EO_INIT(.fptr_getnvsnumberof)               eoprot_ep_as_variables_numberof_Get,
        EO_INIT(.fptr_isidsupported)                eoprot_ep_as_variables_id_isvalid,
        EO_INIT(.fptr_getid)                        eoprot_ep_as_variable_idfromprognumber_Get,
        EO_INIT(.fptr_getnvprognumber)              eoprot_ep_as_variable_progressivenumber_Get,
        EO_INIT(.fptr_getnvrom)                     eoprot_ep_as_variable_rom_Get,
        EO_INIT(.fptr_getnvram)                     eoprot_ep_as_variable_ram_Extract, 
        EO_INIT(.fptr_initialise)                   s_eocfg_eps_eb2_as_ram_initialise   
    },
    
    {   // sk-leftarm-lower
        EO_INIT(.endpoint)                          eoprot_endpoint_skin,
        EO_INIT(.dummy)                             0,
        EO_INIT(.epram_sizeof)                      sizeof(eocfg_EPs_eb2_sk_t),
        EO_INIT(.fptr_getnvsnumberof)               eoprot_ep_sk_variables_numberof_Get,
        EO_INIT(.fptr_isidsupported)                eoprot_ep_sk_variables_id_isvalid,
        EO_INIT(.fptr_getid)                        eoprot_ep_sk_variable_idfromprognumber_Get,
        EO_INIT(.fptr_getnvprognumber)              eoprot_ep_sk_variable_progressivenumber_Get,
        EO_INIT(.fptr_getnvrom)                     eoprot_ep_sk_variable_rom_Get,
        EO_INIT(.fptr_getnvram)                     eoprot_ep_sk_variable_ram_Extract, 
        EO_INIT(.fptr_initialise)                   s_eocfg_eps_eb2_sk_ram_initialise   
    }            
};  EO_VERIFYsizeof(s_eocfg_EPs_vectorof_eb2_data, sizeof(eOnvset_EPcfg_t)*(eocfg_EPs_eb2_numofendpoints));


static void* s_eocfg_eps_eb2_ram[eocfg_EPs_eb2_numofendpoints] =
{
    NULL,       // mn
    NULL,       // mc
    NULL,       // as
    NULL        // sk
};  EO_VERIFYsizeof(s_eocfg_eps_eb2_ram, sizeof(void*)*(eocfg_EPs_eb2_numofendpoints));


static const EOconstvector s_eocfg_EPs_vectorof_eb2_object = 
{
    EO_INIT(.size)                  sizeof(s_eocfg_EPs_vectorof_eb2_data)/sizeof(const eOnvset_EPcfg_t),
    EO_INIT(.item_size)             sizeof(eOnvset_EPcfg_t),
    EO_INIT(.item_array_data)       s_eocfg_EPs_vectorof_eb2_data
};


static const uint8_t s_eocfg_eps_eb2_mn_numberofeachentity[eomn_entities_numberof] = 
{ 
    eocfg_EPs_eb2_mn_numberof_comms, 
    eocfg_EPs_eb2_mn_numberof_appls
};

static const uint8_t s_eocfg_eps_eb2_mc_numberofeachentity[eomc_entities_numberof] = 
{ 
    eocfg_EPs_eb2_mc_numberof_joints, 
    eocfg_EPs_eb2_mc_numberof_motors, 
    eocfg_EPs_eb2_mc_numberof_controllers
};

static const uint8_t s_eocfg_eps_eb2_as_numberofeachentity[eoas_entities_numberof] = 
{ 
    eocfg_EPs_eb2_as_numberof_strains, 
    eocfg_EPs_eb2_as_numberof_maises
};

static const uint8_t s_eocfg_eps_eb2_sk_numberofeachentity[eosk_entities_numberof] = 
{ 
    eocfg_EPs_eb2_sk_numberof_skins
};


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

const eOnvset_DEVcfg_t eocfg_EPs_eb2_object =
{
	EO_INIT(.boardnum)                  eocfg_EPs_eb2_boardnumber,
    EO_INIT(.dummy)                     {0, 0, 0},
    EO_INIT(.fptr_device_initialise)    eocfg_EPs_eb2_Initialise,
    EO_INIT(.vectorof_epcfg)            &s_eocfg_EPs_vectorof_eb2_object,
    EO_INIT(.fptr_ep2indexofepcfg)      s_eocfg_eps_eb2_ep2index
};

const eOnvset_DEVcfg_t* eocfg_EPs_eb2 = &eocfg_EPs_eb2_object;



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern eOresult_t eocfg_EPs_eb2_Initialise(void)
{
    // must initialise the mc, the mn, the ...
    eoprot_ep_mn_number_of_entities_Load(eocfg_EPs_eb2_boardnumber, s_eocfg_eps_eb2_mn_numberofeachentity);
    eoprot_ep_mc_number_of_entities_Load(eocfg_EPs_eb2_boardnumber, s_eocfg_eps_eb2_mc_numberofeachentity);
    eoprot_ep_as_number_of_entities_Load(eocfg_EPs_eb2_boardnumber, s_eocfg_eps_eb2_as_numberofeachentity);
    eoprot_ep_sk_number_of_entities_Load(eocfg_EPs_eb2_boardnumber, s_eocfg_eps_eb2_sk_numberofeachentity);


    return(eores_OK);
}


extern void* eocfg_EPs_eb2_Get_RAM(eOnvEP8_t ep)
{
    uint16_t i = s_eocfg_eps_eb2_ep2index(ep);
    
    if(EOK_uint16dummy == i)
    {
        return(NULL);
    }
    
    return(s_eocfg_eps_eb2_ram[i]);       
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

EO_VERIFYproposition(eocfg_eps_eb2_ep_mn_val, 0x01 == eoprot_endpoint_management);
EO_VERIFYproposition(eocfg_eps_eb2_ep_mc_val, 0x11 == eoprot_endpoint_motioncontrol);
EO_VERIFYproposition(eocfg_eps_eb2_ep_as_val, 0x21 == eoprot_endpoint_analogsensors);
EO_VERIFYproposition(eocfg_eps_eb2_ep_sk_val, 0x31 == eoprot_endpoint_skin);

// VERY IMPORTANT INFO about how to fill the hashtable:
// the table has all entries equal to 0xff except for the entries in positions p = ep&0x3f (the 6 less significant 
// bits of the endpoint) which shall contain the epcfgindex to be applied to s_eocfg_EPs_vectorof_eb1_data[] which
// allows to retrieve the eOnvset_EPcfg_t of the endpoint ep.
static const uint8_t s_eocfg_eps_eb2_hashtable[64] = 
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

static uint16_t s_eocfg_eps_eb2_ep2index(eOnvEP8_t ep)
{    
    if(ep > sizeof(s_eocfg_eps_eb2_hashtable))
    {
        return(EOK_uint16dummy);
    }
    
    uint8_t r = s_eocfg_eps_eb2_hashtable[ep];
    
    if(r < eocfg_EPs_eb2_numofendpoints)
    {
        return(r);
    }
    return(EOK_uint16dummy);
}




// - endpoint mn

static void s_eocfg_eps_eb2_mn_ram_initialise(void *ram)
{
    // 1. assign the ram to the local pointer, so that future calls to eocfg_nvsEP_eb2_Get_RAM() shall give the ram back
    uint16_t i = s_eocfg_eps_eb2_ep2index(eoprot_endpoint_management);    
    if(EOK_uint16dummy != i)
    {   
        s_eocfg_eps_eb2_ram[i] = ram;
    }  
    
    
    // 2. initialise the values of the ram   
    eoprot_ep_mn_fun_INITIALISE(ram);
}



// - endpoint mc

static void s_eocfg_eps_eb2_mc_ram_initialise(void *ram)
{
    // 1. assign the ram to the local pointer, so that future calls to eocfg_nvsEP_eb2_Get_RAM() shall give the ram back
    uint16_t i = s_eocfg_eps_eb2_ep2index(eoprot_endpoint_motioncontrol);    
    if(EOK_uint16dummy != i)
    {   
        s_eocfg_eps_eb2_ram[i] = ram;
    }  
    
    
    // 2. initialise the values of the ram   
    eoprot_ep_mc_fun_INITIALISE(ram);
}



// - endpoint as



static void s_eocfg_eps_eb2_as_ram_initialise(void *ram)
{
    // 1. assign the ram to the local pointer, so that future calls to eocfg_nvsEP_eb2_Get_RAM() shall give the ram back
    uint16_t i = s_eocfg_eps_eb2_ep2index(eoprot_endpoint_analogsensors);    
    if(EOK_uint16dummy != i)
    {   
        s_eocfg_eps_eb2_ram[i] = ram;
    }  
    
 
    
    // 2. initialise the values of the ram   
    eoprot_ep_as_fun_INITIALISE(ram);
}



// - endpoint sk


static void s_eocfg_eps_eb2_sk_ram_initialise(void *ram)
{
    // 1. assign the ram to the local pointer, so that future calls to eocfg_nvsEP_eb2_Get_RAM() shall give the ram back
    uint16_t i = s_eocfg_eps_eb2_ep2index(eoprot_endpoint_skin);    
    if(EOK_uint16dummy != i)
    {   
        s_eocfg_eps_eb2_ram[i] = ram;
    }  
    

    // 2. initialise the values of the ram   
    eoprot_ep_sk_fun_INITIALISE(ram);
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



