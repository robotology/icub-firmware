
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

#include <cstring>
#include <vector>

using namespace std;

#include "embot_hw_bsp_config.h"
#include "embot_hw_flash_bsp.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

namespace embot::hw::sys::DRIVER {    
    // these depend on the hal layer beneath. it may be either USE_HAL_DRIVER ore USE_hal2_DRIVER 
    std::uint32_t _clock(embot::hw::CLOCK clk);
    void _reset();   
    bool _canjump2address(embot::hw::flash::ADDR address);    
    void _jump2address(embot::hw::flash::ADDR address);   
    void _jump2address2(embot::hw::flash::ADDR address);    
    void _relocatevectortable(std::uint32_t offset); 
    uint64_t _uniqueid(); 
    void _delay(embot::core::relTime t);
    std::uint32_t _random();
    void _irq_disable();
    void _irq_enable();
}

constexpr std::uint32_t maxRANDmask = 0x3ff; // 1023 

namespace embot::hw::sys { 
                 
    std::uint32_t clock(embot::hw::CLOCK clk)
    {
        return DRIVER::_clock(clk);
    }
    
    void reset()
    {        
        DRIVER::_reset();
    }
    
    bool canjump2address(embot::hw::flash::ADDR address)
    {
        return DRIVER::_canjump2address(address);
    }
    
    void jump2address(embot::hw::flash::ADDR address)
    {
        DRIVER::_jump2address(address);          
    }
    
    void jump2address2(embot::hw::flash::ADDR address)
    {        
        DRIVER::_jump2address2(address);
    }
    
    void relocatevectortable(std::uint32_t offset)
    {
        DRIVER::_relocatevectortable(offset);        
    }
    
    uint64_t uniqueid()
    {
        return DRIVER::_uniqueid();
    }    

    
    void delay(embot::core::relTime t)
    {   
        DRIVER::_delay(t);
    }
         
    std::uint32_t random()
    {
        return DRIVER::_random();
    }
    
    std::uint32_t minrandom()
    {
        return 0;
    }
    
    std::uint32_t maxrandom()
    {
        return maxRANDmask;
    }
    
    //[[deprecated( "use embot::core::print()" )]]
    int puts(const std::string &str) 
    {
        return embot::core::print(str);
    }
    
    //[[deprecated( "use embot::core::print()" )]]
    int puts(const char* str) 
    {  
        return embot::core::print(str);

    }

    void irq_disable()
    {
        DRIVER::_irq_disable();
    }

    void irq_enable()
    {
        DRIVER::_irq_enable();
    }
       
    
} // namespace embot::hw::sys { 


namespace embot::hw::sys {
            
    void relocatevectortable(embot::hw::FLASHpartitionID id)
    {
        uint32_t addressofapplication = embot::hw::sys::partition(id).address;
        uint32_t defaultvectorlocation = embot::hw::sys::bank(addressofapplication).address;
        if((embot::hw::flash::InvalidADDR == addressofapplication) || (embot::hw::flash::InvalidADDR == defaultvectorlocation))
        {
            return;
        }        
        if(addressofapplication > defaultvectorlocation)
        {
            std::uint32_t vectorlocation = addressofapplication - defaultvectorlocation;
            if(0 != vectorlocation)
            {
                embot::hw::sys::relocatevectortable(vectorlocation);
            }
        }          
    }

} // namespace embot::hw::sys {


namespace embot::hw::sys {
        
    const embot::hw::flash::Partition& partition(const embot::hw::flash::Partition::ID id)
    {
        const embot::hw::flash::Partition *p = embot::hw::flash::bsp::getBSP().partition(id);
        return (nullptr == p) ? embot::hw::flash::InvalidPartition : *p;
    }

    const embot::hw::flash::Partition& partition(const embot::hw::flash::ADDR address)
    {
        const embot::hw::flash::Partition *p = embot::hw::flash::bsp::getBSP().partition(address);
        return (nullptr == p) ? embot::hw::flash::InvalidPartition : *p;
    }
    
    const embot::hw::flash::Bank & bank(const embot::hw::flash::ADDR address)
    {
        const embot::hw::flash::bsp::BankDescriptor * bd = embot::hw::flash::bsp::getBSP().bank(address);             
        return (nullptr == bd) ? embot::hw::flash::InvalidBank : *bd;         
    }
    
    const embot::hw::flash::Bank & bank(const embot::hw::flash::Bank::ID id)
    {
        const embot::hw::flash::bsp::BankDescriptor * bd = embot::hw::flash::bsp::getBSP().bank(id);             
        return (nullptr == bd) ? embot::hw::flash::InvalidBank : *bd; 
    }   
       
    embot::hw::flash::Page page(const embot::hw::flash::ADDR address) 
    {          
        const embot::hw::flash::bsp::BankDescriptor * bd = embot::hw::flash::bsp::getBSP().bank(address);   
        if(nullptr != bd)
        {
            return bd->page(address);
        }               
        return {}; 
    }
 
    embot::hw::flash::ADDR address(const embot::hw::flash::Bank::ID id, const embot::hw::flash::Page::Index index) 
    {
        const embot::hw::flash::bsp::BankDescriptor * bd = embot::hw::flash::bsp::getBSP().bank(id);
        return (nullptr == bd) ? embot::hw::flash::InvalidADDR : bd->index2page(index).address;             
    }      
    
}   // namespace embot::hw::sys { 



