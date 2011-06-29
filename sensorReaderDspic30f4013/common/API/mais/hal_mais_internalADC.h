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
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _INTERNAL_ADC_H_
#define _INTERNAL_ADC_H_

// - doxy begin -------------------------------------------------------------------------------------------------------
/** @file    hal_mais_internalADC.h
    @brief   This file provides functions to interact with internal ADC (12 bits).
    @author  claudio.lorini@iit.it, marco.maggiali@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
    @date
*/

// - external dependencies --------------------------------------------------------------------------------------------
#include <p30f4013.h>
#include <adc12.h>
#include "dspic30_stdint.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

/**
  * @fn     extern void  hal_mais_internalADC_init(void);
  * @brief  initialise internal ADC
  * @param  -
  * @retval none
  */
extern void hal_mais_internalADC12_init(void); 


/**
  * @fn     inline extern void hal_mais_internalADC12_channelSet(uint16_t channel)
  * @brief   wrapper of peripheral func (SetChanADC12)
  * @param  	channel: 
  * @retval none
  */
inline extern void hal_mais_internalADC12_channelSet(uint16_t channel)
{
	SetChanADC12(channel & ADC_CH0_NEG_SAMPLEA_NVREF);
}



/**
  * @fn     inline extern void hal_mais_internalADC_startConvert(void);
  * @brief  wrapper of peripheral func (ConvertADC12)
  * @param  -
  * @retval none
  */
inline extern void hal_mais_internalADC12_startConvert(void)
{
	ConvertADC12();
}




/**
  * @fn     inline extern void hal_mais_internalADC_startConvert(uint8_t bufIndex);
  * @brief  wrapper of peripheral func (ReadADC12)
  * @param  -
  * @retval none
  */
inline extern unsigned int hal_mais_internalADC12_read(uint8_t bufIndex)
{
	return( ReadADC12(bufIndex) );
}


/**
  * @fn     inline extern void hal_mais_internalADC12_prepare(void)
  * @brief  prepare ADC for sampling by configuring status register
  * @param  -
  * @retval none
  */
inline extern void hal_mais_internalADC12_prepare(void)
{
	ADCON1bits.DONE = 0;
}


/**
  * @fn     inline extern void hal_mais_internalADC12_startSampling(void)
  * @brief  prepare ADC for sampling by configuring status register
  * @param  -
  * @retval none
  */
inline extern void hal_mais_internalADC12_startSampling(void)
{
	ADCON1bits.SAMP = 1;
} 



/**
  * @fn     inline extern unsigned char hal_mais_internalADC12_IsConvertionDone(void)
  * @brief  configure DONE bit in status register
  * @param  -
  * @retval none
  */
inline extern unsigned char hal_mais_internalADC12_IsConvertionDone(void)
{
	return(ADCON1bits.DONE);
}



// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


