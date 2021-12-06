/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Maggiali, Davide Pollarolo
 * email:   marco.maggiali@iit.it, davide.pollarolo@iit.it
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


/* @file        hal_adc.h
	@brief      This file implements internal implementation of the hal led module.
	@author     marco.maggiali@iit.it, davide.pollarolo@iit.it
    @date       26/03/2013
**/
// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _HAL_ADC_H_
#define _HAL_ADC_H_

#ifdef __cplusplus
extern "C" {
#endif

// - doxy begin -------------------------------------------------------------------------------------------------------


/** @defgroup doxy_group_hal_adc HAL ADC

    The HAL ADC is a high level module which manages AD conversion
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h"
#include "hal_gpio.h"
#include "hl_dma.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct hal_adc_t;
    @brief      contains the possible adc peripherals.
 **/
typedef enum
{
    hal_adc1 = 0,
    hal_adc2 = 1,
    hal_adc3 = 2
} hal_adc_t;

enum { hal_adc_number = 3 };


/** @typedef    typedef struct hal_adc_init_mode_ t;
    @brief      contains the possible adc initialization mode.
 **/
typedef enum
{
   hal_adc_indipendent               = 0, // dma
   hal_adc_dualmode_reg_simult       = 1, // dma 
   hal_adc_dualmode_inj_simult       = 2, // isr at the eoc
   hal_adc_triplemode_reg_simult     = 3, // dma
   hal_adc_triplemode_inj_simult     = 4, // isr at the eoc
}  hal_adc_init_mode;

/** @typedef    typedef struct hal_adc_cfg_t;
    @brief      contains configuration data of adc.
 **/
typedef struct
{
  hal_adc_init_mode init_mode;    
  hal_adc_t*        simult_adc;                             //pointer to array of hal_adc_cfg, associated to adc to be initted simultaneously [if init_mode = hal_adc_indipendent -->NULL]
  uint8_t           numberof_cfg_channels[hal_adc_number];  //max is 18 for ADC [if init_mode = hal_adc_indipendent -->only array[0] is filled]
  hal_gpio_t*       adc_channels[hal_adc_number];           //ranking of the channel is based on the position inside the array [if init_mode = hal_adc_indipendent -->only array[0] is filled]
  hl_dma_cfg_t*     dma_cfg;                                //meaninful only if init_mode = hal_adc_indipendent
} hal_adc_cfg_t;

extern const hal_adc_cfg_t hal_adc_cfg_default;

enum { hal_adc_max_channels = 16 };

typedef int16_t hal_dma_current_t; // in pos-neg milliA. value hal_NA16 is the invalid one.
typedef uint32_t hal_dma_voltage_t; // in positive milliV. value hal_NA32 is the invalid one


// - declaration of extern public functions ---------------------------------------------------------------------------
// marco.accame on 06dec2021: cleaned the un-used code
//extern hal_result_t hal_adc_dma_init(void);
//extern hal_result_t hal_adc_dma_common_init(void);
//extern hal_result_t hal_adc_dma_single_init(hal_adc_t adc);


extern hal_result_t hal_adc_common_structure_init(void);

/*  Init ADC1 and ADC 3 for:
    - Get Analog Inputs on Motor0 and Motor1 (ADC1)
    - Get Analog Inputs on Motor2 and Motor3 (ADC3)
    Data stored using DMA and accessible using the hal_adc_get_hall_sensor_analog_input... and hal_adc_get_current_motor... functions
*/
extern hal_result_t hal_adc_dma_init_ADC1_ADC3_hall_sensor_current(void);

/*  Init ADC2 for:
    - TV AUX
    - TV IN
    - Temperature Sensor
    Data stored using DMA and accessible using the hal_adc_get_tvaux_tvin_temperature_raw and hal_adc_get_tvaux_tvin_mV functions
*/
extern hal_result_t hal_adc_dma_init_ADC2_tvaux_tvin_temperature (void);

//extern hal_result_t hal_adc_ADC1_ADC3_current_init(void);

//use it if initialized all the ADCs with hal_adc_dma_init()
//extern uint16_t hal_adc_get(uint16_t ADC_type, uint16_t channel );

//use it if initialized ADC1 and ADC3 with hal_adc_ADC1_ADC3_current_init ()
//extern int16_t hal_adc_get_current(uint16_t channel);

//use them if initialized ADC2 with hal_adc_dma_init_ADC2_tvaux_tvin_temperature ()
//extern uint16_t hal_adc_get_tvaux_tvin_temperature_raw(uint16_t channel);
//extern uint32_t hal_adc_get_tvaux_tvin_mV(uint16_t channel);
extern uint32_t hal_adc_get_supplyVoltage_mV(void); //return supplied voltage to the board

//use them if initialized ADC1 and ADC3 with hal_adc_dma_init_ADC1_ADC3_hall_sensor_current ()
//extern uint16_t hal_adc_get_hall_sensor_analog_input_raw(uint8_t motor);

// returns hal_NA32 if argument is not supported or ... 
extern hal_dma_voltage_t hal_adc_get_hall_sensor_analog_input_mV(uint8_t motor);

//use them if initialized ADC1 and ADC3 with hal_adc_dma_init_ADC1_ADC3_hall_sensor_current ()
//extern uint16_t hal_adc_get_current_motor_raw(uint8_t motor);

// returns hal_NA16 if argument is not supported
extern hal_dma_current_t hal_adc_get_current_motor_mA(uint8_t motor);

/* NEW API */
//general init for ADC
//extern hal_result_t hal_adc_init(hal_adc_t adc, const hal_adc_cfg_t *cfg);

/** @}            
    end of group doxy_group_hal_adc  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
