
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_BSP_H_
#define __EMBOT_HW_MOTOR_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"
#include "embot_hw_motor.h"

namespace embot::hw::motor::bsp { 
    
    struct PROP
    { 
        uint32_t tbd {0};
    };
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::MOTOR::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {} 
        constexpr BSP() : SUPP(0), properties({0}) {}
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::MOTOR h) const { return supported(h) ? properties[embot::core::tointegral(h)] : nullptr; }
        void init(embot::hw::MOTOR h) const;
    };
    
    const BSP& getBSP();
                                     
        
} // namespace embot::hw::motor::bsp { 



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


