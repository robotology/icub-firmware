

/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THEICCSERVICE_H_
#define __EMBOT_APP_ETH_THEICCSERVICE_H_

#include "embot_core.h"
#include "embot_app_eth.h"
#include "embot_os_rtos.h"

#include "embot_app_eth_mc_messaging.h"
#include "embot_hw_icc_ltr.h"

namespace embot::app::eth {
      
#if 0
    The singleton `theICCservice` is responsible to ... 
    
#endif
    
        
    class theICCservice
    {
    public:
        static theICCservice& getInstance();
    

    
        enum class ItemType : uint8_t { RAW = 0, CANframe = 1, ROP = 2 };
          
        struct Item
        {
            ItemType type {ItemType::RAW};
            uint8_t filler[3] = {0};
            uint8_t data[20] = {0};
            constexpr Item() = default;
        }; 
        static_assert(24 == sizeof(Item), "sizeof(Item) is not 24");      

        struct ItemCANframe : public Item
        {
            embot::app::msg::Location des {};
            embot::prot::can::Frame frame {};
                
            constexpr ItemCANframe() = default;
                
            constexpr ItemCANframe(const embot::app::msg::Location &d, const embot::prot::can::Frame &f) : des(d), frame(f)
            {
                type = ItemType::CANframe;
                std::memset(filler, 0, sizeof(filler));
                std::memmove(&data[0], &des, sizeof(des));
                std::memmove(&data[4], &frame, sizeof(frame));
            } 
            
            constexpr ItemCANframe(const Item &i)
            {
                if(ItemType::CANframe == i.type)
                {
                    type = ItemType::CANframe;
                    std::memmove(filler, i.filler, sizeof(filler));
                    std::memmove(data, i.data, sizeof(data));
                    // and now des and frame
                    std::memmove(&des, &data[0], sizeof(des));
                    std::memmove(&frame, &data[4], sizeof(frame));  
                }                    
            } 
                        
            Item item() const
            {
                Item i {};
                i.type = type;
                std::memmove(i.filler, filler, sizeof(filler));
                std::memmove(i.data, data, sizeof(data));
                return i;                    
            }

        };        


        struct ItemROP : public Item
        {
            enum class CMD : uint8_t { ask = 0, say = 1, set = 2, sig = 3, ack = 4, nak = 5 };
            using ID = uint8_t;
            static constexpr ID IDnone {0};
            static constexpr ID IDinfo {1};
            static constexpr ID IDunique64 {2};
            static constexpr ID IDval1 {11};
            static constexpr ID IDval2 {12};
            
            CMD cmd {CMD::ask};         // also inside filler[0]
            ID id {IDinfo};             // also inside filler[1]
            uint8_t size {0};           // also inside filler[2]
            void *dat {nullptr};        // points to &data[0]
                            
            constexpr ItemROP() = default;
                
            constexpr ItemROP(CMD c, ID i, uint8_t s, void *d) : cmd(c), id(i), size(s), dat(d)
            {
                type = ItemType::ROP;
                
                size = std::max(s, static_cast<uint8_t>(sizeof(data)));
                
                filler[0] = embot::core::tointegral(cmd);
                filler[1] = id;
                filler[2] = size;
                
                std::memmove(&data[0], dat, size);
            } 
            
            constexpr ItemROP(const Item &i)
            {
                if(ItemType::ROP == i.type)
                {
                    type = ItemType::ROP;
                    std::memmove(filler, i.filler, sizeof(filler));
                    std::memmove(data, i.data, sizeof(data));
                    // and now the shortcuts
                    cmd = static_cast<CMD>(filler[0]);
                    id = filler[1];
                    size = filler[2];
                    dat = &data[0];
                }                    
            } 
            
            Item item() const
            {
                Item i {};
                i.type = type;
                std::memmove(i.filler, filler, sizeof(filler));
                std::memmove(i.data, data, sizeof(data));
                return i;                    
            }

        };           
        
              
        using ItemParser = bool (*)(const Item &item);
        
        enum class modeTX : uint8_t { instant = 0, onflush = 1 };
         
