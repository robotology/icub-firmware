/* 
 * Copyright (C) 2007 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Claudio Lorini, Marco Randazzo, Valentina Gaggero
 * email:   claudio.lorini@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
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
#ifndef _STRAIN_IIR_FILTER_H
#define _STRAIN_IIR_FILTER_H

// - doxy begin -------------------------------------------------------------------------------------------------------
/*	@file 	strain_IIR_filter.h
	@brief 	This file defines data types and functions for IIR filter.
	@author claudio.lorini@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
	@date	10/03/2011
*/

// - external dependencies --------------------------------------------------------------------------------------------
#include <dsp.h>
#include "dspic30_stdint.h"

// - public #define  --------------------------------------------------------------------------------------------------
#define IIR_LPF_N_MAX_BQ 5  // Numero massimo di BiQuads dell'IIR LPF 
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

// EEPROM memorized IIR filter coefs 
// has to be separated from s_eeprom var 'couse inits data in xmem
typedef struct s_eeIIRTransposedCoefs
{
  int16_t IIR_N_BQ;
  fractional IirTransposedCoefs[5*IIR_LPF_N_MAX_BQ];   
} s_eeIIRTransposedCoefs;

// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
extern IIRTransposedStruct iirt[6];
extern s_eeIIRTransposedCoefs IirTransposedCoefs;
extern s_eeIIRTransposedCoefs /*_EEDATA(1)*/ eeIIRTransposedCoefs;
extern fractional IirTransposedState1[6][IIR_LPF_N_MAX_BQ * 2];
extern fractional IirTransposedState2[6][IIR_LPF_N_MAX_BQ * 2];


// - declaration of extern public functions ---------------------------------------------------------------------------

/**
  * @fn
  * @brief
  * @param
  * @retval
  */
//extern fractional* IIRTransposed (int numSamps, fractional* dstSamps, fractional* srcSamps, IIRTransposedStruct* filter);

/**
  * @fn
  * @brief
  * @param
  * @retval
  */
extern void strain_IIRFilter_init();


/**
  * @fn
  * @brief
  * @param
  * @retval
  */
extern void strain_IIRFilter_readFromEE(); 

/**
  * @fn
  * @brief
  * @param
  * @retval
  */
extern void strain_IIRFilter_saveInEE();

// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



