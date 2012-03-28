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
#ifndef _DSPAL_MATRIX_H_
#define _DSPAL_MATRIX_H_


/** @file       dspal_matrix.h
	@brief      This header file implements the public interface to the matrix part of the dsp abtraction layer 
	@author     marco.accame@iit.it
	@date       12/16/2010
**/

/* @defgroup dspal_matrix DSP abstraction layer
    cercecvecve.
    
    @{		
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "dspal_base.h"




// - public #define  --------------------------------------------------------------------------------------------------

 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

/**
* @brief Instance structure for the floating-point matrix structure.
*/
typedef struct
{
    uint16_t    numrows;            /**< number of rows of the matrix.     */
    uint16_t    numcols;            /**< number of columns of the matrix.  */
    dspal_f32_t *pdata;             /**< points to the data of the matrix. */
} dspal_matrix_f32_t;


/**
* @brief Instance structure for the Q15 matrix structure.
*/
typedef struct
{
    uint16_t    numrows;            /**< number of rows of the matrix.     */
    uint16_t    numcols;            /**< number of columns of the matrix.  */
    dspal_q15_t *pdata;             /**< points to the data of the matrix. */
} dspal_matrix_q15_t;


/**
* @brief Instance structure for the Q31 matrix structure.
*/
typedef struct
{
    uint16_t    numrows;            /**< number of rows of the matrix.     */
    uint16_t    numcols;            /**< number of columns of the matrix.  */
    dspal_q31_t *pdata;             /**< points to the data of the matrix. */
} dspal_matrix_q31_t;
                    
    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section (so far)


// - declaration of extern public functions ---------------------------------------------------------------------------


/**
* @brief Floating-point matrix addition.
* @param[in]       *pSrcA points to the first input matrix structure
* @param[in]       *pSrcB points to the second input matrix structure
* @param[out]      *pDst points to output matrix structure
* @return     The function returns either
* <code>dspal_res_NOK_size_mismatch</code> or <code>dspal_res_OK based on the outcome of size checking.
*/
extern dspal_result_t dspal_matrix_add_f32(const dspal_matrix_f32_t * pSrcA, const dspal_matrix_f32_t * pSrcB, dspal_matrix_f32_t * pDst);


/**
* @brief Q15 matrix addition.
* @param[in]       *pSrcA points to the first input matrix structure
* @param[in]       *pSrcB points to the second input matrix structure
* @param[out]      *pDst points to output matrix structure
* @return     The function returns either
* <code>dspal_res_NOK_size_mismatch</code> or <code>dspal_res_OK based on the outcome of size checking.
*/
extern dspal_result_t dspal_matrix_add_q15(const dspal_matrix_q15_t * pSrcA, const dspal_matrix_q15_t * pSrcB, dspal_matrix_q15_t * pDst);


/**
* @brief Q31 matrix addition.
* @param[in]       *pSrcA points to the first input matrix structure
* @param[in]       *pSrcB points to the second input matrix structure
* @param[out]      *pDst points to output matrix structure
* @return     The function returns either
* <code>dspal_res_NOK_size_mismatch</code> or <code>dspal_res_OK based on the outcome of size checking.
*/
extern dspal_result_t dspal_matrix_add_q31(const dspal_matrix_q31_t * pSrcA, const dspal_matrix_q31_t * pSrcB, dspal_matrix_q31_t * pDst);


/**
* @brief Floating-point matrix transpose.
* @param[in]  *pSrc points to the input matrix
* @param[out] *pDst points to the output matrix
* @return 	The function returns either  <code>dspal_res_NOK_size_mismatch</code>
* or <code>dspal_res_OK</code> based on the outcome of size checking.
*/
extern dspal_result_t dspal_matrix_trans_f32(const dspal_matrix_f32_t * pSrc, dspal_matrix_f32_t * pDst);


/**
* @brief Floating-point matrix inverse.
* @param[in]  *src points to the input matrix
* @param[out] *dst points to the output matrix
* @return 	The function returns either  <code>dspal_res_NOK_size_mismatch</code>
* or <code>dspal_res_OK</code> based on the outcome of size checking.
*/
extern dspal_result_t dspal_matrix_inverse_f32(const dspal_matrix_f32_t * src, dspal_matrix_f32_t * dst);


