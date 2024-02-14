
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

#include "embot_app_eth_theICCservice.h"
#include "embot_app_application_theCANtracer.h"

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
    static constexpr embot::os::Event evt_COMM_RXiccframe { embot::core::binary::mask::pos2mask<embot::os::Event>(0) };
    static constexpr embot::os::Event evt_COMM_TXiccframes { embot::core::binary::mask::pos2mask<embot::os::Event>(1) };
          
    std::vector<embot::prot::can::Frame> _tCOMMoutframes;
    embot::hw::CAN _tCOMMcanbus {embot::hw::CAN::none};
    embot::prot::can::Address _tCOMMcanaddress {1};
    
    embot::app::application::dummyCANagentCORE _dummycanagentcore {};
        
    embot::os::EventThread *_t_COMM {nullptr};
    #warning move _location elsewhere in file _info.cpp ....
    embot::msg::Location _location {embot::msg::BUS::icc1, 3};
      
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
    
    // when we have a frame in embot::hw:icc to retrieve
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
//    _tCOMMcanbus = _config.canagentcore->bus();
    _tCOMMcanaddress = _config.canagentcore->address();   

    
    embot::app::theCANtracer::getInstance().initialise({_tCOMMcanaddress});
               
    // the tCOMM    
    embot::os::EventThread::Config tCOMM_cfg 
    {
        _config.tCOMM_stacksize, _config.priority,
        tCOMM_Startup, this,
        _config.tCOMM_timeout,
        tCOMM_OnEvent,
        "tCOMMicc"
    };   
    _t_COMM = new embot::os::EventThread;
    
    // init the CORE parser
    embot::app::application::theCANparserCORE::getInstance().initialise({_config.canagentcore});    
    
    // init the the MSGbroker
    embot::app::bldc::theMSGbroker::getInstance().initialise({});        
        
    // and subscribe the tCOMM to be triggered by the OUT direction of the MSGbroker
    embot::os::Action act {embot::os::EventToThread(evt_COMM_TXiccframes, _t_COMM)};
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
        evthr->setEvent(evt_COMM_RXiccframe);
    }
}

void embot::app::bldc::theCOMM::Impl::tCOMM_Startup(embot::os::Thread *t, void *param)
{
    Impl *impl = reinterpret_cast<Impl*>(param);
    
    // init ICC. at first w/ the default slave configuration. then w/ focused tuning       
    embot::app::eth::theICCservice::getInstance().initialise(embot::app::eth::iccslavecfg);  
    // alert this thread on RX of messages    
    embot::core::Callback oniccRX {impl->alertonrxframe, t};
    embot::app::eth::theICCservice::getInstance().set(oniccRX);
    // tx only on explicit command
    embot::app::eth::theICCservice::getInstance().set(embot::app::eth::theICCservice::modeTX::onflush);
    // use the dummy parser because we get messages one by one
    embot::app::eth::theICCservice::getInstance().set(nullptr);
    
    // pre-allocate output vector of frames
    impl->_tCOMMoutframes.reserve(impl->_config.tCOMMmaxOUTcanframes);    
              
}


void embot::app::bldc::theCOMM::Impl::tCOMM_OnTimeout(embot::os::Thread *t, void *param)
{

#if defined(TEST_theICCservice )

    #warning TEST_theICCservice  is defined ....

//    // print over debugger ... ?
//    // add a canprint to output queue _tCOMMoutframes
//    // send a ...
//    
//    static constexpr embot::core::Time period {5000*embot::core::time1millisec};    
//    static embot::core::Time lastcall {0};    
//    embot::core::Time now = embot::core::now();
//    embot::core::Time delta = now - lastcall;    
//    if(delta < period)
//    {
//        return;
//    }    
//    lastcall = now;
//    
//    // and now what i have to do
//    
//    Impl *impl = reinterpret_cast<Impl*>(param);    

//    embot::core::print("emitting a CAN PRINT w/ string = 5sEc");    
//    embot::app::theCANtracer::getInstance().print("5sEc", impl->_tCOMMoutframes);   
//    t->setEvent(evt_COMM_TXiccframes);    
    
#endif    
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
    
    if(true == embot::core::binary::mask::check(eventmask, evt_COMM_RXiccframe))
    {
        // DEBUG
        impl->tCOMM_OnTimeout(t, param);
        
#if defined(PARSE_RX_ALLINONESHOT)
        
        size_t insideRXQ = embot::app::eth::theICCservice::getInstance().input();
        size_t remainingINrx = 0;
        embot::app::eth::theICCservice::Item item {};
        for(size_t i=0; i<insideRXQ; i++)
        {
            remainingINrx = 0;   
            if(true == embot::app::eth::theICCservice::getInstance().get(item, remainingINrx))
            {
                impl->tCOMM_OnRXframe(t, eventmask, param, item.frame, impl->_tCOMMoutframes); 
            }
        }
        // if any arrives since we called embot::hw::can::inputqueuesize(tCOMMcanbus) ...
        if(remainingINrx > 0)
        {
            t->setEvent(evt_COMM_RXiccframe);                 
        }
        
//        // attempt to get all messages in one shot
//        std::uint8_t insideRXQ = embot::hw::can::inputqueuesize(impl->_tCOMMcanbus);
//        std::uint8_t remainingINrx = 0;
//        embot::hw::can::Frame hwframe {};
//        for(uint8_t i=0; i<insideRXQ; i++)
//        {
//            remainingINrx = 0;            
//            if(embot::hw::resOK == embot::hw::can::get(impl->_tCOMMcanbus, hwframe, remainingINrx))
//            {            
//                impl->tCOMM_OnRXframe(t, eventmask, param, {hwframe.id, hwframe.size, hwframe.data}, impl->_tCOMMoutframes);                                
//            }
//        }
//        // if any arrives since we called embot::hw::can::inputqueuesize(tCOMMcanbus) ...
//        if(remainingINrx > 0)
//        {
//            t->setEvent(evt_COMM_RXiccframe);                 
//        }
#else        

#endif        
    }
    
    if(true == embot::core::binary::mask::check(eventmask, evt_COMM_TXiccframes))
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
        {
            embot::msg::Location destination { embot::msg::BUS::icc1, 0 }; 
            embot::app::eth::theICCservice::Item item { destination, impl->_tCOMMoutframes[i] };            
            bool r = embot::app::eth::theICCservice::getInstance().put(item, embot::core::reltimeWaitForever);
            // if r is false then  we have a failure. but timeout is infinite            
        }
        bool r1 = embot::app::eth::theICCservice::getInstance().flush(embot::core::reltimeWaitForever);        
    } 
    
    // and now we can clear the frames to be trasmitted
    impl->_tCOMMoutframes.clear();          
}


void embot::app::bldc::theCOMM::Impl::tCOMM_OnRXframe(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &outframes)
{
    constexpr uint8_t CANaddressMASTER {0};
    
//    embot::core::print("received: " + frame.to_string());   
    
            
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


