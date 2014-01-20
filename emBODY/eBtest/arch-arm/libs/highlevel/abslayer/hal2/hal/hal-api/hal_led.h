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

#ifndef _HAL_LED_H_
#define _HAL_LED_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_led.h
    @brief      This header file keeps public interface to the hal led module.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       09/16/2011
**/

/** @defgroup doxy_group_hal_led HAL LED

    The HAL LED ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hal_led_t 
    @brief      hal_led_t contains every possible led.
 **/ 
typedef enum  
{ 
    hal_led0 = 0,
    hal_led1 = 1,         
    hal_led2 = 2,
	hal_led3 = 3,
    hal_led4 = 4,         
    hal_led5 = 5,
	hal_led6 = 6,
    hal_led7 = 7
} hal_led_t;

enum { hal_leds_number = 8 };



/** @typedef    typedef struct hal_led_cfg_t;
    @brief      contains configuration data of led peripheral.
 **/
typedef struct
{
    uint8_t         dummy;              /**< dummy   */         
} hal_led_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn			extern hal_result_t hal_led_init(hal_led_t id, const hal_led_cfg_t *cfg);
    @brief  	This function inits a led.
    @details    c rfce.
    @param      led             The led to initialise. 
    @param      cfg             The configuration. It can be NULL.
    @return 	hal_res_NOK_unsupported in case the led is not supported, else hal_res_OK
  */
extern hal_result_t hal_led_init(hal_led_t id, const hal_led_cfg_t *cfg);


/**
    @fn         extern hal_result_t hal_led_on(hal_led_t id)
    @brief      sets the led on
    @param      led             The led. It must be initted before.
    @return     hal_res_NOK_generic in case the led wasn't initted, else hal_res_OK
 **/
extern hal_result_t hal_led_on(hal_led_t id);


/**
    @fn         extern hal_result_t hal_led_on(hal_led_t id)
    @brief      sets the led off
    @param      led             The led. It must be initted before.
    @return     hal_res_NOK_generic in case the led wasn't initted, else hal_res_OK
 **/
extern hal_result_t hal_led_off(hal_led_t id);


/**
    @fn         extern hal_result_t hal_led_on(hal_led_t id)
    @brief      toggles the led
    @param      led             The led. It must be initted before.
    @return     hal_res_NOK_generic in case the led wasn't initted, else hal_res_OK
 **/
extern hal_result_t hal_led_toggle(hal_led_t id);



/** @}            
    end of group doxy_group_hal_led  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



