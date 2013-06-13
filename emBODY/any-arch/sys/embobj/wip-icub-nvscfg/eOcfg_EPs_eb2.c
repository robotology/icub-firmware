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

enum { eocfg_EPs_eb2_numofendpoints = 4 };


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eocfg_eps_eb2_ram_retrieve(eOnvEP_t ep, void* loc, void* rem);
static uint16_t s_eocfg_eps_eb2_ep2index(uint16_t ep);

// endpoint mn
static void* s_eocfg_eps_eb2_mn_epid2nvrom(eOnvEP_t ep, eOnvID_t id);
static void* s_eocfg_eps_eb2_mn_dataepid2nvram(void* data, eOnvEP_t ep, eOnvID_t id);  
static uint16_t s_eocfg_eps_eb2_mn_epid2nvprogressivenumber(eOnvEP_t ep, eOnvID_t id); 
static uint16_t s_eocfg_eps_eb2_mn_ep2nvsnumberof(eOnvEP_t ep);
static void s_eocfg_eps_eb2_mn_ram_initialise(eOnvEP_t ep, void *loc, void *rem);

// endpoint mc
static void* s_eocfg_eps_eb2_mc_epid2nvrom(eOnvEP_t ep, eOnvID_t id);
static void* s_eocfg_eps_eb2_mc_dataepid2nvram(void* data, eOnvEP_t ep, eOnvID_t id);  
static uint16_t s_eocfg_eps_eb2_mc_epid2nvprogressivenumber(eOnvEP_t ep, eOnvID_t id); 
static uint16_t s_eocfg_eps_eb2_mc_ep2nvsnumberof(eOnvEP_t ep);
static void s_eocfg_eps_eb2_mc_ram_initialise(eOnvEP_t ep, void *loc, void *rem);

// endpoint as
static void* s_eocfg_eps_eb2_as_epid2nvrom(eOnvEP_t ep, eOnvID_t id);
static void* s_eocfg_eps_eb2_as_dataepid2nvram(void* data, eOnvEP_t ep, eOnvID_t id);  
static uint16_t s_eocfg_eps_eb2_as_epid2nvprogressivenumber(eOnvEP_t ep, eOnvID_t id); 
static uint16_t s_eocfg_eps_eb2_as_ep2nvsnumberof(eOnvEP_t ep);
static void s_eocfg_eps_eb2_as_ram_initialise(eOnvEP_t ep, void *loc, void *rem);

