
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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

#include "embot_hw_bsp_amcfocm4_config.h"


// --------------------------------------------------------------------------------------------------------------------
// - support maps
// --------------------------------------------------------------------------------------------------------------------


// - support map: begin of embot::hw::gpio

#include "embot_hw_gpio_bsp.h"

#if   !defined(HAL_GPIO_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_gpio)

#error CAVEAT: embot::hw requires GPIO. pls enable it!

namespace embot::hw::gpio {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::GPIO h) const {}    
    const BSP& getBSP() { return thebsp; }    
}

#else
    
namespace embot::hw::gpio {
 
    static const BSP thebsp {        
        // supportmask2d
        {{
            0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff 
        }},            
        // ports
        {{
            GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI, GPIOJ, GPIOK
        }},
        {{
            [](){__HAL_RCC_GPIOA_CLK_ENABLE();}, [](){__HAL_RCC_GPIOB_CLK_ENABLE();}, [](){__HAL_RCC_GPIOC_CLK_ENABLE();}, 
            [](){__HAL_RCC_GPIOD_CLK_ENABLE();}, [](){__HAL_RCC_GPIOE_CLK_ENABLE();}, [](){__HAL_RCC_GPIOF_CLK_ENABLE();},
            [](){__HAL_RCC_GPIOG_CLK_ENABLE();}, [](){__HAL_RCC_GPIOH_CLK_ENABLE();}, [](){__HAL_RCC_GPIOI_CLK_ENABLE();},
            [](){__HAL_RCC_GPIOJ_CLK_ENABLE();}, [](){__HAL_RCC_GPIOK_CLK_ENABLE();}
        }}
    };      

    
    void BSP::init(embot::hw::GPIO h) const {}        
        
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
} // namespace embot::hw::gpio {

#endif // gpio

// - support map: end of embot::hw::gpio


// - support map: begin of embot::hw::led

#include "embot_hw_led_bsp.h"

#if !defined(EMBOT_ENABLE_hw_led)

namespace embot::hw::led {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::LED h) const {}    
    const BSP& getBSP() { return thebsp; }
    
}

#else

namespace embot::hw::led {         
    
    constexpr PROP led1p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::five}  };  
    constexpr PROP led2p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::four}  };  
    constexpr PROP led3p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::eight}  }; 
    constexpr PROP led4p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::nine}  };     
    constexpr PROP led5p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::ten}  }; 
    constexpr PROP led6p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::eleven}  }; 
    
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(LED::one) | mask::pos2mask<uint32_t>(LED::two) 
//        | mask::pos2mask<uint32_t>(LED::three) | mask::pos2mask<uint32_t>(LED::four) 
//        | mask::pos2mask<uint32_t>(LED::five) | mask::pos2mask<uint32_t>(LED::six)
        ,        
        // properties
        {{
            &led3p, &led4p         
        }}        
    };
    
    void clock_enable_H() { __HAL_RCC_GPIOH_CLK_ENABLE(); }
    void clock_enable_D() { __HAL_RCC_GPIOD_CLK_ENABLE(); }
    void BSP::init(embot::hw::LED h) const 
    {
        // activate the clock if cube-mx didn't do that
        //constexpr std::array<embot::core::fpWorker, 6> clockenable { clock_enable_H, clock_enable_H, clock_enable_H, clock_enable_H, clock_enable_H, clock_enable_H};        
        uint8_t i = embot::core::tointegral(h);
        //if(i < clockenable.size())
        //{
        //    clockenable[i]();
        //}
        clock_enable_D(); // no need of the above as every LED is on bus D
        
        // init the gpio
        const embot::hw::GPIO &g = thebsp.properties[i]->gpio;
        embot::hw::gpio::configure(g, embot::hw::gpio::Mode::OUTPUTpushpull, embot::hw::gpio::Pull::nopull, embot::hw::gpio::Speed::low);                        
    } 

    const BSP& getBSP() 
    {
        return thebsp;
    }
              
} // namespace embot::hw::led { 

