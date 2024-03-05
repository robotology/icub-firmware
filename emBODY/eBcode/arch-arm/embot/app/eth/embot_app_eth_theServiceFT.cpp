
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

#include "embot_app_eth_Service_impl.h"

#include "embot_app_eth_theFTservice.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


namespace embot::app::eth {


//struct Operation
//{
//    const eOmn_serv_configuration_t * servcfg {nullptr};
//    embot::app::eth::Service::OnEndOfConfigOperation onendoperation {nullptr};   
//    embot::app::eth::Service *service {nullptr};
//    bool activateafterverify {false};
//    
//    Operation() = default;
//    
//    bool isvalid() { return (nullptr != servcfg) && (nullptr != onendoperation) && (nullptr != service); }
//    void load(embot::app::eth::Service *s, const eOmn_serv_configuration_t *c, embot::app::eth::Service::OnEndOfConfigOperation eop, bool act)
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
    
    embot::app::eth::service::impl::ServiceOperation2 _operation2 {};    
    
    // methods used by theServiceFT 
    
    Impl(theServiceFT *o) : _owner(o) {};      
    
    eOresult_t initialise();
        
    Category category() const { return embot::app::eth::Service::Category::ft; }
    Type type() const { return embot::app::eth::Service::Type::AS_ft; }
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
    bool process(const DescriptorROP &ropdescriptor);
    
//    static void cbk_afterverify(void* p, eObool_t operationisok)
//    {
//        // the embobj service executes a callback where we passed p as Impl * and operationisok contains success of operation
//        // remember that the callback is called after verification is OK or when verification is OK and activation is already done
//        // in any case, the callback should signal to YRI the result of the verify() operation     
//        Impl *impl = reinterpret_cast<Impl*>(p);
//        impl->_operation.onend(static_cast<bool>(operationisok));  
//    }  
    
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


bool embot::app::eth::theServiceFT::Impl::verifyactivate(const embot::app::eth::Service::Config * servcfg, OnEndOfOperation onendofverifyactivate)
{
    _operation2.load(_owner, servcfg, onendofverifyactivate);
    const eOmn_serv_configuration_t * eomnservcfg = reinterpret_cast<const eOmn_serv_configuration_t *>(servcfg->memory());
    embot::app::eth::theFTservice::getInstance().Verify(eomnservcfg, cbk_afterverifyactivate, this, eobool_true);   
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

bool embot::app::eth::theServiceFT::Impl::set(const eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t& numberofthem)
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
    embot::app::eth::theFTservice::canFrameDescriptor cfd 
    {   
        canframedescriptor.port, 
        canframedescriptor.frame,
        embot::app::eth::theFTservice::canFrameDescriptor::Type::autodetect
    }; 
    embot::app::eth::theFTservice::getInstance().AcceptCANframe(cfd);
    return true;
}

bool embot::app::eth::theServiceFT::Impl::process(const DescriptorROP &ropdescriptor)
{
    embot::app::eth::theFTservice::getInstance().process(ropdescriptor.rd, ropdescriptor.nv);
    return true;
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


embot::app::eth::Service::Type embot::app::eth::theServiceFT::type() const
{
    return pImpl->type();
}

embot::app::eth::Service::State embot::app::eth::theServiceFT::state() const
{
    return pImpl->state();
}

void embot::app::eth::theServiceFT::set(embot::app::eth::Service::State s)
{
    pImpl->set(s);
}


bool embot::app::eth::theServiceFT::verifyactivate(const embot::app::eth::Service::Config * servcfg, OnEndOfOperation onendofverifyactivate)
{
    return pImpl->verifyactivate(servcfg, onendofverifyactivate);
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

bool embot::app::eth::theServiceFT::setregulars(const eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t& numberofthem)
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

bool embot::app::eth::theServiceFT::process(const DescriptorFrame &framedescriptor)
{
    return false; //pImpl->process(canframedescriptor);
}

bool embot::app::eth::theServiceFT::process(const DescriptorROP &ropdescriptor)
{
    return pImpl->process(ropdescriptor);
}


// other objects




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


