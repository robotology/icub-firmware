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

#ifndef _HL_ETHTRANSCEIVER_H_
#define _HL_ETHTRANSCEIVER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_ethtransceiver.h
    @brief      This header file implements public interface to the hl ethtransceiver module.
    @author     marco.accame@iit.it
    @date       11/11/2013
**/

/** @defgroup arm_hl_ethtransceiver HL ETHTRANSCEIVER

    The HL ETHTRANSCEIVER ....
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"
#include "hl_eth.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct
{
    hl_bool_t           supported;      /**< hl_true if supported */
} hl_ethtransceiver_mapping_t;

/** @typedef    typedef struct hl_ethtransceiver_cfg_t;
    @brief      contains configuration data of ethtransceiver.
 **/
typedef struct
{
    uint8_t dummy;          /**< dummy...     */
} hl_ethtransceiver_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hl_ethtransceiver_cfg_t hl_ethtransceiver_cfg_default;   // = { .dummy = 0};

// it must be externally declared.
extern const hl_ethtransceiver_mapping_t hl_ethtransceiver_mapping;


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn			extern hl_result_t hl_ethtransceiver_init(const hl_ethtransceiver_cfg_t *cfg)
    @brief  	This function initializes the ethtransceiver attached to the MPU
    @param  	cfg 	        The configuration of the ethtransceiver. It can be NULL.
    @return 	hl_res_NOK_generic in case the ethtransceiver cannot be configured, else hl_res_OK
    @warning    The initialisation may temporarily stop the ethtransceiver, thus multiple calls to this function
                should be avoided as they can corrupt network traffic.
  */
extern hl_result_t hl_ethtransceiver_init(const hl_ethtransceiver_cfg_t *cfg);


extern hl_result_t hl_ethtransceiver_config(hl_eth_phymode_t targetphymode, hl_eth_phymode_t *usedphymode);


/** @fn			extern hl_bool_t hl_ethtransceiver_initted_is(void)
    @brief  	This function tells if the ethtransceiver is already initialised.
    @return 	hl_true or hl_false.
  */
extern hl_bool_t hl_ethtransceiver_initted_is(void);


// must be defined extern
extern hl_result_t hl_ethtransceiver_chip_init(const hl_ethtransceiver_cfg_t *cfg);


extern hl_result_t hl_ethtransceiver_chip_config(hl_eth_phymode_t targetphymode, hl_eth_phymode_t *usedphymode);

/** @}            
    end of group arm_hl_ethtransceiver  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



