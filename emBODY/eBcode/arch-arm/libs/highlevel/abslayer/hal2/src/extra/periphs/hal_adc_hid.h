/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Davide Pollarolo
 * email:   davide.pollarolo@iit.it
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

#ifndef _HAL_ADC_HID_H_
#define _HAL_ADC_HID_H_


/* @file       hal_adc_hid.h
    @brief      This header file implements hidden interface to an ADC
    @author     davide.pollarolo@iit.it
    @date       03/04/2013
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_adc.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
#define ADC1_DR_ADDRESS          ((uint32_t)0x4001204C)	//ADC1BASE +0x4C							
#define ADC2_DR_ADDRESS          ((uint32_t)0x4001214C)
#define ADC3_DR_ADDRESS          ((uint32_t)0x4001224C)
#define ADC_CDR_ADDRESS          ((uint32_t)0x40012308) // common data register?


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct
{
    uint32_t                    supportedmask;
    uint32_t                    data_regs_addresses[hal_adc_number];
} hal_adc_boardconfig_t;


// - declaration of extern hidden variables ---------------------------------------------------------------------------

extern const hal_adc_boardconfig_t hal_adc__theboardconfig;

// - declaration of extern hidden functions ---------------------------------------------------------------------------



#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
