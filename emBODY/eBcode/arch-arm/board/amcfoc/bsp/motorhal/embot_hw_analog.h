/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_ANALOG_H_
#define __EMBOT_HW_ANALOG_H_

#include "embot_core.h"
#include "embot_hw.h"


namespace embot::hw::analog {
        
    struct Configuration
    {
        uint32_t tbd {0};
        constexpr Configuration() = default;
        constexpr bool isvalid() const { return true; }
    };

    void init(const Configuration &config);
    void deinit();
    
    float getVin();
    float getCin();
    float getCoreTemp();
    float getDriver1Temp();
    float getDriver2Temp();
    float getVaux();
    float getVcc();
    float getVcore();
    
    void print_Analogs();
    void MaxMin_analog_reads();
    

} // namespace embot::hw::analog 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

