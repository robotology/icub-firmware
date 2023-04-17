
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
          
    std::vector<embot::prot::can::Frame> _tCOMMoutframes;
    embot::hw::CAN _tCOMMcanbus {embot::hw::CAN::none};
    embot::prot::can::Address _tCOMMcanaddress {1};
    
    embot::app::application::dummyCANagentCORE _dummycanagentcore {};
        
    embot::os::EventThread *_t_COMM {nullptr};
      
    Config _config {};
    bool _initted {false};
    
    
    Impl() = default;  
    
    // for the thread tCOMM and the embot::hw::can driver
    static void tCOMM_Startup(embot::os::Thread *t, void *param);
    static void tCOMM_OnEvent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param);      
    static void alertonrxframe(void *arg);      
    
    // the initialization code
    bool initialise(Config config);       
    // on timeout                
    void tCOMM_OnTimeout(embot::os::Thread *t, void *param);
    
    // when we have a frame in embot::hw::can to retrieve
    void tCOMM_OnRXframe(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &outframes);
        
    // interface to COMM. it is used in this case by theCTRL
    bool add(const embot::prot::can::Frame &frame);
    bool add(const std::vector<embot::prot::can::Frame> &frames);
    bool get(size_t &remaining, embot::prot::can::Frame &frame);
    bool get(size_t &remaining, std::vector<embot::prot::can::Frame> &frames, size_t &retrieved, const size_t max2retrieve);
};

      
bool embot::app::bldc::theCOMM::Impl::initialise(const Config config)
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
    canconfig.rxcapacity = impl->_config.tCOMMmaxINPcanframes;
    canconfig.txcapacity = impl->_config.tCOMMmaxOUTcanframes;
    canconfig.onrxframe = embot::core::Callback(impl->alertonrxframe, t); 
    embot::hw::can::init(impl->_tCOMMcanbus, canconfig);
#if defined(STM32HAL_BOARD_AMC2C)
    #warning marco.accame: attenzione la amc2c non apprezza il can::setfilters
#else        
    embot::hw::can::setfilters(impl->_tCOMMcanbus, impl->_tCOMMcanaddress); 
#endif        
    // pre-allocate output vector of frames
    impl->_tCOMMoutframes.reserve(impl->_config.tCOMMmaxOUTcanframes);

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
                impl->tCOMM_OnRXframe(t, eventmask, param, {hwframe.id, hwframe.size, hwframe.data}, impl->_tCOMMoutframes);                                
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
            impl->tCOMM_OnRXframe(t, eventmask, param, {hwframe.id, hwframe.size, hwframe.data}, impl->_tCOMMoutframes);               
            
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
        embot::app::bldc::theMSGbroker::getInstance().rem(embot::app::bldc::theMSGbroker::Direction::OUT, remaining, impl->_tCOMMoutframes, retrieved);
    }  

    // finally, if we have any packet we transmit them
    size_t num = impl->_tCOMMoutframes.size();
    if(num > 0)
    {
        for(size_t i=0; i<num; i++)
        {   // marco.accame: however, i cannot put more than 255 frames inside the output queue ...
            embot::hw::can::put(impl->_tCOMMcanbus, {impl->_tCOMMoutframes[i].id, impl->_tCOMMoutframes[i].size, impl->_tCOMMoutframes[i].data});                                       
        }
        embot::hw::can::transmit(impl->_tCOMMcanbus);  
    } 
    
    // and now we can clear the frames to be trasmitted
    impl->_tCOMMoutframes.clear();          
}


void embot::app::bldc::theCOMM::Impl::tCOMM_OnRXframe(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &outframes)
{
    constexpr uint8_t CANaddressMASTER {0};
            
    if(true == embot::app::application::theCANparserCORE::getInstance().process(frame, outframes))
    {                   
    }
    else if(CANaddressMASTER == embot::prot::can::frame2sender(frame))
    {
        #warning TODO: call theMSGbroker.add(asRX, frame, outframes) outframes MAY be filled w/ a debug message
        embot::app::bldc::theMSGbroker::getInstance().add(embot::app::bldc::theMSGbroker::Direction::INP, frame);
    }   
}


bool embot::app::bldc::theCOMM::Impl::add(const embot::prot::can::Frame &frame)
{
    return embot::app::bldc::theMSGbroker::getInstance().add(embot::app::bldc::theMSGbroker::Direction::OUT, frame);
}

bool embot::app::bldc::theCOMM::Impl::add(const std::vector<embot::prot::can::Frame> &frames)
{
   return embot::app::bldc::theMSGbroker::getInstance().add(embot::app::bldc::theMSGbroker::Direction::OUT, frames);
}

bool embot::app::bldc::theCOMM::Impl::get(size_t &remaining, embot::prot::can::Frame &frame)
{
   return embot::app::bldc::theMSGbroker::getInstance().rem(embot::app::bldc::theMSGbroker::Direction::INP, remaining, frame);
}

bool embot::app::bldc::theCOMM::Impl::get(size_t &remaining, std::vector<embot::prot::can::Frame> &frames, size_t &retrieved, const size_t max2retrieve)
{
   return embot::app::bldc::theMSGbroker::getInstance().rem(embot::app::bldc::theMSGbroker::Direction::INP, remaining, frames, retrieved, max2retrieve);
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



bool embot::app::bldc::theCOMM::add(const embot::prot::can::Frame &frame)
{
    return pImpl->add(frame);
}

bool embot::app::bldc::theCOMM::add(const std::vector<embot::prot::can::Frame> &frames)
{
    return pImpl->add(frames);
}

bool embot::app::bldc::theCOMM::get(size_t &remaining, embot::prot::can::Frame &frame)
{
    return pImpl->get(remaining, frame);
}

bool embot::app::bldc::theCOMM::get(size_t &remaining, std::vector<embot::prot::can::Frame> &frames, size_t &retrieved, const size_t max2retrieve)
{
    return pImpl->get(remaining, frames, retrieved, max2retrieve);
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


