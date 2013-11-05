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

#ifndef _HL_CHIP_XX_EEPROM_H_
#define _HL_CHIP_XX_EEPROM_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_chip_xx_eeprom.h
    @brief      This header file contains public interface for the management of some i2c EEPROMs under HL.
    @author     marco.accame@iit.it
    @date       10/29/2013
**/

/** @defgroup arm_hl_hl_chip_xx_eeprom HL EEPROM

    The STM32 EEPROM ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"
#include "hl_i2c.h"
#include "hl_gpio.h"


// - public #define  --------------------------------------------------------------------------------------------------

// even if some EEPROMs could manage more than 64k ... we don't manage addresses higher than 64k 
#define         HL_CHIP_XX_EEPROM_maxEEPROMsize                   (64*1024)
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hl_chip_xx_eeprom_chip_t 
    @brief      contains every supported EEPROM chip
 **/ 
typedef enum  
{ 
    hl_chip_xx_eeprom_chip_st_m24lr64       = 0,    /**< capacity 8kbytes; page size 4 bytes; i2c speeds: 400khz. it is also wireless, but we don't care. */
    hl_chip_xx_eeprom_chip_st_m24c64        = 1,    /**< capacity 8kbytes; page size 32 bytes; i2c speeds: 1mhz, 400khz, 100khz.  */
    hl_chip_xx_eeprom_chip_st_m24512        = 2,    /**< capacity 64kbytes; page size 128 bytes; i2c speeds: 1mhz, 400khz, 100khz. */
    hl_chip_xx_eeprom_chip_atmel_at24c512b  = 3,    /**< capacity 64kbytes; page size 128 bytes; i2c speeds: 1mhz, 400khz.  */
    hl_chip_xx_eeprom_chip_atmel_at24c1024b = 4,    /**< capacity 128kbytes; page size 256 bytes; i2c speeds: 1mhz, 400khz.  */
    hl_chip_xx_eeprom_chip_none             = 255   /**< just no device ... yet. */
} hl_chip_xx_eeprom_chip_t;



/** @typedef    typedef struct hl_chip_xx_eeprom_cfg_t;
    @brief      contains configuration data of EEPROM.
 **/
typedef struct
{
    hl_chip_xx_eeprom_chip_t        chip;           /**< specifies the EEPROM chip */  
    hl_i2c_t                        i2cid;          /**< it is the i2c bus where the EEPROM is attached */      
    uint8_t                         hwaddra2a1a0;   /**< put 1 in bit position 2, 1, 0 if pin a2, a1, a0 is high. */
    hl_gpio_val_t                   wp_val;         /**< if hl_gpio_valUNDEF, write protection is not used. Otherwise it is the value which protects from writing. */
    hl_gpio_t                       wp_gpio;        /**< the gpio which is used for write protection. If WP is used, then the EEPROM is always protect except when writing */
} hl_chip_xx_eeprom_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hl_chip_xx_eeprom_cfg_t hl_chip_xx_eeprom_cfg_default;   


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn    	    extern hl_result_t hl_chip_xx_eeprom_init(const hl_chip_xx_eeprom_cfg_t *cfg)
    @brief      This function initialises internal memory and the eeprom device. Initialisation can be done only once.
                If one wants to initialise once more, he/she must call hl_chip_xx_eeprom_deinit() first.
    @param      cfg             The configuration. If NULL it uses the default configuration. 
    @return     In case of successful initialisation is hl_res_OK, in case of error hl_res_NOK_generic.
    @warning    The i2c bus must be initialised before the call of this function.    
  */
extern hl_result_t hl_chip_xx_eeprom_init(const hl_chip_xx_eeprom_cfg_t *cfg);


/** @fn    	    extern hl_result_t hl_chip_xx_eeprom_deinit(void)
    @brief      This function de-initialises internal memory and the eeprom device.
    @return     In case of successful initialisation is hl_res_OK, in case of error hl_res_NOK_generic.
  */
extern hl_result_t hl_chip_xx_eeprom_deinit(const hl_chip_xx_eeprom_cfg_t *cfg);


/** @fn    	    extern hl_result_t hl_chip_xx_eeprom_read(uint32_t address, uint32_t size, uint8_t* buffer)
    @brief      Reads from a specific @e address @e size bytes and puts them in @e buffer.
    @param      address         address at which the function will start to read.
    @param      size            number of bytes to read
    @param      buffer          pointer to buffer
    @param      readbytes       if not NULL, then it contains the number of bytes effectively read
    @return     hl_res_NOK_generic in case buffer is NULL or size is zero; else hl_res_OK
  */
extern hl_result_t hl_chip_xx_eeprom_read(uint32_t address, uint32_t size, uint8_t* buffer, uint32_t* readbytes);


/** @fn    	    extern hl_result_t hl_chip_xx_eeprom_write(uint32_t address, uint32_t size, uint8_t* buffer)
    @brief      Writes into a specific @e address @e size bytes contained inside @e buffer.
    @param      address         address at which the function will start to write.
    @param      size            number of bytes to write
    @param      buffer          pointer to buffer
    @param      writtenbytes    if not NULL, then it contains the number of bytes effectively written
    @return     hl_res_NOK_generic in case buffer is NULL or size is zero; else hl_res_OK
  */
extern hl_result_t hl_chip_xx_eeprom_write(uint32_t address, uint32_t size, uint8_t* buffer, uint32_t* writtenbytes);


/** @}            
    end of group arm_hl_hl_chip_xx_eeprom  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



