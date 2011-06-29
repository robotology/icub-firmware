/* 
 * Copyright (C) 2007 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Claudio Lorini, Marco Maggiali, Marco Randazzo, Valentina Gaggero
 * email:   claudio.lorini@iit.it, marco.maggiali@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
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
// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------
/** @file    hal_mais_internalADC.c
    @brief   This file implements functions to interact with internal ADC (12 bits).
    @author  claudio.lorini@iit.it, marco.maggiali@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
    @date
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include <p30f4013.h>
#include <libpic30.h>

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "hal_mais_internalADC.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
//#include "entity_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

void hal_mais_internalADC12_init(void)
{
  uint16_t PinConfig;
  uint16_t Adcon3_reg, Adcon2_reg, Adcon1_reg;
	
  ADCON1bits.ADON = 0;         // turn off ADC

  ConfigIntADC12(ADC_INT_DISABLE);

  PinConfig  =
    ENABLE_AN0_ANA & // [ADPCFG] enable 0..12 as analog inputs 
    ENABLE_AN1_ANA &
    ENABLE_AN2_ANA &
    ENABLE_AN3_ANA &
    ENABLE_AN4_ANA &
    ENABLE_AN5_ANA &
    ENABLE_AN6_ANA &
    ENABLE_AN7_ANA &
    ENABLE_AN8_ANA &
    ENABLE_AN9_ANA &
    ENABLE_AN10_ANA &
    ENABLE_AN11_ANA;

  Adcon3_reg = ADC_SAMPLE_TIME_10 & 
    ADC_CONV_CLK_SYSTEM & 
    ADC_CONV_CLK_13Tcy;

  Adcon2_reg = ADC_VREF_AVDD_AVSS & 
    ADC_SCAN_OFF & // use ch selected in adchs
    ADC_ALT_BUF_OFF &
    ADC_ALT_INPUT_OFF &
    ADC_SAMPLES_PER_INT_1; // use one buffer, enable done reading

  Adcon1_reg = ADC_MODULE_OFF & 
    ADC_IDLE_CONTINUE & 
    ADC_FORMAT_INTG &
    ADC_CLK_MANUAL &
    ADC_AUTO_SAMPLING_OFF &
    ADC_SAMP_OFF;

  ADPCFG = PinConfig;

  // configure the input scan selection bits
  ADCON3 = Adcon3_reg;
  ADCON2 = Adcon2_reg;
  ADCON1 = Adcon1_reg;

  IFS0bits.ADIF = 0;
  ADCON1bits.ADON = 1;   
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



