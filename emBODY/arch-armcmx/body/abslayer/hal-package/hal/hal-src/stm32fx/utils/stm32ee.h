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
    @brief      hal_eeprom_t contains every possible eeprom device.
 **/ 
typedef enum  
{ 
    stm32ee_device_st_m24lr64       = 0,    /**< capacity 8kbytes; page size 4 bytes; i2c speeds: 400khz. it is also wireless, but we don't care. */
    stm32ee_device_st_m24c64        = 1,    /**< capacity 8kbytes; page size 32 bytes; i2c speeds: 1mhz, 400khz, 100khz.  */
    stm32ee_device_st_m24512        = 2,    /**< capacity 64kbytes; page size 128 bytes; i2c speeds: 1mhz, 400khz, 100khz. */
    stm32ee_device_atmel_at24c512b  = 3,    /**< capacity 64kbytes; page size 128 bytes; i2c speeds: 1mhz, 400khz.  */
    stm32ee_device_atmel_at24c1024b = 4,    /**< capacity 128kbytes; page size 256 bytes; i2c speeds: 1mhz, 400khz.  */
    stm32ee_device_none                     /**< just no device ... yet. */
} stm32ee_device_t;


/** @typedef    typedef void (*stm32ee_void_fp_void_t) (void)
    @brief      stm32ee_void_fp_void_t is a pointer to a function which returns void and has a void argument.
 **/
typedef void (*stm32ee_void_fp_void_t) (void);


/** @typedef    typedef void (*stm32ee_void_fp_voidp_t) (void* p)
    @brief      stm32ee_void_fp_voidp_t is a pointer to a function which returns void and has a void pointer argument.
 **/
typedef void (*stm32ee_void_fp_voidp_t) (void* p);


/** @typedef    typedef struct stm32ee_cfg_t;
    @brief      contains configuration data of eeprom.
 **/
typedef struct
{
    stm32ee_device_t        device;
    uint32_t                i2cspeed;
    uint8_t                 i2cbus; // ?
    uint8_t                 usedmatransfer; // ?
    uint8_t                 dmachannel; // ?
    stm32ee_void_fp_voidp_t functioni2cinit;
    void*                   parameteri2cinit;
    stm32ee_void_fp_void_t  functioni2cdeinit;
    stm32ee_void_fp_void_t  functionontimeout;
    // useofdma?
    // external wait functions?
    // waitparam?
    // external error function?
} stm32ee_cfg_t;

// remember to implement sw reset as seen in atmel datasheet.
 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const stm32ee_cfg_t stm32_cfg_default;   // = { .device = stm32ee_device_st_m24lr64, .i2cbus = 0, .functioni2cinit = NULL, .parameteri2cinit = NULL, .functioni2cdeinit = NULL };


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
extern stm32ee_result_t stm32ee_deinit(void);


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



