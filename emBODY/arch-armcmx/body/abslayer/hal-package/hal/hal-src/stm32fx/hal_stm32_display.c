/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

/* @file       hal_display.c
	@brief      This file implements internal implementation of the hal gpio module.
	@author     marco.accame@iit.it
    @date       06/09/2010
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_DISPLAY

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"

#include "stm32f1.h"

#include "stm32_utility/hal_stm32_display_GLCD.h"

#include "hal_stm32_base_hid.h" 

#include "hal_brdcfg.h"
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_display.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_stm32_display_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_display_cfg_t hal_display_cfg_default =
{
    .res              = hal_display_res_320x240x24
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_display_t2index(t)              ((uint8_t)((t)))

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_display_supported_is(void);
static void s_hal_display_initted_set(void);
static hal_boolval_t s_hal_display_initted_is(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_display_initted = hal_false;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_display_init(const hal_display_cfg_t *cfg)
{
    if(1 == hal_base_hid_params.display_enable)
    {
        if(hal_true != s_hal_display_supported_is())
        {
            return(hal_res_NOK_unsupported);
        }

        GLCD_Init(); 
        s_hal_display_initted_set();
        return(hal_res_OK);
    }
    else
    {
        return(hal_res_NOK_generic);
    }
   
}

extern hal_result_t hal_display_clear(hal_display_color_t color)
{
    if(hal_true != s_hal_display_initted_is())
    {
        return(hal_res_NOK_generic);
    }

    GLCD_Clear(color);
    
    return(hal_res_OK);
}

extern hal_result_t hal_display_settext(hal_display_font_t fnt, hal_display_color_t backcolor, hal_display_color_t textcolor)
{
    if(hal_true != s_hal_display_initted_is())
    {
        return(hal_res_NOK_generic);
    }

    GLCD_SetBackColor(backcolor);
    GLCD_SetTextColor(textcolor);

    return(hal_res_OK);
}

///** @fn         extern void hal_display_SetTextColor(hal_display_color_t color)
//    @brief      ??? with color @e color.  
//    @param      color       The color which ???.
//    @warning    Use only for debug purposes on the MCBSTM32C board
// **/
//extern void hal_display_SetTextColor(hal_display_color_t color);
//extern void hal_display_SetTextColor(hal_display_color_t color)
//{
//    if(1 == hal_base_hid_params->display_enable)
//    {
//        GLCD_SetTextColor(color);
//    }
//}

//
//extern void hal_display_SetBackColor(hal_display_color_t color)
//{
//    if(1 == hal_base_hid_params->display_enable)
//    {
//        GLCD_SetBackColor(color);
//    }
//}



extern hal_result_t hal_display_clearline(uint32_t ln)
{
    if(hal_true != s_hal_display_initted_is())
    {
        return(hal_res_NOK_generic);
    }

    GLCD_ClearLn(ln);

    return(hal_res_OK);
}

extern hal_result_t hal_display_putchar(uint32_t ln, uint8_t  c)
{
    if(hal_true != s_hal_display_initted_is())
    {
        return(hal_res_NOK_generic);
    }

    GLCD_DisplayChar(ln, 0, c);

    return(hal_res_OK);
}

extern hal_result_t hal_display_putstring(uint32_t ln, uint8_t  *s)
{
    if(hal_true != s_hal_display_initted_is())
    {
        return(hal_res_NOK_generic);
    }

    GLCD_DisplayString(ln, 0, s);
 
    return(hal_res_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


extern uint32_t hal_display_hid_getsize(const hal_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern hal_result_t hal_display_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
//    if(NULL == memory)
//    {
//        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_xxx_hid_setmem(): memory missing");
//        return(hal_res_NOK_generic);
//    }

    // removed dependancy from NZI ram
    s_hal_display_initted = hal_false;

    return(hal_res_OK);  
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_display_supported_is(void)
{
    return(hal_base_hid_byte_bitcheck(hal_brdcfg_display__supported_mask, HAL_display_t2index(0)) );
}

static void s_hal_display_initted_set(void)
{
    s_hal_display_initted = hal_true;
}

static hal_boolval_t s_hal_display_initted_is(void)
{
    return(s_hal_display_initted);
}



#endif//HAL_USE_DISPLAY
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



