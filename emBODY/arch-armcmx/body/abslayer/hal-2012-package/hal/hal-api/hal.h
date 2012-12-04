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

#ifndef _HAL_H_
#define _HAL_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal.h
    @brief      This header file implements public interface to the hal modules.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       09/09/2011
**/

/** @defgroup hal HAL

    The Hardware Abstraction Layer offers service to the system which are independent from the underlying
    HW platform.
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------


// inside hal_arch.h are defined or undefined the HAL_HAS_*
#include "hal_arch.h"

#if defined(HAL_HAS_BASE)
#include "hal_base.h"
#endif

#if defined(HAL_HAS_CAN)
#include "hal_can.h"
#endif

#if defined(HAL_HAS_CRC)
#include "hal_crc.h"
#endif

#if defined(HAL_HAS_DISPLAY)
#include "hal_display.h"
#endif

#if defined(HAL_HAS_EEPROM)
#include "hal_device_eeprom.h"
#endif

#if defined(HAL_HAS_ENCODER)
#include "hal_sensor_encoder.h"
#endif

#if defined(HAL_HAS_ETH)
#include "hal_eth.h"
#endif

#if defined(HAL_HAS_FLASH)
#include "hal_flash.h"
#endif

#if defined(HAL_HAS_GPIO)
#include "hal_gpio.h"
#endif

#if defined(HAL_HAS_LED)
#include "hal_actuator_led.h"
#endif

#if defined(HAL_HAS_SYS)
#include "hal_sys.h"
#endif

#if defined(HAL_HAS_TIMER)
#include "hal_timer.h"
#endif

#if defined(HAL_HAS_TRACE)
#include "hal_trace.h"
#endif

#if defined(HAL_HAS_WATCHDOG)
#include "hal_watchdog.h"
#endif



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section


/** @}            
    end of group hal  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



