
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp.h"


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

#include "embot_hw_bsp_amc_config.h"



// - support map: begin of embot::hw::eth

#include "embot_hw_eth.h"
#include "embot_hw_eth_bsp.h"

#if   !defined(EMBOT_ENABLE_hw_eth)

namespace embot { namespace hw { namespace eth { namespace bsp {
    
    
}}}}

#else   // EMBOT_ENABLE_hw_eth

#include "ipal_hal_eth_stm32h7.h"
#include "embot_hw_chip_KSZ8563.h"
    
namespace embot { namespace hw { namespace eth { namespace bsp {
               
    #if   defined(STM32HAL_BOARD_AMC)
    
    // it has HAL_ETH_MODULE_ENABLED  
    // in here we add ....
    
    constexpr PROP eth1p = { &heth }; 
    
    embot::hw::chip::KSZ8563 *ethswitch {nullptr};
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(EtH::one),        
        // properties
        {{
            &eth1p            
        }}        
    }; 
    
    void BSP::init(ipal_hal_eth_cfg_t *cfg) const
    {
        if(nullptr != ethswitch)
        {   // already initted
            return;
        }
        ethswitch = new embot::hw::chip::KSZ8563;
        ethswitch->init({});
        ipal_hal_eth_stm32h7_init(cfg);
    }
    
    void BSP::enable() const
    {
        NVIC_EnableIRQ(ETH_IRQn); 
    }
    
    void BSP::disable() const
    {
        NVIC_DisableIRQ(ETH_IRQn);
    }
        
    void BSP::sendframe(ipal_hal_eth_frame_t *frame) const
    {
        ipal_hal_eth_stm32h7_sendframe(frame);
    }
    
    size_t BSP::get_frame_size() const
    {
        return ipal_hal_eth_stm32h7_get_frame_size();
    }
    
    void BSP::get_frame(size_t length, uint8_t* frame) const
    {
        ipal_hal_eth_stm32h7_get_frame(length, frame);
    }    

    uint64_t BSP::get_mac() const
    {
        return 0;
    }    

    static constexpr embot::hw::chip::KSZ8563::PHY phys[2] = 
    {
        embot::hw::chip::KSZ8563::PHY::one, embot::hw::chip::KSZ8563::PHY::two
    };
        
    bool BSP::islinkup(embot::hw::PHY phy) const
    {
        if(nullptr == ethswitch)
        {            
            return true;
        }
               
        embot::hw::chip::KSZ8563::Link lnk { embot::hw::chip::KSZ8563::Link::DOWN };
        ethswitch->read(phys[embot::core::tointegral(phy)], lnk);
        
        return (embot::hw::chip::KSZ8563::Link::UP == lnk) ? true : false;       
    }
    
    uint32_t BSP::errors(embot::hw::PHY phy, ERR e) const
    {
        if(nullptr == ethswitch)
        {            
            return 0;
        }    

        return 0; // so far
    }

    #else
        #error embot::hw::eth::thebsp must be defined    
    #endif

    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace eth { namespace bsp {
    

    #if defined(HAL_ETH_MODULE_ENABLED)

    extern "C"
    {
        void ETH_IRQHandler(void)
        {
            HAL_ETH_IRQHandler(&heth);
        }

        void ETH_WKUP_IRQHandler(void)
        {
            HAL_ETH_IRQHandler(&heth);
        }
    }
    #endif //#if defined(HAL_ETH_MODULE_ENABLED)
            
#endif // EMBOT_ENABLE_hw_eth


// - support map: end of embot::hw::eth


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

