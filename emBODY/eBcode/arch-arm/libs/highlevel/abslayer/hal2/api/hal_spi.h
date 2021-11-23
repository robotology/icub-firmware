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

#ifdef __cplusplus
extern "C" {
#endif

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
    hal_spi_cpolarity_low       = 0,
    hal_spi_cpolarity_high      = 1
} hal_spi_cpolarity_t;


typedef enum
{
    hal_spi_datacapture_1edge   = 0,
    hal_spi_datacapture_2edge   = 1
} hal_spi_datacapture_t;


typedef enum
{
    hal_spi_datasize_8bit       = 0,
    hal_spi_datasize_16bit      = 1
} hal_spi_datasize_t;


typedef enum
{
    hal_spi_gpio_cfg_sckmosi_pullup     = 0,
    hal_spi_gpio_cfg_sckmosi_nopull     = 1
} hal_spi_gpio_cfg_t;



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
    hal_spi_datasize_t      datasize;           /**< the size of the data item */
    uint8_t                 maxsizeofframe;     /**< the max size of the frame in the frame-based activity. the size can be changed (only lowered) in runtime */
    uint8_t                 capacityoftxfifoofframes; /**< if direction is not hal_spi_dir_rxonly, it specifies the capacity of the fifo of frames to tx */
    uint8_t                 capacityofrxfifoofframes; /**< if direction is not hal_spi_dir_txonly, it specifies the capacity of the fifo of frames to rx */
    hal_callback_t          onframestransmitted;    /**< if not NULL and direction is not hal_spi_dir_rxonly it is called by the ISR when all the frames are transmitted */
    void*                   argonframestransmitted;
    hal_callback_t          onframesreceived;      /**< if not NULL and direction is not hal_spi_dir_txonly it is called by the ISR when all the frames are received */
    void*                   argonframesreceived;
    hal_spi_cpolarity_t     cpolarity;    
    hal_spi_datacapture_t   datacapture;
    uint16_t                gpio_cfg_flags;     /* bit 1 assume values defined in hal_spi_gpio_cfg_t (only for AEA and AMO) can be either 0 or 1: if 0 SCK MOSI is pullUP else 1 is NoPull */
} hal_spi_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_spi_cfg_t hal_spi_cfg_default; /**< = { .ownership = hal_spi_ownership_master, .direction = hal_spi_dir_rxonly, .activity = hal_spi_act_framebased,
                                                         .prescaler = hal_spi_prescaler_64, .maxspeed = 0, .datasize = hal_spi_datasize_8bit, .sizeofframe = 4, 
                                                         .capacityoftxfifoofframes = 0, .capacityofrxfifoofframes = 2, .dummytxvalue = 0, 
                                                         .onframetransm = NULL, .argonframetransm = NULL, .onframereceiv = NULL, .argonframereceiv = NULL };
                                                 */

// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern hal_boolval_t hal_spi_supported_is(hal_spi_t id)
    @brief      This function tells if the specified SPI port is supported by the board. 
    @param      id              identifies SPI id 
    @return     hal_true or hal_false
  */
extern hal_boolval_t hal_spi_supported_is(hal_spi_t id);


/** @fn         extern uint32_t hal_spi_speedofbus_get(hal_spi_t id)
    @brief      this function tells what is the internal bus speed of an spi id. it can be used to chose the right value of the
                prescaler to be assigned inside the hal_spi_cfg_t argument to be passed to hal_spi_init().
    @param      id         the id
    @return     the speed of the internal bus or 0 if the specified spi is not supported
  */
extern uint32_t hal_spi_speedofbus_get(hal_spi_t id);


/** @fn         extern hal_result_t hal_spi_init(hal_spi_t id, const hal_spi_cfg_t *cfg)
    @brief      this function initializes an spi id
    @param      id            the id
    @param      cfg         the configuration
    @return     hal_res_OK or hal_res_NOK_generic on failure
    @warning    a given SPI id can be configured only once. The second call of hal_spi_init(cfg) will do nothing.
                The return value will be hal_res_OK if the parameter cfg is the same as one used the first time,
                otherwise it will be hal_res_NOK_generic to express the fact that the CAN is not initted as wanted.    
  */
extern hal_result_t hal_spi_init(hal_spi_t id, const hal_spi_cfg_t *cfg);


/** @fn         extern hal_boolval_t hal_spi_initted_is(hal_spi_t id)
    @brief      this function tells if the spi is initted
    @param      id            the id
    @return     hal_true or hal_false
  */
extern hal_boolval_t hal_spi_initted_is(hal_spi_t id);


/** @fn         extern hal_result_t hal_spi_raw_enable(hal_spi_t id)
    @brief      it enables the spi for raw use. 
                spi must have been initted with hal_spi_init().
    @param      id            the id
    @return     hal_true or hal_false
  */
extern hal_result_t hal_spi_raw_enable(hal_spi_t id);
    
    
/** @fn         extern hal_result_t hal_spi_raw_disable(hal_spi_t id)
    @brief      it disables the spi for raw use. 
                spi must have been initted with hal_spi_init() and enabled with hal_spi_raw_enable().
    @param      id            the id
    @return     hal_true or hal_false
  */    
extern hal_result_t hal_spi_raw_disable(hal_spi_t id);


/** @fn         extern hal_result_t hal_spi_raw_writeread(hal_spi_t id, uint16_t writedata, uint16_t* dataread)
    @brief      it sends on spi a word of size specified by the field hal_spi_cfg_t::datasize and it waits for reception 
                of a word of similar size. 
                spi must have been initted with hal_spi_init() and enabled with hal_spi_raw_enable().
    @param      id            the id
    @param      writedata     the word written on spi. if hal_spi_cfg_t::datasize is hal_spi_datasize_8bit, only its LSB is sent.
    @param      dataread      if non-NULL, it holds the received word. if hal_spi_cfg_t::datasize is hal_spi_datasize_8bit, only its MSB is zero. 
    @return     hal_true or hal_false
  */    
