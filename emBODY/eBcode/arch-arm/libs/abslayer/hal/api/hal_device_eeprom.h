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

#ifndef _HAL_DEVICE_EEPROM_H_
#define _HAL_DEVICE_EEPROM_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_device_eeprom.h
    @brief      This header file keeps public interface to the hal device eeprom.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       09/09/2011
**/

/** @defgroup arm_hal_device_eeprom HAL EEPROM

    The HAL EEPROM ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hal_device_eeprom_t 
    @brief      hal_device_eeprom_t contains every possible eeprom peripheral.
 **/ 
typedef enum  
{ 
    hal_device_eeprom_emulatedflash    = 0,    /**< the eeprom is emulated in internal flash */
    hal_device_eeprom_i2c_01           = 1,    /**< the eeprom is on I2C. the used I2C port and address are internally defined in the board config file */
    hal_device_eeprom_i2c_02           = 2     /**< one more eeprom on I2C. the used I2C port and address are internally defined in the board config file */
} hal_device_eeprom_t;

enum { hal_device_eeproms_num = 3 };


/** @typedef    typedef struct hal_device_eeprom_cfg_t;
    @brief      contains configuration data of eeprom peripheral.
 **/
typedef struct
{
    void*       flashpagebuffer;    /**< In case of hal_device_eeprom_emulatedflash, it keeps a buffer for the FLASH page */
    uint32_t    flashpagesize;      /**< In case of hal_device_eeprom_emulatedflash, it keeps the size of the flashpagebuffer */
} hal_device_eeprom_cfg_t;


 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_device_eeprom_cfg_t hal_device_eeprom_cfg_default;   // = { .flashpagebuffer = NULL, .flashpagesize = 0 };


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn    	    extern void hal_device_eeprom_init(hal_device_eeprom_t eep, const hal_device_eeprom_cfg_t *cfg);
    @brief      This function initializes EEPROM.
    @param      eep             The eeprom peripheral to configure
    @param      cfg             The configuration. If NULL it uses the default configuration. 
                                In case of hal_device_eeprom_emulatedflash a NULL cfg uses internal buffering if available..
    @return     In case of successful initialisation is hal_res_OK, in case of error hal_NOK_*.
  */
extern hal_result_t hal_device_eeprom_init(hal_device_eeprom_t eep, const hal_device_eeprom_cfg_t *cfg);


/** @fn    	    extern hal_result_t hal_device_eeprom_read(hal_device_eeprom_t eep, uint32_t addr, uint32_t size, void *data)
    @brief      Reads @e size byte from a specific EEPROM address @e addr and puts them in a buffer @e data.
    @param      eep             The eeprom peripheral
    @param      addr            EEPROM address at which the function will start to read.
    @param      size            number of bytes to read
    @param      data            pointer to buffer
    @return     hal_res_NOK_generic in case data is NULL or size is zero; else hal_res_OK
  */
extern hal_result_t hal_device_eeprom_read(hal_device_eeprom_t eep, uint32_t addr, uint32_t size, void *data);


/** @fn    	    hal_device_eeprom_write(hal_device_eeprom_t eep, uint32_t addr, uint32_t size, void *data)
    @brief      Writes a data buffer in EEPROM at a specific address.
    @param      eep             The eeprom peripheral
    @param      addr            EEPROM address at which the function will start to write.
    @param      size            number of bytes to write
    @param      data            pointer to buffer
    @return     hal_res_NOK_generic in case data is NULL or size is zero or page buffer is not big enough; 
                else hal_res_OK.
    @warning    In case of hal_device_eeprom_emulatedflash, if the addresses in [addr, addr+size) are in pages
                with size bigger than teh dimension of the buffer passed in hal_device_eeprom_int(), then an error
                is returned and the write operation is not done completely or is done in an incomplete way.
  */
extern hal_result_t hal_device_eeprom_write(hal_device_eeprom_t eep, uint32_t addr, uint32_t size, void *data);


/** @fn    	    extern hal_result_t hal_device_eeprom_erase(hal_device_eeprom_t eep, uint32_t addr, uint32_t size)
    @brief      Erases the EEPROM in addresses [@e addr, @e addr + @e size). At the end the memory shall be 0xFF
    @param      eep             The eeprom peripheral
    @param      addr         EEPROM address at which the function will start to erase.
    @param      size        number of bytes to erase
    @return     hal_res_NOK_generic in case size is zero or page buffer is not big enough; else hal_res_OK.
    @warning    In case of hal_device_eeprom_emulatedflash, if the addresses in [addr, addr+size) are in pages
                with size bigger than teh dimension of the buffer passed in hal_device_eeprom_int(), then an error
                is returned and the erase operation is not done completely or is done in an incomplete way.
  */
extern hal_result_t hal_device_eeprom_erase(hal_device_eeprom_t eep, uint32_t addr, uint32_t size);


/** @fn    	    extern uint32_t hal_device_eeprom_get_baseaddress(hal_device_eeprom_t eep)
    @brief      Gets the base address of the specified EEPROM
    @param      eep             The eeprom peripheral
    @return     The base address
  */
extern uint32_t hal_device_eeprom_get_baseaddress(hal_device_eeprom_t eep);


/** @fn    	    extern uint32_t hal_device_eeprom_get_totalsize(hal_device_eeprom_t eep)
    @brief      Gets the total size of the specified EEPROM
    @param      eep             The eeprom peripheral
    @return     The total size
  */
extern uint32_t hal_device_eeprom_get_totalsize(hal_device_eeprom_t eep);


/** @fn         extern hal_bool_t hal_device_eeprom_address_is_valid(hal_device_eeprom_t eep, uint32_t addr)
    @brief      tells if @addr is managed by the EEPROM
    @param      eep             The eeprom peripheral
    @param      addr            The address of the EEPROM which one wants to check
    @return     hal_false or hal_true
 **/
extern hal_bool_t hal_device_eeprom_address_is_valid(hal_device_eeprom_t eep, uint32_t addr);


/** @}            
    end of group arm_hal_device_eeprom  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



