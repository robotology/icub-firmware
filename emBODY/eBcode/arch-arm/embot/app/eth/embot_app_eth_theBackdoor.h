

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THEBACKDOOR_H_
#define __EMBOT_APP_ETH_THEBACKDOOR_H_

#include "embot_core.h"
#include "embot_core_binary.h"
#include "embot_os.h"

#include "embot_app_eth.h"
#include "EOpacket.h"

#include <vector>
#include <memory>


namespace embot { namespace app { namespace eth {
      
#if 0
    The singleton `theBackdoor` opens a listening port on the board whuhc can be used 
    for geneneric reception of commands and also transmission of replies or of log messages    
#endif
    
        
    
    class theBackdoor
    {
    public:
        static theBackdoor& getInstance();
                
    public:
        
        using fpOnPacket = void (*)(EOpacket *rxpacket); 
                        
        struct Config
        {   // use embot::os::Priority::belownorm23
            embot::os::Thread::Props thread {embot::os::Priority::belownorm23, 2*1024};
            embot::app::eth::SocketDescriptor socket {{2, 64, 2, 512}, 6666};
            embot::app::eth::SocketAddress hostaddress {embot::app::eth::IPlocalhost, 6666};
            fpOnPacket onreception {nullptr};
            constexpr Config() = default;
            constexpr Config(const embot::os::Thread::Props &t, 
                             const embot::app::eth::SocketDescriptor &s,
                             const embot::app::eth::SocketAddress &h,
                             fpOnPacket onrx) 
                             : thread(t), socket(s), hostaddress(h), onreception(onrx) {}
            constexpr bool isvalid() const { return thread.isvalid(); }
        }; 
                
        bool initialise(const Config &config);  
        
        bool set(fpOnPacket onrx);
        bool transmit(EOpacket *txp);
                     
    private:
        theBackdoor(); 
        ~theBackdoor(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;     
    }; 


}}} // namespace embot { namespace app { namespace eth


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
