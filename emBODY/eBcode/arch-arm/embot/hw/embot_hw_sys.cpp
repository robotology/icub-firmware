/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
#include "stm32hal.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>

using namespace std;


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace hw { namespace sys {
    
    
#if     defined(STM32HAL_BOARD_NUCLEO64)
    
    const std::uint32_t startOfFLASH            = 0x08000000;
    const std::uint32_t addressOfBootloader     = 0x08000000;
    const std::uint32_t maxsizeOfBootloader     = 124*1024;
    const std::uint32_t addressOfStorage        = 0x0801F000;
    const std::uint32_t maxsizeOfStorage        = 4*1024;
    const std::uint32_t addressOfApplication    = 0x08020000;
    const std::uint32_t maxsizeOfApplication    = 128*1024;
    
#elif   defined(STM32HAL_BOARD_MTB4)
    
    const std::uint32_t startOfFLASH            = 0x08000000;
    const std::uint32_t addressOfBootloader     = 0x08000000;
    const std::uint32_t maxsizeOfBootloader     = 124*1024;
    const std::uint32_t addressOfStorage        = 0x0801F000;
    const std::uint32_t maxsizeOfStorage        = 4*1024;
    const std::uint32_t addressOfApplication    = 0x08020000;
    const std::uint32_t maxsizeOfApplication    = 128*1024;

#elif   defined(STM32HAL_BOARD_STRAIN2)
    
    const std::uint32_t startOfFLASH            = 0x08000000;
    const std::uint32_t addressOfBootloader     = 0x08000000;
    const std::uint32_t maxsizeOfBootloader     = 124*1024;
    const std::uint32_t addressOfStorage        = 0x0801F000;
    const std::uint32_t maxsizeOfStorage        = 4*1024;
    const std::uint32_t addressOfApplication    = 0x08020000;
    const std::uint32_t maxsizeOfApplication    = 128*1024;
    
    
#else
    #error you must define some embot::hw::sys constants   
#endif

 
#if     defined(STM32HAL_BOARD_NUCLEO64) || defined(STM32HAL_BOARD_MTB4) || defined(STM32HAL_BOARD_STRAIN2)
    
__asm static void ss_hl_sys_asm_xnumARMv7ops(uint32_t numberof) 
{
   align
dowaitloop
   subs r0,r0,#1
   bne dowaitloop
   bx lr 
   align    
}

    static void ss_bsp_delay(uint64_t t)
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
            
            // with art technology enabled the flash is seen as fast as the cpu. wow. 
            s_hl_sys_numofops1sec /= 3;             


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
        ss_hl_sys_asm_xnumARMv7ops((uint32_t)num);
    }
    
#endif
    
    std::uint32_t clock(CLOCK clk)
    {
        std::uint32_t value = 0;
        switch(clk)
        {
            case embot::hw::sys::CLOCK::pclk1:
            {
                value = HAL_RCC_GetPCLK1Freq();
            } break;
            
            case embot::hw::sys::CLOCK::pclk2:
            {
                value = HAL_RCC_GetPCLK2Freq();
            } break;
            
            default:
            case embot::hw::sys::CLOCK::syscore:
            {
                value = HAL_RCC_GetHCLKFreq();
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


    void delay(embot::common::Time t)
    {   
        ss_bsp_delay(t);
    }
      
    static const std::uint32_t maxRANDmask = 0x3ff; // 1023    
    std::uint32_t random()
    {
        std::uint32_t v = 0;        
        HAL_StatusTypeDef ret = HAL_RNG_GenerateRandomNumber(&hrng, &v);
        ret = ret;        
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
    

}}} // namespace embot { namespace hw { namespace sys { 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

