/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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

#ifndef _HAL_ARCH_CFG_H_
#define _HAL_ARCH_CFG_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_arch_cfg.h
    @brief      This header file keeps public interface to the cfg of hal parts which are specific of architecture.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       09/16/2011
**/

/** @defgroup doxy_group_hal_arch_cfg HAL ARCH CFG GENERIC

    cecece
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#if defined(HAL_USE_ARCH_ARM)
#include "hal_arch_cfg_arm.h"
#elif defined(HAL_USE_ARCH_DSPIC)
#include "hal_arch_cfg_dspic.h"
#elif defined(HAL_USE_ARCH_NONE)
#include "hal_arch_cfg_none.h"
#else
#error please choose an architecture with: HAL_USE_ARCH_ARM, HAL_USE_ARCH_DSPIC, or HAL_USE_ARCH_NONE.
#endif



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


#if defined(HAL_USE_ARCH_ARM)
typedef hal_arch_arm_cfg_t      hal_arch_cfg_t;
#elif defined(HAL_USE_ARCH_DSPIC)
typedef hal_arch_dspic_cfg_t    hal_arch_cfg_t;
#elif defined(HAL_USE_ARCH_NONE)
typedef hal_arch_none_cfg_t     hal_arch_cfg_t;
#endif



 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section


/** @}            
    end of group doxy_group_hal_arch_cfg  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

