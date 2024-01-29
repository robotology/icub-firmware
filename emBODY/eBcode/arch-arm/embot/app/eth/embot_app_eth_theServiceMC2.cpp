
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
#include "embot_app_eth_theServiceMC_impl.h"
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
    embot::app::eth::service::impl::mc::MCservice _mcservice {};
            
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
    state()     idle
    active()    false
    started()   false
#endif    

    return true;
}


bool embot::app::eth::theServiceMC::Impl::stop()
{
    p_traceprint("stop()");

#if 0
    it generates trasnsition started --> activate, so to preceed we must be in started state
    state   started
    active  true
    started true
#endif      

    if(false == _mcservice.active())
    {   // nothing to do because object must be activated
        return true;
    } 
        
    if(false == _mcservice.started())
    {   // it is already stopped
        return true;
    }
    
    // do action
    _mcservice.stop();

    // and then uptdate the state to activated 
    _mcservice.synch(embot::app::eth::Service::State::activated);

#if 0
    we have
    state   activated
    active  true
    started false
#endif
    
    return true;
}

bool embot::app::eth::theServiceMC::Impl::deactivate()
{
    p_traceprint("deactivate()");
    
        
    // if the  service is not activated (so states: idle, failureofverify, verified or verifying) i just clear and synch to idle
    // actually the state verifying may give some problems but that should never happen
    if(false == _mcservice.active())
    {
        _mcservice.clear(); 
        _mcservice.synch(embot::app::eth::Service::State::idle);
        return true;        
    } 
    
#if 0
    now we surely are in
    state   started / activated
    active  true
    started true / false
#endif    
        
    
    // if the service is in state started at first we stop it so that it goes into state activated
    if(true == _mcservice.started())
    {
        Impl::stop();   
    }    

#if 0
    now we surely are in
    state   activated
    active  true
    started false
#endif 
    
    // we just need to make sure the regulars are cleared
    uint8_t n {0};
    Impl::set(nullptr, n);  
    
    // then we call the proper deactivate of the mcservice that sends into idle state
    _mcservice.deactivate();
    
    // i may also force gain the active flag to false and the state idle 
    _mcservice.synch(embot::app::eth::Service::State::idle);
    
    return true;
}


bool embot::app::eth::theServiceMC::Impl::verifyactivate(const embot::app::eth::Service::Config * servcfg, OnEndOfOperation onendofverifyactivate)
{
    p_traceprint("verifyactivate()");
    
#if 0
    state()     any, but normally: idle, activated, started, failureofverify. hopefully never verifying or verified
    active()    any
    started()   any
#endif 
    
    // if we already are in states activated or started we need to go back to idle. we call deactivate() that also calls stop() if needed.
    // but it would be much better to call deactivate() in any case because we want to go to idle in any case.
    // only thing: if we are in verifying state we should just drop the request.

#if 0
    // as it was     
    if(true == _mcservice.active())
    {
        deactivate();        
    }         
#else
    // as it is better to have
    if(embot::app::eth::Service::State::verifying == _mcservice.state())
    {
        // damn: the board is busy in verifying what YRI has asked, YRI is waiting for a reply and so we should never receive this request. 
        // nevertheless we dont continue and ...        
        _mcservice.forcestopofverify();        
        _mcservice.synch(embot::app::eth::Service::State::failureofverify);
        
        if(nullptr != onendofverifyactivate)
        {
            onendofverifyactivate(_owner, false); 
        }        
        return false;                
    }
    
    // in all other cases it is better to deactivate() first
    
    deactivate();

#endif
    
 
#if 0
    now we surely are in
    state     idle
    active    false
    started   false
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
        return false;        
    }
    
    // assign the proper state before calling _mcservice.verifyactivate()
    // because the final state is assigned 
    _mcservice.synch(embot::app::eth::Service::State::verifying);    


#if 0
    we are now
    state   verifying
    active  true
    started false
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
        return false;         
    }
    
    // i dont assign the state verifying now because ... it will be _mcservice
    // that internally does that or change the state.    
    
    return true;
}


bool embot::app::eth::theServiceMC::Impl::start()
{
    p_traceprint("start()");
    
#if 0
    to proceed we must have
    state   activated
    active  true
    started false
#endif      
       
    if(false == _mcservice.active())
    {   // nothing to do because object must be first activated
        return true;
    } 
        
    if(true == _mcservice.started())
    {   // it is already started
        return true;
    }
                       
//    // we just need to start a read of the encoder, nothing else 
//    embot::app::eth::theEncoderReader::getInstance().StartReading();
    
    _mcservice.start();

    
    // now i synch to state started 
    _mcservice.synch(embot::app::eth::Service::State::started);

    
#if 0
    and now we have
    state   started
    active  true
    started true
#endif   
    
    return true;
}



bool embot::app::eth::theServiceMC::Impl::set(const eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t& numberofthem)
{
    p_traceprint("set(ID32)");
    
#if 0
    must be
    state   activated / started
    active  true
    started false / true
#endif      
    
    if(false == _mcservice.active())
    {   // nothing to do because object must be activated
        return true;
    } 

    bool r = embot::app::eth::theServices::getInstance().setREGULARS(_mcservice.core.id32ofRegulars, arrayofid32, p_isID32relevant, &numberofthem);
    
    return r;  
}

bool embot::app::eth::theServiceMC::Impl::tick(bool resetstatus)
{
    p_traceprint("tick()");
    
#if 0
    must be
    state   started
    active  true
    started true
#endif  
    
        
    if(false == _mcservice.started())
    {   // not yet started
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
    p_traceprint("process(DescriptorCANframe)");    
    bool r {false};

    // i can proceed only if service is active .... 
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

bool embot::app::eth::theServiceMC::Impl::process(const DescriptorROP &ropdescriptor)
{
    p_traceprint("process(DescriptorROP)");
    
    bool r {false};
    
    // i can proceed only if service is active .... 
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

bool embot::app::eth::theServiceMC::process(const DescriptorROP &ropdescriptor)
{
    return pImpl->process(ropdescriptor);
}


// other objects




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


