
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_BLDC_EXTFAULT_BSP_H_
#define __EMBOT_HW_MOTOR_BLDC_EXTFAULT_BSP_H_


#include "embot_hw_motor_bldc_extfault.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"
#include "embot_hw_button.h"
#include "embot_hw_led.h"
#include <array>

namespace embot::hw::motor::bldc::extfault::bsp {
    
    static const size_t numofhallsensors {3};

    struct PROP
    {     
        embot::hw::BTN btn {embot::hw::BTN::none};
        embot::hw::LED led {embot::hw::LED::none};
        constexpr PROP() = default;
        constexpr PROP(const embot::hw::BTN &b, const embot::hw::LED &l) : btn(b), led(l) {}
    };    
            
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = 1;
        constexpr BSP(std::uint32_t msk, const PROP* p) : SUPP(msk), prop(p) {}
        constexpr BSP() : SUPP(0), prop(nullptr) {}            

        const PROP* prop {};    
            
        bool init() const;
        bool deinit() const; 
        bool pressed() const;    

        embot::hw::BTN btn() const;        
        embot::hw::LED led() const;  
    };
    
    const BSP& getBSP();   
    
                      
} // namespace embot::hw::motor::bldc::extfault::bsp {





#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

