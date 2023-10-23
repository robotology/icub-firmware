
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theServiceFT.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theFTservice.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


namespace embot::app::eth {


//struct Operation
//{
//    const eOmn_serv_configuration_t * servcfg {nullptr};
//    embot::app::eth::Service::fpOnEndOfOperation onendoperation {nullptr};   
//    embot::app::eth::Service *service {nullptr};
//    bool activateafterverify {false};
//    
//    Operation() = default;
//    
//    bool isvalid() { return (nullptr != servcfg) && (nullptr != onendoperation) && (nullptr != service); }
//    void load(embot::app::eth::Service *s, const eOmn_serv_configuration_t *c, embot::app::eth::Service::fpOnEndOfOperation eop, bool act)
//    {
//        service = s;
//        servcfg = c;
//        onendoperation = eop;
//        activateafterverify = act;
//    }
//    
//    void onend()
//    {
//        if(isvalid())
//        {
//           onendoperation(service, servcfg, activateafterverify);
//        }
//    }
//};

// finally the implementation struct
struct embot::app::eth::theServiceFT::Impl
{
    Config _config {};
    bool _initted {false};   
    theServiceFT *_owner {nullptr};
    State _state {State::idle};
    
    ServiceOperation _operation {};
    
    // methods used by theServiceFT 
    
    Impl(theServiceFT *o) : _owner(o) {};      
    
    eOresult_t initialise();
        
    Category category() const { return embot::app::eth::Service::Category::ft; }
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

//embot::app::eth::theServiceFT::Impl::Impl()
//{
//    // we need it because ... we dont have default values for the C structs and better doing  
//}
      
eOresult_t embot::app::eth::theServiceFT::Impl::initialise()
{
    if(true == _initted)
    {
        return eores_OK;
    }
    
    embot::core::print("embot::app::eth::theServiceFT::Impl::initialise()");
    
    embot::app::eth::theFTservice::getInstance().initialise({}); 
    
    _initted = true;
    return eores_OK;
}


bool embot::app::eth::theServiceFT::Impl::verify(const eOmn_serv_configuration_t * servcfg, bool activateafterverify, fpOnEndOfOperation onendoperation)
{
    _operation.load(_owner, servcfg, onendoperation, activateafterverify);
    embot::app::eth::theFTservice::getInstance().Verify(servcfg, cbk_afterverify, this, activateafterverify ? eobool_true : eobool_false);
    return true;
}

bool embot::app::eth::theServiceFT::Impl::activate(const eOmn_serv_configuration_t * servcfg)
{
    embot::app::eth::theFTservice::getInstance().Activate(servcfg);
    return true;
}

bool embot::app::eth::theServiceFT::Impl::deactivate()
{
    embot::app::eth::theFTservice::getInstance().Deactivate();
    return true;
}

bool embot::app::eth::theServiceFT::Impl::start()
{
    embot::app::eth::theFTservice::getInstance().Start();
    return true;
}

bool embot::app::eth::theServiceFT::Impl::stop()
{
    embot::app::eth::theFTservice::getInstance().Stop();
    return true;
}

bool embot::app::eth::theServiceFT::Impl::set(eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t& numberofthem)
{
    embot::app::eth::theFTservice::getInstance().SetRegulars(arrayofid32, &numberofthem);
    return true;  
}

bool embot::app::eth::theServiceFT::Impl::tick(bool resetstatus)
{
    embot::app::eth::theFTservice::getInstance().Tick();
    return true;
}

bool embot::app::eth::theServiceFT::Impl::report()
{
    embot::app::eth::theFTservice::getInstance().SendReport();
    return true;
}

bool embot::app::eth::theServiceFT::Impl::process(const DescriptorCANframe &canframedescriptor)
{
    // #warning BUT later on u will use the third parameter as well ...
    embot::app::eth::theFTservice::canFrameDescriptor cfd {canframedescriptor.port, canframedescriptor.frame}; 
    embot::app::eth::theFTservice::getInstance().AcceptCANframe(cfd);
    return true;
}

bool embot::app::eth::theServiceFT::Impl::process(const DescriptorROP &ropdescriptor)
{
    embot::app::eth::theFTservice::getInstance().process(ropdescriptor.rd, ropdescriptor.nv);
    return true;
}

// - static

eOresult_t embot::app::eth::theServiceFT::Impl::cbk_afterverify(void* p, eObool_t operationisok)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    impl->_operation.onend();
    
    return eores_OK;
}


} // namespace embot::app::eth {

// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::eth::theServiceFT& embot::app::eth::theServiceFT::getInstance()
{
    static theServiceFT* p = new theServiceFT();
    return *p;
}

embot::app::eth::theServiceFT::theServiceFT()
{
    pImpl = std::make_unique<Impl>(this);
}  

    
embot::app::eth::theServiceFT::~theServiceFT() { }
        

eOresult_t embot::app::eth::theServiceFT::initialise(const Config &config)
{
    pImpl->_config = config;
    return pImpl->initialise();
}

embot::app::eth::Service* embot::app::eth::theServiceFT::service()
{
    return this;
}

embot::app::eth::Service::Category embot::app::eth::theServiceFT::category() const 
{
    return pImpl->category();
}

embot::app::eth::Service::State embot::app::eth::theServiceFT::state() const
{
    return pImpl->state();
}

void embot::app::eth::theServiceFT::set(embot::app::eth::Service::State s)
{
    pImpl->set(s);
}

bool embot::app::eth::theServiceFT::verify(const eOmn_serv_configuration_t * servcfg, bool activateafterverify, fpOnEndOfOperation onendoperation)
{
    return pImpl->verify(servcfg, activateafterverify, onendoperation);
}

bool embot::app::eth::theServiceFT::activate(const eOmn_serv_configuration_t * servcfg)
{
    return pImpl->activate(servcfg);
}

bool embot::app::eth::theServiceFT::deactivate()
{
    return pImpl->deactivate();
}

bool embot::app::eth::theServiceFT::start()
{
    return pImpl->start();
}

bool embot::app::eth::theServiceFT::stop()
{
    return pImpl->stop();
}

bool embot::app::eth::theServiceFT::setregulars(eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t& numberofthem)
{
    return pImpl->set(arrayofid32, numberofthem);
}

bool embot::app::eth::theServiceFT::tick(bool resetstatus)
{
    return pImpl->tick(resetstatus);;
}

bool embot::app::eth::theServiceFT::report()
{
    return pImpl->report();
}

bool embot::app::eth::theServiceFT::process(const DescriptorCANframe &canframedescriptor)
{
    return pImpl->process(canframedescriptor);
}

bool embot::app::eth::theServiceFT::process(const DescriptorROP &ropdescriptor)
{
    return pImpl->process(ropdescriptor);
}


// other objects




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