/**
* @brief Q15 matrix transpose.
* @param[in]  *pSrc points to the input matrix
* @param[out] *pDst points to the output matrix
* @return 	The function returns either  <code>dspal_res_NOK_size_mismatch</code>
* or <code>dspal_res_OK</code> based on the outcome of size checking.
*/
extern dspal_result_t dspal_matrix_trans_q15(const dspal_matrix_q15_t * pSrc, dspal_matrix_q15_t * pDst);


/**
* @brief Q31 matrix transpose.
* @param[in]  *pSrc points to the input matrix
* @param[out] *pDst points to the output matrix
* @return 	The function returns either  <code>dspal_res_NOK_size_mismatch</code>
* or <code>dspal_res_OK</code> based on the outcome of size checking.
*/
extern dspal_result_t dspal_matrix_trans_q31(const dspal_matrix_q31_t * pSrc, dspal_matrix_q31_t * pDst);


/**
* @brief Floating-point matrix multiplication
* @param[in]       *pSrcA points to the first input matrix structure
* @param[in]       *pSrcB points to the second input matrix structure
* @param[out]      *pDst points to output matrix structure
* @return     The function returns either
* <code>dspal_res_NOK_size_mismatch</code> or <code>dspal_res_OK based on the outcome of size checking.
*/
extern dspal_result_t dspal_matrix_mult_f32(const dspal_matrix_f32_t * pSrcA, const dspal_matrix_f32_t * pSrcB, 
                                            dspal_matrix_f32_t * pDst);

/**
* @brief Q15 matrix multiplication
* @param[in]       *pSrcA points to the first input matrix structure
* @param[in]       *pSrcB points to the second input matrix structure
* @param[out]      *pDst points to output matrix structure
* @return     The function returns either
* <code>dspal_res_NOK_size_mismatch</code> or <code>dspal_res_OK based on the outcome of size checking.
*/
extern dspal_result_t dspal_matrix_mult_q15(const dspal_matrix_q15_t * pSrcA, const dspal_matrix_q15_t * pSrcB, 
                                            dspal_matrix_q15_t * pDst, dspal_q15_t * pState);

/**
* @brief Q15 matrix multiplication (fast variant)
* @param[in]       *pSrcA points to the first input matrix structure
* @param[in]       *pSrcB points to the second input matrix structure
* @param[out]      *pDst points to output matrix structure
* @return     The function returns either
* <code>dspal_res_NOK_size_mismatch</code> or <code>dspal_res_OK based on the outcome of size checking.
*/
extern dspal_result_t dspal_matrix_mult_fast_q15(const dspal_matrix_q15_t * pSrcA, const dspal_matrix_q15_t * pSrcB,
                                                 dspal_matrix_q15_t * pDst, dspal_q15_t * pState);

/**
* @brief Q31 matrix multiplication
* @param[in]       *pSrcA points to the first input matrix structure
* @param[in]       *pSrcB points to the second input matrix structure
* @param[out]      *pDst points to output matrix structure
* @return     The function returns either
* <code>dspal_res_NOK_size_mismatch</code> or <code>dspal_res_OK based on the outcome of size checking.
*/
extern dspal_result_t dspal_matrix_mult_q31(const dspal_matrix_q31_t * pSrcA, const dspal_matrix_q31_t * pSrcB, dspal_matrix_q31_t * pDst);


/**
* @brief Q31 matrix multiplication (fast variant)
* @param[in]       *pSrcA points to the first input matrix structure
* @param[in]       *pSrcB points to the second input matrix structure
* @param[out]      *pDst points to output matrix structure
* @return     The function returns either
* <code>dspal_res_NOK_size_mismatch</code> or <code>dspal_res_OK based on the outcome of size checking.
*/
extern dspal_result_t dspal_matrix_mult_fast_q31(const dspal_matrix_q31_t * pSrcA, const dspal_matrix_q31_t * pSrcB, dspal_matrix_q31_t * pDst);


/**
* @brief Floating-point matrix subtraction
* @param[in]       *pSrcA points to the first input matrix structure
* @param[in]       *pSrcB points to the second input matrix structure
* @param[out]      *pDst points to output matrix structure
* @return     The function returns either
* <code>dspal_res_NOK_size_mismatch</code> or <code>dspal_res_OK based on the outcome of size checking.
*/
extern dspal_result_t dspal_matrix_sub_f32(const dspal_matrix_f32_t * pSrcA, const dspal_matrix_f32_t * pSrcB, dspal_matrix_f32_t * pDst);


