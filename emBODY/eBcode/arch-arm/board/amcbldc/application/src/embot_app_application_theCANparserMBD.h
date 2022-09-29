

/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_APP_APPLICATION_THECANPARSERMBD_H_
#define _EMBOT_APP_APPLICATION_THECANPARSERMBD_H_

#include "embot_core.h"
#include "embot_prot_can.h"
#include "embot_prot_can_motor_polling.h"
#include "embot_prot_can_motor_periodic.h"
#include <vector>
#include <memory>

namespace embot { namespace app { namespace application {
    
    class CANagentMBD
    {
    public:
        
        // this one is called by the parser every time a frame is recognised
        virtual bool onrecognisedframe(void *p) = 0; 
        // legacy interface: so far the CANagentMBD keeps actions for these three can frames. just for debug 
        virtual bool get(const embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info &info) = 0;  
        virtual bool get(const embot::prot::can::motor::polling::Message_SET_CONTROL_MODE::Info &info) = 0;      
        virtual bool get(const embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::Info &info, embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::ReplyInfo &replyinfo) = 0;
       
    public:
        virtual ~CANagentMBD() {};         
    };
           
    class theCANparserMBD
    {
    public:
        static theCANparserMBD& getInstance();
        
        
    public:
        
        struct Config
        {
            CANagentMBD* agent {nullptr};
            Config() = default;
            Config(CANagentMBD* a) : agent(a) {}
            bool isvalid() const { return (nullptr == agent) ? false : true; }
        }; 
        
        
        bool initialise(const Config &config); 
        
        // returns true if the canframe has been recognised. if so, any reply is sent if replies.size() > 0
        bool process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);

    private:
        theCANparserMBD(); 
        ~theCANparserMBD(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;     
    };       


}}} // namespace embot { namespace app { namespace application


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
