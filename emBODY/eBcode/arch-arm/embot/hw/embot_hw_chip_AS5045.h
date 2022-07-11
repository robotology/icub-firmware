
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_CHIP_AS5045_H_
#define __EMBOT_HW_CHIP_AS5045_H_

#include "embot_hw_spi.h"

#if 0
## Description

tbd



## Basic usage

You can use the following code, as long as the settings for SPI are specified by the bsp of the board.

```c++
#include "embot_hw_chip_AS5045.h"

bool ok = embot::hw::chip::AS5045::testof_AS5045();

```

Code listing. Usage of the class


## Caveat Emptor

The interface of the device driver is kept intentionally simple and some features are left (for now) inside the private implementation.

## References
[1] tbd

#endif


namespace embot { namespace hw { namespace chip {
    
    class AS5045
    {
      
    public:
        
        using POS = uint16_t;
        
        enum class Parity { even, odd };
        
        struct Status
        {
            uint8_t bits {0};   // TODO: try to use bitsfield C++
            Parity parity {};
            bool ok {false};
            Status() = default;
        };
        
        struct Data
        {
            POS position {0};
            Status status {};
            Data() = default;
            bool isvalid() const { return status.ok; }
        };
        
        struct Config
        {   // contains: spi bus and ... tbd
            embot::hw::SPI spi {embot::hw::SPI::none};
            embot::hw::spi::Config spicfg {};
            constexpr Config() = default;
            constexpr Config(embot::hw::SPI s, const embot::hw::spi::Config &sc) 
                : spi(s), spicfg(sc) {}
            constexpr bool isvalid() const {
                return embot::hw::spi::supported(spi);
            }
        };
        
        static constexpr embot::hw::spi::Config standardspiconfig
        {
            embot::hw::spi::Prescaler::sixtyfour,
            embot::hw::spi::DataSize::eight,
            embot::hw::spi::Mode::two,
            { {embot::hw::gpio::Pull::pullup, embot::hw::gpio::Pull::nopull,      // | miso | mosi |
               embot::hw::gpio::Pull::pulldown, embot::hw::gpio::Pull::pullup} }  // | sclk | sel  |
        };
        
        //static constexpr embot::hw::spi::Config  & getSPIconfig() const;
        
        AS5045();
        ~AS5045();

        bool isinitted() const;
        bool init(const Config &config);
        bool deinit();
        
        bool read(Data &data, embot::core::relTime timeout);
        bool read(Data &data, const embot::core::Callback &oncompletion);
        
    private:
        struct Impl;
        Impl *pImpl;
        POS position;
    };
}}} // namespace embot { namespace hw { namespace chip {


#define EMBOT_HW_CHIP_AS5045_enable_test
#if defined(EMBOT_HW_CHIP_AS5045_enable_test)
namespace embot { namespace hw { namespace chip {
    // it tests the chip and offers an example of use
    bool testof_AS5045();
}}}
#endif



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

