/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license SPI be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _HL_SPI_H_
#define _HL_SPI_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_spi.h
	@brief      This file contains basic initialisation api for hl SPI utilities.
	@author     marco.accame@iit.it
    @date       02/12/2014
**/

/** @defgroup doxy_group_hl_spi HL SPI

    The HL SPI ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"
#include "hl_gpio.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hl_spi_t 
    @brief      hl_spi_t contains the usable SPI ids.
 **/
typedef enum  
{ 
    hl_spi1 = 0,            /**< SPI1        */
    hl_spi2 = 1,            /**< SPI2        */
    hl_spi3 = 2             /**< SPI3        */
} hl_spi_t; 

enum { hl_spis_number = 3 };


/** @typedef    typedef struct hl_spi_gpiomap_t 
    @brief      hl_spi_gpiomap_t contains the pin mapping for SPI peripheral
 **/
typedef struct
{
    hl_gpio_map_t   sck;        /**< gpio map used for sck pin */
    hl_gpio_map_t   miso;       /**< gpio map used for miso pin */ 
    hl_gpio_map_t   mosi;       /**< gpio map used for mosi pin */ 
} hl_spi_gpiomap_t;


/** @typedef    typedef struct hl_spi_mapping_t 
    @brief      hl_spi_mapping_t contains the pin mapping for all SPI peripherals plus some advanced configuration
 **/
typedef struct
{
    uint32_t            supportedmask;             /**< bit in position hl_spix must be 1 if portx is supported */
    hl_spi_gpiomap_t    gpiomap[hl_spis_number];    /**< in position hl_spix there is gpio map of SPIx */
} hl_spi_mapping_t;


/** @typedef    typedef enum hl_spi_mode_t;
    @brief      expresses SPI mode referred to being master or slave.
 **/
typedef enum
{
    hl_spi_mode_master          = 0,
    hl_spi_mode_slave           = 1
} hl_spi_mode_t;


/** @typedef    typedef enum hl_spi_prescaler_t;
    @brief      expresses SPI prescaler referred to the relevant bus speed. 
                In stm32f107 / f407 the spi1 is on fast bus and spi2 ans spi3 are on slow bus. The user must use different prescaler values if
                the effective speed must be the same on all spi buses.
 **/
typedef enum
{
    hl_spi_prescaler_002        = 1,
    hl_spi_prescaler_004        = 2,
    hl_spi_prescaler_008        = 3,
    hl_spi_prescaler_016        = 4,
    hl_spi_prescaler_032        = 5,
    hl_spi_prescaler_064        = 6,
    hl_spi_prescaler_128        = 7,
    hl_spi_prescaler_256        = 8
} hl_spi_prescaler_t;


/** @typedef    typedef struct hl_spi_advcfg_opaque_t hl_spi_advcfg_t
    @brief      contains full configuration for SPI bus as specified by the silicon vendor. 
                it is opaquely declared. see hl_arch.h for definition.                
 **/
typedef struct hl_spi_advcfg_opaque_t hl_spi_advcfg_t;


/** @typedef    typedef struct hl_spi_cfg_t;
    @brief      contains configuration data of SPI peripheral.
 **/
typedef struct
{
    hl_spi_mode_t           mode;
    hl_spi_prescaler_t      prescaler; 
    const hl_spi_advcfg_t*  advcfg;         /**< used if not NULL. it overrides mode and prescaler. */
} hl_spi_cfg_t;


 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hl_spi_cfg_t hl_spi_cfg_default; // = {.mode = hl_spi_mode_master, .prescaler = hl_spi_prescaler_256, .advcfg = NULL };

extern const hl_spi_mapping_t* hl_spi_map;  // it must be defined externally. If NULL, then the entire hl_spi will not be used


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn			extern uint32_t hl_spi_speedofbus_get(hl_spi_t id)
    @brief  	this function tells what is the internal bus speed of an spi id. it can be used to chose the right value of the
                prescaler to be assigned inside the hl_spi_cfg_t argument to be passed to hl_spi_init().
    @param  	id 	        the id of spi
    @return 	the speed of the internal bus or 0 if the specified spi is not supported
  */
extern uint32_t hl_spi_speedofbus_get(hl_spi_t id);


/** @fn         extern hl_result_t hl_spi_init(hl_spi_t id, const hl_spi_cfg_t *cfg)
    @brief      This function configures SPI. It does not configure or enable the SPI transceiver.
    @param      id              identifies SPI id 
    @param      cfg             the configuration of the SPI peripheral
    @return     hl_res_NOK_generic in case of error, else hl_res_OK
  */
extern hl_result_t hl_spi_init(hl_spi_t id, const hl_spi_cfg_t *cfg);


/** @fn         extern hl_boolval_t hl_spi_supported_is(hl_spi_t id)
    @brief      This function tells if a given SPI port is supported.
    @param      id              identifies SPI port
    @return     hl_true or hl_false
  */
extern hl_boolval_t hl_spi_supported_is(hl_spi_t id);


/** @fn         extern hl_boolval_t hl_spi_initted_is(hl_spi_t id)
    @brief      This function tells if a given SPI port is initted.
    @param      id              identifies SPI port
    @return     hl_true or hl_false
  */
extern hl_boolval_t hl_spi_initted_is(hl_spi_t id);


/** @fn         extern hl_result_t hl_spi_enable(hl_spi_t id)
    @brief      This function starts SPI. It must be invoked after hl_spi_init.
    @param      id              identifies SPI port 
    @return     hl_res_NOK_generic in case of error, else hl_res_OK
  */
extern hl_result_t hl_spi_enable(hl_spi_t id);


/** @fn         extern hl_result_t hl_spi_disable(hl_spi_t id)
    @brief      This function disable SPI.
    @param      id              identifies SPI port 
    @return     hl_res_NOK_generic in case of error, else hl_res_OK
  */
extern hl_result_t hl_spi_disable(hl_spi_t id);


/** @}            
    end of group doxy_group_hl_spi  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



