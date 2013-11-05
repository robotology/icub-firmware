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


typedef enum
{
    hl_i2c_speed_100kbps       = 1,
    hl_i2c_speed_200kbps       = 2,
    hl_i2c_speed_400kbps       = 4
} hl_i2c_speed_t;


typedef enum
{
    hl_i2c_mode_master         = 0,
    hl_i2c_mode_slave          = 1     // not supported yet
} hl_i2c_mode_t;


typedef enum
{
    hl_i2c_behaviour_polling    = 0,
    hl_i2c_behaviour_irq        = 1,    // not supported yet
    hl_i2c_behaviour_dma        = 2     // not supported yet
} hl_i2c_behaviour_t;


typedef struct 
{
    uint8_t     numofbytes;         // only 1, 2, 3
    union
    {
        uint8_t     one;
        uint16_t    two;  
        uint8_t     three[3];        
    } bytes;
} hl_i2c_regaddr_t;

typedef uint8_t hl_i2c_devaddr_t;


/** @typedef    typedef enum hl_i2c_cfg_t 
    @brief      hl_i2c_cfg_t contains the configuration for i2c.
                there are some hw configuration which are hidden: dutycycle mode is tlow/thigh = 2, the ack is enabled.
                if slave mode (not supported yet) the ack address is 7 bit.
 **/
typedef struct
{
    hl_i2c_mode_t           mode;
    hl_i2c_behaviour_t      behaviour;
    hl_i2c_speed_t          speed;  
    hl_i2c_devaddr_t        ownaddress; // used only if slave mode
} hl_i2c_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hl_i2c_cfg_t hl_i2c_cfg_default; // = { .mode = hl_i2c_mode_master, .behaviour = hl_i2c_behaviour_polling, .speed = hl_i2c_speed_400kbps, .ownaddress = 0 };


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn			extern hl_result_t hl_i2c_init(hl_i2c_t id, const hl_i2c_cfg_t *cfg)
    @brief  	this function initializes an i2c port
    @param  	id              the id of i2c
    @param  	cfg 	        pointer to configuration data
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


/** @fn			extern hl_result_t hl_i2c_standby(hl_i2c_t id, hl_i2c_devaddr_t devaddr)
    @brief  	this function is used to put in standby a device after it has sent data to the master. Basically, it sends a START followed by the address of the
                device. Then it waits an ACK or NACK from it. It repeats this cycle until an ACK is received. This function is specific for EEPROM use.
    @param  	id              the id of i2c
    @param  	devaddr 	    the address of the device
    @return 	hl_res_NOK_generic on error else hl_res_OK
  */
extern hl_result_t hl_i2c_standby(hl_i2c_t id, hl_i2c_devaddr_t devaddr);


/** @}            
    end of group doxy_group_hl_i2c  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