// endpoint sk
static void* s_eocfg_eps_eb2_sk_epid2nvrom(eOnvEP_t ep, eOnvID_t id);
static void* s_eocfg_eps_eb2_sk_dataepid2nvram(void* data, eOnvEP_t ep, eOnvID_t id);  
static uint16_t s_eocfg_eps_eb2_sk_epid2nvprogressivenumber(eOnvEP_t ep, eOnvID_t id); 
static uint16_t s_eocfg_eps_eb2_sk_ep2nvsnumberof(eOnvEP_t ep);
static void s_eocfg_eps_eb2_sk_ram_initialise(eOnvEP_t ep, void *loc, void *rem);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static const eOnvset_EPcfg_t s_eo_cfg_EPs_vectorof_eb2_data[] =
{  
    {   // mn-ems-board
        EO_INIT(.endpoint)                          eoprot_endpoint_mn_emsboard,
        EO_INIT(.epram_sizeof)                      sizeof(eOprot_ep_mn_emsboard_t),
        EO_INIT(.fptr_ep2nvsnumberof)               s_eocfg_eps_eb2_mn_ep2nvsnumberof,
        EO_INIT(.fptr_epid2nvprogressivenumber)     s_eocfg_eps_eb2_mn_epid2nvprogressivenumber,
        EO_INIT(.fptr_epid2nvrom)                   s_eocfg_eps_eb2_mn_epid2nvrom,
        EO_INIT(.fptr_epramepid2nvram)              s_eocfg_eps_eb2_mn_dataepid2nvram, 
        EO_INIT(.fptr_epram_initialise)             s_eocfg_eps_eb2_mn_ram_initialise,
        EO_INIT(.fptr_epram_retrieve)               s_eocfg_eps_eb2_ram_retrieve
    },        
    
    {   // mc-leftarm-lower
        EO_INIT(.endpoint)                          eoprot_endpoint_mc_leftlowerarm,
        EO_INIT(.epram_sizeof)                      sizeof(eOprot_ep_mc_leftlowerarm_t),
        EO_INIT(.fptr_ep2nvsnumberof)               s_eocfg_eps_eb2_mc_ep2nvsnumberof,
        EO_INIT(.fptr_epid2nvprogressivenumber)     s_eocfg_eps_eb2_mc_epid2nvprogressivenumber,
        EO_INIT(.fptr_epid2nvrom)                   s_eocfg_eps_eb2_mc_epid2nvrom,
        EO_INIT(.fptr_epramepid2nvram)              s_eocfg_eps_eb2_mc_dataepid2nvram, 
        EO_INIT(.fptr_epram_initialise)             s_eocfg_eps_eb2_mc_ram_initialise,
        EO_INIT(.fptr_epram_retrieve)               s_eocfg_eps_eb2_ram_retrieve
    },
    {   // as-leftarm-lower
        EO_INIT(.endpoint)                          eoprot_endpoint_as_leftlowerarm,
        EO_INIT(.epram_sizeof)                      sizeof(eOprot_ep_as_leftlowerarm_t),
        EO_INIT(.fptr_ep2nvsnumberof)               s_eocfg_eps_eb2_as_ep2nvsnumberof,
        EO_INIT(.fptr_epid2nvprogressivenumber)     s_eocfg_eps_eb2_as_epid2nvprogressivenumber,
        EO_INIT(.fptr_epid2nvrom)                   s_eocfg_eps_eb2_as_epid2nvrom,
        EO_INIT(.fptr_epramepid2nvram)              s_eocfg_eps_eb2_as_dataepid2nvram, 
        EO_INIT(.fptr_epram_initialise)             s_eocfg_eps_eb2_as_ram_initialise,
        EO_INIT(.fptr_epram_retrieve)               s_eocfg_eps_eb2_ram_retrieve
    },
    {   // sk-leftarm-lower
        EO_INIT(.endpoint)                          eoprot_endpoint_sk_leftlowerarm,
        EO_INIT(.epram_sizeof)                      sizeof(eOprot_ep_sk_lowerarm_t),
        EO_INIT(.fptr_ep2nvsnumberof)               s_eocfg_eps_eb2_sk_ep2nvsnumberof,
        EO_INIT(.fptr_epid2nvprogressivenumber)     s_eocfg_eps_eb2_sk_epid2nvprogressivenumber,
        EO_INIT(.fptr_epid2nvrom)                   s_eocfg_eps_eb2_sk_epid2nvrom,
        EO_INIT(.fptr_epramepid2nvram)              s_eocfg_eps_eb2_sk_dataepid2nvram, 
        EO_INIT(.fptr_epram_initialise)             s_eocfg_eps_eb2_sk_ram_initialise,
        EO_INIT(.fptr_epram_retrieve)               s_eocfg_eps_eb2_ram_retrieve
    }            
};  EO_VERIFYsizeof(s_eo_cfg_EPs_vectorof_eb2_data, sizeof(eOnvset_EPcfg_t)*(eocfg_EPs_eb2_numofendpoints));


static void* s_eocfg_eps_eb2_ram[][3] =
{
    {NULL, NULL, NULL},     // mn
    {NULL, NULL, NULL},     // mc
    {NULL, NULL, NULL},     // as
    {NULL, NULL, NULL}      // sk
};  EO_VERIFYsizeof(s_eocfg_eps_eb2_ram, 3*sizeof(void*)*(eocfg_EPs_eb2_numofendpoints));


const EOconstvector eo_cfg_EPs_vectorof_eb2_object = 
{
    EO_INIT(.size)                  sizeof(s_eo_cfg_EPs_vectorof_eb2_data)/sizeof(const eOnvset_EPcfg_t),
    EO_INIT(.item_size)             sizeof(eOnvset_EPcfg_t),
    EO_INIT(.item_array_data)       s_eo_cfg_EPs_vectorof_eb2_data
};


