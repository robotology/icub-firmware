
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#ifndef __EMBOT_APP_ETH_SERVICE_IMPL_MC_SERVICE_H_
#define __EMBOT_APP_ETH_SERVICE_IMPL_MC_SERVICE_H_


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_Service.h"
#include "embot_app_eth_Service_impl.h"
#include "embot_app_eth_Service_impl_mc_AGENTs.h"

#include "EoMotionControl.h"
#include "embot_app_eth_Encoder.h"

namespace embot::app::eth::service::impl::mc {

// it performs operations specific to the MC service

struct SERVICE
{       
    SERVICE() = default;
        
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
    bool process(const Service::DescriptorFrame &framedescriptor);
    bool process(const Service::DescriptorROP &ropdescriptor);
    
public:
    // maybe make them private
    embot::app::eth::service::impl::Core core {};
    embot::app::eth::service::impl::mc::AGENTs mcagents {};

private:
    
    bool p_encodersAcquire();
    bool p_encodersApplyValue(uint8_t index, embot::app::eth::encoder::v1::valueInfo *valueinfo);
    void p_JOMOupdatestatus();    
    bool p_applysetpoint(eOprotIndex_t jointindex, eOmc_setpoint_t *setpoint);
    static uint8_t p_getentityindex(eOprot_entity_t entity, const Service::DescriptorCANframe &canframedescriptor);
};

} // namespace embot::app::eth::service::impl::mc {  

#endif 

 


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


