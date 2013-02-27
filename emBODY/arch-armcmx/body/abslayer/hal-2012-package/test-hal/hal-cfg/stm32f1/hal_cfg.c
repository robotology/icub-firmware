/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

/* @file       hal_cfg.c
	@brief      This file keeps internal implementation of the hal.
	@author     marco.accame@iit.it
    @date       09/09/2011
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "hal.h"
#include "hal_base.h"
#include "stdlib.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_cfg.h"


static void s_hal_cfg_on_fatalerror(hal_fatalerror_t errorcode, const char * errormsg);


extern const hal_base_cfg_t hal_base_cfg = 
{   
    .stacksize              = HAL_STACKSIZE,
    .heapsize               = HAL_HEAPSIZE,
    .extfn                  =
    {
        .usr_on_fatal_error                 = s_hal_cfg_on_fatalerror,
        .osal_system_scheduling_suspend     = NULL,
        .osal_system_scheduling_restart     = NULL
    }   
};


extern const hal_base_cfg_t *hal_base_cfgMINE = &hal_base_cfg;


void SysTick_Handler(void)
{
    hal_void_fp_void_t systickhandler = hal_sys_systick_gethandler();

    if(NULL != systickhandler)
    {
        systickhandler();
    }
}


static void s_hal_cfg_on_fatalerror(hal_fatalerror_t errorcode, const char * errormsg)
{
    errorcode = errorcode;
    if(NULL != errormsg)
    {
//        hal_display_putstring(4, (uint8_t*)errormsg);
    }

    for(;;);

}







// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



