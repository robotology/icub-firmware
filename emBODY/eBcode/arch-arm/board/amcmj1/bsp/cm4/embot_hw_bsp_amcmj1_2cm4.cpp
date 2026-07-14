
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>
#include <array>

#include "embot_core_binary.h"
#include "embot_core.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif


using namespace std;
using namespace embot::core::binary;

// --------------------------------------------------------------------------------------------------------------------
// - configuration of peripherals and chips. it is done board by board. it contains a check vs correct STM32HAL_BOARD_*
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"


// --------------------------------------------------------------------------------------------------------------------
// - support maps
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - specialize the bsp
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_amcmj1_2cm4.h"

#include "embot_hw_eeprom.h"
#include "embot_hw_can.h"
#include "embot_hw_sys.h"
#include "embot_hw_gpio.h"


#if     !defined(EMBOT_ENABLE_hw_bsp_specialize)
bool embot::hw::bsp::specialize() { return true; }
#else   

bool embot::hw::bsp::specialize()
{
#if defined(EMBOT_ENABLE_hw_eeprom)    
    embot::hw::eeprom::init(embot::hw::EEPROM::one, {});
#endif
    
#if defined(EMBOT_ENABLE_hw_can_5V) //embot::can::init enables the 5V line, but if we need it before calling it we can use this macro
        
    constexpr embot::hw::GPIO candrivergpiovauxen = 
            {embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::thirteen};    // PWR_VAUXEN_GPIO_Port, PWR_VAUXEN_Pin
            
    constexpr embot::hw::gpio::Config cfgvauxen {
            embot::hw::gpio::Mode::OUTPUTpushpull, 
            embot::hw::gpio::Pull::pulldown, 
            embot::hw::gpio::Speed::low };
    
    embot::hw::gpio::init(candrivergpiovauxen, cfgvauxen);
    embot::hw::gpio::set(candrivergpiovauxen, embot::hw::gpio::State::SET);
    
#endif

    return true;  
}    
    
#endif

extern "C"
{


}    

#if defined(EMBOT_REDEFINE_hw_bsp_DRIVER_uniqueid)

#include "embot_hw_sys.h"
#include "embot_hw_eeprom.h"

constexpr embot::hw::eeprom::ADR uidloc {4096};
uint64_t embot::hw::sys::uniqueid()
{
    static uint64_t val {0};
    
    embot::core::Data d {&val, 8};
    embot::hw::eeprom::read(embot::hw::EEPROM::one, uidloc, d, 10*embot::core::time1millisec);
    
    return val;    
}

bool embot::hw::sys::setuniqueid(uint64_t v)
{
    bool r {true};

    embot::core::Data d {&v, 8};
    embot::hw::eeprom::write(embot::hw::EEPROM::one, uidloc, d, 10*embot::core::time1millisec);

    return r;
}


#endif


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

