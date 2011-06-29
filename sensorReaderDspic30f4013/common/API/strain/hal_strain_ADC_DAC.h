/* 
 * Copyright (C) 2007 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Claudio Lorini, Marco Randazzo, Valentina Gaggero
 * email:  claudio.lorini@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
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
#ifndef _STRAIN_ADC_DAC_H_
#define _STRAIN_ADC_DAC_H_

// - doxy begin -------------------------------------------------------------------------------------------------------
/**	@file 	hal_strain_ADC_DAC.c
	@brief 	This file implements functions to interact with ADC and DAC.
	@author claudio.lorini@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
	@date	-
*/



// - external dependencies --------------------------------------------------------------------------------------------
#include "dspic30_stdint.h"

// - public #define  --------------------------------------------------------------------------------------------------
// DAC Power Down Modes
#define PDM_NORMAL     0xCFFF
#define PDM_1KTOGND    0xDFFF
#define PDM_100KTOGND  0xEFFF
#define PDM_THREESTATE 0xFFFF
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

/**
  * @fn	extern void hal_strain_AD_init(void);
  * @brief  initializes SPI peripheral, because it is used to comunicate with ADC and DAC. 
  * @param
  * @retval
  */
extern void hal_strain_AD_init(void);


/**
  * @fn	extern void hal_strain_AD_setDAC(uint8_t PowerDownMode, uint16_t DACValue);
  * @brief Actualy it isn't used!!
  * @param
  * @retval none
  */
extern void hal_strain_AD_setDAC(uint8_t PowerDownMode, uint16_t DACValue);


/**
  * @fn	extern void hal_strain_AD_getADC(uint16_t *ADCValue);
  * @brief Actualy it isn't used!!
  * @param
  * @retval none
  */
extern void hal_strain_AD_getADC(uint16_t *ADCValue);


/**
  * @fn	extern void hal_strain_AD_setDAC_getADC(uint8_t PowerDownMode, uint16_t DACValue, uint16_t *ADCValue);
  * @brief Sets @DACValue in DAC and reads @*ADCValue from ADC
  * @param
  * @retval none
  */
extern void hal_strain_AD_setDAC_getADC(uint8_t PowerDownMode, uint16_t DACValue, uint16_t *ADCValue);


// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


