/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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
    @date       09/06/2011
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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eocfg_eps_eb1_ram_retrieve(eOnvEP_t ep, void* loc, void* rem);

static uint16_t s_eocfg_eps_eb1_hash(uint16_t ep);

// endpoint mn
static void* s_eocfg_eps_eb1_mn_epid2nvrom(eOnvEP_t ep, eOnvID_t id);
static void* s_eocfg_eps_eb1_mn_dataepid2nvram(void* data, eOnvEP_t ep, eOnvID_t id);  
static uint16_t s_eocfg_eps_eb1_mn_epid2nvprogressivenumber(eOnvEP_t ep, eOnvID_t id); 
static uint16_t s_eocfg_eps_eb1_mn_ep2nvsnumberof(eOnvEP_t ep);
static void s_eocfg_eps_eb1_mn_ram_initialise(eOnvEP_t ep, void *loc, void *rem);

// endpoint mc
static void* s_eocfg_eps_eb1_mc_epid2nvrom(eOnvEP_t ep, eOnvID_t id);
static void* s_eocfg_eps_eb1_mc_dataepid2nvram(void* data, eOnvEP_t ep, eOnvID_t id);  
static uint16_t s_eocfg_eps_eb1_mc_epid2nvprogressivenumber(eOnvEP_t ep, eOnvID_t id); 
static uint16_t s_eocfg_eps_eb1_mc_ep2nvsnumberof(eOnvEP_t ep);
static void s_eocfg_eps_eb1_mc_ram_initialise(eOnvEP_t ep, void *loc, void *rem);

