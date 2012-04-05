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

/* @file       eOcfg_nvsEP_mc_leftleg_con.c
    @brief      This file keeps the constant configuration for the NVs in the base endpoint port
    @author     marco.accame@iit.it
    @date       09/06/2011
**/


#error --> wip

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

#include "eOcfg_nvsEP_mc_leftleg_con.h"


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

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



// this struct contains the default values of the entities of the whole device

extern const eo_cfg_nvsEP_mc_leftleg_t eo_cfg_nvsEP_mc_leftleg_default =
{

}; 

  


// now we have the const netvars


#define OFFSETof__upto15epid2signal                 (0) 
#define CAPACITY__upto15epid2signal                 sizeof(EOarray_of_15eOnvEPID)
extern EOnv_con_t eo_cfg_nvsEP_mc_leftleg__upto15epid2signal =
{   // pos =  00
    .id                 = EOK_cfg_nvsEP_mc_leftleg_NVID__upto15epid2signal,
    .capacity           = CAPACITY__upto15epid2signal,
    .resetval           = (const void*)&eo_cfg_nvsEP_mc_leftleg_default.upto15epid2signal,
    .offset             = OFFSETof__upto15epid2signal
};
#define OFFSETafter__upto15epid2signal              (OFFSETof__upto15epid2signal + CAPACITY__upto15epid2signal)



#define OFFSETof__workingmode                       OFFSETafter__upto15epid2signal
#define CAPACITY__workingmode                       sizeof(ep_mc_leftleg_workingmode_t)
extern EOnv_con_t eo_cfg_nvsEP_mc_leftleg__workingmode =
{   // pos =  01
    .id                 = EOK_cfg_nvsEP_mc_leftleg_NVID__workingmode,
    .capacity           = CAPACITY__workingmode,
    .resetval           = (const void*)&eo_cfg_nvsEP_mc_leftleg_default.workingmode,
    .offset             = OFFSETof__workingmode
};
#define OFFSETafter__workingmode                    (OFFSETof__workingmode + CAPACITY__workingmode)


#define OFFSETof__notanv_filler0                    OFFSETafter__workingmode
#define CAPACITY__notanv_filler0                    sizeof(eo_cfg_nvsEP_mc_leftleg_default.notanv_filler0)
#define OFFSETafter__notanv_filler0                 (OFFSETof__notanv_filler0 + CAPACITY__notanv_filler0)

// guard on alignement of variables. if it doesnt compile then ... the compiler has surely inserted some holes
typedef uint8_t eo_cfg_nvsEP_mc_leftleg_t_GUARD[ ( (OFFSETafter__notanv_filler0) == sizeof(eo_cfg_nvsEP_mc_leftleg_t) ) ? (1) : (0)];

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

extern const EOtreenode eo_cfg_nvsEP_mc_leftleg_tree_con[];

extern const EOtreenode eo_cfg_nvsEP_mc_leftleg_tree_con[] =
{              
    {   // 00
        .data       = (void*)&eo_cfg_nvsEP_mc_leftleg__upto15epid2signal,
        .index      = 0,
        .nchildren  = 0,
        .ichildren  = {0},
        .pchildren  = {NULL}
    },
    {   // 1
        .data       = (void*)&eo_cfg_nvsEP_mc_leftleg__workingmode,
        .index      = 1,
        .nchildren  = 0,
        .ichildren  = {0},
        .pchildren  = {NULL}
    }    
};


const EOconstvector  s_eo_cfg_nvsEP_mc_leftleg_constvector_of_treenodes_EOnv_con = 
{
    .size               =   sizeof(eo_cfg_nvsEP_mc_leftleg_tree_con)/sizeof(const EOtreenode),
    .item_size          =   sizeof(EOtreenode),
    .item_array_data    =   eo_cfg_nvsEP_mc_leftleg_tree_con
};


extern const EOconstvector* const eo_cfg_nvsEP_mc_leftleg_constvector_of_treenodes_EOnv_con = &s_eo_cfg_nvsEP_mc_leftleg_constvector_of_treenodes_EOnv_con;



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



