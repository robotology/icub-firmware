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
            
        // we put hw::init() before because in its inside it also call embot::core::init() 
        // with a pointer to embot::hw::sys::puts() so that embot::core::print() can work       
        embot::hw::init({nullptr, _now, _config.codepartition});
        //embot::core::init({{nullptr, _now}}); // we dont need it
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

#include "embot_os_rtos.h"

namespace embot::os {

    struct Mutex::Impl
    {
        embot::os::rtos::mutex_t *m {nullptr};
        
        Impl() : m(embot::os::rtos::mutex_new()) {}
        ~Impl() { embot::os::rtos::mutex_delete(m); }
        
        bool take(embot::core::relTime timeout = embot::core::reltimeWaitForever)
        {           
            return embot::os::rtos::mutex_take(m, timeout);
        };
        
        void release() 
        {
            embot::os::rtos::mutex_release(m);
        }   
    }; 

    Mutex::Mutex() : pImpl(std::make_unique<Impl>()) {}    
    Mutex::~Mutex() = default;
    bool Mutex::take(embot::core::relTime timeout) { return pImpl->take(timeout); }
    void Mutex::release() { pImpl->release(); }
    
}




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

