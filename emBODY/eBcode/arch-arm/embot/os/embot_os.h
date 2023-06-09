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

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_OS_H_
#define _EMBOT_OS_H_

#include "embot_core.h"
#include "embot_hw.h"
#include "embot_os_common.h"
#include "embot_os_Thread.h"



namespace embot { namespace os {
    
    struct Config
    {         
        embot::core::Time tick {embot::core::time1millisec};                 
       
        embot::os::InitThread::Config initconfig {};
        embot::os::IdleThread::Config idleconfig {};
        embot::core::Callback onOSerror {};
        embot::hw::FLASHpartitionID codepartition { embot::hw::FLASHpartitionID::none };
            
        constexpr Config() = default;
            
        constexpr Config(embot::core::Time _t, 
                         const embot::os::InitThread::Config &ini, 
                         const embot::os::IdleThread::Config &idl, 
                         const embot::core::Callback &err,
                         embot::hw::FLASHpartitionID co = embot::hw::FLASHpartitionID::none) 
                            : tick(_t), initconfig(ini), idleconfig(idl), onOSerror(err), codepartition(co) {}            
        
        bool isvalid() const { if((0 == tick) || (false == initconfig.isvalid()) || (false == idleconfig.isvalid())) { return false; } else { return true; } }        
    }; 
    
    bool init(const Config &config);
    
    bool initialised();
                
    // it starts the scheduler
    [[noreturn]] void start();
    
    // tells if the scheduler has started
    bool started();
    
                   
}} // namespace embot { namespace os {

//#define ADDembotcoremutex
#if defined(ADDembotcoremutex)

namespace embot::core {
    struct Mutex  
    {
        virtual bool take(embot::core::relTime timeout = embot::core::reltimeWaitForever) = 0;
        virtual void release() = 0;        
        ~Mutex() {}
    }; 

    struct dummyMutex final : embot::core::Mutex
    {
        bool take(embot::core::relTime timeout = embot::core::reltimeWaitForever) override { return true; }
        void release() override {}
    }; 
}
namespace embot::os {
            
    struct Mutex final : embot::core::Mutex
    {
        bool take(embot::core::relTime timeout = embot::core::reltimeWaitForever) override;
        void release() override;
        
        Mutex();
        ~Mutex();
                       
        // not copiable nor moveable
        Mutex(const Mutex&) = delete;
        Mutex& operator=(const Mutex&) = delete;
        Mutex(Mutex&&) = delete;
        Mutex& operator=(Mutex&&) = delete;
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;    
    }; 
    
}
#endif

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

