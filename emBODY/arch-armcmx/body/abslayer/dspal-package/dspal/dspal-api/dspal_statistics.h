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
#ifndef _DSPAL_STATISTICS_H_
#define _DSPAL_STATISTICS_H_


/** @file       dspal_statistics.h
	@brief      This header file implements public interface to the statistics part of the dsp abtraction layer 
	@author     marco.accame@iit.it
	@date       12/16/2010
**/

/* @defgroup dspal_statistics DSP abstraction layer
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

 

/** @fn         extern void dspal_statistics_min_q7(dspal_q7_t * pSrc, uint32_t blockSize, dspal_q7_t * result, uint32_t * index)
    @brief      Retrieves the inimum value of a q7 vector.
    @param      pSrc            the input pointer
    @param      blockSize       blockSize is the number of samples to process. 
    @param      result          the output pointer
    @param      index           the index of the minimum value in the input buffer 
    @return     On success: osal_res_OK, otherwise: osal_res_NOK_nullpointer.
 **/
extern void dspal_statistics_min_q7(dspal_q7_t * pSrc, uint32_t blockSize, dspal_q7_t * result, uint32_t * index);


/** @fn         extern void dspal_statistics_min_q15(dspal_q15_t * pSrc,uint32_t blockSize, dspal_q15_t * pResult, uint32_t * pIndex)
    @brief      Retrieves the inimum value of a q15 vector.
    @param      pSrc            the input pointer
    @param      blockSize       blockSize is the number of samples to process. 
    @param      result          the output pointer
    @param      index           the index of the minimum value in the input buffer 
    @return     On success: osal_res_OK, otherwise: osal_res_NOK_nullpointer.
 **/
extern void dspal_statistics_min_q15(dspal_q15_t * pSrc,uint32_t blockSize, dspal_q15_t * pResult, uint32_t * pIndex);


/** @fn         extern void dspal_statistics_min_q31(dspal_q31_t * pSrc,uint32_t blockSize, dspal_q15_t * pResult, uint32_t * pIndex)
    @brief      Retrieves the inimum value of a q31 vector.
    @param      pSrc            the input pointer
    @param      blockSize       blockSize is the number of samples to process. 
    @param      result          the output pointer
    @param      index           the index of the minimum value in the input buffer 
    @return     On success: osal_res_OK, otherwise: osal_res_NOK_nullpointer.
 **/

extern void dspal_statistics_min_q31(dspal_q31_t * pSrc, uint32_t blockSize, dspal_q31_t * pResult, uint32_t * pIndex);


/**
* @brief  Minimum value of a floating-point vector.
* @param[in]  *pSrc is input pointer
* @param[in]  blockSize is the number of samples to process
* @param[out]  *pResult is output pointer
* @param[out]  *pIndex is the array index of the minimum value in the input buffer.
* @return none.
*/
extern void dspal_statistics_min_f32(dspal_f32_t * pSrc, uint32_t blockSize, dspal_f32_t * pResult, uint32_t * pIndex);


/**
* @brief Maximum value of a Q7 vector.
* @param[in]       *pSrc points to the input buffer
* @param[in]       blockSize length of the input vector
* @param[out]      *pResult maximum value returned here
* @param[out]      *pIndex index of maximum value returned here
* @return none.
*/
extern void dspal_statistics_max_q7(dspal_q7_t * pSrc, uint32_t blockSize, dspal_q7_t * pResult, uint32_t * pIndex);


/**
* @brief Maximum value of a Q15 vector.
* @param[in]       *pSrc points to the input buffer
* @param[in]       blockSize length of the input vector
* @param[out]      *pResult maximum value returned here
* @param[out]      *pIndex index of maximum value returned here
* @return none.
*/
extern void dspal_statistics_max_q15(dspal_q15_t * pSrc, uint32_t blockSize, dspal_q15_t * pResult, uint32_t * pIndex);


/**
* @brief Maximum value of a Q31 vector.
* @param[in]       *pSrc points to the input buffer
* @param[in]       blockSize length of the input vector
* @param[out]      *pResult maximum value returned here
* @param[out]      *pIndex index of maximum value returned here
* @return none.
*/
extern void dspal_statistics_max_q31(dspal_q31_t * pSrc, uint32_t blockSize, dspal_q31_t * pResult, uint32_t * pIndex);


/**
* @brief Maximum value of a floating-point vector.
* @param[in]       *pSrc points to the input buffer
* @param[in]       blockSize length of the input vector
* @param[out]      *pResult maximum value returned here
* @param[out]      *pIndex index of maximum value returned here
* @return none.
*/
extern void dspal_statistics_max_f32(dspal_f32_t * pSrc, uint32_t blockSize, dspal_f32_t * pResult, uint32_t * pIndex);


/**
* @brief  Sum of the squares of the elements of a Q31 vector.
* @param[in]  *pSrc is input pointer
* @param[in]  blockSize is the number of samples to process
* @param[out]  *pResult is output value.
* @return none.
*/
extern void dspal_statistics_power_q31(dspal_q31_t * pSrc, uint32_t blockSize, dspal_q63_t * pResult);


/**
* @brief  Sum of the squares of the elements of a floating-point vector.
* @param[in]  *pSrc is input pointer
* @param[in]  blockSize is the number of samples to process
* @param[out]  *pResult is output value.
* @return none.
*/
extern void dspal_statistics_power_f32(dspal_f32_t * pSrc, uint32_t blockSize, dspal_f32_t * pResult);


