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

#include "embot_hw_led.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"
#include "embot_hw_led_bsp.h"

#include <cstring>
#include <vector>
#include "embot_core_binary.h"
#include "embot_hw_gpio.h"


using namespace std;


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

using namespace embot::hw;

#if !defined(EMBOT_ENABLE_hw_led)

namespace embot { namespace hw { namespace led {
    
    bool supported(LED led)     { return false; }
    bool initialised(LED led)   { return false; }            
    result_t init(LED led)      { return resNOK; }
    result_t on(LED led)        { return resNOK; }
    result_t off(LED led)       { return resNOK; }    
    result_t toggle(LED led)    { return resNOK; }
    
}}}

#else

namespace embot { namespace hw { namespace led {

    // initialised mask
    static std::uint32_t initialisedmask = 0;
               
    
    bool supported(LED led)
    {
        return embot::hw::led::getBSP().supported(led);
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
        
        // we do specific init of the peripheral
        embot::hw::led::getBSP().init(led);
       
        
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
        const embot::hw::GPIO *gpio = &embot::hw::led::getBSP().getPROP(led)->gpio;      
        embot::hw::gpio::set(*gpio, embot::hw::led::getBSP().getPROP(led)->on);  
        return resOK;        
    }
    
    result_t off(LED led)
    {
        if(!initialised(led))
        {
            return resNOK;
        }  
        const embot::hw::GPIO *gpio = &embot::hw::led::getBSP().getPROP(led)->gpio;       
        embot::hw::gpio::set(*gpio, embot::hw::led::getBSP().getPROP(led)->off);  
        return resOK;          
    }
    
    result_t toggle(LED led)
    {
        if(!initialised(led))
        {
            return resNOK;
        }  
        const embot::hw::GPIO *gpio = &embot::hw::led::getBSP().getPROP(led)->gpio;       
        embot::hw::gpio::toggle(*gpio);  
        return resOK;          
    }
    
    
}}} // namespace embot { namespace hw { namespace led 


#endif //#if defined(EMBOT_ENABLE_hw_led)



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

