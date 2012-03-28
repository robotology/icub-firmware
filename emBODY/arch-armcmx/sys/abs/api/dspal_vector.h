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
#ifndef _DSPAL_VECTOR_H_
#define _DSPAL_VECTOR_H_


/** @file       dspal_vector.h
	@brief      This header file implements public interface to the vector part of dsp abstraction layer 
	@author     marco.accame@iit.it
	@date       12/16/2010
**/

/* @defgroup dspal_vector DSP abstraction layer
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
* @brief Converts the elements of the floating-point vector to Q31 vector. 
* @param[in]       *pSrc points to the floating-point input vector 
* @param[out]      *pDst points to the Q31 output vector
* @param[in]       blockSize length of the input vector 
* @return none. 
*/
extern void dspal_vector_convert_f32_to_q31(dspal_f32_t * pSrc, dspal_q31_t * pDst, uint32_t blockSize);


/**
* @brief Converts the elements of the floating-point vector to Q15 vector. 
* @param[in]       *pSrc points to the floating-point input vector 
* @param[out]      *pDst points to the Q15 output vector
* @param[in]       blockSize length of the input vector 
* @return          none
*/
extern void dspal_vector_convert_f32_to_q15(dspal_f32_t * pSrc, dspal_q15_t * pDst, uint32_t blockSize);


/**
* @brief Converts the elements of the floating-point vector to Q7 vector. 
* @param[in]       *pSrc points to the floating-point input vector 
* @param[out]      *pDst points to the Q7 output vector
* @param[in]       blockSize length of the input vector 
* @return          none
*/
extern void dspal_vector_convert_f32_to_q7(dspal_f32_t * pSrc, dspal_q7_t * pDst, uint32_t blockSize);


/**
* @brief  Converts the elements of the Q31 vector to Q15 vector.
* @param[in]  *pSrc is input pointer
* @param[out]  *pDst is output pointer
* @param[in]  blockSize is the number of samples to process
* @return none.
*/
extern void dspal_vector_convert_dspal_q31_to_q15(dspal_q31_t * pSrc, dspal_q15_t * pDst, uint32_t blockSize);


/**
* @brief  Converts the elements of the Q31 vector to Q7 vector.
* @param[in]  *pSrc is input pointer
* @param[out]  *pDst is output pointer
* @param[in]  blockSize is the number of samples to process
* @return none.
*/
extern void dspal_vector_convert_dspal_q31_to_q7(dspal_q31_t * pSrc, dspal_q7_t * pDst, uint32_t blockSize);
 
 
/**
* @brief  Converts the elements of the Q31 vector to floating-point vector.
* @param[in]  *pSrc is input pointer
* @param[out]  *pDst is output pointer
* @param[in]  blockSize is the number of samples to process
* @return none.
*/
extern void dspal_vector_convert_dspal_q31_to_f32(dspal_q31_t * pSrc, dspal_f32_t * pDst, uint32_t blockSize);
        
        
/**
* @brief  Converts the elements of the Q15 vector to floating-point vector.
* @param[in]  *pSrc is input pointer
* @param[out]  *pDst is output pointer
* @param[in]  blockSize is the number of samples to process
* @return none.
*/
extern void dspal_vector_convert_dspal_q15_to_f32(dspal_q15_t * pSrc, dspal_f32_t * pDst, uint32_t blockSize);


/**
* @brief  Converts the elements of the Q15 vector to Q31 vector.
* @param[in]  *pSrc is input pointer
* @param[out]  *pDst is output pointer
* @param[in]  blockSize is the number of samples to process
* @return none.
*/
extern void dspal_vector_convert_dspal_q15_to_q31(dspal_q15_t * pSrc, dspal_q31_t * pDst, uint32_t blockSize);


/**
* @brief  Converts the elements of the Q15 vector to Q7 vector.
* @param[in]  *pSrc is input pointer
* @param[out]  *pDst is output pointer
* @param[in]  blockSize is the number of samples to process
* @return none.
*/
extern void dspal_vector_convert_dspal_q15_to_q7(dspal_q15_t * pSrc, dspal_q7_t * pDst, uint32_t blockSize);


/**
* @brief  Converts the elements of the Q7 vector to Q31 vector.
* @param[in]  *pSrc     input pointer
* @param[out]  *pDst    output pointer
* @param[in]  blockSize number of samples to process
* @return none.
*/
extern void dspal_vector_convert_q7_to_q31(dspal_q7_t * pSrc, dspal_q31_t * pDst, uint32_t blockSize);


