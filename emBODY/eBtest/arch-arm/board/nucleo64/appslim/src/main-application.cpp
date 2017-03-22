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
// this is a very simple application running at embot::hw::sys::addressOfApplication (0x0800000 + 128KB) which uses 
// library stm32hal and c++-11 embot::hw namespace
// if APPSLIM_DO_NOTHING is defined .... nothing is done.
// instead, if undefined the application just manage a led and a button. by pressing and releasing btn we toggle 
// the status of led on/off. when the led is on it actually pulses at 4 hz



#undef APPSLIM_DO_NOTHING

#if     defined(APPSLIM_STM32HAL_TICK_NONE)
    #warning  APPSLIM_STM32HAL_TICK_NONE is defined: this application will work, but some funtions of stm32hal which use HAL_GetTick() will not work properly
#elif   defined(APPSLIM_STM32HAL_TICK_SYSTICK)
    #warning  APPSLIM_STM32HAL_TICK_SYSTICK is defined: we start the systick at 1 ms
#else    
    #error: choose a APPSLIM_STM32HAL_TICK_* mode amongst: [APPSLIM_STM32HAL_TICK_NONE, APPSLIM_STM32HAL_TICK_SYSTICK] 
#endif  


// - includes

#include "stm32hal.h" 
#include "eventviewer.h"
#include "embot_hw.h"
     

#if     defined(APPSLIM_STM32HAL_TICK_SYSTICK)
    
static volatile uint64_t s_1mstickcount = 0;
    
void SysTick_Handler(void)
{
    s_1mstickcount++;
}

static uint32_t tick1secget(void)
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

static uint32_t tick1secget(void)
{
    return 0;
}

#endif

    
void idle(void){}
void sig1(void){}
    

int main(void)
{ 
    embot::hw::sys::relocatevectortable(embot::hw::sys::addressOfApplication - embot::hw::sys::startOfFLASH);   
    
    embot::hw::bsp::Config bspconfig(tick1secget);    
    embot::hw::bsp::init(bspconfig);
    
    eventviewer_init();
    eventviewer_load(ev_ID_idle, idle);
    eventviewer_load(ev_ID_first_usrdef, sig1);
    eventviewer_switch_to(ev_ID_idle);

#if     defined(APPSLIM_DO_NOTHING)
    
    // description: do nothing but executing a dummy loop
    
    for(;;);   
    
#else
    
    // description: by pressing btn we toggle the status of led on/off at release. 
    // when the led is on it actually pulses at 4 hz
    // we use funtion HAL_Delay() 
    
    
    embot::hw::button::init(embot::hw::button::BTN::zero);    

    embot::hw::led::init(embot::hw::led::LED::zero);
    
    static bool ledON = true;
    for(;;)
    {
                
        if(ledON)
        {
            embot::hw::sys::delay(50*1000);
            evEntityId_t prev = eventviewer_switch_to(ev_ID_first_usrdef);
            embot::hw::sys::delay(50*1000);
            eventviewer_switch_to(prev);
            
            embot::hw::led::toggle(embot::hw::led::LED::zero);
        }        
            
        
        if(true == embot::hw::button::pressed(embot::hw::button::BTN::zero))
        {
            while(true == embot::hw::button::pressed(embot::hw::button::BTN::zero));
            
            if(ledON)
            {
                ledON = false;
                embot::hw::led::off(embot::hw::led::LED::zero);
            }
            else
            {
                ledON = true;
                embot::hw::led::on(embot::hw::led::LED::zero);
            }
            
        }                
    }
    
#endif
    
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




