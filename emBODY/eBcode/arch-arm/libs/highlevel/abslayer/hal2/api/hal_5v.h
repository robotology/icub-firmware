/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Accame
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

#ifndef _HAL_5V_H_
#define _HAL_5V_H_


#ifdef __cplusplus
extern "C" {
#endif

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_5v.h
    @brief      This header file keeps public interface to the 5V.
    @author     marco.accame@iit.it, 
    @date       05/25/2015
**/

/** @defgroup doxy_group_hal_5v HAL 5V

    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 




/** @typedef    typedef struct hal_5v_cfg_t;
    @brief      contains configuration data of 5v peripheral.
 **/
typedef struct
{
    uint32_t    requiredvoltage;    /**< the required voltage of the board in mV */
    uint32_t    timeout;           /**< the timeout for which the hal_5v_on must wait in microseconds before the extvoltage is at least equal to requiredvoltage  */         
} hal_5v_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn			extern hal_result_t hal_5v_init(const hal_5v_cfg_t *cfg);
    @brief  	This function inits the 5v ... but it does not swith them on.
    @param      cfg             The configuration. It can be NULL.
    @return 	hal_res_NOK_unsupported in case the 5V cannot be initted, else hal_res_OK
  */
extern hal_result_t hal_5v_init(const hal_5v_cfg_t *cfg);


/**
    @fn         extern hal_result_t hal_5v_on(void)
    @brief      sets the 5v on
    @return     hal_res_NOK_generic in case the 5v wasn't initted or if the 5v cannot be swithed on by the timeout specified in hal_5v_cfg_t, else hal_res_OK
 **/
extern hal_result_t hal_5v_on(void);


/**
    @fn         extern hal_result_t hal_5v_off(void)
    @brief      sets the 5v off
    @return     hal_res_NOK_generic in case the 5v wasn't initted
 **/
extern hal_result_t hal_5v_off(void);


/** @}            
    end of group doxy_group_hal_5v  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



