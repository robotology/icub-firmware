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

#ifndef _HAL_SPIENCODER_H_
#define _HAL_SPIENCODER_H_

#ifdef __cplusplus
extern "C" {
#endif

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_spiencoder.h
    @brief      This header file implements interface to a generic hal encoder sensor 
    @author     marco.accame@iit.it
    @date       02/07/2013
**/

/** @defgroup doxy_group_hal_spiencoder HAL SENSOR ENCODER

    The HAL ENCODER ....
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h"
#include "hal_spi.h"

// - public #define  --------------------------------------------------------------------------------------------------
#if !defined(AEA3_SUPPORT)
    #define AEA3_SUPPORT
#endif
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef enum hal_spiencoder_t 
    @brief      hal_spiencoder_t contains every possible encoder sensor. the way the encoder is read from the peripherals
                of the MPU depends on internals of the encoder file. in case of encoders using spi and mux, then the mapping
                between encoders and peripherals is contained in the brdcfg file.
 **/ 
typedef enum  
{ 
    hal_spiencoder1 = 0,
    hal_spiencoder2 = 1,
    hal_spiencoder3 = 2,         
    hal_spiencoder4 = 3,
    hal_spiencoder5 = 4,
    hal_spiencoder6 = 5,
    hal_spiencoderNONE = 255
} hal_spiencoder_t;

enum { hal_spiencoders_number = 6 };

/** @typedef    typedef enum hal_spiencoder_type_t
    @brief      contains the type of the encoder
 **/
typedef enum  
{ 
    hal_spiencoder_typeAEA      = 0, 
    hal_spiencoder_typeAMO      = 1, 
    hal_spiencoder_typeCHAINof2 = 2,
    hal_spiencoder_typeCHAINof3 = 3,
#if defined(AEA3_SUPPORT)
    hal_spiencoder_typeAEA3     = 4,
#endif
    hal_spiencoder_typeAksIM2   = 5,
    hal_spiencoder_typeNONE     = 255
} hal_spiencoder_type_t;

/** @typedef    typedef uint32_t hal_spiencoder_position_t
    @brief      contains positional value of encoder
 **/
typedef uint32_t hal_spiencoder_position_t;


/** @typedef    typedef struct hal_spiencoder_errors_flags
    @brief      contains the flag denoting errors for the encoder
 **/
typedef struct
{
    uint8_t tx_error: 1;
    uint8_t data_error: 1;
    uint8_t data_notready: 1;
    uint8_t chip_error: 1;
} hal_spiencoder_errors_flags;


typedef union
{
    hal_spiencoder_errors_flags     flags; 
    uint32_t                        value; // contains bits whose meaning is specified by hal_spiencoder_diagnostic_type_t 
    uint8_t                         aksim2_status_crc: 3;
} hal_spiencoder_diagnostic_info_t;

typedef enum
{
    hal_spiencoder_diagnostic_type_none = 0,        // there is no diagnostic info available
    hal_spiencoder_diagnostic_type_flags = 1,       // field flags contains the legacy hal_spiencoder_errors_flags used since may 2021
    hal_spiencoder_diagnostic_type_amo_status0 = 2, // field info contains values of register status0 (adr 0x76) of AMO
    hal_spiencoder_diagnostic_type_amo_status1 = 3, // field info contains values of register status1 (adr 0x77) of AMO 
    hal_spiencoder_diagnostic_type_amo_notconn = 4,
    hal_spiencoder_diagnostic_type_aksim2_invalid_data = 5,
    hal_spiencoder_diagnostic_type_aksim2_close_to_limits = 6,
    hal_spiencoder_diagnostic_type_aksim2_crc_error = 7,
    hal_spiencoder_diagnostic_type_aksim2_not_connected = 8,
} hal_spiencoder_diagnostic_type_t;

typedef struct
{
    uint8_t                             type; // use only values inside hal_spiencoder_diagnostic_type_t
    hal_spiencoder_diagnostic_info_t    info;
} hal_spiencoder_diagnostic_t;

//typedef struct
//{
//    uint8_t         type;           // use hal_spiencoder_type_t
//    uint8_t         flags;          // use the bitwise OR of enum hal_spi_encoder_flag_t 
//    uint16_t        value0;         // used for aea, amo, first of chain
//    uint16_t        value1;         // used for second of chain
//    uint16_t        value2;         // used for third of chain
//} hal_spiencoder_value_t;


/** @typedef    typedef struct hal_spiencoder_cfg_t;
    @brief      contains configuration data of encoder.
 **/
typedef struct
{
    hal_interrupt_priority_t    priority;       /**< the priority if the ISR underlying the encoder */
    void (*callback_on_rx)(void *arg);          /**< callback called when a new value for the encoder is available   */
    void*                       arg;            /**< argument of the callback: contains pointer to latest encoder value  */
    hal_spiencoder_type_t       type;           /**< Encoder model. type 1: AEA, type 2: AMO Board, type 3: DAISY CHAIN of 2 encoders, type 4: AEA3 */
    hal_bool_t                  sdata_precheck; /**< If hal_true, performs a validity check on the sensor data transmission before the real reading (not meaningful for AEA board)  */
    uint8_t                     reg_addresses[2]; // array of addresses of registers of AMO to read in daisy chain.
                                                  // for AMO if any value is 0x00 then it is filled with 0x77.
                                                  // valid values: {0x76, 0x77} reads status1 and status2 in two times, {0x77, 0x77} reads always status2,
                                                  //               {0x76, 0x76} reads always status.    
} hal_spiencoder_cfg_t;


/** @typedef    typedef enum hal_spiencoder_stream_t;
    @brief      contains the possible streams. on a given stream are mapped some encodeders. each stream can be acquired in parallel to another. 
 **/
typedef enum
{
    hal_spiencoder_stream0      = 0,
    hal_spiencoder_stream1      = 1,
    hal_spiencoder_streamNONE   = 255   
} hal_spiencoder_stream_t; 

enum { hal_spiencoder_streams_number = 2, hal_spiencoder_maxnumber_in_stream = 3 };




/** @typedef    typedef struct hal_spiencoder_stream_map_t;
    @brief      contains the way the encoders are mapped into the two streams. encoders in different streams can be acquired in parallel 
 **/
typedef struct
{
    uint8_t                 totalnumberofencoders;                                      /**< number of total supported encoders in the given board */
    uint8_t                 stream2numberofencoders[hal_spiencoder_streams_number];     /**< number of encoders in a given stream */
    hal_spiencoder_stream_t encoder2stream[hal_spiencoders_number];                     /**< look up table which gives the stream given the encoder */
    uint8_t                 encoder2indexinstream[hal_spiencoders_number];              /**< index in the proper stream for each encoder */
} hal_spiencoder_stream_map_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_spiencoder_cfg_t hal_spiencoder_cfg_default;   // = { .priority = hal_int_priority15, .callback_on_rx = NULL, .arg = NULL };


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern hal_boolval_t hal_spiencoder_supported_is(hal_spiencoder_t id)
    @brief      This function tells if the specified encoder is supported by the board. 
    @param      id              identifies encoder id 
    @return     hal_true or hal_false
  */
extern hal_boolval_t hal_spiencoder_supported_is(hal_spiencoder_t id);


/** @fn         extern const hal_spiencoder_stream_map_t* hal_spiencoder_stream_map_get(void)
    @brief      This function returns the map of the streams of the encoders. 
    @return     teh required map
  */
extern const hal_spiencoder_stream_map_t* hal_spiencoder_stream_map_get(void);


/** @fn         extern hal_boolval_t hal_spiencoder_initted_is(hal_spiencoder_t id)
    @brief      This function tells if the specified encoder is already initted. 
    @param      id              identifies encoder id 
    @return     hal_true or hal_false
  */
extern hal_boolval_t hal_spiencoder_initted_is(hal_spiencoder_t id);


/** @fn            extern hal_result_t hal_spiencoder_init(hal_spiencoder_t id, const hal_spiencoder_cfg_t *cfg)
    @brief      This function initializes the encoder and whatever is required for its behaviour (mux and spi).
    @param      encoder         the encoder
    @param      cfg             The configuration of the encoder. 
    @return     hal_res_NOK_generic in case the encoder is not supported, else hal_res_OK
  */
extern hal_result_t hal_spiencoder_init(hal_spiencoder_t id, const hal_spiencoder_cfg_t *cfg);


/** @fn            extern hal_result_t hal_spiencoder_read_start(hal_spiencoder_t id)
    @brief      This function starts reading of data from the encoder in a non-blocking way.
                When reading is finished, then the callback on reception is invoked, which can
                retrieve the value with hal_spiencoder_read().
    @param      encoder         the encoder
    @return     hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_spiencoder_read_start(hal_spiencoder_t id);

/** @fn         extern uint32_t hal_spiencoder_get_value(hal_spiencoder_t id, hal_spiencoder_position_t* value)
    @brief      This function reads data previously acquired by a call of hal_spiencoder_start().
    @param      encoder         the encoder
    @param      pos             keeps the position value.
    @param      e_flags         keeps the flags which validate tha positional data
    @return     hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_spiencoder_get_value(hal_spiencoder_t id, hal_spiencoder_position_t* pos, hal_spiencoder_errors_flags* e_flags);

extern hal_result_t hal_spiencoder_get_value2(hal_spiencoder_t id, hal_spiencoder_position_t* pos, hal_spiencoder_diagnostic_t* diagn);

///** @fn         extern hal_result_t hal_spiencoder_get_value2(hal_spiencoder_t id, hal_spiencoder_value_t* value)
//    @brief      This function reads data previously acquired by a call of hal_spiencoder_start().
//    @param      encoder         the encoder
//    @param      val             contains the position readings plus all the error flags.
//    @return     hal_res_NOK_generic on error else hal_res_OK
//  */
//extern hal_result_t hal_spiencoder_get_value2(hal_spiencoder_t id, hal_spiencoder_value_t* val);


/* Testing API (only for AMO board). It's possible to use ONLY the above functions, without losing features */

/** @fn         extern hal_result_t hal_spiencoder_read_start_t2(hal_spiencoder_t id)
    @brief      This function starts reading of data from the encoder of type 2 (AMO board) in a non-blocking way.
                When reading is finished, then the callback on reception is invoked, which can
                retrieve the value with hal_spiencoder_get_value_t2.
    @param      encoder         the encoder
    @param      reg_address     address of the status register to be read to validate the sensor data
    @param      sdata_check     boolean value to choose if it's possible to skip the sensor data status check (FALSE) or if it's needed (TRUE)
    @return     hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_spiencoder_read_start_t2(hal_spiencoder_t id, uint8_t reg_address, hal_bool_t sdata_check);


/** @fn         extern hal_result hal_spiencoder_get_value_t2(hal_spiencoder_t id, uint32_t* pos, hal_bool_t* validity, uint16_t* flags);
    @brief      This function reads data previously acquired by a call of hal_spiencoder_read_start_t2().
    @param      encoder         the encoder
    @param      pos             keeps the position.
    @param      validity        keeps the validity.
    @param      flags           keeps the flags contained inside the register status read.
    @return     hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_spiencoder_get_value_t2(hal_spiencoder_t id, uint32_t* pos, hal_bool_t* validity, uint16_t* flags);


/** @fn         extern hal_result hal_spiencoder_get_frame(hal_spiencoder_t id, uint32_t* byte1, ...);
    @brief      This function reads raw data previously acquired by a call of hal_spiencoder_read_start() or hal_spiencoder_read_start_t2().
                In this way it's possible to read the single bytes written inside the structure for the received frame of sensor data
                It is used only for debugging AMO
    @param      encoder         the encoder
    @param      bytes             keeps the bytes received.
    @return     hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_spiencoder_get_frame(hal_spiencoder_t id, uint8_t* bytes);

/** @fn         hal_spi_t hal_spiencoder_get_spi(hal_spiencoder_t id);
    @brief      This function returns the SPI peripheral associated to the reading of the hal_spiencoder_t given as a parameter.
    @param      encoder         the encoder
    @return     hal_res_NOK_generic if encoder not supported for the board else hal_res_OK;
  */
extern hal_result_t hal_spiencoder_get_spi(hal_spiencoder_t id, hal_spi_t* spiid);


/** @fn         extern hal_result_t hal_spiencoder_deinit(hal_spiencoder_t id)
    @brief      This function deinitializes the encoder and whatever is required for its behaviour (mux and spi).
                If other encoder are initialized using the same MUX and SPI, only the objects associated to the encoder
                are de-initialized.
    @param      encoder         the encoder
    @return     hal_res_NOK_generic on error on deinitialization, else hal_res_OK
  */
extern hal_result_t hal_spiencoder_deinit(hal_spiencoder_t id);

/** @}            
    end of group doxy_group_hal_spiencoder  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



