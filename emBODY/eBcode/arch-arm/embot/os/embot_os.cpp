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

#include "embot_os.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_os_rtos.h"

#include "stdlib.h"
#include "embot_hw.h"
#include "embot_os_theScheduler.h"

#include <new>


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace os {

   
    static core::Time _now()
    {
        return embot::os::rtos::scheduler_timeget();
    }
  
    static bool _initted = false;
    static Config _config {};
    
    bool initialised() { return _initted; }
    
    bool init(const Config &config)
    {
        if(true == initialised())
        {
            return true;
        }
        
        if(false == config.isvalid())
        {
            return false;
        }
        
        _config = config;
        
        embot::core::init({{nullptr, _now}});        
        embot::hw::init({nullptr, _now});
        _initted = true;
        return true;        
    }
    
    bool started()
    { 
        return embot::os::rtos::scheduler_isrunning(); 
    }
    
    
    [[noreturn]] void start()
    {
        if((false == _initted) || (false == _config.isvalid()))
        {
            for(;;);
        }
        
        embot::os::theScheduler::Config cfg { embot::os::theScheduler::Timing(_config.tick), {_config.initconfig, _config.idleconfig, _config.onOSerror} };    
        embot::os::theScheduler &thescheduler = embot::os::theScheduler::getInstance();
        thescheduler.start(cfg);    
        
        for(;;); 
    }
      

}} // namespace embot { namespace os {




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

