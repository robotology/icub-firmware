

/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THEICCSERVICE_H_
#define __EMBOT_APP_ETH_THEICCSERVICE_H_

#include "embot_core.h"
#include "embot_os.h"
#include "embot_hw_icc_ltr.h"

#include <array>


namespace embot::app::eth::icc {
      
#if 0
    
    The singleton `theICCservice` is responsible to exchange data across the two cores in the following way.
    - The object `theICCservice` manages two independent pipes that can transport chuncks of data in full duplex.
    - The two pipes are managed:
      - at higher layer by two high priotity threads: the TX thread that is activated by a user request and the RX thread that is triggered by the underlying HW.
      - at low level each pipe uses two `embot::hw::icc::LTR`: one for TX and one for RX. The letters contain some shared memory that hosts multiple chunks of data
        and some HW mutex to implement inter core signalling.  
    - The generic chunck of data is modeled by the object `Item` (that for now is 24 bytes w/ 23 bytes of payload) that can be used to transport a CAN frames 
      or a ROP via derivation.     
      - the object `ItemCANframe` transports an `embot::prot::can::Frame` 
      - the object `ItemROP` transports a remote operation (ROP) that is able to execute operation on the other core such as set<ID, data>, ask<is>, sig<ID, data> 
        that obey to a specific protocol. 
        - when a set<> is received the core gets the data and if ID is present it uses it and sends back an ack<ID> else it sends a nak<ID>
        = when a ask<ID> is received if ID is present it sends back say<ID, data>> else it sends a nak<ID>  
        - when a sig<ID, data> is received if ID is present it uses data. 
            

    
    
    For the case of CAN frame we use an `ItemCANframe` and we mimic the way the CAN service works. So, Reception is always immediate 
    and is buffered in its internals, ready to be retrieved when needed. The transmission can be either immediate (with `modeTX::instant`)
    or when the flush() is called (with `modeTX::instant`).


    For the case of ROPs we use an `ItemROP` and we offer operations such as ask, set, say, ping, ack, nak, sig.
    The ROP mode can be used to exchange data in a similar way to what we do with CAN frames but up to 20 bytes of payload (easily upgradable to
    more bytes).
    The ROP implements a mechanism of data exhange that requires:
    - an operation: ask, say, set, ping, ack, nak, sig;
    - an ID that refers to a specific variable shared by the two cores (for instance the Unique Identifier of the MPU or its VCC);     
    - the size of the shared variable;
    - the content of the variable.
    
    It is also needed a callback mechanism that allows the caller to give reply to a request for a particular variable.
    This application logic is not implemented in the `theICCservice` except for the case of the special `ItemROP::CMD::ping`.

      
#endif

    enum class ItemType : uint8_t { RAW = 0, CANframe = 1, ROP = 2, TBD = 3 };
      
    struct Item
    {
        ItemType type {ItemType::RAW};
        uint8_t filler[3] = {0};
        uint8_t data[20] = {0};
        constexpr Item() = default;
    }; 
    static_assert(24 == sizeof(Item), "sizeof(Item) is not 24");      

        
    class theICCservice
    {
    public:
        static theICCservice& getInstance();
    
        // the ICC service has two pipes
        enum class Pipe : uint8_t { one = 0, two = 1 };
        static constexpr size_t NumberOfPipes {2};  
                      
              
        using ItemParser = bool (*)(const Item &item);
        static bool ItemParserDummy(const Item &item) { return false; }
        
        enum class modeTX : uint8_t { instant = 0, onflush = 1 };
                       
        struct PipeConfig
        {
            static constexpr size_t maxcapacity {40}; 
            // we can manage 48 Items in each direction by using two icc::LTR of 1024 bytes
            // the check is done inside initialise()            
            
            embot::hw::icc::LTR rxltr {embot::hw::icc::LTR::one};
            embot::hw::icc::LTR txltr {embot::hw::icc::LTR::two};
            size_t rxcapacity {32}; 
            size_t txcapacity {32};
            modeTX modetx {modeTX::instant};
            embot::core::Callback onrx {};
            ItemParser itemparser {nullptr};
            
