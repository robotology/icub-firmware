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

#ifndef _HAL_DISPLAY_H_
#define _HAL_DISPLAY_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_device.h
    @brief      This header file keeps public interface to the device display module.
    @author     marco.accame@iit.it
    @date       11/08/2012
**/

/** @defgroup doxy_group_hal_display HAL DISPLAY

    The HAL DISPLAY ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hal_display_dev_t 
    @brief      hal_display_dev_t contains every possible display.
 **/ 
typedef enum  
{ 
    hal_display_dev1 = 0
} hal_display_dev_t;

enum { hal_display_devs_number = 1 };

/** @typedef    typedef enum hal_display_resolution_t 
    @brief      hal_display_resolution_t expresses resolution of the display in term of pixel width, height and bits
                per pixel
 **/ 
typedef enum
{
    hal_display_res_320x240x24       = 0
} hal_display_resolution_t;

enum { hal_display_resolutions_number = 1 };


/** @typedef    typedef enum hal_display_color_t 
    @brief      hal_display_color_t express some color used by the display.
 **/ 
typedef enum
{
    hal_display_color_black        = 0,
    hal_display_color_darkgrey     = 1,
    hal_display_color_lightgrey    = 2,
    hal_display_color_white        = 3,
    hal_display_color_red          = 4,
    hal_display_color_green        = 5,
    hal_display_color_blue         = 6,
    hal_display_color_yellow       = 7,
    hal_display_color_cyan         = 8,
    hal_display_color_magenta      = 9 
} hal_display_color_t;

enum { hal_display_colors_number = 10 };

typedef enum
{
    hal_display_font_size_6x8       = 0,
    hal_display_font_size_16x24     = 1
} hal_display_font_size_t;

enum { hal_display_font_sizes_number = 2 };

typedef struct
{
    hal_display_font_size_t         textsize;
    hal_display_color_t             textcolor;
    hal_display_color_t             backcolor;
} hal_display_font_t;


/** @typedef    typedef struct hal_display_cfg_t;
    @brief      contains configuration data of display peripheral.
 **/
typedef struct
{
    hal_display_resolution_t    res;
    hal_display_color_t         screencolor;
    hal_display_font_t          font;
} hal_display_cfg_t;


 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_display_cfg_t hal_display_cfg_default;   // = { .res = hal_display_res_320x240x24, .fon = hal_display_font_24x16 };


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern hal_result_t hal_display_init(hal_display_dev_t display, const hal_display_cfg_t *cfg)
    @brief      Initialise an external display. 
    @param      cfg         The configuration
    @return     hal_res_OK or hal_res_NOK_generic on failure or if the display is disabled by the base config.
 **/
extern hal_result_t hal_display_init(hal_display_dev_t dev, const hal_display_cfg_t *cfg);


/** @fn         extern hal_result_t hal_display_clear(hal_display_dev_t dev, hal_display_color_t color)
    @brief      Clear the whole display with color @e color.  
    @param      color       The color which teh display will be.
 **/
extern hal_result_t hal_display_clearscreen(hal_display_dev_t dev);


/** @fn         extern hal_result_t hal_display_settext(hal_display_dev_t dev, hal_display_font_t fnt, hal_display_color_t backcolor, hal_display_color_t textcolor)
    @brief      Sets the display so that text uses a given font, a given background color and a given color.  
    @param      fnt         The font to be used.
    @param      backcolor   The background color of the text
    @param      textcolor   The color used for text.
 **/
extern hal_result_t hal_display_setfont(hal_display_dev_t dev, hal_display_font_t font);


/** @fn         extern hal_result_t hal_display_clearline(hal_display_dev_t dev, uint16_t line)
    @brief      Clear a complete line in teh dispalay.  
    @param      line        The target line.
 **/
extern hal_result_t hal_display_clearline(hal_display_dev_t dev, uint16_t line);


/** @fn         extern hal_result_t hal_display_putchar(hal_display_dev_t dev, uint16_t line, char chr)
    @brief      Display a character on a given line 
    @param      line        The target line.
    @param      chr         The target character.
 **/
extern hal_result_t hal_display_putchar(hal_display_dev_t dev, uint16_t line, uint16_t column, char chr);


/** @fn         extern hal_result_t hal_display_putstring(hal_display_dev_t dev, uint16_t line, char* str)
    @brief      Display a null-terminated string on a given line 
    @param      line        The target line.
    @param      str         The null-terminated string.
 **/
extern hal_result_t hal_display_putstring(hal_display_dev_t dev, uint16_t line, uint16_t column, char* str);


/** @}            
    end of group doxy_group_hal_display  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



