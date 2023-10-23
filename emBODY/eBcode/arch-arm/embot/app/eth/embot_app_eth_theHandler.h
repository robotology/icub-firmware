

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


namespace embot::app::eth {
      
#if 0
 
    ### The object theHandler    
    
    The object `theHandler` handles the transition of the application across its states.
    It is the same as the legacy EOMtheEMSappl object but in the embot environment.
    
    There are three states: `IDLE`, `RUN` and `FATALERROR`.
    The application starts in the `IDLE` state. There is a dedicated object that manages this state with a dedicatde thread.
    When the first service is started (after proper verification and activation) `theHandler` moves it to `RUN`.
    If other services come along the state stay always `RUN`. And also it stays `RUN` until the last service is 
    stopped. When all services are stopped (and deactivated) theHandler moves back to state `IDLE`.
    
    The actual transitions ...  
    
    To manage the legacy data structure used by EOMtheEMSappl use utilities in namespace embot::app::eth::legacy::thehandler

#endif
    
    class theHandler
    {
    public:
        static theHandler& getInstance();
                
    public:
                  
        enum class State : uint8_t { IDLE = 0, RUN = 1, FATALERROR = 2, NONE = 255 };    
        enum class Command : uint8_t { go2IDLE = 0, go2RUN = 1, go2FATALERROR = 2, dummy = 255};
        
        // this method gives the final state associated to a command
        static constexpr State tostate(Command c) { return static_cast<State>(c); }
        
                        
        struct Config
        {
            uint32_t tbd {0};
            constexpr Config() = default;
            constexpr Config(uint32_t t) : tbd(t) {}
            constexpr bool isvalid() const { return (666 == tbd) ? false : true; }
        }; 
        
        bool initialise(const Config &config);  
        
        // this method gets the current state
        State state() const;
               
                       
               
        // this method processes a request.
        // the request is for now to move to a new state.
        // so, in what does it differ from moveto(State state) ??
        // this process(Command cmd) internally calls other objects that decide IF and WHEN to force
        // the transition and to call moveto()
        // for instance, if we are in State::RUN and we process(Command::go2IDLE), then the transition 
        // to State::IDLE will be managed by the object that runs the RUN mode.
        // in our case, the object EOMtheEMSrunner will make sure that the substate RUN::tx is reached 
        // before moving to State::IDLE
        // this method is called by 
        // - function eoprot_fun_UPDT_mn_appl_cmmnds_go2state() and by 
        // - object embot::app::eth::theServices 
        bool process(Command cmd);


        // this method immediately moves to the new state
        // it is used by EOMtheEMSconfigurator or EOMtheEMSrunner when they decide they need to move to a new state
        bool moveto(State state); 
   

        // this method immediately transmits a ropdescriptor from the CTRL socket to the host
        // it is used by embot::app::eth::theServices to send back a result
        bool transmit(const eOropdescriptor_t &ropdes); 


        // this method is un-used, so far.
        // it forces the transmission from the CTRL socket to the host
        bool forcetransmission();          

    private:        
                     
    private:
        theHandler(); 
        ~theHandler(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;     
    }; 


} // namespace embot::app::eth

namespace embot::app::eth::legacy::thehandler {
    
    // the embOBJ code uses two C enum to represent the states and the commands (called events).
    // they are eOsmStatesEMSappl_t and eOsmEventsEMSappl_t
    // now, to allow use of theHandler inside legacy embOBJ code and avoid confusion in here we give:
    // - an enum class eoApplEVENT that has static_cast<> conversion w/ eOsmEventsEMSappl_t,
    // - an enum class eoApplSTATE that has static_cast<> conversion w/ eOsmStatesEMSappl_t,
    // - suitable conversion methods to be ALWAYS used when treating eOsmEventsEMSappl_t and eOsmStatesEMSappl_t

    enum class eoApplSTATE : uint8_t { cfg = 0, run = 1, err = 2 };        
    constexpr embot::app::eth::theHandler::State tostate(const eoApplSTATE s) { return static_cast<embot::app::eth::theHandler::State>(s); }       
    
    enum class eoApplEVENT : uint8_t { go2run = 0, go2err = 1, go2cfg = 2, dummy = 255 };         
    constexpr embot::app::eth::theHandler::Command tocommand(const eoApplEVENT e) 
    {
        constexpr std::array<embot::app::eth::theHandler::Command, 3> lut2command { 
            embot::app::eth::theHandler::Command::go2RUN, 
            embot::app::eth::theHandler::Command::go2FATALERROR, 
            embot::app::eth::theHandler::Command::go2IDLE
        };
        if(eoApplEVENT::dummy == e) return embot::app::eth::theHandler::Command::dummy;
        else                        return lut2command[embot::core::tointegral(e)]; 
    } 

    constexpr eoApplEVENT toevent(embot::app::eth::theHandler::State s)
    {
        constexpr std::array<eoApplEVENT, 3> lut2event { 
            eoApplEVENT::go2cfg, 
            eoApplEVENT::go2run, 
            eoApplEVENT::go2err
        };
        if(embot::app::eth::theHandler::State::NONE == s)   return eoApplEVENT::dummy;
        else                                                return lut2event[embot::core::tointegral(s)];         
    }
    
}

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
