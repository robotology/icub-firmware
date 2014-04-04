/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _HAL_GYROSCOPE_HID_H_
#define _HAL_GYROSCOPE_HID_H_


/* @file       hal_gyroscope_hid.h
    @brief      This header file implements hidden interface to a gyroscope
    @author     marco.accame@iit.it
    @date       10/24/2012
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h"



// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_gyroscope.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct
{
    void*                                   initpar; 
} hal_gyroscope_driver_cfg_t;

typedef struct
{   
    hal_res_fp_int32_voidp_t                init;   // init(id, void* initpar)
    hal_res_fp_int32_voidp_voidp_voidp_t    read;   // read(id, int32_t* angratex, int32_t* angratey, int32_t* angratez)
} hal_gyroscope_driver_fun_t;

typedef struct
{ 
    hal_gyroscope_driver_cfg_t              cfg;    
    hal_gyroscope_driver_fun_t              fun;
} hal_gyroscope_driver_t;   

typedef struct
{
    uint32_t                                supportedmask;
    hal_gyroscope_driver_t                  driver[hal_gyroscopes_number];
} hal_gyroscope_boardconfig_t;

// - declaration of extern hidden variables ---------------------------------------------------------------------------

extern const hal_gyroscope_boardconfig_t hal_gyroscope__theboardconfig;

// - declaration of extern hidden functions ---------------------------------------------------------------------------




#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




