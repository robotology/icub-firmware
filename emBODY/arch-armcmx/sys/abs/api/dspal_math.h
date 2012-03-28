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
#ifndef _DSPAL_MATH_H_
#define _DSPAL_MATH_H_


/** @file       dspal_math.h
	@brief      This header file implements public interface to the math part of dsp abstraction layer 
	@author     marco.accame@iit.it
	@date       12/16/2010
**/

/* @defgroup dspal_math DSP abstraction layer
    cercecvecve.
    
    @{		
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "dspal_base.h"




// - public #define  --------------------------------------------------------------------------------------------------

 
// - declaration of public user-defined types ------------------------------------------------------------------------- 


                    
    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section (so far)


// - declaration of extern public functions ---------------------------------------------------------------------------


/**
* @brief  Fast approximation to the trigonometric sine function for floating-point data.
* @param[in] x input value in radians.
* @return  sin(x).
*/
extern dspal_f32_t dspal_math_sin_f32(dspal_f32_t x);


/**
* @brief  Fast approximation to the trigonometric sine function for Q31 data.
* @param[in] x Scaled input value in radians.
* @return  sin(x).
*/
extern dspal_q31_t dspal_math_sin_q31(dspal_q31_t x);


/**
* @brief  Fast approximation to the trigonometric sine function for Q15 data.
* @param[in] x Scaled input value in radians.
* @return  sin(x).
*/
extern dspal_q15_t dspal_math_sin_q15(dspal_q15_t x);


/**
* @brief  Fast approximation to the trigonometric cosine function for floating-point data.
* @param[in] x input value in radians.
* @return  cos(x).
*/
extern dspal_f32_t dspal_math_cos_f32(dspal_f32_t x);


/**
* @brief Fast approximation to the trigonometric cosine function for Q31 data.
* @param[in] x Scaled input value in radians.
* @return  cos(x).
*/
extern dspal_q31_t dspal_math_cos_q31(dspal_q31_t x);


/**
* @brief  Fast approximation to the trigonometric cosine function for Q15 data.
* @param[in] x Scaled input value in radians.
* @return  cos(x).
*/
extern dspal_q15_t dspal_math_cos_q15(dspal_q15_t x);


/**
* @brief  Floating-point square root function.
* @param[in]  in     input value.
* @param[out] *pOut  square root of input value.
* @return The function returns dspal_res_OK if input value is positive value or dspal_res_NOK_argument_error if
* <code>in</code> is negative value and returns zero output for negative values.
*/
extern dspal_result_t dspal_math_sqrt_f32(dspal_f32_t in, dspal_f32_t *pOut);


/**
* @brief Q31 square root function.
* @param[in]   in    input value.  The range of the input value is [0 +1) or 0x00000000 to 0x7FFFFFFF.
* @param[out]  *pOut square root of input value.
* @return The function returns dspal_res_OK if input value is positive value or dspal_res_NOK_argument_error if
* <code>in</code> is negative value and returns zero output for negative values.
*/
extern dspal_result_t dspal_math_sqrt_q31(dspal_q31_t in, dspal_q31_t *pOut);


/**
* @brief  Q15 square root function.
* @param[in]   in     input value.  The range of the input value is [0 +1) or 0x0000 to 0x7FFF.
* @param[out]  *pOut  square root of input value.
* @return The function returns dspal_res_OK if input value is positive value or dspal_res_NOK_argument_error if
* <code>in</code> is negative value and returns zero output for negative values.
*/
extern dspal_result_t dspal_math_sqrt_q15(dspal_q15_t in, dspal_q15_t *pOut);


/*
* @brief  Floating-point sin_cos function.
* @param[in]  theta    input value in degrees 
* @param[out] *pSinVal points to the processed sine output. 
* @param[out] *pCosVal points to the processed cos output. 
* @return none.
*/
extern void dspal_math_sin_cos_f32(dspal_f32_t theta, dspal_f32_t *pSinVal, dspal_f32_t *pCcosVal);


/*
* @brief  Q31 sin_cos function.
* @param[in]  theta    scaled input value in degrees 
* @param[out] *pSinVal points to the processed sine output. 
* @param[out] *pCosVal points to the processed cosine output. 
* @return none.
*/
extern void dspal_math_sin_cos_q31(dspal_q31_t theta, dspal_q31_t *pSinVal, dspal_q31_t *pCosVal);

       
             
/* @}            
    end of group dspal_math  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

