/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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


// this is a template file for advanced configuration of cmsis.
// it is possible to redefine two functions which are weakly defined in system_xxxx.c:
// void SystemInit(void):            is called in the reset handler before jumping to main and is responsible to initialise 
//                                   the system (e.g., set internal clocks, external memory etc).
// void SystemCoreClockUpdate(void): is responsible to update the variable SystemCoreClock according to configured registers. 


#include "hl_core.h"    // contains declarations in cmsis 
 
#include "hl_cfg_plus_target.h"  
#include "hl_cfg_plus_modules.h"   

// // redefined the SystemCoreClock to have value ...
// uint32_t SystemCoreClock = HSI_VALUE;
// //
// void SystemInit(void)
// {   
//     // does nothing, so it relies upon default configuration of registers
//     // then we just update the SystemCoreClock, so that we have correct timing at least in systick
//     // ... we must call SystemCoreClockUpdate() again inside main() otherwise SystemCoreClock is not updated.
//     SystemCoreClockUpdate();    
// }


#if     defined(HL_USE_BRD_EMS4RD)

// need to use a different SystemInit() because we dont have the normal clock as in mcbstm32f400 

//#include "ems004_sys.c"

#include "hl_gpio.h"
#include "hl_sys.h"
//#include "hal_trace.h"

extern void hl_system_stm32fx_before_setsysclock(void)
{
#if defined(EMS4RD_USE_MICREL_AS_MANAGED_DEVICE)    
    SystemCoreClockUpdate();
    hl_gpio_t ethslv = {.port = hl_gpio_portB, .pin = hl_gpio_pin2};
    hl_gpio_pin_output_init(ethslv);
    
    hl_gpio_t notethrst = {.port = hl_gpio_portH, .pin = hl_gpio_pin1};
    hl_gpio_pin_output_init(notethrst);
    
    hl_gpio_pin_write(ethslv, hl_gpio_valSET);
    hl_gpio_pin_write(notethrst, hl_gpio_valRESET);
    hl_sys_delay(110*1000);    
    hl_gpio_pin_write(notethrst, hl_gpio_valSET);
    uint32_t i = 0;
    i++;
//    hal_trace_puts("executed hl_system_stm32fx_before_setsysclock() w/ code for EMS4RD_USE_MICREL_AS_MANAGED_DEVICE"); 
#endif    
}

#endif//defined(HL_USE_BRD_EMS4RD)



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






