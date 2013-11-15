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

#ifndef _HL_I2C_H_
#define _HL_I2C_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_i2c.h
    @brief      This header file implements i2c utilities for hl library
    @author     marco.accame@iit.it
    @date       10/21/2013
**/

/** @defgroup doxy_group_hl_i2c HL I2C utilities

    The HL I2C ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"
#include "hl_gpio.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hl_i2c_t 
    @brief      hl_i2c_t contains the possible I2C ports 
 **/
typedef enum
{
    hl_i2c1 = 0,
    hl_i2c2 = 1,
    hl_i2c3 = 2
} hl_i2c_t;

enum { hl_i2cs_number = 3 };


/** @typedef    typedef enum hl_i2c_speed_t 
    @brief      contains the possible I2C speeds 
 **/
typedef enum
{
    hl_i2c_speed_100kbps       = 1,
    hl_i2c_speed_200kbps       = 2,
    hl_i2c_speed_400kbps       = 4
} hl_i2c_speed_t;


/** @typedef    typedef enum hl_i2c_mode_t 
    @brief      contains the possible I2C modes 
 **/
typedef enum
{
    hl_i2c_mode_master         = 0,     /**< the mode is master */
    hl_i2c_mode_slave          = 1      /**< the mode is slave (not yet supported) */
} hl_i2c_mode_t;


/** @typedef    typedef enum hl_i2c_behaviour_t 
    @brief      contains the possible I2C behaviours.  
 **/
typedef enum
{
    hl_i2c_behaviour_polling    = 0,    /**< internal operations are done by polling / directly writing the registers */
    hl_i2c_behaviour_irq        = 1,    /**< internal operations are done using IRQ handler (not supported yet) */
    hl_i2c_behaviour_dma        = 2     /**< data transfer is done using DMA (not supported yet) */
} hl_i2c_behaviour_t;



/** @typedef    typedef uint8_t hl_i2c_devaddr_t 
    @brief      contains the address for the I2C device.
 **/
typedef uint8_t hl_i2c_devaddr_t;


/** @typedef    typedef struct hl_i2c_regaddr_t 
    @brief      contains the address for the registers internal to the I2C device.
 **/
typedef struct 
{
    uint8_t     numofbytes;             /**< tells if register address is on one, two or three bytes. If zero, register is not used */
    union
    {
        uint8_t     one;
        uint16_t    two;  
        uint8_t     three[3];        
    } bytes;                            /** the address of the register */
} hl_i2c_regaddr_t;



/** @typedef    typedef struct hl_i2c_gpiomap_t 
    @brief      hl_i2c_gpiomap_t contains the pin mapping for an i2c peripherals
 **/
typedef struct
{
    hl_gpio_map_t   scl;       /**< gpio map used for scl pin */
    hl_gpio_map_t   sda;       /**< gpio map used for sda pin */ 
} hl_i2c_gpiomap_t;


/** @typedef    typedef struct hl_i2c_mapping_t 
    @brief      hl_i2c_mapping_t contains the pin mapping for all i2c peripherals
 **/
typedef struct
{
    uint8_t             supported_mask;             /**< bit in position hl_i2cx must be 1 if portx is supported */
    hl_i2c_gpiomap_t    gpiomap[hl_i2cs_number];    /**< in position hl_i2cx there is gpio map of I2Cx */
} hl_i2c_mapping_t;


/** @typedef    typedef struct hl_i2c_advcfg_opaque_t hl_i2c_advcfg_t
    @brief      contains full configuration for i2c bus as specified by the silicon vendor. 
                it is opaquely declared. see hl_arch.h for definition.                
 **/
typedef struct hl_i2c_advcfg_opaque_t hl_i2c_advcfg_t;


/** @typedef    typedef struct hl_i2c_cfg_t 
    @brief      hl_i2c_cfg_t contains the runtime configuration for i2c.
                there are some hw configuration which are hidden: dutycycle mode is tlow/thigh = 2, the ack is enabled.
                if slave mode (not supported yet) the ack address is 7 bit.
 **/
