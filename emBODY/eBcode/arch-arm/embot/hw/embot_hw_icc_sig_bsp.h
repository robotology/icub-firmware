
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_ICC_SIG_BSP_H_
#define __EMBOT_HW_ICC_SIG_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"
#include "embot_hw_icc_sig.h"

namespace embot::hw::icc::sig::bsp {
       
    struct PROP
    {   
        embot::hw::MTX mtx {embot::hw::MTX::none};    
    };
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::icc::SIG::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {} 
        constexpr BSP() : SUPP(0), properties({0}) {}

        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::icc::SIG s) const { return supported(s) ? properties[embot::core::tointegral(s)] : nullptr; }
        void init(embot::hw::icc::SIG s) const;
    };
    
    const BSP& getBSP();  
                                             
}   // namespace embot::hw::icc::sig::bsp {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


