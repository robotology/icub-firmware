
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor_bldc_extfault.h"


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


#if !defined(EMBOT_ENABLE_hw_motor_bldc_extfault)

#warning EMBOT_ENABLE_hw_motor_bldc_fault is undefined, so we use dummy implementation 

namespace embot::hw::motor::bldc::extfault {
    
    bool init(const Configuration &config) { return true; }
    bool deinit() { return true; }
    bool initialized() { return true; }
    bool pressed() { return false; }
    embot::hw::LED led() { return embot::hw::LED::none; }
    
}

#elif defined(EMBOT_ENABLE_hw_motor_bldc_extfault)


#include "embot_hw_motor_bldc_extfault_bsp.h"


namespace embot::hw::motor::bldc::extfault {

    bool _initialised {false};
    
    bool init(const Configuration &config)
    {         
        _initialised = embot::hw::motor::bldc::extfault::bsp::getBSP().init();
        return _initialised;     
    }

    bool deinit()
    {
        embot::hw::motor::bldc::extfault::bsp::getBSP().deinit();
        _initialised = false;        
        return true;     
    }
    
    bool initialised()
    {
        return _initialised;
    }      

    bool pressed()
    {
        if(false == _initialised)
        {
            return false;
        }
        
        return embot::hw::motor::bldc::extfault::bsp::getBSP().pressed();        
    }
    
    embot::hw::LED led() 
    {     
        return embot::hw::motor::bldc::extfault::bsp::getBSP().led();       
    }
 

} // namespace embot::hw::motor::extfault {


#endif // #elif defined(EMBOT_ENABLE_hw_motor_extfault)

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

