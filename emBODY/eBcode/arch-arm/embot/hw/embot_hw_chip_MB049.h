
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_CHIP_MB049_H_
#define __EMBOT_HW_CHIP_MB049_H_

#include "embot_hw_spi.h"

#if 0
## Description

tbd



## Basic usage

You can use the following code, as long as the settings for SPI are specified by the bsp of the board.

```c++
#include "embot_hw_chip_MB049.h"

bool ok = embot::hw::chip::MB049::testof_MB049();

```

Code listing. Usage of the class


## Caveat Emptor

The interface of the device driver is kept intentionally simple and some features are left (for now) inside the private implementation.

## References
[1] tbd

#endif


namespace embot::hw::chip {
    
    class MB049
    {
      
    public:
        
        //Lookup table for polynome = 0x97
        static constexpr uint8_t ab_CRC8_LUT[256] = {
        0x00, 0x97, 0xB9, 0x2E, 0xE5, 0x72, 0x5C, 0xCB, 0x5D, 0xCA, 0xE4, 0x73, 0xB8, 0x2F, 0x01, 0x96,
        0xBA, 0x2D, 0x03, 0x94, 0x5F, 0xC8, 0xE6, 0x71, 0xE7, 0x70, 0x5E, 0xC9, 0x02, 0x95, 0xBB, 0x2C,
        0xE3, 0x74, 0x5A, 0xCD, 0x06, 0x91, 0xBF, 0x28, 0xBE, 0x29, 0x07, 0x90, 0x5B, 0xCC, 0xE2, 0x75, 
        0x59, 0xCE, 0xE0, 0x77, 0xBC, 0x2B, 0x05, 0x92, 0x04, 0x93, 0xBD, 0x2A, 0xE1, 0x76, 0x58, 0xCF, 
        0x51, 0xC6, 0xE8, 0x7F, 0xB4, 0x23, 0x0D, 0x9A, 0x0C, 0x9B, 0xB5, 0x22, 0xE9, 0x7E, 0x50, 0xC7, 
        0xEB, 0x7C, 0x52, 0xC5, 0x0E, 0x99, 0xB7, 0x20, 0xB6, 0x21, 0x0F, 0x98, 0x53, 0xC4, 0xEA, 0x7D, 
        0xB2, 0x25, 0x0B, 0x9C, 0x57, 0xC0, 0xEE, 0x79, 0xEF, 0x78, 0x56, 0xC1, 0x0A, 0x9D, 0xB3, 0x24, 
        0x08, 0x9F, 0xB1, 0x26, 0xED, 0x7A, 0x54, 0xC3, 0x55, 0xC2, 0xEC, 0x7B, 0xB0, 0x27, 0x09, 0x9E, 
        0xA2, 0x35, 0x1B, 0x8C, 0x47, 0xD0, 0xFE, 0x69, 0xFF, 0x68, 0x46, 0xD1, 0x1A, 0x8D, 0xA3, 0x34, 
        0x18, 0x8F, 0xA1, 0x36, 0xFD, 0x6A, 0x44, 0xD3, 0x45, 0xD2, 0xFC, 0x6B, 0xA0, 0x37, 0x19, 0x8E, 
        0x41, 0xD6, 0xF8, 0x6F, 0xA4, 0x33, 0x1D, 0x8A, 0x1C, 0x8B, 0xA5, 0x32, 0xF9, 0x6E, 0x40, 0xD7,
        0xFB, 0x6C, 0x42, 0xD5, 0x1E, 0x89, 0xA7, 0x30, 0xA6, 0x31, 0x1F, 0x88, 0x43, 0xD4, 0xFA, 0x6D, 
        0xF3, 0x64, 0x4A, 0xDD, 0x16, 0x81, 0xAF, 0x38, 0xAE, 0x39, 0x17, 0x80, 0x4B, 0xDC, 0xF2, 0x65, 
        0x49, 0xDE, 0xF0, 0x67, 0xAC, 0x3B, 0x15, 0x82, 0x14, 0x83, 0xAD, 0x3A, 0xF1, 0x66, 0x48, 0xDF, 
        0x10, 0x87, 0xA9, 0x3E, 0xF5, 0x62, 0x4C, 0xDB, 0x4D, 0xDA, 0xF4, 0x63, 0xA8, 0x3F, 0x11, 0x86, 
        0xAA, 0x3D, 0x13, 0x84, 0x4F, 0xD8, 0xF6, 0x61, 0xF7, 0x60, 0x4E, 0xD9, 0x12, 0x85, 0xAB, 0x3C};
        
        using POS = uint32_t;
        
        struct Status
        {
            uint8_t bits {0};
            bool ok {false};
            Status() = default;
        };
        
        struct Data
        {
            POS position {0};
            uint16_t multiturncounter {0};
            Status status {};
            uint8_t crc {0};
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
            embot::hw::spi::Prescaler::sixtyfour,  //probably the right value is: onehundredtwentyeigth, this has to be tested (I doubled the SPI BUS clock freq so the prescaler must be doubled)
            embot::hw::spi::DataSize::eight,
            embot::hw::spi::Mode::one,
            { {embot::hw::gpio::Pull::pullup, embot::hw::gpio::Pull::pulldown,      // | miso | mosi |
               embot::hw::gpio::Pull::pulldown, embot::hw::gpio::Pull::pullup} }    // | sclk | sel  |
        };
        
        //static constexpr embot::hw::spi::Config  & getSPIconfig() const;
        
        MB049();
        ~MB049();

        bool isinitted() const;
        bool init(const Config &config);
        bool deinit();
        
        bool read(Data &data, embot::core::relTime timeout);
        bool read(Data &data, const embot::core::Callback &oncompletion);
        
        /* CRC 0x97 Polynomial, 64-bit input data, right alignment, calculation over 64 bits */
        uint8_t CRC_SPI_97_64bit(uint64_t dw_InputData);

        
    private:
        struct Impl;
        Impl *pImpl;
        POS position;
    };
} // namespace embot::hw::chip {


#define EMBOT_HW_CHIP_MB049_enable_test
#if defined(EMBOT_HW_CHIP_MB049_enable_test)
namespace embot { namespace hw { namespace chip {
    // it tests the chip and offers an example of use
    bool testof_MB049();
}}}
#endif



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

