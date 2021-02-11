
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_app_skeleton_os_evthreadcan.h"

#include "tCOMM.h"

// --------------------------------------------------------------------------------------------------------------------
// config start

constexpr embot::app::theCANboardInfo::applicationInfo applInfo 
{ 
    embot::prot::can::versionOfAPPLICATION {1, 0, 1},    
    embot::prot::can::versionOfCANPROTOCOL {2, 0}    
};

constexpr std::uint16_t threadIDLEstacksize = 2048;
constexpr std::uint16_t threadINITstacksize = 6*1024;
constexpr std::uint16_t threadCOMMstacksize = 4096;
constexpr embot::core::relTime osTick = 1000*embot::core::time1microsec; 
constexpr std::uint8_t maxINPcanframes = 16;
constexpr std::uint8_t maxOUTcanframes = 48;
constexpr embot::core::relTime threadCOMMtimeout = 200*embot::core::time1millisec;

static void *paramINIT = nullptr;
static void *paramIDLE = nullptr;
static void *paramERR = nullptr;
static void *paramCOMM = nullptr;

constexpr embot::os::theTimerManager::Config tmcfg {1024, 16, embot::os::Priority::system50};
constexpr embot::os::theCallbackManager::Config cmcfg {1024, 16, embot::os::Priority::system49};
    
    
static const embot::app::skeleton::os::basic::sysConfig syscfg { threadINITstacksize, paramINIT, threadIDLEstacksize, paramIDLE, paramERR, tmcfg, cmcfg, osTick};

static const embot::app::skeleton::os::evthreadcan::evtConfig COMMthreadConfig { threadCOMMstacksize, paramCOMM, threadCOMMtimeout};

static const embot::app::skeleton::os::evthreadcan::canConfig COMMcancfg { maxINPcanframes, maxOUTcanframes };

// config end
// --------------------------------------------------------------------------------------------------------------------


class mySYS final : public embot::app::skeleton::os::evthreadcan::SYSTEMevtcan
{
public:
    mySYS(const embot::app::skeleton::os::basic::sysConfig &cfg) 
        : SYSTEMevtcan(cfg) {}
        
    void userdefOnIdle(embot::os::Thread *t, void* idleparam) const override;
    void userdefonOSerror(void *errparam) const override;
    void userdefInit_Extra(embot::os::EventThread* evthr, void *initparam) const override;
};


static const mySYS mysys { syscfg };
static const embot::app::ctrl::tCOMM t_comm { COMMthreadConfig, COMMcancfg, applInfo };
constexpr embot::app::skeleton::os::evthreadcan::CFG cfg{ &mysys, &t_comm };

// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    embot::app::skeleton::os::evthreadcan::run(cfg);
    for(;;);    
}


// - here is the tailoring of the board.
//   the init thread will start:
//   - the COMM thread is the one defined in the embot::app::skeleton::os::evthreadcan::evThreadCAN object
//     it will deal only with RX of CAN frames, passing them to parser, obtaining from them possible replies,
//     transmitting them back.
//     it will also accept TX of CAN frames as other thread may request.

#define enableTRACE_all


#include "embot_os_theScheduler.h"
#include "embot_app_theLEDmanager.h"
#include "embot_app_application_theCANparserBasic.h"
#include "embot_app_application_theCANtracer.h"

#include "embot_app_application_thePOSreader2.h"
#include "embot_app_application_theCANparserPOS.h"
#include "embot_hw_tlv493d.h"

#include "embot_app_application_thePZMdriver.h"
#include "embot_app_application_theCANparserMC.h"

#include "embot_hw_bsp.h"
#include "embot_hw_bsp_config.h"
#include "embot_hw_sys.h"

#include "tSNSR.h"
#include "tCOMM.h"
#include "tCTRL.h"


void mySYS::userdefOnIdle(embot::os::Thread *t, void* idleparam) const
{
    static int a = 0;
    a++;        
}

void mySYS::userdefonOSerror(void *errparam) const
{
    static int code = 0;
    embot::os::theScheduler::getInstance().getOSerror(code);
    for(;;);    
}


