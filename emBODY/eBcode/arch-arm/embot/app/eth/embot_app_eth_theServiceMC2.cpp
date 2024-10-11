
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theServiceMC.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_Service_impl.h"
#include "embot_app_eth_Service_impl_mc_SERVICE.h"
#include "embot_app_eth_theErrorManager.h"
#include "embot_os_theScheduler.h"
#include "embot_app_eth_theServices.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


namespace embot::app::eth {


struct embot::app::eth::theServiceMC::Impl
{         
    theServiceMC *_owner {nullptr};       
    theServiceMC::Config _config {}; // so far empty        
    // it contains a great deal of useful code
    embot::app::eth::service::impl::mc::SERVICE _mcservice {};
            
    Impl(theServiceMC *o) : _owner(o) {};            
    bool initialise(); 
        
    // interface to Service          
    Category category() const { return embot::app::eth::Service::Category::mc; }
    Type type() const { return _mcservice.type(); }
    State state() const { return _mcservice.state(); }
    void set(State s) { _mcservice.state(s); }
    
    bool verifyactivate(const embot::app::eth::Service::Config * servcfg, OnEndOfOperation onendofverifyactivate);
    bool deactivate();
    bool start();
    bool stop();
    bool set(const eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t& numberofthem);
    bool tick(bool resetstatus);
    bool report();
    bool process(const DescriptorCANframe &canframedescriptor);
    bool process(const DescriptorFrame &framedescriptor);
    bool process(const DescriptorROP &ropdescriptor);
    
    // others
private: 
   
    static constexpr char p_objname[] = "theServiceMC";

    void p_traceprint(const char *str)
    {
        embot::app::eth::theErrorManager::getInstance().trace(str, {p_objname, embot::os::theScheduler::getInstance().scheduled()}); 
    }  

