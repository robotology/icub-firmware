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

/* @file       eOcfg_nvsEP_mn_comm_con.c
    @brief      This file keeps the constant configuration for the NVs in the base endpoint port
    @author     marco.accame@iit.it
    @date       09/06/2011
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"


#include "EOnv_hid.h"
#include "EOtreenode_hid.h"


#include "EOconstvector_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_mn_comm_con.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_mn_comm_con_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

   


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static uint16_t s_hash(uint16_t id);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



// this struct contains the default values of the entities of the whole device

const eo_cfg_nvsEP_mn_comm_t eo_cfg_nvsEP_mn_comm_default =
{
    EO_INIT(.ropsigcfgcommand)
    {
        EO_INIT(.array)
        {
            EO_INIT(.head)
            {
                EO_INIT(.capacity)      NUMOFROPSIGCFG,
                EO_INIT(.itemsize)      sizeof(eOropSIGcfg_t),
                EO_INIT(.size)          0       // it is the only field in the header that can change
            },
            EO_INIT(.data)              {0}     // data can also change
        },
        EO_INIT(.cmmnd)                 ropsigcfg_cmd_none,
        EO_INIT(.filler03)              {0xf1, 0xf2, 0xf3}
    }
}; 

  


// now we have the const netvars


#define OFFSETof__ropsigcfgcommand                 (0) 
#define CAPACITY__ropsigcfgcommand                 sizeof(eOmn_ropsigcfg_command_t)
EOnv_con_t eo_cfg_nvsEP_mn_comm__ropsigcfgcommand =
{   // pos =  00
    EO_INIT(.id)        EOK_cfg_nvsEP_mn_comm_NVID__ropsigcfgcommand,
    EO_INIT(.capacity)  CAPACITY__ropsigcfgcommand,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mn_comm_default.ropsigcfgcommand,
    EO_INIT(.offset)    OFFSETof__ropsigcfgcommand,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mn_comm_NVFUNTYP__ropsigcfgcommand),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mn_comm_NVFUNTYP__ropsigcfgcommand)
};
#define OFFSETafter__ropsigcfgcommand              (OFFSETof__ropsigcfgcommand + CAPACITY__ropsigcfgcommand)



// guard on alignment of variables. if it doesnt compile then ... the compiler has surely inserted some holes

EO_VERIFYproposition(eocfg_nvsep_mn_comm, ( (OFFSETafter__ropsigcfgcommand) == sizeof(eo_cfg_nvsEP_mn_comm_t) ) );


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

const eo_cfg_nvsEP_mn_comm_t *eo_cfg_nvsEP_mn_comm_defaultvalue = &eo_cfg_nvsEP_mn_comm_default;

//EOtreenode eo_cfg_nvsEP_mn_comm_tree_con[];

EOtreenode eo_cfg_nvsEP_mn_comm_tree_con[] =
{              
    {   // 00
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mn_comm__ropsigcfgcommand,
        EO_INIT(.index)     0,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0}
    }
};  EO_VERIFYsizeof(eo_cfg_nvsEP_mn_comm_tree_con, sizeof(EOtreenode)*(EOK_cfg_nvsEP_mn_comm_con_NUMofVARS));


const EOconstvector  s_eo_cfg_nvsEP_mn_comm_constvector_of_treenodes_EOnv_con = 
{
    EO_INIT(.size)              sizeof(eo_cfg_nvsEP_mn_comm_tree_con)/sizeof(EOtreenode),
    EO_INIT(.item_size)         sizeof(EOtreenode),
    EO_INIT(.item_array_data)   eo_cfg_nvsEP_mn_comm_tree_con
};


const EOconstvector* const eo_cfg_nvsEP_mn_comm_constvector_of_treenodes_EOnv_con = &s_eo_cfg_nvsEP_mn_comm_constvector_of_treenodes_EOnv_con;


const eOuint16_fp_uint16_t eo_cfg_nvsEP_mn_comm_fnptr_hashfunction_id2index = eo_cfg_nvsEP_mn_comm_hashfunction_id2index;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern eOnvID_t eo_cfg_nvsEP_mn_comm_comm_NVID_Get(eOcfg_nvsEP_mn_commNVindex_t nvindex)
{
    static const uint8_t s_eo_cfg_nvsEP_mn_comm_con_nvs_funtyp[] =
    {
        EOK_cfg_nvsEP_mn_comm_NVFUNTYP__ropsigcfgcommand
        
    };  EO_VERIFYsizeof(s_eo_cfg_nvsEP_mn_comm_con_nvs_funtyp, EOK_cfg_nvsEP_mn_comm_con_NUMofVARS);


    if(nvindex >= EOK_cfg_nvsEP_mn_comm_con_NUMofVARS)
    {
        return(EOK_uint16dummy);
    }
    return(EO_nv_ID(s_eo_cfg_nvsEP_mn_comm_con_nvs_funtyp[nvindex], EOK_cfg_nvsEP_mn_comm_con_NVIDoff(nvindex)));    
}

extern uint16_t eo_cfg_nvsEP_mn_comm_hashfunction_id2index(uint16_t id)
{
    #define IDTABLESIZE     1

    // in order to always have a hit the table s_idtable[] it must be of size equal to max{ s_hash(id) }, thus if we
    // use an id of value 16 and s_hash() just keeps the lsb, then the size must be 17 
    // if there are holes, they shall have EOK_uint16dummy in other entries. for example, if we have ids = {0, 7, 16}
    // then the table shall be of size 17, shall contain 0xffff everywhere but in positions 0, 7, 16 where the values
    // are ... 0, 7, 16
    
    uint16_t index;

    static const uint16_t s_idtable[] = 
    { 
        EOK_cfg_nvsEP_mn_comm_NVID__ropsigcfgcommand
        
    };  EO_VERIFYsizeof(s_idtable, sizeof(uint16_t)*(IDTABLESIZE));

    index = s_hash(id);
    
    if((index < IDTABLESIZE) && (id == s_idtable[index]) )
    {
        return(index);
    }
    else
    {
        return(EOK_uint16dummy);
    }
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static uint16_t s_hash(uint16_t id)
{
    uint16_t off = EO_nv_OFF(id);
    return(EOK_cfg_nvsEP_mn_comm_con_NVindex(off));
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



