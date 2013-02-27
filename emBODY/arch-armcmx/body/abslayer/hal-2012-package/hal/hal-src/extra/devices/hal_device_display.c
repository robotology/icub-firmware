/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Marco Accame
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

/* @file       hal_device_display.c
	@brief      This file implements the device display module.
	@author     marco.accame@iit.it
    @date       11/08/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_DEVICE_DISPLAY


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "hal_utility_bits.h"

#include "hal_base_hid.h" 

#include "hal_brdcfg.h"
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_display.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_device_display_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_device_display_dev2index(t)              ((uint8_t)((t)))


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const hal_display_cfg_t hal_display_cfg_default =
{
    .res            = hal_display_res_320x240x24,
    .screencolor    = hal_display_color_cyan,
    .font           =
    {
        .textsize       = hal_display_font_size_6x8, //hal_display_font_size_24x16,
        .textcolor      = hal_display_color_black,   
        .backcolor      = hal_display_color_cyan//hal_display_color_white
    }
};




// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    hal_display_cfg_t           cfg;
} hal_device_display_info_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_device_display_supported_is(hal_display_dev_t dev);
static void s_hal_device_display_initted_set(hal_display_dev_t dev);
static hal_boolval_t s_hal_device_display_initted_is(hal_display_dev_t dev);

static hal_result_t s_hal_device_display_hw_init(hal_display_dev_t dev, const hal_display_cfg_t *cfg);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_device_display_initted[hal_display_devs_number] = { hal_false };

static hal_device_display_info_t s_hal_device_display_info[hal_display_devs_number] = { {.cfg = {.res = hal_display_res_320x240x24, .font = {hal_display_font_size_16x24, hal_display_color_black, hal_display_color_white}}} };

static const uint16_t s_hal_device_display_colormap[hal_display_colors_number] = 
{
    /*hal_display_color_black        =*/  0x0000,
    /*hal_display_color_darkgrey     =*/  0x7BEF,
    /*hal_display_color_lightgrey    =*/  0xC618,
    /*hal_display_color_white        =*/  0xFFFF,
    /*hal_display_color_red          =*/  0xF800,
    /*hal_display_color_green        =*/  0x07E0,
    /*hal_display_color_blue         =*/  0x001F,
    /*hal_display_color_yellow       =*/  0xFFE0,
    /*hal_display_color_cyan         =*/  0x07FF,
    /*hal_display_color_magenta      =*/  0xF81F 
};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_display_init(hal_display_dev_t dev, const hal_display_cfg_t *cfg)
{
    if(hal_true != s_hal_device_display_supported_is(dev))
    {
        return(hal_res_NOK_unsupported);
    }
    
    if(NULL == cfg)
    {
        cfg = &hal_display_cfg_default;
    }

    // in here we should add a brdcfg_  function which takes care of initting the device used and the required periphs.
    // e.g., in mcbstm32c a given 320x240 display is used together with spi. the init function and pointers to 
    // other functions must be taken from hal_brdcfg_mcbstm32c.[c, h]

    if(hal_res_OK != s_hal_device_display_hw_init(dev, cfg)) // contains GLCD_Init()
    {
        return(hal_res_NOK_generic);
    }
    
    memcpy(&s_hal_device_display_info[HAL_device_display_dev2index(dev)].cfg, cfg, sizeof(hal_display_cfg_t));
    
    s_hal_device_display_initted_set(dev);
    
    hal_display_clearscreen(dev);    
    hal_display_setfont(dev, cfg->font);
    
    return(hal_res_OK);
}

extern hal_result_t hal_display_clearscreen(hal_display_dev_t dev)
{
    if(hal_true != s_hal_device_display_initted_is(dev))
    {
        return(hal_res_NOK_generic);
    }
   
    if(NULL == hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_dev2index(dev)].chipif.clear)
    {
        return(hal_res_NOK_generic);
    }
    hal_display_cfg_t *cfg = &s_hal_device_display_info[HAL_device_display_dev2index(dev)].cfg;
    
    return(hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_dev2index(dev)].chipif.clear(s_hal_device_display_colormap[cfg->screencolor]));    
}

extern hal_result_t hal_display_setfont(hal_display_dev_t dev, hal_display_font_t font)
{
    if(hal_true != s_hal_device_display_initted_is(dev))
    {
        return(hal_res_NOK_generic);
    }
    
    
    if(NULL == hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_dev2index(dev)].chipif.settextproperties)
    {
        return(hal_res_NOK_generic);
    }
    
    memcpy(&s_hal_device_display_info[HAL_device_display_dev2index(dev)].cfg.font, &font, sizeof(hal_display_font_t));
    
    return(hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_dev2index(dev)].chipif.settextproperties(font.textsize, s_hal_device_display_colormap[font.backcolor], s_hal_device_display_colormap[font.textcolor]));        
}


extern hal_result_t hal_display_clearline(hal_display_dev_t dev, uint16_t line)
{
    if(hal_true != s_hal_device_display_initted_is(dev))
    {
        return(hal_res_NOK_generic);
    }

    if(NULL == hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_dev2index(dev)].chipif.clearline)
    {
        return(hal_res_NOK_generic);
    }
    return(hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_dev2index(dev)].chipif.clearline(line));
}

extern hal_result_t hal_display_putchar(hal_display_dev_t dev, uint16_t line, uint16_t column, char chr)
{
    if(hal_true != s_hal_device_display_initted_is(dev))
    {
        return(hal_res_NOK_generic);
    }

    
    if(NULL == hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_dev2index(dev)].chipif.putchar)
    {
        return(hal_res_NOK_generic);
    }
    return(hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_dev2index(dev)].chipif.putchar(line, column, chr));
}

extern hal_result_t hal_display_putstring(hal_display_dev_t dev, uint16_t line, uint16_t column, char* str)
{
    if(hal_true != s_hal_device_display_initted_is(dev))
    {
        return(hal_res_NOK_generic);
    }


    if(NULL == hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_dev2index(dev)].chipif.putstring)
    {
        return(hal_res_NOK_generic);
    }
    return(hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_dev2index(dev)].chipif.putstring(line, column, str));
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


extern uint32_t hal_device_display_hid_getsize(const hal_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern hal_result_t hal_device_display_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
//    if(NULL == memory)
//    {
//        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_xxx_hid_setmem(): memory missing");
//        return(hal_res_NOK_generic);
//    }

    // removed dependancy from NZI ram
    memset(s_hal_device_display_initted, hal_false, sizeof(s_hal_device_display_initted));

    return(hal_res_OK);  
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_device_display_supported_is(hal_display_dev_t dev)
{
    return(hal_utility_bits_byte_bitcheck(hal_brdcfg_device_display__theconfig.supported_mask, HAL_device_display_dev2index(dev)) );
}

static void s_hal_device_display_initted_set(hal_display_dev_t dev)
{
    s_hal_device_display_initted[HAL_device_display_dev2index(dev)] = hal_true;
}

static hal_boolval_t s_hal_device_display_initted_is(hal_display_dev_t dev)
{
    return(s_hal_device_display_initted[HAL_device_display_dev2index(dev)]);
}

static hal_result_t s_hal_device_display_hw_init(hal_display_dev_t dev, const hal_display_cfg_t *cfg)
{
    if((NULL != hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_dev2index(dev)].chipif.init))
    {
        return(hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_dev2index(dev)].chipif.init(hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_dev2index(dev)].chipif.initpar));
    }
    else
    {
        return(hal_res_NOK_generic);
    }
}



#endif//HAL_USE_DEVICE_DISPLAY

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



