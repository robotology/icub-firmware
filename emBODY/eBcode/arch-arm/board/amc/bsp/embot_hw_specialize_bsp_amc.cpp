/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:    Marco Accame
 * email:     marco.accame@iit.it
 * Coauthor:  Kevin Sangalli
 * email:     kevin.sangalli@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>
#include <array>

#include "embot_core_binary.h"
#include "embot_core.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif


using namespace std;
using namespace embot::core::binary;

// --------------------------------------------------------------------------------------------------------------------
// - configuration of peripherals and chips. it is done board by board. it contains a check vs correct STM32HAL_BOARD_*
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_amc_config.h"


// --------------------------------------------------------------------------------------------------------------------
// - board specific methods
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_amc.h"
#include "embot_hw_icc_printer.h"

namespace embot::hw::bsp::amc {
    
    OnSpecialize defOnSpec {};
    
    void set(const OnSpecialize& onsp)
    {
        defOnSpec = onsp;
    }
    
}


// --------------------------------------------------------------------------------------------------------------------
// - specialize the bsp
// --------------------------------------------------------------------------------------------------------------------

//#if defined(EMBOT_ENABLE_hw_J5_powersupply)

#include "embot_hw_gpio.h"
void J5power(bool on)
{
    constexpr embot::hw::GPIO J5pc {embot::hw::GPIO::PORT::F, embot::hw::GPIO::PIN::five};
    
    constexpr embot::hw::gpio::Config J5pcCfg {
        embot::hw::gpio::Mode::OUTPUTpushpull, 
        embot::hw::gpio::Pull::nopull, 
        embot::hw::gpio::Speed::medium
    };
    static bool initted {false};
    
    if(!initted)
    {
        embot::hw::gpio::init(J5pc, J5pcCfg);
        initted = true;
    }
    
    embot::hw::gpio::set(J5pc, on ? embot::hw::gpio::State::SET : embot::hw::gpio::State::RESET);        
    HAL_Delay(10); // wait for 10 ms to stabilize ...
}
//#endif

//#if defined(EMBOT_ENABLE_hw_spi_123_atstartup)    
#include "embot_hw_gpio.h"
#include "embot_hw_spi_bsp_amc.h"
// it selects spi1 / spi2 / spi3 in connector J5
void prepare_connector_j5_spi123()
{
    // ok, i know it does not compile... because:
    // todo: if we define EMBOT_ENABLE_hw_spi_123_atstartup then we must not call s_J5_SPIpinout() in runtime
    embot::hw::spi::bsp::s_J5_SPIpinout(embot::hw::SPI::one, true);
    embot::hw::spi::bsp::s_J5_SPIpinout(embot::hw::SPI::two, true);
    embot::hw::spi::bsp::s_J5_SPIpinout(embot::hw::SPI::three, true);
}
//#endif

#if   !defined(EMBOT_ENABLE_hw_bsp_specialize)
bool embot::hw::bsp::specialize() { return true; }
#else   

#if   defined(STM32HAL_BOARD_AMC)
    
       
    bool embot::hw::bsp::specialize()
    {
        embot::core::print("Called embot::hw::bsp::specialize");
        constexpr uint32_t hsem0 {0};
        constexpr uint32_t procID0 {0};
        
        switch(amc::defOnSpec.cm4mode)
        {
            case amc::OnSpecialize::CM4MODE::activateandhold:
            {
                // 1. init the hsems and take hsem-0
                __HAL_RCC_HSEM_CLK_ENABLE();
                HAL_HSEM_FastTake(hsem0);

                // 2. enable the second core
                HAL_RCCEx_EnableBootCore(RCC_BOOT_C2);                
            } break;
 
            case amc::OnSpecialize::CM4MODE::release:
            {
                // 1. init the hsems (just in case) and release hsem-0
                __HAL_RCC_HSEM_CLK_ENABLE();
                HAL_HSEM_Release(hsem0, procID0);          

#if defined(EMBOT_ENABLE_hw_icc_printer)
                // and also activate the printer
                embot::hw::icc::printer::theServer::getInstance().initialise({});
#endif                
            } break;
            
            
            case amc::OnSpecialize::CM4MODE::activate:
            {
                // 1. init the hsems and take hsem-0
                __HAL_RCC_HSEM_CLK_ENABLE();
                HAL_HSEM_FastTake(hsem0);

                // 2. enable the second core
                HAL_RCCEx_EnableBootCore(RCC_BOOT_C2);   

                // 3. and release hsem-0
                HAL_HSEM_Release(hsem0, procID0);
                
            } break;            
            
            case amc::OnSpecialize::CM4MODE::donothing:
            default: 
            {
                break;
            }
        }
        
        if(true == amc::defOnSpec.enableJ5spi_123_at_startup)
        {
            prepare_connector_j5_spi123();
        }
        
        if(true == amc::defOnSpec.enableJ5powersupply_at_startup)
        {
            J5power(true);
        }
       
        return true;
    }

   
    #else
        #error embot::hw::bsp::specialize() must be defined    
    #endif  
#endif  //EMBOT_ENABLE_hw_bsp_specialize


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

