
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_CHIP_M95512DF_H_
#define __EMBOT_HW_CHIP_M95512DF_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_spi.h"
#include "embot_hw_gpio.h"

    
#if 0
## Description
This class `embot::hw::chip::M95512DF` implements a device driver for the chip [M95512-DF](https://www.st.com/en/memories/m95512-df.html) which is an SPI EEPROM of size 64 KB and page size 128 B.

```
                Vcc
                 |
             ---------
            |         |
       D ---|         |
       C ---|         |
      nS --o| M95xxx  |--- Q
      nW --o|         |
   nHOLD --o|         |
            |         |
             --------- 
                |
               Vss

```
**Figure 1**. Logic diagram.



| Signal name | Function           | Direction | Description                                                  |
| ----------- | ------------------ | --------- | ------------------------------------------------------------ |
| C           | Serial Clock       | Input     |                                                              |
| D           | Serial data input  | Input     |                                                              |
| Q           | Serial data output | Output    |                                                              |
| nS          | Chip select        | Input     | When this input signal is high, the device is deselected. Driving Chip select low selects the device. |
| nW          | Write protect      | Input     | The main purpose of this input signal is to freeze the size of the area of memory that is protected against Write instructions. |
| nHOLD       | Hold               | Input     | Used to pause any serial communications with the device without deselecting the device |
| Vcc         | Supply voltage     | -         |                                                              |
| Vss         | Ground             | -         |                                                              |

**Table 1**. Signals.



## Basic usage

You can use the following code, as long as the settings for SPI are specified by the bsp of the board.

```c++
#include "embot_hw_chip_M95512DF.h"

bool ok = embot::hw::chip::M95512DF::testof_M95512DF();

```

Code listing. Usage of the class


## Caveat Emptor

The interface of the device driver is kept intentionally simple and some features are left (for now) inside the private implementation.    

## References
[1] Datasheet of M95512-W M95512-R M95512-DF, DS4192 - Rev 24 - September 2021

#endif    


namespace embot { namespace hw { namespace chip {
    
    class M95512DF
    {
      
    public:
        
        using ADR = uint16_t;
        static constexpr size_t storagesize {64*1024};
        static constexpr size_t pagelength {128};
        
        struct PinControl
        {   // contains: chip select, write protection and hold (see page 2 of [1]). if in the PCB they are not attached to a GPIO use dummy value {}
            embot::hw::GPIO nS {}; 
            embot::hw::GPIO nW {};
            embot::hw::GPIO nHOLD {}; 
            // it also has a config for the pins. 
            // if config.isvalid() then the M95512DF will also initialise the pin else ... just use them
            embot::hw::gpio::Config config {
                embot::hw::gpio::Mode::OUTPUTpushpull,
                embot::hw::gpio::Pull::nopull,
                embot::hw::gpio::Speed::veryhigh
            };                   
            constexpr PinControl() = default;
            constexpr PinControl(const embot::hw::GPIO& ns, const embot::hw::GPIO& nw, const embot::hw::GPIO& nhold, 
                                 const embot::hw::gpio::Config& cfg)
                : nS(ns), nW(nw), nHOLD(nhold), config(cfg) {}
            constexpr bool isvalid() const { 
                return embot::hw::gpio::supported(nW) && embot::hw::gpio::supported(nHOLD) && embot::hw::gpio::supported(nS); 
            }        
        };
                                
        struct Config
        {   // contains: spi bus and pin control            
            embot::hw::SPI spi {embot::hw::SPI::none};
            embot::hw::spi::Config spicfg {};
            PinControl pincontrol {};                
            constexpr Config() = default;
                constexpr Config(embot::hw::SPI s, const embot::hw::spi::Config &sc, const PinControl &pc) 
                : spi(s), spicfg(sc), pincontrol(pc) {}   
            constexpr bool isvalid() const { 
                return embot::hw::spi::supported(spi) && pincontrol.isvalid(); 
            }
        }; 
        
        M95512DF();
        ~M95512DF();

        bool isinitted() const;
        bool init(const Config &config);  
        bool deinit();
               
        bool read(ADR adr, embot::core::Data &data);        
        bool write(ADR adr, const embot::core::Data &data); 
        bool erase();
        bool erase(ADR adr, size_t size); 

    private:        
        struct Impl;
        Impl *pImpl;    
    };
    
    
}}} // namespace embot { namespace hw { namespace chip {


#undef EMBOT_HW_CHIP_M95512DF_enable_test   
#if defined(EMBOT_HW_CHIP_M95512DF_enable_test)    
namespace embot { namespace hw { namespace chip {
    // it tests the chip and offers an example of use
    bool testof_M95512DF();
}}}
#endif



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

