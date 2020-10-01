
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



// ------------------------------------------------------------------------------------------------------------------
// - minimal 
// - embot::hw
// -------------------------------------------------------------------------------------------------------------------

#include "embot_hw.h"

void embot::hw::bsp::init() {}
    

// ------------------------------------------------------------------------------------------------------------------
// - minimal 
// - embot::hw::sys
// -------------------------------------------------------------------------------------------------------------------
    
#include "embot_hw_sys.h"    
    
#include "hal_trace.h"

namespace embot { namespace hw { namespace sys {
       
    int puts(const std::string &str) 
    {
        return puts(str.c_str());
    }
    
    int puts(const char* str) 
    {    
        return hal_trace_puts(str);  
    }
   

}}} // namespace embot { namespace hw { namespace sys { 


// ------------------------------------------------------------------------------------------------------------------
// - minimal 
// - embot::hw::led
// -------------------------------------------------------------------------------------------------------------------

#include "embot_hw_led.h"

#include "hal_led.h"

namespace embot { namespace hw { namespace led {
    
    hal_led_t tohal(LED led)
    {
        return static_cast<hal_led_t>(embot::core::tointegral(led));
    }
    
    // initialised mask
    static std::uint32_t initialisedmask = 0;
                
    
    bool supported(LED led)
    {
        return (hal_true == hal_led_supported_is(tohal(led)));
    }
    
    bool initialised(LED led)
    {
        return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(led));
    }
            
    result_t init(LED led)
    {
        if(!supported(led))
        {
            return resNOK;
        }  
        
        if(initialised(led))
        {   // dont need to re-init
            return resOK;
        }
        
        
        if(!embot::hw::initialised())
        {   // requires embot::hw::bsp::init()
            return resNOK;
        }
        
        if(hal_res_OK != hal_led_init(tohal(led), nullptr))
        {
            return resNOK;
        }
       
        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(led));
        
        // we just switch it off        
        embot::hw::led::off(led);
        
        return resOK;        

    }

          
    result_t on(LED led)
    {
        if(!initialised(led))
        {
            return resNOK;
        }  
        return (hal_res_OK == hal_led_on(tohal(led))) ? resOK : resNOK;        
    }
    
    result_t off(LED led)
    {
        if(!initialised(led))
        {
            return resNOK;
        }  
        return (hal_res_OK == hal_led_off(tohal(led))) ? resOK : resNOK;          
    }
    
    result_t toggle(LED led)
    {
        if(!initialised(led))
        {
            return resNOK;
        }  
        return (hal_res_OK == hal_led_toggle(tohal(led))) ? resOK : resNOK;          
    }

}}}

    
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

