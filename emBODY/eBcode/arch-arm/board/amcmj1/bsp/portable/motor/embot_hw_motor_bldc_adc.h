
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_BLDC_ADC_H_
#define __EMBOT_HW_MOTOR_BLDC_ADC_H_


#include "embot_hw_motor_bldc.h"


namespace embot::hw::motor::bldc::adc {
    
    struct Configuration
    {
        enum class ACQUISITION { immediate };

        ACQUISITION acquisition { ACQUISITION::immediate };    
        embot::hw::motor::bldc::OnCurrents oncurrents {};
        
        constexpr Configuration() = default;
        constexpr Configuration(const embot::hw::motor::bldc::OnCurrents& o) : oncurrents(o) {}
        constexpr bool isvalid() const { return true; }
    };

    struct Calibration
    {
        enum class CALIBRATION { none, current };

        CALIBRATION calibration {CALIBRATION::current};
        embot::core::relTime timeout {400*embot::core::time1millisec};
        
        constexpr Calibration() = default;
        constexpr Calibration(CALIBRATION &c, embot::core::relTime t) : calibration(c), timeout(t) {}

        constexpr bool isvalid() const { return true; }
    };

    // -- public interfaces to be used in embot::hw::motor

    // init() starts ADC acquisition straight away (ACQUISITION::immediate). 


    bool init(embot::hw::MOTOR m, const Configuration &config);
    bool deinit(embot::hw::MOTOR m);
    bool calibrate(embot::hw::MOTOR m, const Calibration &calib);
    bool set(embot::hw::MOTOR m, const embot::hw::motor::bldc::OnCurrents &cbk);
                   
} // namespace embot::hw::motor::bldc::adc {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

