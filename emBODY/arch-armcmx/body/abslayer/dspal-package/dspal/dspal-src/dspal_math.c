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

/* @file       dspal_math.c
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

#include "dspal_math.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "dspal_math_hid.h"

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

extern uint32_t dspal_math_hid_getsize(const dspal_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern dspal_result_t dspal_math_hid_setmem(const dspal_cfg_t *cfg, uint32_t *memory)
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




