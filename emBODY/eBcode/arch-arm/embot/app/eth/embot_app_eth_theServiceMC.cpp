
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

//#include "embot_app_eth_theFTservice.h"
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
    
    ServiceOperation _operation {};
    
    // methods used by theServiceMC 
    
    Impl(theServiceMC *o) : _owner(o) {};      
    
    eOresult_t initialise();
        
    Category category() const { return embot::app::eth::Service::Category::mc; }
    State state() const { return _state; }
    void set(State s) { _state = s; }
    
    bool verify(const eOmn_serv_configuration_t * servcfg, bool activateafterverify, fpOnEndOfOperation onendoperation);
    bool activate(const eOmn_serv_configuration_t * servcfg);
    bool deactivate();
    bool start();
    bool stop();
    bool set(eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t& numberofthem);
    bool tick(bool resetstatus);
    bool report();
    bool process(const DescriptorCANframe &canframedescriptor);
    bool process(const DescriptorROP &ropdescriptor);
    
    static eOresult_t cbk_afterverify(void* p, eObool_t operationisok);    

private:  
    

};

//embot::app::eth::theServiceMC::Impl::Impl()
//{
//    // we need it because ... we dont have default values for the C structs and better doing  
//}
      
eOresult_t embot::app::eth::theServiceMC::Impl::initialise()
{
    if(true == _initted)
    {
        return eores_OK;
    }
    
    embot::core::print("embot::app::eth::theServiceMC::Impl::initialise()");

//#warning TODO: add embot::app::eth::theMCservice::getInstance().initialise({});    
//    embot::app::eth::theFTservice::getInstance().initialise({}); 
    _eom = eo_motioncontrol_Initialise();
    
    _initted = true;
    return eores_OK;
}


bool embot::app::eth::theServiceMC::Impl::verify(const eOmn_serv_configuration_t * servcfg, bool activateafterverify, fpOnEndOfOperation onendoperation)
{
    _operation.load(_owner, servcfg, onendoperation, activateafterverify);
    //embot::app::eth::theFTservice::getInstance().Verify(servcfg, cbk_afterverify, this, activateafterverify ? eobool_true : eobool_false);
    eo_motioncontrol_Verify(_eom, servcfg, cbk_afterverify, eobool_true);    
    return true;
}

bool embot::app::eth::theServiceMC::Impl::activate(const eOmn_serv_configuration_t * servcfg)
{
//    #warning TODO: add embot::app::eth::theMCservice::getInstance().Activate(servcfg);
    eo_motioncontrol_Activate(_eom, servcfg);
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

bool embot::app::eth::theServiceMC::Impl::set(eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t& numberofthem)
{
    //#warning TODO: add  embot::app::eth::theMCservice::getInstance().SetRegulars(arrayofid32, &numberofthem);
    eo_motioncontrol_SetRegulars(_eom, arrayofid32, &numberofthem);
    #warning possibile baco ... set(regulars) potrebbe non funzionare come dovrebbe quando abbiamo due servizi
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
    //#warning BUT later on u will use the third parameter as well ...
    //embot::app::eth::theFTservice::canFrameDescriptor cfd {canframedescriptor.port, canframedescriptor.frame}; 
    //embot::app::eth::theFTservice::getInstance().AcceptCANframe(cfd);
    //#warning TODO: add  embot::app::eth::theMCservice::getInstance().AcceptCANframe()
    eo_motioncontrol_AcceptCANframe(_eom, canframedescriptor.frame, canframedescriptor.port, eo_motcon_canframe_unknown);
    return true;
}

bool embot::app::eth::theServiceMC::Impl::process(const DescriptorROP &ropdescriptor)
{
    #warning TODO: add  embot::app::eth::theMCservice::getInstance().process(ropdescriptor.rd, ropdescriptor.nv);
    return true;
}

// - static

eOresult_t embot::app::eth::theServiceMC::Impl::cbk_afterverify(void* p, eObool_t operationisok)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    impl->_operation.onend();
    
    return eores_OK;
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
        

eOresult_t embot::app::eth::theServiceMC::initialise(const Config &config)
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

embot::app::eth::Service::State embot::app::eth::theServiceMC::state() const
{
    return pImpl->state();
}

void embot::app::eth::theServiceMC::set(embot::app::eth::Service::State s)
{
    pImpl->set(s);
}

bool embot::app::eth::theServiceMC::verify(const eOmn_serv_configuration_t * servcfg, bool activateafterverify, fpOnEndOfOperation onendoperation)
{
    return pImpl->verify(servcfg, activateafterverify, onendoperation);
}

bool embot::app::eth::theServiceMC::activate(const eOmn_serv_configuration_t * servcfg)
{
    return pImpl->activate(servcfg);
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

bool embot::app::eth::theServiceMC::setregulars(eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t& numberofthem)
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

bool embot::app::eth::theServiceMC::process(const DescriptorROP &ropdescriptor)
{
    return pImpl->process(ropdescriptor);
}


// other objects




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


