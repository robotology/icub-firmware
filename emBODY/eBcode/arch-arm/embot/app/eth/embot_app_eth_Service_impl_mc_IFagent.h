
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#ifndef __EMBOT_APP_ETH_SERVICE_IMPL_MC_IF_AGENT_H_
#define __EMBOT_APP_ETH_SERVICE_IMPL_MC_IF_AGENT_H_


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_Service_impl.h"


    
namespace embot::app::eth::service::impl::mc {    


struct IFagent 
{
    virtual embot::app::eth::Service::Type type() const = 0;
    virtual bool clear() = 0;  
    virtual bool load(embot::app::eth::Service *serv, const eOmn_serv_configuration_t *sc) = 0;
    virtual size_t numberofjomos() const = 0;
    // if andactivate is true at end of an OK verification it is internally called activate()
    // and finally is called onend(serv, true)
    // if activate is false at end of an OK verification it is called only onend(service, true)
    // and activate() must be called externally.
    // we use the mode andactivate = true    
    virtual bool verify(embot::app::eth::Service::OnEndOfOperation onend, bool andactivate = true) = 0;    
    virtual bool activate() = 0;
    virtual bool forceidle() = 0;
    virtual bool deactivate() = 0;
    virtual bool stop() = 0;
    virtual bool start() = 0;
    
protected:
    virtual ~IFagent() {};    // cannot delete from interface but only from derived object 
};

struct AGENTnone : public IFagent
{       
    AGENTnone() = default;
        
    embot::app::eth::Service::Type type() const override
    { return embot::app::eth::Service::Type::none; }
    
    bool clear() override 
    { return false; }
    
    bool load(embot::app::eth::Service *serv, const eOmn_serv_configuration_t *sc) override
    { return false; }
    
    size_t numberofjomos() const override
    { return 0; }
    
    bool verify(embot::app::eth::Service::OnEndOfOperation onend, bool andactivate = true) override
    { return false; }

    bool forceidle() override
    { return false; }
    
    bool activate() override
    { return false; }
    
    bool deactivate() override
    { return false; }
    
    bool stop() override
    { return false; }  

    bool start() override
    { return false; }    
    
};


} // namespace embot::app::eth::service::impl::mc { 


#endif // #define __EMBOT_APP_ETH_SERVICE_IMPL_MC_IF_H_

 


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


