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

#ifndef _HL_CFG_PLUS_TARGET_H_
#define _HL_CFG_PLUS_TARGET_H_

//#include "hl_target.h"

// -- definitions of macros used in hl

#if     !defined(HL_CFG_PLUS_TARGET_BY_FILE)
    #warning HL: HL_CFG_PLUS_TARGET_BY_FILE is not defined and you are using hl_cfg_plus_target.h
#endif


#if     defined(HL_USE_BRD_MCBSTM32)
 
    #define HL_USE_MPU_NAME_STM32F103RB
    
#elif   defined(HL_USE_BRD_MCBSTM32C)

    #define HL_USE_MPU_NAME_STM32F107VC
    
    #undef HL_CFG_MPUSPEED_HSEBYPASS
    #define HL_CFG_MPUSPEED_INTclockspeed    ((uint32_t) 8000000)
    #define HL_CFG_MPUSPEED_EXTclockspeed    ((uint32_t)25000000)    

#elif   defined(HL_USE_BRD_EMS001)    

    #define HL_USE_MPU_NAME_STM32F107VC   

#else
    #error ERR: cannot define MPU_NAME
#endif


// MPU ARCH:
#include "hl_mpu2arch.h"


// MPU SPEED CFG:
#include "hl_mpuspeed.h"

#endif  // include-guard



