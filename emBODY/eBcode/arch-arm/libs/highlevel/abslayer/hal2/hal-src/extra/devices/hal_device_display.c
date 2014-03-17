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

#include "hal_heap.h"
 
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

#define HAL_device_display_id2index(t)              ((uint8_t)((t)))


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
    hal_display_cfg_t           config;
} hal_device_display_internal_item_t;

typedef struct
{
    uint8_t                                 initted;
    hal_device_display_internal_item_t*     items[hal_displays_number];   
} hal_device_display_theinternals_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_device_display_supported_is(hal_display_t id);
static void s_hal_device_display_initted_set(hal_display_t id);
static hal_boolval_t s_hal_device_display_initted_is(hal_display_t id);

static hal_result_t s_hal_device_display_hw_init(hal_display_t id, const hal_display_cfg_t *cfg);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

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
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static hal_device_display_theinternals_t s_hal_device_display_theinternals =
{
    .initted            = 0,
    .items              = { NULL }   
};

//static uint8_t s_hal_device_display_initted = 0;
//static hal_device_display_internal_item_t* s_hal_device_display_internals[hal_displays_number] = { NULL };


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_display_init(hal_display_t id, const hal_display_cfg_t *cfg)
{
    hal_device_display_internal_item_t *intitem = s_hal_device_display_theinternals.items[HAL_device_display_id2index(id)];
    if(hal_true != s_hal_device_display_supported_is(id))
    {
        return(hal_res_NOK_unsupported);
    }
    
    if(NULL == cfg)
    {
        cfg = &hal_display_cfg_default;
    }
    
    // if it does not have ram yet, then attempt to allocate it.
    if(NULL == intitem)
    {
        intitem = s_hal_device_display_theinternals.items[HAL_device_display_id2index(id)] = hal_heap_new(sizeof(hal_device_display_internal_item_t));
        // minimal initialisation of the internal item
        // nothing to init.      
    } 

    // in here we should add a brdcfg_  function which takes care of initting the device used and the required periphs.
    // e.g., in mcbstm32c a given 320x240 display is used together with spi. the init function and pointers to 
    // other functions must be taken from hal_brdcfg_mcbstm32c.[c, h]

    if(hal_res_OK != s_hal_device_display_hw_init(id, cfg)) // contains GLCD_Init()
    {
        return(hal_res_NOK_generic);
    }
    
    memcpy(&intitem->config, cfg, sizeof(hal_display_cfg_t));
    
    s_hal_device_display_initted_set(id);
    
    hal_display_clearscreen(id);    
    hal_display_setfont(id, cfg->font);
    
    return(hal_res_OK);
}

extern hal_result_t hal_display_clearscreen(hal_display_t id)
{
    hal_device_display_internal_item_t *intitem = s_hal_device_display_theinternals.items[HAL_device_display_id2index(id)];
    
    if(hal_true != s_hal_device_display_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
   
    if(NULL == hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_id2index(id)].chipif.clear)
    {
        return(hal_res_NOK_generic);
    }
    
    hal_display_cfg_t *cfg = &intitem->config;
    
    return(hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_id2index(id)].chipif.clear(s_hal_device_display_colormap[cfg->screencolor]));    
}

extern hal_result_t hal_display_setfont(hal_display_t id, hal_display_font_t font)
{
    hal_device_display_internal_item_t *intitem = s_hal_device_display_theinternals.items[HAL_device_display_id2index(id)];
    
    if(hal_true != s_hal_device_display_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    
    if(NULL == hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_id2index(id)].chipif.settextproperties)
    {
        return(hal_res_NOK_generic);
    }
    
    memcpy(&intitem->config.font, &font, sizeof(hal_display_font_t));
    
    return(hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_id2index(id)].chipif.settextproperties(font.textsize, s_hal_device_display_colormap[font.backcolor], s_hal_device_display_colormap[font.textcolor]));        
}


extern hal_result_t hal_display_clearline(hal_display_t id, uint16_t line)
{
    if(hal_true != s_hal_device_display_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    if(NULL == hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_id2index(id)].chipif.clearline)
    {
        return(hal_res_NOK_generic);
    }
    return(hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_id2index(id)].chipif.clearline(line));
}

extern hal_result_t hal_display_putchar(hal_display_t id, uint16_t line, uint16_t column, char chr)
{
    if(hal_true != s_hal_device_display_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    
    if(NULL == hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_id2index(id)].chipif.putchar)
    {
        return(hal_res_NOK_generic);
    }
    return(hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_id2index(id)].chipif.putchar(line, column, chr));
}

extern hal_result_t hal_display_putstring(hal_display_t id, uint16_t line, uint16_t column, char* str)
{
    if(hal_true != s_hal_device_display_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }


    if(NULL == hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_id2index(id)].chipif.putstring)
    {
        return(hal_res_NOK_generic);
    }
    return(hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_id2index(id)].chipif.putstring(line, column, str));
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------



extern hal_result_t hal_device_display_hid_static_memory_init(void)
{
    memset(&s_hal_device_display_theinternals, 0, sizeof(s_hal_device_display_theinternals));
    return(hal_res_OK);  
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_device_display_supported_is(hal_display_t id)
{
    return(hal_utility_bits_byte_bitcheck(hal_brdcfg_device_display__theconfig.supported_mask, HAL_device_display_id2index(id)) );
}

static void s_hal_device_display_initted_set(hal_display_t id)
{
    hal_utility_bits_byte_bitset(&s_hal_device_display_theinternals.initted, HAL_device_display_id2index(id));
}

static hal_boolval_t s_hal_device_display_initted_is(hal_display_t id)
{
    return(hal_utility_bits_byte_bitcheck(s_hal_device_display_theinternals.initted, HAL_device_display_id2index(id)));
}

static hal_result_t s_hal_device_display_hw_init(hal_display_t id, const hal_display_cfg_t *cfg)
{
    if((NULL != hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_id2index(id)].chipif.init))
    {
        return(hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_id2index(id)].chipif.init(hal_brdcfg_device_display__theconfig.devcfg[HAL_device_display_id2index(id)].chipif.initpar));
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



