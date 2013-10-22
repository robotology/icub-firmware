/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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

#ifndef _6SG_CONFIG_H_
#define _6SG_CONFIG_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       6sg_config.h
    @brief      This file provides configuration types and function for read/ configuration data from/to eeprom.  
    @author     valentina.gaggero@iit.it
    @date       07/14/2011
**/

// - external dependencies --------------------------------------------------------------------------------------------

#warning -> acemor: a cosa serve qui il dsp.h ? se non serve togliere
#warning VALE-> il dsp serve per il tipo fractional...se non ricordo male
#include <dsp.h>

#include <stdint.h>

#include "hal_base.h"
#include "eEcommon.h"


// - public #define  --------------------------------------------------------------------------------------------------

#warning -> nei .h si dovrebbe usare un prefisso che indichi da dove viene la macro ad esempio CFG6SG_

#define AN_CHANNEL_NUM      6   /**< 6sg board has 6 analogic channels */
#define IIR_LPF_N_MAX_BQ    5   /**< number of BiQuads for the IIR LPF TODO: capire!!! */

#define BOARD_TYPE_6SG      0x9 // TODO da spostare in common????
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

// asfidanken: rimosso questa struttura dati


//#warning -> valentina ha scritto: IMPORTANTE: QUESTA STRUTTURA DATI, DOVRà ESSERE CONDIVISA CON IL BOOTLOADER.
//#warning -> acemor: instad of general_ee_config_data_t use the struct defined in eEcommon and used in teh bootloader

#define ADDITIONAL_INFO_MAX_LEN     32

typedef struct
{
    eEversion_t exe_file;
    eEversion_t can_protocol;
} srcCode_version_info_t;


/** @typedef    typedef struct general_ee_config_data_t;
    @brief      contains some data shared with bootloader and they must be saved in eeprom.
                In general, applications and bootloader share some info saved in eeprom.
                some of them are mirrored in general_ee_config_data_t. 
    @warning    currently additional info and serial number is not in shared memory, 
                but in application memory, so I removed them from this struct
                TODO: move them in shared memory!!
 **/
typedef struct
{
    uint8_t                 board_address;
//    uint8_t                 additional_info[ADDITIONAL_INFO_MAX_LEN]; 
//    uint8_t                 serial_number[7];
    srcCode_version_info_t  verinfo;
} general_ee_config_data_t;

/** @typedef    typedef struct amplifier_gain_t;
    @brief      contains gain values. The amplifier AD8555 applay gain in two steps for each channel. 
 **/
typedef struct
{
    uint16_t first_step;    /**< possible values belong to [0,127] range */
    uint16_t second_step;   /**< possible values belong to [0,7] range */
} amplifier_gain_t; 


/** @typedef    typedef struct 6SG_ee_config_data_t;
    @brief      contains all data necessary for 6SG application that must be saved in eeprom. 
 **/
typedef struct
{
    uint8_t additional_info[ADDITIONAL_INFO_MAX_LEN]; /**< string where use can annotate something. All board has this field */
    uint8_t serial_number[7];   

    uint16_t            an_channel_offset[AN_CHANNEL_NUM];           /**< offset to apply to each channel. */
    int16_t             tr_matrix[AN_CHANNEL_NUM][AN_CHANNEL_NUM];   /**< SG2TF transformation matrix. */
    amplifier_gain_t    an_channel_gain[AN_CHANNEL_NUM];
    int16_t             calibration_tare[AN_CHANNEL_NUM];
    int16_t             full_scales[AN_CHANNEL_NUM];
    uint8_t             sw_matrix_gain;          /**< unique value to applay to all channels. It is set during calibration phase */
    uint8_t             can_msg_datarate;       /**< framerate of outgoing messages. (num of msec) */
    fractional          iir_tr_coef[5 * IIR_LPF_N_MAX_BQ];
    uint16_t            checksum;
    //IIR_filters_data_t    iir_filter;
} SIXsg_ee_config_data_t; 


/** @typedef    typedef enum calculating_data_mode_t;
    @brief      6sg application can calulate data in one of these mode. 
                TODO: spega i vari modi...quando sono chiari!!!
 **/
