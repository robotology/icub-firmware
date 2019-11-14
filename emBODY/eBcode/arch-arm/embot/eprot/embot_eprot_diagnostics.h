
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - brief
//   it contains types of the eth protocol used inside icub: the rop 
//

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_EPROT_DIAGNOSTICS_H_
#define _EMBOT_EPROT_DIAGNOSTICS_H_

#include "embot_common.h"
#include "embot_binary.h"
#include "embot_utils.h"
#include "embot_eprot.h"


namespace embot { namespace eprot { namespace diagnostics {
    
    constexpr embot::eprot::Tag tag_wholeitem = 0;
    constexpr embot::eprot::Tag tag_config = 1;  
    constexpr embot::eprot::Tag tag_config_enable = 2;  
    constexpr embot::eprot::Tag tag_info = 3;  
    constexpr embot::eprot::Tag tag_info_basic = 4;  
    
    
    struct Config
    {
        // -> memory layout
        uint8_t enable {1};
        uint8_t filler[7] {0};
        // <- memory layout
        
        Config() = default;
        
        constexpr static uint16_t sizeofobject = 8;
    }; static_assert(sizeof(Config) == Config::sizeofobject, "embot::eprot::diagnostics::Config has wrong sizeofobject. it must be 2");
    
        
    // 16 bits -> FFU(4) | EXT(2) | ADR(4) | SRC(3) | TYP(3) 
    enum class TYP : uint16_t { info = 0, debug = 1, warning = 2, error = 3, fatal = 4, max = 7 }; // uses 3 bits -> up to value = 7
    enum class SRC : uint16_t { board = 0, can1 = 1, can2 = 2, max  = 7 }; // uses 3 bits -> up to value = 7 
    enum class ADR : uint16_t { zero = 0, one = 1, two, three, four, five, six, seven, eigth, nine, ten, eleven, twelve, thirteen, fourteen, fifteen = 15, max = 15 };
    enum class EXT : uint16_t { none = 0, verbal = 1, compact1 = 2, max = 3 }; // uses 2 bits -> up to value = 3
    enum class FFU : uint16_t { none = 0, max = 15 };

    struct InfoProperties 
    {
        // -> memory layout
        uint16_t flags {0};
        // <- memory layout
        
        InfoProperties() = default;
        
        constexpr InfoProperties(TYP t, SRC s, ADR a, EXT e, FFU f = FFU::none) : flags(load(t, s, a, e, f)) {}
        
        // legacy ...
        constexpr InfoProperties(uint16_t fl) : flags(fl) {}
                
        constexpr uint16_t load(TYP t, SRC s, ADR a, EXT e, FFU f = FFU::none)
        {
            flags = (embot::common::tointegral(t)) | (embot::common::tointegral(s) << 3) | 
                    (embot::common::tointegral(a) << 6) | (embot::common::tointegral(e) << 10) | (embot::common::tointegral(f) << 12);
            return flags;
        }
        
        constexpr TYP getTYP() const { return static_cast<TYP>((flags>>0)&0x7); }
        constexpr SRC getSRC() const { return static_cast<SRC>((flags>>3)&0x7); }
        constexpr ADR getADR() const { return static_cast<ADR>((flags>>6)&0xf); }
        constexpr EXT getEXT() const { return static_cast<EXT>((flags>>10)&0x3); }
        constexpr FFU getFFU() const { return static_cast<FFU>((flags>>12)&0xf); }        
               
        constexpr static uint16_t sizeofobject = 2;
    }; static_assert(sizeof(InfoProperties) == InfoProperties::sizeofobject, "embot::eprot::diagnostics::InfoProperties has wrong sizeofobject. it must be 2");
    
    
    struct InfoBasic
    {
        // -> memory layout
        embot::common::Time timestamp {0};
        uint32_t code {0};
        InfoProperties flags { TYP::info, SRC::board, ADR::zero, EXT::none }; 
        uint16_t par16 {0};
        uint64_t par64 {0};
        // <- memory layout

        // methods
        InfoBasic() = default;
        constexpr InfoBasic(embot::common::Time t, uint32_t c, InfoProperties p, uint16_t p16, uint64_t p64) : timestamp(t), code(c), flags(p), par16(p16), par64(p64) {}
        // constexpr bool load(void *rawmemory) { if(nullptr == rawmemory) { return false; } std::memmove(this, rawmemory, sizeofobject); return true; }
        constexpr static embot::eprot::ID32 id32 = embot::eprot::getID32(embot::eprot::EP::management, embot::eprot::EN::mnInfo, 0, tag_info_basic);
        constexpr static uint16_t sizeofobject = 24;
    };  static_assert(sizeof(InfoBasic) == InfoBasic::sizeofobject, "embot::eprot::diagnostics::InfoBasic has wrong sizeofobject. it must be 24");

    
    struct Info
    {
        constexpr static uint16_t extrasizeof = 48; 
        
        // -> memory layout
        InfoBasic basic {};
        uint8_t extra[extrasizeof] {0};  
        // -> memory layout
        
        // methods
        Info() =  default;
            
        constexpr static embot::eprot::ID32 id32 = embot::eprot::getID32(embot::eprot::EP::management, embot::eprot::EN::mnInfo, 0, tag_info);
        constexpr static uint16_t sizeofobject = 72;        
    };  static_assert(sizeof(Info) == Info::sizeofobject, "embot::eprot::diagnostics::Info has wrong sizeofobject. it must be 72");


}}} // namespace embot { namespace eprot { namespace diagnostics {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