            constexpr PipeConfig() = default;
            
            constexpr PipeConfig(embot::hw::icc::LTR rl, embot::hw::icc::LTR tl,
                                size_t r, size_t t, 
                                modeTX mt, const embot::core::Callback &orx, ItemParser ip)
                            :   rxltr(rl), txltr(tl),
                                rxcapacity(std::min(r, maxcapacity)), txcapacity(std::min(t, maxcapacity)),
                                modetx(mt), onrx(orx), itemparser(ip)
                            {}
                                
            bool isvalid() const { return true; } // for now always valid. but we check vs it inside initialise()
        };            
         
        enum Mode : uint8_t { master = 0, slave = 1 };
     
        struct Config
        {                   
            Mode mode {Mode::master};
            embot::os::Priority rxpriority {embot::os::Priority::system54};
            embot::os::Priority txpriority {embot::os::Priority::system53};
            std::array<PipeConfig, NumberOfPipes> pipeconfig {};
            
            constexpr Config() = default;
            
            constexpr Config(   Mode mo, embot::os::Priority rp, embot::os::Priority tp, const std::array<PipeConfig, NumberOfPipes> p)
                            :   mode (mo), rxpriority(rp), txpriority(tp), pipeconfig(p)
                            {}
                                
            bool isvalid() const { return true; } // for now always valid. but we check vs it inside initialise()
        };      
        
        bool initialise(const Config &config);
        

        bool set(Pipe pipe, modeTX txm);
        bool set(Pipe pipe, const embot::core::Callback &onr);
        bool set(Pipe pipe, ItemParser itmp);
    
        // if modeTX::onflush we accumulate and tx on flush else we tx immediately
        bool put(Pipe pipe, const Item &item, embot::core::relTime timeout = embot::core::reltimeWaitForever);
        size_t output(Pipe pipe) const;
        bool flush(Pipe pipe, const embot::core::Callback &cbk);
        bool flush(Pipe pipe, embot::core::relTime timeout = embot::core::reltimeWaitForever);
        bool flushed(Pipe pipe) const;
        
        size_t input(Pipe pipe) const;
        // we can get the Item one by one and use it as we like
        bool get(Pipe pipe, Item &item, size_t &remaining);
        // or we can parse them all as for instance theCANservice does
        bool parse(Pipe pipe);
                    
    private:
        theICCservice(); 
        ~theICCservice(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    }; 


    constexpr theICCservice::PipeConfig pipeCANmaster
    {
        embot::hw::icc::LTR::one, embot::hw::icc::LTR::two, 32, 32, theICCservice::modeTX::onflush,
        {}, nullptr        
    };

    constexpr theICCservice::PipeConfig pipeROPmaster
    {
        embot::hw::icc::LTR::three, embot::hw::icc::LTR::four, 32, 32, theICCservice::modeTX::onflush,
        {}, nullptr        
    };
    
    constexpr theICCservice::Config iccmastercfg
    {
        theICCservice::Mode::master, embot::os::Priority::system54, embot::os::Priority::system53,
        {{pipeCANmaster, pipeROPmaster}}
    }; 

    constexpr theICCservice::PipeConfig pipeCANslave
    {
        embot::hw::icc::LTR::two, embot::hw::icc::LTR::one, 32, 32, theICCservice::modeTX::onflush,
        {}, nullptr        
    };

    constexpr theICCservice::PipeConfig pipeROPslave
    {
        embot::hw::icc::LTR::four, embot::hw::icc::LTR::three, 32, 32, theICCservice::modeTX::onflush,
        {}, nullptr        
    };    

    constexpr theICCservice::Config iccslavecfg
    {
        theICCservice::Mode::slave, embot::os::Priority::system54, embot::os::Priority::system53,
        {{pipeCANslave, pipeROPslave}}
    };   
          

} // namespace embot::app::eth:icc {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
