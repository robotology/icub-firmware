

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THEHANDLER_H_
#define __EMBOT_APP_ETH_THEHANDLER_H_

#include "embot_core.h"

#include "eOrop.h"

#include <vector>
#include <memory>


namespace embot { namespace app { namespace eth {
      
    // former eom_emsappl_GetHandle
    class theHandler
    {
    public:
        static theHandler& getInstance();
                
    public:
        
    enum class State : uint8_t { IDLE = 0, RUN = 1, FATALERROR = 2 };    
    enum class Command : uint8_t { go2IDLE = 0, go2RUN = 1, go2FATALERROR = 2, dummy = 255};
                        
        struct Config
        {
            uint32_t tbd {0};
            constexpr Config() = default;
            constexpr Config(uint32_t t) : tbd(t) {}
            constexpr bool isvalid() const { return (666 == tbd) ? false : true; }
        }; 
        
        bool initialise(const Config &config);  
        
               
        
        // process(Command cmd) should be called by external objects
        // moveto(State s) should be called EOMtheEMSconfigurator and EOMtheEMSrunner
        // which are owned by theHandler ... so, maybe we remove it from interface ??? 
        // after a process() the effect is that eventually the threads owned by RUNNER etc will call a moveto()
        // after a moveto() the state is adjusted straight away
        // 
        
        bool process(Command cmd);
        bool moveto(State state); // maybe remove it from here ...
        State state() const;
               
        bool transmit();        
        bool transmit(const eOropdescriptor_t &ropdes); 

    private:        
                     
    private:
        theHandler(); 
        ~theHandler(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;     
    }; 


}}} // namespace embot { namespace app { namespace eth


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
