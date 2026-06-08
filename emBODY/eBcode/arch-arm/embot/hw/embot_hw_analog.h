
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_ANALOG_H_
#define __EMBOT_HW_ANALOG_H_

#include "embot_core.h"
#include "embot_hw.h"

#define EMBOT_HW_ANALOG_removelegacyAPI

namespace embot::hw::analog {
    
    enum class Value : uint8_t { Vin = 0, Cin = 1, Tcore = 2, Vaux = 3, Vcc = 4, Vcore = 5, Tmotordriver = 6, Tmotor = 7, maxnumberof = 8 };
        
    struct Configuration
    {
        uint32_t tbd {0};
        constexpr Configuration() = default;
        constexpr bool isvalid() const { return true; }
    };

    // init() is not done twice unless a deinit() is called.
    // so, it is possible to init(), deinit() and then init() again
    void init(const Configuration &config);
    void deinit();  
    
    // m is used only for Value::Tmotordriver and Value::Tmotor    
    bool supported(const Value v, embot::hw::MOTOR m = embot::hw::MOTOR::none);
    
    // m is used only for Value::Tmotordriver and Value::Tmotor
    float get(const Value v, embot::hw::MOTOR m = embot::hw::MOTOR::none);    

#if defined(EMBOT_HW_ANALOG_removelegacyAPI)
    float getVin();
    float getCin();
    float getCoreTemp();
    float getDriver1Temp();
    float getDriver2Temp();
    float getVaux();
    float getVcc();
    float getVcore();
#endif

} // namespace embot::hw::analog 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

