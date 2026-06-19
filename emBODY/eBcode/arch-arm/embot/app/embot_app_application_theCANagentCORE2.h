
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_APPLICATION_THECANAGENTCORE2_H_
#define __EMBOT_APP_APPLICATION_THECANAGENTCORE2_H_

#include <vector>
#include <memory>

#include "embot_app_msg.h"
#include "embot_app_application_CANagentCORE.h"



namespace embot::app::application {
       
    class theCANagentCORE2 : public embot::app::application::CANagentCORE
    {    
    public:
        static theCANagentCORE2& getInstance();
           
        struct Storage
        {
            enum class Mode : uint8_t { ram = 0 }; // later on also eeprom and flash
            Mode mode {Mode::ram};
            uint32_t address {0};
            Storage() = default;
        };
   
        struct Config
        {
            embot::prot::can::Board board {embot::prot::can::Board::generic}; 
            embot::prot::can::applicationInfo applicationinfo {{0,0,1}, {2,0}};
            embot::app::msg::Location location {embot::app::msg::BUS::icc1, 1}; // but also {embot::app::msg::BUS::can1, 1}
            const char *boardinfo {"hi, i am a generic board"};  
            Storage storage {};            
            constexpr Config() = default;
            constexpr Config(const embot::prot::can::Board brd, 
                             const embot::prot::can::applicationInfo& ai, 
                             const embot::app::msg::Location &loc,    
                             const char *bi,
                             const Storage &st) 
                : board(brd), applicationinfo(ai), location(loc), 
                  boardinfo(bi), storage(st) {}
        };
                
        bool initialise(const Config &config);   
                
        // interface to CANagentCORE
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
        theCANagentCORE2(); 
        ~theCANagentCORE2(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;      
    };

} // namespace embot::app::application {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
