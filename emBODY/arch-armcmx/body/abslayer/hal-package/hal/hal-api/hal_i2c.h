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
  * @brief  	This function initializes i2c interface
  * @param  	i2cx	        I2C port
  * @param  	cfg 	        pointer to configuration data
  * @return 	hal_res_NOK_generic on error else hal_res_OK
  * @warning    this function is used just for ... the eeprom ... thus shoudl not be used by the user
  */
extern hal_result_t hal_i2c_init(hal_i2c_port_t port, const hal_i2c_cfg_t *cfg);

extern hal_result_t hal_i2c_transaction_begin(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr);

extern hal_result_t hal_i2c_transaction_transmit(hal_i2c_port_t port, uint8_t* data, uint16_t size, hal_bool_t sendstart, hal_bool_t sendstop);

extern hal_result_t hal_i2c_transaction_receive(hal_i2c_port_t port, uint8_t* data, uint16_t size, hal_bool_t sendstart, hal_bool_t sendnack, hal_bool_t sendstop);

// ??
extern hal_result_t hal_i2c_transaction_stop(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr);

extern hal_result_t hal_i2c_transaction_end(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr);


extern hal_result_t hal_i2c_read(hal_i2c_port_t port, uint8_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size);

extern hal_result_t hal_i2c_write(hal_i2c_port_t port, uint8_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size);





/** @}            
    end of group arm_hal_i2c  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



