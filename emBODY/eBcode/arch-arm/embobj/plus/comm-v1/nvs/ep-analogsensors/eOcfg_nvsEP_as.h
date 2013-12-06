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
#ifndef _EOCFG_NVSEP_AS_H_
#define _EOCFG_NVSEP_AS_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       eOcfg_nvsEP_as.h
	@brief      This header file gives the ...
	@author     marco.accame@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_2uilsdede2345er Configuration of the bodypart 
    In here there are constants for the max number of joints and motors in an endpoint 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOnv.h"
#include "EoAnalogSensors.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum eOcfg_nvsEP_as_endpoint_t;
    @brief      It contains all the endpoints used for motion control. The grouping is done by sub-bodypart
                in such a way that each ep is contained by a exactly one EMS board in iCub ver 2.5
 **/
typedef enum
{
    endpoint_as_leftupperarm                        = 0x0021,   /**< used by EB1 board. it has a strain */
    endpoint_as_leftlowerarm                        = 0x0022,   /**< used by EB2 board. it has a mais   */
    
    endpoint_as_rightupperarm                       = 0x0023,   /**< used by EB3 board. it has a strain */
    endpoint_as_rightlowerarm                       = 0x0024,   /**< used by EB4 board. it has a mais   */

    endpoint_as_leftupperleg                        = 0x0026,   /**< used by EB6 board. it has a strain */    
    endpoint_as_rightupperleg                       = 0x0028    /**< used by EB8 board. it has a strain */

} eOcfg_nvsEP_as_endpoint_t;

enum { endpoint_as_TOTALnumber = 6 };



/** @typedef    typedef uint16_t eOcfg_nvsEP_as_strainNumber_t;
    @brief      It contains an index for a strain in a manner that is independent from the endpoint.
                This number is used to retrieve the network variable of that strain.
 **/
typedef uint16_t eOcfg_nvsEP_as_strainNumber_t;

enum { strainNumberMAX = 1};



/** @typedef    typedef enum eOcfg_nvsEP_as_strainNVindex_t;
    @brief      It contains an index for all the network variables in a strain. The indices are consecutive and without
                holes, so that the enum value can be changed by a normal index.
 **/
typedef enum
{
    strainNVindex_sconfig                                           =  0,
    strainNVindex_sconfig__mode                                     =  1,
    strainNVindex_sconfig__datarate                                 =  2,
    strainNVindex_sconfig__signaloncefullscale                      =  3,
    strainNVindex_sstatus                                           =  4,
    strainNVindex_sstatus__fullscale                                =  5,
    strainNVindex_sstatus__calibratedvalues                         =  6,
    strainNVindex_sstatus__uncalibratedvalues                       =  7
} eOcfg_nvsEP_as_strainNVindex_t;

enum { strainNVindex_TOTALnumber = 8};



/** @typedef    typedef uint16_t eOcfg_nvsEP_as_maisNumber_t;
    @brief      It contains an index for a mais in a manner that is independnt from the endpoint of bodypart.
 **/
typedef uint16_t eOcfg_nvsEP_as_maisNumber_t;

enum { maisNumberMAX = 1};



/** @typedef    typedef enum eOcfg_nvsEP_as_maisNVindex_t;
    @brief      It contains an index for all the network variables in a mais. The indices are consecutive and without
                holes.
 **/
typedef enum
{
    maisNVindex_mconfig                                             =  0,
    maisNVindex_mconfig__mode                                       =  1,
    maisNVindex_mconfig__datarate                                   =  2,
    maisNVindex_mconfig__resolution                                 =  3,
    maisNVindex_mstatus                                             =  4,
    maisNVindex_mstatus__the15values                                =  5
} eOcfg_nvsEP_as_maisNVindex_t;

enum { maisNVindex_TOTALnumber = 6};


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOas_strain_t* eo_cfg_nvsEP_as_strain_defaultvalue;
extern const eOas_mais_t* eo_cfg_nvsEP_as_mais_defaultvalue;

// - declaration of extern public functions ---------------------------------------------------------------------------


extern uint16_t eo_cfg_nvsEP_as_strain_numbermax_Get(eOcfg_nvsEP_as_endpoint_t ep);


extern uint16_t eo_cfg_nvsEP_as_mais_numbermax_Get(eOcfg_nvsEP_as_endpoint_t ep);



/** @fn         extern eOnvID_t eo_cfg_nvsEP_as_strain_NVID_Get(eOcfg_nvsEP_as_endpoint_t ep, eOcfg_nvsEP_as_strainNumber_t s, eOcfg_nvsEP_as_strainNVindex_t snvindex)
    @brief      This function retrieves the eOnvID_t of a network variable with index @e snvindex for the strain number @e s, given the endpoint @e ep
    @param      ep              the endpoint
    @param      s               the strain number 
    @param      snvinxed        the index of the nv inside the strain
    @return     the nvid or EOK_uint16dummy in case of failure.
  */
extern eOnvID_t eo_cfg_nvsEP_as_strain_NVID_Get(eOcfg_nvsEP_as_endpoint_t ep, eOcfg_nvsEP_as_strainNumber_t s, eOcfg_nvsEP_as_strainNVindex_t snvindex);


/** @fn         extern eOnvID_t eo_cfg_nvsEP_as_mais_NVID_Get(eOcfg_nvsEP_as_endpoint_t ep, eOcfg_nvsEP_as_maisNumber_t m, eOcfg_nvsEP_as_maisNVindex_t mnvindex)
    @brief      This function retrieves the eOnvID_t of a network variable with index @e nnvindex for the mais number @e m, given the endpoint @e ep
    @param      ep              the endpoint
    @param      m               the mais number 
    @param      mnvinxed        the index of the nv inside the mais
    @return     the nvid or EOK_uint16dummy in case of failure.
  */
extern eOnvID_t eo_cfg_nvsEP_as_mais_NVID_Get(eOcfg_nvsEP_as_endpoint_t ep, eOcfg_nvsEP_as_maisNumber_t m, eOcfg_nvsEP_as_maisNVindex_t mnvindex);


/** @}            
    end of group eo_2uilsdede2345er  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




