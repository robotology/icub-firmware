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

#ifndef _HAL_DEVICE_DISPLAY_H_
#define _HAL_DEVICE_DISPLAY_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_device_display.h
    @brief      This header file keeps public interface to the device display module.
    @author     marco.accame@iit.it
    @date       11/08/2012
**/

/** @defgroup hal_device_display HAL DISPLAY

    The HAL DISPLAY ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef enum hal_device_display_resolution_t 
    @brief      hal_device_display_resolution_t expresses resolution of the display in term of pixel width, height and bits
                per pixel
 **/ 
typedef enum
{
    hal_device_display_res_320x240x24       = 0
} hal_device_display_resolution_t;

enum { hal_device_display_resolutions_number = 1 };


/** @typedef    typedef enum hal_device_display_font_t 
    @brief      hal_device_display_font_t express format of text used by the display.
 **/ 
typedef enum
{
    hal_device_display_font_24x16           = 0
} hal_device_display_font_t;

enum { hal_device_display_fonts_number = 1 };

/** @typedef    typedef enum hal_device_display_color_t 
    @brief      hal_device_display_color_t express some color used by the display.
    @warning    The values in this hal_device_display_color_t must not be changed to avoid incoherency with the MPU's vendor's
                library. So far they are tuned for STM32F1x.    
 **/ 
typedef enum
{
    hal_device_display_col_black        = 0,
    hal_device_display_col_darkgrey     = 1,
    hal_device_display_col_lightgrey    = 2,
    hal_device_display_col_white        = 3,
    hal_device_display_col_red          = 4,
    hal_device_display_col_green        = 5,
    hal_device_display_col_blue         = 6,
    hal_device_display_col_yellow       = 7,
    hal_device_display_col_cyan         = 8,
    hal_device_display_col_magenta      = 9 
} hal_device_display_color_t;

enum { hal_device_display_colors_number = 10 };


/** @typedef    typedef struct hal_device_display_cfg_t;
    @brief      contains configuration data of display peripheral.
 **/
typedef struct
{
    hal_device_display_resolution_t    res;
} hal_device_display_cfg_t;


 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_device_display_cfg_t hal_device_display_cfg_default;   // = { .res = hal_device_display_res_320x240x24 };


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern hal_result_t hal_device_display_init(const hal_device_display_cfg_t *cfg)
    @brief      Initialise an external display. 
    @param      cfg         The configuration
    @return     hal_res_OK or hal_res_NOK_generic on failure or if the display is disabled by the base config.
 **/
extern hal_result_t hal_device_display_init(const hal_device_display_cfg_t *cfg);


/** @fn         extern hal_result_t hal_device_display_clear(hal_device_display_color_t color)
    @brief      Clear the whole display with color @e color.  
    @param      color       The color which teh display will be.
 **/
extern hal_result_t hal_device_display_clear(hal_device_display_color_t color);


/** @fn         extern hal_result_t hal_device_display_settext(hal_device_display_font_t fnt, hal_device_display_color_t backcolor, hal_device_display_color_t textcolor)
    @brief      Sets the display so that text uses a given font, a given background color and a given color.  
    @param      fnt         The font to be used.
    @param      backcolor   The background color of the text
    @param      textcolor   The color used for text.
    @warning    Use only for debug purposes on the MCBSTM32C board
 **/
extern hal_result_t hal_device_display_settext(hal_device_display_font_t fnt, hal_device_display_color_t backcolor, hal_device_display_color_t textcolor);


/** @fn         extern hal_result_t hal_device_display_clearline(uint32_t ln)
    @brief      Clear a complete line in teh dispalay.  
    @param      ln          The target line.
    @warning    Use only for debug purposes on the MCBSTM32C board
 **/
extern hal_result_t hal_device_display_clearline(uint32_t ln);


/** @fn         extern hal_result_t hal_device_display_putchar(uint32_t ln, uint8_t  c)
    @brief      Display a character on a given line 
    @param      ln          The target line.
    @param      c           The target character.
    @warning    Use only for debug purposes on the MCBSTM32C board
 **/
extern hal_result_t hal_device_display_putchar(uint32_t ln, uint8_t  c);


/** @fn         extern hal_result_t hal_device_display_putstring(uint32_t ln, uint8_t  *s)
    @brief      Display a null-terminated string on a given line 
    @param      ln          The target line.
    @param      s           The null-terminated string.
    @warning    Use only for debug purposes on the MCBSTM32C board
 **/
extern hal_result_t hal_device_display_putstring(uint32_t ln, uint8_t  *s);


/** @}            
    end of group arm_hal_device_display  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



