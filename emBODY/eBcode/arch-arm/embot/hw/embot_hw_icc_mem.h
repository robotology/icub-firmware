
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_ICC_MEM_H_
#define __EMBOT_HW_ICC_MEM_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_icc.h"
    
namespace embot::hw::icc::mem {
    
#if 0
    The MEM offers very basic data exchange across cores.... BUT not across two threads of the same core.
    For that you need a proper mutex embot::os::Mutex 
    Data is written and read in atomic way, but no notification is offered, so the received must continuously poll.           
#endif   
    
    struct Config
    {
        embot::core::Mutex *mu {nullptr}; 
        constexpr Config() = default;
        Config(embot::core::Mutex *m) : mu(m) {}
    };
    
//    static embot::core::dummyMutex dm {};
//    struct BetterConfig
//    {
////        static constexpr embot::core::dummyMutex dm {};
//        embot::core::Mutex *mu { &dm };
//        constexpr BetterConfig() = default;
//        BetterConfig(embot::core::Mutex *m) : mu((nullptr != m) ? m : &dm) {}
//    };
    
    bool supported(embot::hw::icc::MEM m); 
    size_t size(embot::hw::icc::MEM m);
    result_t init(embot::hw::icc::MEM m);
    bool initialised(embot::hw::icc::MEM m);
    
    // atomic set() / get()
    size_t set(embot::hw::icc::MEM m, const embot::core::Data &data, embot::core::relTime timeout = embot::core::reltimeWaitForever);    
    size_t get(embot::hw::icc::MEM m, embot::core::Data &data, embot::core::relTime timeout = embot::core::reltimeWaitForever); 
    
    // take(), modify memory(), release()
    // the usage is for instance ... manipulate a FIFO mapped inded the shared memory in mutual access mode  
    bool take(embot::hw::icc::MEM m, embot::core::relTime timeout = embot::core::reltimeWaitForever);
    void * memory(embot::hw::icc::MEM m);
    void release(embot::hw::icc::MEM m);
    
} // namespace embot::hw::icc::mem {

namespace embot::hw::icc::mem::test {
    
    void init(embot::hw::icc::DIR dir, const embot::hw::icc::mem::Config &cfg = {});
    void tick();
}
    

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