    static bool p_isID32relevant(uint32_t id32);    
};


      
bool embot::app::eth::theServiceMC::Impl::initialise()
{
    p_traceprint("initialise()");

    if(true == _mcservice.initted())
    {
        return true;
    }
    
    constexpr size_t maxregulars {maxREGULARs};
    _mcservice.initialise(maxregulars);
    
    // synch service to theServices. at this stage it will impose embot::app::eth::Service::State::idle 
    _mcservice.synch(embot::app::eth::Service::State::idle);
    
#if 0
    we are in:
    state()     State::idle
    active()    false
    started()   false
#endif    

    return true;
}


bool embot::app::eth::theServiceMC::Impl::stop()
{
    p_traceprint("stop()");

#if 0
    it generates the transition State::started --> State::activated, 
    so to preceed we must be in:
    state()     State::started
    active()    true
    started()   true
#endif      
    
    if(embot::app::eth::Service::State::started != _mcservice.state())
    {
        // it is not is State::started, so we dont proceed
        // the return value is not very important, but we use:
        // - true if we already are in the target State::activated, or
        // - false to tell that stop() was wrongly called 
        return embot::app::eth::Service::State::activated == _mcservice.state();
    }
    
    // do action
    _mcservice.stop();

    // and then update the state to activated 
    _mcservice.synch(embot::app::eth::Service::State::activated);

#if 0
    we are in:
    state()     State::activated
    active()    true
    started()   false
#endif
    
    return true;
}

bool embot::app::eth::theServiceMC::Impl::deactivate()
{
    p_traceprint("deactivate()");
    
    // we should call deactivate() only if we are in State::activated or State::failureofverifying, BUT
    // we want to be sure that this action ALWAYS sends the service in State::idle, SO:
    // there are other state to be considered:
    
    embot::app::eth::Service::State s = _mcservice.state();
    
    
    // 1. if the service is in State::verifying, then YRI is doing some great mess because it has just sent two consecutive
    // requests, the former of verifyactivate and the latter of deactivate.
    // it is difficult to recover because the State::verifying requires some background activity that cannot be easily stopped,
    // rather it must terminate by itself. also, the verification activities depend on the kind of service type, so at the end on
    // the underlying embot::app::eth::service::impl::mc::IFagent.
    // SO, we just call a forceidle() method that will do the right thing and we just return
    //
    // 2. if the state is in State::verified, then we are in the same situation of a previous verifyactivate request from YRI but we 
    // have just terminated the verification phase. even in here, we call a forceidle() method and we just return
    // 

    if((embot::app::eth::Service::State::verifying == s) || (embot::app::eth::Service::State::verified == s))
    {
        _mcservice.forceidle(); 
        _mcservice.synch(embot::app::eth::Service::State::idle);
        return true;        
    } 
    
    // 3. for States::idle or ::failureofverify i just clear, synch to ::idle and return true.     
    
    if((embot::app::eth::Service::State::idle == s) || (embot::app::eth::Service::State::failureofverify == s))
    {
        _mcservice.clear(); 
        _mcservice.synch(embot::app::eth::Service::State::idle);
        return true;        
    }      

    // 4. if the service is activated (so States::started or ::activated), we proceed as follows 
#if 0
    we are in:
    state()     State::started or State::activated
    active()    true
    started()   false or false
#endif    
    
    // if the service is in State::started at first we stop it so that it goes into state ::activated
    if(embot::app::eth::Service::State::started == s)
    {
        Impl::stop();   
    }    

#if 0
    we are in:
    state()     State::activated
    active()    true
    started()   false
#endif 
    
    // we just need to make sure the regulars are cleared
    uint8_t n {0};
    Impl::set(nullptr, n);  
    
    // then we call the proper deactivate of the mcservice that sends into ::idle
    _mcservice.deactivate();
    
    // i may also force again to State::idle 
    _mcservice.synch(embot::app::eth::Service::State::idle);


#if 0
    we are in:
    state()     State::idle
    active()    false
    started()   false
#endif 
        
    return true;
}


bool embot::app::eth::theServiceMC::Impl::verifyactivate(const embot::app::eth::Service::Config * servcfg, OnEndOfOperation onendofverifyactivate)
{
    p_traceprint("verifyactivate()");
    
    // we should call verifyactivate() only if we are in State::idle, BUT
    // we want to be sure that any wrong request of verifyactivate() does not harm the service, SO:
    // there are other state to be considered:
    
    embot::app::eth::Service::State s = _mcservice.state();

#if 0
    we are in:
    state()     any
    active()    any
    started()   any
#endif 

    // 1. if the service is in State::verifying, then YRI is doing some great mess because it has just sent two consecutive
    // requests of verifyactivate.
    // it is difficult to recover because the State::verifying requires some background activity that cannot be easily stopped,
    // rather it must terminate by itself. also, the verification activities depend on the kind of service type, so at the end on
    // the underlying embot::app::eth::service::impl::mc::IFagent.
    // SO, we just call a forceidle() method that will do the right thing and we just return false with a failure
    //
    // 2. if the state is in State::verified, then we are in the same situation of a previous verifyactivate request from YRI but we 
    // have just terminated the verification phase. even in here, we call a forceidle() method and we just return
    // 
    
    if((embot::app::eth::Service::State::verifying == s) || (embot::app::eth::Service::State::verified == s))
    {
        // damn: the board is busy in verifying what YRI has asked, YRI is waiting for a reply
        // so we should never have received this request, SO: we trye to recover.
        // we ask the underlying verification to interrupt itself and go in a safe State::idle
        // and in here we transmit back a failure 
        //         
        _mcservice.forceidle();        
        _mcservice.synch(embot::app::eth::Service::State::idle);
        
        if(nullptr != onendofverifyactivate)
        {
            onendofverifyactivate(_owner, false); 
        } 
        #if 0
            we are in:
            state()     State::failureofverify
            active()    false
            started()   false
        #endif           
        return false;                
    }
    
    // 3. for States::idle or ::failureofverify i just clear, synch to ::idle and continue.
    
    if((embot::app::eth::Service::State::idle == s) || (embot::app::eth::Service::State::failureofverify == s))
    {
        _mcservice.clear(); 
        _mcservice.synch(embot::app::eth::Service::State::idle);      
    }     
    
    // 4. if we are in State::activated or ::started I need first to go back to ::idle. 
    // a call to deactivate() does it perfectly and also such to State::idle)

    if((embot::app::eth::Service::State::activated == s) || (embot::app::eth::Service::State::started == s))
    {    
        deactivate();
    }
    
 
    // 5. now we are sure we are in State::idle, so we proceed safely
    
#if 0

    state()     State::idle
    active()    false
    started()   false
#endif 
            
    if((nullptr == servcfg) || (embot::app::eth::Service::Config::Type::eOmn_serv != servcfg->type()))
    { 
        _mcservice.synch(embot::app::eth::Service::State::failureofverify);        
        if(nullptr != onendofverifyactivate)
        {
            onendofverifyactivate(_owner, false); 
        }        
        return false;
    }
        
    // now we check if service configuration is OK

    const eOmn_serv_configuration_t * svc =  reinterpret_cast<const eOmn_serv_configuration_t*>(servcfg->memory());      
    const embot::app::eth::Service::Type t {static_cast<const embot::app::eth::Service::Type>(svc->type)};
    
    if(false == _mcservice.supported(t))  
    {
        p_traceprint("verifyactivate() has unrecognised type");   
        _mcservice.synch(embot::app::eth::Service::State::failureofverify);        
        if(nullptr != onendofverifyactivate)
        {
            onendofverifyactivate(_owner, false); 
        }        
        return false;
    }
    

    // ok, i can verify
     
    // load and copy inside the service configuration
    if(false == _mcservice.load(_owner, svc))
    {
        _mcservice.synch(embot::app::eth::Service::State::failureofverify);        
        if(nullptr != onendofverifyactivate)
        {
            onendofverifyactivate(_owner, false); 
        } 
        #if 0
            we have failed the verification because for wrong configuration
            we are in:
            state()     State::failureofverify
            active()    false
            started()   false
        #endif           
        return false;        
    }
    
    // assign the proper state before calling _mcservice.verifyactivate()
    // even if _mcservice.verifyactivate() may change it internally
    _mcservice.synch(embot::app::eth::Service::State::verifying);    

#if 0
    we are in:
    state()     State::verifying
    active()    false
    started()   false
#endif 
    
    // start the process. at end of verification we may go to states:
    // - failureofverify, or
    // - verified and then into activated   
    if(false == _mcservice.verifyactivate(onendofverifyactivate))
    {
        _mcservice.synch(embot::app::eth::Service::State::failureofverify);        
        if(nullptr != onendofverifyactivate)
        {
            onendofverifyactivate(_owner, false); 
        } 
        #if 0
            the verification can be done immediatley and has failed because required resources are not present
            we are in:
            state()     State::failureofverify
            active()    false
            started()   false
        #endif         
        return false;         
    }
    else
    {
#if 0
    if the verification can be done immediately the service is also activated, so:  
    state()     State::activated 
    active()    true
    started()   false 
    else (as in the case of a CAN discovery) the service stays in State::verifying until 
    the check is done. it will be another thread that will send the service in State::failureofverifying or
    ::verified and then ::activated
    for the case of CAN discovery only it will be the CAN receiving thread etc.        
#endif         
    }
    
    return true;
}


bool embot::app::eth::theServiceMC::Impl::start()
{
    p_traceprint("start()");

#if 0
    it generates the transition State::activated --> State::started, 
    so to preceed we must be in:
    state()     State::activated
    active()    true
    started()   false
#endif      
    
    if(embot::app::eth::Service::State::activated != _mcservice.state())
    {
        // it is not is State::activated, so we dont proceed
        // the return value is not very important, but we use:
        // - true if we already are in the target State::started, or
        // - false to tell that start() was wrongly called 
        return embot::app::eth::Service::State::started == _mcservice.state();
    }    
          
    _mcservice.start();

    
    // now i also synch to State::started 
    _mcservice.synch(embot::app::eth::Service::State::started);
   
#if 0
    we are in:
    state()     State::started
    active()    true
    started()   true
#endif  
    
    return true;
}



bool embot::app::eth::theServiceMC::Impl::set(const eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t& numberofthem)
{
    p_traceprint("set(ID32)");
    
#if 0
    it must be activated, so:
    state()     State::activated or ::started
    active()    true
    started()   false or true
#endif      
    
    if(false == _mcservice.active())
    {   // nothing to do because object must be activated first
        return false;
    } 

    bool r = embot::app::eth::theServices::getInstance().setREGULARS(_mcservice.core.id32ofRegulars, arrayofid32, p_isID32relevant, &numberofthem);
    
    return r;  
}

bool embot::app::eth::theServiceMC::Impl::tick(bool resetstatus)
{
//    p_traceprint("tick()");
    
#if 0
    it is called by theHandler during the DO phase every 1 ms but we executed it only if:
    state()     State::started
    active()    true
    started()   true
#endif     
    
        
    if(false == _mcservice.started())
    {   // not yet started, so we dont execute the tick(). we may return true
        return true;
    }
     
    _mcservice.tick();
       
    return true;
}

bool embot::app::eth::theServiceMC::Impl::report()
{
    p_traceprint("report()");
    //#warning TODO: add embot::app::eth::theServiceMC::Impl::report()
    return true;
}

    
bool embot::app::eth::theServiceMC::Impl::process(const DescriptorCANframe &canframedescriptor)
{
//    p_traceprint("process(DescriptorCANframe)");    
    bool r {false};

    // i proceed a CAN frame only if service is active .... 
    if(false == _mcservice.active())
    {
        return r;
    }
    
    if(nullptr == canframedescriptor.frame)
    {
        return r;
    }
    
    return _mcservice.process(canframedescriptor);
}

bool embot::app::eth::theServiceMC::Impl::process(const DescriptorFrame &framedescriptor)
{
//    p_traceprint("process(DescriptorFrame)");    
    bool r {false};

    // i proceed a CAN/ICC frame only if service is active .... 
    if(false == _mcservice.active())
    {
        return r;
    }
        
    return _mcservice.process(framedescriptor);
}

bool embot::app::eth::theServiceMC::Impl::process(const DescriptorROP &ropdescriptor)
{
//    p_traceprint("process(DescriptorROP)");
    
    bool r {false};
    
    // i proceed a ROP only if the service is active .... 
    if(false == _mcservice.active())
    {
        return r;
    }
    
    // and if we have a valid descriptor
    if((nullptr == ropdescriptor.rd) || (nullptr == ropdescriptor.nv))
    {
        return r;
    }
    
    return _mcservice.process(ropdescriptor);
}


// --------------------------------------------------------------------------------------------------------------------
// - private methods of Impl
// --------------------------------------------------------------------------------------------------------------------


bool embot::app::eth::theServiceMC::Impl::p_isID32relevant(uint32_t id32)
{
    static const uint32_t mask0 = (((uint32_t)eoprot_endpoint_motioncontrol) << 24) | (((uint32_t)eoprot_entity_mc_joint) << 16);
    static const uint32_t mask1 = (((uint32_t)eoprot_endpoint_motioncontrol) << 24) | (((uint32_t)eoprot_entity_mc_motor) << 16);
    static const uint32_t mask2 = (((uint32_t)eoprot_endpoint_motioncontrol) << 24) | (((uint32_t)eoprot_entity_mc_controller) << 16);
    
    if((id32 & mask0) == mask0)
    {
        return(true);
    }
    if((id32 & mask1) == mask1)
    {
        return(true);
    } 
    if((id32 & mask2) == mask2)
    {
        return(true);
    }    
    
    return(false); 
}

} // namespace embot::app::eth {

// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::eth::theServiceMC& embot::app::eth::theServiceMC::getInstance()
{
    static theServiceMC* p = new theServiceMC();
    return *p;
}

embot::app::eth::theServiceMC::theServiceMC()
{
    pImpl = std::make_unique<Impl>(this);
}  

    
embot::app::eth::theServiceMC::~theServiceMC() { }
        

bool embot::app::eth::theServiceMC::initialise(const Config &config)
{
    pImpl->_config = config;
    return pImpl->initialise();
}

embot::app::eth::Service* embot::app::eth::theServiceMC::service()
{
    return this;
}

embot::app::eth::Service::Category embot::app::eth::theServiceMC::category() const 
{
    return pImpl->category();
}

embot::app::eth::Service::Type embot::app::eth::theServiceMC::type() const
{
    return pImpl->type();
}

embot::app::eth::Service::State embot::app::eth::theServiceMC::state() const
{
    return pImpl->state();
}

void embot::app::eth::theServiceMC::set(embot::app::eth::Service::State s)
{
    pImpl->set(s);
}

bool embot::app::eth::theServiceMC::verifyactivate(const embot::app::eth::Service::Config * servcfg, OnEndOfOperation onendofverifyactivate)
{
    return pImpl->verifyactivate(servcfg, onendofverifyactivate);
}

bool embot::app::eth::theServiceMC::deactivate()
{
    return pImpl->deactivate();
}

bool embot::app::eth::theServiceMC::start()
{
    return pImpl->start();
}

bool embot::app::eth::theServiceMC::stop()
{
    return pImpl->stop();
}

bool embot::app::eth::theServiceMC::setregulars(const eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t& numberofthem)
{
    return pImpl->set(arrayofid32, numberofthem);
}

bool embot::app::eth::theServiceMC::tick(bool resetstatus)
{
    return pImpl->tick(resetstatus);
}

bool embot::app::eth::theServiceMC::report()
{
    return pImpl->report();
}

bool embot::app::eth::theServiceMC::process(const DescriptorCANframe &canframedescriptor)
{
    return pImpl->process(canframedescriptor);
}

bool embot::app::eth::theServiceMC::process(const DescriptorFrame &framedescriptor)
{
    return pImpl->process(framedescriptor);
}

bool embot::app::eth::theServiceMC::process(const DescriptorROP &ropdescriptor)
{
    return pImpl->process(ropdescriptor);
}


// other objects




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


