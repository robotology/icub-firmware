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
    @brief      This header file defines memory map for the ems001 board.
    @author     marco.accame@iit.it
    @date       11/20/2013
**/

#define EENV_USE_MEMORYMAP_STM32F1_256K_V1

#if     defined(EENV_USE_MEMORYMAP_STM32F1_256K_V1)

#include "eEmemorymap_stm32f1_256k_v1.h"

#endif//defined()

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




