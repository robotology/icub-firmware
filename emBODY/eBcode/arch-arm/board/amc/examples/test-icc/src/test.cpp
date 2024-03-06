
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


#include "embot_app_eth_theICCservice.h"

// the client transmits, the server receives
//#define iccAMCisSERVER
//#define iccAMCisCLIENT
#define iccAMCisCLIENTSERVER


namespace test::iccservice { 
    
    bool itemparser(const embot::app::eth::theICCservice::Item &item);
    
    embot::core::Callback onrx {};
    
    embot::app::eth::theICCservice::Config config
    {
        embot::app::eth::theICCservice::Mode::master,
        embot::hw::icc::LTR::one, embot::hw::icc::LTR::two,
        16, 16,
        embot::app::eth::theICCservice::modeTX::onflush,
        embot::os::Priority::system54, embot::os::Priority::system53,
        onrx,
        itemparser
    };

    void init()
    {
        embot::app::eth::theICCservice::getInstance().initialise(config);
        embot::app::eth::theICCservice::getInstance().set(embot::app::eth::theICCservice::modeTX::onflush); 
            
//        embot::prot::can::Frame frame {0x111, 8, {1, 2, 3, 4, 5, 6, 7, 8}};
//        embot::app::msg::Location loc {embot::app::msg::BUS::icc1, 3};
//        embot::app::eth::theICCservice::getInstance().put({loc, frame});
    }
    
    volatile bool flushed {false};
    
    constexpr embot::core::relTime tout {5*embot::core::time1millisec};
    
    void hello(void*p)
    {
        flushed = embot::app::eth::theICCservice::getInstance().flushed();
    }
    
    
    embot::core::Callback ontxdone {hello, nullptr};
    
    
    bool itemparser(const embot::app::eth::theICCservice::Item &item)
    {
        // i just print the received frame 
        embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};
        embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"RX frame", thr}, {}, 
            item.frame.to_string()
            );           
        
        return true;
    }
    
    void tick(size_t tt)
    { 
//        if(0 != (tt%2))
//        {
//            return;
//        }
        
        embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};   

#if defined(iccAMCisCLIENT)
        uint8_t n = tt;
        embot::prot::can::Frame frame {0x111, 8, {n, 0, 1, 1, 1, 1, 1, 1}};
        embot::app::msg::Location loc {embot::app::msg::BUS::icc1, 3};
        embot::app::eth::theICCservice::getInstance().put({loc, frame});

        embot::prot::can::Frame frame1 {0x111, 8, {n, 1, 1, 1, 1, 1, 1, 1}};
        embot::app::msg::Location loc1 {embot::app::msg::BUS::icc1, 3};
        embot::app::eth::theICCservice::getInstance().put({loc1, frame1});
        
        size_t n2tx = embot::app::eth::theICCservice::getInstance().output();
        
        embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"icc::tick(): ", thr}, {}, 
            "to be tx = " + std::to_string(n2tx)
            );        
                           
//        flushed = embot::app::eth::theICCservice::getInstance().flush(tout);
        embot::app::eth::theICCservice::getInstance().flush(ontxdone);
        flushed = flushed;
            
        for(;;)
        {
            if(true == flushed)
            {
                break;
            }
        }
            
#elif defined(iccAMCisSERVER)

        // i process the received frames 
        size_t rxitems = embot::app::eth::theICCservice::getInstance().input();
        
        embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"icc::tick(): ", thr}, {}, 
            "received = " + std::to_string(rxitems)
            ); 
        
        if(rxitems > 0)
        {
            embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"icc::tick(): ", thr}, {}, 
                "parsing them" 
                );             
            embot::app::eth::theICCservice::getInstance().parse();
        }
        
#elif defined(iccAMCisCLIENTSERVER)

        // i process the received frames 
        size_t rxitems = embot::app::eth::theICCservice::getInstance().input();
        
        embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"icc::tick(): ", thr}, {}, 
            "received = " + std::to_string(rxitems)
            ); 
        
        if(rxitems > 0)
        {
            embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"icc::tick(): ", thr}, {}, 
                "parsing them" 
                );             
            embot::app::eth::theICCservice::getInstance().parse();
        }

        // i transmit two frames 
        uint8_t n = tt;
        embot::prot::can::Frame frame {0x111, 8, {n, 0, 1, 1, 1, 1, 1, 1}};
        embot::app::msg::Location loc {embot::app::msg::BUS::icc1, 3};
        embot::app::eth::theICCservice::getInstance().put({loc, frame});

        embot::prot::can::Frame frame1 {0x112, 8, {n, 1, 1, 1, 1, 1, 1, 1}};
        embot::app::msg::Location loc1 {embot::app::msg::BUS::icc1, 3};
        embot::app::eth::theICCservice::getInstance().put({loc1, frame1});
        
        size_t n2tx = embot::app::eth::theICCservice::getInstance().output();
        
        embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"icc::tick(): ", thr}, {}, 
            "to be tx = " + std::to_string(n2tx)
            );        
          
        // and i lock until all are arrived.
            
        // flushed = embot::app::eth::theICCservice::getInstance().flush(tout);
        flushed = embot::app::eth::theICCservice::getInstance().flush(ontxdone);
        flushed = flushed;
            
        for(;;)
        {
            if(true == flushed)
            {
                break;
            }
        }
             
#endif
            
    }
    

}       

#include "embot_hw_icc_printer.h"

#define iccprinterAMCisSERVER
//#define iccprinterAMCisCLIENT


namespace test::iccprinter {
    
    
    void init()
    {       
#if defined(iccprinterAMCisSERVER)
        embot::hw::icc::printer::theServer::getInstance().initialise({});
#elif defined(iccAMCisCLIENT)       
        embot::hw::icc::printer::theClient::getInstance().initialise({});    
#endif          
    }
    
    void tick(size_t tt)
    {         
#if defined(iccprinterAMCisSERVER)
    
#elif defined(iccAMCisCLIENT)       
        embot::hw::icc::printer::theClient::getInstance().post("hello there, i am a cm7"); 
#endif         
    }    
    
}


namespace test::any {
    
    void init()
    {
        test::iccservice::init();  
        // test::iccprinter::init();        
    }

    void deinit()
    {

    }


    void tick(size_t tt)
    {
//        embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};
//        embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"test::any::tick()", thr}, {}, "just called");
        
        test::iccservice::tick(tt);
        // test::iccprinter::tick(tt);
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

