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

#ifndef _EOCFG_NVSEP_SK_EMSBOARD_CON_H_
#define _EOCFG_NVSEP_SK_EMSBOARD_CON_H_




/** @file       eOcfg_nvsEP_sk_emsboard_con.h
	@brief      This header file gives the constant configuration for the NVs in the endpoint bodysensors
	@author     marco.accame@iit.it
	@date       04/05/2012
**/

/** @defgroup eo_werasfdgr234 Configuation of the NVs ...
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EOconstvector.h"
#include "EOarray.h"
#include "EOnv.h"

#include "EoSkin.h"


 

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section



// - declaration of public user-defined types ------------------------------------------------------------------------- 




/** @typedef    typedef struct eo_cfg_nvsEP_sk_emsboard_con_strainNVindex_t
    @brief      It contains an index for all the network variables in a strain.
 **/
typedef enum
{
    skinEMSboardNVindex_sconfig__sigmode                        =  0,
    skinEMSboardNVindex_sstatus__arrayof10canframe              =  1
} eo_cfg_nvsEP_sk_emsboard_con_skinNVindex_t;

enum { skinEMSboardNVindex_TOTALnumber = 2 };



/** @typedef    enum varsSKemsboard_TOTALnumber;
    @brief      It contains the total number of network variables managed by the endpoint
 **/
enum {varsSKemsboard_TOTALnumber = skinEMSboardNVindex_TOTALnumber };


/** @typedef    typedef struct eo_cfg_nvsEP_sk_emsboard_t;
    @brief      contains all the variables in the skin managed by an ems board
 **/
typedef struct                      // size is 176 = 176                
{
    eOskin_someskin_t               someskin;
} eo_cfg_nvsEP_sk_emsboard_t;       EO_VERIFYsizeof(eo_cfg_nvsEP_sk_emsboard_t, 176);

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern eOnvID_t eo_cfg_nvsEP_sk_emsboard_NVID_Get(eo_cfg_nvsEP_sk_emsboard_con_skinNVindex_t snvindex)
    @brief      This function retrieves the eOnvID_t of a network variable with index @e snvindex for the skin
    @param      snvinxed        the index of the nv inside the skin
    @return     the nvid or EOK_uint16dummy in case of failure.
  */
extern eOnvID_t eo_cfg_nvsEP_sk_emsboard_NVID_Get(eo_cfg_nvsEP_sk_emsboard_con_skinNVindex_t snvindex);




// - declarations used to load the endpoint into a EOnvsCfg object ----------------------------------------------------


// it is a EOconstvector where each element is a EOtreenode whose data field is a EOnv_con_t object (id, capacity, valuedef, offset)
extern const EOconstvector* const eo_cfg_nvsEP_sk_emsboard_constvector_of_treenodes_EOnv_con;

// if not NULL it contains a mapping from IDs to index inside eo_cfg_nvsEP_sk_emsboard_constvector_of_treenodes_EOnv_con
extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_sk_emsboard_fptr_hashfunction_id2index;

// the size of the ram to be used
#define EOK_cfg_nvsEP_sk_emsboard_RAMSIZE                                            (sizeof(eo_cfg_nvsEP_sk_emsboard_t));

// the hash function
extern uint16_t eo_cfg_nvsEP_sk_emsboard_hashfunction_id2index(uint16_t id);


extern const eo_cfg_nvsEP_sk_emsboard_t *eo_cfg_nvsEP_sk_emsboard_defaultvalues;

/** @}            
    end of group eo_werasfdgr234  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------