const eOnvset_DEVcfg_t eo_cfg_EPs_eb2_object =
{
    EO_INIT(.vectorof_epcfg)        &eo_cfg_EPs_vectorof_eb2_object,
    EO_INIT(.fptr_ep2indexofepcfg)  s_eocfg_eps_eb2_ep2index
};




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

const eOnvset_DEVcfg_t* eo_cfg_EPs_eb2 = &eo_cfg_EPs_eb2_object;



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern void* eo_cfg_nvsEP_eb2_Get_RAM(eOnvEP_t ep, eOnvsetOwnership_t ownership)
{
    uint16_t i = s_eocfg_eps_eb2_ep2index(ep);
    
    if(EOK_uint16dummy == i)
    {
        return(NULL);
    }
    
    return(s_eocfg_eps_eb2_ram[i][(eo_nvset_ownership_local == ownership) ? (1) : (2)]);       
}

extern void* eo_cfg_nvsEP_eb2_Get_locallyownedRAM(eOnvEP_t ep)
{
    uint16_t i = s_eocfg_eps_eb2_ep2index(ep);
    
    if(EOK_uint16dummy == i)
    {
        return(NULL);
    }
    
    return(s_eocfg_eps_eb2_ram[i][0]);       
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

// the use of the hashtable is valid only if there are 4 endpoint categories and the highest offset is 15.
// 4 rows of 16 elements.

EO_VERIFYproposition(eocfg_eps_eb2_cat, eoprot_endpoint_categories_numberof == 4);
EO_VERIFYproposition(eocfg_eps_eb2_hig, eoprot_endpoint_offset_highestvalue < 16);

// VERY IMPORTANT INFO about how to fill the hashtable:
// the table has all entries equal to 0xff except for the entries in positions p = ep&0x3f (the 6 less significant 
// bits of the endpoint) which shall contain the epcfgindex to be applied to s_eo_cfg_EPs_vectorof_eb2_data[] which
// allows to retrieve the eOnvset_EPcfg_t of the endpoint ep.
static const uint8_t s_eocfg_eps_eb2_hashtable[64] = 
{   // i use uint8_t instead of uint16_t to save space.
    // 00-15: MN eoprot_endpoint_mn_emsboard has p=0x01=1 and its EPcfg is in epcfgindex = 0 -> hashtable[1] = 0
    0xff,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    // 16-31: MC mc_leftlowerarm has p=0x12=18 and its EPcfg is in epcfgindex = 1 -> hashtable[18] = 1
    0xff, 0xff,    1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    // 32-47: AS as_leftlowerarm has p=0x22=34 and its EPcfg is in epcfgindex = 2 -> hashtable[34] = 2
    0xff, 0xff,    2, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    // 48-63: sk_leftlowerarm has p=0x32=50 and its EPcfg is in epcfgindex = 3 -> hashtable[50] = 3
    0xff, 0xff,    3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff     
};  EO_VERIFYsizeof(s_eocfg_eps_eb2_hashtable, eoprot_endpoint_categories_numberof*eoprot_endpoint_maxnum_in_category);


static uint16_t s_eocfg_eps_eb2_ep2index(uint16_t ep)
{    
    uint8_t r = s_eocfg_eps_eb2_hashtable[ep & 0x3f];
    
    if(r < eocfg_EPs_eb2_numofendpoints)
    {
        return(r);
    }
    return(EOK_uint16dummy);
}


static void s_eocfg_eps_eb2_ram_retrieve(eOnvEP_t ep, void* loc, void* rem)
{
    uint16_t i = s_eocfg_eps_eb2_ep2index(ep);
    
    if(EOK_uint16dummy != i)
    {   
        if((NULL != loc) && (NULL != rem))
        {   // remotely owned
            s_eocfg_eps_eb2_ram[i][1] = loc;
            s_eocfg_eps_eb2_ram[i][2] = rem;
        }
        else
        {
            s_eocfg_eps_eb2_ram[i][0] = loc;
        }
    }
}

// - endpoint mn

static void* s_eocfg_eps_eb2_mn_epid2nvrom(eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_mn_variable_rom_Get(ep, id));
}

