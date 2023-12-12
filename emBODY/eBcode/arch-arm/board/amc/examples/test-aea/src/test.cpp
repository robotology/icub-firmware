
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

#include "embot_hw_encoder.h"

#include "embot_app_eth_theBackdoor.h"

#include "EOpacket.h"

namespace test::encoder {
    
    enum class MODE {blocking, callback, asInEncoderReader};   

    
    constexpr size_t nENCs {3};    
    constexpr bool useAEA3 {true};
    constexpr MODE mode {MODE::callback};
    // {MODE::asInEncoderReader}; {MODE::callback}; {MODE::blocking};
    constexpr embot::core::Time periodofreport {2*embot::core::time1millisec};
    
    constexpr embot::hw::encoder::Config cfgAEA {embot::hw::encoder::Type::chipAS5045};
    constexpr embot::hw::encoder::Config cfgAEA3 {embot::hw::encoder::Type::chipMA730};

    std::array<embot::hw::encoder::POS, nENCs> spiRawValues {};
    std::array<volatile bool, nENCs> done {}; 
        
    embot::core::Time lastReport {0};
    
    
    
    std::array<embot::core::Callback, nENCs> thecallbacks {};
    std::array<embot::hw::ENCODER, nENCs> encs {};  
    volatile embot::hw::result_t hwres {embot::hw::resNOK};        
    void onreadingdone(void *p)
    {
        embot::hw::ENCODER e = *reinterpret_cast<embot::hw::ENCODER*>(p);
        uint8_t i = embot::core::tointegral(e);
        
        hwres = embot::hw::encoder::getValue(e, spiRawValues[i]);
        done[i] = true;
    }
    
    EOpacket *packet {nullptr};
    
    eOipv4addr_t hostip {EO_COMMON_IPV4ADDR(10, 0, 1, 104)};
    eOipv4port_t hostport {6666};
                
    void init()
    {
        embot::hw::encoder::Config cfg {};
        
        cfg = (useAEA3) ? cfgAEA3 : cfgAEA;
        
        for(size_t i=0; i<nENCs; i++)
        {
            embot::hw::ENCODER enc {static_cast<embot::hw::ENCODER>(i)};
            embot::hw::encoder::init(enc, cfg);
            encs[i] = enc;
            thecallbacks[i] = {onreadingdone, &encs[i]};
        }  

        lastReport = 0; //embot::core::now();   

        packet = eo_packet_New(512);    

        eo_packet_Addressing_Set(packet, hostip, hostport);         
    }

    void deinit()
    {
        for(size_t i=0; i<nENCs; i++)
        {
            embot::hw::ENCODER enc {static_cast<embot::hw::ENCODER>(i)};
            embot::hw::encoder::deinit(enc);
        }
    }
    
    
    void start()
    {
        for(size_t i=0; i<nENCs; i++)
        {
            spiRawValues[i] = 60666;
        }
        
        switch(mode)
        {
            case MODE::blocking:
            {
                // do nothing
            } break;
            
            case MODE::callback:
            {
                for(size_t i=0; i<nENCs; i++)
                {
                    embot::hw::ENCODER enc {static_cast<embot::hw::ENCODER>(i)};
                    done[i] = false;
                    embot::hw::encoder::startRead(enc, thecallbacks[i]);
                } 
                
            } break;

            case MODE::asInEncoderReader:
            {
                for(size_t i=0; i<nENCs; i++)
                {
                    embot::hw::ENCODER enc {static_cast<embot::hw::ENCODER>(i)};
                    embot::hw::encoder::startRead(enc, {});
                }                
            } break; 

            default: {} break;
            
        } 
       
    }

    void read()
    {  
        constexpr embot::core::Time timeout {200*embot::core::time1microsec};
        switch(mode)
        {
            case MODE::blocking:
            {
                for(size_t i=0; i<nENCs; i++)
                {
                    embot::hw::ENCODER enc {static_cast<embot::hw::ENCODER>(i)};
                    embot::hw::encoder::read(enc, spiRawValues[i], timeout);
                } 
                
            } break;
            
            case MODE::callback:
            {
                for(size_t i=0; i<nENCs; i++)
                {
                    for(;;)
                    {
                        if(true == done[i]) { break; }                        
                    }
                } 
                // we now have the values
                
            } break;

            case MODE::asInEncoderReader:
            {
                // very important .... we should probably wait for some time ... 
                embot::core::wait(timeout);
                
                for(size_t i=0; i<nENCs; i++)
                {
                    embot::hw::ENCODER enc {static_cast<embot::hw::ENCODER>(i)};
                    embot::hw::encoder::getValue(enc, spiRawValues[i]);
                } 
                
            } break; 

            default: {} break;
            
        }               
        
    }
    
    char sss[512] = {0};

    void print()
    {
        embot::core::Time n {embot::core::now()};
        
        if((n-lastReport) < periodofreport)
        {
            return;
        }            
        
        lastReport = n;
        
        std::string s {embot::core::TimeFormatter(n).to_string() + ":"};
        for(size_t i=0; i<nENCs; i++)
        {
            s += " ";
            s += std::to_string(spiRawValues[i]);           
        } 
        
//       embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"print()", nullptr}, {}, s); 
        
//        embot::core::print(s);
        
        std::snprintf(sss, sizeof(sss), "%s\n", s.c_str());             
            
        eo_packet_Payload_Set(packet, reinterpret_cast<uint8_t*>(sss), std::strlen(sss));    
        embot::app::eth::theBackdoor::getInstance().transmit(packet);    
        
        
#if 0 

open a shell and digit
```bash
$ netcat -u 10.0.1.99 -p 6666 6666
```
       
#endif        
    }        

    void tick(size_t tt)
    {
        start();
        read();
        print();
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
    test::encoder::init();   
}


void test_embot_hw_tick()
{
    static size_t tt {0};
    tt++;    
    
    test::encoder::tick(tt);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

