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

/* @file       hal_trace.c
	@brief      This file implements internal implementation of the hal trace module.
	@author     marco.accame@iit.it
    @date       09/12/2011
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_PERIPH_TRACE

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"

#include "hal_middleware_interface.h"
#include "hal_brdcfg.h"
#include "hal_base_hid.h" 

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_trace.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_periph_trace_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern volatile int32_t ITM_RxBuffer = ITM_RXBUFFER_EMPTY;

const hal_trace_cfg_t hal_trace_cfg_default =
{
    .dummy  = 0
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    uint8_t         nothing;     
} hal_trace_theinternals_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_trace_supported_is(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// static hal_trace_theinternals_t s_hal_trace_theinternals =
// {
//     .nothing = 0
// };

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_trace_init(const hal_trace_cfg_t* cfg)
{
    
    if(hal_true != s_hal_trace_supported_is())
    {
        return(hal_res_NOK_unsupported);
    }
    
       
    if(NULL == cfg)
    {
        cfg = &hal_trace_cfg_default;
    }

    
    ITM_RxBuffer = ITM_RXBUFFER_EMPTY;
    
    return(hal_res_OK);
}


extern int hal_trace_getchar(void) 
{
    if(hal_true != s_hal_trace_supported_is())
    {
        return(-1);
    }

    while(ITM_CheckChar() != 1);
    return(ITM_ReceiveChar());
}    


extern int hal_trace_putchar(int ch) 
{
    if(hal_true != s_hal_trace_supported_is())
    {
        return(-1);
    }
    return(ITM_SendChar(ch));    
}

extern int hal_trace_puts(const char * str) 
{
    if(hal_true != s_hal_trace_supported_is())
    {
        return(0);
    }
    
    uint32_t ch;
    int16_t num = 0;
    while('\0' != (ch = *str))
    {
        ITM_SendChar(ch);
        str++;
        num++;
    }
     
    ITM_SendChar('\n');
    return(++num);    
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


extern hal_result_t hal_trace_hid_static_memory_init(void)
{
    hal_trace_init(NULL);
    return(hal_res_OK);  
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_trace_supported_is(void)
{
    return(hal_brdcfg_trace__theconfig.supported); 
}



#endif//HAL_USE_PERIPH_TRACE

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