/**
* @brief Q15 matrix subtraction
* @param[in]       *pSrcA points to the first input matrix structure
* @param[in]       *pSrcB points to the second input matrix structure
* @param[out]      *pDst points to output matrix structure
* @return     The function returns either
* <code>dspal_res_NOK_size_mismatch</code> or <code>dspal_res_OK based on the outcome of size checking.
*/
extern dspal_result_t dspal_matrix_sub_q15(const dspal_matrix_q15_t * pSrcA, const dspal_matrix_q15_t * pSrcB, dspal_matrix_q15_t * pDst);


/**
* @brief Q31 matrix subtraction
* @param[in]       *pSrcA points to the first input matrix structure
* @param[in]       *pSrcB points to the second input matrix structure
* @param[out]      *pDst points to output matrix structure
* @return     The function returns either
* <code>dspal_res_NOK_size_mismatch</code> or <code>dspal_res_OK based on the outcome of size checking.
*/
extern dspal_result_t dspal_matrix_sub_q31(const dspal_matrix_q31_t * pSrcA, const dspal_matrix_q31_t * pSrcB, dspal_matrix_q31_t * pDst);


/**
* @brief Floating-point matrix scaling.
* @param[in]  *pSrc points to the input matrix
* @param[in]  scale scale factor
* @param[out] *pDst points to the output matrix
* @return     The function returns either
* <code>dspal_res_NOK_size_mismatch</code> or <code>dspal_res_OK based on the outcome of size checking.
*/
extern dspal_result_t dspal_matrix_scale_f32(const dspal_matrix_f32_t * pSrc, dspal_f32_t scale, dspal_matrix_f32_t * pDst);


/**
* @brief Q15 matrix scaling.
* @param[in]       *pSrc points to input matrix
* @param[in]       scaleFract fractional portion of the scale factor
* @param[in]       shift number of bits to shift the result by
* @param[out]      *pDst points to output matrix
* @return     The function returns either
* <code>dspal_res_NOK_size_mismatch</code> or <code>dspal_res_OK based on the outcome of size checking.
*/
extern dspal_result_t dspal_matrix_scale_q15(const dspal_matrix_q15_t * pSrc, dspal_q15_t scaleFract, int32_t shift, dspal_matrix_q15_t * pDst);


/**
* @brief Q31 matrix scaling.
* @param[in]       *pSrc points to input matrix
* @param[in]       scaleFract fractional portion of the scale factor
* @param[in]       shift number of bits to shift the result by
* @param[out]      *pDst points to output matrix structure
* @return     The function returns either
* <code>dspal_res_NOK_size_mismatch</code> or <code>dspal_res_OK based on the outcome of size checking.
*/
extern dspal_result_t dspal_matrix_scale_q31(const dspal_matrix_q31_t * pSrc, dspal_q31_t scaleFract, int32_t shift, dspal_matrix_q31_t * pDst);


/**
* @brief  Q31 matrix initialization.
* @param[in,out] *S             points to an instance of the floating-point matrix structure.
* @param[in]     nRows          number of rows in the matrix.
* @param[in]     nColumns       number of columns in the matrix.
* @param[in]     *pData	       points to the matrix data array.
* @return        none
*/
extern void dspal_matrix_init_q31(dspal_matrix_q31_t * S, uint16_t nRows, uint16_t nColumns, dspal_q31_t   *pData);


/**
* @brief  Q15 matrix initialization.
* @param[in,out] *S             points to an instance of the floating-point matrix structure.
* @param[in]     nRows          number of rows in the matrix.
* @param[in]     nColumns       number of columns in the matrix.
* @param[in]     *pData	       points to the matrix data array.
* @return        none
*/
extern void dspal_matrix_init_q15(dspal_matrix_q15_t * S, uint16_t nRows, uint16_t nColumns, dspal_q15_t    *pData);


/**
* @brief  Floating-point matrix initialization.
* @param[in,out] *S             points to an instance of the floating-point matrix structure.
* @param[in]     nRows          number of rows in the matrix.
* @param[in]     nColumns       number of columns in the matrix.
* @param[in]     *pData	       points to the matrix data array.
* @return        none
*/
extern void dspal_matrix_init_f32(dspal_matrix_f32_t * S, uint16_t nRows, uint16_t nColumns, dspal_f32_t   *pData);

    
             
/* @}            
    end of group dspal_matrix  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

