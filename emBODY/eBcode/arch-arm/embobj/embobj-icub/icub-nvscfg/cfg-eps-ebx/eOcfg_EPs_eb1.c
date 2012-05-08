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



#include "eOcfg_nvsEP_mn.h"

#include "eOcfg_nvsEP_mngmnt_con.h"
#include "eOcfg_nvsEP_mngmnt_usr.h"

#include "eOcfg_nvsEP_mc_any_con_bodypart.h"

#include "eOcfg_nvsEP_mc_upperarm_con.h"
#include "eOcfg_nvsEP_mc_upperarm_usr.h"


#include "eOcfg_nvsEP_as_onestrain_con.h"
#include "eOcfg_nvsEP_as_onestrain_usr.h"

#include "eOcfg_nvsEP_mn.h"


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

static void s_eocfg_eps_ebx_ram_retrieve(eOnvEP_t ep, void* loc, void* rem);

static uint16_t s_hash(uint16_t ep);

static uint16_t s_eo_cfg_nvsEP_eb1_hashfunction_ep2index(uint16_t ep);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


extern const EOconstvector  s_eo_cfg_nvsEP_mngmnt_constvector_of_treenodes_EOnv_con;
extern const EOconstvector  s_eo_cfg_nvsEP_mngmnt_usr_constvector_of_EOnv_usr;


extern const EOconstvector  s_eo_cfg_nvsEP_mc_upperarm_constvector_of_treenodes_EOnv_con;
extern const EOconstvector  s_eo_cfg_nvsEP_mc_upperarm_usr_constvector_of_EOnv_usr;

extern const EOconstvector  s_eo_cfg_nvsEP_as_onestrain_constvector_of_treenodes_EOnv_con;
extern const EOconstvector  s_eo_cfg_nvsEP_as_onestrain_usr_constvector_of_EOnv_usr;


static const eOnvscfg_EP_t s_eo_cfg_EPs_vectorof_eb1_data[] =
{  
    {   // mngmnt
        EO_INIT(.endpoint)                          endpoint_mn_mngmnt,
        EO_INIT(.sizeof_endpoint_data)              EOK_cfg_nvsEP_mngmnt_RAMSIZE,
        EO_INIT(.hashfunction_id2index)             eo_cfg_nvsEP_mngmnt_hashfunction_id2index,
        EO_INIT(.constvector_of_treenodes_EOnv_con) &s_eo_cfg_nvsEP_mngmnt_constvector_of_treenodes_EOnv_con, 
        EO_INIT(.constvector_of_EOnv_usr)           &s_eo_cfg_nvsEP_mngmnt_usr_constvector_of_EOnv_usr, 
        EO_INIT(.endpoint_data_init)                eo_cfg_nvsEP_mngmnt_usr_initialise,
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
    
};

static void* s_eocfg_eps_ebx_ram[][3] =
{
    {NULL, NULL, NULL},   // mngmnt
    {NULL, NULL, NULL},      
    {NULL, NULL, NULL}    
};

static const EOconstvector s_eo_cfg_EPs_vectorof_eb1 = 
{
    EO_INIT(.size)                  sizeof(s_eo_cfg_EPs_vectorof_eb1_data)/sizeof(const eOnvscfg_EP_t),
    EO_INIT(.item_size)             sizeof(eOnvscfg_EP_t),
    EO_INIT(.item_array_data)       s_eo_cfg_EPs_vectorof_eb1_data
};





// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


extern const EOconstvector* const eo_cfg_EPs_vectorof_eb1 = &s_eo_cfg_EPs_vectorof_eb1;

extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb1_fptr_hashfunction_ep2index = s_eo_cfg_nvsEP_eb1_hashfunction_ep2index;



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern void* eo_cfg_nvsEP_eb1_Get_remotelyownedRAM(eOnvEP_t ep, eOnvscfgOwnership_t ownership)
{
    uint16_t i = s_hash(ep);
    
    if(EOK_uint16dummy == i)
    {
        return(NULL);
    }
    
    return(s_eocfg_eps_ebx_ram[i][(eo_nvscfg_ownership_local == ownership) ? (1) : (2)]);       
}

extern void* eo_cfg_nvsEP_eb1_Get_locallyownedRAM(eOnvEP_t ep)
{
    uint16_t i = s_hash(ep);
    
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

static uint8_t s_hashtable[64] = 
{
    // 00-15: BS endpoint_mn_mngmnt is 1 and is in pos 0
    0xff, 0,    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    // 16-31: MC endpoint_mc_leftupperarm is 0x11 andis in pos 1, endpoint_as_leftupperarm is and is in pos 2.
    0xff, 1,    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    // 32-47: AS endpoint_as_leftupperarm is 0x21 and is in pos 2.
    0xff, 2,    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    // 48-63: SK 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff     
};

static uint16_t s_hash(uint16_t ep)
{
     
#if 0   
    uint16_t r = ep & 0x3f;
    
    if(endpoint_mn_mngmnt == r)
    {
        return(0);
    }
    else if(endpoint_mc_leftupperarm == r)
    {
        return(1);
    }
    else if(endpoint_as_leftupperarm == r)
    {
        return(2);
    }
#else
    uint16_t r = s_hashtable[ep & 0x3f];
    if(0xff != r)
    {
        return(r);
    }
    return(EOK_uint16dummy);
#endif
}

static uint16_t s_eo_cfg_nvsEP_eb1_hashfunction_ep2index(uint16_t ep)
{
    // in order to always have a hit the table s_eptable[] it must be of size equal to max{ s_hash(ep) }, thus if we
    // use an ep of value 16 and s_hash() just keeps the lsb, then the size must be 17 
    // if there are holes, they shall have EOK_uint16dummy in other entries. for example, if we have eps = {0, 7, 16}
    // then the table shall be of size 17, shall contain 0xffff everywhere but in positions 0, 7, 16 where the values
    // are ... 0, 7, 16    


    #define EPTABLESIZE     3

    static const uint16_t s_eptable[EPTABLESIZE] = 
    { 
        endpoint_mn_mngmnt,        endpoint_mc_leftupperarm,       endpoint_as_leftupperarm
    };
   
    uint16_t index = s_hash(ep);
    
    if((index < EPTABLESIZE) && (ep == s_eptable[index]) )
    {
        return(index);
    }
    else
    {
        return(EOK_uint16dummy);
    }
}

static void s_eocfg_eps_ebx_ram_retrieve(eOnvEP_t ep, void* loc, void* rem)
{
    uint16_t i = s_hash(ep);
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



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



