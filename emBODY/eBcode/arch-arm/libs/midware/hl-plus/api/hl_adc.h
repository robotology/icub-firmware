/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Davide Pollarolo
 * email:   davide.pollarolo@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license ADC be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/
#include "hal_middleware_interface.h"

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _HL_ADC_H_
#define _HL_ADC_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_adc.h
	@brief      This file contains basic initialisation api for hl ADC utilities.
	@author     davide.pollarolo@iit.it
    @date       07/04/2015
**/

/** @defgroup doxy_group_hl_adc HL ADC

    The HL ADC
    
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"
#include "hl_core.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 
typedef enum
{
    hl_adc1 = 0,
    hl_adc2 = 1,
    hl_adc3 = 2
} hl_adc_t;

enum { hl_adcs_number = 3 };

typedef struct
{
    // put advanced stm config here
    uint8_t   dummy;
} hl_adc_advcfg_t;


typedef struct
{
    //the config must included also the adv one
    uint8_t   dummy;
    hl_adc_advcfg_t* advcfg; 
} hl_adc_cfg_t;

/** @typedef    typedef struct hl_adc_mapping_t 
 **/
typedef struct
{
    uint32_t            supportedmask;            
    uint32_t            data_regs_addresses[hl_adcs_number];
} hl_adc_mapping_t;
 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hl_adc_cfg_t hl_adc_cfg_default;

extern const hl_adc_mapping_t* hl_adc_map; 

// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern hl_result_t hl_adc_init(hl_adc_t id, const hl_adc_cfg_t *cfg)
    @brief      This function configures ADC.
    @param      id              identifies ADC id 
    @param      cfg             the configuration of the ADC peripheral
    @return     hl_res_NOK_generic in case of error, else hl_res_OK
  */
extern hl_result_t hl_adc_init(hl_adc_t id, const hl_adc_cfg_t *cfg);

/** @fn         extern hl_boolval_t hl_adc_supported_is(hl_adc_t id)
    @brief      This function tells if a given ADC periph is supported.
    @param      id              identifies ADC
    @return     hl_true or hl_false
  */
extern hl_boolval_t hl_adc_supported_is(hl_adc_t id);


/** @fn         extern hl_boolval_t hl_adc_initted_is(hl_adc_t id)
    @brief      This function tells if a given adc is initted.
    @param      id              identifies ADC port
    @return     hl_true or hl_false
  */
extern hl_boolval_t hl_adc_initted_is(hl_adc_t id);


/** @fn         extern hl_result_t hl_adc_enable(hl_adc_t id)
    @brief      This function starts ADC. It must be invoked after hl_adc_init.
    @param      id              identifies ADC 
    @return     hl_res_NOK_generic in case of error, else hl_res_OK
  */
extern hl_result_t hl_adc_enable(hl_adc_t id);


/** @fn         extern hl_result_t hl_adc_disable(hl_adc_t id)
    @brief      This function disable ADC.
    @param      id              identifies ADC port 
    @return     hl_res_NOK_generic in case of error, else hl_res_OK
  */
extern hl_result_t hl_adc_disable(hl_adc_t id);

/** @}            
    end of group doxy_group_hl_adc  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



