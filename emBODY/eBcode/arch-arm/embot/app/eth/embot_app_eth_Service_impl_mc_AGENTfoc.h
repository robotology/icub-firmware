
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#ifndef __EMBOT_APP_ETH_SERVICE_IMPL_MC_AGENTFOC_H_
#define __EMBOT_APP_ETH_SERVICE_IMPL_MC_AGENTFOC_H_


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_Service_impl_mc_IFagent.h"


#include "embot_app_eth_theErrorManager.h"
#include "EOerror.h"
    
namespace embot::app::eth::service::impl::mc {    

struct AGENTfoc : public IFagent
{ 
    static const size_t maxjomos {4};
           
    AGENTfoc() = default;
        
    void initialise(embot::app::eth::service::impl::Core *c);

    embot::app::eth::Service::Type type() const override;    
    bool clear() override;    
    bool load(embot::app::eth::Service *serv, const eOmn_serv_configuration_t *sc) override;    
    size_t numberofjomos() const override;    
    bool verify(embot::app::eth::Service::OnEndOfOperation onend, bool andactivate = true) override;    
    bool activate() override;
    bool deactivate() override;
    bool stop() override;
    bool start() override;  
    
protected:  

    // p2core points to an external object loaded w/ initialise()
    // it is reponsibility of the AGENTfoc object to update its state etc
    embot::app::eth::service::impl::Core *p2core {nullptr};
    
    // servconfig, foc and jomodescriptors all points to memory
    // stored in p2core->serviceconfig
    const eOmn_serv_configuration_t *servconfig {nullptr}; 
    const eOmn_serv_config_data_mc_foc_t *foc {nullptr};        
    EOconstarray* jomodescriptors {nullptr};                
    
    // internally stored w/ load() 
    embot::app::eth::Service *service {nullptr};
    // internally stored w/ verify(). it is executed after activate() if triggeractivate is true
    // else after verify()
    embot::app::eth::Service::OnEndOfOperation afterverifyactivate {nullptr};
    bool triggeractivate {true};
    // computed from the loaded servconfig
    size_t numofjomos {0};   
    // service objects    
    embot::app::eth::service::impl::CANtools cantools {};        
    embot::app::eth::theErrorManager::Descriptor desc {};
        
        
    // it emits diagnostics messages        
    void emit(embot::app::eth::theErrorManager::Severity s, eOerror_code_t errorcode);
        
    // it synch the state of the embot::app::eth::service::impl::Core and of the MC service to the new state   
    void synch(embot::app::eth::Service::State s);

#if 0
    verification is done in two steps:
    - 01: encoders -> it triggers verify_step01_onENDof_verifyencoders()
    - 02: presence of can boards (foc or amcbldc) -> it triggers verify_step02_onENDof_candiscovery() 
        if both steps are ok then p2core->state gets ::verified, and if triggeractivate is true then it is executed activate()        
#endif
        
    
    // static methods used for callbacks: void *tHIS must be a pointer to this AGENTfoc struct
    static void verify_step01_onENDof_verifyencoders(void *tHIS, bool operationisok);
    static eOresult_t verify_step02_onENDof_candiscovery(void *tHIS, EOtheCANdiscovery2* cd2, eObool_t searchisok);
}; 



} // namespace embot::app::eth::service::impl::mc { 


#endif 

 


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


