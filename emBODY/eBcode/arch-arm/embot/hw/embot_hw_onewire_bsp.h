
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_ONEWIRE_BSP_H_
#define _EMBOT_HW_ONEWIRE_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"


namespace embot { namespace hw { namespace onewire {
    
    struct PROP
    {   
        embot::hw::GPIO gpio {embot::hw::GPIO::PORT::none, embot::hw::GPIO::PIN::none};  
    };
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::ONEWIRE::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {}
        constexpr BSP() : SUPP(0), properties({0}) {}        
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::ONEWIRE h) const { return supported(h) ? properties[embot::core::tointegral(h)] : nullptr; }
        void init(embot::hw::ONEWIRE h) const;
    };
    
    const BSP& getBSP();                                      
        
}}} // namespace embot { namespace hw { namespace onewire { 



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


