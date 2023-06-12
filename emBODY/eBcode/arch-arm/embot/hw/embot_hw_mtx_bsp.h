
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MTX_BSP_H_
#define __EMBOT_HW_MTX_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"
#include "embot_hw_mtx.h"

namespace embot::hw::mtx::bsp {
    
    using hwSEM = uint32_t;
    
    struct PROP
    {   
        hwSEM hsem {0};    
    };
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::MTX::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {} 
        constexpr BSP() : SUPP(0), properties({0}) {}
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::MTX h) const { return supported(h) ? properties[embot::core::tointegral(h)] : nullptr; }
        void init(embot::hw::MTX h) const;
        
        constexpr embot::hw::MTX toMTX(const hwSEM hsem) const
        {
            embot::hw::MTX mm {embot::hw::MTX::none};
            for(uint8_t s=0; s<maxnumberof; s++)
            {
                const PROP *pm = properties[s];
                if((nullptr != pm) && (hsem == pm->hsem))
                {
                    mm = static_cast<embot::hw::MTX>(s);
                    break;
                }
            }            
            return mm;
        }           
    };
    
    const BSP& getBSP();   
    
                                   
}   // namespace embot::hw::mtx::bsp {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