/**
* @brief  Sum of the squares of the elements of a Q15 vector.
* @param[in]  *pSrc is input pointer
* @param[in]  blockSize is the number of samples to process
* @param[out]  *pResult is output value.
* @return none.
*/
extern void dspal_statistics_power_q15(dspal_q15_t * pSrc, uint32_t blockSize, dspal_q63_t * pResult);


/**
* @brief  Sum of the squares of the elements of a Q7 vector.
* @param[in]  *pSrc is input pointer
* @param[in]  blockSize is the number of samples to process
* @param[out]  *pResult is output value.
* @return none.
*/
extern void dspal_statistics_power_q7(dspal_q7_t * pSrc, uint32_t blockSize, dspal_q31_t * pResult);


/**
* @brief  Mean value of a Q7 vector.
* @param[in]  *pSrc is input pointer
* @param[in]  blockSize is the number of samples to process
* @param[out]  *pResult is output value.
* @return none.
*/
extern void dspal_statistics_mean_q7(dspal_q7_t * pSrc, uint32_t blockSize, dspal_q7_t * pResult);


/**
* @brief  Mean value of a Q15 vector.
* @param[in]  *pSrc is input pointer
* @param[in]  blockSize is the number of samples to process
* @param[out]  *pResult is output value.
* @return none.
*/
extern void dspal_statistics_mean_q15(dspal_q15_t * pSrc, uint32_t blockSize, dspal_q15_t * pResult);


/**
* @brief  Mean value of a Q31 vector.
* @param[in]  *pSrc is input pointer
* @param[in]  blockSize is the number of samples to process
* @param[out]  *pResult is output value.
* @return none.
*/
extern void dspal_statistics_mean_q31(dspal_q31_t * pSrc, uint32_t blockSize, dspal_q31_t * pResult);


/**
* @brief  Mean value of a floating-point vector.
* @param[in]  *pSrc is input pointer
* @param[in]  blockSize is the number of samples to process
* @param[out]  *pResult is output value.
* @return none.
*/
extern void dspal_statistics_mean_f32(dspal_f32_t * pSrc, uint32_t blockSize, dspal_f32_t * pResult);

/**
* @brief  Variance of the elements of a floating-point vector.
* @param[in]  *pSrc is input pointer
* @param[in]  blockSize is the number of samples to process
* @param[out]  *pResult is output value.
* @return none.
*/
extern void dspal_statistics_var_f32(dspal_f32_t * pSrc, uint32_t blockSize, dspal_f32_t * pResult);


/**
* @brief  Variance of the elements of a Q31 vector.
* @param[in]  *pSrc is input pointer
* @param[in]  blockSize is the number of samples to process
* @param[out]  *pResult is output value.
* @return none.
*/
extern void dspal_statistics_var_q31(dspal_q31_t * pSrc, uint32_t blockSize, dspal_q63_t * pResult);


/**
* @brief  Variance of the elements of a Q15 vector.
* @param[in]  *pSrc is input pointer
* @param[in]  blockSize is the number of samples to process
* @param[out]  *pResult is output value.
* @return none.
*/
extern void dspal_statistics_var_q15(dspal_q15_t * pSrc, uint32_t blockSize, dspal_q31_t * pResult);

/**
* @brief  Root Mean Square of the elements of a floating-point vector.
* @param[in]  *pSrc is input pointer
* @param[in]  blockSize is the number of samples to process
* @param[out]  *pResult is output value.
* @return none.
*/
extern void dspal_statistics_rms_f32(dspal_f32_t * pSrc, uint32_t blockSize, dspal_f32_t * pResult);


/**
* @brief  Root Mean Square of the elements of a Q31 vector.
* @param[in]  *pSrc is input pointer
* @param[in]  blockSize is the number of samples to process
* @param[out]  *pResult is output value.
* @return none.
*/
extern void dspal_statistics_rms_q31(dspal_q31_t * pSrc, uint32_t blockSize, dspal_q31_t * pResult);


/**
* @brief  Root Mean Square of the elements of a Q15 vector.
* @param[in]  *pSrc is input pointer
* @param[in]  blockSize is the number of samples to process
* @param[out]  *pResult is output value.
* @return none.
*/
extern void dspal_statistics_rms_q15(dspal_q15_t * pSrc, uint32_t blockSize, dspal_q15_t * pResult);


/**
* @brief  Standard deviation of the elements of a floating-point vector.
* @param[in]  *pSrc is input pointer
* @param[in]  blockSize is the number of samples to process
* @param[out]  *pResult is output value.
* @return none.
*/
extern void dspal_statistics_std_f32(dspal_f32_t * pSrc, uint32_t blockSize, dspal_f32_t * pResult);


/**
* @brief  Standard deviation of the elements of a Q31 vector.
* @param[in]  *pSrc is input pointer
* @param[in]  blockSize is the number of samples to process
* @param[out]  *pResult is output value.
* @return none.
*/
extern void dspal_statistics_std_q31(dspal_q31_t * pSrc, uint32_t blockSize, dspal_q31_t * pResult);


/**
* @brief  Standard deviation of the elements of a Q15 vector.
* @param[in]  *pSrc is input pointer
* @param[in]  blockSize is the number of samples to process
* @param[out]  *pResult is output value.
* @return none.
*/
extern void dspal_statistics_std_q15(dspal_q15_t * pSrc, uint32_t blockSize, dspal_q15_t * pResult);

       
             
/* @}            
    end of group dspal_statistics  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

