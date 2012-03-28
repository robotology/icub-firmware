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

#ifndef _OSAL_ARCH_CFG_ARM_H_
#define _OSAL_ARCH_CFG_ARM_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       osal_arch_cfg_arm.h
    @brief      This header file keeps public interface to the cfg of osal parts which are specific of arm architecture.
    @author     marco.accame@iit.it
    @date       12/05/2011
**/

/** @defgroup osal_arch_cfg_arm OSAL CFG ARCH ARM

    cecece
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------
// empty-section


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef struct osal_arch_arm_cfg_t 
    @brief      osal_arch_arm_cfg_t contains osal configuration for the parts which are specifics only of ARM architecture.
 **/  
typedef struct
{   
    uint32_t        nothingsofar;           /**< nothing so far */
} osal_arch_arm_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section


/** @}            
    end of group osal_arch_cfg_arm  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



