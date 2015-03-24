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
#ifndef _EEMEMORYMAP_H_
#define _EEMEMORYMAP_H_


// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       eEmemorymap.h
    @brief      This header file defines memory map for the ems004 board.
    @author     marco.accame@iit.it
    @date       1/13/2014
**/

#define EENV_USE_MEMORYMAP_STM32F4_1024K_V2

#if     defined(EENV_USE_MEMORYMAP_STM32F4_1024K_V2)

#include "eEmemorymap_stm32f4_1024k_v2.h"

#endif//defined()

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