        enum Mode : uint8_t { master = 0, slave = 1 };
        
        // TOBEDONE
        // the master calls init(). if the slave init() it stops forever until
        // the first has finished. when done it sends via icc::LTR a done message.
        // the first gets the message and marks the handshaken state. 
        // the public API, apart set(), can be called only when handhsaken is true.         
        
        struct Config
        {
            static constexpr size_t maxcapacity {40}; 
            // we can manage 48 Items in each direction by using two icc::LTR of 1024 bytes
            // the check is done inside initialise()            
            
            Mode mode {Mode::master};
            embot::hw::icc::LTR rxltr {embot::hw::icc::LTR::one};
            embot::hw::icc::LTR txltr {embot::hw::icc::LTR::two};
            size_t rxcapacity {32}; 
            size_t txcapacity {32};
            modeTX modetx {modeTX::instant};
            embot::os::Priority rxpriority {embot::os::Priority::system54};
            embot::os::Priority txpriority {embot::os::Priority::system53};
            embot::core::Callback onrx {};
            ItemParser itemparser {nullptr};
            
            constexpr Config() = default;
            
            constexpr Config(   Mode mo, embot::hw::icc::LTR rl, embot::hw::icc::LTR tl,
                                size_t r, size_t t, modeTX mt, embot::os::Priority rp, embot::os::Priority tp, const embot::core::Callback &orx, ItemParser ip)
                            :   mode (mo), rxltr(rl), txltr(tl),
                                rxcapacity(std::min(r, maxcapacity)), txcapacity(std::min(t, maxcapacity)),
                                modetx(mt), 
                                rxpriority(rp), txpriority(tp), 
                                onrx(orx),
                                itemparser(ip)
                            {}
                                
            bool isvalid() const { return true; } // for now always valid. but we check vs it inside initialise()
        };      
        
        bool initialise(const Config &config);
        
        // pay attention: so far we dont protect the following three set() w/ a mutex
        bool set(modeTX txm);
        bool set(const embot::core::Callback &onr);
        bool set(ItemParser itmp);
    
        // if modeTX::onflush we accumulate and tx on flush else we tx immediately
        bool put(const Item &item, embot::core::relTime timeout = embot::core::reltimeWaitForever);
        size_t output() const;
        bool flush(const embot::core::Callback &cbk);
        bool flush(embot::core::relTime timeout = embot::core::reltimeWaitForever);
        bool flushed() const;
        
        size_t input() const;
        // we get one by one and we parse it w/ something external ... the betters solution
        // timeout ? not for now
        bool get(Item &item, size_t &remaining);
        // BUT theCANservice has a parse all method, so .... 
        bool parse();
        
        // service for ROP. it is to be configured w/:
        // - other 2 dedicated threads
        // - immediate tx and rx (not deferred)
        // - for now using fixed LTR, fifos etc
        // the exchange is bidirectional 
        bool ask(uint8_t id, uint8_t size, void *reply, embot::core::relTime timeout = embot::core::reltimeWaitForever);
        bool say(uint8_t id, uint8_t size, void *value);
        bool set(uint8_t id, uint8_t size, void *value, embot::core::relTime timeout = embot::core::reltimeWaitForever);
        bool ack(uint8_t id);
        
    
    private:
        theICCservice(); 
        ~theICCservice(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    }; 

    
    constexpr theICCservice::Config iccmastercfg
    {
        theICCservice::Mode::master, embot::hw::icc::LTR::one, embot::hw::icc::LTR::two, 32, 32, theICCservice::modeTX::onflush, 
        embot::os::Priority::system54, embot::os::Priority::system53,
        {}, nullptr
    };       

    constexpr theICCservice::Config iccslavecfg
    {
        theICCservice::Mode::slave, embot::hw::icc::LTR::two, embot::hw::icc::LTR::one, 32, 32, theICCservice::modeTX::onflush, 
        embot::os::Priority::system54, embot::os::Priority::system53,
        {}, nullptr
    };     

} // namespace embot::app::eth {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
