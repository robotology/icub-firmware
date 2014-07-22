/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Maggiali
 * email:   marco.maggiali@iit.it
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
	@author     marco.maggiali@iit.it, 
    @date       26/03/2013
**/
// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _HAL_ADC_H_
#define _HAL_ADC_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_adc.h
    @brief      This header file implements interface to a generic hal i2c module
    @author     marco.maggiali@iit.it, 
    @date       26/03/2013
**/

/** @defgroup doxy_group_hal_adc HAL ADC

    The HAL ADC is a high level module whcih manages AD conversion

    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 
extern hal_result_t hal_adc_dma_init(void);
extern hal_result_t hal_adc_inj_init(void);
extern void hal_adc_currentCalibration(void);
extern void hal_adc_currentConfig(void);
extern uint16_t hal_get_current(uint16_t channel);
void ADC_IRQHandler(void);


/** @typedef    typedef enum hal_adc_t 
    @brief      hal_adc_t contains the possible AD channels  
 **/


extern uint16_t hal_get_adc(uint16_t ADC_type, uint16_t channel );
/** @}            
    end of group doxy_group_hal_adc  
 **/

#endif  // include-guard



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
