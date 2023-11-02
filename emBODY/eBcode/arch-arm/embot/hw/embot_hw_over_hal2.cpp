
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_types.h"
#include "embot_core_binary.h"
#include "embot_core.h"

#include "embot_hw_bsp_config.h"

#if defined(USE_STM32HAL) && defined(USE_hal2_DRIVER)
    #include "stm32hal.h" 
#else
    #warning this implementation is only for stm32hal over hal2 driver
#endif

#warning THIS is a WIP file in which we implement some embot::hw drivers using hal2


using namespace std;
using namespace embot::core::binary;

// --------------------------------------------------------------------------------------------------------------------
// - definition of public functions 
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_can.h"
#include "embot_hw_can_bsp.h"

#if !defined(EMBOT_ENABLE_hw_can)

namespace embot::hw::can {
    
    bool supported(embot::hw::CAN p) { return false; }    
    bool initialised(embot::hw::CAN p) { return false; }    
    result_t init(embot::hw::CAN p, const Config &config) { return resNOK; }    
    result_t enable(embot::hw::CAN p)  { return resNOK; }    
    result_t disable(embot::hw::CAN p)  { return resNOK; }    
    result_t put(embot::hw::CAN p, const Frame &frame)  { return resNOK; }    
    std::uint8_t outputqueuesize(embot::hw::CAN p)  { return 0; }    
    result_t transmit(embot::hw::CAN p)  { return resNOK; }   
    std::uint8_t inputqueuesize(embot::hw::CAN p)  { return 0; }    
    result_t get(embot::hw::CAN p, Frame &frame, std::uint8_t &remaining)  { return resNOK; }  
    
}

#else

namespace embot::hw::can {
    
    constexpr hal_can_t convert(embot::hw::CAN c)
    { 
        return static_cast<hal_can_t>(c); 
    };
    
    constexpr embot::hw::result_t result(hal_result_t r)
    { 
        return (hal_res_OK == r) ? embot::hw::resOK : embot::hw::resNOK;
    }

    constexpr hal_boolval_t halbool(bool b)
    { 
        return (b) ? hal_true : hal_false;
    }    
    
    void fill(hal_can_cfg_t &can_cfg, const Config &config, embot::hw::CAN p)
    {
        
    }
    
    static std::uint32_t initialisedmask = 0;
    
    bool supported(embot::hw::CAN p)
    {
        return embot::hw::can::getBSP().supported(p);
    }

    bool initialised(embot::hw::CAN p)
    {
        return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(p));
    } 
    

    result_t init(embot::hw::CAN p, const Config &config)
    {
        if(false == supported(p))
        {
            return resNOK;
        }
        
        if(true == initialised(p))
        {
            return resOK;
        }  

        hal_can_cfg_t cfg {};
        fill(cfg, config, p);
            
        if(hal_res_OK != hal_can_init(convert(p), &cfg))
        {
            return resNOK;
        }
               
        std::uint8_t index = embot::core::tointegral(p);        
        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(p));

        return resOK;
    }

    result_t enable(embot::hw::CAN p)  
    { 
        if(false == initialised(p))
        {
            return resNOK;
        }          
        return result(hal_can_enable(convert(p))); 
    }

    
    result_t disable(embot::hw::CAN p)
    { 
        if(false == initialised(p))
        {
            return resNOK;
        }          
        return result(hal_can_disable(convert(p))); 
    }
    
    result_t put(embot::hw::CAN p, const Frame &frame)
    {
        if(false == initialised(p))
        {
            return resNOK;
        } 
        
        return resNOK;
    }
    
    std::uint8_t outputqueuesize(embot::hw::CAN p)  { return 0; }    
    result_t transmit(embot::hw::CAN p)  { return resNOK; }   
    std::uint8_t inputqueuesize(embot::hw::CAN p)  { return 0; }    
    result_t get(embot::hw::CAN p, Frame &frame, std::uint8_t &remaining)  { return resNOK; }  
    
}



#endif




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

