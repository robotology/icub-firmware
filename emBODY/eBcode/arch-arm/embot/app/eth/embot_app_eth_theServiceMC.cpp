
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
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


//#error this implemenation is now obsolete, use embot_app_eth_theServiceMC2.cpp

#if !defined(USE_EOtheMCamc)
    #error embot_app_eth_theServiceMC.cpp needs USE_EOtheMCamc. consider using embot_app_eth_theServiceMC2.cpp
#endif

#include "embot_app_eth_Service_impl.h"

#include "EOtheMotionController.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


namespace embot::app::eth {



// finally the implementation struct
struct embot::app::eth::theServiceMC::Impl
{
    Config _config {};
    bool _initted {false};   
    theServiceMC *_owner {nullptr};
    State _state {State::idle};
    
    EOtheMotionController *_eom {nullptr};
    
    embot::app::eth::service::impl::ServiceOperation2 _operation2 {};
        
    // methods used by theServiceMC 
    
    Impl(theServiceMC *o) : _owner(o) {};      
    
    eOresult_t initialise();
        
    Category category() const { return embot::app::eth::Service::Category::mc; }
    Type type() const;
    State state() const { return _state; }
    void set(State s) { _state = s; }
    
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
        
    static void cbk_afterverifyactivate(void* p, eObool_t operationisok)
    {
        // the embobj service executes a callback where we passed p as Impl * and operationisok contains success of operation
        // remember that the callback is called after verification is OK or when verification is OK and activation is already done
        // in any case, the callback should signal to YRI the result of the verify() operation     
        Impl *impl = reinterpret_cast<Impl*>(p);
        impl->_operation2.onend(static_cast<bool>(operationisok));  
    }      

private:  
    

};

embot::app::eth::Service::Type embot::app::eth::theServiceMC::Impl::type() const
{
    eOmotioncontroller_mode_t m = eo_motioncontrol_GetMode(_eom);
    return (eo_motcon_mode_foc == m) ? embot::app::eth::Service::Type::MC_foc : embot::app::eth::Service::Type::none;
}
      
eOresult_t embot::app::eth::theServiceMC::Impl::initialise()
{
    if(true == _initted)
    {
        return eores_OK;
    }
    
    embot::core::print("embot::app::eth::theServiceMC::Impl::initialise()");

    _eom = eo_motioncontrol_Initialise();
    
    _initted = true;
    return eores_OK;
}

bool embot::app::eth::theServiceMC::Impl::verifyactivate(const embot::app::eth::Service::Config * servcfg, OnEndOfOperation onendofverifyactivate)
{
    _operation2.load(_owner, servcfg, onendofverifyactivate);
    const eOmn_serv_configuration_t * eomnservcfg = reinterpret_cast<const eOmn_serv_configuration_t *>(servcfg->memory());    
    eo_motioncontrol_Verify2(_eom, eomnservcfg, cbk_afterverifyactivate, this, eobool_true);    
    return true;
}


bool embot::app::eth::theServiceMC::Impl::deactivate()
{
    //#warning TODO: add embot::app::eth::theMCservice::getInstance().Deactivate();
    eo_motioncontrol_Deactivate(_eom);
    return true;
}

bool embot::app::eth::theServiceMC::Impl::start()
{
    //#warning TODO: add embot::app::eth::theMCservice::getInstance().Start();
    eo_motioncontrol_Start(_eom);
    return true;
}

bool embot::app::eth::theServiceMC::Impl::stop()
{
    //#warning TODO: add  embot::app::eth::theMCservice::getInstance().Stop();
    eo_motioncontrol_Stop(_eom);
    return true;
}

bool embot::app::eth::theServiceMC::Impl::set(const eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t& numberofthem)
{
    if(eores_NOK_generic == eo_motioncontrol_SetRegulars(_eom, const_cast<eOmn_serv_arrayof_id32_t*>(arrayofid32), &numberofthem)) {
        embot::core::print("[WARNING] eores_NOK_generic returned  during eo_motioncontrol_SetRegulars");
    }
    return true;  
}

bool embot::app::eth::theServiceMC::Impl::tick(bool resetstatus)
{
    //#warning TODO: add  embot::app::eth::theMCservice::getInstance().Tick();
    eo_motioncontrol_Tick(_eom);
    return true;
}

bool embot::app::eth::theServiceMC::Impl::report()
{
    //#warning TODO: add  embot::app::eth::theMCservice::getInstance().SendReport();
    eo_motioncontrol_SendReport(_eom);
    return true;
}

bool embot::app::eth::theServiceMC::Impl::process(const DescriptorCANframe &canframedescriptor)
{
    eo_motioncontrol_AcceptCANframe(_eom, canframedescriptor.frame, canframedescriptor.port, eo_motcon_canframe_unknown);
    return true;
}

bool embot::app::eth::theServiceMC::Impl::process(const DescriptorFrame &framedescriptor)
{   
    bool r {false};   
    
    return r;
}

bool embot::app::eth::theServiceMC::Impl::process(const DescriptorROP &ropdescriptor)
{
    bool r {false};   
    
    return r;
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
    return pImpl->tick(resetstatus);;
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