// endpoint as
static void* s_eocfg_eps_eb1_as_epid2nvrom(eOnvEP_t ep, eOnvID_t id);
static void* s_eocfg_eps_eb1_as_dataepid2nvram(void* data, eOnvEP_t ep, eOnvID_t id);  
static uint16_t s_eocfg_eps_eb1_as_epid2nvprogressivenumber(eOnvEP_t ep, eOnvID_t id); 
static uint16_t s_eocfg_eps_eb1_as_ep2nvsnumberof(eOnvEP_t ep);
static void s_eocfg_eps_eb1_as_ram_initialise(eOnvEP_t ep, void *loc, void *rem);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static const eOnvset_EPcfg_t s_eo_cfg_EPs_vectorof_eb1_data[] =
{  
    {   // mn-ems-board
        EO_INIT(.endpoint)                          eoprot_endpoint_mn_emsboard,
        EO_INIT(.epram_sizeof)                      sizeof(eOprot_ep_mn_emsboard_t),
        EO_INIT(.fptr_ep2nvsnumberof)               s_eocfg_eps_eb1_mn_ep2nvsnumberof,
        EO_INIT(.fptr_epid2nvprogressivenumber)     s_eocfg_eps_eb1_mn_epid2nvprogressivenumber,
        EO_INIT(.fptr_epid2nvrom)                   s_eocfg_eps_eb1_mn_epid2nvrom,
        EO_INIT(.fptr_epramepid2nvram)              s_eocfg_eps_eb1_mn_dataepid2nvram, 
        EO_INIT(.fptr_epram_initialise)             s_eocfg_eps_eb1_mn_ram_initialise,
        EO_INIT(.fptr_epram_retrieve)               s_eocfg_eps_eb1_ram_retrieve
    },        
    
    {   // mc-leftarm-upper
        EO_INIT(.endpoint)                          eoprot_endpoint_mc_leftupperarm,
        EO_INIT(.epram_sizeof)                      sizeof(eOprot_ep_mc_upperarm_t),
        EO_INIT(.fptr_ep2nvsnumberof)               s_eocfg_eps_eb1_mc_ep2nvsnumberof,
        EO_INIT(.fptr_epid2nvprogressivenumber)     s_eocfg_eps_eb1_mc_epid2nvprogressivenumber,
        EO_INIT(.fptr_epid2nvrom)                   s_eocfg_eps_eb1_mc_epid2nvrom,
        EO_INIT(.fptr_epramepid2nvram)              s_eocfg_eps_eb1_mc_dataepid2nvram, 
        EO_INIT(.fptr_epram_initialise)             s_eocfg_eps_eb1_mc_ram_initialise,
        EO_INIT(.fptr_epram_retrieve)               s_eocfg_eps_eb1_ram_retrieve
    },
    {   // as-leftarm-upper
        EO_INIT(.endpoint)                          eoprot_endpoint_as_leftupperarm,
        EO_INIT(.epram_sizeof)                      sizeof(eOprot_ep_as_upperarm_t),
        EO_INIT(.fptr_ep2nvsnumberof)               s_eocfg_eps_eb1_as_ep2nvsnumberof,
        EO_INIT(.fptr_epid2nvprogressivenumber)     s_eocfg_eps_eb1_as_epid2nvprogressivenumber,
        EO_INIT(.fptr_epid2nvrom)                   s_eocfg_eps_eb1_as_epid2nvrom,
        EO_INIT(.fptr_epramepid2nvram)              s_eocfg_eps_eb1_as_dataepid2nvram, 
        EO_INIT(.fptr_epram_initialise)             s_eocfg_eps_eb1_as_ram_initialise,
        EO_INIT(.fptr_epram_retrieve)               s_eocfg_eps_eb1_ram_retrieve
    }      

//     {   // mc-leftarm-upper
//         EO_INIT(.endpoint)                          eoprot_endpoint_mc_leftupperarm,
//         EO_INIT(.epram_sizeof)                      sizeof(eo_cfg_nvsEP_mc_upperarm_t),
//         EO_INIT(.fptr_ep2nvsnumberof)               s_eocfg_eps_eb1_mc_ep2nvsnumberof,
//         EO_INIT(.fptr_epid2nvprogressivenumber)     s_eocfg_eps_eb1_mc_epid2nvprogressivenumber,
//         EO_INIT(.fptr_epid2nvrom)                   s_eocfg_eps_eb1_mc_epid2nvrom,
//         EO_INIT(.fptr_epramepid2nvram)              s_eocfg_eps_eb1_mc_dataepid2nvram, 
//         EO_INIT(.fptr_epram_initialise)             NULL,
//         EO_INIT(.fptr_epram_retrieve)               NULL
//     }
    
#if 0    
    {   // mn-comm
        EO_INIT(.endpoint)                          endpoint_mn_comm,
        EO_INIT(.sizeof_endpoint_data)              EOK_cfg_nvsEP_mn_comm_RAMSIZE,
        EO_INIT(.hashfunction_id2index)             eo_cfg_nvsEP_mn_comm_hashfunction_id2index,
        EO_INIT(.constvector_of_treenodes_EOnv_con) &s_eo_cfg_nvsEP_mn_comm_constvector_of_treenodes_EOnv_con, 
        EO_INIT(.constvector_of_EOnv_usr)           &s_eo_cfg_nvsEP_mn_comm_usr_constvector_of_EOnv_usr, 
        EO_INIT(.endpoint_data_init)                eo_cfg_nvsEP_mn_comm_usr_initialise,
        EO_INIT(.endpoint_data_retrieve)            s_eocfg_eps_ebx_ram_retrieve
    },
    
    {   // mn-appl
        EO_INIT(.endpoint)                          endpoint_mn_appl,
        EO_INIT(.sizeof_endpoint_data)              EOK_cfg_nvsEP_mn_appl_RAMSIZE,
        EO_INIT(.hashfunction_id2index)             eo_cfg_nvsEP_mn_appl_hashfunction_id2index,
        EO_INIT(.constvector_of_treenodes_EOnv_con) &s_eo_cfg_nvsEP_mn_appl_constvector_of_treenodes_EOnv_con, 
        EO_INIT(.constvector_of_EOnv_usr)           &s_eo_cfg_nvsEP_mn_appl_usr_constvector_of_EOnv_usr, 
        EO_INIT(.endpoint_data_init)                eo_cfg_nvsEP_mn_appl_usr_initialise,
        EO_INIT(.endpoint_data_retrieve)            s_eocfg_eps_ebx_ram_retrieve
    },     
    
    {   // mc-leftarm-upper
        EO_INIT(.endpoint)                          endpoint_mc_leftupperarm,
        EO_INIT(.sizeof_endpoint_data)              sizeof(eo_cfg_nvsEP_mc_upperarm_t),
        EO_INIT(.hashfunction_id2index)             eo_cfg_nvsEP_mc_upperarm_hashfunction_id2index,
        EO_INIT(.constvector_of_treenodes_EOnv_con) &s_eo_cfg_nvsEP_mc_upperarm_constvector_of_treenodes_EOnv_con, 
        EO_INIT(.constvector_of_EOnv_usr)           &s_eo_cfg_nvsEP_mc_upperarm_usr_constvector_of_EOnv_usr, 
        EO_INIT(.endpoint_data_init)                eo_cfg_nvsEP_mc_upperarm_usr_initialise,
        EO_INIT(.endpoint_data_retrieve)            s_eocfg_eps_ebx_ram_retrieve
    },
    
    {   // as-leftupperarm: a strain
        EO_INIT(.endpoint)                          endpoint_as_leftupperarm,
        EO_INIT(.sizeof_endpoint_data)              sizeof(eo_cfg_nvsEP_as_onestrain_t),
        EO_INIT(.hashfunction_id2index)             eo_cfg_nvsEP_as_onestrain_hashfunction_id2index,
        EO_INIT(.constvector_of_treenodes_EOnv_con) &s_eo_cfg_nvsEP_as_onestrain_constvector_of_treenodes_EOnv_con, 
        EO_INIT(.constvector_of_EOnv_usr)           &s_eo_cfg_nvsEP_as_onestrain_usr_constvector_of_EOnv_usr, 
        EO_INIT(.endpoint_data_init)                eo_cfg_nvsEP_as_onestrain_usr_initialise,
        EO_INIT(.endpoint_data_retrieve)            s_eocfg_eps_ebx_ram_retrieve
    }
#endif    
    
};

