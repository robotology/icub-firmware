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
    hal_spi_act_raw             = 0,    /**< the communication is done using raw bytes. the spi can be slave only */
    hal_spi_act_framebased      = 1     /**< the communication is done by frames */
} hal_spi_activity_t;


/** @typedef    typedef enum hal_spi_speed_t
    @brief      contains speed of the communication channel 
 **/
typedef enum
{
    hal_spi_speed_dontuse           =   0,              /**< use the prescaler */
    hal_spi_speed_0562kbps          =   562500,         /**< valid only for fast sys bus @ 72mhz */
    hal_spi_speed_1125kbps          =  1125000,         /**< valid only for fast sys bus @ 72mhz */
    hal_spi_speed_2250kbps          =  2250000,         /**< valid only for fast sys bus @ 72mhz */
    hal_spi_speed_4500kbps          =  4500000,         /**< valid only for fast sys bus @ 72mhz */
    hal_spi_speed_9000kbps          =  9000000,         /**< valid only for fast sys bus @ 72mhz */
    hal_spi_speed_18000kbps         = 18000000,         /**< valid only for fast sys bus @ 72mhz */
    hal_spi_speed_0652kbps          =   652250,         /**< valid only for fast sys bus @ 84mhz */
    hal_spi_speed_1312kbps          =  1312500,         /**< valid only for fast sys bus @ 84mhz */
    hal_spi_speed_2625kbps          =  2625000,         /**< valid only for fast sys bus @ 84mhz */
    hal_spi_speed_5250kbps          =  5250000,         /**< valid only for fast sys bus @ 84mhz */
    hal_spi_speed_10500kbps         = 10500000,         /**< valid only for fast sys bus @ 84mhz */
    hal_spi_speed_21000kbps         = 21000000,         /**< valid only for fast sys bus @ 84mhz */
} hal_spi_speed_t;



/** @typedef    typedef enum hal_spi_prescaler_t
    @brief      contains prescaler to be applied to fast sys bus to achieve the spi speed.
 **/
typedef enum
{
    hal_spi_prescaler_dontuse   =   0,                  /**< if prescaler is not used, then internals use the hal_spi_speed_t instead */
//    hal_spi_prescaler_002       =   1,
    hal_spi_prescaler_004       =   2,
    hal_spi_prescaler_008       =   3,
    hal_spi_prescaler_016       =   4,
    hal_spi_prescaler_032       =   5,
    hal_spi_prescaler_064       =   6,
    hal_spi_prescaler_128       =   7,
    hal_spi_prescaler_256       =   8
} hal_spi_prescaler_t;


/** @typedef    typedef enum hal_spi_cfg_t 
    @brief      hal_spi_cfg_t contains the configuration for spi
 **/
typedef struct
{
    hal_spi_ownership_t     ownership;          /**< the communication ownership: master or slave */
    hal_spi_direction_t     direction;          /**< the communication direction: tx, rx or both */
    hal_spi_activity_t      activity;           /**< the activity: single frame, multi frame, or continuous */
    hal_spi_prescaler_t     prescaler;
    hal_spi_speed_t         speed;              /**< the communication speed */
    uint8_t                 sizeofframe;
    uint8_t                 capacityoftxfifoofframes; /**< if direction is not hal_spi_dir_rxonly, it specifies the capacity of the fifo of frames to tx */
    uint8_t                 capacityofrxfifoofframes; /**< if direction is not hal_spi_dir_txonly, it specifies the capacity of the fifo of frames to rx */
    uint8_t                 dummytxvalue;       /**< it specifies which is the value to transmit in case the fifo is empty or in case direction is hal_spi_dir_rxonly */ 
    hal_callback_t          onframetransm;      /**< if not NULL and direction is not hal_spi_dir_rxonly it is called when a frame is transmitted */
    void*                   argonframetransm;
    hal_callback_t          onframereceiv;      /**< if not NULL and direction is not hal_spi_dir_txonly it is called when a frame is received */
    void*                   argonframereceiv;
} hal_spi_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_spi_cfg_t hal_spi_cfg_default; /**< = {.ownership = hal_spi_ownership_master, .dir = hal_spi_dir_rxonly, .activity = hal_spi_act_framebased,
                                                        .prescaler = hal_spi_prescaler_128, .speed = hal_spi_speed_dontuse, .sizeofframe = 4, 
                                                        .capacityoftxfifoofframes = 0, .capacityofrxfifoofframes = 2, .dummytxvalue = 0, 
                                                        .onframetransm = NULL, .argonframetransm = NULL, .argonframereceiv = NULL, .onframereceiv = NULL};
                                                 */

// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn			extern hal_result_t hal_spi_init(hal_spi_port_t port, const hal_spi_cfg_t *cfg)
    @brief  	this function initializes an spi port
    @param  	port	        the port
    @param  	cfg 	        pointer to configuration data
    @return 	hal_res_OK or hal_res_NOK_generic on failure
  */
extern hal_result_t hal_spi_init(hal_spi_port_t port, const hal_spi_cfg_t *cfg);


extern hal_result_t hal_spi_raw_master_writeread(hal_spi_port_t port, uint8_t byte, uint8_t* readbyte);


/** @fn			extern hal_result_t hal_spi_send(hal_spi_port_t port, uint8_t* txframe, uint8_t size)
    @brief  	this functionj adds the frame to the transmission buffer. in master ownership if sendnow is hal_true it also starts 
                transmission. 
                if the port is configured having activity hal_spi_act_continuous, then the sendnow enables or disables the activity.
                the txframe, if its size is not zero, is sent as soon as a new frame starts. otherwise, frames are taken from the 
                txfifoofframes or are just zero data.
    @param  	port	        the port
    @param  	txframe 	    the frame to transmit
    @return 	hal_res_OK or hal_res_NOK_generic on failure
  */
extern hal_result_t hal_spi_put(hal_spi_port_t port, uint8_t* txframe);

// 0 -> svuoto il buffer tx.
// num -> il numeo indicato. si prende prima dal buffer tx se esiste. poi il frame dummy.
// 255 -> lunghezza infinita. si usa per lo slave o per trasmettere continuamente. si ferma con hal_spi_stop().
extern hal_result_t hal_spi_start(hal_spi_port_t port, uint8_t lengthofburst);

extern hal_result_t hal_spi_stop(hal_spi_port_t port);


/** @fn			extern hal_result_t hal_spi_get(hal_spi_port_t port, uint8_t* size)
    @brief  	this function retrieves a received frame. 
    @param  	port	        the port
    @param  	rxframe 	    the frame to transmit
    @return 	hal_res_OK if a valid frame is available or hal_res_NOK_generic on failure
  */
extern hal_result_t hal_spi_get(hal_spi_port_t port, uint8_t* rxframe, uint8_t* remainingrxframes);



extern hal_result_t hal_spi_on_framereceiv_set(hal_spi_port_t port, hal_callback_t onframereceiv, void* arg); 

extern hal_result_t hal_spi_on_frametransm_set(hal_spi_port_t port, hal_callback_t onframetransm, void* arg); 

/** @}            
    end of group doxy_group_hal_spi  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



