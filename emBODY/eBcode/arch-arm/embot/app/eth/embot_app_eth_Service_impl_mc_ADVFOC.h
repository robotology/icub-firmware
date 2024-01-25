
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#ifndef __EMBOT_APP_ETH_SERVICE_IMPL_MC_ADVFOC_H_
#define __EMBOT_APP_ETH_SERVICE_IMPL_MC_ADVFOC_H_


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_Service_impl_mc_if.h"


    
namespace embot::app::eth::service::impl::mc {    


struct mcOBJadvfoc : public IFmcobj
{    
    
    mcOBJadvfoc() = default;
    
    size_t numofjomos {0};
    
#if 0

    verification is done in two steps:
    - 01: encoders
    - 02: presence of icc resources
    - 03: presence of can boards 
    
    
#endif
    

    embot::app::eth::Service::Type type() const override
    { 
        return embot::app::eth::Service::Type::MC_advfoc; 
    }
    
    bool clear() override
    {
        bool r {true};
        
        return r;
    }
    
    
    bool load(embot::app::eth::Service *serv, const eOmn_serv_configuration_t *sc) override
    {
        bool r {true};
                       
        return r;        
    }

    size_t numberofjomos() const override
    {
       return numofjomos; 
    }
    
    bool verifyactivate(embot::app::eth::Service::OnEndOfOperation onend) override
    {
        bool r {true};
        
//        void *owner {nullptr};
//        // i assign ... and bla bla bla.
//        // i call 
//        EOconstarray *carrayofjomodes {nullptr};
//        eOmn_serv_diagn_cfg_t dc = {0, 0};
//        const embot::core::Confirmer conf {step01_onENDof_verifyencoders, owner};
//        embot::app::eth::theEncoderReader::getInstance().Verify({carrayofjomodes, dc}, true, conf);
        
        return r;
        
    }
    
    bool deactivate() override
    {
        bool r {true};
        
        
        
                       
        return r;           
    }

}; 

} // namespace embot::app::eth::service::impl::mc { 


#endif // #define __EMBOT_APP_ETH_SERVICE_IMPL_MC_ADVFOC_H_

 


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


