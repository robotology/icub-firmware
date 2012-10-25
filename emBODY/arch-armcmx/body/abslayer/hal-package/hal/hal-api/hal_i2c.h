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

/** @defgroup arm_hal_i2c HAL I2C

    The HAL I2C is a high level module whcih manages i2c communication
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hal_i2c_port_t 
    @brief      hal_i2c_port_t contains the possible I2C ports 
 **/
typedef enum
{
    hal_i2c_port1 = 0,
    hal_i2c_port2 = 1,
    hal_i2c_port3 = 2
} hal_i2c_port_t;

enum { hal_i2c_ports_number = 3 };


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
    hal_bool_t              usedma;     // not supported yet 
} hal_i2c_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_i2c_cfg_t hal_i2c_cfg_default; // = { .mode = hal_i2c_mode_master, .speed = hal_i2c_speed_400kbps, .ownaddress = 0, .usedma = hal_false };


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn			extern hal_result_t hal_i2c_init(hal_i2c_port_t port, const hal_i2c_cfg_t *cfg)
    @brief  	this function initializes an i2c port
    @param  	port	        the port
    @param  	cfg 	        pointer to configuration data
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_i2c_init(hal_i2c_port_t port, const hal_i2c_cfg_t *cfg);

/** @fn			extern hal_result_t hal_i2c_transaction_begin(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr)
    @brief  	this function begins a transaction: locks the bus for other attempts of hal_i2c_transaction_begin(), sends a START, write 
                the address of the device to talk to.
    @param  	port	        the port
    @param  	devaddr 	    the address of the device used for this transaction
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_i2c_transaction_begin(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr);

/** @fn			extern hal_result_t hal_i2c_transaction_transmit(hal_i2c_port_t port, uint8_t* data, uint16_t size, hal_bool_t sendstart, hal_bool_t sendstop)
    @brief  	this function transmits data inside a transaction: if sendstart is hal_true it sends a START and the address of the device with write flag, 
                sends to the device size bytes, and if sendstop is hal_true it sends a STOP condition. 
    @param  	port	        the port
    @param  	data	        the data to write
    @param      size            the size of data to write
    @param      sendstart       if hal_true a START is sent before sending data
    @param      sendstop        if hal_true a STOP is sent after having sent the data
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_i2c_transaction_transmit(hal_i2c_port_t port, uint8_t* data, uint16_t size, hal_bool_t sendstart, hal_bool_t sendstop);

/** @fn			extern hal_result_t hal_i2c_transaction_receive(hal_i2c_port_t port, uint8_t* data, uint16_t size, hal_bool_t sendstart, hal_bool_t sendnack, hal_bool_t sendstop)
    @brief  	this function receives data inside a transaction: if sendstart is hal_true it sends a START and the address of the device with read flag, 
                receives from the device size bytes, if sendnack is hal_true it does not ack last received byte, and finally if sendstop is hal_true it sends a STOP condition. 
    @param  	port	        the port
    @param  	data	        the data to write
    @param      size            the size of data to write
    @param      sendstart       if hal_true a START is sent before receiving data
    @param      sendnack        if hal_true the ACK is not sent back to the device after last reading, so that the device stops transmitting
    @param      sendstop        if hal_true a STOP is sent after having received the data
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_i2c_transaction_receive(hal_i2c_port_t port, uint8_t* data, uint16_t size, hal_bool_t sendstart, hal_bool_t sendnack, hal_bool_t sendstop);

//extern hal_result_t hal_i2c_transaction_stop(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr);

/** @fn			extern hal_result_t hal_i2c_transaction_ends(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr)
    @brief  	this function ends a transaction: unlocks the bus to allow others to use it
    @param  	port	        the port
    @param  	devaddr 	    the address of the device used for the transaction
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_i2c_transaction_end(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr);


/** @fn			extern hal_result_t hal_i2c_read(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
    @brief  	this function is a typical reading transaction, where the master wants to read @e size bytes from the register @e regaddr of the device with
                address @e devaddr.
    @details    this function can be sustituted with some transaction calls. See internals for details.    
    @param  	port	        the port
    @param  	devaddr 	    the address of the device
    @param  	regaddr 	    the address of register
    @param  	data            pointer holding the data to read
    @param  	size            teh size of data to read.
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_i2c_read(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size);


/** @fn			extern hal_result_t hal_i2c_write(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
    @brief  	this function is a typical writing transaction, where the master wants to write @e size bytes inside the register @e regaddr of the device with
                address @e devaddr.
    @details    this function can be sustituted with some transaction calls. See internals for details.    
    @param  	port	        the port
    @param  	devaddr 	    the address of the device
    @param  	regaddr 	    the address of register
    @param  	data            pointer holding the data to write
    @param  	size            teh size of data to write.
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_i2c_write(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size);


/** @fn			extern hal_result_t hal_i2c_standby(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr)
    @brief  	this function is used to put in standby a device after it has sent data to teh master. Basically, it sends a START followed by the address of the
                device. Then it waits an ACK or NACK from it. It repeats this cycle until an ACK is received. This function is specific for EEPROM use.
    @param  	port	        the port
    @param  	devaddr 	    the address of the device
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_i2c_standby(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr);



/** @}            
    end of group arm_hal_i2c  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



