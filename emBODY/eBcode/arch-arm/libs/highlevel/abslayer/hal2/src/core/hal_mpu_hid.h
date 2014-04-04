/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _HAL_CPU_HID_H_
#define _HAL_CPU_HID_H_


/* @file       hal_mpu_hid.h
    @brief      This header file implements hidden interface to hal.
    @author     marco.accame@iit.it
    @date       02/27/2013
 **/


// - external dependencies --------------------------------------------------------------------------------------------



// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_mpu.h"


// - #define used with hidden structs ---------------------------------------------------------------------------------
// empty-section

// - definition of hidden structs -------------------------------------------------------------------------------------


typedef struct 
{
    uint32_t                        max;
    uint32_t                        cpu;
    uint32_t                        fastbus;
    uint32_t                        slowbus;
} hal_mpu_speeds_t;


typedef struct
{
    hal_mpu_arch_t              architecture;
    hal_mpu_type_t              type;
    hal_mpu_name_t              name;
    hal_mpu_speeds_t            speeds;             // the speeds for cpu, fast bus and slow bus that the system has
} hal_mpu_boardconfig_t;

// - declaration of extern hidden variables ---------------------------------------------------------------------------

extern const hal_mpu_boardconfig_t hal_mpu__theboardconfig;

// - declaration of extern hidden functions ---------------------------------------------------------------------------




// - definition of extern hidden inline functions ---------------------------------------------------------------------
// empty-section

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