typedef struct
{
    hl_i2c_mode_t           mode;       /**< the mode of the drive: so far only hl_i2c_mode_master */
    hl_i2c_behaviour_t      behaviour;  /**< the behavior of the drive: so far only hl_i2c_behaviour_polling */
    hl_i2c_speed_t          speed;      /**< the i2c speed */      
    hl_i2c_devaddr_t        ownaddress; /**< address of the slave: used only if mode is hl_i2c_mode_slave */
    const hl_i2c_advcfg_t*  advcfg;     /**< used if not NULL. it overrides ... speed, ownaddress, */
} hl_i2c_cfg_t;


 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hl_i2c_cfg_t hl_i2c_cfg_default; // = { .mode = hl_i2c_mode_master, .behaviour = hl_i2c_behaviour_polling, .speed = hl_i2c_speed_400kbps, .ownaddress = 0, .advcfg = NULL };

// it must be externally declared.
extern const hl_i2c_mapping_t hl_i2c_mapping;



// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn			extern hl_result_t hl_i2c_init(hl_i2c_t id, const hl_i2c_cfg_t *cfg)
    @brief  	this function initializes an i2c port
    @param  	id              the id of i2c
    @param  	cfg 	        pointer to configuration. If NULL it is used hl_i2c_cfg_default.
    @return 	hl_res_OK on success
  */
extern hl_result_t hl_i2c_init(hl_i2c_t id, const hl_i2c_cfg_t *cfg);


/** @fn			extern hl_bool_t hl_i2c_initted_is(hl_i2c_t id)
    @brief  	this function tells if an i2c port has already been initted
    @param  	id              the id of i2c
    @return 	hl_true or hl_false
  */
extern hl_bool_t hl_i2c_initted_is(hl_i2c_t id);


/** @fn			extern hl_result_t hl_i2c_ping(hl_i2c_t id, hl_i2c_devaddr_t devaddr)
    @brief  	this function is used to verify if a device with a given address is present on the bus.
    @param  	id              the id of i2c
    @param  	devaddr 	    the address of the device
    @return 	hl_res_NOK_generic on error or if the device does not respond, else hl_res_OK
  */
extern hl_result_t hl_i2c_ping(hl_i2c_t id, hl_i2c_devaddr_t devaddr);


/** @fn			extern hl_result_t hl_i2c_read(hl_i2c_t id, hl_i2c_devaddr_t devaddr, hl_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
    @brief  	this function is a typical reading transaction, where the master wants to read @e size bytes from the register @e regaddr of the device with
                address @e devaddr.
    @details    this function can be sustituted with some transaction calls. See internals for details.    
    @param  	id              the id of i2c
    @param  	devaddr 	    the address of the device
    @param  	regaddr 	    the address of register
    @param  	data            pointer holding the data to read
    @param  	size            teh size of data to read.
    @return 	hl_res_NOK_generic on error else hl_res_OK
  */
extern hl_result_t hl_i2c_read(hl_i2c_t id, hl_i2c_devaddr_t devaddr, hl_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size);


/** @fn			extern hl_result_t hl_i2c_write(hl_i2c_t id, hl_i2c_devaddr_t devaddr, hl_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
    @brief  	this function is a typical writing transaction, where the master wants to write @e size bytes inside the register @e regaddr of the device with
                address @e devaddr.
    @details    this function can be sustituted with some transaction calls. See internals for details.    
    @param  	id              the id of i2c
    @param  	devaddr 	    the address of the device
    @param  	regaddr 	    the address of register
    @param  	data            pointer holding the data to write
    @param  	size            teh size of data to write.
    @return 	hl_res_NOK_generic on error else hl_res_OK
  */
extern hl_result_t hl_i2c_write(hl_i2c_t id, hl_i2c_devaddr_t devaddr, hl_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size);


/** @fn			extern hl_result_t hl_i2c_is_device_ready(hl_i2c_t id, hl_i2c_devaddr_t devaddr)
    @brief  	this function is used to wait for a device (an EEPROM) until it is ready again after it has received data. 
                Basically, it sends a START followed by the address of the device. Then it waits an ACK or NACK from it
                for some time. If it receives an ACK it returns hl_res_OK to tell that the device is ready. If it returns
                hl_res_NOK_generic it may be that teh device is not ready yet and some more calls to hl_i2c_is_device_ready()
                are required.  This function is specific for EEPROM use.
    @param  	id              the id of i2c
    @param  	devaddr 	    the address of the device
    @return 	hl_res_OK if teh device is ready, hl_res_NOK_generic if not. 
  */
extern hl_result_t hl_i2c_is_device_ready(hl_i2c_t id, hl_i2c_devaddr_t devaddr);


/** @}            
    end of group doxy_group_hl_i2c  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