static void* s_eocfg_eps_eb2_mn_dataepid2nvram(void* data, eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_mn_variable_ram_Extract(data, ep, id));
}

static uint16_t s_eocfg_eps_eb2_mn_epid2nvprogressivenumber(eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_mn_variable_progressivenumber_Get(ep, id));
    // or the slower ... eoprot_ep_variable_ID2prognumber
}

static uint16_t s_eocfg_eps_eb2_mn_ep2nvsnumberof(eOnvEP_t ep)
{
    return(eoprot_ep_mn_variables_numberof_Get(ep));  
    // or the slower ... eoprot_ep_variables_numberof_get(ep) which must detect which type of endpoint we have and the call the former   
}

static void s_eocfg_eps_eb2_mn_ram_initialise(eOnvEP_t ep, void *loc, void *rem)
{
    eoprot_ep_mn_fun_INITIALISE(ep, loc, rem);
}


// - endpoint mc

static void* s_eocfg_eps_eb2_mc_epid2nvrom(eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_mc_variable_rom_Get(ep, id));
}

static void* s_eocfg_eps_eb2_mc_dataepid2nvram(void* data, eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_mc_variable_ram_Extract(data, ep, id));
}

static uint16_t s_eocfg_eps_eb2_mc_epid2nvprogressivenumber(eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_mc_variable_progressivenumber_Get(ep, id));
    // or the slower ... eoprot_ep_variable_ID2prognumber
}

static uint16_t s_eocfg_eps_eb2_mc_ep2nvsnumberof(eOnvEP_t ep)
{
    return(eoprot_ep_mc_variables_numberof_Get(ep));  
    // or the slower ... eoprot_ep_variables_numberof_get(ep) which must detect which type of endpoint we have and the call the former   
}

static void s_eocfg_eps_eb2_mc_ram_initialise(eOnvEP_t ep, void *loc, void *rem)
{
    eoprot_ep_mc_fun_INITIALISE(ep, loc, rem);
}


// - endpoint as

static void* s_eocfg_eps_eb2_as_epid2nvrom(eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_as_variable_rom_Get(ep, id));
}

static void* s_eocfg_eps_eb2_as_dataepid2nvram(void* data, eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_as_variable_ram_Extract(data, ep, id));
}

static uint16_t s_eocfg_eps_eb2_as_epid2nvprogressivenumber(eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_as_variable_progressivenumber_Get(ep, id));
    // or the slower ... eoprot_ep_variable_ID2prognumber
}

static uint16_t s_eocfg_eps_eb2_as_ep2nvsnumberof(eOnvEP_t ep)
{
    return(eoprot_ep_as_variables_numberof_Get(ep));  
    // or the slower ... eoprot_ep_variables_numberof_get(ep) which must detect which type of endpoint we have and the call the former   
}

static void s_eocfg_eps_eb2_as_ram_initialise(eOnvEP_t ep, void *loc, void *rem)
{
    eoprot_ep_as_fun_INITIALISE(ep, loc, rem);
}


// - endpoint sk

static void* s_eocfg_eps_eb2_sk_epid2nvrom(eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_sk_variable_rom_Get(ep, id));
}

static void* s_eocfg_eps_eb2_sk_dataepid2nvram(void* data, eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_sk_variable_ram_Extract(data, ep, id));
}

static uint16_t s_eocfg_eps_eb2_sk_epid2nvprogressivenumber(eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_sk_variable_progressivenumber_Get(ep, id));
    // or the slower ... eoprot_ep_variable_ID2prognumber
}

static uint16_t s_eocfg_eps_eb2_sk_ep2nvsnumberof(eOnvEP_t ep)
{
    return(eoprot_ep_sk_variables_numberof_Get(ep));  
    // or the slower ... eoprot_ep_variables_numberof_get(ep) which must detect which type of endpoint we have and the call the former   
}

static void s_eocfg_eps_eb2_sk_ram_initialise(eOnvEP_t ep, void *loc, void *rem)
{
    eoprot_ep_sk_fun_INITIALISE(ep, loc, rem);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