/**
* @brief  Converts the elements of the Q7 vector to Q15 vector.
* @param[in]  *pSrc     input pointer
* @param[out] *pDst     output pointer
* @param[in]  blockSize number of samples to process
* @return none.
*/
extern void dspal_vector_convert_q7_to_q15(dspal_q7_t * pSrc, dspal_q15_t * pDst, uint32_t blockSize);
         

/**
* @brief  Converts the elements of the Q7 vector to floating-point vector.
* @param[in]  *pSrc is input pointer
* @param[out]  *pDst is output pointer
* @param[in]  blockSize is the number of samples to process
* @return none.
*/
extern void dspal_vector_convert_q7_to_f32(dspal_q7_t * pSrc, dspal_f32_t * pDst, uint32_t blockSize);
           

/**
* @brief  Copies the elements of a floating-point vector. 
* @param[in]  *pSrc input pointer
* @param[out]  *pDst output pointer
* @param[in]  blockSize number of samples to process
* @return none.
*/
extern void dspal_vector_copy_f32(dspal_f32_t * pSrc, dspal_f32_t * pDst, uint32_t blockSize);


/**
* @brief  Copies the elements of a Q7 vector. 
* @param[in]  *pSrc input pointer
* @param[out]  *pDst output pointer
* @param[in]  blockSize number of samples to process
* @return none.
*/
extern void dspal_vector_copy_q7(dspal_q7_t * pSrc, dspal_q7_t * pDst, uint32_t blockSize);


/**
* @brief  Copies the elements of a Q15 vector. 
* @param[in]  *pSrc input pointer
* @param[out]  *pDst output pointer
* @param[in]  blockSize number of samples to process
* @return none.
*/
extern void dspal_vector_copy_q15(dspal_q15_t * pSrc, dspal_q15_t * pDst, uint32_t blockSize);


/**
* @brief  Copies the elements of a Q31 vector. 
* @param[in]  *pSrc input pointer
* @param[out]  *pDst output pointer
* @param[in]  blockSize number of samples to process
* @return none.
*/
extern void dspal_vector_copy_q31(dspal_q31_t * pSrc, dspal_q31_t * pDst, uint32_t blockSize);


/**
* @brief  Fills a constant value into a floating-point vector. 
* @param[in]  value input value to be filled
* @param[out]  *pDst output pointer
* @param[in]  blockSize number of samples to process
* @return none.
*/
extern void dspal_vector_fill_f32(dspal_f32_t value, dspal_f32_t * pDst, uint32_t blockSize);


/**
* @brief  Fills a constant value into a Q7 vector. 
* @param[in]  value input value to be filled
* @param[out]  *pDst output pointer
* @param[in]  blockSize number of samples to process
* @return none.
*/
extern void dspal_vector_fill_q7(dspal_q7_t value, dspal_q7_t * pDst, uint32_t blockSize);


/**
* @brief  Fills a constant value into a Q15 vector. 
* @param[in]  value input value to be filled
* @param[out]  *pDst output pointer
* @param[in]  blockSize number of samples to process
* @return none.
*/
extern void dspal_vector_fill_q15(dspal_q15_t value, dspal_q15_t * pDst, uint32_t blockSize);


/**
* @brief  Fills a constant value into a Q31 vector. 
* @param[in]  value input value to be filled
* @param[out]  *pDst output pointer
* @param[in]  blockSize number of samples to process
* @return none.
*/
extern void dspal_vector_fill_q31(dspal_q31_t value, dspal_q31_t * pDst, uint32_t blockSize);


/**
* @brief Floating-point vector addition.
* @param[in]       *pSrcA points to the first input vector
* @param[in]       *pSrcB points to the second input vector
* @param[out]      *pDst points to the output vector
* @param[in]       blockSize number of samples in each vector
* @return none.
*/
extern void dspal_vector_add_f32(dspal_f32_t * pSrcA, dspal_f32_t * pSrcB, dspal_f32_t * pDst, uint32_t blockSize);


