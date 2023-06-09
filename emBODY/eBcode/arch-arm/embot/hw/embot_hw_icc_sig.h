
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_ICC_SIG_H_
#define __EMBOT_HW_ICC_SIG_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_icc.h"

namespace embot::hw::icc::sig {
    
    
    struct Config
    {        
        DIR direction {DIR::none};
        
        constexpr Config() = default;
        constexpr Config(DIR d) : direction(d) {}
    };
    
    bool supported(embot::hw::icc::SIG s);   
    result_t init(embot::hw::icc::SIG s, const Config &cfg);
    bool initialised(embot::hw::icc::SIG s);
    
    // -- transmitter methods. they can be used only for DIR::tx or Dir:txrx
    
    // one entity sends the signal set()
    bool set(embot::hw::icc::SIG s, embot::core::relTime timeout = embot::core::reltimeWaitForever);    
    
    // -- receiver methods.  they can be used only for DIR::rx or DIR::txrx
    
    // the entity which waits must subscribe 
    bool subscribe(embot::hw::icc::SIG s, const embot::hw::Subscription &onsignal);   
    // or it can check() in polling mode.   
    bool check(embot::hw::icc::SIG s, bool andclear = true);
    // we can also explicitly clear() the signal
    bool clear(embot::hw::icc::SIG s);    

} // namespace embot::hw::icc::sig {   
    
    
namespace embot::hw::icc::sig::test {

    void init(DIR dir);
    void tick();
}    

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


