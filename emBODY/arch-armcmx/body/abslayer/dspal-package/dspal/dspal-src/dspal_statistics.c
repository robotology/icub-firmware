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

/* @file       dspal_statistics.c
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

#include "dspal_statistics.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "dspal_statistics_hid.h"

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



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern uint32_t dspal_statistics_hid_getsize(const dspal_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern dspal_result_t dspal_statistics_hid_setmem(const dspal_cfg_t *cfg, uint32_t *memory)
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




