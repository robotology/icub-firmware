/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _HAL_CHIP_XX_EEPROM_H_
#define _HAL_CHIP_XX_EEPROM_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_chip_xx_eeprom.h
    @brief      This header file contains public interface for the management of some i2c EEPROMs using hal apis.
    @author     marco.accame@iit.it
    @date       09/14/2012
**/

/** @defgroup arm_hal_hal_chip_xx_eeprom HAL EEPROM

    The STM32 EEPROM ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"
#include "hal_i2c.h"


// - public #define  --------------------------------------------------------------------------------------------------

// even if some eeproms could manage more than 64k ... we dont manage addresses higher than 64k 
#define         HAL_CHIP_XX_EEPROM_maxEEPROMsize                   (64*1024)
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hal_chip_xx_eeprom_chip_t 
    @brief      contains every possible eeprom device.
 **/ 
typedef enum  
{ 
    hal_chip_xx_eeprom_chip_st_m24lr64       = 0,    /**< capacity 8kbytes; page size 4 bytes; i2c speeds: 400khz. it is also wireless, but we don't care. */
    hal_chip_xx_eeprom_chip_st_m24c64        = 1,    /**< capacity 8kbytes; page size 32 bytes; i2c speeds: 1mhz, 400khz, 100khz.  */
    hal_chip_xx_eeprom_chip_st_m24512        = 2,    /**< capacity 64kbytes; page size 128 bytes; i2c speeds: 1mhz, 400khz, 100khz. */
    hal_chip_xx_eeprom_chip_atmel_at24c512b  = 3,    /**< capacity 64kbytes; page size 128 bytes; i2c speeds: 1mhz, 400khz.  */
    hal_chip_xx_eeprom_chip_atmel_at24c1024b = 4,    /**< capacity 128kbytes; page size 256 bytes; i2c speeds: 1mhz, 400khz.  */
    hal_chip_xx_eeprom_chip_none             = 255   /**< just no device ... yet. */
} hal_chip_xx_eeprom_chip_t;



typedef struct
{
    hal_chip_xx_eeprom_chip_t       chip;
    hal_i2c_t                       i2cport;      
    uint8_t                         hwaddra2a1a0;       /**< put a 1 in pos 2, 1, 0 if the a2, a1, a0 is high. */
} hal_chip_xx_eeprom_chip_cfg_t;


typedef struct
{
    hal_res_fp_void_t               wrpr_init;
    hal_res_fp_void_t               wrpr_deinit;
    hal_res_fp_void_t               wrpr_enable;
    hal_res_fp_void_t               wrpr_disable;
} hal_chip_xx_eeprom_wrpr_cfg_t;


/** @typedef    typedef struct hal_chip_xx_eeprom_cfg_t;
    @brief      contains configuration data of eeprom.
 **/
typedef struct
{
    hal_chip_xx_eeprom_chip_cfg_t   chipcfg;
    //hal_chip_xx_eeprom_i2c_ext_t       i2cext;
    hal_chip_xx_eeprom_wrpr_cfg_t   wrprcfg;
} hal_chip_xx_eeprom_cfg_t;

// remember to implement sw reset as seen in atmel datasheet.
 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_chip_xx_eeprom_cfg_t hal_chip_xx_eeprom_cfg_default;   


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn    	    extern hal_result_t hal_chip_xx_eeprom_init(const hal_chip_xx_eeprom_cfg_t *cfg)
    @brief      This function initialises internal memory and the eeprom device. Initialisation can be done only once.
                If one wants to initialise once more, he/she must call hal_chip_xx_eeprom_deinit() first.
    @param      cfg             The configuration. If NULL it uses the default configuration. 
    @return     In case of successful initialisation is hal_res_OK, in case of error hal_res_NOK_generic.
  */
extern hal_result_t hal_chip_xx_eeprom_init(const hal_chip_xx_eeprom_cfg_t *cfg);


/** @fn    	    extern hal_result_t hal_chip_xx_eeprom_deinit(void)
    @brief      This function de-initialises internal memory and the eeprom device.
    @return     In case of successful initialisation is hal_res_OK, in case of error hal_res_NOK_generic.
  */
extern hal_result_t hal_chip_xx_eeprom_deinit(const hal_chip_xx_eeprom_cfg_t *cfg);


/** @fn    	    extern hal_result_t hal_chip_xx_eeprom_read(uint32_t address, uint32_t size, uint8_t* buffer)
    @brief      Reads from a specific @e address @e size bytes and puts them in @e buffer.
    @param      address         address at which the function will start to read.
    @param      size            number of bytes to read
    @param      buffer          pointer to buffer
    @param      readbytes       if not NULL, then it contains the number of bytes effectively read
    @return     hal_res_NOK_generic in case buffer is NULL or size is zero; else hal_res_OK
  */
extern hal_result_t hal_chip_xx_eeprom_read(uint32_t address, uint32_t size, uint8_t* buffer, uint32_t* readbytes);


/** @fn    	    extern hal_result_t hal_chip_xx_eeprom_write(uint32_t address, uint32_t size, uint8_t* buffer)
    @brief      Writes into a specific @e address @e size bytes contained inside @e buffer.
    @param      address         address at which the function will start to write.
    @param      size            number of bytes to write
    @param      buffer          pointer to buffer
    @param      writtenbytes    if not NULL, then it contains the number of bytes effectively written
    @return     hal_res_NOK_generic in case buffer is NULL or size is zero; else hal_res_OK
  */
extern hal_result_t hal_chip_xx_eeprom_write(uint32_t address, uint32_t size, uint8_t* buffer, uint32_t* writtenbytes);



/** @}            
    end of group arm_hal_hal_chip_xx_eeprom  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