extern hal_result_t hal_spi_raw_writeread(hal_spi_t id, uint16_t writedata, uint16_t* dataread);


/** @fn         extern hal_result_t hal_spi_start(hal_spi_t id, uint8_t numberofframes)
    @brief      this function starts communication. in case of hal_spi_ownership_master, hal_spi_dir_rxonly, and hal_spi_act_framebased
                the function sends numberofframes dummy frames filled with values specified by funtion hal_spi_set_isrtxframe() WHICH
                MUST be called every time. Upon reception of a frame from the slave, it stores it inside its internal fifo.
                when all the numberofframes frames are received,  it triggers  the cfg->onframesreceiv(arg) callback, then the spi is 
                automatically stopped.
    @param      id                      the id
    @param      numberofframes          the number of frames to manage. it cannot be zero otherwise there is a failure
    @return     hal_res_OK or hal_res_NOK_generic on failure
  */
extern hal_result_t hal_spi_start(hal_spi_t id, uint8_t numberofframes);


/** @fn         extern hal_bool_t hal_spi_active_is(hal_spi_t id)
    @brief      this function tells if the spi is active. 
    @param      id                the id
    @return     hal_true if there is communication activity, hal_false if it has finished transferring data or spi was not initted yet.
  */
extern hal_bool_t hal_spi_active_is(hal_spi_t id);


/** @fn         extern hal_result_t hal_spi_get(hal_spi_t id, uint8_t* rxframe, uint8_t* remainingrxframes)
    @brief      this function retrieves a received frame. it can be called when all the frames have been received and thus hal_spi_active_is() returns
                hal_false. However, it can also be called inside the cfg->onframereceiv(arg) callback after the reception of each frame.
    @param      id                    the id
    @param      rxframe             the received frame
    @param      remainingrxframes   if not NULL, it is filled with the number of frames remaining in the internal fifo.
    @return     hal_res_OK if a valid frame is available, hal_res_NOK_nodata if no frame is available, or hal_res_NOK_generic on failure
  */
extern hal_result_t hal_spi_get(hal_spi_t id, uint8_t* rxframe, uint8_t* remainingrxframes);


/** @fn         extern hal_result_t hal_spi_on_framesreceived_set(hal_spi_t id, hal_callback_t onframeereceived, void* arg)
    @brief      this function allows changing the cfg->onframereceiv(arg) callback originally configured at hal_spi_init().
    @param      id                    the id
    @param      onframereceiv       the callback (it can be NULL).
    @param      arg                 its argument
    @return     hal_res_OK if spi is initted, hal_res_NOK_generic on failure
  */
extern hal_result_t hal_spi_on_framesreceived_set(hal_spi_t id, hal_callback_t onframesreceived, void* arg); 


/** @fn         extern hal_result_t hal_spi_rx_isr_enable(hal_spi_t id)
    @brief      this function enable the rx isr for the SPI
    @param      id                    the id
    @return     hal_res_OK if procedure is successful, hal_res_NOK_generic on failure
  */
extern hal_result_t hal_spi_rx_isr_enable(hal_spi_t id);


/** @fn         extern hal_result_t hal_spi_rx_isr_disable(hal_spi_t id)
    @brief      this function disable the rx isr for the SPI
    @param      id                    the id
    @return     hal_res_OK if procedure is successful, hal_res_NOK_generic on failure
  */
extern hal_result_t hal_spi_rx_isr_disable(hal_spi_t id);


/** @fn         extern hal_result_t hal_spi_periph_enable(hal_spi_t id)
    @brief      this function enable the SPI peripherical
    @param      id                    the id
    @return     hal_res_OK if procedure is successful, hal_res_NOK_generic on failure
  */
extern hal_result_t hal_spi_periph_enable(hal_spi_t id);


/** @fn         extern hal_result_t hal_spi_periph_disable(hal_spi_t id)
    @brief      this function disable the SPI peripherical
    @param      id                    the id
    @return     hal_res_OK if procedure is successful, hal_res_NOK_generic on failure
  */
extern hal_result_t hal_spi_periph_disable(hal_spi_t id);


/** @fn         extern hal_result_t hal_spi_set_sizeofframe(hal_spi_t id, uint8_t framesize);
    @brief      this function set the size of frame to be used by the isr to call the callback function
    @param      id              the id
    @param      framesize       number of bytes contained in the frame
    @return     hal_res_OK if procedure is successful, hal_res_NOK_generic on failure
    @warning    the framesize cannot be higher than cfg->sizeofframe defined by hal_spi_init(). 
  */
extern hal_result_t hal_spi_set_sizeofframe(hal_spi_t id, uint8_t framesize);


/** @fn         extern hal_result_t hal_spi_set_isrtxframe(hal_spi_t id, const uint8_t* txframe)
    @brief      this function set the tx frame by copying the bytes array pointed by txframe 
    @param      id          the id
    @param      txframe     pointer to the transmission frame. it must be of size framesize*1 or framesize*2
                            depending on the value of cfg->datasize
    @return     hal_res_OK if procedure is successful, hal_res_NOK_generic on failure
  */
extern hal_result_t hal_spi_set_isrtxframe(hal_spi_t id, const uint8_t* txframe);


/** @fn         extern hal_result_t hal_spi_deinit(hal_spi_t id)
    @brief      this function deinitializes an SPI id and all the associated resources
    @param      id            the id
    @return     hal_res_OK or hal_res_NOK_generic on failure
  */
extern hal_result_t hal_spi_deinit(hal_spi_t id);

/** @}            
    end of group doxy_group_hal_spi  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



