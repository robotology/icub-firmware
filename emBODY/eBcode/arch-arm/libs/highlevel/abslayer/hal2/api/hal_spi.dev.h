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

    The HAL SPI is a high level module which manages spi communication
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hal_spi_t 
    @brief      hal_spi_t contains the possible SPI ports 
 **/
typedef enum
{
    hal_spi1 = 0,
    hal_spi2 = 1,
    hal_spi3 = 2
} hal_spi_t;

enum { hal_spis_number = 3 };


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
    hal_spi_act_raw             = 0,    /**< the communication is done using raw bytes. the spi can be master only */
    hal_spi_act_framebased      = 1     /**< the communication is done by frames */
} hal_spi_activity_t;

/** @typedef    typedef enum hal_spi_prescaler_t
    @brief      contains prescaler to be applied to fast sys bus to achieve the spi speed.
 **/
typedef enum
{
    hal_spi_prescaler_auto      =   0,                  /**<  */
    hal_spi_prescaler_002       =   1,
    hal_spi_prescaler_004       =   2,
    hal_spi_prescaler_008       =   3,
    hal_spi_prescaler_016       =   4,
    hal_spi_prescaler_032       =   5,
    hal_spi_prescaler_064       =   6,
    hal_spi_prescaler_128       =   7,
    hal_spi_prescaler_256       =   8
} hal_spi_prescaler_t;


typedef enum
{
    hal_spi_cpolarity_low             = 0,
    hal_spi_cpolarity_high      			= 1
} hal_spi_cpolarity_t;


/** @typedef    typedef enum hal_spi_cfg_t 
    @brief      hal_spi_cfg_t contains the configuration for spi
 **/
typedef struct
{
    hal_spi_ownership_t     ownership;          /**< the communication ownership: master or slave */
    hal_spi_direction_t     direction;          /**< the communication direction: tx, rx or both */
    hal_spi_activity_t      activity;           /**< the activity: raw or frame-based */
    hal_spi_prescaler_t     prescaler;          /**< if hal_spi_prescaler_auto then use .maxspeed to determine the correct prescaler, otherwise use what is specified */
    uint32_t                maxspeed;           /**< used only in case of hal_spi_prescaler_auto. it is the max spi speed in Hz that can be reached */
    uint8_t                 sizeofframe;        /**< the size of the frame in the frame-based activity */
    uint8_t                 capacityoftxfifoofframes; /**< if direction is not hal_spi_dir_rxonly, it specifies the capacity of the fifo of frames to tx */
    uint8_t                 capacityofrxfifoofframes; /**< if direction is not hal_spi_dir_txonly, it specifies the capacity of the fifo of frames to rx */
    uint8_t                 dummytxvalue;       /**< it specifies which is the value to transmit in case the fifo is empty or in case direction is hal_spi_dir_rxonly */
		uint8_t                 starttxvalue;	
    hal_callback_t          onframetransm;      /**< if not NULL and direction is not hal_spi_dir_rxonly it is called by the ISR when a frame is transmitted */
    void*                   argonframetransm;
    hal_callback_t          onframereceiv;      /**< if not NULL and direction is not hal_spi_dir_txonly it is called by the ISR when a frame is received */
    void*                   argonframereceiv;
		hal_spi_cpolarity_t			cpolarity;					// if 0 SPI_CPOL_Low, if 1 SPI_CPOL_High
} hal_spi_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_spi_cfg_t hal_spi_cfg_default; /**< = { .ownership = hal_spi_ownership_master, .direction = hal_spi_dir_rxonly, .activity = hal_spi_act_framebased,
                                                         .prescaler = hal_spi_prescaler_64, .maxspeed = 0, .sizeofframe = 4, 
                                                         .capacityoftxfifoofframes = 0, .capacityofrxfifoofframes = 2, .dummytxvalue = 0, 
                                                         .onframetransm = NULL, .argonframetransm = NULL, .onframereceiv = NULL, .argonframereceiv = NULL };
                                                 */

// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn			extern uint32_t hal_spi_speedofbus_get(hal_spi_t id)
    @brief  	this function tells what is the internal bus speed of an spi id. it can be used to chose the right value of the
                prescaler to be assigned inside the hal_spi_cfg_t argument to be passed to hal_spi_init().
    @param  	id 	    the id
    @return 	the speed of the internal bus or 0 if the specified spi is not supported
  */
extern uint32_t hal_spi_speedofbus_get(hal_spi_t id);


