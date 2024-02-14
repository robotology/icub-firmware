

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
    
    
        struct Item
        {
            embot::msg::Location des {};
            embot::prot::can::Frame frame {};
            constexpr Item() = default;
        };
        // static_assert(20 == sizeof(Item), "sizeof(Item) is not 20");
        
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
            static constexpr size_t maxcapacity {48}; 
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
        
        // pay attention: so far we don protect the following three set() w/ a mutex
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