static void* s_eocfg_eps_ebx_ram[][3] =
{
    {NULL, NULL, NULL},   // mn-comm
    {NULL, NULL, NULL},
    {NULL, NULL, NULL},      
    {NULL, NULL, NULL}    
};

const EOconstvector eo_cfg_EPs_vectorof_eb1_object = 
{
    EO_INIT(.size)                  sizeof(s_eo_cfg_EPs_vectorof_eb1_data)/sizeof(const eOnvset_EPcfg_t),
    EO_INIT(.item_size)             sizeof(eOnvset_EPcfg_t),
    EO_INIT(.item_array_data)       s_eo_cfg_EPs_vectorof_eb1_data
};


const eOnvset_DEVcfg_t eo_cfg_EPs_eb1_object =
{
    EO_INIT(.vectorof_epcfg)        &eo_cfg_EPs_vectorof_eb1_object,
    EO_INIT(.fptr_ep2indexofepcfg)  eo_cfg_nvsEP_eb1_hashfunction_ep2index
};





// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


const EOconstvector* const eo_cfg_EPs_vectorof_eb1 = &eo_cfg_EPs_vectorof_eb1_object;

const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb1_fptr_hashfunction_ep2index = eo_cfg_nvsEP_eb1_hashfunction_ep2index;

const eOnvset_DEVcfg_t* eo_cfg_EPs_eb1 = &eo_cfg_EPs_eb1_object;



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern uint16_t eo_cfg_nvsEP_eb1_hashfunction_ep2index(uint16_t ep)
{
    return(s_eocfg_eps_eb1_hash(ep));
}

extern void* eo_cfg_nvsEP_eb1_Get_remotelyownedRAM(eOnvEP_t ep, eOnvsetOwnership_t ownership)
{
    uint16_t i = s_eocfg_eps_eb1_hash(ep);
    
    if(EOK_uint16dummy == i)
    {
        return(NULL);
    }
    
    return(s_eocfg_eps_ebx_ram[i][(eo_nvset_ownership_local == ownership) ? (1) : (2)]);       
}

