
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#ifndef __EMBOT_APP_ETH_SERVICE_IMPL_MC_IF_H_
#define __EMBOT_APP_ETH_SERVICE_IMPL_MC_IF_H_


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_Service_impl.h"


    
namespace embot::app::eth::service::impl::mc {    


struct IFmcobj 
{
    virtual embot::app::eth::Service::Type type() const = 0;
    virtual bool clear() = 0;    
    virtual bool load(embot::app::eth::Service *serv, const eOmn_serv_configuration_t *sc) = 0;
    virtual size_t numberofjomos() const = 0;
    virtual bool verifyactivate(embot::app::eth::Service::OnEndOfOperation onend) = 0;
    virtual bool deactivate() = 0;
    
protected:
    virtual ~IFmcobj() {};    // cannot delete from interface but only from derived object 
};

struct mcOBJnone : public IFmcobj
{       
    mcOBJnone() = default;
        
    embot::app::eth::Service::Type type() const override
    { return embot::app::eth::Service::Type::none; }
    
    bool clear() override 
    { return false; }
    
    bool load(embot::app::eth::Service *serv, const eOmn_serv_configuration_t *sc) override
    { return false; }
    
    size_t numberofjomos() const override
    { return 0; }
    
    bool verifyactivate(embot::app::eth::Service::OnEndOfOperation onend) override
    { return false; }
    
    bool deactivate() override
    { return false; }
    
};


} // namespace embot::app::eth::service::impl::mc { 


#endif // #define __EMBOT_APP_ETH_SERVICE_IMPL_MC_IF_H_

 


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


