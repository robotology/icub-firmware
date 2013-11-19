/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

#ifndef _HAL_ARCH_CFG_ARM_H_
#define _HAL_ARCH_CFG_ARM_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_arch_cfg_arm.h
    @brief      This header file keeps public interface to the cfg of hal parts which are specific of arm architecture.
    @author     marco.accame@iit.it, valentina.gaggero@iit.it
    @date       09/16/2011
**/

/** @defgroup hal_arch_cfg_arm HAL CFG ARCH ARM

    cecece
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------
// empty-section


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef struct hal_arch_arm_cfg_t 
    @brief      hal_arch_arm_cfg_t contains hal configuration for the parts which are specifics only of ARM architecture.
 **/  
typedef struct
{   
    uint32_t        nothingsofar;           /**< nothing so far */
} hal_arch_arm_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section


/** @}            
    end of group hal_arch_cfg_arm  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



