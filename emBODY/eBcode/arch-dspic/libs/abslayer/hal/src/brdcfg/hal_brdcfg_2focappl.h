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
#ifndef _HAL_BRDCFG_2FOCAPPL_H_
#define _HAL_BRDCFG_2FOCAPPL_H_

/* @file       hal_brdcfg_2focappl.h
    @brief      This header file defines ...
    @author     marco.accame@iit.it
    @date       08/21/2014
 **/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"


#ifdef HAL_USE_CRC
    #include "hal_crc.h"
#endif

#ifdef HAL_USE_EEPROM
    #include "hal_eeprom.h"
#endif

#ifdef HAL_USE_FLASH
    #include "hal_flash.h"
#endif


// - public #define  --------------------------------------------------------------------------------------------------



// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------


#ifdef HAL_USE_CRC
    extern const uint8_t hal_brdcfg_crc__supported_mask; //           = 0x03;
#endif//HAL_USE_CRC


// - declaration of extern public functions ---------------------------------------------------------------------------


#ifdef HAL_USE_EEPROM
    extern const uint8_t hal_brdcfg_eeprom__supported_mask;
    extern const uint32_t hal_brdcfg_eeprom__emflash_baseaddress;
    extern const uint32_t hal_brdcfg_eeprom__emflash_totalsize;
#endif//HAL_USE_EEPROM 

  

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


