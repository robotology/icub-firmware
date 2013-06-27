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

/* @file       eOcfg_EPs_eb1.c
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



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_EPs_eb1.h"


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

static uint16_t s_eocfg_eps_eb1_ep2index(uint16_t ep);

// endpoint mn
static void* s_eocfg_eps_eb1_mn_epid2nvrom(eOnvEP_t ep, eOnvID_t id);
static void* s_eocfg_eps_eb1_mn_dataepid2nvram(void* data, eOnvEP_t ep, eOnvID_t id);  
static eOnvID_t s_eocfg_eps_eb1_mn_epnvprogressivenumber2id(eOnvEP_t ep, uint16_t prognum); 
static uint16_t s_eocfg_eps_eb1_mn_epid2nvprogressivenumber(eOnvEP_t ep, eOnvID_t id); 
static uint16_t s_eocfg_eps_eb1_mn_ep2nvsnumberof(eOnvEP_t ep);
static void s_eocfg_eps_eb1_mn_ram_initialise(eOnvEP_t ep, void *ram);

// endpoint mc
static void* s_eocfg_eps_eb1_mc_epid2nvrom(eOnvEP_t ep, eOnvID_t id);
static void* s_eocfg_eps_eb1_mc_dataepid2nvram(void* data, eOnvEP_t ep, eOnvID_t id);  
static eOnvID_t s_eocfg_eps_eb1_mc_epnvprogressivenumber2id(eOnvEP_t ep, uint16_t prognum);
static uint16_t s_eocfg_eps_eb1_mc_epid2nvprogressivenumber(eOnvEP_t ep, eOnvID_t id); 
static uint16_t s_eocfg_eps_eb1_mc_ep2nvsnumberof(eOnvEP_t ep);
static void s_eocfg_eps_eb1_mc_ram_initialise(eOnvEP_t ep, void *ram);

// endpoint as
static void* s_eocfg_eps_eb1_as_epid2nvrom(eOnvEP_t ep, eOnvID_t id);
static void* s_eocfg_eps_eb1_as_dataepid2nvram(void* data, eOnvEP_t ep, eOnvID_t id);  
static eOnvID_t s_eocfg_eps_eb1_as_epnvprogressivenumber2id(eOnvEP_t ep, uint16_t prognum);
static uint16_t s_eocfg_eps_eb1_as_epid2nvprogressivenumber(eOnvEP_t ep, eOnvID_t id); 
static uint16_t s_eocfg_eps_eb1_as_ep2nvsnumberof(eOnvEP_t ep);
static void s_eocfg_eps_eb1_as_ram_initialise(eOnvEP_t ep, void *ram);



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


static const eOnvset_EPcfg_t s_eocfg_EPs_vectorof_eb1_data[] =
{  
    {   // management
        EO_INIT(.endpoint)                          eoprot_endpoint_management,
        EO_INIT(.epram_sizeof)                      sizeof(eocfg_EPs_eb1_mn_t),
        EO_INIT(.fptr_ep2nvsnumberof)               s_eocfg_eps_eb1_mn_ep2nvsnumberof,
        EO_INIT(.fptr_epnvprogressivenumber2id)     s_eocfg_eps_eb1_mn_epnvprogressivenumber2id,
        EO_INIT(.fptr_epid2nvprogressivenumber)     s_eocfg_eps_eb1_mn_epid2nvprogressivenumber,
        EO_INIT(.fptr_epid2nvrom)                   s_eocfg_eps_eb1_mn_epid2nvrom,
        EO_INIT(.fptr_epramepid2nvram)              s_eocfg_eps_eb1_mn_dataepid2nvram, 
        EO_INIT(.fptr_epram_initialise)             s_eocfg_eps_eb1_mn_ram_initialise
    },        

    {   // motion-control
        EO_INIT(.endpoint)                          eoprot_endpoint_motioncontrol,
        EO_INIT(.epram_sizeof)                      sizeof(eocfg_EPs_eb1_mc_t),
        EO_INIT(.fptr_ep2nvsnumberof)               s_eocfg_eps_eb1_mc_ep2nvsnumberof,
        EO_INIT(.fptr_epnvprogressivenumber2id)     s_eocfg_eps_eb1_mc_epnvprogressivenumber2id,
        EO_INIT(.fptr_epid2nvprogressivenumber)     s_eocfg_eps_eb1_mc_epid2nvprogressivenumber,
        EO_INIT(.fptr_epid2nvrom)                   s_eocfg_eps_eb1_mc_epid2nvrom,
        EO_INIT(.fptr_epramepid2nvram)              s_eocfg_eps_eb1_mc_dataepid2nvram, 
        EO_INIT(.fptr_epram_initialise)             s_eocfg_eps_eb1_mc_ram_initialise
    },
 
    {   // analog-sensors
        EO_INIT(.endpoint)                          eoprot_endpoint_analogsensors,
        EO_INIT(.epram_sizeof)                      sizeof(eocfg_EPs_eb1_as_t),
        EO_INIT(.fptr_ep2nvsnumberof)               s_eocfg_eps_eb1_as_ep2nvsnumberof,
        EO_INIT(.fptr_epnvprogressivenumber2id)     s_eocfg_eps_eb1_as_epnvprogressivenumber2id,
        EO_INIT(.fptr_epid2nvprogressivenumber)     s_eocfg_eps_eb1_as_epid2nvprogressivenumber,
        EO_INIT(.fptr_epid2nvrom)                   s_eocfg_eps_eb1_as_epid2nvrom,
        EO_INIT(.fptr_epramepid2nvram)              s_eocfg_eps_eb1_as_dataepid2nvram, 
        EO_INIT(.fptr_epram_initialise)             s_eocfg_eps_eb1_as_ram_initialise
    }  
    
};  EO_VERIFYsizeof(s_eocfg_EPs_vectorof_eb1_data, sizeof(eOnvset_EPcfg_t)*(eocfg_EPs_eb1_numofendpoints));


static void* s_eocfg_eps_eb1_ram[eocfg_EPs_eb1_numofendpoints] =
{
    NULL,       // mn
    NULL,       // mc
    NULL        // as
};  EO_VERIFYsizeof(s_eocfg_eps_eb1_ram, sizeof(void*)*(eocfg_EPs_eb1_numofendpoints));


const EOconstvector eocfg_EPs_vectorof_eb1_object = 
{
    EO_INIT(.size)                  sizeof(s_eocfg_EPs_vectorof_eb1_data)/sizeof(const eOnvset_EPcfg_t),
    EO_INIT(.item_size)             sizeof(eOnvset_EPcfg_t),
    EO_INIT(.item_array_data)       s_eocfg_EPs_vectorof_eb1_data
};



static const uint8_t s_eocfg_eps_eb1_mn_numberofeachentity[eomn_entities_numberof] = 
{ 
    eocfg_EPs_eb1_mn_numberof_comms, 
    eocfg_EPs_eb1_mn_numberof_appls
};

static const uint8_t s_eocfg_eps_eb1_mc_numberofeachentity[eomc_entities_numberof] = 
{ 
    eocfg_EPs_eb1_mc_numberof_joints, 
    eocfg_EPs_eb1_mc_numberof_motors, 
    eocfg_EPs_eb1_mc_numberof_controllers
};

static const uint8_t s_eocfg_eps_eb1_as_numberofeachentity[eoas_entities_numberof] = 
{ 
    eocfg_EPs_eb1_as_numberof_strains, 
    eocfg_EPs_eb1_as_numberof_maises
};



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

const eOnvset_DEVcfg_t eocfg_EPs_eb1_object =
{
    EO_INIT(.fptr_device_initialise)    eocfg_EPs_eb1_Initialise,     
    EO_INIT(.vectorof_epcfg)            &eocfg_EPs_vectorof_eb1_object,
    EO_INIT(.fptr_ep2indexofepcfg)      s_eocfg_eps_eb1_ep2index
};

const eOnvset_DEVcfg_t* eocfg_EPs_eb1 = &eocfg_EPs_eb1_object;



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

#warning --> someone must call eocfg_EPs_eb1_Initialise()

extern eOresult_t eocfg_EPs_eb1_Initialise(void)
{
    // must initialise the mc, the mn, the ...
    eoprot_ep_mn_number_of_entities_Load(eocfg_EPs_eb1_boardnumber, s_eocfg_eps_eb1_mn_numberofeachentity);
    eoprot_ep_mc_number_of_entities_Load(eocfg_EPs_eb1_boardnumber, s_eocfg_eps_eb1_mc_numberofeachentity);
    eoprot_ep_as_number_of_entities_Load(eocfg_EPs_eb1_boardnumber, s_eocfg_eps_eb1_as_numberofeachentity);

    return(eores_OK);
}


extern void* eocfg_EPs_eb1_Get_RAM(eOnvEP_t ep)
{
    uint16_t i = s_eocfg_eps_eb1_ep2index(ep);
    
    if(EOK_uint16dummy == i)
    {
        return(NULL);
    }
    
    return(s_eocfg_eps_eb1_ram[i]);       
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

EO_VERIFYproposition(eocfg_eps_eb1_ep_mn_val, 0x0001 == eoprot_endpoint_management);
EO_VERIFYproposition(eocfg_eps_eb1_ep_mc_val, 0x0011 == eoprot_endpoint_motioncontrol);
EO_VERIFYproposition(eocfg_eps_eb1_ep_as_val, 0x0021 == eoprot_endpoint_analogsensors);

// VERY IMPORTANT INFO about how to fill the hashtable:
// the table has all entries equal to 0xff except for the entries in positions p = ep&0x3f (the 6 less significant 
// bits of the endpoint) which shall contain the epcfgindex to be applied to s_eocfg_EPs_vectorof_eb1_data[] which
// allows to retrieve the eOnvset_EPcfg_t of the endpoint ep.
static const uint8_t s_eocfg_eps_eb1_hashtable[64] = 
{   // i use uint8_t instead of uint16_t to save space.
    // 00-15: MN eoprot_endpoint_mn_emsboard has p=0x01=1 and its EPcfg is in epcfgindex = 0 -> hashtable[1] = 0
    0xff,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    // 16-31: eoprot_endpoint_motioncontrol has p=0x11=17 and its EPcfg is in epcfgindex = 1 -> hashtable[17] = 1
    0xff,    1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    // 32-47: AS as_leftupperarm has p=0x21=33 and its EPcfg is in epcfgindex = 2 -> hashtable[33] = 2
    0xff,    2, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    // 48-63: SK not present -> all row is 0xff.
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff     
};  //EO_VERIFYsizeof(s_eocfg_eps_eb1_hashtable, eoprot_endpoint_categories_numberof*eoprot_endpoint_maxnum_in_category);

static uint16_t s_eocfg_eps_eb1_ep2index(uint16_t ep)
{    
    uint8_t r = s_eocfg_eps_eb1_hashtable[ep & 0x3f];
    
    if(r < eocfg_EPs_eb1_numofendpoints)
    {
        return(r);
    }
    return(EOK_uint16dummy);
}




// - endpoint mn

static void* s_eocfg_eps_eb1_mn_epid2nvrom(eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_mn_variable_rom_Get(id));
}

static void* s_eocfg_eps_eb1_mn_dataepid2nvram(void* data, eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_mn_variable_ram_Extract(data, eocfg_EPs_eb1_boardnumber, id));
}

static eOnvID_t s_eocfg_eps_eb1_mn_epnvprogressivenumber2id(eOnvEP_t ep, uint16_t prognum)
{
    return(eoprot_ep_mn_variable_idfromprognumber_Get(eocfg_EPs_eb1_boardnumber, prognum));
}

static uint16_t s_eocfg_eps_eb1_mn_epid2nvprogressivenumber(eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_mn_variable_progressivenumber_Get(eocfg_EPs_eb1_boardnumber, id));
}

static uint16_t s_eocfg_eps_eb1_mn_ep2nvsnumberof(eOnvEP_t ep)
{
    return(eoprot_ep_mn_variables_numberof_Get(eocfg_EPs_eb1_boardnumber));
}


static void s_eocfg_eps_eb1_mn_ram_initialise(eOnvEP_t ep, void *ram)
{
    // 1. assign the ram to the local pointer, so that future calls to eocfg_nvsEP_eb1_Get_RAM() shall give the ram back
    uint16_t i = s_eocfg_eps_eb1_ep2index(ep);    
    if(EOK_uint16dummy != i)
    {   
        s_eocfg_eps_eb1_ram[i] = ram;
    }  
    
    #warning --> someone must call eoprot_ep_mn_number_of_boards_Load() before ...
    // 2. load the number of entities of motion control in this board
    eoprot_ep_mn_number_of_entities_Load(eocfg_EPs_eb1_boardnumber, s_eocfg_eps_eb1_mn_numberofeachentity);
    
    
    // 3. initialise the values of the ram   
    eoprot_ep_mn_fun_INITIALISE(ep, ram);
}


// - endpoint mc

static void* s_eocfg_eps_eb1_mc_epid2nvrom(eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_mc_variable_rom_Get(id));
}

static void* s_eocfg_eps_eb1_mc_dataepid2nvram(void* data, eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_mc_variable_ram_Extract(data, eocfg_EPs_eb1_boardnumber, id));
}

static eOnvID_t s_eocfg_eps_eb1_mc_epnvprogressivenumber2id(eOnvEP_t ep, uint16_t prognum)
{
    return(eoprot_ep_mc_variable_idfromprognumber_Get(eocfg_EPs_eb1_boardnumber, prognum));
}

static uint16_t s_eocfg_eps_eb1_mc_epid2nvprogressivenumber(eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_mc_variable_progressivenumber_Get(eocfg_EPs_eb1_boardnumber, id));
}

static uint16_t s_eocfg_eps_eb1_mc_ep2nvsnumberof(eOnvEP_t ep)
{
    return(eoprot_ep_mc_variables_numberof_Get(eocfg_EPs_eb1_boardnumber));
}

static void s_eocfg_eps_eb1_mc_ram_initialise(eOnvEP_t ep, void *ram)
{
    // 1. assign the ram to the local pointer, so that future calls to eocfg_nvsEP_eb1_Get_RAM() shall give the ram back
    uint16_t i = s_eocfg_eps_eb1_ep2index(ep);    
    if(EOK_uint16dummy != i)
    {   
        s_eocfg_eps_eb1_ram[i] = ram;
    }  
    
    #warning --> someone must call eoprot_ep_mc_number_of_boards_Load() before ...
    // 2. load the number of entities of motion control in this board
    eoprot_ep_mc_number_of_entities_Load(eocfg_EPs_eb1_boardnumber, s_eocfg_eps_eb1_mc_numberofeachentity);
    
    
    // 3. initialise the values of the ram   
    eoprot_ep_mc_fun_INITIALISE(ep, ram);
}


// - endpoint as

static void* s_eocfg_eps_eb1_as_epid2nvrom(eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_as_variable_rom_Get(id));
}

static void* s_eocfg_eps_eb1_as_dataepid2nvram(void* data, eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_as_variable_ram_Extract(data, eocfg_EPs_eb1_boardnumber, id));
}

static eOnvID_t s_eocfg_eps_eb1_as_epnvprogressivenumber2id(eOnvEP_t ep, uint16_t prognum)
{
    return(eoprot_ep_as_variable_idfromprognumber_Get(eocfg_EPs_eb1_boardnumber, prognum));
}

static uint16_t s_eocfg_eps_eb1_as_epid2nvprogressivenumber(eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_as_variable_progressivenumber_Get(eocfg_EPs_eb1_boardnumber, id));
}

static uint16_t s_eocfg_eps_eb1_as_ep2nvsnumberof(eOnvEP_t ep)
{
    return(eoprot_ep_as_variables_numberof_Get(eocfg_EPs_eb1_boardnumber));
}

static void s_eocfg_eps_eb1_as_ram_initialise(eOnvEP_t ep, void *ram)
{
    // 1. assign the ram to the local pointer, so that future calls to eocfg_nvsEP_eb1_Get_RAM() shall give the ram back
    uint16_t i = s_eocfg_eps_eb1_ep2index(ep);    
    if(EOK_uint16dummy != i)
    {   
        s_eocfg_eps_eb1_ram[i] = ram;
    }  
    
    #warning --> someone must call eoprot_ep_as_number_of_boards_Load() before ...
    // 2. load the number of entities of motion control in this board
    eoprot_ep_as_number_of_entities_Load(eocfg_EPs_eb1_boardnumber, s_eocfg_eps_eb1_as_numberofeachentity);
    
    
    // 3. initialise the values of the ram   
    eoprot_ep_as_fun_INITIALISE(ep, ram);
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