/**
* @brief Q7 vector addition.
* @param[in]       *pSrcA points to the first input vector
* @param[in]       *pSrcB points to the second input vector
* @param[out]      *pDst points to the output vector
* @param[in]       blockSize number of samples in each vector
* @return none.
*/
extern void dspal_vector_add_q7(dspal_q7_t * pSrcA, dspal_q7_t * pSrcB, dspal_q7_t * pDst, uint32_t blockSize);


/**
* @brief Q15 vector addition.
* @param[in]       *pSrcA points to the first input vector
* @param[in]       *pSrcB points to the second input vector
* @param[out]      *pDst points to the output vector
* @param[in]       blockSize number of samples in each vector
* @return none.
*/
extern void dspal_vector_add_q15(dspal_q15_t * pSrcA, dspal_q15_t * pSrcB, dspal_q15_t * pDst, uint32_t blockSize);


/**
* @brief Q31 vector addition.
* @param[in]       *pSrcA points to the first input vector
* @param[in]       *pSrcB points to the second input vector
* @param[out]      *pDst points to the output vector
* @param[in]       blockSize number of samples in each vector
* @return none.
*/
extern void dspal_vector_add_q31(dspal_q31_t * pSrcA, dspal_q31_t * pSrcB, dspal_q31_t * pDst, uint32_t blockSize);


/**
* @brief Floating-point vector subtraction.
* @param[in]       *pSrcA points to the first input vector
* @param[in]       *pSrcB points to the second input vector
* @param[out]      *pDst points to the output vector
* @param[in]       blockSize number of samples in each vector
* @return none.
*/
extern void dspal_vector_sub_f32(dspal_f32_t * pSrcA, dspal_f32_t * pSrcB, dspal_f32_t * pDst, uint32_t blockSize);


/**
* @brief Q7 vector subtraction.
* @param[in]       *pSrcA points to the first input vector
* @param[in]       *pSrcB points to the second input vector
* @param[out]      *pDst points to the output vector
* @param[in]       blockSize number of samples in each vector
* @return none.
*/
extern void dspal_vector_sub_q7(dspal_q7_t * pSrcA, dspal_q7_t * pSrcB, dspal_q7_t * pDst, uint32_t blockSize);


/**
* @brief Q15 vector subtraction.
* @param[in]       *pSrcA points to the first input vector
* @param[in]       *pSrcB points to the second input vector
* @param[out]      *pDst points to the output vector
* @param[in]       blockSize number of samples in each vector
* @return none.
*/
extern void dspal_vector_sub_q15(dspal_q15_t * pSrcA, dspal_q15_t * pSrcB, dspal_q15_t * pDst, uint32_t blockSize);


/**
* @brief Q31 vector subtraction.
* @param[in]       *pSrcA points to the first input vector
* @param[in]       *pSrcB points to the second input vector
* @param[out]      *pDst points to the output vector
* @param[in]       blockSize number of samples in each vector
* @return none.
*/
extern void dspal_vector_sub_q31(dspal_q31_t * pSrcA, dspal_q31_t * pSrcB, dspal_q31_t * pDst, uint32_t blockSize);


/**
* @brief Multiplies a floating-point vector by a scalar.
* @param[in]       *pSrc points to the input vector
* @param[in]       scale scale factor to be applied
* @param[out]      *pDst points to the output vector
* @param[in]       blockSize number of samples in the vector
* @return none.
*/
extern void dspal_vector_scale_f32(dspal_f32_t * pSrc, dspal_f32_t scale, dspal_f32_t * pDst, uint32_t blockSize);


/**
* @brief Multiplies a Q7 vector by a scalar.
* @param[in]       *pSrc points to the input vector
* @param[in]       scaleFract fractional portion of the scale value
* @param[in]       shift number of bits to shift the result by
* @param[out]      *pDst points to the output vector
* @param[in]       blockSize number of samples in the vector
* @return none.
*/
extern void dspal_vector_scale_q7(dspal_q7_t * pSrc, dspal_q7_t scaleFract, int8_t shift, dspal_q7_t * pDst, uint32_t blockSize);


/**
* @brief Multiplies a Q15 vector by a scalar.
* @param[in]       *pSrc points to the input vector
* @param[in]       scaleFract fractional portion of the scale value
* @param[in]       shift number of bits to shift the result by
* @param[out]      *pDst points to the output vector
* @param[in]       blockSize number of samples in the vector
* @return none.
*/
extern void dspal_vector_scale_q15(dspal_q15_t * pSrc, dspal_q15_t scaleFract, int8_t shift, dspal_q15_t * pDst, uint32_t blockSize);