typedef enum
{
    filtMode_none = 0,      /**< no filters are applied to data read by adc. The last read value is used in calculation */
    filtMode_iir =1,        /**< iir filter is applied to data read by adc */
    filtMode_average =2     /**< average filter is applied to data read by adc. CURRENTLY NOT IMPLEMENTED!!!!*/
} filtering_data_mode_t;


/** @typedef    typedef enum trasmission_outgoingMsg_mode_t;
    @brief      indicates the ways in which data can be sent 
 **/
typedef enum
{
    tx_outMsg_off = 0,              /**< don't send anything */
    tx_outMsg_torqueData_on,        /**< send only torque value (normal running mode)  */
    tx_outMsg_readOnly,         /**< read from adc, but don't send data  */
    tx_outMsg_uncalibData_on //tx_outMsg_torqueForceData_on    /**< send uncalib data (all six axes) */
} trasmission_outgoingMsg_mode_t;


/** @typedef    typedef struct SIXsg_config_data_t;
    @brief      contains infomation about 6SG application 's behaviour.
                They must be set in compile time.
                Currently does not support commands that allow you to change its behavior at run time.
 **/
typedef struct
{
    uint8_t                         send_ack_each_cmd;      /**< if = 1 send ack for each command (polling msg) */
    uint8_t                         save_in_ee_immediately; /**< if = 1 save current values of ee_data in eeprom each time a cmd change a value of data in eeprom. */      
    filtering_data_mode_t           filt_data_mode;
    trasmission_outgoingMsg_mode_t  tx_outMsg_mode; 
} config_behaviour_t;

// asfidanken: rimosso ee_shared_data_t

/** @typedef    typedef struct SIXsg_config_data_t;
    @brief      collects all relevant information of the application 6SG
 **/
typedef struct
{
    general_ee_config_data_t    gen_ee_data;
    SIXsg_ee_config_data_t      SIXsg_ee_data;  /**< contains specific information for 6SG application.*/
    config_behaviour_t          behaviour_cfg;
} SIXsg_config_data_t;



    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

//extern const uint8_t asfidanken_board_address;

// - declaration of extern public functions ---------------------------------------------------------------------------

///** @fn         extern srcCode_version_info_t* srcCode_version_get_handler(void);
//    @brief      returns pointer to memory containg 6SG application and CAN protocol version infomation
//    @param      -
//    @return     srcCode_version_info_t pointer
// **/
//extern srcCode_version_info_t* srcCode_version_get_handler(void);


/** @fn         extern hal_result_t SIXsg_config_init(void)
    @brief      init 6sg appl config. If eeprom is empty, save in eeprom default config
    @param      -
    @return     ok on success, othewise nok
 **/
extern hal_result_t SIXsg_config_init(config_behaviour_t *cfg_behav);


/** @fn         hal_result_t SIXsg_config_get(SIXsg_config_data_t *cfg_ptr)
    @brief      return pointer to 6sg application
    @param      -
    @return     NULL if SIXsg_config_init config is not ititalized with SIXsg_config_init func.
 **/
extern SIXsg_config_data_t * SIXsg_config_get(void);




extern hal_result_t SIXsg_config_save_to_eeprom(SIXsg_config_data_t *cfg_ptr);


///** @fn         extern hal_result_t SIXsg_config_read_from_eeprom(SIXsg_config_data_t *cfg_ptr)
//    @brief      reads from eeprom both general data and 6sg data and sets them in cfg_ptr,
//                i.e., saves the first in "gen_ee_data" field and the second in "SIXsg_ee_data" field of
//                SIXsg_config_data_t struct.
//                if eeprom is empty, set the default vaules in both fields.
//    @param      cfg_ptr         pointer to SIXsg_config_data_t struct.
//    @return     HAL_RES_NOK_ERROR_WRONG_PARAM if cfg_ptr
//                HAL_RES_OK otherwise
// **/
//extern hal_result_t SIXsg_config_read_from_eeprom(SIXsg_config_data_t *cfg_ptr);


extern hal_result_t SIXsg_boardAddress_save_to_eeprom(SIXsg_config_data_t *cfg_ptr);

// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


