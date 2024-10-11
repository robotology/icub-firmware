
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_APPLICATION_CANAGENTMC2_H_
#define __EMBOT_APP_APPLICATION_CANAGENTMC2_H_

//#include <vector>
//#include <memory>

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_prot_can.h"
#include "embot_prot_can_bootloader.h"
#include "embot_prot_can_motor_periodic.h"
#include "embot_prot_can_motor_polling.h"
#include "embot_prot_can_analog_periodic.h"
#include "embot_prot_can_analog_polling.h"

namespace embot::app::application {
    
    
    class CANagentMC2
    {
    public:
        
        virtual bool get(const embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info &info) = 0;  
    
        virtual bool get(const embot::prot::can::motor::polling::Message_SET_CONTROL_MODE::Info &info) = 0; 
        virtual bool get(const embot::prot::can::motor::polling::Message_SET_CURRENT_LIMIT::Info &info) = 0;  
        virtual bool get(const embot::prot::can::motor::polling::Message_SET_CURRENT_PID::Info &info) = 0; 
        virtual bool get(const embot::prot::can::motor::polling::Message_SET_VELOCITY_PID::Info &info) = 0; 
        virtual bool get(const embot::prot::can::motor::polling::Message_SET_MOTOR_CONFIG::Info &info) = 0;  
        virtual bool get(const embot::prot::can::motor::polling::Message_SET_TEMPERATURE_LIMIT::Info &info) = 0; 
    
        virtual bool get(const embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::Info &info, embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::ReplyInfo &replyinfo) = 0;
        virtual bool get(const embot::prot::can::motor::polling::Message_GET_CURRENT_LIMIT::Info &info, embot::prot::can::motor::polling::Message_GET_CURRENT_LIMIT::ReplyInfo &replyinfo) = 0; 
        virtual bool get(const embot::prot::can::motor::polling::Message_GET_CURRENT_PID::Info &info, embot::prot::can::motor::polling::Message_GET_CURRENT_PID::ReplyInfo &replyinfo) = 0;    
        virtual bool get(const embot::prot::can::motor::polling::Message_GET_VELOCITY_PID::Info &info, embot::prot::can::motor::polling::Message_GET_VELOCITY_PID::ReplyInfo &replyinfo) = 0; 
        virtual bool get(const embot::prot::can::motor::polling::Message_GET_MOTOR_CONFIG::Info &info, embot::prot::can::motor::polling::Message_GET_MOTOR_CONFIG::ReplyInfo &replyinfo) = 0; 
        virtual bool get(const embot::prot::can::motor::polling::Message_GET_TEMPERATURE_LIMIT::Info &info, embot::prot::can::motor::polling::Message_GET_TEMPERATURE_LIMIT::ReplyInfo &replyinfo) = 0;
    
    public:
        virtual ~CANagentMC2() {}         
    };
    
    class dummyCANagentMC2 : public CANagentMC2 
    {
    public:
        
        dummyCANagentMC2() {}
        virtual ~dummyCANagentMC2() {}
            
        bool get(const embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info &info) override { return false; } 
        
        bool get(const embot::prot::can::motor::polling::Message_SET_CONTROL_MODE::Info &info) override { return false; }  
        bool get(const embot::prot::can::motor::polling::Message_SET_CURRENT_LIMIT::Info &info) override { return false; } 
        bool get(const embot::prot::can::motor::polling::Message_SET_CURRENT_PID::Info &info) override { return false; }          
        bool get(const embot::prot::can::motor::polling::Message_SET_VELOCITY_PID::Info &info) override { return false; }  
        bool get(const embot::prot::can::motor::polling::Message_SET_MOTOR_CONFIG::Info &info) override { return false; } 
        bool get(const embot::prot::can::motor::polling::Message_SET_TEMPERATURE_LIMIT::Info &info) override { return false; }  
        
        bool get(const embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::Info &info, embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::ReplyInfo &replyinfo) override { return false; } 
        bool get(const embot::prot::can::motor::polling::Message_GET_CURRENT_LIMIT::Info &info, embot::prot::can::motor::polling::Message_GET_CURRENT_LIMIT::ReplyInfo &replyinfo) override { return false; } 
        bool get(const embot::prot::can::motor::polling::Message_GET_CURRENT_PID::Info &info, embot::prot::can::motor::polling::Message_GET_CURRENT_PID::ReplyInfo &replyinfo) override { return false; }    
        bool get(const embot::prot::can::motor::polling::Message_GET_VELOCITY_PID::Info &info, embot::prot::can::motor::polling::Message_GET_VELOCITY_PID::ReplyInfo &replyinfo) override { return false; } 
        bool get(const embot::prot::can::motor::polling::Message_GET_MOTOR_CONFIG::Info &info, embot::prot::can::motor::polling::Message_GET_MOTOR_CONFIG::ReplyInfo &replyinfo) override { return false; } 
        bool get(const embot::prot::can::motor::polling::Message_GET_TEMPERATURE_LIMIT::Info &info, embot::prot::can::motor::polling::Message_GET_TEMPERATURE_LIMIT::ReplyInfo &replyinfo) override { return false; }
              
    }; 


} // namespace embot::app::application {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
