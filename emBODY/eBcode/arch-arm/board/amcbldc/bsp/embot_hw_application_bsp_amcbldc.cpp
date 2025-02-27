/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Kevin Sangalli
 * email:   kevin.sangalli@iit.it
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
#include "embot_hw_sys.h"

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

#include "embot_hw_bsp_amcbldc_config.h"

// --------------------------------------------------------------------------------------------------------------------
// - board specific methods
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_amcbldc.h"

#include "analog.h"

namespace embot::hw::bsp::amcbldc {
    
    #include <tuple>
    #include <array>
    
#if 0
//std::make_tuple(0x00460048, 0x484E500E, 0x20343356) // scheda centrale sul wrist setup (etichettata come scheda numero 2)
//std::make_tuple(0x00470047, 0x484E500E, 0x20343356) // scheda sul wrist setup etichettata come scheda numero 3
//std::make_tuple(0x00480022, 0x484E500E, 0x20343356) // scheda sul wrist setup etichettata come scheda numero 1
std::make_tuple(0x00470027, 0x484E500E, 0x20343356 ) // scheda jointlab camozzi
    
    0x00470045, 0x484E500E, 0x20343356 labeled as A1. caveat: the fault is broken
#endif
    
    constexpr std::array< std::tuple<uint32_t, uint32_t, uint32_t>, 7> theUIDsOfRevA =    
    {
        std::make_tuple(0x00470045, 0x484E500E, 0x20343356),    // 01  probably broken
        std::make_tuple(0x00460046, 0x484E500E, 0x20343356),    // 02  probably broken
        std::make_tuple(0x00470047, 0x484E500E, 0x20343356),    // 03: scheda sul wrist setup etichettata come scheda numero 3
        std::make_tuple(0x00480022, 0x484E500E, 0x20343356),    // scheda sul wrist setup etichettata come scheda numero 1 
        std::make_tuple(0x00470027, 0x484E500E, 0x20343356),    // jointlab camozzi 
        std::make_tuple(0x00460048, 0x484E500E, 0x20343356),    // scheda centrale sul wrist setup (etichettata come scheda numero 2)
        std::make_tuple(0x00470045, 0x484E500E, 0x20343356)     // labeled as A1 in red ink. caveat: the fault is broken
    };    

#if 0    
    constexpr std::array< std::tuple<uint32_t, uint32_t, uint32_t>, 10> theUIDsOfRevB =    
    {
        std::make_tuple(0x00400041, 0x4650500F, 0x20313432),    // 01
        std::make_tuple(0x003F0046, 0x4650500F, 0x20313432),    // 02
        std::make_tuple(0x00400034, 0x4650500F, 0x20313432),    // 03
        std::make_tuple(0x003E004F, 0x4650500F, 0x20313432),    // 04 
        std::make_tuple(0x003F0042, 0x4650500F, 0x20313432),    // 05
        std::make_tuple(0x0040002C, 0x4650500F, 0x20313432),    // 06
        std::make_tuple(0x003F0029, 0x4650500F, 0x20313432),    // 07
        std::make_tuple(0x003F0027, 0x4650500F, 0x20313432),    // 08
        std::make_tuple(0x00400032, 0x4650500F, 0x20313432),    // 09
        std::make_tuple(0x00400043, 0x4650500F, 0x20313432)     // 10
    };
#endif
    
    
    Revision revision()
    {        
        static Revision revision {Revision::none};
        
        if(Revision::none == revision)
        {        
            revision = Revision::B;            
            volatile uint32_t *myuID = ((volatile uint32_t *)(UID_BASE));
            auto target = std::make_tuple(myuID[0], myuID[1], myuID[2]);
            for(const auto &i : theUIDsOfRevA)
            {
                if(i == target)
                {
                    revision =  Revision::A;
                }
            }
        }                
        
        return revision;
    }
    
    embot::hw::BTN EXTFAULTbutton()
    {
        return (Revision::A == revision()) ? embot::hw::BTN::one : embot::hw::BTN::two;
    }
    
    float getVIN()
    {
        float v {0.0};
        
        v = analogVin() * 0.001;
        return v;
        
    }

    float getCIN()
    {
        float c {0.0};
        
        c = analogCin() * 0.001;
        return c;
        
    }

}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

