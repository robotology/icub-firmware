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
#ifndef _DSPAL_BASE_HID_H_
#define _DSPAL_BASE_HID_H_


/* @file       dspal_base_hid.h
    @brief      This header file implements hidden interface to parts of hal specifics of architecture
    @author     marco.accame@iit.it
    @date       09/19/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

//#warning    defined CM3, MATRIX_CHECH, ROUNDING
#define ARM_MATH_CM3
#define ARM_MATH_MATRIX_CHECK
#define ARM_MATH_ROUNDING

//#include "dspal_base.h"
#include "cmsisdsp.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "dspal_base.h"


// - #define used with hidden structs ---------------------------------------------------------------------------------

//#warning    defined CM3, MATRIX_CHECH, ROUNDING
//#define ARM_MATH_CM3
//#define ARM_MATH_MATRIX_CHECK
//#define ARM_MATH_ROUNDING


#define RES         dspal_result_t
#define CMATF32P    const arm_matrix_instance_f32*
#define MATF32P     arm_matrix_instance_f32*
#define CMATQ31P    const arm_matrix_instance_q31*
#define MATQ31P     arm_matrix_instance_q31*
#define CMATQ15P    const arm_matrix_instance_q15*
#define MATQ15P     arm_matrix_instance_q15*

#define FROM_CMSISSTATUS_TO_DSPALRESULT(e)    ((ARM_MATH_SUCCESS==(e))?(dspal_res_OK):((dspal_result_t)(((int8_t)(e)-30))))


// - definition of hidden structs -------------------------------------------------------------------------------------
// empty-section

// - declaration of extern hidden variables ---------------------------------------------------------------------------

extern dspal_cfg_t dspal_base_hid_cfg;


// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern uint32_t dspal_base_hid_getsize(const dspal_cfg_t *cfg);
extern dspal_result_t dspal_base_hid_setmem(const dspal_cfg_t *cfg, uint32_t *memory);


extern void dspal_base_hid_error(dspal_errorcode_t err_code, const char *err_msg);

// - definition of extern hidden inline functions ---------------------------------------------------------------------
// empty-section

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