/**
* @brief Multiplies a Q31 vector by a scalar.
* @param[in]       *pSrc points to the input vector
* @param[in]       scaleFract fractional portion of the scale value
* @param[in]       shift number of bits to shift the result by
* @param[out]      *pDst points to the output vector
* @param[in]       blockSize number of samples in the vector
* @return none.
*/
extern void dspal_vector_scale_q31(dspal_q31_t * pSrc, dspal_q31_t scaleFract, int8_t shift, dspal_q31_t * pDst, uint32_t blockSize);


/**
* @brief Q7 vector absolute value.
* @param[in]       *pSrc points to the input buffer
* @param[out]      *pDst points to the output buffer
* @param[in]       blockSize number of samples in each vector
* @return none.
*/
extern void dspal_vector_abs_q7(dspal_q7_t * pSrc, dspal_q7_t * pDst, uint32_t blockSize);


/**
* @brief Floating-point vector absolute value.
* @param[in]       *pSrc points to the input buffer
* @param[out]      *pDst points to the output buffer
* @param[in]       blockSize number of samples in each vector
* @return none.
*/
extern void dspal_vector_abs_f32(dspal_f32_t * pSrc, dspal_f32_t * pDst, uint32_t blockSize);


/**
* @brief Q15 vector absolute value.
* @param[in]       *pSrc points to the input buffer
* @param[out]      *pDst points to the output buffer
* @param[in]       blockSize number of samples in each vector
* @return none.
*/
extern void dspal_vector_abs_q15(dspal_q15_t * pSrc, dspal_q15_t * pDst, uint32_t blockSize);


/**
* @brief Q31 vector absolute value.
* @param[in]       *pSrc points to the input buffer
* @param[out]      *pDst points to the output buffer
* @param[in]       blockSize number of samples in each vector
* @return none.
*/
extern void dspal_vector_abs_q31(dspal_q31_t * pSrc, dspal_q31_t * pDst, uint32_t blockSize);


/**
* @brief Dot product of floating-point vectors.
* @param[in]       *pSrcA points to the first input vector
* @param[in]       *pSrcB points to the second input vector
* @param[in]       blockSize number of samples in each vector
* @param[out]      *result output result returned here
* @return none.
*/
extern void dspal_vector_dot_prod_f32(dspal_f32_t * pSrcA, dspal_f32_t * pSrcB, uint32_t blockSize, dspal_f32_t * result);


/**
* @brief Dot product of Q7 vectors.
* @param[in]       *pSrcA points to the first input vector
* @param[in]       *pSrcB points to the second input vector
* @param[in]       blockSize number of samples in each vector
* @param[out]      *result output result returned here
* @return none.
*/
extern void dspal_vector_dot_prod_q7(dspal_q7_t * pSrcA, dspal_q7_t * pSrcB, uint32_t blockSize, dspal_q31_t * result);


/**
* @brief Dot product of Q15 vectors.
* @param[in]       *pSrcA points to the first input vector
* @param[in]       *pSrcB points to the second input vector
* @param[in]       blockSize number of samples in each vector
* @param[out]      *result output result returned here
* @return none.
*/
extern void dspal_vector_dot_prod_q15(dspal_q15_t * pSrcA, dspal_q15_t * pSrcB, uint32_t blockSize, dspal_q63_t * result);


/**
* @brief Dot product of Q31 vectors.
* @param[in]       *pSrcA points to the first input vector
* @param[in]       *pSrcB points to the second input vector
* @param[in]       blockSize number of samples in each vector
* @param[out]      *result output result returned here
* @return none.
*/
extern void dspal_vector_dot_prod_q31(dspal_q31_t * pSrcA, dspal_q31_t * pSrcB, uint32_t blockSize, dspal_q63_t * result);


/**
* @brief  Shifts the elements of a Q7 vector a specified number of bits.
* @param[in]  *pSrc points to the input vector
* @param[in]  shiftBits number of bits to shift.  A positive value shifts left; a negative value shifts right.
* @param[out]  *pDst points to the output vector
* @param[in]  blockSize number of samples in the vector
* @return none.
*/
extern void dspal_vector_shift_q7(dspal_q7_t * pSrc, int8_t shiftBits, dspal_q7_t * pDst, uint32_t blockSize);


