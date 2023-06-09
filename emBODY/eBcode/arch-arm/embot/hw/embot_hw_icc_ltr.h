
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_ICC_LTR_H_
#define __EMBOT_HW_ICC_LTR_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_icc.h"

namespace embot::hw::icc::ltr {
    
#if 0
    The LTR offers advanced data exchange across cores w/ a post() and a read(). The data exchange is always acked by the reader.

    To offer effective data exchange, you must init() the same LTR w/ DIR::tx in one core and w/ DIR::rx on the other.
    The following funtions can be called in both DIR modes: supported(), size(), init(), initialised()
    The others can be called only with a specific DIR:
    DIR::tx -> post(), acked()
    DIR::rx -> subscribe(), available(), read()
    
        
    The sender will send away data using the function post() and can verify if the data was read by the 
    receiver with function acked(). It is important to call the next post() only when acked() returns true.
        
    The posting of data can happen in the following possible modes:
    - Non blocking mode.
      - With a callback executed when the received reads the data and sends the ack. At execution of the callback 
        acked() is guaranteed to return true.
      - With a timeout specified equal 0. In this case before a new post() one must call acked() until it returns true.
    - Blocking mode.
      - with infinite timeout. At exit, acked() is guaranteed to return true.
      - with non infinite timeout. At exit, acked() must be called to verify if teh received has read the data.
          
      
      The receiver will be able to retrieve data and send the ack which unlocks the sender in two modes:
      - In polling. It must call available() until it returns true. At this point it can call read().
      - By subscription. As soon as data is available a callback is called, where we can call read().           

#endif 
      
    
    struct Config
    {        
        DIR direction {DIR::none};
        
        constexpr Config() = default;
        constexpr Config(DIR d) : direction(d) {}
            
        constexpr bool isvalid() const 
        {
            return (DIR::rx == direction) || (DIR::tx == direction);
        }
    };
    
    bool supported(embot::hw::icc::LTR l); 
    size_t size(embot::hw::icc::LTR l);
    result_t init(embot::hw::icc::LTR l, const Config &cfg);
    bool initialised(embot::hw::icc::LTR l);
    
    // tx methods
    size_t post(embot::hw::icc::LTR l, const embot::core::Data &data, embot::core::relTime acktimeout);
    size_t post(embot::hw::icc::LTR l, const embot::core::Data &data, const embot::core::Callback &onack); 
    bool acked(embot::hw::icc::LTR l);
    
    // rx methods
    bool subscribe(embot::hw::icc::LTR l, const embot::hw::Subscription &onreceived);
    bool available(embot::hw::icc::LTR l);   
    size_t read(embot::hw::icc::LTR l, embot::core::Data &data); 

    
    // applications:
    // 1. printer: the client sends a string to the server
    // 2. a thread on CM7 posts a message to a thread on CM4. this one gets immediately and      
    #warning .... place embot::hw::icc::thePrinterClient and thePrinterServer in dedicated object files
    
        
}   // namespace embot::hw::icc::ltr {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


