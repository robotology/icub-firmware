
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_bldc_theCTRL.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_core.h"

#include <array>

#include "embot_os_rtos.h"
#include "embot_os_Thread.h"
#include "embot_os_Action.h"
#include "embot_os_Timer.h"

// --------------------------------------------------------------------------------------------------------------------
// - defines
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


namespace embot::app::application {



} // end of namespace


void thrCTRL(void* p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }

struct embot::app::bldc::theCTRL::Impl
{
    static constexpr std::uint8_t tCTRLmaxINPcanframes = 4;
    static constexpr std::uint8_t tCTRLmaxOUTcanframes = 4;      
    
    static constexpr embot::os::Event evt_CTRL_tick { embot::core::binary::mask::pos2mask<embot::os::Event>(0) };
//    static constexpr embot::os::Event evt_CTRL_TXcanframes { embot::core::binary::mask::pos2mask<embot::os::Event>(1) };
         
    
    static constexpr embot::core::relTime _tCTRL_timeout {50*embot::core::time1millisec};
    
    embot::app::application::dummyCANagentCORE _dummycanagentcore {};
    embot::app::bldc::dummyCOMM _dummycomm {};
        
    std::vector<embot::app::bldc::MSG> _tCTRLoutframes {};
    std::vector<embot::app::bldc::MSG> _tCTRLinpframes {};
        
//    embot::hw::CAN _tCTRLcanbus {embot::hw::CAN::none};
//    embot::prot::can::Address _tCTRLcanaddress {1};
        
    embot::os::EventThread *_t_CTRL {nullptr};
    embot::os::Timer * _tCTRL_tickTimer {nullptr};
      
    Config _config {};
    bool _initted {false};
    
    
    Impl() = default;  
    
    // for the thread tCTRL and the embot::hw::can driver
    static void tCTRL_Startup(embot::os::Thread *t, void *param);
    static void tCTRL_OnEvent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param);      
    static void alertonrxframe(void *arg);      
    
    // the initialization code
    bool initialise(Config config);       
    // on timeout                
    void tCTRL_OnTimeout(embot::os::Thread *t, void *param);
    
    // when we have a frame in embot::hw::can to retrieve
    void tCTRL_OnRXframe(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, const embot::app::bldc::MSG &frame, std::vector<embot::app::bldc::MSG> &outframes);
    

    bool subscribe(embot::app::bldc::COMM *comm);
};

      
bool embot::app::bldc::theCTRL::Impl::initialise(const Config config)
{
    if(true == _initted)
    {
        return true;
    }
    
    embot::core::print("embot::app::bldc::theCTRL::Impl::initialise()");
    
    if(false == config.isvalid())
    {
        return false;
    }
    
    _config = config;
    
    if(nullptr == _config.comm)
    {
        _config.comm = &_dummycomm;
    }
    
    if(nullptr == _config.canagentcore)
    {
        _config.canagentcore = &_dummycanagentcore;
    }
    
//    // retrieve some CAN constants
//    _tCTRLcanbus = _config.canagentcore->bus();
//    _tCTRLcanaddress = _config.canagentcore->address();      
    

    // the tCOMM    
    embot::os::EventThread::Config tCTRL_cfg 
    {
        _config.tCTRL_stacksize, _config.priority,
        tCTRL_Startup, this,
        _tCTRL_timeout,
        tCTRL_OnEvent,
        "tCTRL"
    };   
    _t_CTRL = new embot::os::EventThread;
                 

    // and now ... start the tCTRL   
    _t_CTRL->start(tCTRL_cfg, thrCTRL);
    
    _initted = true;
    return _initted;
}

    

void embot::app::bldc::theCTRL::Impl::tCTRL_Startup(embot::os::Thread *t, void *param)
{
    Impl *impl = reinterpret_cast<Impl*>(param);
    
    impl->_tCTRLoutframes.reserve(tCTRLmaxOUTcanframes); 
    impl->_tCTRLinpframes.reserve(tCTRLmaxINPcanframes);    
    
    // start a timer which ticks the thread tCTRL
    impl->_tCTRL_tickTimer = new embot::os::Timer;
    embot::os::Action act(embot::os::EventToThread(evt_CTRL_tick, t));
    embot::os::Timer::Config cfg{impl->_config.tCTRL_period, act, embot::os::Timer::Mode::forever, 0};
    impl->_tCTRL_tickTimer->start(cfg);   

    if(nullptr != impl->_config.onstartup)
    {
        impl->_config.onstartup(impl->_config.canagentcore->address());
    }        
 }

void embot::app::bldc::theCTRL::Impl::tCTRL_OnTimeout(embot::os::Thread *t, void *param)
{
    
}    

void embot::app::bldc::theCTRL::Impl::tCTRL_OnEvent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{
    Impl *impl = reinterpret_cast<Impl*>(param);
    
    // manage the events
    if(0 == eventmask)
    {
        impl->tCTRL_OnTimeout(t, param);
        return;
    }
    
    if(true == embot::core::binary::mask::check(eventmask, evt_CTRL_tick))
    {
        // for a start get up to 4 canframes in here.
        impl->_tCTRLinpframes.clear();
        impl->_tCTRLoutframes.clear();
        
        size_t numRXframes {1}; 
        //embot::app::application::theMSGbroker::getInstance().size(embot::app::application::theMSGbroker::Direction::INP);

        if(numRXframes > 0)
        {
            size_t retrieved {0};
            size_t remaining {0};
            impl->_config.comm->get(remaining, impl->_tCTRLinpframes, retrieved, tCTRLmaxINPcanframes);
        }
        
        impl->_config.ontick(impl->_tCTRLinpframes, impl->_tCTRLoutframes);
        
        size_t numTXframes = impl->_tCTRLoutframes.size();
        
        if(numTXframes > 0)
        {
            impl->_config.comm->add(impl->_tCTRLoutframes);
        }       
    }     
}


bool embot::app::bldc::theCTRL::Impl::subscribe(embot::app::bldc::COMM *comm)
{
    bool r {nullptr != comm};
    
    if(true == r)
    {
        _config.comm = comm;
    }
    
    return r;
}


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::bldc::theCTRL& embot::app::bldc::theCTRL::getInstance()
{
    static theCTRL* p = new theCTRL();
    return *p;
}

embot::app::bldc::theCTRL::theCTRL()
{
    pImpl = std::make_unique<Impl>();
}  
    
embot::app::bldc::theCTRL::~theCTRL() { }
        
bool embot::app::bldc::theCTRL::initialise(const Config &config)
{
    return pImpl->initialise(config);
}

bool embot::app::bldc::theCTRL::subscribe(embot::app::bldc::COMM *comm)
{
    return pImpl->subscribe(comm);
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


