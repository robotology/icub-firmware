
/*
 * Copyright (C) 2018 iCub Facility - Istituto Italiano di Tecnologia
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
// - public interface
// --------------------------------------------------------------------------------------------------------------------


#include "lowlevel.h"
#include "stm32hal.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

    
#if defined(STM32HAL_STM32L4) || defined(STM32HAL_STM32H7) || defined(STM32HAL_STM32G4)

#if __ARMCOMPILER_VERSION > 6000000
int _execOperations(uint32_t i)
{
  int res = 0;
  __asm 
  (
    "DOWAITLOOP:                            \t\n"
    "SUBS %[input_i], %[input_i], #1        \t\n"
    "BNE DOWAITLOOP                         \t\n"
    : [result] "=&r" (res)
    : [input_i] "r" (i)
  );
  return 0;
}
#else
__asm void _execOperations(uint32_t numberof) 
{
   align
dowaitloop
   subs r0,r0,#1
   bne dowaitloop
   bx lr 
   align    
}
#endif

#else 
    #error you must define a _execOperations() function
#endif

    void asmEXECcycles(uint32_t numcycles)
    {
        _execOperations(numcycles);        
    }
  
    void lowlevel_delay(uint64_t t)
    {
        static uint64_t s_hl_sys_numofops1sec = 0;
        static uint32_t s_hl_sys_used_systemcoreclock = 0;
        if(s_hl_sys_used_systemcoreclock != SystemCoreClock)
        {

            // to occupy a millisec i execute an operation for a number of times which depends on: 
            // SystemCoreClock, cortex gain(1.25 dmips/mhz), flash access, etc.
            // to overcome all this i just consider SystemCoreClock w/out 1.25 gain and i measures
            // extra gain with on a simple assembly function which should take 4 cycles per iteration (?).      
            //s_hl_sys_numofops1sec = (5*(SystemCoreClock)) / 4; 
            s_hl_sys_used_systemcoreclock = SystemCoreClock;
            s_hl_sys_numofops1sec = SystemCoreClock;

            #if defined(STM32H7) & defined(CORE_CM7)
            // empirically removed ...
            #elif defined(STM32H7) & defined(CORE_CM4)
            // same as other CM4 cores: L4 and G4
            s_hl_sys_numofops1sec /= 3;
            #elif defined(STM32L4) 
            s_hl_sys_numofops1sec /= 3;
            #elif defined(STM32G4) 
            s_hl_sys_numofops1sec /= 3;
            #warning specify tuning must be verified
            #else
            #error specify tuning             
            #endif

            // at this point i normalise the variable to keep not the nymber of operations for 1 sec,
            // but for 1024*1024 microsec. by doing so, later on i shift by 20 instead of using a division. 
            s_hl_sys_numofops1sec <<= 20;
            s_hl_sys_numofops1sec /= 1000000;
        }
                
        volatile uint64_t num = s_hl_sys_numofops1sec * t;
        num >>= 20; 
        //num -= offset; //we may remove some cycles to compensates for previous instructions, but ... we dont do it. it depends on c compiler optimisation 
        if(0 == num)
        {
            return;
        }
        asmEXECcycles((uint32_t)(num));    
    }


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

