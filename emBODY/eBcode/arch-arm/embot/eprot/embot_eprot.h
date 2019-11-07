
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - brief
//   it contains types of the eth protocol used inside icub: the rop 
//

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_EPROT_H_
#define _EMBOT_EPROT_H_

#include "embot_common.h"
#include "embot_utils.h"
#include <stdio.h>

namespace embot { namespace eprot {

    
    struct IPv4
    {
        uint32_t v {0};
        IPv4() = default;
        constexpr IPv4(uint32_t i) : v(i) {}
        constexpr IPv4(uint8_t a, uint8_t b, uint8_t c, uint8_t d) : v((a<<24)|(b<<16)|(c<<8)|(d)) {}
        constexpr IPv4(const char *str) { 
            v = 0;
            if(nullptr == str) { return; }
            uint8_t a{0}; uint8_t b{0}; uint8_t c{0}; uint8_t d{0}; 
            if(4 == std::sscanf(str, "%hhi.%hhi.%hhi.%hhi", &a, &b, &c, &d)) 
            {
                v = ((a<<24)|(b<<16)|(c<<8)|(d)); 
            }    
        }
        constexpr char * tostring(char *s, size_t size) const { 
            if(nullptr != s) { std::snprintf(s, size, "%d.%d.%d.%d", v>>24, (v>>16)&0xff, (v>>8)&0xff, (v)&0xff); }
            return s;
        }
    };

    using Endpoint = std::uint8_t;
    using Entity = std::uint8_t;
    using Index = std::uint8_t;
    using Tag = std::uint8_t;

    constexpr uint8_t none08 = 0xff;
    constexpr uint8_t unknown08 = 0xfe;

    enum class EP : uint8_t { management = 0, motioncontrol = 1, analogsensors = 2, skin = 3, max = skin, unknown = unknown08, none = none08 }; 
    static constexpr uint8_t maxValueOfEP = embot::common::tointegral(EP::max);
    
    enum class EN : uint8_t { 
        // EP::management
        mnComm = 0, mnApp = 1, mnInfo = 2, mnService = 3, mnMax = mnService,
        // EP::motioncontrol
        mcJoint = 0, mcMotor = 1, mcController = 2, mcMax = mcController,
        // EP::analogsensors
        asStrain = 0, asMais = 1, asTemperature = 2, asInertialLegacy = 3, asInertial3 = 4, asIMU = 4, asPSC = 5, asMax = asPSC,
        // EP::skin
        skSkinPatch = 0, skMax = skSkinPatch,
        // commons
        unknown = unknown08, none = none08
    }; 
    static constexpr uint8_t maxValueOfENs = embot::common::tointegral(EN::asMax);
    
    static constexpr uint8_t maxvEN = 5;
       
    using ID32 = std::uint32_t;

    constexpr ID32 getID32(EP ep, EN en, Index i, Tag t)
    {
        return ((static_cast<uint32_t>(ep) << 24) | (static_cast<uint32_t>(en) << 16) | (static_cast<uint32_t>(i&0xff) << 8) | (static_cast<uint32_t>(t&0xff) << 0));
    }

    constexpr ID32 ID32none = getID32(EP::none, EN::none, none08, none08); // 0xffffffff

    constexpr EP getEP(ID32 id32)
    {
        uint8_t x = (id32 >> 24) & 0xff;
        return ((x <= embot::common::tointegral(EP::max)) || (x == embot::common::tointegral(EP::none))) ? (static_cast<EP>(x)) : (EP::unknown);
    }

#if 0
    // boh, forse meglio una mappa in rom. questo codice necessita c++17
    constexpr EN getEN(ID32 id32)
    {
        EN en = EN::unknown;
        EP ep = getEP(id32);
        uint8_t x = (id32 >> 16) & 0xff;
        auto sel = [](uint8_t v, EN m) { return ((v <= embot::common::tointegral(m)) || (v == embot::common::tointegral(EN::none))) ? (static_cast<EN>(v)) : (EN::unknown); }; 
        switch(ep)
        {
            case EP::management:
            {
                en = sel(x, EN::mnMax);
            } break;

            case EP::motioncontrol:
            {
                en = sel(x, EN::mcMax);
            } break;

            case EP::analogsensors:
            {
                en = sel(x, EN::asMax);
            } break;           

            case EP::skin:
            {
                en = sel(x, EN::skMax);
            } break;   

            case EP::none:
            {
                en = EN::none;
            } break;

            case EP::unknown:
            default:
            {
            } break;

        }
        return en;
    }
#else
    // ecco la versione con mappa in rom
    constexpr EN getEN(ID32 id32)
    {
        EN en = EN::unknown;
        EP ep = getEP(id32);
        constexpr EN map[maxValueOfEP+1][maxValueOfENs+1] = 
        { 
            {EN::mnComm, EN::mnApp, EN::mnInfo, EN::mnService, EN::unknown, EN::unknown}, 
            {EN::mcJoint, EN::mcMotor, EN::mcController, EN::unknown, EN::unknown, EN::unknown}, 
            {EN::asStrain, EN::asMais, EN::asTemperature, EN::asInertialLegacy, EN::asIMU, EN::asPSC},
            {EN::skSkinPatch, EN::unknown, EN::unknown, EN::unknown, EN::unknown, EN::unknown} 
        };
        if((EP::none != ep) && (EP::unknown != ep))
        {
            uint8_t x = (id32 >> 16) & 0xff;
            if(x <= maxValueOfENs)
            {
                en = map[embot::common::tointegral(ep)][x];
            }
        }
 
        return en;
    }
#endif

    constexpr Index getIndex(ID32 id32)
    {
        return (id32 >> 8) & 0xff;
    }

    constexpr Tag getTag(ID32 id32)
    {
        return (id32) & 0xff;
    }

}} // namespace embot { namespace eprot { 


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
