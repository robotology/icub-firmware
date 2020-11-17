
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_CAN_BSP_H_
#define _EMBOT_HW_CAN_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"


namespace embot { namespace hw { namespace can {
    
    #if defined(HAL_CAN_MODULE_ENABLED)
        using CAN_Handle = CAN_HandleTypeDef;    
    #elif defined(HAL_FDCAN_MODULE_ENABLED)
        using CAN_Handle = FDCAN_HandleTypeDef;
    #else
        using CAN_Handle = void;
    #endif
    
    struct PROP
    { 
        CAN_Handle* handle {nullptr};   
    };
   
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::CAN::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {}
        constexpr BSP() : SUPP(0), properties({0}) {}            
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::CAN h) const { return supported(h) ? properties[embot::core::tointegral(h)] : nullptr; }
        void init(embot::hw::CAN h) const;
        constexpr embot::hw::CAN toID(const PROP& p) const
        { 
            if(nullptr == p.handle) { return embot::hw::CAN::none; }
            for(uint8_t i=0; i<maxnumberof; i++) { if(p.handle == properties[i]->handle) return static_cast<embot::hw::CAN>(i); }
            return embot::hw::CAN::none;
        }
    };
    
    const BSP& getBSP();                                        
        
}}} // namespace embot { namespace hw { namespace can { 



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


