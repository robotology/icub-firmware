/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _HL_CAN_H_
#define _HL_CAN_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_can.h
	@brief      This file contains basic initialisation api for hl can utilities.
	@author     marco.accame@iit.it
    @date       11/11/2013
**/

/** @defgroup doxy_group_hl_can HL CAN

    The HL CAN ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hl_can_t 
    @brief      hl_can_t contains the usable can ports.
 **/
typedef enum  
{ 
    hl_can1 = 0,            /**< CAN1        */
    hl_can2 = 1             /**< CAN2        */
} hl_can_t; 

enum { hl_cans_number = 2 };


/** @typedef    typedef struct hl_can_gpiomap_t 
    @brief      hl_can_gpiomap_t contains the pin mapping for can peripheral
 **/
typedef struct
{
    hl_gpio_map_t   rx;         /**< gpio map used for rx pin */
    hl_gpio_map_t   tx;         /**< gpio map used for tx pin */ 
} hl_can_gpiomap_t;


typedef struct hl_can_advcfg_bitsampling_opaque_t hl_can_advcfg_bitsampling_t;
typedef struct hl_can_advcfg_full_opaque_t hl_can_advcfg_full_t;

/** @typedef    typedef struct hl_can_advcfg_t;
    @brief      enables advanced configuration for can. 
 **/
typedef struct
{
    const hl_can_advcfg_bitsampling_t*  bitsampling;    /**< if not NULL it specifies how to sample the can bit. */    
    const hl_can_advcfg_full_t*         full;           /**< if not NULL it contains the full options of can hw initialisation. it overrides hl_can_cfg_t::baudrate */
} hl_can_advconf_t;


/** @typedef    typedef struct hl_can_mapping_t 
    @brief      hl_can_mapping_t contains the pin mapping for all can peripherals plus some advanced configuration
 **/
typedef struct
{
    uint8_t             supported_mask;             /**< bit in position hl_canx must be 1 if portx is supported */
    hl_can_gpiomap_t    gpiomap[hl_cans_number];    /**< in position hl_canx there is gpio map of CANx */
    hl_can_advconf_t    advconf[hl_cans_number];
} hl_can_mapping_t;



/** @typedef    typedef enum hl_can_baudrate_t;
    @brief      expresses can baudrate. 
 **/
typedef enum
{
    hl_can_baudrate_1mbps      = 0,
    hl_can_baudrate_500kbps    = 1    
} hl_can_baudrate_t;


/** @typedef    typedef struct hl_can_cfg_t;
    @brief      contains configuration data of can peripheral.
 **/
typedef struct
{
    hl_can_baudrate_t          baudrate; 
} hl_can_cfg_t;


 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hl_can_cfg_t hl_can_cfg_default;

extern const hl_can_mapping_t hl_can_mapping;



// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern hl_result_t hl_can_init(hl_can_t id, const hl_can_cfg_t *cfg)
    @brief      This function configures CAN. It does not configure or enable the CAN transceiver.
    @param      port            identifies CAN port (CAN1 or CAN2)
    @param      cfg             the configuration of the can peripheral
    @return     hl_res_NOK_generic in case of error, else hl_res_OK
  */
extern hl_result_t hl_can_init(hl_can_t id, const hl_can_cfg_t *cfg);


/** @fn         extern hl_result_t hl_can_enable(hl_can_t id)
    @brief      This function starts CAN. It must be invoked after hl_can_init.
    @param      port            identifies CAN port (CAN1 or CAN2)
    @return     hl_res_NOK_generic in case of error, else hl_res_OK
  */
extern hl_result_t hl_can_enable(hl_can_t id);


/** @fn         extern hl_result_t hl_can_disable(hl_can_t id)
    @brief      This function disable CAN.
    @param      port            identifies CAN port (CAN1 or CAN2)
    @return     hl_res_NOK_generic in case of error, else hl_res_OK
  */
extern hl_result_t hl_can_disable(hl_can_t id);


/** @}            
    end of group doxy_group_hl_can  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



