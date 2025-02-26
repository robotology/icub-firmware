
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Simone Girardi
 * email:   simone.girardi@iit.it
 * Author:  Kevin Sangalli
 * email:   kevin.sangalli@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_CHIP_MA730_H_
#define __EMBOT_HW_CHIP_MA730_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_spi.h"
#include "embot_hw_gpio.h"

    
#if 0
## Description

### SSI
In this particular setup, `amc` and `amcfoc` so far, we are using the chip MA730 in SSI mode!
Also the datasheet tells that is it not a classic SSI, but has some particular configuration. 
From the datasheet:
- This sensor uses an SSI with a low SSCK idle state instead of the high SSCK idle state used in the standard protocol. 
- This sensor also requires a dummy rising edge on the SSCK signal at the beginning of the SSI frame, while the standard SSI does not.

In this code, we read the chip using a SPI bus configured in mode one to be compliant to the required chip functioning. 
Also we discard the first bit while reading the data.
The maximum frequency of the clock is 5MHz in SSI mode, the minimum frequency is 62.5kHz.


### SPI
In a future case in which the chip will be used in an SPI standard configuration, the chip ask for mode 0 or 3 for the SPI.
The maximum frequency for the clock is 25MHz.
Right now this is not implemented and not used here.


## Basic usage

You can use the following code, as long as the settings for SPI are specified by the bsp of the board.

```c++
#include "embot_hw_chip_MA730.h"

bool ok = embot::hw::chip::MA730::testof_MA730();

```

Code listing. Usage of the class


## Caveat Emptor

The interface of the device driver is kept intentionally simple and some features are left (for now) inside the private implementation.    

## References
[1] tbd

#endif    


namespace embot::hw::chip {
    
    class MA730
    {
      
    public:
        
        using POS = uint16_t;
        
        struct Status
        {
            uint8_t bits {0};   // TODO: try to use bitsfield C++
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
        
        //remember: we are using the chip in SSI mode!!
        //for SSI we use mode one but we discard the first bit (implemented in embot::hw::chip::MA730::Impl::read in embot_hw_chip_MA730.cpp)
        //with this prescaler we are driving the SPI clock to 781.2kHz. 
        //we are going "slow" because we are using the SPI to read a sensor external to the board and connected with a wire.
        static constexpr embot::hw::spi::Config standardspiconfig
        { 
            embot::hw::spi::Prescaler::onehundredtwentyeigth, 
            embot::hw::spi::DataSize::eight, 
            embot::hw::spi::Mode::one,
            { {embot::hw::gpio::Pull::pullup, embot::hw::gpio::Pull::nopull,      // | miso | mosi |
               embot::hw::gpio::Pull::pulldown, embot::hw::gpio::Pull::pullup} }  // | sclk | sel  |
        };
        
        MA730();
        ~MA730();

        bool isinitted() const;
        bool init(const Config &config);  
        bool deinit();
               
        bool read(Data &data, embot::core::relTime timeout);  
        bool read(Data &data, const embot::core::Callback &oncompletion);        

    private:        
        struct Impl;
        Impl *pImpl;    
    };
    
    
    } // namespace embot::hw::chip {


#define EMBOT_HW_CHIP_MA730_enable_test   
#if defined(EMBOT_HW_CHIP_MA730_enable_test)    
namespace embot { namespace hw { namespace chip {
    // it tests the chip and offers an example of use
    bool testof_MA730();
}}}
#endif



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

