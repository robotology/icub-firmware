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
    
#include "embot_common.h"
#include "embot_binary.h"
    
#include "embot_hw_bsp.h"
#include "embot_hw_led.h"
#include "embot_hw_sys.h"    

#if     defined(APPSLIM_STM32HAL_TICK_SYSTICK)
    
static volatile uint64_t s_1mstickcount = 0;
    
void SysTick_Handler(void)
{
    s_1mstickcount++;
}

static void tick1msecinit(void)
{
    HAL_SYSTICK_Config(SystemCoreClock/1000);
}
    
static uint32_t tick1msecget(void)
{
    return (uint32_t)s_1mstickcount;
}

#else

static void tick1msecinit(void)
{   
}

static uint32_t tick1msecget(void)
{   // one must always provide one function which counts forward
    static uint32_t n = 0;
    return n++;
}

#endif


static void some_other_demonstrations(void);


int main(void)
{ 
#if defined(APPSLIM_RELOCATEVECTORTABLE)
    embot::hw::sys::relocatevectortable(embot::hw::sys::addressOfApplication - embot::hw::sys::startOfFLASH);   
#endif
    
    embot::hw::bsp::Config bspconfig(tick1msecinit, tick1msecget);     
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
    
    led1ON = embot::binary::bit::check(mask32, 0); 

    
    embot::hw::led::on(embot::hw::led::LED::one);  

    embot::hw::led::on(embot::hw::led::LED::two);
    
    for(;;)
    {

        embot::hw::sys::delay(250*1000);  
        HAL_Delay(250);

        embot::hw::led::toggle(embot::hw::led::LED::one);
        
        if(PERIOD == ++count)
        {
            // in here, i want to demonstrate functions of the embot::binary::bit namespace which does bit manipulation of all integer types: 8, 16, 32, 64 bit long
            count = 0;
            embot::binary::bit::toggle(mask32, 0);              
            led1ON = embot::binary::bit::check(mask32, 0);     
            some_other_demonstrations();
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

// other static functions

static void some_other_demonstrations(void)
{
    // i want to count how many bits are set.
    const uint64_t themask64 = 0x1000000000110011; // surely five
    const uint32_t themask32 = 0x11000011; // surely four
    const uint16_t themask16 = 0x1011; // surely three
    const uint8_t themask08 = 0x11; // surely two
    
    uint8_t numberofonebits = 0;
    
    numberofonebits = embot::binary::bit::count(themask64);
    numberofonebits = embot::binary::bit::count(themask32);
    numberofonebits = embot::binary::bit::count(themask16);
    numberofonebits = embot::binary::bit::count(themask08);
    numberofonebits = numberofonebits;
    
    
    // i want to manipulate group of bits: nibbles
    // it is useful to assign nibbles in a position, to clear them
    
    uint64_t destination = 0;
    bool equal = false;
    embot::binary::nibble::assign(destination, 0x3, 4);
    destination = destination;
    
    embot::binary::nibble::assign(destination, 0xf, 3);
    destination = destination;
        
    equal = embot::binary::nibble::check(destination, 0x3, 4);
    equal = equal;
    
    embot::binary::nibble::assign(destination, 0x0, 4);
    destination = destination;    
    
    equal = embot::binary::nibble::check(destination, 0x3, 4);
    equal = equal;

    // i want to manipulate group of bits: masks of any form
    
    uint64_t msk = 0x010101;
    destination = 0;
    embot::binary::mask::set(destination, msk);
    destination = destination;
    equal = embot::binary::mask::check(destination, msk);
    equal = equal;
    embot::binary::mask::toggle(destination, static_cast<uint64_t>(0x111111));
    embot::binary::mask::toggle(destination, msk);
    embot::binary::mask::toggle(destination, msk);
    embot::binary::mask::toggle(destination, msk);
    destination = destination;

}




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




