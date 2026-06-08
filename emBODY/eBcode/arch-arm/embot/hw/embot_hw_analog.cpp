
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Kevin Sangalli
 * email:   marco.accame@iit.it, kevin.sanngalli@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_analog.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - configuration of peripherals and chips. it is done board by board. it contains a check vs correct STM32HAL_BOARD_*
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


#if !defined(EMBOT_ENABLE_hw_analog)

#warning EMBOT_ENABLE_hw_analog is undefined, so we use dummy implementation 

namespace embot::hw::analog {

    void init(const Configuration &config) {}    
    void deinit() {}        
    bool supported(const Value v, embot::hw::MOTOR m) { return false; }    
    float get(const Value v, embot::hw::MOTOR m)  { return 0.0; } 

#if defined(EMBOT_HW_ANALOG_removelegacyAPI)
    float getVin() { return 0.0; }
    float getCin() { return 0.0; }
    float getCoreTemp() { return 0.0; }
    float getDriver1Temp() { return 0.0; }
    float getDriver2Temp() { return 0.0; }
    float getVaux()  { return 0.0; }
    float getVcc()   { return 0.0; }
    float getVcore() { return 0.0; }    
#endif // #if defined(EMBOT_HW_ANALOG_removelegacyAPI)

}

#elif defined(EMBOT_ENABLE_hw_analog)  

#include "embot_hw_analog_bsp.h"

namespace embot::hw::analog {
    
    bool initted {false};  
    
    void init(const Configuration &config)
    {                        
        if(false == initted)
        {
            embot::hw::analog::bsp::getBSP().init(config);
            initted = true;
        } 
    }

    void deinit()
    {
        embot::hw::analog::bsp::getBSP().deinit();
        initted = false;
    }
    
    bool supported(const Value v, embot::hw::MOTOR m) 
    {
        return embot::hw::analog::bsp::getBSP().supported(v, m);        
    }
    
    float get(const Value v, embot::hw::MOTOR m)
    {
        return embot::hw::analog::bsp::getBSP().get(v, m);
    }

#if defined(EMBOT_HW_ANALOG_removelegacyAPI)
    
    float getVin()
    {
        constexpr Value v {Value::Vin};
        return embot::hw::analog::bsp::getBSP().get(v);
    }

    float getCin()
    { 
        constexpr Value v {Value::Cin};
        return embot::hw::analog::bsp::getBSP().get(v);     
    }
    
    float getCoreTemp()
    {
        constexpr Value v {Value::Tcore};
        return embot::hw::analog::bsp::getBSP().get(v);       
    }
        
    float getDriver1Temp()
    {
        constexpr Value v {Value::Tmotordriver};
        return embot::hw::analog::bsp::getBSP().get(v, embot::hw::MOTOR::one);
    }
    
    float getDriver2Temp()
    {
        constexpr Value v {Value::Tmotordriver};
        return embot::hw::analog::bsp::getBSP().get(v, embot::hw::MOTOR::two);
    }
    
    float getVaux()
    {
        constexpr Value v {Value::Vaux};
        return embot::hw::analog::bsp::getBSP().get(v);
    }
    
    float getVcc()
    {
        constexpr Value v {Value::Vcc};
        return embot::hw::analog::bsp::getBSP().get(v);
    }
    
    float getVcore()
    {
        constexpr Value v {Value::Vcore};
        return embot::hw::analog::bsp::getBSP().get(v);
    } 
    
#endif // #if defined(EMBOT_HW_ANALOG_removelegacyAPI)
    
} // namespace embot::hw::analog {

 
#endif // #if defined(EMBOT_ENABLE_hw_analog)





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

