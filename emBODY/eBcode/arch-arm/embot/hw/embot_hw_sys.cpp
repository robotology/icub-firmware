
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
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_sys.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stm32hal.h"

#include <cstring>
#include <vector>

using namespace std;

#include "embot_hw_bsp.h"
#include "embot_hw_bsp_config.h"
#include "embot_hw_lowlevel.h"



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace hw { namespace sys {
       
    
    static void ss_delay(uint64_t t)
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

            #if defined(STM32H7)
            // empirically removed ...
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
        embot::hw::lowlevel::asmEXECcycles(static_cast<uint32_t>(num));
    }
    
    
    std::uint32_t clock(embot::hw::CLOCK clk)
    {
        std::uint32_t value = 0;
        switch(clk)
        {
            case embot::hw::CLOCK::pclk1:
            {
                value = HAL_RCC_GetPCLK1Freq();
            } break;
            
            case embot::hw::CLOCK::pclk2:
            {
                value = HAL_RCC_GetPCLK2Freq();
            } break;
            
            default:
            case embot::hw::CLOCK::syscore:
            {
                //value = HAL_RCC_GetHCLKFreq(); it has a bug for H7 ??
                value = SystemCoreClock;
            } break;            
        }   
        return value;
    }
    
    void reset()
    {        
        NVIC_SystemReset();
    }
    
    bool canjump2address(std::uint32_t address)
    {
        volatile std::uint32_t * firstword32 = reinterpret_cast<volatile std::uint32_t *>(address);
        if(0xffffffff == *firstword32)
        {   // so far i disable only the jumpt to erase flash
            return false;
        }
        return true;
    }
    
    void jump2address(std::uint32_t address)
    {
        volatile std::uint32_t jumpaddr;
        void (*app_fn)(void) = NULL;
        
        if(false == canjump2address(address))
        {
            return;
        }


        // prepare jump address 
        jumpaddr = *(volatile std::uint32_t*) (address + 4);
        // prepare jumping function
        app_fn = (void (*)(void)) jumpaddr;
        // initialize user application's stack pointer 
        __set_MSP(*(volatile uint32_t*) address);
        // jump.
        app_fn(); 
        
        // never in here.
        return;                 
    }
    
    void relocatevectortable(std::uint32_t offset)
    {
        SCB->VTOR = FLASH_BASE | (offset & (uint32_t)0x1FFFFF80);        
    }

//    embot::core::Time now()
//    {
//        return embot::hw::bsp::now();        
//    }

    void delay(embot::core::relTime t)
    {   
        ss_delay(t);
    }
      
    constexpr std::uint32_t maxRANDmask = 0x3ff; // 1023    
    std::uint32_t random()
    {
#if defined(HAL_RNG_MODULE_ENABLED)   
        std::uint32_t v = 0;        
        HAL_RNG_GenerateRandomNumber(&hrng, &v);
#else
        std::uint32_t v; // not initialised on purpose. it gets the value on the stack
        // marco.accame: i dont remember why i dont use srand() and rand() ...
#endif        
        return v & maxRANDmask;
    }
    
    std::uint32_t minrandom()
    {
        return 0;
    }
    
    std::uint32_t maxrandom()
    {
        return maxRANDmask;
    }
    
    int puts(const std::string &str) 
    {
        return puts(str.c_str());
    }
    
    int puts(const char* str) 
    {    
 
        if(nullptr == str)
        {
            return(0);
        }

    
        std::uint32_t ch;
        int num = 0;
        while('\0' != (ch = *str))
        {
            ITM_SendChar(ch);
            str++;
            num++;
        }
         
        ITM_SendChar('\n');
        return(++num);    
    }

    

}}} // namespace embot { namespace hw { namespace sys { 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

