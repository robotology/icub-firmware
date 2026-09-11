
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// api
#include "test.h"


// dependencies
#include "embot_core.h"
#include "embot_os_theScheduler.h"
#include "embot_app_eth_theErrorManager.h"

#include "embot_hw_can.h"
#include "embot_prot_can.h"
#include "embot_app_theLEDmanager.h"

#include "embot_hw_i2c.h"

#include "embot_app_eth_theBackdoor.h"

#include "EOpacket.h"


namespace test::i2c
{
    constexpr uint32_t actionsRate {1000};
    static uint32_t timepassed = 0;
    
    constexpr size_t nI2Cs {3}; // maxnumberof is 4 but we are only using i2c1, i2c2, i2c3
    constexpr embot::hw::i2c::Config cfg {};
    constexpr embot::hw::i2c::ADR addressToTest7bit {0x6A};
    constexpr embot::hw::i2c::ADR addressToTestHAL {static_cast<embot::hw::i2c::ADR>(addressToTest7bit << 1)};
    constexpr embot::core::relTime pingTimeout {3*embot::core::time1millisec};
        
    //constexpr embot::hw::tlv493d::Config tlvConf {};
        
    void init()
    {
        
        // Default Config. We just need init and deinit

        for(size_t i=0; i<nI2Cs; i++)
        {
            embot::hw::I2C i2c {static_cast<embot::hw::I2C>(i)};
            embot::hw::i2c::init(i2c, cfg);
        }
        
        //embot::hw::tlv493d::init(embot::hw::TLV493D::one, tlvConf);
    }

    void deinit()
    {
        for(size_t i=0; i<nI2Cs; i++)
        {
            embot::hw::I2C i2c {static_cast<embot::hw::I2C>(i)};
            embot::hw::i2c::deinit(i2c);
        }
        
        //embot::hw::tlv493d::deinit(embot::hw::TLV493D::one);
    }

    
    void pingAddress()
    {
        char message[512] = {0};
        std::string result {};
        
        for(size_t i=0; i<nI2Cs; i++)
        {
            embot::hw::I2C i2c {static_cast<embot::hw::I2C>(i)};
            const bool responds = embot::hw::i2c::ping(i2c, addressToTestHAL, pingTimeout);
            
            result += "\nI2C channel #" + std::to_string((uint8_t)i2c);
            result += " address 0x6A (7-bit): ";
            result += responds ? "RESPONDING" : "NOT RESPONDING";
            result += "\n";
        }
        
        std::snprintf(message, sizeof(message), "%s", result.c_str());
        embot::app::eth::theErrorManager::getInstance().emit(
            embot::app::eth::theErrorManager::Severity::trace,
            {"i2c::pingAddress()", nullptr}, {}, message);
    }
        
    void tick(size_t tt)
    {
        // do actions at a rate of 1 call per second
        
        if(++timepassed >= actionsRate)
        {
            embot::core::print("Starting I2C address 0x6A test");
            
            pingAddress();
            
            embot::core::print("Ending I2C address 0x6A test");
            
            timepassed = 0;
        }
    }
}


namespace test::any {
    
    void init()
    {
      
    }

    void deinit()
    {

    }


    void tick(size_t tt)
    {

    }

}


void test_embot_hw_init()
{
    test::i2c::init();   
}


void test_embot_hw_tick()
{
    static size_t tt {0};
    tt++;    
    
    test::i2c::tick(tt);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

