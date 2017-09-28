/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
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


// --------------------------------------------------------------------------------------------------------------------
// - description of application
//
// this is a very simple application running at embot::hw::sys::addressOfApplication (0x0800000 + 128KB) (or +0KB
// if teh target is 0offset) which uses library stm32hal initialed thriugh a thin c++-11 layer (embot::hw namespace)

// if APPSLIM_DO_NOTHING is defined .... nothing is done.
// instead, if undefined the application just manage a led and a button. by pressing and releasing btn we toggle 
// the status of led on/off. when the led is on it actually pulses at 4 hz

// the mtb4 board does not have a button, hence i have commented it out
// the mtb4 board does not support (?) trace, hence i have removed eventviewer.



#undef  APPSLIM_DO_NOTHING
#undef  APPSLIM_USE_BUTTON
#undef  APPSLIM_USE_EVENTVIEWER



#if     defined(APPSLIM_STM32HAL_TICK_NONE)
    #warning  APPSLIM_STM32HAL_TICK_NONE is defined: this application will work, but some funtions of stm32hal which use HAL_GetTick() will not work properly
#elif   defined(APPSLIM_STM32HAL_TICK_SYSTICK)
    #warning  APPSLIM_STM32HAL_TICK_SYSTICK is defined: we start the systick at 1 ms
#else    
    #error: choose a APPSLIM_STM32HAL_TICK_* mode amongst: [APPSLIM_STM32HAL_TICK_NONE, APPSLIM_STM32HAL_TICK_SYSTICK] 
#endif  


// - includes

#include "stm32hal.h" 
#include "embot_hw.h"
#include "embot_hw_bsp.h"
#include "embot_hw_led.h"
#include "embot_hw_sys.h"    

#if     defined(APPSLIM_STM32HAL_TICK_SYSTICK)
    
static volatile uint64_t s_1mstickcount = 0;
    
void SysTick_Handler(void)
{
    s_1mstickcount++;
}

static uint32_t tick1msecget(void)
{
    static bool started = false;
    if(!started)
    {
        started = true;
        HAL_SYSTICK_Config(SystemCoreClock/1000);
    }
    return (uint32_t)s_1mstickcount;
}

#else

static uint32_t tick1msecget(void)
{
    return 0;
}

#endif

  

int main(void)
{ 
#if defined(APPSLIM_RELOCATEVECTORTABLE)
    embot::hw::sys::relocatevectortable(embot::hw::sys::addressOfApplication - embot::hw::sys::startOfFLASH);   
#endif
    
    embot::hw::bsp::Config bspconfig(tick1msecget);    
    embot::hw::bsp::init(bspconfig);
    

#if     defined(APPSLIM_DO_NOTHING)
    
    // description: do nothing but executing a dummy loop
    
    for(;;);   
    
#else
    
    // description: 
    // led zero (red) always pulses at 1 hz                                 
    // led one (blue) pulses at 1 hz every five seconds for five seconds.  
    
    // RED      []_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_[]_
    // BLUE     []_[]_[]_[]_[]________________[]_[]_[]_[]_[]________________[]_[]_[]_[]_[]________________[]_[]_[]_[]_[]________________[]_[]_[]_[]_[]_

    
    embot::hw::led::init(embot::hw::led::LED::one);
    embot::hw::led::init(embot::hw::led::LED::two);
    
    static bool led1ON = true;
    static uint32_t mask32 = 0x01;    
    static uint32_t count = 0;
    static const uint32_t PERIOD = 10;
    
    led1ON = embot::common::bit::check(mask32, 0);  
    
    embot::hw::led::on(embot::hw::led::LED::one);  

    embot::hw::led::on(embot::hw::led::LED::two);
    
    for(;;)
    {

        embot::hw::sys::delay(500*1000);  

        embot::hw::led::toggle(embot::hw::led::LED::one);
        
        if(PERIOD == ++count)
        {
            // in here, i want to demonstrate functions of the embot::common::bit namespace which do bit manipulation of all integer types: 8, 16, 32, 64 bit long
            count = 0;
            embot::common::bit::toggle(mask32, 0);              
            led1ON = embot::common::bit::check(mask32, 0);                 
        }
                
        if(true == led1ON)
        {                                        
            embot::hw::led::toggle(embot::hw::led::LED::two);
        } 
        else
        {
            embot::hw::led::off(embot::hw::led::LED::two);
        }    
                    
    }
    
#endif
    
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




