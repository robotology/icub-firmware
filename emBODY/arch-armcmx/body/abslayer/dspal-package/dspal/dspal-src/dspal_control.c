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

/* @file       dspal_control.c
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

#include "dspal_control.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "dspal_control_hid.h"

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


extern void dspal_control_pid_init_f32(dspal_control_pid_f32_t * S, int32_t resetStateFlag)
{
    arm_pid_f32((arm_pid_instance_f32*)S, resetStateFlag);
}


extern void dspal_control_pid_reset_f32(dspal_control_pid_f32_t * S)
{
    arm_pid_reset_f32((arm_pid_instance_f32*)S);
}


extern void dspal_control_pid_init_q31(dspal_control_pid_q31_t * S, int32_t resetStateFlag)
{
    arm_pid_q31((arm_pid_instance_q31*)S, resetStateFlag);
}


extern void dspal_control_pid_reset_q31(dspal_control_pid_q31_t * S)
{
    arm_pid_reset_q31((arm_pid_instance_q31*)S);
}


extern void dspal_control_pid_init_q15(dspal_control_pid_q15_t * S, int32_t resetStateFlag)
{
    arm_pid_q15((arm_pid_instance_q15*)S, resetStateFlag);
}


extern void dspal_control_pid_reset_q15(dspal_control_pid_q15_t * S)
{
    arm_pid_reset_q15((arm_pid_instance_q15*)S);
}


extern dspal_f32_t dspal_control_pid_f32(dspal_control_pid_f32_t * S, dspal_f32_t in)
{
    return(arm_pid_f32((arm_pid_instance_f32*)S, in));
}


extern dspal_q31_t dspal_control_pid_q31(dspal_control_pid_q31_t * S, dspal_q31_t in)
{
    return(arm_pid_q31((arm_pid_instance_q31*)S, in));
}


extern dspal_q15_t dspal_control_pid_q15(dspal_control_pid_q15_t * S, dspal_q15_t in)
{
    return(arm_pid_q15((arm_pid_instance_q15*)S, in));
}


extern void dspal_control_clarke_f32(dspal_f32_t Ia, dspal_f32_t Ib, dspal_f32_t * pIalpha, dspal_f32_t * pIbeta)
{
    arm_clarke_f32(Ia, Ib, pIalpha, pIbeta);
}


extern void dspal_control_clarke_q31(dspal_q31_t Ia, dspal_q31_t Ib, dspal_q31_t * pIalpha, dspal_q31_t * pIbeta)
{
    arm_clarke_q31(Ia, Ib, pIalpha, pIbeta);
}


extern void dspal_control_clarke_inv_f32(dspal_f32_t Ialpha, dspal_f32_t Ibeta, dspal_f32_t * pIa, dspal_f32_t * pIb)
{
    arm_inv_clarke_f32(Ialpha, Ibeta, pIa, pIb);
}


extern void dspal_control_clarke_inv_q31(dspal_q31_t Ialpha, dspal_q31_t Ibeta, dspal_q31_t * pIa, dspal_q31_t * pIb)
{
    arm_inv_clarke_q31(Ialpha, Ibeta, pIa, pIb);
}


extern void dspal_control_park_f32(dspal_f32_t Ialpha, dspal_f32_t Ibeta, dspal_f32_t * pId, dspal_f32_t * pIq, dspal_f32_t sinVal, dspal_f32_t cosVal)
{
    arm_park_f32(Ialpha, Ibeta, pId, pIq, sinVal, cosVal);
}


extern void dspal_control_park_q31(dspal_q31_t Ialpha, dspal_q31_t Ibeta, dspal_q31_t * pId, dspal_q31_t * pIq, dspal_q31_t sinVal, dspal_q31_t cosVal)
{
    arm_park_q31(Ialpha, Ibeta, pId, pIq, sinVal, cosVal);
}


extern void dspal_control_park_inv_f32(dspal_f32_t Id, dspal_f32_t Iq, dspal_f32_t * pIalpha, dspal_f32_t * pIbeta, dspal_f32_t sinVal, dspal_f32_t cosVal)
{
    arm_inv_park_f32(Id, Iq, pIalpha, pIbeta, sinVal, cosVal);
}


extern void dspal_control_park_inv_q31(dspal_q31_t Id, dspal_q31_t Iq, dspal_q31_t * pIalpha, dspal_q31_t * pIbeta, dspal_q31_t sinVal, dspal_q31_t cosVal)
{
    arm_inv_park_q31(Id, Iq, pIalpha, pIbeta, sinVal, cosVal);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern uint32_t dspal_control_hid_getsize(const dspal_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern dspal_result_t dspal_control_hid_setmem(const dspal_cfg_t *cfg, uint32_t *memory)
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




