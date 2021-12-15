
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_APP_APPLICATION_THEMBDAGENT_H_
#define _EMBOT_APP_APPLICATION_THEMBDAGENT_H_


#include "embot_app_application_theCANparserMBD.h"


#include <memory>

namespace embot { namespace app { namespace application {
           
    class theMBDagent : public CANagentMBD
    {
    public:
        static theMBDagent& getInstance();
    
        struct Config
        {
            uint32_t dummy {0};            
            constexpr Config() = default;
        };
                
        bool initialise(const Config &config);   
        bool tick(const std::vector<embot::prot::can::Frame> &inpframes, std::vector<embot::prot::can::Frame> &outframes);
                
        // interface to CANagentMBD
        virtual bool onrecognisedframe(void *p); 
        // legacy interface
        virtual bool get(const embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info &info);        
        virtual bool get(const embot::prot::can::motor::polling::Message_SET_CONTROL_MODE::Info &info);
        virtual bool get(const embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::Info &info, embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::ReplyInfo &replyinfo);
        
    private:
        theMBDagent(); 
        ~theMBDagent(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;    
    };       
    
}}} // namespace embot { namespace app { namespace application


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
