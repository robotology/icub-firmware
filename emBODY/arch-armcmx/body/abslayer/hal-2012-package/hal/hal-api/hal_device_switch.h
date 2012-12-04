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

#ifndef _HAL_DEVICE_SWITCH_H_
#define _HAL_DEVICE_SWITCH_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_device_switch.h
    @brief      This header file implements public interface to the hal switch module.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       10/18/2011
**/

/** @defgroup arm_hal_device_switch HAL SWITCH

    The HAL SWITCH ....
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "hal_base.h"
#include "hal_eth.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct hal_device_switch_cfg_t;
    @brief      contains configuration data of switch.
 **/
typedef struct
{
    uint8_t dummy;          /**< dummy...     */
} hal_device_switch_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_device_switch_cfg_t hal_device_switch_cfg_default;   // = { .dummy = 0};


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn			extern hal_result_t hal_device_switch_init(const hal_device_switch_cfg_t *cfg)
    @brief  	This function initializes the switch attached to the MPU
    @param  	cfg 	        The configuration of the switch. It can be NULL.
    @return 	hal_res_NOK_generic in case the switch cannot be configured, else hal_res_OK
    @warning    The initialisation may temporarily stop the switch, thus multiple calls to this function
                should be avoided as they can corrupt network traffic.
  */
extern hal_result_t hal_device_switch_init(const hal_device_switch_cfg_t *cfg);

extern hal_result_t hal_device_switch_configure(hal_eth_phymode_t targetphymode, hal_eth_phymode_t *usedphymode);


/** @fn			extern hal_bool_t hal_device_switch_initted_is(void)
    @brief  	This function tells if the switch is already initialsied.
    @return 	hal_true or hal_false.
  */
extern hal_bool_t hal_device_switch_initted_is(void);

/** @}            
    end of group arm_hal_device_switch  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



