
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


## References
[1] tbd

#endif    


namespace embot::hw::chip {
    
    class KSZ8563
    {
      
    public:
           
        // the PHYs are where we can attach a ... eth cable 
        
        enum class PHY : uint8_t { one = 0, two = 1};
        constexpr static size_t numberofPHYs {2};
        static constexpr PHY J6phy {PHY::one};
        static constexpr PHY J7phy {PHY::two};
        // we can read the link (status) on a PHY
        enum class Link : uint8_t { DOWN = 0, UP = 1 }; 

        // the ports are the two PHYs plus the RMII / MII connection
        enum class PORT : uint8_t { one = 0, two = 1, three = 2 };

        constexpr static size_t numberofPORTs {3};
        // we can read the MIB (Management Information Base) counters for each port
        enum class MIB : uint8_t { RxCRCerror = 0x06, RxUnicast = 0x0C, RxByteCnt = 0x80 , TxByteCnt =0x81};
        enum class REG_MIB : uint8_t { REG_MIB_CSR = 0x0000, REG_MIB_DR  = 0x0004};  /* 32 bits  - MIB Control and Status Register, 32 bits  - MIB Data Register */
        enum class COMMAND : uint8_t { WRITE = 2, READ  = 3};
        static constexpr uint8_t MIB_CSR_READ_ENABLE_POS = 25;
        static constexpr uint8_t MIB_INDEX_POS = 16;
        static constexpr uint32_t MIB_CSR_COUNTER_VALID = 1<<25;
        static constexpr uint32_t MIB_CSR_OVERFLOW = 1<<31;

        struct MIBdata
        {
            enum class Size : uint8_t { bits30 = 0, bits36 = 1 };
            bool overflow {false};
            Size size {Size::bits30}; 
            uint32_t v32 {0}; 
            uint8_t v8 {0};
            uint8_t filler {0};
            constexpr MIBdata() = default;
            constexpr MIBdata(Size s, bool ov, uint32_t mibcountervalue, uint8_t nibble9)
            {
                load(s, ov, mibcountervalue, nibble9);
            }
            constexpr void load(Size s, bool ov, uint32_t mibcountervalue, uint8_t nibble9)
            {
                size = s; overflow = ov;
                // constexpr static uint32_t mask30 {0x3fffffff};
                // i am not sure if the hw register really has 30 bits, so better not to risk masking bits 31 and 30 off
                v32 = (Size::bits30 == s) ? (0xffffffff & mibcountervalue) : (mibcountervalue);
                v8 = (Size::bits30 == s) ? (0) : (0x0f & nibble9);
            }
            uint64_t value() const { return (static_cast<uint64_t>(v8) << 32) | v32; }
            
                
        };
        
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
        
        
        static constexpr embot::hw::spi::Config standardspiconfig
        {
            embot::hw::spi::Prescaler::eight,
            embot::hw::spi::DataSize::eight,
            embot::hw::spi::Mode::zero,
            { {embot::hw::gpio::Pull::nopull, embot::hw::gpio::Pull::nopull,    // | miso | mosi |
               embot::hw::gpio::Pull::nopull, embot::hw::gpio::Pull::nopull} }  // | sclk | sel  |
        };
        
        
        struct Config
        {   // contains: spi bus and ... tbd            
            embot::hw::SPI spi {embot::hw::SPI::five};
            embot::hw::spi::Config spicfg = standardspiconfig; // w/ Mode::zero   
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
        bool readMIB(PORT port, MIB mib, MIBdata &data, embot::core::relTime timeout = 5*embot::core::time1millisec);  
    private:        
        struct Impl;
        Impl *pImpl;    
    };
     
    
    
    
    
} // namespace embot::hw::chip {


//#define EMBOT_HW_CHIP_KSZ8563_enable_test   
#if defined(EMBOT_HW_CHIP_KSZ8563_enable_test)    
namespace embot::hw::chip {
    // it tests the chip and offers an example of use
    bool testof_KSZ8563();
}
#endif



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