#endif // led

// - support map: end of embot::hw::led



// - support map: begin of embot::hw::testpoint

#include "embot_hw_testpoint_bsp.h"

#if !defined(EMBOT_ENABLE_hw_testpoint)

namespace embot { namespace hw { namespace testpoint {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::TESTPOINT h) const {}
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else

namespace embot { namespace hw { namespace testpoint {
    
    #if   defined(STM32HAL_BOARD_AMC2C)

    
    constexpr PROP tp1 = { .on = embot::hw::gpio::State::SET, .off = embot::hw::gpio::State::RESET, .gpio = {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::five}  };
    constexpr PROP tp2 = { .on = embot::hw::gpio::State::SET, .off = embot::hw::gpio::State::RESET, .gpio = {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::eight}  };
    constexpr PROP tp3 = { .on = embot::hw::gpio::State::SET, .off = embot::hw::gpio::State::RESET, .gpio = {embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::zero}  };
    constexpr PROP tp4 = { .on = embot::hw::gpio::State::SET, .off = embot::hw::gpio::State::RESET, .gpio = {embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::thirteen}}; 
    
    constexpr BSP thebsp {
        // maskofsupported
        mask::pos2mask<uint32_t>(TESTPOINT::one) | mask::pos2mask<uint32_t>(TESTPOINT::two) | 
        mask::pos2mask<uint32_t>(TESTPOINT::three) | mask::pos2mask<uint32_t>(TESTPOINT::four) 
        ,
        // properties
        {{
            &tp1, &tp2, &tp3, &tp4
        }}
    };
    
    void clock_enable_A() { __HAL_RCC_GPIOA_CLK_ENABLE(); }
    void clock_enable_C() { __HAL_RCC_GPIOC_CLK_ENABLE(); }
    void BSP::init(embot::hw::TESTPOINT h) const 
    {
        // activate the clock if cube-mx didn't do that
        uint8_t i = embot::core::tointegral(h);

        clock_enable_A();
        clock_enable_C();
        
        // init the gpio
        const embot::hw::GPIO &g = thebsp.properties[i]->gpio;
        embot::hw::gpio::configure(g, embot::hw::gpio::Mode::OUTPUTpushpull, embot::hw::gpio::Pull::nopull, embot::hw::gpio::Speed::high);
    } 

    #else
        #error embot::hw::testpoint::thebsp must be defined
    #endif
    
    const BSP& getBSP()
    {
        return thebsp;
    }

}}} // namespace embot { namespace hw {  namespace testpoint {

#endif // testpoint

// - support map: end of embot::hw::testpoint



// - support map: begin of embot::hw::button

#include "embot_hw_button.h"
#include "embot_hw_button_bsp.h"
#include "embot_hw_gpio_bsp.h"

#if !defined(EMBOT_ENABLE_hw_button)

namespace embot::hw::button {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::BTN h) const {}
    void BSP::onEXTI(const embot::hw::gpio::PROP &p) const {}
    const BSP& getBSP() { return thebsp; }
    
}

#else

namespace embot::hw::button {
    
    constexpr PROP btn1p = { .pressed = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::E, embot::hw::GPIO::PIN::fifteen}, 
                             .pull = embot::hw::gpio::Pull::nopull, .irqn = EXTI15_10_IRQn  };  
 
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(BTN::one),        
        // properties
        {{
            &btn1p, nullptr, nullptr            
        }}        
    };
    
    
    void BSP::init(embot::hw::BTN h) const {}
    
    void BSP::onEXTI(const embot::hw::gpio::PROP &p) const
    {
        const embot::hw::GPIO gpio = embot::hw::gpio::getBSP().getGPIO(p);
        switch(gpio.pin)
        {
            case  embot::hw::GPIO::PIN::fifteen:
            {
                embot::hw::button::onexti(BTN::one);
            } break;
            

            default:
            {
            } break;           
        }              
    }
    
    // we put in here the IRQHandlers + the exti callback

    extern "C" {
            
        void EXTI15_10_IRQHandler(void)
        {
            HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
        }
        
        void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
        {
             embot::hw::button::getBSP().onEXTI({nullptr, GPIO_Pin, nullptr});            
        }        
    }
        
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
} // namespace embot::hw::button {

