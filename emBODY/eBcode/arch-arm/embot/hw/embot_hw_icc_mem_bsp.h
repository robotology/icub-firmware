
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_ICC_MEM_BSP_H_
#define __EMBOT_HW_ICC_MEM_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"
#include "embot_hw_icc_mem.h"

namespace embot::hw::icc::mem::bsp {
             
    // --  MEM
    
    struct PROP
    {
        embot::hw::MTX mtx {embot::hw::MTX::none};
        embot::core::Data data {};
        
        constexpr PROP() = default;
        constexpr PROP(void *address, size_t size, embot::hw::MTX s) : data(address, size), mtx(s) {}
        constexpr bool isvalid() const 
        { 
            return data.isvalid() && (embot::core::tointegral(mtx) < embot::core::tointegral(embot::hw::MTX::maxnumberof)); 
        }
        constexpr void * address() const { return data.pointer; }
        constexpr size_t size() const { return data.capacity; } 
        constexpr embot::hw::MTX semaphore() const { return mtx; };
    };    
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::icc::MEM::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {} 
        constexpr BSP() : SUPP(0), properties({0}) {}
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::icc::MEM h) const { return supported(h) ? properties[embot::core::tointegral(h)] : nullptr; }
        void init(embot::hw::icc::MEM h) const;
    };
    
    const BSP& getBSP();    

} // namespace embot::hw::icc::mem::bsp {

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


