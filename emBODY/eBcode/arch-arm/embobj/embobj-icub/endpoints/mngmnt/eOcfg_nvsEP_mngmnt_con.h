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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOCFG_NVSEP_MNGMNT_CON_H_
#define _EOCFG_NVSEP_MNGMNT_CON_H_




/** @file       eOcfg_nvsEP_mngmnt_con.h
	@brief      This header file gives the constant configuration for the NVs in the management endpoint port
	@author     marco.accame@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_uilsdede Configuation of the NVs for management of the ems board
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOconstvector.h"

#include "EOarray.h"
#include "EOnv.h"
#include "EOrop.h"


// - public #define  --------------------------------------------------------------------------------------------------



// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef enum
{
    endpoint_mngmnt                             = 0x0001 
} eo_cfg_nvsEP_mngmnt_endpoint_t;


/** @typedef    typedef enum eo_cfg_nvsEP_mc_any_con_bodypart_jointNVindex_t;
    @brief      It contains an index for all the network variables in a joint. The indices are consecutive and without
                holes, so that the enum value can be changed by a normal index.
 **/
typedef enum
{
    mngmntNVindex__ropsigcfgassign                          =  0,
    mngmntNVindex__ropsigcfgcommand                         =  1
} eo_cfg_nvsEP_mngmnt_NVindex_t;

enum { mngmntNVindex_TOTALnumber = 2};

#undef _EP_MNGMNT_USE_10ROPSIGCFG_
#if defined(_EP_MNGMNT_USE_10ROPSIGCFG_)
#define NUMOFROPSIGCFG 10
typedef struct              // size is 4+10*6=64 bytes
{
    eOarray_head_t          head;
    uint8_t                 data[NUMOFROPSIGCFG*sizeof(eOropSIGcfg_t)];
} EOarray_of_10eOropSIGcfg; EO_VERIFYsizeof(EOarray_of_10eOropSIGcfg, 64);
typedef EOarray_of_10eOropSIGcfg eo_cfg_nvsEP_mngmnt_ropsigcfg_array_t; 
#else
#define NUMOFROPSIGCFG 18
typedef struct              // size is 4+18*6=112 bytes
{
    eOarray_head_t          head;
    uint8_t                 data[NUMOFROPSIGCFG*sizeof(eOropSIGcfg_t)];
} EOarray_of_18eOropSIGcfg; EO_VERIFYsizeof(EOarray_of_18eOropSIGcfg, 112);
typedef EOarray_of_18eOropSIGcfg eo_cfg_nvsEP_mngmnt_ropsigcfg_array_t;  
#endif

typedef enum 
{
    ropsigcfg_cmd_none      = 0,
    ropsigcfg_cmd_pushback  = 1,
    ropsigcfg_cmd_popback   = 2
} eo_cfg_nvsEP_mngmnt_ropsigcfg_commandtype_t;

typedef struct              // size is 6+1+1 = 8 bytes
{
    eOropSIGcfg_t           ropsigcfg;                  /**< the ropsigcfg to use if the commandtype requires it. */
    eOenum08_t              commandtype;                /**< use eo_cfg_nvsEP_mngmnt_ropsigcfg_commandtype_t */
    uint8_t                 filler01[01];
} eo_cfg_nvsEP_mngmnt_ropsigcfg_command_t;  EO_VERIFYsizeof(eo_cfg_nvsEP_mngmnt_ropsigcfg_command_t, 8);


/** @typedef    typedef struct eo_cfg_nvsEP_mngmnt_t;
    @brief      contains all the variables in the management endpoint.
 **/
typedef struct              // size is 112+8+0 = 120 bytes
{
    eo_cfg_nvsEP_mngmnt_ropsigcfg_array_t   ropsigcfgassign;           // assign the whole array
    eo_cfg_nvsEP_mngmnt_ropsigcfg_command_t ropsigcfgcommand;          // push-back or pop-back a single item
} eo_cfg_nvsEP_mngmnt_t;                    EO_VERIFYsizeof(eo_cfg_nvsEP_mngmnt_t, 120)

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eo_cfg_nvsEP_mngmnt_t eo_cfg_nvsEP_mngmnt_default;


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern eOnvID_t eo_cfg_nvsEP_mngmnt_NVID_Get(eo_cfg_nvsEP_mngmnt_NVindex_t nvindex)
    @brief      This function retrieves the eOnvID_t of a network variable with index @e nvindex.
                As an alternative use the #define values in the _hid.h file.
    @param      nvinxed         the index of the nv
    @return     the nvid or EOK_uint16dummy in case of failure.
  */
extern eOnvID_t eo_cfg_nvsEP_mngmnt_NVID_Get(eo_cfg_nvsEP_mngmnt_NVindex_t nvindex);



// - declarations used to load the endpoint into a EOnvsCfg object ----------------------------------------------------



// it is a EOconstvector where each element is a EOtreenode whose data field is a EOnv_con_t object (id, capacity, valuedef, offset)
extern const EOconstvector* const eo_cfg_nvsEP_mngmnt_constvector_of_treenodes_EOnv_con;

// if not NULL it contains a mapping from IDs to index inside eo_cfg_nvsEP_mngmnt_constvector_of_treenodes_EOnv_con
extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_mngmnt_fnptr_hashfunction_id2index;

// the size of the ram to be used
#define EOK_cfg_nvsEP_mngmnt_RAMSIZE                                            (sizeof(eo_cfg_nvsEP_mngmnt_t))

// same as the eo_cfg_nvsEP_mngmnt_fnptr_hashfunction_id2index pointer  
extern uint16_t eo_cfg_nvsEP_mngmnt_hashfunction_id2index(uint16_t nvid);



/** @}            
    end of group eo_uilsdede  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




