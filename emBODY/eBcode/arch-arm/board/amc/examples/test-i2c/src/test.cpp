
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
    enum class MODE {blocking};
    
    constexpr size_t nI2Cs {3}; // maxnumberof is 4 but we are only using i2c1, i2c2, i2c3
    constexpr MODE mode {MODE::blocking};
    constexpr embot::hw::i2c::Config cfg {};
    std::array<embot::hw::i2c::ADR, nI2Cs> i2cAddresses {};
        
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

    
    void discovery()
    {
        char sss[512] = {0};
        std::string s {};
            
        bool res = false;
        
        
        std::vector<embot::hw::i2c::ADR> adrsvec = {};
        
        for(size_t i=0; i<nI2Cs; i++)
        {
            embot::hw::I2C i2c {static_cast<embot::hw::I2C>(i)};
            embot::hw::i2c::discover(i2c, adrsvec);
            i2cAddresses[i] = adrsvec[0];
            
            s += "\n";
            s += "I2C channel #" + std::to_string((uint8_t)i2c) + " adrs: ";
            for(size_t j=0; j<adrsvec.size(); j++)
            {
                s += std::to_string(adrsvec[j]);
                s += " ";                
            }
            s += "\n";
        }
        
//        
//        res = embot::hw::tlv493d::isalive(embot::hw::TLV493D::one, 1000*embot::core::time1millisec);
//        s += "Is the tlv chip alive: " + std::to_string(res);
        std::snprintf(sss, sizeof(sss), "%s\n", s.c_str());
        embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"print()", nullptr}, {}, sss); 
    }
    
    void ping(std::array<embot::hw::i2c::ADR, nI2Cs> &adrs)
    {
        char sss[512] = {0};
        std::string s {};
        
        embot::core::relTime timeout = 500*embot::core::time1millisec;
        
        for(size_t i=0; i<nI2Cs; i++)
        {
            embot::hw::I2C i2c {static_cast<embot::hw::I2C>(i)};
            embot::hw::i2c::ping(i2c, adrs[i], timeout);
            
            s += "\n";
            s += "I2C channel #" + std::to_string((uint8_t)i2c) + " pinged adr: " + std::to_string((uint8_t)adrs[i]);
            
            s += "\n";
        }
        std::snprintf(sss, sizeof(sss), "%s\n", s.c_str());
        embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"print()", nullptr}, {}, sss); 
    }
        
    void tick(size_t tt)
    {
        // do actions at a rate of 1 call per second
        
        if(++timepassed > actionsRate)
        {
            embot::core::print("Starting call to Actions of discovery and ping");
            
            discovery();
            ping(i2cAddresses);
            
            embot::core::print("Ending call to Actions of discovery and ping");
            
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

