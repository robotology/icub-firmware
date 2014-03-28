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

#ifndef _HAL_MPU_NAME_H_
#define _HAL_MPU_NAME_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_mpu_name.h
    @brief      This header file keeps public interface to the hal parts which are specific of a particular mpu
    @author     marco.accame@iit.it
    @date       02/27/2013
**/

/** @defgroup doxy_group_hal_mpu_name HAL MPU NAME

    cecece
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/



// - public #define  --------------------------------------------------------------------------------------------------


// - external dependencies --------------------------------------------------------------------------------------------


#if     defined(HAL_USE_MPU_NAME_STM32F107VC)

#include "hal_mpu_name_stm32f107vc.h"

#elif   defined(HAL_USE_MPU_NAME_STM32F407IG)

#include "hal_mpu_name_stm32f407ig.h"

#else//
    #error --> to use this file you must specify a valid HAL_USE_MPU_NAME_xxx
#endif


// - declaration of public user-defined types ------------------------------------------------------------------------- 


 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section



// - declaration of extern public functions ---------------------------------------------------------------------------




/** @}            
    end of group doxy_group_hal_mpu_name  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



