
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#ifndef __EMBOT_APP_ETH_THESERVICEMC_IMPL_H_
#define __EMBOT_APP_ETH_THESERVICEMC_IMPL_H_


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_Service_impl.h"

#include "embot_app_eth_Service_impl_mc_if.h"
#include "embot_app_eth_Service_impl_mc_FOC.h"
#include "embot_app_eth_Service_impl_mc_ADVFOC.h"

#include "embot_app_eth_Encoder.h"
#include "embot_app_eth_theServiceMC.h"



namespace embot::app::eth::service::impl::mc {    

// it offers services specific to the type of MC loaded, so far only: MC_foc, MC_advfoc
// it just enables the proper IFmcobj according to the loaded configuration
// then it assign the joints and motor pointers according to the configuration
// and implements the specific operations for verify, activate, deactivate etc.
struct MCobjects
{
    static constexpr size_t maxjomos {embot::app::eth::theServiceMC::maxJOMOs};
    
    // just to clarify what we can offer
    static constexpr bool supported(embot::app::eth::Service::Type t)
    {
        return (embot::app::eth::Service::Type::MC_advfoc == t) || (embot::app::eth::Service::Type::MC_foc == t);
    }     
        
    embot::app::eth::service::impl::Core *p2core {nullptr};
    
    std::array<eOmc_joint_t*, maxjomos> joints {nullptr};
    std::array<eOmc_motor_t*, maxjomos> motors {nullptr};

    // we hold three types of IFmcobj: none, foc, advfoc
    mcOBJnone mcnone {};    
    mcOBJfoc mcfoc {};
    mcOBJadvfoc mcadvfoc {};
    
    IFmcobj *themcobjectinuse {&mcnone};
    
    embot::app::eth::Service::Type mcservicetype {embot::app::eth::Service::Type::none};
        
    
    MCobjects() = default;
        
    void initialise(embot::app::eth::service::impl::Core *p);
    
    void clear();
    
    bool load(embot::app::eth::Service *serv, const eOmn_serv_configuration_t *sc);
    
    size_t numberofjomos() const;
    
    bool verifyactivate(embot::app::eth::Service::OnEndOfOperation onend);

    bool deactivate();
    
    bool stop();
    
    bool start();

    IFmcobj * getMCobj();    
    
       
    static bool supported(const eOmn_serv_configuration_t *sc);
        
};


// it performs operations specific to the MC service
struct MCservice
{
    embot::app::eth::service::impl::Core core {};
    MCobjects mcobjects {};
        
    MCservice() = default;
        
    bool initted() const;
    
    void initialise(size_t maxregs);

    Service::Type type() const;

    void clear();

    bool active() const;
    
    bool started() const;
    
    embot::app::eth::Service::State state() const;
    
    void state(embot::app::eth::Service::State s);
    
    void synch(embot::app::eth::Service::State s);
    
    void synch();
    
    size_t numberofjomos() const;
    
    eOmc_joint_t* joint(size_t n);

    eOmc_motor_t* motor(size_t n);
    
    bool load(embot::app::eth::Service *serv, const eOmn_serv_configuration_t *sc);

    bool verifyactivate(embot::app::eth::Service::OnEndOfOperation onend);
    
    bool deactivate();
 
    static bool supported(embot::app::eth::Service::Type t);
    
    bool stop();
    
    bool forcestopofverify();
    
    bool start();

    bool tick();
    
    bool process(const Service::DescriptorCANframe &canframedescriptor);   

    bool process(const Service::DescriptorROP &ropdescriptor);
    
private:


    bool p_encodersAcquire();
    bool p_encodersApplyValue(uint8_t index, embot::app::eth::encoder::v1::valueInfo *valueinfo);
    void p_JOMOupdatestatus();    
    bool p_applysetpoint(eOprotIndex_t jointindex, eOmc_setpoint_t *setpoint);
    static uint8_t p_getentityindex(eOprot_entity_t entity, const Service::DescriptorCANframe &canframedescriptor);
};

} // namespace embot::app::eth {  


#endif // #define __EMBOT_APP_ETH_THESERVICEMC_IMPL_H_

 


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


