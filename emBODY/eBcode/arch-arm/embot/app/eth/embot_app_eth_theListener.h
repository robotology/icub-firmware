

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THELISTENER_H_
#define __EMBOT_APP_ETH_THELISTENER_H_

#include "embot_core.h"
#include "embot_core_binary.h"
#include "embot_app_eth.h"
#include "embot_os.h"

#include <vector>
#include <memory>


namespace embot { namespace app { namespace eth {
      
#if 0
    The singleton `theListener` is responsible to start a thread which opens a server
    socket which listens for requests from the `FirmwareUpdater` 
    
#endif
    
        
    
    class theListener
    {
    public:
        static theListener& getInstance();
                
    public:
                        
        struct Config
        {   // use embot::os::Priority::belownorm22
            embot::os::Thread::Props thread {embot::os::Priority::belownorm22, 4*1024};
            embot::app::eth::SocketDescriptor socket {{2, 32, 1, 128}, 3333};
            embot::app::eth::SocketAddress hostaddress {embot::app::eth::IPlocalhost, 3333};
            constexpr Config() = default;
            constexpr Config(const embot::os::Thread::Props &t, 
                             const embot::app::eth::SocketDescriptor &s,
                             const embot::app::eth::SocketAddress &h) 
                             : thread(t), socket(s), hostaddress(h) {}
            constexpr bool isvalid() const { return thread.isvalid(); }
        }; 
                
        bool initialise(const Config &config);  
                     
    private:
        theListener(); 
        ~theListener(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;     
    }; 


}}} // namespace embot { namespace app { namespace eth


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