#endif // button

// - support map: end of embot::hw::button


// - support map: begin of embot::hw::flash

#include "embot_hw_flash_bsp.h"

#if !defined(EMBOT_ENABLE_hw_flash)

namespace embot { namespace hw { namespace flash { namespace bsp {
    
    constexpr BSP thebsp { };
    void BSP::init() const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}}

#else
    
namespace embot { namespace hw { namespace flash { namespace bsp {
     
    
    constexpr uint8_t numbanks {2};
    constexpr uint32_t banksize {1024*1024};
    constexpr uint32_t pagesize {128*1024};
    constexpr BankDescriptor bank01 { Bank::ID::one, 0x08000000, banksize, pagesize };
    constexpr BankDescriptor bank02 { Bank::ID::two, 0x08100000, banksize, pagesize };
    constexpr theBanks thebanks 
    {
        numbanks, 
        { &bank01, &bank02 }
    }; 
    
    // on Bank::one
    constexpr Partition ldr {Partition::ID::eloader,        &bank01,    bank01.address,         128*1024}; 
    constexpr Partition upd {Partition::ID::eupdater,       &bank01,    ldr.address+ldr.size,   256*1024};
    constexpr Partition a00 {Partition::ID::eapplication00, &bank01,    upd.address+upd.size,   512*1024};  
    constexpr Partition b00 {Partition::ID::buffer00,       &bank01,    a00.address+a00.size,   128*1024};
    
    // on Bank::two
    constexpr Partition a01 {Partition::ID::eapplication01, &bank02,    bank02.address,         512*1024};     
    constexpr Partition b01 {Partition::ID::buffer01,       &bank02,    a01.address+a01.size,   512*1024};
    
    constexpr thePartitions thepartitions
    {
        { &ldr, &upd, &a00, &b00, &a01, &b01 }
    };

    constexpr BSP thebsp {        
        thebanks,
        thepartitions
    };   
                  
    
    void BSP::init() const {}
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace flash { namespace bsp {

#endif // flash

// - support map: end of embot::hw::flash


// --------------------------------------------------------------------------------------------------------------------
// - board specific methods
// --------------------------------------------------------------------------------------------------------------------

extern "C" {
      
    void HAL_MspInit(void)
    {
        __HAL_RCC_SYSCFG_CLK_ENABLE();

        /** Enable the VREF clock
        */
        __HAL_RCC_VREF_CLK_ENABLE();
        /** Disable the Internal Voltage Reference buffer
        */
        HAL_SYSCFG_DisableVREFBUF();
        /** Configure the internal voltage reference buffer high impedance mode
        */
        HAL_SYSCFG_VREFBUF_HighImpedanceConfig(SYSCFG_VREFBUF_HIGH_IMPEDANCE_ENABLE);            
    }
}





// --------------------------------------------------------------------------------------------------------------------
// - specialize the bsp
// --------------------------------------------------------------------------------------------------------------------


#if     !defined(EMBOT_ENABLE_hw_bsp_specialize)
bool embot::hw::bsp::specialize() { return true; }
#else   


void waitHWmutex(uint32_t mtx)  __attribute__ ((section (".ram_func")));
void waitHWmutex(uint32_t mtx)
{
    volatile uint32_t m = mtx;
    for(;;)
    {
        if(HAL_OK == HAL_HSEM_FastTake(m))
        {
            break;
        }
    }        
}



bool embot::hw::bsp::specialize()
{
    // very important: it waits for the unlock called by the application of amc core cm7
    waitHWmutex(0);
    

    return true;
}


#endif  //EMBOT_ENABLE_hw_bsp_specialize


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

