
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_bldc_theCOMM.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_core.h"

#include <array>

#include "embot_os_rtos.h"
#include "embot_os_Thread.h"
#include "embot_os_Action.h"

#include "embot_app_msg.h"
#include "embot_hw_can.h"

#include "embot_app_application_theCANparserCORE.h"
#include "embot_app_bldc_theMSGbroker.h"

// --------------------------------------------------------------------------------------------------------------------
// - defines
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


namespace embot::app::application {



} // end of namespace

void thrCOMM(void* p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }

struct embot::app::bldc::theCOMM::Impl
{
    static constexpr embot::os::Event evt_COMM_RXcanframe { embot::core::binary::mask::pos2mask<embot::os::Event>(0) };
    static constexpr embot::os::Event evt_COMM_TXcanframes { embot::core::binary::mask::pos2mask<embot::os::Event>(1) };
          
    std::vector<embot::prot::can::Frame> _tCOMMtmpCANframes;
    std::vector<embot::app::bldc::MSG> _tCOMMoutmessages;
    embot::hw::CAN _tCOMMcanbus {embot::hw::CAN::none};
    embot::prot::can::Address _tCOMMcanaddress {1};
    
    embot::app::application::dummyCANagentCORE _dummycanagentcore {};
        
    embot::os::EventThread *_t_COMM {nullptr};
      
    Config _config {};
    bool _initted {false};
    
    embot::app::msg::Location CANlocation {embot::app::msg::BUS::can1, 1}; 
    
    Impl() = default;  
    
    // for the thread tCOMM and the embot::hw::can driver
    static void tCOMM_Startup(embot::os::Thread *t, void *param);
    static void tCOMM_OnEvent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param);      
    static void alertonrxframe(void *arg);      
    
    // the initialization code
    bool initialise(const Config &config);       
    // on timeout                
    void tCOMM_OnTimeout(embot::os::Thread *t, void *param);
    
    // when we have a msg to retrieve from embot::hw::can
    void tCOMM_OnRXmessage(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, const embot::app::bldc::MSG &msg, std::vector<embot::app::bldc::MSG> &outmsgs);
        
    // interface to COMM. it is used in this case by theCTRL
    bool add(const embot::app::bldc::MSG &msg);
    bool add(const std::vector<embot::app::bldc::MSG> &msgs);
    bool get(size_t &remaining, embot::app::bldc::MSG &msg);
    bool get(size_t &remaining, std::vector<embot::app::bldc::MSG> &msgs, size_t &retrieved, const size_t max2retrieve);
};

      
bool embot::app::bldc::theCOMM::Impl::initialise(const Config &config)
{
    if(true == _initted)
    {
        return true;
    }
    
    embot::core::print("embot::app::bldc::theCOMM::Impl::initialise()");
    
    if(false == config.isvalid())
    {
        return false;
    }
    
    _config = config;
    
    if(nullptr == _config.canagentcore)
    {
        _config.canagentcore = &_dummycanagentcore;
    }    
    
    // retrieve some CAN constants
    _tCOMMcanbus = _config.canagentcore->bus();
    _tCOMMcanaddress = _config.canagentcore->address();

    embot::app::msg::BUS cb = (embot::hw::CAN::none == _tCOMMcanbus) ? (embot::app::msg::BUS::none) : static_cast<embot::app::msg::BUS>(_tCOMMcanbus);
    CANlocation.set(cb, _tCOMMcanaddress);
               
    // the tCOMM    
    embot::os::EventThread::Config tCOMM_cfg 
    {
        _config.tCOMM_stacksize, _config.priority,
        tCOMM_Startup, this,
        _config.tCOMM_timeout,
        tCOMM_OnEvent,
        "tCOMM"
    };   
    _t_COMM = new embot::os::EventThread;
    
    // init the CORE parser
    embot::app::application::theCANparserCORE::getInstance().initialise({_config.canagentcore});    
    
    // init the the MSGbroker
    embot::app::bldc::theMSGbroker::getInstance().initialise({});        
      
    // and subscribe the tCOMM to be triggered by the OUT direction of the MSGbroker
    embot::os::Action act {embot::os::EventToThread(evt_COMM_TXcanframes, _t_COMM)};
    embot::app::bldc::theMSGbroker::getInstance().subscribe(embot::app::bldc::theMSGbroker::Direction::OUT, act);            
    
    // and now ... start the tCOMM   
    _t_COMM->start(tCOMM_cfg, thrCOMM);
        
    
    _initted = true;
    return _initted;
}

