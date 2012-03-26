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

#ifndef _HAL_ARCH_NONE_H_
#define _HAL_ARCH_NONE_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_arch_none.h
    @brief      This header file keeps public interface to the hal parts which are specific of no architecture.
    @author     marco.accame@iit.it
    @date       09/16/2011
**/

/** @defgroup hal_arch_none HAL ARCH NONE

    cecece
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/



// - public #define  --------------------------------------------------------------------------------------------------


#if !defined(HAL_USE_ARCH_NONE)
#define HAL_USE_ARCH_NONE
#endif

//#define HAL_HAS_BASE
//#define HAL_HAS_CAN
//#define HAL_HAS_CRC
//#define HAL_HAS_DISPLAY
//#define HAL_HAS_EEPROM
//#define HAL_HAS_ENCODER
//#define HAL_HAS_ETH
//#define HAL_HAS_FLASH
//#define HAL_HAS_GPIO
//#define HAL_HAS_LED
//#define HAL_HAS_SYS
//#define HAL_HAS_TIMER
//#define HAL_HAS_TRACE
//#define HAL_HAS_WATCHDOG
  

// - external dependencies --------------------------------------------------------------------------------------------
// empty-section



// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section
 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section



// - declaration of extern public functions ---------------------------------------------------------------------------




/** @}            
    end of group hal_arch_arm  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