/**
* @brief  Shifts the elements of a Q15 vector a specified number of bits.
* @param[in]  *pSrc points to the input vector
* @param[in]  shiftBits number of bits to shift.  A positive value shifts left; a negative value shifts right.
* @param[out]  *pDst points to the output vector
* @param[in]  blockSize number of samples in the vector
* @return none.
*/
extern void dspal_vector_shift_q15(dspal_q15_t * pSrc, int8_t shiftBits, dspal_q15_t * pDst, uint32_t blockSize);


/**
* @brief  Shifts the elements of a Q31 vector a specified number of bits.
* @param[in]  *pSrc points to the input vector
* @param[in]  shiftBits number of bits to shift.  A positive value shifts left; a negative value shifts right.
* @param[out]  *pDst points to the output vector
* @param[in]  blockSize number of samples in the vector
* @return none.
*/
extern void dspal_vector_shift_q31(dspal_q31_t * pSrc, int8_t shiftBits, dspal_q31_t * pDst, uint32_t blockSize);


/**
* @brief  Adds a constant offset to a floating-point vector.
* @param[in]  *pSrc points to the input vector
* @param[in]  offset is the offset to be added
* @param[out]  *pDst points to the output vector
* @param[in]  blockSize number of samples in the vector
* @return none.
*/
extern void dspal_vector_offset_f32(dspal_f32_t * pSrc, dspal_f32_t offset, dspal_f32_t * pDst, uint32_t blockSize);


/**
* @brief  Adds a constant offset to a Q7 vector.
* @param[in]  *pSrc points to the input vector
* @param[in]  offset is the offset to be added
* @param[out]  *pDst points to the output vector
* @param[in]  blockSize number of samples in the vector
* @return none.
*/
extern void dspal_vector_offset_q7(dspal_q7_t * pSrc, dspal_q7_t offset, dspal_q7_t * pDst, uint32_t blockSize);


/**
* @brief  Adds a constant offset to a Q15 vector.
* @param[in]  *pSrc points to the input vector
* @param[in]  offset is the offset to be added
* @param[out]  *pDst points to the output vector
* @param[in]  blockSize number of samples in the vector
* @return none.
*/
extern void dspal_vector_offset_q15(dspal_q15_t * pSrc, dspal_q15_t offset, dspal_q15_t * pDst, uint32_t blockSize);


/**
* @brief  Adds a constant offset to a Q31 vector.
* @param[in]  *pSrc points to the input vector
* @param[in]  offset is the offset to be added
* @param[out]  *pDst points to the output vector
* @param[in]  blockSize number of samples in the vector
* @return none.
*/
extern void dspal_vector_offset_q31(dspal_q31_t * pSrc, dspal_q31_t offset, dspal_q31_t * pDst, uint32_t blockSize);


/**
* @brief  Negates the elements of a floating-point vector.
* @param[in]  *pSrc points to the input vector
* @param[out]  *pDst points to the output vector
* @param[in]  blockSize number of samples in the vector
* @return none.
*/
extern void dspal_vector_negate_f32(dspal_f32_t * pSrc, dspal_f32_t * pDst, uint32_t blockSize);


/**
* @brief  Negates the elements of a Q7 vector.
* @param[in]  *pSrc points to the input vector
* @param[out]  *pDst points to the output vector
* @param[in]  blockSize number of samples in the vector
* @return none.
*/
extern void dspal_vector_negate_q7(dspal_q7_t * pSrc, dspal_q7_t * pDst, uint32_t blockSize);


/**
* @brief  Negates the elements of a Q15 vector.
* @param[in]  *pSrc points to the input vector
* @param[out]  *pDst points to the output vector
* @param[in]  blockSize number of samples in the vector
* @return none.
*/
extern void dspal_vector_negate_q15(dspal_q15_t * pSrc, dspal_q15_t * pDst, uint32_t blockSize);


/**
* @brief  Negates the elements of a Q31 vector.
* @param[in]  *pSrc points to the input vector
* @param[out]  *pDst points to the output vector
* @param[in]  blockSize number of samples in the vector
* @return none.
*/
extern void dspal_vector_negate_q31(dspal_q31_t * pSrc, dspal_q31_t * pDst, uint32_t blockSize);

       
             
/* @}            
    end of group dspal_vector  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

