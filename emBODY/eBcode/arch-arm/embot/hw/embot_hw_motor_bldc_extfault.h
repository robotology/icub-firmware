
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_BLDC_EXTFAULT_H_
#define __EMBOT_HW_MOTOR_BLDC_EXTFAULT_H_


#include "embot_hw_motor_bldc.h"
#include "embot_hw_types.h"

namespace embot::hw::motor::bldc::extfault {
        
    struct Configuration
    {
        uint32_t dummy {0};
        constexpr Configuration() = default;
        constexpr bool isvalid() const { return true; }
    };

    bool init(const Configuration &config);
    bool deinit();
    bool initialised();
    
    bool pressed();
    
    embot::hw::LED led();

} // namespace embot::hw::motor::bldc::extfault {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