/** @fn			extern hal_result_t hal_spi_init(hal_spi_t id, const hal_spi_cfg_t *cfg)
    @brief  	this function initializes an spi id
    @param  	id	        the id
    @param  	cfg 	    the configuration
    @return 	hal_res_OK or hal_res_NOK_generic on failure
    @warning    a given SPI id can be configured only once. The second call of hal_spi_init(cfg) will do nothing.
                The return value will be hal_res_OK if the parameter cfg is the same as one used the first time,
                otherwise it will be hal_res_NOK_generic to express the fact that the CAN is not initted as wanted.    
  */
extern hal_result_t hal_spi_init(hal_spi_t id, const hal_spi_cfg_t *cfg);


/** @fn			extern hal_boolval_t hal_spi_initted_is(hal_spi_t id)
    @brief  	this function tells if the spi is initted
    @param  	id	        the id
    @return 	hal_true or hal_false
  */
extern hal_boolval_t hal_spi_initted_is(hal_spi_t id);


/** @fn			extern hal_result_t hal_spi_start(hal_spi_t id, uint8_t numberofframes)
    @brief  	this function starts communication. in case of hal_spi_ownership_master, hal_spi_dir_rxonly, and hal_spi_act_framebased
                the function sends numberofframes dummy frames filled with values specified by cfg->dummytxvalue. upon reception of a
                frame from the slave, it stores it inside its internal fifo and triggers the cfg->onframereceiv(arg) callback.
                when all the numberofframes frames are received, then the spi is automatically stopped.
    @param  	id	            the id
    @param  	numberofframes   the number of frames to manage (FOR FUTURE USE. NOW IT IS IS ALWAYS 1)
    @return 	hal_res_OK or hal_res_NOK_generic on failure
  */
extern hal_result_t hal_spi_start(hal_spi_t id, uint8_t numberofframes);


/** @fn			extern hal_bool_t hal_spi_active_is(hal_spi_t id)
    @brief  	this function tells if the spi is active. 
    @param  	id	            the id
    @return 	hal_true if there is communication activity, hal_false if it has finished transferring data or spi was not initted yet.
  */
extern hal_bool_t hal_spi_active_is(hal_spi_t id);


/** @fn			extern hal_result_t hal_spi_get(hal_spi_t id, uint8_t* rxframe, uint8_t* remainingrxframes)
    @brief  	this function retrieves a received frame. it can be called when all the frames have been received and thus hal_spi_active_is() returns
                hal_false. However, it can also be called inside the cfg->onframereceiv(arg) callback after the reception of each frame.
    @param  	id	                the id
    @param  	rxframe             the received frame
    @param  	remainingrxframes   if not NULL, it is filled with the number of frames remaining in the internal fifo.
    @return 	hal_res_OK if a valid frame is available, hal_res_NOK_nodata if no frame is available, or hal_res_NOK_generic on failure
  */
extern hal_result_t hal_spi_get(hal_spi_t id, uint8_t* rxframe, uint8_t* remainingrxframes);


/** @fn			extern hal_result_t hal_spi_on_framereceiv_set(hal_spi_t id, hal_callback_t onframereceiv, void* arg)
    @brief  	this function allows changing the cfg->onframereceiv(arg) callback originally configured at hal_spi_init().
    @param  	id	                the id
    @param  	onframereceiv       the callback (it can be NULL).
    @param  	arg                 its argument
    @return 	hal_res_OK if spi is initted, hal_res_NOK_generic on failure
  */
extern hal_result_t hal_spi_on_framereceiv_set(hal_spi_t id, hal_callback_t onframereceiv, void* arg); 

extern hal_result_t hal_spi_rx_isr_enable(hal_spi_t id);
extern hal_result_t hal_spi_rx_isr_disable(hal_spi_t id);
extern hal_result_t hal_spi_periph_enable(hal_spi_t id);
extern hal_result_t hal_spi_periph_disable(hal_spi_t id);
extern hal_result_t hal_spi_set_isrtxframe(hal_spi_t id, const uint8_t* txframe);
extern hal_result_t hal_spi_set_sizeofframe(hal_spi_t id, uint8_t framesize);

//Test to read a register for the encoder
/*
extern hal_result_t hal_spi_read_reg_init(hal_spi_t id, char address);
extern hal_result_t hal_spi_read_reg_execute(hal_spi_t id);
*/
/** @}            
    end of group doxy_group_hal_spi  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