// and now comes what depends on the hal

#if defined(USE_HAL_DRIVER)    

#include "embot_hw_lowlevel.h"

namespace embot::hw::sys::DRIVER {   

    // these depend on the hal layer beneath
    std::uint32_t _clock(embot::hw::CLOCK clk)
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
    
    void _reset()
    {
        NVIC_SystemReset();
    }
    
    bool _canjump2address(embot::hw::flash::ADDR address)
    {
        volatile std::uint32_t * firstword32 = reinterpret_cast<volatile std::uint32_t *>(address);
        if(0xffffffff == *firstword32)
        {   // so far i disable only the jumpt to erase flash
            return false;
        }
        return true;
    }
    
    void _jump2address(embot::hw::flash::ADDR address)
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
    
    void _jump2address2(embot::hw::flash::ADDR address)
    {
        if(false == canjump2address(address))
        {
            return;
        }
        
        // as found in https://developer.arm.com/documentation/ka001423/1-0
        // it seems that allows the application to use IRQ handlers and RTOS
        
        // make sure that the address argument is safely copied into memory before we change the stack
        volatile uint32_t flashstart = address;
        static volatile std::uint32_t resetlocation = 0;
        resetlocation = *(volatile std::uint32_t*) (flashstart + 4);
        // now change everything
        __disable_irq();
        memset((uint32_t *)NVIC->ICER, 0xFF, sizeof(NVIC->ICER));
        SysTick->CTRL = 0;
        SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;
        SCB->VTOR = FLASH_BASE | (flashstart & (uint32_t)0x1FFFFF80);
        __set_MSP( ((unsigned int *)(SCB->VTOR))[0] );
        __set_CONTROL( 0 );
        __enable_irq();
        
        // and jump
        void (*reset_fn)(void) = NULL;
        reset_fn = (void (*)(void)) resetlocation;        
        reset_fn();  
        
        // never in here.
        return;          
    }

    void _relocatevectortable(std::uint32_t offset)
    {
        SCB->VTOR = FLASH_BASE | (offset & (uint32_t)0x1FFFFF80);
    }
    
    uint64_t _uniqueid()
    {
        uint64_t r = 0;
#if defined(STM32H7)    
        r = HAL_GetUIDw0() | static_cast<uint64_t>(HAL_GetUIDw1()) << 32;
        r += HAL_GetUIDw2();
#endif    
        return r;  
    }
    
    void _delay(embot::core::relTime t)
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
        embot::hw::lowlevel::asmEXECcycles(static_cast<uint32_t>(num));    
    }

    std::uint32_t _random()
    {
#if defined(HAL_RNG_MODULE_ENABLED) && !defined(EMBOT_ENABLE_hw_sys_emulateRAND)
        std::uint32_t v = 0;        
        HAL_RNG_GenerateRandomNumber(&hrng, &v);
#else
        std::uint32_t v; // not initialised on purpose. it gets the value on the stack
        // marco.accame: i dont remember why i dont use srand() and rand() ...
#endif        
        return v & maxRANDmask;
    }
    
    void _irq_disable()
    {
        __disable_irq();
    }

    void _irq_enable()
    {
        __enable_irq();
    }
}

#elif defined(USE_hal2_DRIVER)
 

//#define HL_USE_MPU_NAME_STM32F407IG
//#include "hl_core.h"
// sadly i cannot use hl w/ compiler v6 and c++, so i wiil not use basic cmsis functions

extern uint32_t SystemCoreClock;
    
namespace embot::hw::sys::DRIVER {    
       
    std::uint32_t _clock(embot::hw::CLOCK clk)
    {
        std::uint32_t value = 0;
        switch(clk)
        {            
            case embot::hw::CLOCK::syscore:
            {
                value = SystemCoreClock;
            } break;   

            default:              
            case embot::hw::CLOCK::pclk1:
            case embot::hw::CLOCK::pclk2:
            {
            } break;            
        }   
        return value;
    }
    
    void _reset()
    {        
        hal_sys_systemreset();
    }
    
    bool _canjump2address(embot::hw::flash::ADDR address)
    {
        volatile std::uint32_t * firstword32 = reinterpret_cast<volatile std::uint32_t *>(address);
        if(0xffffffff == *firstword32)
        {   // so far i disable only the jumpt to erase flash
            return false;
        }
        return true;
    }
    
    void _jump2address(embot::hw::flash::ADDR address)
    {
        if(false == canjump2address(address))
        {
            return;
        }

        hal_sys_executenowataddress(address);
        
        // never in here.
        return;                 
    }
    
    void _jump2address2(embot::hw::flash::ADDR address)
    {        
        _jump2address(address);         
    }    
    
    void _relocatevectortable(std::uint32_t offset)
    {
        hal_sys_vectortable_relocate(offset);
    }
        
    uint64_t _uniqueid()
    {  
        return hal_uniqueid_id64bit_get();        
    }    
    
    void _delay(embot::core::relTime t)
    {   
        hal_sys_delay(t);
    }    
       
    std::uint32_t _random()
    {
        std::uint32_t v; // not initialised on purpose. it gets the value on the stack  
        return v & maxRANDmask;
    }    
        
    void _irq_disable()
    {
        hal_sys_irq_disable();
    }

    void _irq_enable()
    {
        hal_sys_irq_enable();
    }    

} // namespace embot::hw::sys {

#else
    #error either HAL or hal2 driver
#endif    


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