void embot::app::bldc::theCOMM::Impl::alertonrxframe(void *arg)
{
    embot::os::EventThread* evthr = reinterpret_cast<embot::os::EventThread*>(arg);
    if(nullptr != evthr)
    {
        evthr->setEvent(evt_COMM_RXcanframe);
    }
}

void embot::app::bldc::theCOMM::Impl::tCOMM_Startup(embot::os::Thread *t, void *param)
{
    Impl *impl = reinterpret_cast<Impl*>(param);
    
    // init the can ...
    embot::hw::can::Config canconfig {};
    canconfig.rxcapacity = impl->_config.tCOMMmaxINPmessages;
    canconfig.txcapacity = impl->_config.tCOMMmaxOUTmessages;
    canconfig.onrxframe = embot::core::Callback(impl->alertonrxframe, t); 
    embot::hw::can::init(impl->_tCOMMcanbus, canconfig);
    embot::hw::can::setfilters(impl->_tCOMMcanbus, impl->_tCOMMcanaddress);        
    // pre-allocate output vector of messages
    impl->_tCOMMoutmessages.reserve(impl->_config.tCOMMmaxOUTmessages);
    // and can frames
    impl->_tCOMMtmpCANframes.reserve(impl->_config.tCOMMmaxOUTmessages);

    // and ok, enable it
    embot::hw::can::enable(impl->_tCOMMcanbus);
        
//    // init the CORE parser
//    embot::app::application::theCANparserCORE::getInstance().initialise({impl->_config.canagentcore});    
//    
//    // init the the MSGbroker
//    embot::app::bldc::theMSGbroker::getInstance().initialise({});        
//        
//    // and subscribe the OUT direction to this thread
//    embot::os::Action act {embot::os::EventToThread(evt_COMM_TXcanframes, t)};
//    embot::app::bldc::theMSGbroker::getInstance().subscribe(embot::app::bldc::theMSGbroker::Direction::OUT, act);        
}

void embot::app::bldc::theCOMM::Impl::tCOMM_OnTimeout(embot::os::Thread *t, void *param)
{
    
}

#define PARSE_RX_ALLINONESHOT

void embot::app::bldc::theCOMM::Impl::tCOMM_OnEvent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{
    Impl *impl = reinterpret_cast<Impl*>(param);
    
   // manage the events
    if(0 == eventmask)
    {
        impl->tCOMM_OnTimeout(t, param);
        return;
    }
    
    if(true == embot::core::binary::mask::check(eventmask, evt_COMM_RXcanframe))
    {
#if defined(PARSE_RX_ALLINONESHOT)
        // attempt to get all messages in one shot
        std::uint8_t insideRXQ = embot::hw::can::inputqueuesize(impl->_tCOMMcanbus);
        std::uint8_t remainingINrx = 0;
        embot::hw::can::Frame hwframe {};
        for(uint8_t i=0; i<insideRXQ; i++)
        {
            remainingINrx = 0;            
            if(embot::hw::resOK == embot::hw::can::get(impl->_tCOMMcanbus, hwframe, remainingINrx))
            {
                embot::app::bldc::MSG msg { impl->CANlocation, {hwframe.id, hwframe.size, hwframe.data}};
                impl->tCOMM_OnRXmessage(t, eventmask, param, msg, impl->_tCOMMoutmessages);                                
            }
        }
        // if any arrives since we called embot::hw::can::inputqueuesize(tCOMMcanbus) ...
        if(remainingINrx > 0)
        {
            t->setEvent(evt_COMM_RXcanframe);                 
        }
#else        
        // get all the messages, one at a time.
        // we pass it through the basic parser. if not parsed we it inside the theMSGbroker
        embot::hw::can::Frame hwframe;
        std::uint8_t remainingINrx = 0;
        if(embot::hw::resOK == embot::hw::can::get(impl->_tCOMMcanbus, hwframe, remainingINrx))
        {            
            impl->tCOMM_OnRXmessage(t, eventmask, param, {hwframe.id, hwframe.size, hwframe.data}, impl->_tCOMMoutmessages);               
            
            if(remainingINrx > 0)
            {
                t->setEvent(evt_COMM_RXcanframe);                 
            }
        }
#endif        
    }
    
    if(true == embot::core::binary::mask::check(eventmask, evt_COMM_TXcanframes))
    {        
        // get all the messages from the theMSGbroker and put them inside the tCOMMoutframes
        size_t remaining {0};
        size_t retrieved {0};
        embot::app::bldc::theMSGbroker::getInstance().rem(embot::app::bldc::theMSGbroker::Direction::OUT, remaining, impl->_tCOMMoutmessages, retrieved);
    }  

    // finally, if we have any packet we transmit them
    size_t num = impl->_tCOMMoutmessages.size();
    if(num > 0)
    {
        for(size_t i=0; i<num; i++)
        {   // marco.accame: however, i cannot put more than 255 frames inside the output queue ...
            embot::hw::can::Frame hwframe {impl->_tCOMMoutmessages[i].frame.id, impl->_tCOMMoutmessages[i].frame.size, impl->_tCOMMoutmessages[i].frame.data};
            embot::hw::can::put(impl->_tCOMMcanbus, hwframe);                                       
        }
        embot::hw::can::transmit(impl->_tCOMMcanbus);  
    } 
    
    // and now we can clear the msgs to be trasmitted
    impl->_tCOMMoutmessages.clear();          
}


