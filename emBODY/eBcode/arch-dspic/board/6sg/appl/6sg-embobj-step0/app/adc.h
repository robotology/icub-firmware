/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Andrea Merello
 * email:   valentina.gaggero@iit.it, andrea.merello@iit.it
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

#ifndef _ADC_H_
#define _ADC_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       adc.h
    @brief      This file provides interface to use adc AD7689 and to filter adc data. 
    @author     valentina.gaggero@iit.it, andrea.merello@iit.it
    @date       07/14/2011
**/

// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"
#include "6sg_config.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef enum adc_state_t;
    @brief      Contains possible ADC states
 **/
typedef enum
{
    adc_st_started = 0, /**< if adc_start is already invoked */
    adc_st_stopped = 1  /**< ADC is in this state at startup or afetr adc_stop has been invoked */
} adc_state_t;


   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern hal_result_t adc_init(SIXsg_config_data_t *cfg_ptr);
    @brief      Initialises adc
    @param      cgf_ptr         pointer to configuration
    @return     hal_res_NOK_hwerror if adc is non initilized correctly becaiuse an hardware error occurred.
                hal_res_NOK_nullpointer if cfg_ptr is NULL
                hal_res_OK otherwise
  */
extern hal_result_t adc_init(SIXsg_config_data_t *cfg_ptr);


/** @fn         extern void adc_start(void); 
    @brief      Starts adc sampling if in 6SG configuration tx_outMsg_mode was set not equal to tx_outMsg_off
**/
extern void adc_start(void); 


/** @fn         extern void adc_stop(void); 
    @brief      Stops adc sampling
**/
extern void adc_stop(void);


/** @fn         extern hal_result_t adc_get_data(int16_t *buff); 
    @brief      Puts in buffer the most recent filtered data
    @param      buff            pointer vector with AN_CHANNEL_NUM elements
    @return     hal_res_NOK_nullpointer if buff is NULL 
                hal_res_NOK_nodata if no data is ready
                hal_res_OK otherwise
**/
extern hal_result_t adc_get_data(int16_t *buff);


/** @fn         extern adc_state_t adc_is_running(void); 
    @brief      return ADC state
    @return     adc_st_started: if ADC is sampling
                adc_st_stopped: otherwise
**/
extern adc_state_t adc_is_running(void);


/** @fn         extern hal_result_t adc_get_ch_value(uint8_t ch_num, uint8_t filtered, int16_t *value); 
    @brief      puts in value the value og analog channel. if filtered = 1 then èput filtered value, else raw value
    @param      ch_num          number of adc channel.it must belong to [0, AN_CHANNEL_NUM)
    @param      filtered        if 0 then return raw values, otherwise filtered value
    @param      value           pointer to memory where function will put channel value
    @return     hal_res_NOK_nullpointer if buff is NULL 
                hal_res_NOK_nodata if no data is ready
                hal_res_OK otherwise
**/
extern hal_result_t adc_get_ch_value(uint8_t ch_num, uint8_t filtered, int16_t *value);


/** @fn         extern void adc_filtering_start(void);
    @brief      Starts filtering values read from adc. This function is used with EmbObj FOOP.
**/
extern void adc_filtering_start(void);


/** @fn         extern void adc_filtering_start(void);
    @brief      Filter the next channel values. This function is used with EmbObj FOOP.
 
**/
extern void adc_filtering_continue(void);

// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


