
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_ETH_BSP_H_
#define __EMBOT_HW_ETH_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"
#include "embot_hw_eth.h"
#include "embot_hw_gpio_bsp.h"
#include "ipal.h"

namespace embot { namespace hw { namespace eth { namespace bsp {
    
#if defined(HAL_ETH_MODULE_ENABLED)        
    using ETH_Handle = ETH_HandleTypeDef;
    using ETH_Device = ETH_TypeDef;
#else
    using ETH_Handle = void;
    using ETH_Device = void;
#endif
 
#if 1 

    struct PROP
    {
        ETH_Handle* handle {nullptr}; 
        constexpr PROP() = default;
        constexpr PROP(ETH_Handle *h) : handle(h) {} 
    };
       
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::EtH::maxnumberof);
        std::array<const PROP*, maxnumberof> properties {}; 
        
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {} 
        constexpr BSP() : SUPP(0), properties({0}) {}
               
        constexpr const PROP * getPROP(embot::hw::EtH h) const { return supported(h) ? properties[embot::core::tointegral(h)] : nullptr; }
        
//        // bool init(embot::hw::EtH h, const Config &config) const;
//        bool init(embot::hw::EtH h) const;
//        bool deinit(embot::hw::EtH h) const;
        
        void init(ipal_hal_eth_cfg_t *cfg) const;
        void enable() const;
        void disable() const;
        void sendframe(ipal_hal_eth_frame_t *frame) const;
        size_t get_frame_size() const;
        void get_frame(size_t length, uint8_t* frame) const; 
        uint64_t get_mac() const;    
        bool islinkup(embot::hw::PHY phy) const;
        uint64_t errors(embot::hw::PHY phy, embot::hw::eth::ERR e) const;        
        
    };
    
    const BSP& getBSP();
    
    // sadly I cannot put a ETH_TypeDef* inside a constexpr data staructure, so i need this funtion to get ETH1 / ETH2 etc
    ETH_Device* getDEVICE(embot::hw::EtH h);
    
#endif
    
}}}} //namespace embot { namespace hw { namespace eth { namespace bsp {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


