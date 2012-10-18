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

#ifndef _STM32EE_H_
#define _STM32EE_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       stm32ee.h
    @brief      This header file contains public interface for the management of some i2c EEPROMs using stm32 apis.
    @author     marco.accame@iit.it
    @date       09/14/2012
**/

/** @defgroup arm_hal_stm32ee STM32 EEPROM

    The STM32 EEPROM ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"

#include "stm32gpio.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum stm32ee_result_t 
    @brief      stm32ee_result_t contains function results for this module.
 **/ 
typedef enum  
{ 
    stm32ee_res_OK          = 0,  
    stm32ee_res_NOK         = 1
} stm32ee_result_t;

/** @typedef    typedef enum stm32ee_device_t 
    @brief      contains every possible eeprom device.
 **/ 
typedef enum  
{ 
    stm32ee_device_st_m24lr64       = 0,    /**< capacity 8kbytes; page size 4 bytes; i2c speeds: 400khz. it is also wireless, but we don't care. */
    stm32ee_device_st_m24c64        = 1,    /**< capacity 8kbytes; page size 32 bytes; i2c speeds: 1mhz, 400khz, 100khz.  */
    stm32ee_device_st_m24512        = 2,    /**< capacity 64kbytes; page size 128 bytes; i2c speeds: 1mhz, 400khz, 100khz. */
    stm32ee_device_atmel_at24c512b  = 3,    /**< capacity 64kbytes; page size 128 bytes; i2c speeds: 1mhz, 400khz.  */
    stm32ee_device_atmel_at24c1024b = 4,    /**< capacity 128kbytes; page size 256 bytes; i2c speeds: 1mhz, 400khz.  */
    stm32ee_device_none             = 255   /**< just no device ... yet. */
} stm32ee_device_t;


/** @typedef    typedef struct stm32ee_regaddr_t 
    @brief      contains the address of register of an eeprom.
 **/
typedef struct 
{
    uint8_t     numofbytes;         /**< only 1, 2, 3 */
    union
    {
        uint8_t     three[3];
        uint16_t    two;
        uint8_t     one;
    } bytes;                        /**< teh address */
} stm32ee_regaddr_t;




typedef int8_t (*stm32ee_int8_fp_uint8_voidp_t) (uint8_t, void* p);

typedef int8_t (*stm32ee_int8_fp_uint8_uint8_regaddr_uint8p_uint16_t) (uint8_t, uint8_t, stm32ee_regaddr_t, uint8_t*, uint16_t);

typedef int8_t (*stm32ee_int8_fp_uint8_uint8_t) (uint8_t, uint8_t);


typedef struct
{
    stm32ee_int8_fp_uint8_voidp_t                           i2cinit;
    stm32ee_int8_fp_uint8_voidp_t                           i2cdeinit;
    void*                                                   i2cpar;
    stm32ee_int8_fp_uint8_uint8_regaddr_uint8p_uint16_t     i2cread;
    stm32ee_int8_fp_uint8_uint8_regaddr_uint8p_uint16_t     i2cwrite;
    stm32ee_int8_fp_uint8_uint8_t                           i2cstandby;
} stm32ee_i2c_ext_t;


typedef struct
{
    stm32ee_device_t        device;
    uint8_t                 i2cport;            /**< it can be 1, 2, 3. */
    uint8_t                 hwaddra2a1a0;       /**< put a 1 in pos 2, 1, 0 if the a2, a1, a0 is high. */
    stm32gpio_val_t         wpval;              /**< 1 if protection is high, 0, if it is low, 3 if not used */
    stm32gpio_gpio_t        wppin;
} stm32ee_dev_cfg_t;


/** @typedef    typedef struct stm32ee_cfg_t;
    @brief      contains configuration data of eeprom.
 **/
typedef struct
{
    stm32ee_dev_cfg_t       devcfg;
    stm32ee_i2c_ext_t       i2cext;
} stm32ee_cfg_t;

// remember to implement sw reset as seen in atmel datasheet.
 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const stm32ee_cfg_t stm32_cfg_default;   


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn    	    extern stm32ee_result_t stm32ee_init(const stm32ee_cfg_t *cfg)
    @brief      This function initialises internal memory and the eeprom device. Initialisation can be done only once.
                If one wants to initialise once more, he/she must call stm32ee_deinit() first.
    @param      cfg             The configuration. If NULL it uses the default configuration. 
    @return     In case of successful initialisation is stm32ee_res_OK, in case of error stm32ee_res_NOK.
  */
extern stm32ee_result_t stm32ee_init(const stm32ee_cfg_t *cfg);


/** @fn    	    extern stm32ee_result_t stm32ee_deinit(void)
    @brief      This function de-initialises internal memory and the eeprom device.
    @return     In case of successful initialisation is stm32ee_res_OK, in case of error stm32ee_res_NOK.
  */
extern stm32ee_result_t stm32ee_deinit(const stm32ee_cfg_t *cfg);


/** @fn    	    extern stm32ee_result_t stm32ee_read(uint32_t address, uint32_t size, uint8_t* buffer)
    @brief      Reads from a specific @e address @e size bytes and puts them in @e buffer.
    @param      address         address at which the function will start to read.
    @param      size            number of bytes to read
    @param      buffer          pointer to buffer
    @param      readbytes       if not NULL, then it contains the number of bytes effectively read
    @return     stm32ee_res_NOK in case buffer is NULL or size is zero; else stm32ee_res_OK
  */
extern stm32ee_result_t stm32ee_read(uint32_t address, uint32_t size, uint8_t* buffer, uint32_t* readbytes);


/** @fn    	    extern stm32ee_result_t stm32ee_write(uint32_t address, uint32_t size, uint8_t* buffer)
    @brief      Writes into a specific @e address @e size bytes contained inside @e buffer.
    @param      address         address at which the function will start to write.
    @param      size            number of bytes to write
    @param      buffer          pointer to buffer
    @param      writtenbytes    if not NULL, then it contains the number of bytes effectively written
    @return     stm32ee_res_NOK in case buffer is NULL or size is zero; else stm32ee_res_OK
  */
extern stm32ee_result_t stm32ee_write(uint32_t address, uint32_t size, uint8_t* buffer, uint32_t* writtenbytes);



/** @}            
    end of group arm_hal_stm32ee  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



