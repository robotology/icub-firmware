
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
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

#include "embot_hw_types.h"
#include "embot_core_binary.h"
#include "embot_core.h"

#include "embot_hw_bsp_config.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif


using namespace std;
using namespace embot::core::binary;

// --------------------------------------------------------------------------------------------------------------------
// - definition of public functions 
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace hw { namespace bsp {
    
    static uint32_t _get1millitick()
    {
        return embot::core::now() / 1000;        
    }
    
    bool init(const embot::hw::Config &config)
    {          
        // init the chosen hal
        stm32hal_config_t cfg = {0};
        cfg.tick1ms_init = config.initmicrotime;
        cfg.tick1ms_get = _get1millitick;       
        stm32hal_init(&cfg);
        
        // perform further initialization defined board by board        
        specialize();

        return true;
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
    
    int print(const std::string &str)
    {
        return puts(str.c_str());
    }    

}}}



// --------------------------------------------------------------------------------------------------------------------
// - utilities for support maps 
// --------------------------------------------------------------------------------------------------------------------

// so far only gpio is mandatory for every board
#include "embot_hw_gpio_bsp.h"

#if   !defined(HAL_GPIO_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_gpio)
    #error CAVEAT: embot::hw requires GPIO. pls enable it!
#else
    
namespace embot { namespace hw { namespace gpio {
    
//    extern const embot::hw::gpio::BSP thebsp;
     
    // transforms the language of STM32 HAL into what embot uses
    // for instance: {GPIOA, GPIO_PIN_0} into {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::zero}
//    constexpr embot::hw::GPIO BSP::getGPIO(const embot::hw::gpio::PROP &p) const
//    {
//        embot::hw::GPIO gpio {embot::hw::GPIO::PORT::none, embot::hw::GPIO::PIN::none};
//        switch(p.stmpin)
//        {
//            case GPIO_PIN_0:  { gpio.pin = embot::hw::GPIO::PIN::zero; } break;
//            case GPIO_PIN_1:  { gpio.pin = embot::hw::GPIO::PIN::one; } break;
//            case GPIO_PIN_2:  { gpio.pin = embot::hw::GPIO::PIN::two; } break;
//            case GPIO_PIN_3:  { gpio.pin = embot::hw::GPIO::PIN::three; } break;
//            case GPIO_PIN_4:  { gpio.pin = embot::hw::GPIO::PIN::four; } break;
//            case GPIO_PIN_5:  { gpio.pin = embot::hw::GPIO::PIN::five; } break;
//            case GPIO_PIN_6:  { gpio.pin = embot::hw::GPIO::PIN::six; } break;
//            case GPIO_PIN_7:  { gpio.pin = embot::hw::GPIO::PIN::seven; } break;
//            case GPIO_PIN_8:  { gpio.pin = embot::hw::GPIO::PIN::eight; } break;
//            case GPIO_PIN_9:  { gpio.pin = embot::hw::GPIO::PIN::nine; } break;
//            case GPIO_PIN_10: { gpio.pin = embot::hw::GPIO::PIN::ten; } break;
//            case GPIO_PIN_11: { gpio.pin = embot::hw::GPIO::PIN::eleven; } break;
//            case GPIO_PIN_12: { gpio.pin = embot::hw::GPIO::PIN::twelve; } break;
//            case GPIO_PIN_13: { gpio.pin = embot::hw::GPIO::PIN::thirteen; } break;
//            case GPIO_PIN_14: { gpio.pin = embot::hw::GPIO::PIN::fourteen; } break;
//            case GPIO_PIN_15: { gpio.pin = embot::hw::GPIO::PIN::fifteen; } break;
//            case GPIO_PIN_All: { gpio.pin = embot::hw::GPIO::PIN::none; } break;
//            default: { gpio.pin = embot::hw::GPIO::PIN::none; } break;
//        }
//        
//        if(nullptr != p.stmport)
//        {
//            for(int i=0; i<thebsp.ports.size(); i++)
//            {
//                if(thebsp.ports[i] == p.stmport) 
//                {
//                    gpio.port = static_cast<embot::hw::GPIO::PORT>(i);
//                    break;
//                }
//            }
//        }
//        
//        return gpio;
//    }    
              
}}} // namespace embot { namespace hw {  namespace gpio {

#endif // gpio


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