embot::app::ctrl::tSNSR *t_snsr {nullptr};
embot::app::ctrl::tCTRL *t_ctrl {nullptr};

    static constexpr embot::os::Event evtACQUIREfaps =embot::core::binary::mask::pos2mask<embot::os::Event>(0);
    static constexpr embot::os::Event evtNOREPLYfaps =embot::core::binary::mask::pos2mask<embot::os::Event>(1);

    static constexpr embot::os::Event evtSNSR09_askdata = embot::core::binary::mask::pos2mask<embot::os::Event>(5);  
    static constexpr embot::os::Event evtSNSR09_dataready = embot::core::binary::mask::pos2mask<embot::os::Event>(6);  
    static constexpr embot::os::Event evtSNSR09_noreply = 0; //embot::core::binary::mask::pos2mask<embot::os::Event>(7);

    static constexpr embot::os::Event evtSNSR01_askdata = embot::core::binary::mask::pos2mask<embot::os::Event>(8);  
    static constexpr embot::os::Event evtSNSR01_dataready = embot::core::binary::mask::pos2mask<embot::os::Event>(9);  
    static constexpr embot::os::Event evtSNSR01_noreply = 0; //embot::core::binary::mask::pos2mask<embot::os::Event>(10);
    static constexpr embot::os::Event evtSNSR02_askdata = embot::core::binary::mask::pos2mask<embot::os::Event>(11);  
    static constexpr embot::os::Event evtSNSR02_dataready = embot::core::binary::mask::pos2mask<embot::os::Event>(12);  
    static constexpr embot::os::Event evtSNSR02_noreply = 0; //embot::core::binary::mask::pos2mask<embot::os::Event>(13);
    static constexpr embot::os::Event evtSNSR03_askdata = embot::core::binary::mask::pos2mask<embot::os::Event>(14);  
    static constexpr embot::os::Event evtSNSR03_dataready = embot::core::binary::mask::pos2mask<embot::os::Event>(15);  
    static constexpr embot::os::Event evtSNSR03_noreply = 0; //embot::core::binary::mask::pos2mask<embot::os::Event>(16);
    static constexpr embot::os::Event evtSNSR04_askdata = embot::core::binary::mask::pos2mask<embot::os::Event>(17);  
    static constexpr embot::os::Event evtSNSR04_dataready = embot::core::binary::mask::pos2mask<embot::os::Event>(18);  
    static constexpr embot::os::Event evtSNSR04_noreply = 0; //embot::core::binary::mask::pos2mask<embot::os::Event>(19);
    static constexpr embot::os::Event evtSNSR05_askdata = embot::core::binary::mask::pos2mask<embot::os::Event>(20);  
    static constexpr embot::os::Event evtSNSR05_dataready = embot::core::binary::mask::pos2mask<embot::os::Event>(21);  
    static constexpr embot::os::Event evtSNSR05_noreply = 0; //embot::core::binary::mask::pos2mask<embot::os::Event>(22);
    static constexpr embot::os::Event evtSNSR06_askdata = embot::core::binary::mask::pos2mask<embot::os::Event>(23);  
    static constexpr embot::os::Event evtSNSR06_dataready = embot::core::binary::mask::pos2mask<embot::os::Event>(24);  
    static constexpr embot::os::Event evtSNSR06_noreply = 0; //embot::core::binary::mask::pos2mask<embot::os::Event>(25);

    static constexpr embot::os::Event evtSNSR07_askdata = embot::core::binary::mask::pos2mask<embot::os::Event>(26);  
    static constexpr embot::os::Event evtSNSR07_dataready = embot::core::binary::mask::pos2mask<embot::os::Event>(27);  
    static constexpr embot::os::Event evtSNSR07_noreply = 0; //embot::core::binary::mask::pos2mask<embot::os::Event>(28);

    static constexpr embot::os::Event evtSNSR08_askdata = embot::core::binary::mask::pos2mask<embot::os::Event>(29);  
    static constexpr embot::os::Event evtSNSR08_dataready = embot::core::binary::mask::pos2mask<embot::os::Event>(30);  
    static constexpr embot::os::Event evtSNSR08_noreply = 0; //embot::core::binary::mask::pos2mask<embot::os::Event>(31);

    using namespace embot::app::application;



        constexpr thePOSreader2::Sensor s1 {
            thePOSreader2::sensorType::tlv, embot::hw::ANY::one, embot::prot::can::analog::posLABEL::zero,
            evtSNSR01_askdata,
            evtSNSR01_dataready,
            evtSNSR01_noreply,
            5*embot::core::time1millisec // timeout  
        };
        
        constexpr thePOSreader2::Sensor s2 {
            thePOSreader2::sensorType::tlv, embot::hw::ANY::two, embot::prot::can::analog::posLABEL::one, 
            evtSNSR02_askdata,
            evtSNSR02_dataready,
            evtSNSR02_noreply,
            0 // 5*embot::core::time1millisec // timeout  
        };  

        constexpr thePOSreader2::Sensor s3 {
            thePOSreader2::sensorType::tlv, embot::hw::ANY::three, embot::prot::can::analog::posLABEL::two, 
            evtSNSR03_askdata,
            evtSNSR03_dataready,
            evtSNSR03_noreply,
            0 //5*embot::core::time1millisec // timeout  
        };  

        constexpr thePOSreader2::Sensor s4 {
            thePOSreader2::sensorType::tlv, embot::hw::ANY::four, embot::prot::can::analog::posLABEL::three,  
            evtSNSR04_askdata,
            evtSNSR04_dataready,
            evtSNSR04_noreply,
            0 //5*embot::core::time1millisec // timeout  
        }; 
        
        constexpr thePOSreader2::Sensor s5 {
            thePOSreader2::sensorType::tlv, embot::hw::ANY::five, embot::prot::can::analog::posLABEL::four, 
            evtSNSR05_askdata,
            evtSNSR05_dataready,
            evtSNSR05_noreply,
            0 //5*embot::core::time1millisec // timeout  
        };   

        constexpr thePOSreader2::Sensor s6 {
            thePOSreader2::sensorType::tlv, embot::hw::ANY::six, embot::prot::can::analog::posLABEL::five,  
            evtSNSR06_askdata,
            evtSNSR06_dataready,
            evtSNSR06_noreply,
            0 //5*embot::core::time1millisec // timeout  
        };     

        constexpr thePOSreader2::Sensor s7 {
            thePOSreader2::sensorType::lr17, embot::hw::ANY::one, embot::prot::can::analog::posLABEL::six,  
            evtSNSR07_askdata,
            evtSNSR07_dataready,
            evtSNSR07_noreply,
            0 //5*embot::core::time1millisec // timeout  
        };   

        constexpr thePOSreader2::Sensor s8 {
            thePOSreader2::sensorType::qe, embot::hw::ANY::one, embot::prot::can::analog::posLABEL::seven,  
            evtSNSR08_askdata,
            evtSNSR08_dataready,
            evtSNSR08_noreply,
            0 //5*embot::core::time1millisec // timeout  
        };          

        constexpr thePOSreader2::Sensor s9 {
            thePOSreader2::sensorType::qe, embot::hw::ANY::two, embot::prot::can::analog::posLABEL::eight,  
            evtSNSR09_askdata,
            evtSNSR09_dataready,
            evtSNSR09_noreply,
            0 //5*embot::core::time1millisec // timeout  
        };          
    
