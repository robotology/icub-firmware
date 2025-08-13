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
    float getMot1Temp();
    float getMot2Temp();
    
    
        
    /* ADC related constants */
    constexpr float    ADC_VREF = 2.5;     /*V*/
    constexpr uint8_t  ADC_NUMBER_OF_BITS = 12;
    constexpr uint16_t ADC_RESOLUTION = (1U << ADC_NUMBER_OF_BITS);
    constexpr float    ADC_LSB = ADC_VREF / static_cast<float>(ADC_RESOLUTION);
    constexpr uint8_t  NUMBER_OF_ADC3_CHANNELS = 10;
    
    
    //values taken from stm32h7xx_ll_adc.h
    #define TS_CAL1                     *TEMPSENSOR_CAL1_ADDR    
    #define TS_CAL2                     *TEMPSENSOR_CAL2_ADDR    
    
    
#if 0 
    //These limits are not used right now
    /* Currents, voltages and temperature limits */
    #define AIN_MAX_INPUT_CURRENT   (0.400)
    #define AIN_MAX_INPUT_VOLTAGE   (58.0)
    #define AIN_MIN_INPUT_VOLTAGE   (9.5)
    #define AIN_MAX_VCORE           (1.21*1.05)
    #define AIN_MIN_VCORE           (1.21*0.95)
    #define AIN_MAX_VCC             (3.33*1.05)
    #define AIN_MIN_VCC             (3.33*0.95)
    #define AIN_MAX_VAUX            (5.00*1.05)
    #define AIN_MIN_VAUX            (5.00*0.95)
    #define AIN_MAX_CORE_TEMP       (125.0)
    #define AIN_MAX_PTC1_TEMP       (90.0)
    #define AIN_MAX_PTC2_TEMP       (90.0)
#endif

       
    /* AMC-FOC schematics related constants */
    constexpr float R73  = 33000.0;
    constexpr float R84  = 22000.0;
    constexpr float R85  = 22000.0;
    constexpr float R74  = 33000.0;
    constexpr float R139 = 560000.0;
    constexpr float R75  = 24000.0;
    constexpr float R140 = 0.1;
    constexpr float U18_GAIN  = 20.0;
    constexpr float VAUX_ATTEN  = ((R73+R84)/R84);
    constexpr float VCC_ATTEN   = ((R85+R74)/R74);
    constexpr float VCORE_ATTEN = 1.0;
    constexpr float VIN_ATTEN   = ((R139+R75)/R75);
    constexpr float CIN_ATTEN   = (1.0/(R140*U18_GAIN));
    constexpr float PTC_GAIN    = (132.0/2.5);
    constexpr float PTC_OFFS    = (-12.0);

        
    enum class Ain3Channels
    {
        PTC1,
        PTC2,
        CIN,
        VIN,
        VAUX,
        VCC,
        VCORE,
        VBAT,
        VREFINT,
        TEMP
    };
    
    
    
    

} // namespace embot::hw::analog {

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

