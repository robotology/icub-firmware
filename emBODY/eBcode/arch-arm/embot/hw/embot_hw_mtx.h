
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#if 0

## The `embot::hw::mtx` driver

This driver is a wrapper of the HSEM available in the STM32 dual core MPUs.

The MTX offers mutual exclusion between cores, but not between threads running on the same core.
For this latter you must use embot::os::mutex_t. It also offers a callback on the release of the MTX.  
    
The usage of a  given`embot::hw::MTX m {embot::hw::MTX::one}` is simple:
- it must be initialised with `embot::hw::mtx::init(m)`;
- after that it can be taken and released with the proper functions; 
- when taken by a core it cannot be taken by another core, so we provide function `bool embot::hw::mtx::check(m)`
  to verify if a core can take the mutex and we also allow to take it w/ a timeout;
- we also allow to subscribe a given mutex so that a callback is called when the mutex is released;
  we use `bool subscribe(embot::hw::MTX m, const embot::hw::Subscription &onrelease)`.
  Note that the callback is executed also when the mutex is released by the same core. 
  That is sadly a HW behavior.

#endif 

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MTX_H_
#define __EMBOT_HW_MTX_H_

#include "embot_core.h"
#include "embot_hw_types.h"


namespace embot::hw::mtx {
            
    bool supported(embot::hw::MTX m);   
    result_t init(embot::hw::MTX m);  
    bool initialised(embot::hw::MTX m);
    
    bool take(embot::hw::MTX m, embot::core::relTime timeout = embot::core::reltimeWaitForever);    
    void release(embot::hw::MTX m);
    bool check(embot::hw::MTX m);
    
    bool subscribe(embot::hw::MTX m, const embot::hw::Subscription &onrelease);     
        
}   // namespace embot::hw::mtx {

    
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


