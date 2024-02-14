
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#ifndef __EMBOT_MSG__
#define __EMBOT_MSG__


#include "embot_core.h"
#include "embot_core_binary.h"

// needed for: eOlocation_t
#include "EoCommon.h"


namespace embot::msg {
  
    // embot::msg::BUS has same values as eObus_t 
    enum class BUS : uint8_t {can1 = 0, can2 = 1, local = 2, icc1 = 3, icc2 = 4, ffu1 = 5, ffu2 = 6, none = 7}; 
    static constexpr uint8_t numberofBUSes {7};
    using ADR = uint8_t;
    
    inline std::string bus_to_string(const BUS b)
    {
        static const char *buses[] = 
        {
            "BUS::can1",   "BUS::can2", "BUS::local", "BUS::icc1", "BUS::icc2", "BUS::ffu1", "BUS::ffu2", "BUS::none"    
        };
        
        return std::string(buses[embot::core::tointegral(b)]);
    }
    
    enum class typeofBUS : uint8_t { 
        CAN = 0,        // the can bus
        LOCAL = 1,      // used for local HW devices directly managed by the CPU
        ICC = 2,        // the inter-core-communication bus used to transfer from one core to another
        FFU = 3,
        NONE = 4
    };
    
    inline typeofBUS bus_to_type(const BUS b)
    {
        static const typeofBUS types[] = 
        {
            typeofBUS::CAN,   typeofBUS::CAN, typeofBUS::LOCAL, typeofBUS::ICC, typeofBUS::ICC, typeofBUS::FFU, typeofBUS::FFU, typeofBUS::NONE   
        }; 
        
        return types[embot::core::tointegral(b)];        
    }
    
    
    // embot::msg::Location is its a C++ version of eOlocation_t that does not have the same binary footprint
    // embot::msg::Location::PACKED is a C++ version of eOlocation_t that has its same binary footprint
    // we use it just for ... easy migration between types
    
    // NOTE: 
    // 1. do we need the PACKED? 
    // 2. can we remove the constexpr Location(const eOlocation_t &l) ???
    //    maybe we can use constexpr Location(const void *p2eolocation) ....
    struct Location        
    {  
        struct PACKED { // PACKED has the same binary format as eOlocation_t
            uint8_t bus:3; uint8_t ffu:1; uint8_t adr:4; 
            constexpr PACKED() : bus(7), ffu(0), adr(0) {}
            constexpr PACKED(uint8_t b, uint8_t a, uint8_t f = 0) : bus (b&0x0f), adr(a&0x0f), ffu(f) {}
            constexpr PACKED(const void *p2eolocation) : bus(7), ffu(0), adr(0) { 
                if(nullptr != p2eolocation) { const PACKED *p = reinterpret_cast<const PACKED*>(p2eolocation); bus = p->bus; ffu = p->ffu; adr = p->adr; } 
            }
            bool operator==(const PACKED& rhs) const
            {
                return (rhs.bus == bus) && (rhs.adr == adr) && (rhs.ffu == ffu);
            }
        };
                         
        constexpr Location() = default;
        constexpr Location(BUS b, ADR a) : bus(b), adr(a), ffu(0) { packed.bus = embot::core::tointegral(bus); packed.adr = adr; packed.ffu = 0;  } 
        constexpr Location(const PACKED &p) : packed(p) { bus = static_cast<BUS>(packed.bus); adr = packed.adr; ffu = 0; } 
        constexpr Location(const void *p2eolocation) : packed(p2eolocation) { bus = static_cast<BUS>(packed.bus); adr = packed.adr; ffu = 0; }    
        constexpr Location(const eOlocation_t &l)  : packed(&l) { bus = static_cast<BUS>(packed.bus); adr = packed.adr; ffu = 0; }
        
        void set(BUS b, ADR a)
        {
            bus = b; adr = a; ffu = 0;
            packed.bus = embot::core::tointegral(bus); packed.adr = adr; packed.ffu = 0;
        }
        
        BUS getbus() const { return bus; }
        ADR getadr() const { return adr; }
        
        std::string to_string() const
        {
            return bus_to_string(bus) + ":" + std::to_string(adr);
        }
        
        constexpr bool isvalid() const { return embot::msg::BUS::none != bus; }  
        constexpr bool isCAN() const { return (embot::msg::BUS::can1 == bus) || (embot::msg::BUS::can2 == bus); }       
        constexpr bool isICC() const { return (embot::msg::BUS::icc1 == bus) || (embot::msg::BUS::icc2 == bus); }
        constexpr typeofBUS typeofbus() const { return bus_to_type(bus); }
    
        bool operator==(const Location& rhs) const
        {
            return (rhs.bus == bus) && (rhs.adr == adr) && (rhs.ffu == ffu); // && (rhs.packed == packed);
        } 
        
    protected:
        
        // i have chosen protected because i prefer to use ctor() and set() to fill values so that we avoid incomplete initialization
        // so, i need get() to retrieve the values  
        
        // packed may be dropped ...
        PACKED packed {embot::core::tointegral(BUS::none), 0};
       
        BUS bus {BUS::none};
        ADR adr {0};        
        uint8_t ffu {0};                
    };
    static_assert(sizeof(Location::PACKED) == 1, "err");
    static_assert(sizeof(Location) == 4, "err");
    
    // Location can be constructed from a eOlocation_t using the dedicated constructor
    // or with the following fill()    
    void fill(Location &loc, const eOlocation_t &l);  
    // if needed, we can also uncomment this
    // void fill(Location &loc, const eObrd_location_t &l); 
    // but we need: #include "EoBoards.h"  
    
    Location transform(const eOlocation_t &l);
    //eOlocation_t transform(const Location &l);
    
}

 
#endif  // include-guard


