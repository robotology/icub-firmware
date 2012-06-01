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

/* @file       eOcfg_nvsEP_sk_emsboard_con.c
    @brief      This file keeps ....
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

#include "eOcfg_nvsEP_sk_emsboard_con.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_sk_emsboard_con_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_hash(uint16_t id);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// this struct contains the default values of the entities of the whole device

const eo_cfg_nvsEP_sk_emsboard_t eo_cfg_nvsEP_sk_emsboard_default =
{
    EO_INIT(.someskin)
    {
        EO_INIT(.sconfig)           
        {
            EO_INIT(.sigmode)       skin_sigmode_dontsignal,
            EO_INIT(.filler03)      {0xf1, 0xf2, 0xf3}
        },
        EO_INIT(.sinputs)           
        {
            EO_INIT(.filler04)      {0xf1, 0xf2, 0xf3, 0xf4}
        },
        EO_INIT(.sstatus)       
        {
            EO_INIT(.arrayof10canframes)
            {
                EO_INIT(.head)
                {
                    EO_INIT(.capacity)      10,
                    EO_INIT(.itemsize)      sizeof(eOutil_canframe_t),
                    EO_INIT(.size)          0     // it is the only one in the header that can change
                }
            },
            EO_INIT(.filler04)          {0xf1, 0xf2, 0xf3, 0xf4}         
        }
    }
}; 

  


// now we have the const netvars

#define OFFSETof_sconfig__sigmode                 (0) 
#define CAPACITY_sconfig__sigmode                 sizeof(eOenum08_t)
EOnv_con_t eo_cfg_nvsEP_sk_emsboard_sconfig__sigmode =
{   // pos =  00
    EO_INIT(.id)        EOK_cfg_nvsEP_sk_emsboard_NVID_sconfig__sigmode,
    EO_INIT(.capacity)  CAPACITY_sconfig__sigmode,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_sk_emsboard_default.someskin.sconfig.sigmode,
    EO_INIT(.offset)    OFFSETof_sconfig__sigmode,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_sk_emsboard_NVFUNTYP_sconfig__sigmode),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_sk_emsboard_NVFUNTYP_sconfig__sigmode)
};
#define OFFSETafter_sconfig__sigmode              (OFFSETof_sconfig__sigmode + CAPACITY_sconfig__sigmode)


#define OFFSETof_sconfig_filler03                           OFFSETafter_sconfig__sigmode
#define CAPACITY_sconfig_filler03                           3
//
#define OFFSETafter_sconfig_filler03                        (OFFSETof_sconfig_filler03 + CAPACITY_sconfig_filler03)


#define OFFSETof_sinputs_filler04                           OFFSETafter_sconfig_filler03
#define CAPACITY_sinputs_filler04                           4
//
#define OFFSETafter_sinputs_filler04                        (OFFSETof_sinputs_filler04 + CAPACITY_sinputs_filler04)


#define OFFSETof_sstatus__arrayof10canframes                 (OFFSETafter_sinputs_filler04) 
#define CAPACITY_sstatus__arrayof10canframes                 sizeof(EOarray_of_10canframes)
EOnv_con_t eo_cfg_nvsEP_sk_emsboard_sstatus__arrayof10canframes =
{   // pos =  01
    EO_INIT(.id)        EOK_cfg_nvsEP_sk_emsboard_NVID_sstatus__arrayof10canframes,
    EO_INIT(.capacity)  CAPACITY_sstatus__arrayof10canframes,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_sk_emsboard_default.someskin.sstatus.arrayof10canframes,
    EO_INIT(.offset)    OFFSETof_sstatus__arrayof10canframes,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_sk_emsboard_NVFUNTYP_sstatus__arrayof10canframes),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_sk_emsboard_NVFUNTYP_sstatus__arrayof10canframes)
};
#define OFFSETafter_sstatus__arrayof10canframes              (OFFSETof_sstatus__arrayof10canframes + CAPACITY_sstatus__arrayof10canframes)



#define OFFSETof_sstatus_filler04                           OFFSETafter_sstatus__arrayof10canframes
#define CAPACITY_sstatus_filler04                           4
//
#define OFFSETafter_sstatus_filler04                        (OFFSETof_sstatus_filler04 + CAPACITY_sstatus_filler04)


// guard on alignment of variables. if it doesnt compile then ... the compiler has surely inserted some holes

EO_VERIFYproposition(eocfg_nvsep_sk_emsboard, ( (OFFSETafter_sstatus_filler04) == sizeof(eOskin_someskin_t) ) );



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

const eo_cfg_nvsEP_sk_emsboard_t *eo_cfg_nvsEP_sk_emsboard_defaultvalues = &eo_cfg_nvsEP_sk_emsboard_default;

EOtreenode eo_cfg_nvsEP_sk_emsboard_tree_con[] =
{ 
    {   // 00
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_sk_emsboard_sconfig__sigmode,
        EO_INIT(.index)     0,
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) {0}
    },             
    {   // 01
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_sk_emsboard_sstatus__arrayof10canframes,
        EO_INIT(.index)     0,
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) {0}
    }
};  EO_VERIFYsizeof(eo_cfg_nvsEP_sk_emsboard_tree_con, sizeof(EOtreenode)*(varsSKemsboard_TOTALnumber));





const EOconstvector  s_eo_cfg_nvsEP_sk_emsboard_constvector_of_treenodes_EOnv_con = 
{
    EO_INIT(.size)              sizeof(eo_cfg_nvsEP_sk_emsboard_tree_con)/sizeof(EOtreenode), //EOK_cfg_nvsEP_wholebody_numberof,
    EO_INIT(.item_size)         sizeof(EOtreenode),
    EO_INIT(.item_array_data)   eo_cfg_nvsEP_sk_emsboard_tree_con
};


const EOconstvector* const eo_cfg_nvsEP_sk_emsboard_constvector_of_treenodes_EOnv_con = &s_eo_cfg_nvsEP_sk_emsboard_constvector_of_treenodes_EOnv_con;


const eOuint16_fp_uint16_t eo_cfg_nvsEP_sk_emsboard_fptr_hashfunction_id2index = eo_cfg_nvsEP_sk_emsboard_hashfunction_id2index;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern eOnvID_t eo_cfg_nvsEP_sk_emsboard_NVID_Get(eo_cfg_nvsEP_sk_emsboard_con_skinNVindex_t snvindex)
{
    static const uint8_t s_eo_cfg_nvsEP_sk_emsboard_con_nvs_funtyp[] =
    {
        EOK_cfg_nvsEP_sk_emsboard_NVFUNTYP_sconfig__sigmode,    EOK_cfg_nvsEP_sk_emsboard_NVFUNTYP_sstatus__arrayof10canframes
        
    };  EO_VERIFYsizeof(s_eo_cfg_nvsEP_sk_emsboard_con_nvs_funtyp, varsSKemsboard_TOTALnumber);


    if(snvindex >= varsSKemsboard_TOTALnumber)
    {
        return(EOK_uint16dummy);
    }
    return(EO_nv_ID(s_eo_cfg_nvsEP_sk_emsboard_con_nvs_funtyp[snvindex], EOK_cfg_nvsEP_sk_emsboard_con_NVIDoff(snvindex)));    
}


extern uint16_t eo_cfg_nvsEP_sk_emsboard_hashfunction_id2index(uint16_t id)
{
    #define IDTABLESIZE         (2)
    
    uint16_t index;
    
    static const uint16_t s_idtable[] = 
    {
        EOK_cfg_nvsEP_sk_emsboard_NVID_sconfig__sigmode,        EOK_cfg_nvsEP_sk_emsboard_NVID_sstatus__arrayof10canframes
        
    };  EO_VERIFYsizeof(s_idtable, sizeof(uint16_t)*(IDTABLESIZE));
    
    
    index = s_hash(id);
    
   
    if((index < (IDTABLESIZE)) && (id == s_idtable[index]) )
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
    return(EOK_cfg_nvsEP_sk_emsboard_con_NVindex(off));
}





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



