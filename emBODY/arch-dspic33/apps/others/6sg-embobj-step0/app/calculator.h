/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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

#ifndef _CALCULATOR_H_
#define _CALCULATOR_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       calculator.h
    @brief      This file provides functions to calculate force and torque values of adc's values.
                This entity contains current tare also
    @author     valentina.gaggero@iit.it
    @date       07/14/2011
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"
#include "6sg_config.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef union calc_data_output_t;
    @brief      lets you to use calculate's output value as a simple array or divided in semantic data. 
 **/
typedef union
{
    int16_t array[AN_CHANNEL_NUM];
    struct // TODO: add packed attribute???
    {
        int16_t force[3];
        int16_t torque[3];
    }s; //s stand for semantic data
} calc_data_output_t;
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern void calculator_init(SIXsg_config_data_t *cfg_ptr);
    @brief      initialises calculator. this function must be called befor using a calculator's service.
    @param      cfg_ptr         pointer to SIXsg_config_data_t struct.
    @return     hal_res_NOK_nullpointer if cfg_ptr is NULL.(in this case calculator may crash.)
                hal_res_OK otherwise
 **/
extern hal_result_t calculator_init(SIXsg_config_data_t *cfg_ptr);


/** @fn         extern hal_result_t calculate_torque_and_force_data(int16_t *input, data_output_t *output);
    @brief      calculates force and torque values given input values. In 6SG application, input values are ADC's values.
    @param      input          array of input values.
    @param      output         pointer to memory where function puts force and torque values
    @return     hal_res_NOK_nullpointer if input or output is null pointer
                hal_res_OK otherwise
 **/
extern hal_result_t calculate_torque_and_force_data(int16_t *input, calc_data_output_t *output);


/** @fn         extern hal_result_t calculate_torque_and_force_data(int16_t *input, data_output_t *output);
    @brief      gets current tare value of channle @ch_num
    @param      ch_num         numbero of analog channel
    @param      value          pointer where function will put the required value
    @return     HAL_RES_NOK_ERROR_WRONG_PARAM if ch_null is bigger then AN_CHANNEL_NUM
                or value is a null pointer
                hal_res_OK otherwise
 **/
extern hal_result_t calc_current_tare_get_value(uint8_t ch_num, int16_t *value);


/** @fn         extern hal_result_t calc_current_tare_set_value(uint8_t ch_num, int16_t value);
    @brief      sets current tare value of channle @ch_num
    @param      ch_num         number of analog channel
    @param      value          value of current tare
    @return     HAL_RES_NOK_ERROR_WRONG_PARAM if ch_null is bigger then AN_CHANNEL_NUM
                or value is a null pointer
                HAL_RES_OK otherwise
 **/
extern hal_result_t calc_current_tare_set_value(uint8_t ch_num, int16_t value);


/** @fn         extern void calc_current_tare_init(void);
    @brief      sets 0 current tare for each channel
    @param      
    @return     
 **/
extern void calc_current_tare_init(void);


/** @fn         extern void calc_current_tare_init(void);
    @brief      sets the actula calibrate value as current tare for each channel
    @param      
    @return     
 **/extern void calc_current_tare_reset(void);

// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