#if defined(USE_thePOSreader2_qe)      
    constexpr std::array<thePOSreader2::Sensor, thePOSreader2::numberofpositions> sposmod2par { s1, s4, s2, s5, s3, s6, s7, s8, s9 };
#else     
    //constexpr std::array<thePOSreader2::Sensor, thePOSreader2::numberofpositions> sposdaisy { s1, s2, s3, s4, s5, s6, s7, {}, {} };    
    constexpr std::array<thePOSreader2::Sensor, thePOSreader2::numberofpositions> sposmod2par { s1, s4, s2, s5, s3, s6, s7, {}, {} };
#endif

void mySYS::userdefInit_Extra(embot::os::EventThread* evthr, void *initparam) const
{
    // inside the init thread: put the init of many things ...  
    // led manager
    static const std::initializer_list<embot::hw::LED> alltheleds = 
    {
        embot::hw::LED::one, embot::hw::LED::two, embot::hw::LED::three, 
        embot::hw::LED::four, embot::hw::LED::five, embot::hw::LED::six, embot::hw::LED::seven
    };  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(alltheleds);    
    theleds.get(embot::hw::LED::seven).pulse(embot::core::time1second); 
    
    volatile static uint32_t clockspeed = 0;
    
    clockspeed = embot::hw::sys::clock(embot::hw::CLOCK::syscore);

    clockspeed = clockspeed;
    
    // the communication thread is already initted. 
           
    t_snsr = new embot::app::ctrl::tSNSR;  
    t_snsr->initialise({});  
        
   
    // init the posreader2 ...
    
    embot::os::Thread *tComm = embot::app::skeleton::os::evthreadcan::getEVTthread();
    embot::os::Thread *tSnsr = t_snsr->getThread();

    #if defined(EMBOT_ENABLE_hw_tlv493d_U27off) && defined(EMBOT_ENABLE_hw_tlv493d_J13off)
        
        
    constexpr std::array<thePOSreader2::Sensor,thePOSreader2::numberofpositions> sposmod2par4FINGERS { s1, s2, s3, s4 };    
    embot::app::application::thePOSreader2::Config configmod2par4FINGERS
    {
        embot::app::application::thePOSreader2::AcquisitionMode::mod2parallel,    
        50*embot::core::time1millisec,  // acquisition time
        5*embot::core::time1millisec, // timeout for every acquisition step (1 step for fullyparallel, 2 for mod2parallel, n for daisychain)        
        tSnsr,          // reader thread 
        tComm,          // transmitter thread
        sposmod2par4FINGERS,
        { evtACQUIREfaps, evtNOREPLYfaps, embot::app::ctrl::evtTRANSMITfaps,  }    // associated events such as ... read sensor1, read sensor2, tranmsit sensors etc.
    };  

    embot::app::application::thePOSreader2 &thepos = embot::app::application::thePOSreader2::getInstance();
    thepos.initialise(configmod2par4FINGERS);          
    #else  
    embot::app::application::thePOSreader2::Config configmod2par
    {
        embot::app::application::thePOSreader2::AcquisitionMode::mod2parallel,    
        50*embot::core::time1millisec,  // acquisition time
        5*embot::core::time1millisec, // timeout for every acquisition step (1 step for fullyparallel, 2 for mod2parallel, n for daisychain)        
        tSnsr,          // reader thread 
        tComm,          // transmitter thread
        sposmod2par,
        { evtACQUIREfaps, evtNOREPLYfaps, embot::app::ctrl::evtTRANSMITfaps,  }    // associated events such as ... read sensor1, read sensor2, tranmsit sensors etc.
    };  

    embot::app::application::thePOSreader2 &thepos = embot::app::application::thePOSreader2::getInstance();
    thepos.initialise(configmod2par);  
    #endif

    // init parser of POS CAN messages and link it to its agent: thePOSreader2
    embot::app::application::theCANparserPOS &canparserpos = embot::app::application::theCANparserPOS::getInstance();
    embot::app::application::theCANparserPOS::Config configparserpos { &thepos };
    canparserpos.initialise(configparserpos);  
    
     
    // now the control object
    
    t_ctrl = new embot::app::ctrl::tCTRL;     
    t_ctrl->initialise({0, 1000*embot::core::time1microsec});  
        
   
    // init the pzmdriver ...    
    embot::os::Thread *tCtrl = t_ctrl->getThread();   
    
    constexpr embot::core::relTime velocityTimeout = 100*embot::core::time1millisec;

    embot::app::application::thePZMdriver::Config pzmconfig
    {
        tCtrl,
        embot::os::bitpos2event(3),
        embot::os::bitpos2event(4),
        velocityTimeout
    };  

    embot::app::application::thePZMdriver &thepzmdriver = embot::app::application::thePZMdriver::getInstance();
    thepzmdriver.initialise(pzmconfig);  

    // init parser of MC CAN messages and link it to its agent: thePOSreader2
    embot::app::application::theCANparserMC &canparsermc = embot::app::application::theCANparserMC::getInstance();
    embot::app::application::theCANparserMC::Config configparsermc { &thepzmdriver };
    canparsermc.initialise(configparsermc);      
        
}




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

