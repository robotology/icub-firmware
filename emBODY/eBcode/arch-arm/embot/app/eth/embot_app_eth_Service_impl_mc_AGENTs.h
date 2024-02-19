
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#ifndef __EMBOT_APP_ETH_SERVICE_IMPL_MC_AGENTS_H_
#define __EMBOT_APP_ETH_SERVICE_IMPL_MC_AGENTS_H_


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theServiceMC.h"

#include "embot_app_eth_Service_impl.h"
#include "embot_app_eth_Service_impl_mc_AGENTfoc.h"
#include "embot_app_eth_Service_impl_mc_AGENTadvfoc.h"



namespace embot::app::eth::service::impl::mc {    

// it offers services specific to the type of MC loaded, so far only: MC_foc, MC_advfoc
// it just enables the proper IFagent according to the loaded configuration
// then it assigns the joints and motor pointers according to the configuration
// and implements the specific operations for verifyactivate(), deactivate() etc.
    
struct AGENTs
{
    static constexpr size_t maxjomos {embot::app::eth::theServiceMC::maxJOMOs};
    
    static constexpr bool supported(embot::app::eth::Service::Type t)
    {
        return (embot::app::eth::Service::Type::MC_foc == t) || (embot::app::eth::Service::Type::MC_advfoc == t);
    } 
    
    
    AGENTs() = default;
        
    void initialise(embot::app::eth::service::impl::Core *p);
    
    void clear();
    
    bool load(embot::app::eth::Service *serv, const eOmn_serv_configuration_t *sc);
    
    size_t numberofjomos() const;
    
    bool verifyactivate(embot::app::eth::Service::OnEndOfOperation onend);
    
    bool forceidle();

    bool deactivate();
    
    bool stop();
    
    bool start();

    IFagent * activeagent();    
    
    eOmc_joint_t * joint(size_t n) const;
    eOmc_motor_t * motor(size_t n) const;
    
    embot::app::eth::Service::Type type() const;
    
    static bool supported(const eOmn_serv_configuration_t *sc);    
        
private:
         
    // we need a pointer to the core service configuration
    embot::app::eth::service::impl::Core *_p2core {nullptr};     

    // we hold in here the pointers to the protocol joints and motors
    std::array<eOmc_joint_t*, maxjomos> _joints {nullptr};
    std::array<eOmc_motor_t*, maxjomos> _motors {nullptr};
   

    // we hold at most three types of IFagent: none, foc, advfoc
    AGENTnone _mcnone {};    
    AGENTfoc _mcfoc {};
    AGENTadvfoc _mcadvfoc {};   
    // and we have in here the active agent         
    IFagent *_theactiveagent {&_mcnone};   
    // and its service type
    embot::app::eth::Service::Type _mcservicetype {embot::app::eth::Service::Type::none};    
};

} // namespace embot::app::eth::service::impl::mc { 

#endif // 



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