extern void* eo_cfg_nvsEP_eb1_Get_locallyownedRAM(eOnvEP_t ep)
{
    uint16_t i = s_eocfg_eps_eb1_hash(ep);
    
    if(EOK_uint16dummy == i)
    {
        return(NULL);
    }
    
    return(s_eocfg_eps_ebx_ram[i][0]);       
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

EO_VERIFYproposition(eocfg_eps_eb1_cat, eoprot_endpoint_categories_numberof == 4);
EO_VERIFYproposition(eocfg_eps_eb1_hig, eoprot_endpoint_offset_highestvalue < 16);

// VERY IMPORTANT INFO about how to fill the hashtable:
// the table has all entries equal to 0xff except for the entries in positions p = ep&0x3f (the 6 less significant 
// bits of the endpoint) which shall contain the epcfgindex to be applied to s_eo_cfg_EPs_vectorof_eb1_data[] which
// allows to retrieve the eOnvset_EPcfg_t of the endpoint ep.
static const uint8_t s_eocfg_eps_eb1_hashtable[64] = 
{
    // 00-15: MN eoprot_endpoint_mn_emsboard has p=0x01=1 and its EPcfg is in epcfgindex = 0 -> hashtable[1] = 0
    0xff,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    // 16-31: MC mc_leftupperarm has p=0x11=17 and its EPcfg is in epcfgindex = 1 -> hashtable[17] = 1
    0xff,    1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    // 32-47: AS as_leftupperarm has p=0x21=33 and its EPcfg is in epcfgindex = 2 -> hashtable[33] = 2
    0xff,    2, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    // 48-63: SK not present -> all row is 0xff.
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff     
};  EO_VERIFYsizeof(s_eocfg_eps_eb1_hashtable, eoprot_endpoint_categories_numberof*eoprot_endpoint_maxnum_in_category);

static uint16_t s_eocfg_eps_eb1_hash(uint16_t ep)
{    
    uint8_t r = s_eocfg_eps_eb1_hashtable[ep & 0x3f];
    if(0xff != r)
    {
        return(r);
    }
    return(EOK_uint16dummy);
}


static void s_eocfg_eps_eb1_ram_retrieve(eOnvEP_t ep, void* loc, void* rem)
{
    uint16_t i = s_eocfg_eps_eb1_hash(ep);
    if(EOK_uint16dummy != i)
    {   
        if((NULL != loc) && (NULL != rem))
        {   // remotely owned
            s_eocfg_eps_ebx_ram[i][1] = loc;
            s_eocfg_eps_ebx_ram[i][2] = rem;
        }
        else
        {
            s_eocfg_eps_ebx_ram[i][0] = loc;
        }
    }
}

// - endpoint mn

static void* s_eocfg_eps_eb1_mn_epid2nvrom(eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_mn_variable_rom_Get(ep, id));
}

static void* s_eocfg_eps_eb1_mn_dataepid2nvram(void* data, eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_mn_variable_ram_Extract(data, ep, id));
}

static uint16_t s_eocfg_eps_eb1_mn_epid2nvprogressivenumber(eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_mn_variable_progressivenumber_Get(ep, id));
    // or the slower ... eoprot_ep_variable_ID2prognumber
}

static uint16_t s_eocfg_eps_eb1_mn_ep2nvsnumberof(eOnvEP_t ep)
{
    return(eoprot_ep_mn_variables_numberof_Get(ep));  
    // or the slower ... eoprot_ep_variables_numberof_get(ep) which must detect which type of endpoint we have and the call the former   
}

static void s_eocfg_eps_eb1_mn_ram_initialise(eOnvEP_t ep, void *loc, void *rem)
{
    eoprot_ep_mn_fun_INITIALISE(ep, loc, rem);
}


// - endpoint mc

static void* s_eocfg_eps_eb1_mc_epid2nvrom(eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_mc_variable_rom_Get(ep, id));
}

static void* s_eocfg_eps_eb1_mc_dataepid2nvram(void* data, eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_mc_variable_ram_Extract(data, ep, id));
}

static uint16_t s_eocfg_eps_eb1_mc_epid2nvprogressivenumber(eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_mc_variable_progressivenumber_Get(ep, id));
    // or the slower ... eoprot_ep_variable_ID2prognumber
}

static uint16_t s_eocfg_eps_eb1_mc_ep2nvsnumberof(eOnvEP_t ep)
{
    return(eoprot_ep_mc_variables_numberof_Get(ep));  
    // or the slower ... eoprot_ep_variables_numberof_get(ep) which must detect which type of endpoint we have and the call the former   
}

static void s_eocfg_eps_eb1_mc_ram_initialise(eOnvEP_t ep, void *loc, void *rem)
{
    eoprot_ep_mc_fun_INITIALISE(ep, loc, rem);
}


// - endpoint as

static void* s_eocfg_eps_eb1_as_epid2nvrom(eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_as_variable_rom_Get(ep, id));
}

static void* s_eocfg_eps_eb1_as_dataepid2nvram(void* data, eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_as_variable_ram_Extract(data, ep, id));
}

static uint16_t s_eocfg_eps_eb1_as_epid2nvprogressivenumber(eOnvEP_t ep, eOnvID_t id)
{
    return(eoprot_ep_as_variable_progressivenumber_Get(ep, id));
    // or the slower ... eoprot_ep_variable_ID2prognumber
}

static uint16_t s_eocfg_eps_eb1_as_ep2nvsnumberof(eOnvEP_t ep)
{
    return(eoprot_ep_as_variables_numberof_Get(ep));  
    // or the slower ... eoprot_ep_variables_numberof_get(ep) which must detect which type of endpoint we have and the call the former   
}

static void s_eocfg_eps_eb1_as_ram_initialise(eOnvEP_t ep, void *loc, void *rem)
{
    eoprot_ep_as_fun_INITIALISE(ep, loc, rem);
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



