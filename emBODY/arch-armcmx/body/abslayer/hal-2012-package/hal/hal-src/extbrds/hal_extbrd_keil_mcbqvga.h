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

#ifndef _HAL_EXTBRD_KEIL_MCBQVGA_H_
#define _HAL_EXTBRD_KEIL_MCBQVGA_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_extbrd_keil_mcbqvga.h
    @brief      This header file implements interface to the display board w/ qvga lcd w/ touch screen mounted 
                on some keil's dev kits (mcbstm32c, mcbstm32f200, mcbstm32f400)
    @author     marco.accame@iit.it
    @date       10/24/2012
**/

/** @defgroup arm_hal_extbrd_keil_mcbqvga HAL EXTERNAL BOARD KEIL QVGA DISPLAY 

    The HAL ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/

// mcbstm32f400:   AMPIRE AM240320L8TNQW + other
/*
    The LCD-TFT color graphics display panel (AMPIRE AM240320L8TNQW) contains a 240x320 pixel screen and connects using a 16-bit interface. 
    The LCD panel is mounted on the MCBSTM32C Display Board, and connects to MCBSTM32C Base Board via 30-pin header.
    This LCD panel includes a 4-wire, resistive touch screen controlled by an STMicroelectronics STMPE811 touch screen controller (IC2).
*/

// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef enum
{
    hal_extbrd_keil_mcbqvga_mode_spi        = 0
    //hal_extbrd_keil_mcbqvga_mode_bus16      = 1       used in mcbstm32f400 but here it is not yet supported.
} hal_extbrd_keil_mcbqvga_mode_t;



/** @typedef    typedef enum hal_extbrd_keil_mcbqvga_cfg_t 
    @brief      hal_extbrd_keil_mcbqvga_cfg_t contains the configuration for the sensor.
 **/
typedef struct
{
    hal_extbrd_keil_mcbqvga_mode_t      mode;
    hal_spi_port_t                      spiport;
} hal_extbrd_keil_mcbqvga_cfg_t;

typedef enum
{
    hal_extbrd_keil_mcbqvga_fontsize_6x8    = 0,
    hal_extbrd_keil_mcbqvga_fontsize_16x24  = 1
} hal_extbrd_keil_mcbqvga_fontsize_t;

// must be in rgb565 starting the red from the most significant bit. 0x0000 is black, 0xffff is white 
typedef uint16_t hal_extbrd_keil_mcbqvga_color_t;


 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_extbrd_keil_mcbqvga_cfg_t hal_extbrd_keil_mcbqvga_cfg_default; 


// - declaration of extern public functions ---------------------------------------------------------------------------

extern inline hal_extbrd_keil_mcbqvga_color_t hal_extbrd_keil_mcbqvga_rgb2color(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t col = ((r & 0x1f)<<11) | ((g & 0x3f)<<6) | ((b & 0x1f)<<0);
    return((uint16_t)col);
}

/** @fn			extern hal_result_t hal_extbrd_keil_mcbqvga_init(const hal_extbrd_keil_mcbqvga_cfg_t *cfg)
    @brief  	this function initializes the external board 
    @param  	cfg 	        pointer to configuration data
    @return 	hal_res_NOK_generic on error else hal_res_OK
  */
extern hal_result_t hal_extbrd_keil_mcbqvga_init(const hal_extbrd_keil_mcbqvga_cfg_t *cfg);

    
extern hal_result_t hal_extbrd_keil_mcbqvga_clear(hal_extbrd_keil_mcbqvga_color_t color);

extern hal_result_t hal_extbrd_keil_mcbqvga_settextproperties(hal_extbrd_keil_mcbqvga_fontsize_t fontsize, hal_extbrd_keil_mcbqvga_color_t backcolor, hal_extbrd_keil_mcbqvga_color_t textcolor);

extern hal_result_t hal_extbrd_keil_mcbqvga_clearline(uint16_t line);

extern hal_result_t hal_extbrd_keil_mcbqvga_putchar(uint16_t line, uint16_t column, char chr);

extern hal_result_t hal_extbrd_keil_mcbqvga_putstring(uint16_t line,  uint16_t column, char* str);

/** @}            
    end of group arm_hal_extbrd_keil_mcbqvga  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



