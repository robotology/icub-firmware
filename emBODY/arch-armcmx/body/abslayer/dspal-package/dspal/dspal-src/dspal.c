
/* @file       dspal.c
	@brief      This file implements internal implementation of the dspal module
	@author     marco.accame@iit.it
    @date       12/16/2010
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#warning    defined CM3, MATRIX_CHECH, ROUNDING
#define ARM_MATH_CM3
#define ARM_MATH_MATRIX_CHECK
#define ARM_MATH_ROUNDING

#include "cmsisdsp.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "dspal.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------


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

#define RES         dspal_result_t
#define CMATF32P    const arm_matrix_instance_f32*
#define MATF32P     arm_matrix_instance_f32*
#define CMATQ31P    const arm_matrix_instance_q31*
#define MATQ31P     arm_matrix_instance_q31*
#define CMATQ15P    const arm_matrix_instance_q15*
#define MATQ15P     arm_matrix_instance_q15*

#define FROM_CMSISSTATUS_TO_DSPALRESULT(e)    ((ARM_MATH_SUCCESS==(e))?(dspal_res_OK):((dspal_result_t)(((int8_t)(e)-1))))


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_dspal_error(dspal_errorcode_t err_code, const char *err_msg);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static dspal_cfg_t s_dspal_cfg = {0};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern uint32_t dspal_base_memory_getsize(const dspal_cfg_t *cfg, uint32_t *size08aligned)
{
    if(NULL != size08aligned)
    {
        *size08aligned = 0;
    }
    return(0);
}

extern dspal_result_t dspal_base_initialise(const dspal_cfg_t *cfg, uint64_t *data08aligned)
{

    if(NULL == cfg)
    {
        s_dspal_error(dspal_error_missingconfiguration, "dspal: NULL cfg in dspal_initialise()");
        return(dspal_res_NOK_generic);    
    }
    
    memcpy(&s_dspal_cfg, cfg, sizeof(dspal_cfg_t));    
   

    if(dspal_dsplibtype_armcmsis != cfg->dsplibtype)
    {
        s_dspal_error(dspal_error_unsupportedbehaviour, "dspal: unsupported library");
        return(dspal_res_NOK_generic);
    }

    return(dspal_res_OK);
}

 

extern void dspal_statistics_min_q7(dspal_q7_t * pSrc, uint32_t blockSize, dspal_q7_t * result, uint32_t * index)
{
    arm_min_q7(pSrc, blockSize, result, index);
}

extern void dspal_statistics_min_q15(dspal_q15_t * pSrc, uint32_t blockSize, dspal_q15_t * pResult, uint32_t * pIndex)
{
    arm_min_q15(pSrc, blockSize, pResult, pIndex);
}


extern void dspal_statistics_min_q31(dspal_q31_t * pSrc, uint32_t blockSize, dspal_q31_t * pResult, uint32_t * pIndex)
{
    arm_min_q31(pSrc, blockSize, pResult, pIndex);
}



extern void dspal_statistics_min_f32(dspal_f32_t * pSrc, uint32_t blockSize, dspal_f32_t * pResult, uint32_t * pIndex)
{
    arm_min_f32(pSrc, blockSize, pResult, pIndex);
}


extern void dspal_statistics_max_q7(dspal_q7_t * pSrc, uint32_t blockSize, dspal_q7_t * pResult, uint32_t * pIndex)
{
    arm_max_q7(pSrc, blockSize, pResult, pIndex);
}



extern void dspal_statistics_max_q15(dspal_q15_t * pSrc, uint32_t blockSize, dspal_q15_t * pResult, uint32_t * pIndex)
{
    arm_max_q15(pSrc, blockSize, pResult, pIndex);
}



extern void dspal_statistics_max_q31(dspal_q31_t * pSrc, uint32_t blockSize, dspal_q31_t * pResult, uint32_t * pIndex)
{
    arm_max_q31(pSrc, blockSize, pResult, pIndex);
}



extern void dspal_statistics_max_f32(dspal_f32_t * pSrc, uint32_t blockSize, dspal_f32_t * pResult, uint32_t * pIndex)
{
    arm_max_f32(pSrc, blockSize, pResult, pIndex);
}



extern void dspal_statistics_power_q31(dspal_q31_t * pSrc, uint32_t blockSize, dspal_q63_t * pResult)
{
    arm_power_q31(pSrc, blockSize, pResult);
}



extern void dspal_statistics_power_f32(dspal_f32_t * pSrc, uint32_t blockSize, dspal_f32_t * pResult)
{
    arm_power_f32(pSrc, blockSize, pResult);
}



extern void dspal_statistics_power_q15(dspal_q15_t * pSrc, uint32_t blockSize, dspal_q63_t * pResult)
{
    arm_power_q15(pSrc, blockSize, pResult);
}


extern void dspal_statistics_power_q7(dspal_q7_t * pSrc, uint32_t blockSize, dspal_q31_t * pResult)
{
    arm_power_q7(pSrc, blockSize, pResult);
}



extern void dspal_statistics_mean_q7(dspal_q7_t * pSrc, uint32_t blockSize, dspal_q7_t * pResult)
{
    arm_mean_q7(pSrc, blockSize, pResult);
}


extern void dspal_statistics_mean_q15(dspal_q15_t * pSrc, uint32_t blockSize, dspal_q15_t * pResult)
{
    arm_mean_q15(pSrc, blockSize, pResult);
}


extern void dspal_statistics_mean_q31(dspal_q31_t * pSrc, uint32_t blockSize, dspal_q31_t * pResult)
{
    arm_mean_q31(pSrc, blockSize, pResult);
}


extern void dspal_statistics_mean_f32(dspal_f32_t * pSrc, uint32_t blockSize, dspal_f32_t * pResult)
{
    arm_mean_f32(pSrc, blockSize, pResult);
}



extern void dspal_statistics_var_f32(dspal_f32_t * pSrc, uint32_t blockSize, dspal_f32_t * pResult)
{
    arm_var_f32(pSrc, blockSize, pResult);
}



extern void dspal_statistics_var_q31(dspal_q31_t * pSrc, uint32_t blockSize, dspal_q63_t * pResult)
{
    arm_var_q31(pSrc, blockSize, pResult);
}



extern void dspal_statistics_var_q15(dspal_q15_t * pSrc, uint32_t blockSize, dspal_q31_t * pResult)
{
    arm_var_q15(pSrc, blockSize, pResult);
}


extern void dspal_statistics_rms_f32(dspal_f32_t * pSrc, uint32_t blockSize, dspal_f32_t * pResult)
{
    arm_rms_f32(pSrc, blockSize, pResult);
}



extern void dspal_statistics_rms_q31(dspal_q31_t * pSrc, uint32_t blockSize, dspal_q31_t * pResult)
{
    arm_rms_q31(pSrc, blockSize, pResult);
}


extern void dspal_statistics_rms_q15(dspal_q15_t * pSrc, uint32_t blockSize, dspal_q15_t * pResult)
{
    arm_rms_q15(pSrc, blockSize, pResult);
}



extern void dspal_statistics_std_f32(dspal_f32_t * pSrc, uint32_t blockSize, dspal_f32_t * pResult)
{
    arm_std_f32(pSrc, blockSize, pResult);
}



extern void dspal_statistics_std_q31(dspal_q31_t * pSrc, uint32_t blockSize, dspal_q31_t * pResult)
{
    arm_std_q31(pSrc, blockSize, pResult);
}



extern void dspal_statistics_std_q15(dspal_q15_t * pSrc, uint32_t blockSize, dspal_q15_t * pResult)
{
    arm_std_q15(pSrc, blockSize, pResult);
}



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



extern dspal_f32_t dspal_math_sin_f32(dspal_f32_t x)
{
    return(arm_sin_f32(x));
}



extern dspal_q31_t dspal_math_sin_q31(dspal_q31_t x)
{
    return(arm_sin_q31(x));
}



extern dspal_q15_t dspal_math_sin_q15(dspal_q15_t x)
{
    return(arm_sin_q15(x));
}


extern dspal_f32_t dspal_math_cos_f32(dspal_f32_t x)
{
    return(arm_cos_f32(x));
}



extern dspal_q31_t dspal_math_cos_q31(dspal_q31_t x)
{
    return(arm_cos_q31(x));
}



extern dspal_q15_t dspal_math_cos_q15(dspal_q15_t x)
{
    return(arm_cos_q15(x));
}




extern dspal_result_t dspal_math_sqrt_f32(dspal_f32_t in, dspal_f32_t *pOut)
{
    arm_status s = arm_sqrt_f32(in, pOut);

    return(FROM_CMSISSTATUS_TO_DSPALRESULT(s));
}



extern dspal_result_t dspal_math_sqrt_q31(dspal_q31_t in, dspal_q31_t *pOut)
{
    arm_status s = arm_sqrt_q31(in, pOut);

    return(FROM_CMSISSTATUS_TO_DSPALRESULT(s));
}


extern dspal_result_t dspal_math_sqrt_q15(dspal_q15_t in, dspal_q15_t *pOut)
{
    arm_status s = arm_sqrt_q15(in, pOut);

    return(FROM_CMSISSTATUS_TO_DSPALRESULT(s));
}


extern void dspal_math_sin_cos_f32(dspal_f32_t theta, dspal_f32_t *pSinVal, dspal_f32_t *pCcosVal)
{
    arm_sin_cos_f32(theta, pSinVal, pCcosVal);
}


extern void dspal_math_sin_cos_q31(dspal_q31_t theta, dspal_q31_t *pSinVal, dspal_q31_t *pCosVal)
{
    arm_sin_cos_q31(theta, pSinVal, pCosVal);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_dspal_error(dspal_errorcode_t err_code, const char *err_msg) 
{
    if(NULL != s_dspal_cfg.extfn.usr_on_fatal_error)
    {
        s_dspal_cfg.extfn.usr_on_fatal_error(err_code, err_msg);
    }

    for(;;);
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




