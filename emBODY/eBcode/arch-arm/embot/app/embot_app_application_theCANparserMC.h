

/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_APP_APPLICATION_THECANPARSERMC_H_
#define _EMBOT_APP_APPLICATION_THECANPARSERMC_H_

#include "embot_core.h"
#include "embot_prot_can.h"
#include "embot_prot_can_motor_polling.h"
#include "embot_prot_can_motor_periodic.h"
#include <vector>
#include <memory>

namespace embot { namespace app { namespace application {
    
    class CANagentMC
    {
    public:
        // interface
        virtual bool get(const embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info &info) = 0;        
       
    public:
        virtual ~CANagentMC() {};         
    };
           
    class theCANparserMC
    {
    public:
        static theCANparserMC& getInstance();
        
        
    public:
        
        struct Config
        {
            CANagentMC* agent {nullptr};
            Config() = default;
            Config(CANagentMC* a) : agent(a) {}
            bool isvalid() const { return (nullptr == agent) ? false : true; }
        }; 
        
        
        bool initialise(const Config &config); 
        
        // returns true if the canframe has been recognised. if so, any reply is sent if replies.size() > 0
        bool process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);

    private:
        theCANparserMC(); 
        ~theCANparserMC(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;     
    };       


}}} // namespace embot { namespace app { namespace application


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
