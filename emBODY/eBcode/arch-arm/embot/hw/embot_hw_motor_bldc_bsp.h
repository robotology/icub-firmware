
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_BLDC_BSP_H_
#define __EMBOT_HW_MOTOR_BLDC_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"
#include "embot_hw_motor_bldc.h"

namespace embot::hw::motor::bldc::bsp { 
    
    struct PROP
    { 
        uint32_t tbd {0};
    };
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::hw::motor::bldc::MAXnumber;
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {} 
        constexpr BSP() : SUPP(0), properties({0}) {}
            
        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::MOTOR m) const { return supported(m) ? properties[embot::core::tointegral(m)] : nullptr; }
        
        // it prepares the HW peripherals
        bool init(embot::hw::MOTOR m) const;
        // and it deinit them
        bool deinit(embot::hw::MOTOR m) const; 
        
        // the bldc motor much depends on the board, so in here we duplicate most of the functionalities that depend on the board
        
        bool configure(embot::hw::MOTOR m, const Config &config) const;
        bool enable(MOTOR m, bool on) const;
        //bool enabled(MOTOR m) const;
        
        bool fault(MOTOR m, bool on) const;
        //bool faulted(MOTOR m) const;

        bool set(MOTOR m, const embot::hw::motor::bldc::OnCurrents &oncurrents) const;  
        
        HallStatus hall(MOTOR m) const;
        Angle angle(MOTOR m, Encoder enc) const; 
        
        bool set(MOTOR m, const PWM3 &pwm) const;
        Voltage powersupply(MOTOR m) const;        
        
    };
    
    const BSP& getBSP();
                                     
        
} // namespace embot::hw::motor::bldc::bsp { 



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


