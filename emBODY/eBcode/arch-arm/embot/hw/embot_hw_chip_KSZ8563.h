
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_CHIP_KSZ8563_H_
#define __EMBOT_HW_CHIP_KSZ8563_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_spi.h"
#include "embot_hw_gpio.h"

    
#if 0
## Description

tbd



## Basic usage

You can use the following code, as long as the settings for SPI are specified by the bsp of the board.

```c++
#include "embot_hw_chip_KSZ8563.h"

bool ok = embot::hw::chip::KSZ8563::testof_KSZ8563();

```

Code listing. Usage of the class


## Caveat Emptor

The interface of the device driver is kept intentionally simple and some features are left (for now) inside the private implementation.    

## References
[1] tbd

#endif    


namespace embot { namespace hw { namespace chip {
    
    class KSZ8563
    {
      
    public:
           
        enum class PHY : uint8_t { one = 0, two = 1};
        constexpr static size_t numberofPHYs {2};
        static constexpr PHY J6phy {PHY::one};
        static constexpr PHY J7phy {PHY::two};

        enum class Link : uint8_t { DOWN = 0, UP = 1 }; 
        
        struct PinControl
        {   // contains: chip select
            embot::hw::GPIO nS {embot::hw::GPIO::PORT::F, embot::hw::GPIO::PIN::six}; 
            embot::hw::gpio::Config config {
                embot::hw::gpio::Mode::OUTPUTpushpull,
                embot::hw::gpio::Pull::nopull,
                embot::hw::gpio::Speed::veryhigh
            };                   
            constexpr PinControl() = default;
            constexpr PinControl(const embot::hw::GPIO& ns, const embot::hw::gpio::Config& cfg)
                : nS(ns), config(cfg) {}
            constexpr bool isvalid() const { 
                return embot::hw::gpio::supported(nS); 
            }        
        };
        
        struct Config
        {   // contains: spi bus and ... tbd            
            embot::hw::SPI spi {embot::hw::SPI::five};
            embot::hw::spi::Config spicfg {}; // w/ Mode::zero
            PinControl pincontrol {};
            constexpr Config() = default;
            constexpr Config(embot::hw::SPI s, const embot::hw::spi::Config &sc, const PinControl &pc) 
                : spi(s), spicfg(sc), pincontrol(pc) {}   
            constexpr bool isvalid() const { 
                return embot::hw::spi::supported(spi); 
            }
        }; 
        
        KSZ8563();
        ~KSZ8563();

        bool isinitted() const;
        bool init(const Config &config);  
        bool deinit();
               
        bool read(PHY phy, Link &link, embot::core::relTime timeout = 5*embot::core::time1millisec);  

    private:        
        struct Impl;
        Impl *pImpl;    
    };
    
    
}}} // namespace embot { namespace hw { namespace chip {


//#define EMBOT_HW_CHIP_KSZ8563_enable_test   
#if defined(EMBOT_HW_CHIP_KSZ8563_enable_test)    
namespace embot { namespace hw { namespace chip {
    // it tests the chip and offers an example of use
    bool testof_KSZ8563();
}}}
#endif



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

