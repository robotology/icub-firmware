
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Kevin Sangalli
 * email:   marco.accame@iit.it, kevin.sanngalli@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor_bldc_extfault_bsp.h"

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



#if defined(EMBOT_ENABLE_hw_motor_bldc_extfault)


#if defined(EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_motor_bldc_extfault)

namespace embot::hw::motor::bldc::extfault::bsp {
    
    constexpr BSP thebsp { };   
    
    bool BSP::init() const 
    {
        return false;       
    } 

    bool BSP::deinit() const 
    {
        return false;       
    } 
    
    bool BSP::pressed() const
    {     
        return false;
    }     

    embot::hw::BTN BSP::btn() const
    {     
        return embot::hw::BTN::none;
    }  
    
    embot::hw::LED BSP::led() const
    {
        return embot::hw::LED::none;
    }
    
    const BSP& getBSP() 
    {
        return thebsp;
    }

} // namespace embot::hw::motor::bldc::extfault::bsp 



#elif !defined(EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_motor_bldc_extfault)
    

namespace embot::hw::motor::bldc::extfault::bsp {
        
    constexpr uint32_t supportedmask = 1;
    
    constexpr PROP p { embot::hw::BTN::one, embot::hw::LED::one };
    
    constexpr BSP thebsp { supportedmask, &p };   

    bool BSP::init() const 
    {
        embot::hw::led::init(p.led);
        embot::hw::button::init(p.btn, {embot::hw::button::Mode::Polling, {}, 0});
        return true;
    } 

    bool BSP::deinit() const 
    {
//        embot::hw::led::deinit(p.led);
//        embot::hw::button::deinit(p.btn);
        return true;   
    } 
    
    bool BSP::pressed() const
    {     
        return embot::hw::button::pressed(p.btn);
    }     

    embot::hw::BTN BSP::btn() const
    {     
        return p.btn;
    }  
    
    embot::hw::LED BSP::led() const
    {
        return p.led;
    }
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
     
    
} // namespace embot::hw::motor::bldc::extfault::bsp {    


#endif // #elif !defined(EMBOT_HW_MOTOR_BLDC_board_use_fake_implementation__hw_motor_bldc_extfault)



#endif // #if defined(EMBOT_ENABLE_hw_analog)





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

