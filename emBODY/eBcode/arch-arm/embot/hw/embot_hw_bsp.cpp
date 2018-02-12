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

#include "embot_hw_bsp.h"
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
  

namespace embot { namespace hw { namespace bsp {
    
    static bool initted = false; 
    
    static embot::common::fpGetU64 s_timenow = nullptr;
    
    static embot::common::Time s_timenowstm32hal()
    {
        return static_cast<embot::common::Time>(embot::common::time1millisec)*HAL_GetTick();
    }

    bool initialised()
    {
        return initted;
    }

    result_t init(const Config &config)
    {
        if(true == embot::hw::bsp::initialised())
        {
            return resOK;
        }
        
        // put whatwever is required for ...        
        stm32hal_config_t cfg = {0};
        cfg.tick1ms_init = config.stm32hal.init1millitick;
        cfg.tick1ms_get = config.stm32hal.get1millitick;
        
        stm32hal_init(&cfg);
        
        s_timenow = config.get1microtime;
        
        if(nullptr == s_timenow)
        {
            s_timenow = s_timenowstm32hal;
        }
        
        initted = true;
        return resOK;
    }
    
    
    embot::common::Time now()
    {
        return s_timenow();
    }
    

}}} // namespace embot { namespace hw { namespace bsp {




    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

