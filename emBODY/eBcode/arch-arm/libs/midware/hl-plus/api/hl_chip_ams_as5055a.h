/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _HL_CHIP_AMS_AS5055A_H_
#define _HL_CHIP_AMS_AS5055A_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_chip_ams_as5055a.h
    @brief      This header file implements interface to a AMS AS5055A magnetic position sensor chip
    @author     marco.accame@iit.it
    @date       8feb2016
**/

/** @defgroup arm_hl_chip_ams_as5055a AMS AS5055A magnetic position sensor chip 

 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"
#include "hl_spi.h"
#include "hl_gpio.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

//#define TEST_AMS_AS5055A_AS_MAGGIALI
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef enum
{
    hl_chip_ams_as5055a_channel1    = 0,
    hl_chip_ams_as5055a_channel2    = 1,
    hl_chip_ams_as5055a_channelALL  = 254,
    hl_chip_ams_as5055a_channelNONE = 255   
} hl_chip_ams_as5055a_channel_t;


enum 
{ 
    hl_chip_ams_as5055a_channels_numberof       = 2,    // maximum number of channels that this driver can manage 
    hl_chip_ams_as5055a_max_chips_in_channel    = 3     // maximum number of chips that we can put in daisy chain in each channel. 
};

typedef struct
{
    hl_spi_mapping_t*           spimap2use;         // the spi map (sck, miso, mosi) to use.
    hl_spi_prescaler_t          prescaler;          // the only possible spi param. The AS5055A SPI must be master, and then with mode=1 (CPOL=0, CPHA=1) or (SPI_CPOL_Low, SPI_CPHA_2Edge).               
} hl_chip_ams_as5055a_spicfg_t;

/** @typedef    typedef struct hl_chip_ams_as5055a_cfg_t 
    @brief      contains the configuration for the chip. prior to call hl_chip_ams_as5055a_init(), the user MUST initialise the hl_spi_map variable 
 **/
typedef struct
{
    hl_spi_t                        spiid;
    uint8_t                         numberofchained;    // upto to hl_chip_ams_as5055a_max_chips_in_channel
    hl_chip_ams_as5055a_spicfg_t*   spicfg;             // if NULL, the driver must find the hl_spi already initted. otherwise it uses this field to init it.
    hl_gpio_t                       nsel;
    hl_gpio_t                       nint;
} hl_chip_ams_as5055a_cfg_t;


typedef enum
{
    hl_chip_ams_as5055a_resetmode_master                        = 0,
    hl_chip_ams_as5055a_resetmode_software_plus_spiregisters    = 1
} hl_chip_ams_as5055a_resetmode_t;


typedef enum
{
    hl_chip_ams_as5055a_readmode_start_wait_read    = 0,
    hl_chip_ams_as5055a_readmode_start_only         = 1,
    hl_chip_ams_as5055a_readmode_read_only          = 2
} hl_chip_ams_as5055a_readmode_t;
 

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern hl_result_t hl_chip_ams_as5055a_init(hl_chip_ams_as5055a_channel_t chn, const hl_chip_ams_as5055a_cfg_t *cfg)
    @brief      this function initializes the chip. 
    @param      chn     channel to use
    @param      cfg     pointer to configuration data
    @return     hl_res_NOK_generic on error else hl_res_OK
  */
extern hl_result_t hl_chip_ams_as5055a_init(hl_chip_ams_as5055a_channel_t chn, const hl_chip_ams_as5055a_cfg_t *cfg);


/** @fn         extern hl_result_t hl_chip_ams_as5055a_reset(hl_chip_ams_as5055a_channel_t chn)
    @brief      this function initializes the chip
    @param      chn     channel to use
    @param      cfg     pointer to configuration data
    @return     hl_res_NOK_generic on error else hl_res_OK
  */
extern hl_result_t hl_chip_ams_as5055a_reset(hl_chip_ams_as5055a_channel_t chn, hl_chip_ams_as5055a_resetmode_t resetmode);


/** @fn         extern hl_result_t hl_chip_ams_as5055a_read_angulardata(hl_chip_ams_as5055a_channel_t chn, unt16_t* value0, unt16_t* value1, uint16_t* value2)
    @brief      this function reads the values.
    @param      chn     channel to use
    @return     hl_res_NOK_generic on error else hl_res_OK
  */
extern hl_result_t hl_chip_ams_as5055a_read_angulardata(hl_chip_ams_as5055a_channel_t chn, hl_chip_ams_as5055a_readmode_t readmode, uint16_t* value0, uint16_t* value1, uint16_t* value2);


extern hl_result_t hl_chip_ams_as5055a_read_errorstatus(hl_chip_ams_as5055a_channel_t chn, uint16_t* value0, uint16_t* value1, uint16_t* value2);


/** @}            
    end of group arm_hl_chip_ams_as5055a  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



