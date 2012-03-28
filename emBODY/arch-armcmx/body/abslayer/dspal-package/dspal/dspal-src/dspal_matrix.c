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

/* @file       dspal_matrix.c
	@brief      This file implements internal implementation of the dspal module
	@author     marco.accame@iit.it
    @date       12/16/2010
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"


#include "dspal_base_hid.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "dspal_matrix.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "dspal_matrix_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section




// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern dspal_result_t dspal_matrix_add_f32(const dspal_matrix_f32_t * pSrcA, const dspal_matrix_f32_t * pSrcB, dspal_matrix_f32_t * pDst)
{
    arm_status s = arm_mat_add_f32((CMATF32P)pSrcA, (CMATF32P)pSrcB, (MATF32P)pDst);
    
    return(FROM_CMSISSTATUS_TO_DSPALRESULT(s));
}



extern dspal_result_t dspal_matrix_add_q15(const dspal_matrix_q15_t * pSrcA, const dspal_matrix_q15_t * pSrcB, dspal_matrix_q15_t * pDst)
{
    arm_status s = arm_mat_add_q15((CMATQ15P)pSrcA, (CMATQ15P)pSrcB, (MATQ15P)pDst);

    return(FROM_CMSISSTATUS_TO_DSPALRESULT(s));
}


extern dspal_result_t dspal_matrix_add_q31(const dspal_matrix_q31_t * pSrcA, const dspal_matrix_q31_t * pSrcB, dspal_matrix_q31_t * pDst)
{
    arm_status s = arm_mat_add_q31((CMATQ31P)pSrcA, (CMATQ31P)pSrcB, (MATQ31P)pDst);

    return(FROM_CMSISSTATUS_TO_DSPALRESULT(s));
}



extern dspal_result_t dspal_matrix_trans_f32(const dspal_matrix_f32_t * pSrc, dspal_matrix_f32_t * pDst)
{
    arm_status s = arm_mat_trans_f32((CMATF32P)pSrc, (MATF32P)pDst);

    return(FROM_CMSISSTATUS_TO_DSPALRESULT(s));
}



extern dspal_result_t dspal_matrix_trans_q15(const dspal_matrix_q15_t * pSrc, dspal_matrix_q15_t * pDst)
{
    arm_status s = arm_mat_trans_q15((CMATQ15P)pSrc, (MATQ15P)pDst);

    return(FROM_CMSISSTATUS_TO_DSPALRESULT(s));
}



extern dspal_result_t dspal_matrix_trans_q31(const dspal_matrix_q31_t * pSrc, dspal_matrix_q31_t * pDst)
{
    arm_status s = arm_mat_trans_q31((CMATQ31P)pSrc, (MATQ31P)pDst);

    return(FROM_CMSISSTATUS_TO_DSPALRESULT(s));
}



extern dspal_result_t dspal_matrix_mult_f32(const dspal_matrix_f32_t * pSrcA, const dspal_matrix_f32_t * pSrcB, 
                                            dspal_matrix_f32_t * pDst)
{
    arm_status s = arm_mat_mult_f32((CMATF32P)pSrcA, (CMATF32P)pSrcB, (MATF32P)pDst);

    return(FROM_CMSISSTATUS_TO_DSPALRESULT(s));
}



extern dspal_result_t dspal_matrix_mult_q15(const dspal_matrix_q15_t * pSrcA, const dspal_matrix_q15_t * pSrcB, 
                                            dspal_matrix_q15_t * pDst, dspal_q15_t * pState)
{
    arm_status s = arm_mat_mult_q15((CMATQ15P)pSrcA, (CMATQ15P)pSrcB, (MATQ15P)pDst, pState);

    return(FROM_CMSISSTATUS_TO_DSPALRESULT(s));
}


extern dspal_result_t dspal_matrix_mult_fast_q15(const dspal_matrix_q15_t * pSrcA, const dspal_matrix_q15_t * pSrcB,
                                                 dspal_matrix_q15_t * pDst, dspal_q15_t * pState)
{
    arm_status s = arm_mat_mult_fast_q15((CMATQ15P)pSrcA, (CMATQ15P)pSrcB, (MATQ15P)pDst, pState);

    return(FROM_CMSISSTATUS_TO_DSPALRESULT(s));
}


extern dspal_result_t dspal_matrix_mult_q31(const dspal_matrix_q31_t * pSrcA, const dspal_matrix_q31_t * pSrcB, dspal_matrix_q31_t * pDst)
{
    arm_status s = arm_mat_mult_q31((CMATQ31P)pSrcA, (CMATQ31P)pSrcB, (MATQ31P)pDst);

    return(FROM_CMSISSTATUS_TO_DSPALRESULT(s));
}



extern dspal_result_t dspal_matrix_mult_fast_q31(const dspal_matrix_q31_t * pSrcA, const dspal_matrix_q31_t * pSrcB, dspal_matrix_q31_t * pDst)
{
    arm_status s = arm_mat_mult_fast_q31((CMATQ31P)pSrcA, (CMATQ31P)pSrcB, (MATQ31P)pDst);

    return(FROM_CMSISSTATUS_TO_DSPALRESULT(s));
}




extern dspal_result_t dspal_matrix_sub_f32(const dspal_matrix_f32_t * pSrcA, const dspal_matrix_f32_t * pSrcB, dspal_matrix_f32_t * pDst)
{
    arm_status s = arm_mat_sub_f32((CMATF32P)pSrcA, (CMATF32P)pSrcB, (MATF32P)pDst);

    return(FROM_CMSISSTATUS_TO_DSPALRESULT(s));
}



extern dspal_result_t dspal_matrix_sub_q15(const dspal_matrix_q15_t * pSrcA, const dspal_matrix_q15_t * pSrcB, dspal_matrix_q15_t * pDst)
{
    arm_status s = arm_mat_sub_q15((CMATQ15P)pSrcA, (CMATQ15P)pSrcB, (MATQ15P)pDst);

    return(FROM_CMSISSTATUS_TO_DSPALRESULT(s));
}



extern dspal_result_t dspal_matrix_sub_q31(const dspal_matrix_q31_t * pSrcA, const dspal_matrix_q31_t * pSrcB, dspal_matrix_q31_t * pDst)
{
    arm_status s = arm_mat_sub_q31((CMATQ31P)pSrcA, (CMATQ31P)pSrcB, (MATQ31P)pDst);

    return(FROM_CMSISSTATUS_TO_DSPALRESULT(s));
}



extern dspal_result_t dspal_matrix_scale_f32(const dspal_matrix_f32_t * pSrc, dspal_f32_t scale, dspal_matrix_f32_t * pDst)
{
    arm_status s = arm_mat_scale_f32((CMATF32P)pSrc, scale, (MATF32P)pDst);

    return(FROM_CMSISSTATUS_TO_DSPALRESULT(s));
}



extern dspal_result_t dspal_matrix_scale_q15(const dspal_matrix_q15_t * pSrc, dspal_q15_t scaleFract, int32_t shift, dspal_matrix_q15_t * pDst)
{
    arm_status s = arm_mat_scale_q15((CMATQ15P)pSrc, scaleFract, shift, (MATQ15P)pDst);

    return(FROM_CMSISSTATUS_TO_DSPALRESULT(s));
}



extern dspal_result_t dspal_matrix_scale_q31(const dspal_matrix_q31_t * pSrc, dspal_q31_t scaleFract, int32_t shift, dspal_matrix_q31_t * pDst)
{
    arm_status s = arm_mat_scale_q31((CMATQ31P)pSrc, scaleFract, shift, (MATQ31P)pDst);

    return(FROM_CMSISSTATUS_TO_DSPALRESULT(s));
}


extern dspal_result_t dspal_matrix_inverse_f32(const dspal_matrix_f32_t * src, dspal_matrix_f32_t * dst)
{
    arm_status s = arm_mat_inverse_f32((CMATF32P)src, (MATF32P)(dst));

    return(FROM_CMSISSTATUS_TO_DSPALRESULT(s));
}


extern void dspal_matrix_init_q31(dspal_matrix_q31_t * S, uint16_t nRows, uint16_t nColumns, dspal_q31_t *pData)
{
    arm_mat_init_q31((MATQ31P)S, nRows, nColumns, pData);
}



extern void dspal_matrix_init_q15(dspal_matrix_q15_t * S, uint16_t nRows, uint16_t nColumns, dspal_q15_t *pData)
{
    arm_mat_init_q15((MATQ15P)S, nRows, nColumns, pData);
}



extern void dspal_matrix_init_f32(dspal_matrix_f32_t * S, uint16_t nRows, uint16_t nColumns, dspal_f32_t *pData)
{
    arm_mat_init_f32((MATF32P)S, nRows, nColumns, pData);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern uint32_t dspal_matrix_hid_getsize(const dspal_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern dspal_result_t dspal_matrix_hid_setmem(const dspal_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
//    if(NULL == memory)
//    {
//        dspal_base_hid_error(dspal_fatalerror_missingmemory, "dspal_xxx_hid_setmem(): memory missing");
//        return(dspal_res_NOK_generic);
//    }
    return(dspal_res_OK);  
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




