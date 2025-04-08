
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_DUALCORE_BSP_H_
#define __EMBOT_HW_DUALCORE_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"
#include "embot_hw_dualcore.h"


namespace embot::hw::dualcore::bsp {
    

    struct PROP
    {
        embot::hw::dualcore::CORE core {embot::hw::dualcore::CORE::cm7}; 
        embot::hw::dualcore::BOOT boot {embot::hw::dualcore::BOOT::cm7master};
        embot::hw::MTX mtx {embot::hw::MTX::one};         
        constexpr PROP() = default;
        constexpr PROP(embot::hw::dualcore::CORE c, embot::hw::dualcore::BOOT b, embot::hw::MTX m) : core(c), boot(b), mtx(m) {} 
    };
       
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static embot::hw::MpU thempu {embot::hw::MpU::one};
        constexpr static std::uint8_t maxnumberof = 1;
        std::array<const PROP*, maxnumberof> properties {}; 
        
        constexpr BSP(std::array<const PROP*, maxnumberof> pro) : SUPP(0x1), properties(pro) {} 
        constexpr BSP() : SUPP(0), properties({0}) {}
               
        constexpr const PROP * getPROP() const { return supported() ? properties[embot::core::tointegral(thempu)] : nullptr; }
        
        bool supported() const { return embot::hw::bsp::SUPP::supported(thempu); }
        
        bool ismaster() const {
            const PROP *p = getPROP();
            bool v { false };
            if(nullptr != p)
            {
                switch(p->core) 
                {
                    case embot::hw::dualcore::CORE::cm7: { v = embot::hw::dualcore::BOOT::cm7master == p->boot; } break;
                    case embot::hw::dualcore::CORE::cm4: { v = embot::hw::dualcore::BOOT::cm4master == p->boot; } break;                
                    default: {} break; 
                }
            }
            return v;            
        } 
        
        bool config(const Config &c) const;
        const Config& config() const;
        bool init() const;
    };
    
    const BSP& getBSP();
    
    
} // namespace embot::hw::dualcore::bsp {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


