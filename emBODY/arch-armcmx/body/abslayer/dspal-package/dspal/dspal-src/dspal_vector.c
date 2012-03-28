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

/* @file       dspal_vector.c
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

#include "dspal_vector.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "dspal_vector_hid.h"

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


extern void dspal_vector_convert_f32_to_q31(dspal_f32_t * pSrc, dspal_q31_t * pDst, uint32_t blockSize)
{
    arm_float_to_q31(pSrc, pDst, blockSize);
}




extern void dspal_vector_convert_f32_to_q15(dspal_f32_t * pSrc, dspal_q15_t * pDst, uint32_t blockSize)
{
    arm_float_to_q15(pSrc, pDst, blockSize);
}




extern void dspal_vector_convert_f32_to_q7(dspal_f32_t * pSrc, dspal_q7_t * pDst, uint32_t blockSize)
{
    arm_float_to_q7(pSrc, pDst, blockSize);
}





extern void dspal_vector_convert_dspal_q31_to_q15(dspal_q31_t * pSrc, dspal_q15_t * pDst, uint32_t blockSize)
{
    arm_q31_to_q15(pSrc, pDst, blockSize);
}




extern void dspal_vector_convert_dspal_q31_to_q7(dspal_q31_t * pSrc, dspal_q7_t * pDst, uint32_t blockSize)
{
    arm_q31_to_q7(pSrc, pDst, blockSize);
}


         

extern void dspal_vector_convert_dspal_q31_to_f32(dspal_q31_t * pSrc, dspal_f32_t * pDst, uint32_t blockSize)
{
    arm_q31_to_float(pSrc, pDst, blockSize);
}


        
 
extern void dspal_vector_convert_dspal_q15_to_f32(dspal_q15_t * pSrc, dspal_f32_t * pDst, uint32_t blockSize)
{
    arm_q15_to_float(pSrc, pDst, blockSize);
}




extern void dspal_vector_convert_dspal_q15_to_q31(dspal_q15_t * pSrc, dspal_q31_t * pDst, uint32_t blockSize)
{
    arm_q15_to_q31(pSrc, pDst, blockSize);
}




extern void dspal_vector_convert_dspal_q15_to_q7(dspal_q15_t * pSrc, dspal_q7_t * pDst, uint32_t blockSize)
{
    arm_q15_to_q7(pSrc, pDst, blockSize);
}



extern void dspal_vector_convert_q7_to_q31(dspal_q7_t * pSrc, dspal_q31_t * pDst, uint32_t blockSize)
{
    arm_q7_to_q31(pSrc, pDst, blockSize);
}




extern void dspal_vector_convert_q7_to_q15(dspal_q7_t * pSrc, dspal_q15_t * pDst, uint32_t blockSize)
{
    arm_q7_to_q15(pSrc, pDst, blockSize);
}


extern void dspal_vector_convert_q7_to_f32(dspal_q7_t * pSrc, dspal_f32_t * pDst, uint32_t blockSize)
{
    arm_q7_to_float(pSrc, pDst, blockSize);
}


extern void dspal_vector_copy_f32(dspal_f32_t * pSrc, dspal_f32_t * pDst, uint32_t blockSize)
{
    arm_copy_f32(pSrc, pDst, blockSize);
}


extern void dspal_vector_copy_q7(q7_t * pSrc, q7_t * pDst, uint32_t blockSize)
{
    arm_copy_q7(pSrc, pDst, blockSize);
}



extern void dspal_vector_copy_q15(q15_t * pSrc, q15_t * pDst, uint32_t blockSize)
{
    arm_copy_q15(pSrc, pDst, blockSize);
}



extern void dspal_vector_copy_q31(q31_t * pSrc, q31_t * pDst, uint32_t blockSize)
{
    arm_copy_q31(pSrc, pDst, blockSize);
}



extern void dspal_vector_fill_f32(dspal_f32_t value, dspal_f32_t * pDst, uint32_t blockSize)
{
    arm_fill_f32(value, pDst, blockSize);
}



extern void dspal_vector_fill_q7(dspal_q7_t value, dspal_q7_t * pDst, uint32_t blockSize)
{
    arm_fill_q7(value, pDst, blockSize);
}



extern void dspal_vector_fill_q15(dspal_q15_t value, dspal_q15_t * pDst, uint32_t blockSize)
{
    arm_fill_q15(value, pDst, blockSize);
}



extern void dspal_vector_fill_q31(dspal_q31_t value, dspal_q31_t * pDst, uint32_t blockSize)
{
    arm_fill_q31(value, pDst, blockSize);
}


extern void dspal_vector_add_f32(dspal_f32_t * pSrcA, dspal_f32_t * pSrcB, dspal_f32_t * pDst, uint32_t blockSize)
{
    arm_add_f32(pSrcA, pSrcB, pDst, blockSize);
}


extern void dspal_vector_add_q7(dspal_q7_t * pSrcA, dspal_q7_t * pSrcB, dspal_q7_t * pDst, uint32_t blockSize)
{
    arm_add_q7(pSrcA, pSrcB, pDst, blockSize);
}


extern void dspal_vector_add_q15(dspal_q15_t * pSrcA, dspal_q15_t * pSrcB, dspal_q15_t * pDst, uint32_t blockSize)
{
    arm_add_q15(pSrcA, pSrcB, pDst, blockSize);
}



extern void dspal_vector_add_q31(dspal_q31_t * pSrcA, dspal_q31_t * pSrcB, dspal_q31_t * pDst, uint32_t blockSize)
{
    arm_add_q31(pSrcA, pSrcB, pDst, blockSize);
}



extern void dspal_vector_sub_f32(dspal_f32_t * pSrcA, dspal_f32_t * pSrcB, dspal_f32_t * pDst, uint32_t blockSize)
{
    arm_sub_f32(pSrcA, pSrcB, pDst, blockSize);
}



extern void dspal_vector_sub_q7(dspal_q7_t * pSrcA, dspal_q7_t * pSrcB, dspal_q7_t * pDst, uint32_t blockSize)
{
    arm_sub_q7(pSrcA, pSrcB, pDst, blockSize);
}



extern void dspal_vector_sub_q15(dspal_q15_t * pSrcA, dspal_q15_t * pSrcB, dspal_q15_t * pDst, uint32_t blockSize)
{
    arm_sub_q15(pSrcA, pSrcB, pDst, blockSize);
}



extern void dspal_vector_sub_q31(dspal_q31_t * pSrcA, dspal_q31_t * pSrcB, dspal_q31_t * pDst, uint32_t blockSize)
{
    arm_sub_q31(pSrcA, pSrcB, pDst, blockSize);
}


extern void dspal_vector_scale_f32(dspal_f32_t * pSrc, dspal_f32_t scale, dspal_f32_t * pDst, uint32_t blockSize)
{
    arm_scale_f32(pSrc, scale, pDst, blockSize);
}



extern void dspal_vector_scale_q7(dspal_q7_t * pSrc, dspal_q7_t scaleFract, int8_t shift, dspal_q7_t * pDst, uint32_t blockSize)
{
    arm_scale_q7(pSrc, scaleFract, shift, pDst, blockSize);
}



extern void dspal_vector_scale_q15(dspal_q15_t * pSrc, dspal_q15_t scaleFract, int8_t shift, dspal_q15_t * pDst, uint32_t blockSize)
{
    arm_scale_q15(pSrc, scaleFract, shift, pDst, blockSize);
}


extern void dspal_vector_scale_q31(dspal_q31_t * pSrc, dspal_q31_t scaleFract, int8_t shift, dspal_q31_t * pDst, uint32_t blockSize)
{
    arm_scale_q31(pSrc, scaleFract, shift, pDst, blockSize);
}


extern void dspal_vector_abs_q7(dspal_q7_t * pSrc, dspal_q7_t * pDst, uint32_t blockSize)
{
    arm_abs_q7(pSrc, pDst, blockSize);
}


extern void dspal_vector_abs_f32(dspal_f32_t * pSrc, dspal_f32_t * pDst, uint32_t blockSize)
{
    arm_abs_f32(pSrc, pDst, blockSize);
}


extern void dspal_vector_abs_q15(dspal_q15_t * pSrc, dspal_q15_t * pDst, uint32_t blockSize)
{
    arm_abs_q15(pSrc, pDst, blockSize);
}


extern void dspal_vector_abs_q31(dspal_q31_t * pSrc, dspal_q31_t * pDst, uint32_t blockSize)
{
    arm_abs_q31(pSrc, pDst, blockSize);
}



extern void dspal_vector_dot_prod_f32(dspal_f32_t * pSrcA, dspal_f32_t * pSrcB, uint32_t blockSize, dspal_f32_t * result)
{
    arm_dot_prod_f32(pSrcA, pSrcB, blockSize, result);
}


extern void dspal_vector_dot_prod_q7(dspal_q7_t * pSrcA, dspal_q7_t * pSrcB, uint32_t blockSize, dspal_q31_t * result)
{
    arm_dot_prod_q7(pSrcA, pSrcB, blockSize, result);
}


extern void dspal_vector_dot_prod_q15(dspal_q15_t * pSrcA, dspal_q15_t * pSrcB, uint32_t blockSize, dspal_q63_t * result)
{
    arm_dot_prod_q15(pSrcA, pSrcB, blockSize, result);
}


extern void dspal_vector_dot_prod_q31(dspal_q31_t * pSrcA, dspal_q31_t * pSrcB, uint32_t blockSize, dspal_q63_t * result)
{
    arm_dot_prod_q31(pSrcA, pSrcB, blockSize, result);
}


extern void dspal_vector_shift_q7(dspal_q7_t * pSrc, int8_t shiftBits, dspal_q7_t * pDst, uint32_t blockSize)
{
    arm_shift_q7(pSrc, shiftBits, pDst, blockSize);
}


extern void dspal_vector_shift_q15(dspal_q15_t * pSrc, int8_t shiftBits, dspal_q15_t * pDst, uint32_t blockSize)
{
    arm_shift_q15(pSrc, shiftBits, pDst, blockSize);
}


extern void dspal_vector_shift_q31(dspal_q31_t * pSrc, int8_t shiftBits, dspal_q31_t * pDst, uint32_t blockSize)
{
    arm_shift_q31(pSrc, shiftBits, pDst, blockSize);
}



extern void dspal_vector_offset_f32(dspal_f32_t * pSrc, dspal_f32_t offset, dspal_f32_t * pDst, uint32_t blockSize)
{
    arm_offset_f32(pSrc, offset, pDst, blockSize);
}


extern void dspal_vector_offset_q7(dspal_q7_t * pSrc, dspal_q7_t offset, dspal_q7_t * pDst, uint32_t blockSize)
{
    arm_offset_q7(pSrc, offset, pDst, blockSize);
}


extern void dspal_vector_offset_q15(dspal_q15_t * pSrc, dspal_q15_t offset, dspal_q15_t * pDst, uint32_t blockSize)
{
    arm_offset_q15(pSrc, offset, pDst, blockSize);
}


extern void dspal_vector_offset_q31(dspal_q31_t * pSrc, dspal_q31_t offset, dspal_q31_t * pDst, uint32_t blockSize)
{
    arm_offset_q31(pSrc, offset, pDst, blockSize);
}



extern void dspal_vector_negate_f32(dspal_f32_t * pSrc, dspal_f32_t * pDst, uint32_t blockSize)
{
    arm_negate_f32(pSrc, pDst, blockSize);
}


extern void dspal_vector_negate_q7(dspal_q7_t * pSrc, dspal_q7_t * pDst, uint32_t blockSize)
{
    arm_negate_q7(pSrc, pDst, blockSize);
}


extern void dspal_vector_negate_q15(dspal_q15_t * pSrc, dspal_q15_t * pDst, uint32_t blockSize)
{
    arm_negate_q15(pSrc, pDst, blockSize);
}


extern void dspal_vector_negate_q31(dspal_q31_t * pSrc, dspal_q31_t * pDst, uint32_t blockSize)
{
    arm_negate_q31(pSrc, pDst, blockSize);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern uint32_t dspal_vector_hid_getsize(const dspal_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern dspal_result_t dspal_vector_hid_setmem(const dspal_cfg_t *cfg, uint32_t *memory)
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




