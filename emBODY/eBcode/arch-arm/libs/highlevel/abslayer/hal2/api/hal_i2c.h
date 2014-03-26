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

#ifndef _HAL_I2C_H_
#define _HAL_I2C_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_i2c.h
    @brief      This header file implements interface to a generic hal i2c module
    @author     marco.accame@iit.it, valentina.gagegro@iit.it
    @date       10/23/2012
**/

/** @defgroup doxy_group_hal_i2c HAL I2C

    The HAL I2C is a high level module whcih manages i2c communication
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hal_i2c_t 
    @brief      hal_i2c_t contains the possible I2C ports 
 **/
typedef enum
{
    hal_i2c1 = 0,
    hal_i2c2 = 1,
    hal_i2c3 = 2
} hal_i2c_t;

enum { hal_i2cs_number = 3 };


typedef enum
{
    hal_i2c_speed_100kbps       = 1,
    hal_i2c_speed_200kbps       = 2,
    hal_i2c_speed_400kbps       = 4
} hal_i2c_speed_t;


typedef enum
{
    hal_i2c_mode_master         = 0,
    hal_i2c_mode_slave          = 1     // not supported yet
} hal_i2c_mode_t;


typedef struct 
{
    uint8_t     numofbytes;         // only 1, 2, 3
    union
    {
        uint8_t     three[3];
        uint16_t    two;
        uint8_t     one;
    } bytes;
} hal_i2c_regaddr_t;

typedef uint8_t hal_i2c_devaddr_t;


/** @typedef    typedef enum hal_i2c_cfg_t 
    @brief      hal_i2c_cfg_t contains the configuration for i2c.
                there are some hw configuration which are hidden: dutycycle mode is tlow/thigh = 2, the ack is enabled.
                if slave mode (not supported yet) the ack address is 7 bit.
 **/
typedef struct
{
    hal_i2c_mode_t          mode;
    hal_i2c_speed_t         speed;  
    hal_i2c_devaddr_t       ownaddress; // used only if slave mode
} hal_i2c_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_i2c_cfg_t hal_i2c_cfg_default; // = { .mode = hal_i2c_mode_master, .speed = hal_i2c_speed_400kbps, .ownaddress = 0 };


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn			extern hal_result_t hal_i2c_init(hal_i2c_t id, const hal_i2c_cfg_t *cfg)
    @brief  	this function initializes an i2c port
    @param  	id              the id of i2c
    @param  	cfg 	        pointer to configuration data
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_i2c_init(hal_i2c_t id, const hal_i2c_cfg_t *cfg);


/** @fn			extern hal_bool_t hal_i2c_initted_is(hal_i2c_t id)
    @brief  	this function tells if an i2c port has already been initted
    @param  	id              the id of i2c
    @return 	hal_true or hal_false
  */
extern hal_bool_t hal_i2c_initted_is(hal_i2c_t id);


/** @fn			extern hal_result_t hal_i2c_ping(hal_i2c_t id, hal_i2c_devaddr_t devaddr)
    @brief  	this function is used to verify if a device with a given address is present on the bus.
    @param  	id              the id of i2c
    @param  	devaddr 	    the address of the device
    @return 	hal_res_NOK_generic on error or if the device does not respond, else hal_res_OK
  */
extern hal_result_t hal_i2c_ping(hal_i2c_t id, hal_i2c_devaddr_t devaddr);


/** @fn			extern hal_result_t hal_i2c_read(hal_i2c_t id, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
    @brief  	this function is a typical reading transaction, where the master wants to read @e size bytes from the register @e regaddr of the device with
                address @e devaddr.
    @details    this function can be sustituted with some transaction calls. See internals for details.    
    @param  	id              the id of i2c
    @param  	devaddr 	    the address of the device
    @param  	regaddr 	    the address of register
    @param  	data            pointer holding the data to read
    @param  	size            teh size of data to read.
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_i2c_read(hal_i2c_t id, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size);


/** @fn			extern hal_result_t hal_i2c_write(hal_i2c_t id, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
    @brief  	this function is a typical writing transaction, where the master wants to write @e size bytes inside the register @e regaddr of the device with
                address @e devaddr.
    @details    this function can be sustituted with some transaction calls. See internals for details.    
    @param  	id              the id of i2c
    @param  	devaddr 	    the address of the device
    @param  	regaddr 	    the address of register
    @param  	data            pointer holding the data to write
    @param  	size            teh size of data to write.
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_i2c_write(hal_i2c_t id, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size);


/** @fn			extern hal_result_t hal_i2c_standby(hal_i2c_t id, hal_i2c_devaddr_t devaddr)
    @brief  	this function is used to put in standby a device after it has sent data to teh master. Basically, it sends a START followed by the address of the
                device. Then it waits an ACK or NACK from it. It repeats this cycle until an ACK is received. This function is specific for EEPROM use.
    @param  	id              the id of i2c
    @param  	devaddr 	    the address of the device
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_i2c_standby(hal_i2c_t id, hal_i2c_devaddr_t devaddr);



/** @}            
    end of group doxy_group_hal_i2c  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



