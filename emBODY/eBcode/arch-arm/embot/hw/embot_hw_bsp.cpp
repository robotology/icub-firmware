
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

    
namespace embot::hw::bsp {
    
    bool _initted {false};
    
    bool initialised()
    {
        return _initted;
    }
    
    void initialised(bool v)
    {
        _initted = v;
    }
    
    static uint32_t _get1millitick()
    {
        return embot::core::now() / 1000;        
    }
    
    bool init(const embot::hw::Config &config)
    {                  
        embot::hw::bsp::DRIVER::init(config);
        
        embot::hw::bsp::initialised(true);
        
        // perform further initialization defined board by board        
        specialize();
        
        return true;
    }
    
    int puts(const char* str) 
    {     
        return embot::hw::bsp::DRIVER::puts(str);
    }
    
    int print(const std::string &str)
    {
        return puts(str.c_str());
    } 

}


// in here definition of code that depends on the DRIVER

namespace embot::hw::bsp::DRIVER {


#if defined(USE_HAL_DRIVER) 

#if defined(EMBOT_REDEFINE_hw_bsp_DRIVER_init)
    // pls provide a different implementation
#else
    bool init(const embot::hw::Config &config)
    {          
        // init the chosen hal
        stm32hal_config_t cfg = {0};
        cfg.tick1ms_init = config.initmicrotime;
        cfg.tick1ms_get = embot::hw::bsp::_get1millitick;       
        stm32hal_init(&cfg);
        
        return true;
    }
#endif

#if defined(EMBOT_REDEFINE_hw_bsp_DRIVER_puts)
#else
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
#endif

#elif defined(USE_hal2_DRIVER)

#if defined(EMBOT_REDEFINE_hw_bsp_DRIVER_init)
#else
    bool init(const embot::hw::Config &config)
    {          
        // init the chosen hal
        stm32hal_config_t cfg = {0};
        cfg.tick1ms_init = config.initmicrotime;
        cfg.tick1ms_get = embot::hw::bsp::_get1millitick;       
        stm32hal_init(&cfg);

        return true;
    }
#endif

#if defined(EMBOT_REDEFINE_hw_bsp_DRIVER_puts)
#else
int puts(const char* str)
{
    return hal_trace_puts(str);
}
#endif

#else
    #error embot::hw::bsp::DRIVER supports either HAL or hal2 DRIVER    
#endif   

} // namespace embot::hw::bsp::DRIVER {

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

