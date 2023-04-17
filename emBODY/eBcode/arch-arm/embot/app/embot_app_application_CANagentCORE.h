
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_APPLICATION_CANAGENTCORE_H_
#define __EMBOT_APP_APPLICATION_CANAGENTCORE_H_

#include <vector>
#include <memory>

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_prot_can.h"
#include "embot_prot_can_bootloader.h"
#include "embot_prot_can_motor_periodic.h"
#include "embot_prot_can_motor_polling.h"
#include "embot_prot_can_analog_periodic.h"
#include "embot_prot_can_analog_polling.h"

namespace embot { namespace app { namespace application {
    
    
    class CANagentCORE
    {
    public:
        
        virtual const embot::prot::can::applicationInfo & applicationinfo() const = 0;        
        virtual embot::hw::CAN bus() const = 0;
        virtual embot::prot::can::Address address() const = 0;
        
        // interface: returns true is we need to tx a reply else false
        virtual bool get(const embot::prot::can::bootloader::Message_BROADCAST::Info &info, embot::prot::can::bootloader::Message_BROADCAST::ReplyInfo &replyinfo) = 0;
        virtual bool get(const embot::prot::can::bootloader::Message_GET_ADDITIONAL_INFO::Info &info, embot::prot::can::bootloader::Message_GET_ADDITIONAL_INFO::ReplyInfo &replyinfo) = 0;   
        virtual bool get(const embot::prot::can::shared::Message_GET_VERSION::Info &info, embot::prot::can::shared::Message_GET_VERSION::ReplyInfo &replyinfo) = 0;    
        virtual bool get(const embot::prot::can::bootloader::Message_GET_TIMEOFLIFE::Info &info, embot::prot::can::bootloader::Message_GET_TIMEOFLIFE::ReplyInfo &replyinfo) = 0;
        
        virtual bool set(const embot::prot::can::bootloader::Message_BOARD::Info &info) = 0;        
        virtual bool set(const embot::prot::can::bootloader::Message_SET_ADDITIONAL_INFO2::Info &info) = 0; 
        // for motor::polling::CMD::SET_BOARD_ID and  analog::polling::CMD::SET_BOARD_ADX    
        virtual bool set(const embot::prot::can::shared::Message_SET_ID::Info &info) = 0;
        // for the bootloader more powerful version    
        virtual bool set(const embot::prot::can::bootloader::Message_SETCANADDRESS::Info &info) = 0;   
        
    public:
        virtual ~CANagentCORE() {}         
    };
    
    class dummyCANagentCORE : public CANagentCORE 
    {
    public:
        
        dummyCANagentCORE() {}
        virtual ~dummyCANagentCORE() {}
            
        static constexpr embot::prot::can::applicationInfo ai {{0, 0, 1}, {0, 1}};
               
        const embot::prot::can::applicationInfo & applicationinfo() const override { return ai; }   
        embot::hw::CAN bus() const override { return embot::hw::CAN::one; }        
        embot::prot::can::Address address() const override { return 1; }
        
        bool get(const embot::prot::can::bootloader::Message_BROADCAST::Info &info, embot::prot::can::bootloader::Message_BROADCAST::ReplyInfo &replyinfo) override { return true; }
        bool get(const embot::prot::can::bootloader::Message_GET_ADDITIONAL_INFO::Info &info, embot::prot::can::bootloader::Message_GET_ADDITIONAL_INFO::ReplyInfo &replyinfo) override { return true; }   
        bool get(const embot::prot::can::shared::Message_GET_VERSION::Info &info, embot::prot::can::shared::Message_GET_VERSION::ReplyInfo &replyinfo) override { return true; }    
        bool get(const embot::prot::can::bootloader::Message_GET_TIMEOFLIFE::Info &info, embot::prot::can::bootloader::Message_GET_TIMEOFLIFE::ReplyInfo &replyinfo) override { return true; }
        
        bool set(const embot::prot::can::bootloader::Message_BOARD::Info &info) override { return false; }        
        bool set(const embot::prot::can::bootloader::Message_SET_ADDITIONAL_INFO2::Info &info) override { return false; } 
        bool set(const embot::prot::can::shared::Message_SET_ID::Info &info) override { return false; } 
        bool set(const embot::prot::can::bootloader::Message_SETCANADDRESS::Info &info) override { return false; }   
    }; 


}}} // namespace embot { namespace app { namespace application


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
