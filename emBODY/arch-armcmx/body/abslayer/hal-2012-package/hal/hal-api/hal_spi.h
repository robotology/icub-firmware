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

#ifndef _HAL_SPI_H_
#define _HAL_SPI_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_spi.h
    @brief      This header file implements interface to a generic hal spi module
    @author     marco.accame@iit.it
    @date       10/29/2012
**/

/** @defgroup doxy_group_hal_spi HAL SPI

    The HAL SPI is a high level module whcih manages spi communication
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hal_spi_port_t 
    @brief      hal_spi_port_t contains the possible SPI ports 
 **/
typedef enum
{
    hal_spi_port1 = 0,
    hal_spi_port2 = 1,
    hal_spi_port3 = 2
} hal_spi_port_t;

enum { hal_spi_ports_number = 3 };


/** @typedef    typedef enum hal_spi_mode_t
    @brief      contains the modes with respect to responsibility of initiating the communication 
 **/
typedef enum
{
    hal_spi_ownership_master        = 0,    /**< it can initiate the communication */
    hal_spi_ownership_slave         = 1     /**< it cannot initiate the communication and must wait for the master initiative */    
} hal_spi_ownership_t;


/** @typedef    typedef enum hal_spi_direction_t 
    @brief      contains the direction mode of the communication channel 
 **/
typedef enum
{
    hal_spi_dir_txrx            = 0,    /**< the channel is able to transmit and receive at the same time */
    hal_spi_dir_txonly          = 1,    /**< the channel is able to transmit only */
    hal_spi_dir_rxonly          = 2     /**< the channel is able to receive only */  
} hal_spi_direction_t;


/** @typedef    typedef enum hal_spi_activity_t 
    @brief      contains the direction mode of the communication channel 
 **/
typedef enum
{
    hal_spi_act_raw08bits       = 2,    /**<  */
    hal_spi_act_singleframe     = 0,    /**< the channel is enabled to manage one frame only and after that it is automatically disabled */
    hal_spi_act_continuous      = 1     /**< NOT SUPPORTED NOW: after the channel is enabled, it exchanges frames, possibly empty, until it is disabled */
} hal_spi_activity_t;


/** @typedef    typedef enum hal_spi_speed_t
    @brief      contains speed of the communication channel 
 **/
typedef enum
{
    hal_spi_speed_500kbps       = 5,
    hal_spi_speed_1mbps         = 10,
    hal_spi_speed_2mbps         = 20,
    hal_spi_speed_4mbps         = 40,
    hal_spi_speed_8mbps         = 80,
    hal_spi_speed_9mbps         = 90,
    hal_spi_speed_18mbps        = 180
} hal_spi_speed_t;


/** @typedef    typedef enum hal_spi_cfg_t 
    @brief      hal_spi_cfg_t contains the configuration for spi
 **/
typedef struct
{
    hal_spi_ownership_t     ownership;          /**< the communication ownership: master or slave */
    hal_spi_direction_t     dir;                /**< the communication direction: tx, rx or both */
    hal_spi_activity_t      activity;           /**< the activity: single frame or continuous */
    hal_spi_speed_t         speed;              /**< the communication speed */
    uint8_t                 sizeoftxframe;      /**< if tx is enables, it tells how many bytes to transmit inside a frame */
    uint8_t                 sizeofrxframe;      /**< if rx is enables, it tells how many bytes to receive inside a frame */
    hal_callback_t          onframetransm;      /**< if not NULL, it is called when a frame is transmitted. its argument is NULL */
    hal_callback_t          onframereceiv;      /**< if not NULL, it is called when a frame is received. its argument points to the received frame */
    uint8_t                 sizeoftxfifoofframes; //?? si potrebbe bufferizzare le trasmissioni e le ricezioni .....
    uint8_t                 sizeofrxfifoofframes;
} hal_spi_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_spi_cfg_t hal_spi_cfg_default; // = { .ownership = hal_spi_ownership_master, .dir = hal_spi_dir_txrx, .activity = hal_spi_act_singleframe,
                                                //     .speed = hal_spi_speed_1mbps, .sizeoftxframe = 4, .sizeofrxframe = 4, .onframetransm = NULL, .onframereceiv = NULL };


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn			extern hal_result_t hal_spi_init(hal_spi_port_t port, const hal_spi_cfg_t *cfg)
    @brief  	this function initializes an spi port
    @param  	port	        the port
    @param  	cfg 	        pointer to configuration data
    @return 	hal_res_OK or hal_res_NOK_generic on failure
  */
extern hal_result_t hal_spi_init(hal_spi_port_t port, const hal_spi_cfg_t *cfg);

extern void hal_test_spima_spisl(void);


extern hal_result_t hal_spi_write(hal_spi_port_t port, uint8_t byte, uint8_t* readbyte);
extern hal_result_t hal_spi_read(hal_spi_port_t port, uint8_t* byte);


/** @fn			extern hal_result_t hal_spi_send(hal_spi_port_t port, uint8_t* txframe, uint8_t size)
    @brief  	this functionj adds the frame to the transmission buffer. in master ownership if sendnow is hal_true it also starts 
                transmission. 
                if the port is configured having activity hal_spi_act_continuous, then the sendnow enables or disables the activity.
                the txframe, if its size is not zero, is sent as soon as a new frame starts. otherwise, frames are taken from the 
                txfifoofframes or are just zero data.
    @param  	port	        the port
    @param  	txframe 	    the frame to transmit
    @param  	size 	        the size of the frame
    @return 	hal_res_OK or hal_res_NOK_generic on failure
  */
extern hal_result_t hal_spi_put(hal_spi_port_t port, uint8_t* txframe, uint8_t size, hal_bool_t sendnow);

extern hal_result_t hal_spi_start(hal_spi_port_t port);

extern hal_result_t hal_spi_stop(hal_spi_port_t port);

// add a hal_spi_start() and hal_spi_stop() ???


/** @fn			extern hal_result_t hal_spi_get(hal_spi_port_t port, uint8_t* rxframe, uint8_t* size)
    @brief  	this function retrieves a received frame. 
    @param  	port	        the port
    @param  	rxframe 	    the frame to transmit
    @param  	size 	        the size of the frame
    @return 	hal_res_OK if a valid frame is available or hal_res_NOK_generic on failure
  */
extern hal_result_t hal_spi_get(hal_spi_port_t port, uint8_t* rxframe, uint8_t* size);




/** @}            
    end of group doxy_group_hal_spi  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



