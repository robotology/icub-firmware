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

/* @file       eom_emsappl_ebx.c
	@brief      This file keeps the vcececew
	@author     marco.accame@iit.it
    @date       05/21/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMStransceiver_hid.h"
#include "string.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eom_emsappl_ebx.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------



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



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

// defined as __weak elsewhere 
extern void eom_emstransceiver_hid_userdef_add_endpoints(eOboardtransceiver_cfg_t *brdcfg)
{
    
#if     defined(EOMTHEEMSAPPLCFG_USE_EB1)
    extern const EOconstvector* const eo_cfg_EPs_vectorof_eb1;
    extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb1_fptr_hashfunction_ep2index;  
    const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb1;
    eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb1_fptr_hashfunction_ep2index;
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB2)
    extern const EOconstvector* const eo_cfg_EPs_vectorof_eb2;
    extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb2_fptr_hashfunction_ep2index;
    const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb2;
    eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb2_fptr_hashfunction_ep2index;
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB3)
    extern const EOconstvector* const eo_cfg_EPs_vectorof_eb3;
    extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb3_fptr_hashfunction_ep2index;
    const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb3;
    eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb3_fptr_hashfunction_ep2index;
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB4)
    extern const EOconstvector* const eo_cfg_EPs_vectorof_eb4;
    extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb4_fptr_hashfunction_ep2index;
    const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb4;
    eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb4_fptr_hashfunction_ep2index;
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB5)
    extern const EOconstvector* const eo_cfg_EPs_vectorof_eb5;
    extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb5_fptr_hashfunction_ep2index;
    const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb5;
    eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb5_fptr_hashfunction_ep2index;
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB6)
    extern const EOconstvector* const eo_cfg_EPs_vectorof_eb6;
    extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb6_fptr_hashfunction_ep2index;
    const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb6;
    eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb6_fptr_hashfunction_ep2index;
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB7)
    extern const EOconstvector* const eo_cfg_EPs_vectorof_eb7;
    extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb7_fptr_hashfunction_ep2index;
    const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb7;
    eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb7_fptr_hashfunction_ep2index;    
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB8)
    extern const EOconstvector* const eo_cfg_EPs_vectorof_eb8;
    extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb8_fptr_hashfunction_ep2index;
    const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb8;
    eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb8_fptr_hashfunction_ep2index;
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB9)
    extern const EOconstvector* const eo_cfg_EPs_vectorof_eb9;
    extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb9_fptr_hashfunction_ep2index;
    const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb9;
    eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb9_fptr_hashfunction_ep2index;
#else
    #error --> you must specify a EOMTHEEMSAPPLCFG_USE_EBx
#endif

    
    brdcfg->vectorof_endpoint_cfg      = vectorof_endpoint_cfg;
    brdcfg->hashfunction_ep2index      = hashfunction_ep2index;    
}

// // defined as __weak elsewhere 
// extern eOboardtransceiver_cfg_t* eom_emstransceiver_hid_userdef_get_cfg(const eOemstransceiver_cfg_t *cfg)
// {
//  
//     // every board uses the same sizes even if they are dimensioned for the worst case of EB2.
//     const eo_transceiver_sizes_t sizes =   
//     {
//         .capacityofpacket               = 1024,
//         .capacityofrop                  = 256,
//         .capacityofropframeregulars     = 768,
//         .capacityofropframeoccasionals  = 128,
//         .capacityofropframereplies      = 128,
//         .maxnumberofregularrops         = 32
//     };  

//     static eOboardtransceiver_cfg_t boardtxrxcfg = {0};

// #if     defined(EOMTHEEMSAPPLCFG_USE_EB1)
//     extern const EOconstvector* const eo_cfg_EPs_vectorof_eb1;
//     extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb1_fptr_hashfunction_ep2index;  
//     const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb1;
//     eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb1_fptr_hashfunction_ep2index;
// #elif   defined(EOMTHEEMSAPPLCFG_USE_EB2)
//     extern const EOconstvector* const eo_cfg_EPs_vectorof_eb2;
//     extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb2_fptr_hashfunction_ep2index;
//     const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb2;
//     eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb2_fptr_hashfunction_ep2index;
// #elif   defined(EOMTHEEMSAPPLCFG_USE_EB3)
//     extern const EOconstvector* const eo_cfg_EPs_vectorof_eb3;
//     extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb3_fptr_hashfunction_ep2index;
//     const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb3;
//     eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb3_fptr_hashfunction_ep2index;
// #elif   defined(EOMTHEEMSAPPLCFG_USE_EB4)
//     extern const EOconstvector* const eo_cfg_EPs_vectorof_eb4;
//     extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb4_fptr_hashfunction_ep2index;
//     const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb4;
//     eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb4_fptr_hashfunction_ep2index;
// #elif   defined(EOMTHEEMSAPPLCFG_USE_EB5)
//     extern const EOconstvector* const eo_cfg_EPs_vectorof_eb5;
//     extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb5_fptr_hashfunction_ep2index;
//     const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb5;
//     eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb5_fptr_hashfunction_ep2index;
// #elif   defined(EOMTHEEMSAPPLCFG_USE_EB6)
//     extern const EOconstvector* const eo_cfg_EPs_vectorof_eb6;
//     extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb6_fptr_hashfunction_ep2index;
//     const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb6;
//     eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb6_fptr_hashfunction_ep2index;
// #elif   defined(EOMTHEEMSAPPLCFG_USE_EB7)
//     extern const EOconstvector* const eo_cfg_EPs_vectorof_eb7;
//     extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb7_fptr_hashfunction_ep2index;
//     const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb7;
//     eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb7_fptr_hashfunction_ep2index;    
// #elif   defined(EOMTHEEMSAPPLCFG_USE_EB8)
//     extern const EOconstvector* const eo_cfg_EPs_vectorof_eb8;
//     extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb8_fptr_hashfunction_ep2index;
//     const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb8;
//     eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb8_fptr_hashfunction_ep2index;
// #elif   defined(EOMTHEEMSAPPLCFG_USE_EB9)
//     extern const EOconstvector* const eo_cfg_EPs_vectorof_eb9;
//     extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb9_fptr_hashfunction_ep2index;
//     const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb9;
//     eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb9_fptr_hashfunction_ep2index;
// #endif    

//     
//  
//     boardtxrxcfg.vectorof_endpoint_cfg          = vectorof_endpoint_cfg;
//     boardtxrxcfg.hashfunction_ep2index          = hashfunction_ep2index;
//     boardtxrxcfg.remotehostipv4addr             = cfg->hostipv4addr;
//     boardtxrxcfg.remotehostipv4port             = cfg->hostipv4port;
//     memcpy(&boardtxrxcfg.sizes, &sizes, sizeof(eo_transceiver_sizes_t));

//     return(&boardtxrxcfg);
// }


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------









// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



