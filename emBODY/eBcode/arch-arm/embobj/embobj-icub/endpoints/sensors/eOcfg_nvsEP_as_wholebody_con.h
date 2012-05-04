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

#ifndef _EOCFG_NVSEP_AS_WHOLEBODY_CON_H_
#define _EOCFG_NVSEP_AS_WHOLEBODY_CON_H_


#error --> dont use: obsolete

/** @file       eOcfg_nvsEP_as_wholebody_con.h
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

#include "EoSensors.h"

#include "eOcfg_nvsEP_as_any_con_body.h"


 

// - public #define  --------------------------------------------------------------------------------------------------

//#define EOK_cfg_nvsEP_as_wholebody_EP               (0x0030) 



// - declaration of public user-defined types ------------------------------------------------------------------------- 


typedef enum
{
    endpoint_as_wholebody       = 0x0020 
} eo_cfg_nvsEP_as_wholebody_endpoint_t;


/** @typedef    typedef enum eo_cfg_nvsEP_as_wholebody_con_strainNumber_t
    @brief      It contains an index for all the strain in the body.
 **/
typedef enum
{
    strainLeftArm_00 = 0,   
    strainRigthArm_01, strainLeftLeg_02, strainRigthLeg_03
} eo_cfg_nvsEP_as_wholebody_con_strainNumber_t;

enum { strainWholeBody_TOTALnumber = 4}; 



/** @typedef    typedef eo_cfg_nvsEP_as_strainNVindex_t eo_cfg_nvsEP_as_wholebody_con_strainNVindex_t
    @brief      It contains an index for all the network variables in a strain.
 **/
typedef eo_cfg_nvsEP_as_strainNVindex_t eo_cfg_nvsEP_as_wholebody_con_strainNVindex_t;



/** @typedef    typedef enum eo_cfg_nvsEP_as_wholebody_con_maisNumber_t
    @brief      It contains an index for all the mais in the body.
 **/
typedef enum
{
    maisLeftHand_00 = 0,   
    maisRigthHand_01
} eo_cfg_nvsEP_as_wholebody_con_maisNumber_t;

enum { maisWholeBody_TOTALnumber = 2}; 

/** @typedef    typedef eo_cfg_nvsEP_as_maisNVindex_t eo_cfg_nvsEP_as_wholebody_con_maisNVindex_t
    @brief      It contains an index for all the network variables in a mais.
 **/
typedef eo_cfg_nvsEP_as_maisNVindex_t eo_cfg_nvsEP_as_wholebody_con_maisNVindex_t;




/** @typedef    enum varsASwholebody_TOTALnumber;
    @brief      It contains the total number of network variables managed by the endpoint leftleg
 **/
enum {varsASwholebody_TOTALnumber = strainWholeBody_TOTALnumber*strainNVindex_TOTALnumber + maisWholeBody_TOTALnumber*maisNVindex_TOTALnumber };


/** @typedef    typedef struct eo_cfg_nvsEP_as_wholebody_t;
    @brief      contains all the variables in the wholebody of kind analog sensors
 **/
typedef struct                      // size is 56*4+48*2 = 320                
{
    eOsnsr_strain_t                 strains[strainWholeBody_TOTALnumber];
    eOsnsr_mais_t                   maises[maisWholeBody_TOTALnumber];
} eo_cfg_nvsEP_as_wholebody_t;      EO_VERIFYsizeof(eo_cfg_nvsEP_as_wholebody_t, 320);

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------


extern const eOsnsr_strain_t* eo_cfg_nvsEP_as_wholebody_strain_defaultvalue;
extern const eOsnsr_mais_t* eo_cfg_nvsEP_as_wholebody_mais_defaultvalue;

// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern eOnvID_t eo_cfg_nvsEP_as_wholebody_strain_NVID_Get(eo_cfg_nvsEP_as_wholebody_con_strainNumber_t s, eo_cfg_nvsEP_as_wholebody_con_strainNVindex_t snvindex)
    @brief      This function retrieves the eOnvID_t of a network variable with index @e snvindex for the strain number @e s
    @param      s               the strain number 
    @param      snvinxed        the index of the nv inside the strain
    @return     the nvid or EOK_uint16dummy in case of failure.
  */
extern eOnvID_t eo_cfg_nvsEP_as_wholebody_strain_NVID_Get(eo_cfg_nvsEP_as_wholebody_con_strainNumber_t s, eo_cfg_nvsEP_as_wholebody_con_strainNVindex_t snvindex);


/** @fn         extern eOnvID_t eo_cfg_nvsEP_as_wholebody_mais_NVID_Get(eo_cfg_nvsEP_as_wholebody_con_maisNumber_t s, eo_cfg_nvsEP_as_wholebody_con_maisNVindex_t mnvindex)
    @brief      This function retrieves the eOnvID_t of a network variable with index @e snvindex for the strain number @e s
    @param      m               the mais number 
    @param      mnvinxed        the index of the nv inside the mais
    @return     the nvid or EOK_uint16dummy in case of failure.
  */
extern eOnvID_t eo_cfg_nvsEP_as_wholebody_mais_NVID_Get(eo_cfg_nvsEP_as_wholebody_con_maisNumber_t s, eo_cfg_nvsEP_as_wholebody_con_maisNVindex_t mnvindex);




// - declarations used to load the endpoint into a EOnvsCfg object ----------------------------------------------------


// it is a EOconstvector where each element is a EOtreenode whose data field is a EOnv_con_t object (id, capacity, valuedef, offset)
extern const EOconstvector* const eo_cfg_nvsEP_as_wholebody_constvector_of_treenodes_EOnv_con;

// if not NULL it contains a mapping from IDs to index inside eo_cfg_nvsEP_as_wholebody_constvector_of_treenodes_EOnv_con
extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_as_wholebody_fptr_hashfunction_id2index;

// the size of the ram to be used
#define EOK_cfg_nvsEP_as_wholebody_RAMSIZE                                            (sizeof(eo_cfg_nvsEP_as_wholebody_t));

// the hash function
extern uint16_t eo_cfg_nvsEP_as_wholebody_hashfunction_id2index(uint16_t id);

/** @}            
    end of group eo_werasfdgr234  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------







