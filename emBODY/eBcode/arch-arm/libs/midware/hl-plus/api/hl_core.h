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

#ifndef _HL_CORE_H_
#define _HL_CORE_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_core.h
    @brief      This header file keeps core interface to build hl library
    @author     marco.accame@iit.it
    @date       10/21/2013
**/

// - external dependencies --------------------------------------------------------------------------------------------


#if     defined(HL_CFG_PLUS_TARGET_BY_FILE)
#include "hl_cfg_plus_target.h"
#endif



#if     defined(HL_USE_MPU_ARCH_STM32F1)

    #include "../../core/stm32f1/api/stm32f1.h"
    
#elif   defined(HL_USE_MPU_ARCH_STM32F2)

    #include "../../core/stm32f2/api/stm32f2.h"

#elif   defined(HL_USE_MPU_ARCH_STM32F4)

    #include "../../core/stm32f4/api/stm32f4.h"
    
#else
    #error HL: specify a HL_USE_MPU_ARCH_*
#endif

// - public #define  --------------------------------------------------------------------------------------------------


  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------



// - declaration of extern public functions ---------------------------------------------------------------------------




#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



