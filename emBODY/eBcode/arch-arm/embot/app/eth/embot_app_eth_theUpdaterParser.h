
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THEUPDATERPARSER_H_
#define __EMBOT_APP_ETH_THEUPDATERPARSER_H_

#include <memory>

#include "EOpacket.h"
#include "embot_core.h"
#include "embot_os_Thread.h"
#include "embot_app_eth.h"

namespace embot { namespace app { namespace eth {
      
#if 0
    The singleton `theUpdaterParser` is responsible to decode the frames coming from `FirmwareUpdater`    
#endif
           
    
    class theUpdaterParser
    { 
    // for the updater protocol
    public:
        static theUpdaterParser& getInstance();
                
    public:
        
        struct Config
        {   // if eok_ipv4addr_localhost ... we are allowed to talk w/ any address
            embot::app::eth::SocketAddress host {embot::app::eth::IPlocalhost, 3333};
            uint16_t txpktcapacity {64};
            embot::os::Thread *owner {nullptr};            
            constexpr Config() = default;
            constexpr Config(const embot::app::eth::SocketAddress &h, uint16_t c, embot::os::Thread *o) 
                : host(h), txpktcapacity(c), owner(o) {}
            constexpr bool isvalid() const { return true; }
        }; 
                
        bool initialise(const Config &config); 
        
        bool parse(EOpacket* rxpkt); 
        bool getreply(EOpacket** txpkt);        
                     
    private:
        theUpdaterParser(); 
        ~theUpdaterParser(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;       
    };


}}} // namespace embot { namespace app { namespace eth


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
