
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_icc_bsp_amc2c.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>
#include <array>

#include "embot_core_binary.h"
#include "embot_core.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif


using namespace std;
using namespace embot::core::binary;

// --------------------------------------------------------------------------------------------------------------------
// - configuration of peripherals and chips. it is done board by board. it contains a check vs correct STM32HAL_BOARD_*
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_amc2c_config.h"


// --------------------------------------------------------------------------------------------------------------------
// - support maps
// --------------------------------------------------------------------------------------------------------------------


// - support map: begin of embot::hw::icc::sig

#include "embot_hw_icc_sig_bsp.h"

#if !defined(EMBOT_ENABLE_hw_icc_sig)

namespace embot::hw::icc::sig::bsp {
        
    // -- SIG
    
    constexpr BSP thebsp {};
    void BSP::init(embot::hw::icc::SIG h) const {}    
    const BSP & getBSPsig() { return thebsp; }    
    
}

#elif defined(EMBOT_ENABLE_hw_icc_sig)

namespace embot::hw::icc::sig::bsp {
                
    // -- SIG

    constexpr PROP sig01 = { embot::hw::MTX::five };
    constexpr PROP sig02 = { embot::hw::MTX::six };
    constexpr PROP sig03 = { embot::hw::MTX::seven };
    constexpr PROP sig04 = { embot::hw::MTX::eight };
    
    
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(SIG::one) | mask::pos2mask<uint32_t>(SIG::two) | 
        mask::pos2mask<uint32_t>(SIG::three) | mask::pos2mask<uint32_t>(SIG::four),        
        // properties
        {{
            &sig01, &sig02, &sig03, &sig04    
        }}        
    };
    

    void BSP::init(embot::hw::icc::SIG h) const { }    
    
    const BSP & getBSP() { return thebsp; }  
    
}

#endif // #elif defined(EMBOT_ENABLE_hw_icc_sig)


// - support map: begin of embot::hw::icc::mem

#include "embot_hw_icc_mem_bsp.h"

#if !defined(EMBOT_ENABLE_hw_icc_mem)

namespace embot::hw::icc::mem::bsp {
    
    // -- MEM
    
    constexpr BSP thebsp {};    
    void BSP::init(embot::hw::icc::MEM h) const {}
    const BSP & getBSP() { return thebsp; }  
    
}

#elif defined(EMBOT_ENABLE_hw_icc_mem)

namespace embot::hw::icc::mem::bsp {

    // -- MEM
    
    constexpr uint32_t startmemory {0x38001000};
    constexpr uint32_t memsize {256};
    
    static const PROP mm1 = {
        reinterpret_cast<void*>(startmemory),
        memsize,
        MTX::nine
    };
    static const PROP mm2 = {
        reinterpret_cast<void*>(startmemory+1*memsize),
        memsize,
        MTX::ten
    };
    static const PROP mm3 = {
        reinterpret_cast<void*>(startmemory+2*memsize),
        memsize,
        MTX::eleven
    };
    static const PROP mm4 = {
        reinterpret_cast<void*>(startmemory+3*memsize),
        memsize,
        MTX::twelve
    };  
    
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(MEM::one) | mask::pos2mask<uint32_t>(MEM::two) | 
        mask::pos2mask<uint32_t>(MEM::three) | mask::pos2mask<uint32_t>(MEM::four),        
        // properties
        {{
            &mm1, &mm2, &mm3, &mm4      
        }}        
    };
    
    void BSP::init(embot::hw::icc::MEM h) const { }    
        
    const BSP & getBSP() { return thebsp; }  
 
} // namespace embot::hw::icc::mem::bsp {

#endif // #elif defined(EMBOT_ENABLE_hw_icc_mem)    


// - support map: begin of embot::hw::icc::ltr

#include "embot_hw_icc_ltr_bsp.h"

#if !defined(EMBOT_ENABLE_hw_icc_ltr)

namespace embot::hw::icc::ltr::bsp {
    
    // -- LTR
    
    constexpr BSP thebsp {};    
    void BSP::init(embot::hw::icc::LTR h) const {}
    const BSP & getBSP() { return thebsp; }  
    
}

#elif defined(EMBOT_ENABLE_hw_icc_ltr)

namespace embot::hw::icc::ltr::bsp {

    // LTR

    constexpr PROP ltr01 = { embot::hw::icc::MEM::four, embot::hw::icc::SIG::four, embot::hw::icc::SIG::three };
       
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(LTR::one),        
        // properties
        {{
            &ltr01
        }}        
    };
    

    void BSP::init(embot::hw::icc::LTR h) const { }            

    const BSP & getBSP() 
    {
        return thebsp;
    }     

} // namespace embot::hw::icc::ltr::bsp {

#endif // #elif defined(EMBOT_ENABLE_hw_icc_ltr)    


//namespace embot::hw::icc::sig::bsp {
//        
////    // -- SIG
////    constexpr BSPsig thebsp_sig {};
////    void BSPsig::init(embot::hw::icc::SIG h) const {}    
////    const BSPsig & getBSPsig() { return thebsp_sig; }  
//       
////    // -- MEM
////    constexpr BSPmem thebsp_mem {};    
////    void BSPmem::init(embot::hw::icc::MEM h) const {}
////    const BSPmem & getBSPmem() { return thebsp_mem; }  

////    // LTR      
////    constexpr BSPltr thebsp_ltr {};
////    void BSPltr::init(embot::hw::icc::LTR h) const {}
////    const BSPltr & getBSPltr() { return thebsp_ltr; }    

//}

//#elif defined(EMBOT_ENABLE_hw_icc)


//    

//    


//    // LTR

//    constexpr PROPltr ltr01 = { embot::hw::icc::MEM::four, embot::hw::icc::SIG::four, embot::hw::icc::SIG::three };
//       
//    constexpr BSPltr thebsp_ltr {        
//        // maskofsupported
//        mask::pos2mask<uint32_t>(LTR::one),        
//        // properties
//        {{
//            &ltr01
//        }}        
//    };
//    

//    void BSPltr::init(embot::hw::icc::LTR h) const
//    {
//    }            

//    const BSPltr & getBSPltr() 
//    {
//        return thebsp_ltr;
//    }      

//} // namespace embot::hw::icc::bsp {


//#endif // #elif defined(EMBOT_ENABLE_hw_icc)


//// - support map: end of embot::hw::icc


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

