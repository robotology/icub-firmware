
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_BOARD_AMCFOC_1CM7_THECANAGENTCORE_H_
#define __EMBOT_APP_BOARD_AMCFOC_1CM7_THECANAGENTCORE_H_

#include <vector>
#include <memory>

#include "embot_app_msg.h"
#include "embot_app_application_CANagentCORE.h"


namespace embot::app::board::amcfoc::cm7 {
       
    class theCANagentCORE : public embot::app::application::CANagentCORE
    {    
    public:
        static theCANagentCORE& getInstance();
    
        struct Config
        {
            embot::prot::can::Board board {embot::prot::can::Board::amc2c}; // but we can also use amcbldc
            embot::prot::can::applicationInfo applicationinfo {{0,0,1}, {2,0}};
            embot::app::msg::Location location {embot::app::msg::BUS::icc1, 3}; // but also {embot::app::msg::BUS::can2, 3}
            const char *boardinfo {"hello, i am an amc1cm7"};            
            constexpr Config() = default;
            constexpr Config(const embot::prot::can::Board brd, 
                             const embot::prot::can::applicationInfo& ai, 
                             const embot::app::msg::Location &loc,    
                             const char *bi) 
                : board(brd), applicationinfo(ai), location(loc), 
                  boardinfo(bi) {}
        };
                
        bool initialise(const Config &config);   
                
        // interface to theCANagentCORE
        const embot::prot::can::applicationInfo & applicationinfo() const override;   
        embot::hw::CAN bus() const override;        
        embot::prot::can::Address address() const override;       
                    
        bool get(const embot::prot::can::bootloader::Message_BROADCAST::Info &info, embot::prot::can::bootloader::Message_BROADCAST::ReplyInfo &replyinfo) override;
        bool get(const embot::prot::can::bootloader::Message_GET_ADDITIONAL_INFO::Info &info, embot::prot::can::bootloader::Message_GET_ADDITIONAL_INFO::ReplyInfo &replyinfo) override;   
        bool get(const embot::prot::can::shared::Message_GET_VERSION::Info &info, embot::prot::can::shared::Message_GET_VERSION::ReplyInfo &replyinfo) override;    
        bool get(const embot::prot::can::bootloader::Message_GET_TIMEOFLIFE::Info &info, embot::prot::can::bootloader::Message_GET_TIMEOFLIFE::ReplyInfo &replyinfo) override;
        
        bool set(const embot::prot::can::bootloader::Message_BOARD::Info &info) override;        
        bool set(const embot::prot::can::bootloader::Message_SET_ADDITIONAL_INFO2::Info &info) override; 
        bool set(const embot::prot::can::shared::Message_SET_ID::Info &info) override; 
        bool set(const embot::prot::can::bootloader::Message_SETCANADDRESS::Info &info) override;   
        
    private:
        theCANagentCORE(); 
        ~theCANagentCORE(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;      
    };

} // namespace embot::app::board::amcfoc::cm7 {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
