/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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
#ifndef _DSPAL_FIR_H_
#define _DSPAL_FIR_H_


/** @file       dspal_fir.h
	@brief      This header file implements public interface to the fir part of dsp abtraction layer 
	@author     marco.accame@iit.it
	@date       12/16/2010
**/

/* @defgroup dspal_fir DSP abstraction layer
    cercecvecve.
    
    @{		
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "dspal_base.h"




// - public #define  --------------------------------------------------------------------------------------------------

 
// - declaration of public user-defined types ------------------------------------------------------------------------- 


/**
* @brief Instance structure for the Q7 FIR filter.
*/
typedef struct
{
    uint16_t numTaps;               /**< number of filter coefficients in the filter. */
    dspal_q7_t *pState;             /**< points to the state variable array. The array is of length numTaps+blockSize-1. */
    dspal_q7_t *pCoeffs;            /**< points to the coefficient array. The array is of length numTaps.*/
} dspal_fir_q7_t;

/**
* @brief Instance structure for the Q15 FIR filter.
*/
typedef struct
{
    uint16_t numTaps;               /**< number of filter coefficients in the filter. */
    dspal_q15_t *pState;            /**< points to the state variable array. The array is of length numTaps+blockSize-1. */
    dspal_q15_t *pCoeffs;           /**< points to the coefficient array. The array is of length numTaps.*/
} dspal_fir_q15_t;

/**
* @brief Instance structure for the Q31 FIR filter.
*/
typedef struct
{
    uint16_t numTaps;               /**< number of filter coefficients in the filter. */
    dspal_q31_t *pState;            /**< points to the state variable array. The array is of length numTaps+blockSize-1. */
    dspal_q31_t *pCoeffs;           /**< points to the coefficient array. The array is of length numTaps. */
} dspal_fir_q31_t;

/**
* @brief Instance structure for the floating-point FIR filter.
*/
typedef struct
{
    uint16_t numTaps;               /**< number of filter coefficients in the filter. */
    dspal_f32_t *pState;              /**< points to the state variable array. The array is of length numTaps+blockSize-1. */
    dspal_f32_t *pCoeffs;             /**< points to the coefficient array. The array is of length numTaps. */
} dspal_fir_f32_t;
  

                    
    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section (so far)


       
             
/* @}            
    end of group dspal_fir  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

