/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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

#ifndef _HAL_ETHTRANSCEIVER_H_
#define _HAL_ETHTRANSCEIVER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_ethtransceiver.h
    @brief      This header file implements public interface to the hal ethtransceiver module.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       10/18/2011
**/

/** @defgroup arm_hal_ethtransceiver HAL ETHTRANSCEIVER

    The HAL ETHTRANSCEIVER ....
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "hal_base.h"
#include "hal_eth.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct hal_ethtransceiver_cfg_t;
    @brief      contains configuration data of ethtransceiver.
 **/
typedef struct
{
    uint8_t dummy;          /**< dummy...     */
} hal_ethtransceiver_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_ethtransceiver_cfg_t hal_ethtransceiver_cfg_default;   // = { .dummy = 0};


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn			extern hal_result_t hal_ethtransceiver_init(const hal_ethtransceiver_cfg_t *cfg)
    @brief  	This function initializes the ethtransceiver attached to the MPU
    @param  	cfg 	        The configuration of the ethtransceiver. It can be NULL.
    @return 	hal_res_NOK_generic in case the ethtransceiver cannot be configured, else hal_res_OK
    @warning    The initialisation may temporarily stop the ethtransceiver, thus multiple calls to this function
                should be avoided as they can corrupt network traffic.
  */
extern hal_result_t hal_ethtransceiver_init(const hal_ethtransceiver_cfg_t *cfg);

extern hal_result_t hal_ethtransceiver_config(hal_eth_phymode_t targetphymode, hal_eth_phymode_t *usedphymode);


/** @fn			extern hal_bool_t hal_ethtransceiver_initted_is(void)
    @brief  	This function tells if the ethtransceiver is already initialised.
    @return 	hal_true or hal_false.
  */
extern hal_bool_t hal_ethtransceiver_initted_is(void);

/** @}            
    end of group arm_hal_ethtransceiver  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



