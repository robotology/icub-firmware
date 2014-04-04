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

#ifndef _HAL_H_
#define _HAL_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal.h
    @brief      This header file implements public interface to the hal modules.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       09/09/2011
**/

/** @defgroup doxy_group_hal HAL

    The Hardware Abstraction Layer offers service to the system which are independent from the underlying
    HW platform.
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h"

// -- the core part of hal: base, cpu, and sys.
#include "hal_core.h"

// -- the other modules of hal included in alphabetical order. they may not be available for some boards.

#include "hal_accelerometer.h"
#include "hal_can.h"
#include "hal_cantransceiver.h"
#include "hal_eeprom.h"
#include "hal_encoder.h"
#include "hal_eth.h"
#include "hal_ethtransceiver.h"
#include "hal_flash.h"
#include "hal_gpio.h"
#include "hal_gyroscope.h"
#include "hal_i2c.h"
#include "hal_led.h"
#include "hal_mux.h"
#include "hal_spi.h"
#include "hal_switch.h"
#include "hal_termometer.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_uniqueid.h"
#include "hal_watchdog.h"








// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section


/** @}            
    end of group doxy_group_hal  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



