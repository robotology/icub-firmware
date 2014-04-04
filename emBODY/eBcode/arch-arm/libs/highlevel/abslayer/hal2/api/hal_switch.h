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

#ifndef _HAL_SWITCH_H_
#define _HAL_SWITCH_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_switch.h
    @brief      This header file implements public interface to the hal switch module.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       10/18/2011
**/

/** @defgroup doxy_group_hal_switch HAL SWITCH

    The HAL SWITCH ....
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h"
#include "hal_ethtransceiver.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hal_switch_t 
    @brief      contains ids of every possible eth switch.
 **/ 
typedef enum  
{ 
    hal_switch1         = 0             /**< the only one */
} hal_switch_t;

enum { hal_switches_number = 1 };


/** @typedef    typedef struct hal_switch_cfg_t;
    @brief      contains configuration data of switch.
 **/
typedef struct
{
    uint8_t dummy;          /**< dummy...     */
} hal_switch_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_switch_cfg_t hal_switch_cfg_default;   // = { .dummy = 0};


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn			extern hal_result_t hal_switch_init(const hal_switch_cfg_t *cfg)
    @brief  	This function initializes the switch.
    @param  	cfg 	        The configuration of the switch. It can be NULL.
    @return 	hal_res_NOK_generic in case the switch cannot be configured, else hal_res_OK
  */
extern hal_result_t hal_switch_init(const hal_switch_cfg_t *cfg);


/** @fn			extern hal_result_t hal_switch_start(hal_ethtransceiver_phymode_t *usedmiiphymode)
    @brief  	This function starts the switch 
    @param  	usedmiiphymode      The used phy mode of the rmii
    @return 	hal_res_NOK_generic in case the switch cannot be configured, else hal_res_OK
  */
extern hal_result_t hal_switch_start(hal_ethtransceiver_phymode_t *usedmiiphymode);


/** @fn			extern hal_bool_t hal_switch_initted_is(void)
    @brief  	This function tells if the switch is already initialsied.
    @return 	hal_true or hal_false.
  */
extern hal_bool_t hal_switch_initted_is(void);


/** @fn			extern hal_bool_t hal_switch_started_is(void)
    @brief  	This function tells if the switch is already started.
    @return 	hal_true or hal_false.
  */
extern hal_bool_t hal_switch_started_is(void);

/** @}            
    end of group doxy_group_hal_switch  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



