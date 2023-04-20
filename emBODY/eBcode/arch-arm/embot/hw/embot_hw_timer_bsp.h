
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_TIMER_BSP_H_
#define __EMBOT_HW_TIMER_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"


namespace embot { namespace hw { namespace timer {
             
#if   defined(HAL_TIM_MODULE_ENABLED)    
    using TIM_t = TIM_TypeDef;
    using TIM_Handle = TIM_HandleTypeDef;
#else
    using TIM_t = void;
    using TIM_Handle = void;
#endif
    
    struct PROP
    {   
        TIM_t* TIMx {nullptr};
        TIM_Handle* handle {nullptr};  
        embot::hw::CLOCK clock {embot::hw::CLOCK::none}; // the clock used by the timer
        uint32_t speed {0};        
        bool isonepulse {false};
        bool mastermode {false};      
    };
 
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::TIMER::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {} 
        constexpr BSP() : SUPP(0), properties({0}) {}
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::TIMER h) const { return supported(h) ? properties[embot::core::tointegral(h)] : nullptr; }
        void init(embot::hw::TIMER h) const;
    };
    
    const BSP& getBSP();
    
}}} // namespace embot { namespace hw { namespace timer { 



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