void embot::app::bldc::theCOMM::Impl::tCOMM_OnRXmessage(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, const embot::app::bldc::MSG &msg, std::vector<embot::app::bldc::MSG> &outmsgs)
{
    constexpr uint8_t CANaddressMASTER {0};
    
    _tCOMMtmpCANframes.clear();

    if(true == embot::app::application::theCANparserCORE::getInstance().process(msg.frame, _tCOMMtmpCANframes))
    {                   
    }
    else if(CANaddressMASTER == embot::prot::can::frame2sender(msg.frame))
    {
        embot::app::bldc::theMSGbroker::getInstance().add(embot::app::bldc::theMSGbroker::Direction::INP, msg);
    }

    if(false == _tCOMMtmpCANframes.empty())
    {
        for(const auto &i : _tCOMMtmpCANframes)
        {
            outmsgs.push_back({msg.location, i});
        }
    }
}


bool embot::app::bldc::theCOMM::Impl::add(const embot::app::bldc::MSG &msg)
{
    return embot::app::bldc::theMSGbroker::getInstance().add(embot::app::bldc::theMSGbroker::Direction::OUT, msg);
}

bool embot::app::bldc::theCOMM::Impl::add(const std::vector<embot::app::bldc::MSG> &msgs)
{
   return embot::app::bldc::theMSGbroker::getInstance().add(embot::app::bldc::theMSGbroker::Direction::OUT, msgs);
}

bool embot::app::bldc::theCOMM::Impl::get(size_t &remaining, embot::app::bldc::MSG &msg)
{
   return embot::app::bldc::theMSGbroker::getInstance().rem(embot::app::bldc::theMSGbroker::Direction::INP, remaining, msg);
}

bool embot::app::bldc::theCOMM::Impl::get(size_t &remaining, std::vector<embot::app::bldc::MSG> &msgs, size_t &retrieved, const size_t max2retrieve)
{
   return embot::app::bldc::theMSGbroker::getInstance().rem(embot::app::bldc::theMSGbroker::Direction::INP, remaining, msgs, retrieved, max2retrieve);
}


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::bldc::theCOMM& embot::app::bldc::theCOMM::getInstance()
{
    static theCOMM* p = new theCOMM();
    return *p;
}

embot::app::bldc::theCOMM::theCOMM()
{
    pImpl = std::make_unique<Impl>();
}  
    
embot::app::bldc::theCOMM::~theCOMM() { }
        
bool embot::app::bldc::theCOMM::initialise(const Config &config)
{
    return pImpl->initialise(config);
}

//bool embot::app::bldc::theCOMM::subscribe(Direction dir, const embot::os::Action &action)
//{
//    return pImpl->subscribe(dir, action);
//}



bool embot::app::bldc::theCOMM::add(const embot::app::bldc::MSG &msg)
{
    return pImpl->add(msg);
}

bool embot::app::bldc::theCOMM::add(const std::vector<embot::app::bldc::MSG> &msgs)
{
    return pImpl->add(msgs);
}

bool embot::app::bldc::theCOMM::get(size_t &remaining, embot::app::bldc::MSG &msg)
{
    return pImpl->get(remaining, msg);
}

bool embot::app::bldc::theCOMM::get(size_t &remaining, std::vector<embot::app::bldc::MSG> &msgs, size_t &retrieved, const size_t max2retrieve)
{
    return pImpl->get(remaining, msgs, retrieved, max2retrieve);
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


