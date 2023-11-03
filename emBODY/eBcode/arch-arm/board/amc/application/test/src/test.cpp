
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

namespace test::can { 
    
    constexpr embot::hw::CAN bus {embot::hw::CAN::one};
    constexpr uint8_t addr {0};
    uint32_t rxcount {0};   
    uint32_t txcount {0};
    
    void alertonrxframe(void *p)
    {
        rxcount ++;       
    }

    void alertontxframe(void *p)
    {
        txcount ++;        
    }  

    void emitONcan( embot::app::eth::theErrorManager::Severity sev,
                    const embot::app::eth::theErrorManager::Caller &caller, 
                    const embot::app::eth::theErrorManager::Descriptor &des, 
                    const std::string &str);

    void init()
    {
        // i init the can driver
        embot::hw::can::Config canconfig {};
        canconfig.rxcapacity = 32;
        canconfig.txcapacity = 32;
        canconfig.onrxframe = embot::core::Callback(can::alertonrxframe, nullptr); 
        canconfig.ontxframe = embot::core::Callback(can::alertontxframe, nullptr);
        embot::hw::can::init(can::bus, canconfig);     
        // embot::hw::can::setfilters(bus, addr); 
        // and ok, enable it
        embot::hw::can::enable(can::bus);   

        // embot::app::eth::theErrorManager::getInstance().initialise(emitONcan);
    }
    
    void tick(size_t tt)
    {                     
        // process rx can frames
        std::uint8_t insideRXQ = embot::hw::can::inputqueuesize(can::bus);
        std::uint8_t remainingINrx = 0;
        embot::hw::can::Frame hwframe {};
        for(uint8_t i=0; i<insideRXQ; i++)
        {
            remainingINrx = 0;            
            if(embot::hw::resOK == embot::hw::can::get(can::bus, hwframe, remainingINrx))
            {            
                // print info abou the can frame
                // we have a .to_string() for the prot::canframe ...
                
                embot::prot::can::Frame frame {hwframe.id, hwframe.size, hwframe.data};                    
                embot::core::print("RX frame: " + frame.to_string());                
            }
        } 
        
        embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};
        embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::warning, {"test::any::tick()", thr}, {0xaabbccdd, 0x1234, 0}, "just called");
        
        

//        // and now tx the can frames we need
//        constexpr size_t num {8};
//        for(size_t i=0; i<num; i++)
//        {   
//            embot::prot::can::Frame frame {0x123, 8, {static_cast<uint8_t>(tt), static_cast<uint8_t>(i), 2, 3, 4, 5, 6, 7}};
//            embot::hw::can::put(can::bus, {frame.id, frame.size, frame.data});                                       
//        }
//        embot::hw::can::transmit(can::bus);          
    }
    
    
    void emitONcan( embot::app::eth::theErrorManager::Severity sev,
                    const embot::app::eth::theErrorManager::Caller &caller, 
                    const embot::app::eth::theErrorManager::Descriptor &des, 
                    const std::string &str)
    {
        std::string timenow = embot::core::TimeFormatter(embot::core::now()).to_string();
        std::string eobjstr = (true == caller.isvalid()) ? caller.objectname : "OBJ";
        std::string threadname = (true == caller.isvalid()) ? caller.owner->getName() : "THR";
        std::string severity = embot::app::eth::theErrorManager::to_cstring(sev);
        
        embot::core::print(std::string("[[") + severity + "]] @" + timenow + " (" + eobjstr + ", " + threadname + "): " + str);
        
        if(embot::app::eth::theErrorManager::Severity::trace == sev) 
        {
            return;
        } 

        // you may in here send the diagnostics message over a channel, maybe udp or can
        if(true == des.isvalid())
        {
            embot::prot::can::Frame frame 
            {
                0x701, 
                8, 
                {
                    0x66, 
                    static_cast<uint8_t>(des.code&0xff), static_cast<uint8_t>((des.code>>8)&0xff), 
                    static_cast<uint8_t>((des.code>>16)&0xff), static_cast<uint8_t>((des.code>>24)&0xff),
                    0, 
                    static_cast<uint8_t>((des.par16)&0xff), static_cast<uint8_t>((des.par16>>8)&0xff)
                }
            };
            
            embot::hw::can::put(can::bus, {frame.id, frame.size, frame.data});                         
        }
      
        if(embot::app::eth::theErrorManager::Severity::fatal == sev)
        {
            // fatal error. i must do something. maybe send the system in an error state and ...
            embot::app::theLEDmanager::getInstance().get(embot::hw::LED::one).stop();
            for(;;)
            {
                embot::hw::led::toggle(embot::hw::LED::one);
                embot::core::wait(200*embot::core::time1millisec);            
            }
        }        
        
    }   

}        


namespace test::any {
    
    void init()
    {
        test::can::init();            
    }

    void deinit()
    {

    }


    void tick(size_t tt)
    {
        embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};
        embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"test::any::tick()", thr}, {}, "just called");
        
        test::can::tick(tt);

    }

}


void test_embot_hw_init()
{
    test::any::init();   
}


void test_embot_hw_tick()
{
    static size_t tt {0};
    tt++;    
    
    test::any::tick(tt);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

