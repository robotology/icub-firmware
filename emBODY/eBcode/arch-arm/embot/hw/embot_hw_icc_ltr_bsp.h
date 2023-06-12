
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_ICC_LTR_BSP_H_
#define __EMBOT_HW_ICC_LTR_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"
#include "embot_hw_icc.h"
#include "embot_hw_icc_ltr.h"


namespace embot::hw::icc::ltr::bsp {
       
    // LTR
    
    struct PROP
    {                 
        embot::hw::icc::MEM mem {embot::hw::icc::MEM::none};
        embot::hw::icc::SIG sig {embot::hw::icc::SIG::none}; 
        embot::hw::icc::SIG ack {embot::hw::icc::SIG::none};        
    };
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::icc::LTR::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {} 
        constexpr BSP() : SUPP(0), properties({0}) {}

        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::icc::LTR l) const { return supported(l) ? properties[embot::core::tointegral(l)] : nullptr; }
        void init(embot::hw::icc::LTR l) const;
    };
    
    const BSP& getBSP();  
   
                                             
}   // namespace embot::hw::icc::ltr::bsp {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


