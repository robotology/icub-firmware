
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
    embot::prot::can::versionOfAPPLICATION {1, 0, 0},    
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

#include "embot_app_application_theFAPreader.h"
#include "embot_app_application_thePOSreader2.h"
#include "embot_app_application_theCANparserPOS.h"
#include "embot_hw_tlv493d.h"


#include "embot_hw_bsp.h"
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

    //static constexpr embot::os::Event evtTRANSMITfaps = embot::core::binary::mask::pos2mask<embot::os::Event>(2); 

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
    
    using namespace embot::app::application;

 


#if defined(USE_thePOSreader2)

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
        
        constexpr std::array<thePOSreader2::Sensor,thePOSreader2::numberofpositions> sposdaisy { s1, s2, s3, s4, s5, s6, s7 };
        constexpr std::array<thePOSreader2::Sensor,thePOSreader2::numberofpositions> sposmod2par { s1, s4, s2, s5, s3, s6, s7 };


#else
        constexpr theFAPreader::Sensor s1 {
            embot::hw::TLV493D::one, 
            embot::hw::tlv493d::Config{ embot::hw::tlv493d::Config::startupMODE::dontresetCHIP }, 
            evtSNSR01_askdata,
            evtSNSR01_dataready,
            evtSNSR01_noreply,
            5*embot::core::time1millisec // timeout  
        };
        
        constexpr theFAPreader::Sensor s2 {
            embot::hw::TLV493D::two, 
            embot::hw::tlv493d::Config{ embot::hw::tlv493d::Config::startupMODE::dontresetCHIP }, 
            evtSNSR02_askdata,
            evtSNSR02_dataready,
            evtSNSR02_noreply,
            0 // 5*embot::core::time1millisec // timeout  
        };  

        constexpr theFAPreader::Sensor s3 {
            embot::hw::TLV493D::three, 
            embot::hw::tlv493d::Config{ embot::hw::tlv493d::Config::startupMODE::dontresetCHIP }, 
            evtSNSR03_askdata,
            evtSNSR03_dataready,
            evtSNSR03_noreply,
            0 //5*embot::core::time1millisec // timeout  
        };  

        constexpr theFAPreader::Sensor s4 {
            embot::hw::TLV493D::four, 
            embot::hw::tlv493d::Config{ embot::hw::tlv493d::Config::startupMODE::dontresetCHIP }, 
            evtSNSR04_askdata,
            evtSNSR04_dataready,
            evtSNSR04_noreply,
            0 //5*embot::core::time1millisec // timeout  
        }; 
        
        constexpr theFAPreader::Sensor s5 {
            embot::hw::TLV493D::five, 
            embot::hw::tlv493d::Config{ embot::hw::tlv493d::Config::startupMODE::dontresetCHIP }, 
            evtSNSR05_askdata,
            evtSNSR05_dataready,
            evtSNSR05_noreply,
            0 //5*embot::core::time1millisec // timeout  
        };   

        constexpr theFAPreader::Sensor s6 {
            embot::hw::TLV493D::six, 
            embot::hw::tlv493d::Config{ embot::hw::tlv493d::Config::startupMODE::dontresetCHIP }, 
            evtSNSR06_askdata,
            evtSNSR06_dataready,
            evtSNSR06_noreply,
            0 //5*embot::core::time1millisec // timeout  
        };          
        
        constexpr std::array<theFAPreader::Sensor,theFAPreader::numberofpositions> sfapsdaisy { s1, s2, s3, s4, s5, s6 };
        constexpr std::array<theFAPreader::Sensor,theFAPreader::numberofpositions> sfapsmod2par { s1, s4, s2, s5, s3, s6 };
        //constexpr std::array<theFAPreader::Sensor,theFAPreader::numberofpositions> sfaps { s1, {}, s3, s4, {}, s6 };
        //constexpr std::array<theFAPreader::Sensor,theFAPreader::numberofpositions> sfaps { s6, s5, s4, s3, s2, s1 };
        //constexpr std::array<theFAPreader::Sensor,theFAPreader::numberofpositions> sfaps { s6, {}, s4, s3, {}, s1 };
   
#endif


void mySYS::userdefInit_Extra(embot::os::EventThread* evthr, void *initparam) const
{
    // inside the init thread: put the init of many things ...  
    // led manager
    static const std::initializer_list<embot::hw::LED> alltheleds = {embot::hw::LED::one};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(alltheleds);    
    theleds.get(embot::hw::LED::one).pulse(embot::core::time1second); 
    
    volatile static uint32_t clockspeed = 0;
    
    clockspeed = embot::hw::sys::clock(embot::hw::CLOCK::syscore);

    clockspeed = clockspeed;
    
    // the communication thread is already initted. 
           
    t_snsr = new embot::app::ctrl::tSNSR;  
    t_snsr->initialise({});  
        
   
    // init the fapreader ...
    
    embot::os::Thread *tComm = embot::app::skeleton::os::evthreadcan::getEVTthread();
    embot::os::Thread *tSnsr = t_snsr->getThread();

#if defined(USE_thePOSreader2)

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

    // init parser of POS CAN messages and link it to its agent: thePOSreader2
    embot::app::application::theCANparserPOS &canparserpos = embot::app::application::theCANparserPOS::getInstance();
    embot::app::application::theCANparserPOS::Config configparserpos { &thepos };
    canparserpos.initialise(configparserpos);  
    
#else
        
    embot::app::application::theFAPreader::Config configfapmod2par
    {
        embot::app::application::theFAPreader::AcquisitionMode::mod2parallel,    
        50*embot::core::time1millisec,  // acquisition time
        5*embot::core::time1millisec, // timeout for every acquisition step (1 step for fullyparallel, 2 for mod2parallel, n for daisychain)        
        tSnsr,          // reader thread 
        tComm,          // transmitter thread
        sfapsmod2par,
        { evtACQUIREfaps, evtNOREPLYfaps, embot::app::ctrl::evtTRANSMITfaps,  }    // associated events such as ... read sensor1, read sensor2, tranmsit sensors etc.
    };  

    embot::app::application::theFAPreader::Config configfapS4
    {
        embot::app::application::theFAPreader::AcquisitionMode::mod2parallel,        
        100*embot::core::time1millisec,  // acquisition time
        10*embot::core::time1millisec, // timeout for every acquisition step (1 step for fullyparallel, 2 for mod2parallel, n for daisychain)        
        tSnsr,          // reader thread 
        tComm,          // transmitter thread
        { s4, {}, {}, {}, {}, {} },          // vector of configured sensors 
        { evtACQUIREfaps, evtNOREPLYfaps, embot::app::ctrl::evtTRANSMITfaps,  }    // associated events such as ... read sensor1, read sensor2, tranmsit sensors etc.
    };  
    
    embot::app::application::theFAPreader::Config configfapdaisy
    {
        embot::app::application::theFAPreader::AcquisitionMode::daisychain,         
        100*embot::core::time1millisec,  // acquisition time
        10*embot::core::time1millisec, // timeout for every acquisition step (1 step for fullyparallel, 2 for mod2parallel, n for daisychain)        
        tSnsr,          // reader thread 
        tComm,          // transmitter thread
        sfapsdaisy,          // vector of configured sensors 
        { evtACQUIREfaps, evtNOREPLYfaps, embot::app::ctrl::evtTRANSMITfaps,  }    // associated events such as ... read sensor1, read sensor2, tranmsit sensors etc.
    };    
    embot::app::application::theFAPreader &thefap = embot::app::application::theFAPreader::getInstance();
    thefap.initialise(configfapmod2par);
    //thefap.initialise(configfapdaisy); 
    //thefap.initialise(configfapS4);     

    // init parser of POS CAN messages and link it to its agent: theFAPreader
    embot::app::application::theCANparserPOS &canparserpos = embot::app::application::theCANparserPOS::getInstance();
    embot::app::application::theCANparserPOS::Config configparserpos { &thefap };
    canparserpos.initialise(configparserpos);   
    
#endif    

    // start activities 
    t_ctrl = new embot::app::ctrl::tCTRL;     
//    t_ctrl->start({0, 1000*embot::core::time1microsec});  
    
    t_snsr->start();
    
    //thefap.start(100*embot::core::time1millisec);
     
                   
//    // init agent of imu
//    embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
//    embot::app::application::theIMU::Config configimu(embot::hw::bsp::strain2::imuBOSCH, embot::hw::bsp::strain2::imuBOSCHconfig, evIMUtick, evIMUdataready, evthr);
//    theimu.initialise(configimu);

//    // init canparser imu and link it to its agent
//    embot::app::application::theCANparserIMU &canparserimu = embot::app::application::theCANparserIMU::getInstance();
//    embot::app::application::theCANparserIMU::Config configparserimu { &theimu };
//    canparserimu.initialise(configparserimu);             
    
}


//namespace test
//{
//    embot::os::EventThread *tCOMM {nullptr};
//    embot::os::EventThread *tSNSR {nullptr};
//    
//    static constexpr embot::os::Event evtACQUIREfaps =embot::core::binary::mask::pos2mask<embot::os::Event>(1);
//    static constexpr embot::os::Event evtTRANSMITfaps = embot::core::binary::mask::pos2mask<embot::os::Event>(2); 

//    static constexpr embot::os::Event evtSNSR00_askdata = embot::core::binary::mask::pos2mask<embot::os::Event>(8);  
//    static constexpr embot::os::Event evtSNSR00_dataready = embot::core::binary::mask::pos2mask<embot::os::Event>(9);  
//    static constexpr embot::os::Event evtSNSR00_noreply = 0; //embot::core::binary::mask::pos2mask<embot::os::Event>(10);
//    static constexpr embot::os::Event evtSNSR01_askdata = embot::core::binary::mask::pos2mask<embot::os::Event>(11);  
//    static constexpr embot::os::Event evtSNSR01_dataready = embot::core::binary::mask::pos2mask<embot::os::Event>(12);  
//    static constexpr embot::os::Event evtSNSR01_noreply = 0; //embot::core::binary::mask::pos2mask<embot::os::Event>(13);
//    
//    using namespace embot::app::application;
//    
//    // this is the init of the system.
//    void tINIT_startup()
//    {
//        // create two threads
//        tCOMM = new embot::os::EventThread;
//        tSNSR = new embot::os::EventThread;
//        
//        // and init them 
//        // ... omissis so far
//        
//        // init theFAPreader
//        embot::app::application::theFAPreader &thefap = embot::app::application::theFAPreader::getInstance();
//        
//        constexpr theFAPreader::Sensor s0 {
//            embot::hw::TLV493D::one, 
//            embot::hw::tlv493d::Config{ embot::hw::i2c::Descriptor{embot::hw::I2C::one, embot::hw::i2c::Speed::standard100} }, 
//            evtSNSR00_askdata,
//            evtSNSR00_dataready,
//            evtSNSR00_noreply,
//            5*embot::core::time1millisec // timeout  
//        };
//        
//        constexpr theFAPreader::Sensor s1 {
//            embot::hw::TLV493D::two, 
//            embot::hw::tlv493d::Config{ embot::hw::i2c::Descriptor{embot::hw::I2C::one, embot::hw::i2c::Speed::standard100} }, 
//            evtSNSR01_askdata,
//            evtSNSR01_dataready,
//            evtSNSR01_noreply,
//            5*embot::core::time1millisec // timeout  
//        };        
//        constexpr std::array<theFAPreader::Sensor,theFAPreader::numberofpositions> sfaps { s0, s1 };
//        embot::app::application::theFAPreader::Config configfap
//        {
//            embot::app::application::theFAPreader::AcquisitionMode::daisychain, 
//            50*embot::core::time1millisec,  // acquisition time            
//            tSNSR,          // reader thread 
//            tCOMM,          // transmitter thread
//            sfaps,          // vector of configured sensors 
//            { evtACQUIREfaps, evtTRANSMITfaps,  }    // associated events such as ... read sensor1, read sensor2, tranmsit sensors etc.
//        };        
//        thefap.initialise(configfap); 
//        
//        // init parser of POS CAN messages and link it to its agent: theFAPreader
//        embot::app::application::theCANparserPOS &canparserpos = embot::app::application::theCANparserPOS::getInstance();
//        embot::app::application::theCANparserPOS::Config configparserpos { &thefap };
//        canparserpos.initialise(configparserpos); 
//    }
//    
//    
//    // this is executed by thread tCOMM when a can frame arrives.
//    // if the parser recognises one valid can frame, it decodes it and passes the relevant info to the associated agent.
//    // if any reply is required, the parser adds it by pushing it back inside the outframe vector.    
//    void tCOMM_userdefOnEventRXcanframe(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, 
//                                        const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &outframes)
//    {
//       
//        // process w/ the basic parser. if not recognised call the parsers specific of the board
//        if(true == embot::app::application::theCANparserBasic::getInstance().process(frame, outframes))
//        {                   
//        }
//        else if(true == embot::app::application::theCANparserPOS::getInstance().process(frame, outframes))
//        {
//            // in here the thread tCOMM has called the parser which calls methods of theFAPreader which for instance
//            // configure and start acquisition with a given rate.
//        }       
//       
//   }
//   
//   // this is executed by the tCOMM thread if an event other than RXcanframe is snt to it.
//   // for instance it could be an event sent by the thread tSNSR (or by a periodic timer) which 
//   // asks to tCOMM to retrieve the CAN frames with the values of some sensors and transmit them.
//   void tCOMM_userdefOnEventANYother(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, 
//                                      std::vector<embot::prot::can::Frame> &outframes)
//   {
//       
//        // the theFAPreader running on the tSNSR thread sends a transmission request to this tCOMM thread. 
//        // in here the tCOMM thread receives the tx request for the FAP values.
//        // tCOMM gets an handle of the object theFAPreader (which can work shared amongst threads)
//        // and it gets the frames to tx by means of the method process() which is thread-safe.
//        // at this point it has the outframes to transmit. 
//        if(true == embot::core::binary::mask::check(eventmask, evtTRANSMITfaps))
//        {   
//            embot::app::application::theFAPreader &thefap = embot::app::application::theFAPreader::getInstance(); 
//            thefap.get(outframes);            
//        }  

//        // the same as above can happen with other tx requests, for instance from theMCmanager (still to be designed)
//#if 0        
//        if(true == embot::core::binary::mask::check(eventmask, evtTXmcstatus))
//        {   
//            embot::app::application::theMCmanager &themc = embot::app::application::theMCmanager::getInstance(); 
//            themc.process(evtTXmcstatus, outframes);            
//        }           
//#endif
//        
//   }
//   
//   // this is what the tSNSR executes when triggered

//    void tSNSR_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
//    {   
//        if(0 == eventmask)
//        {   // timeout ...         
//            return;
//        }
//       
//        // we may separate the events or ... we can just pass the event mask to theFAPreader
////        if(true == embot::core::binary::mask::check(eventmask, evtSNSR00_askdata)) 
////        {   
//            // trigger acquisition of this sensor with a callback which sends an evt evtSNSR01_ready
//            embot::app::application::theFAPreader &thefap = embot::app::application::theFAPreader::getInstance(); 
//            thefap.process(eventmask);
////        }

//    }        
//   
//   
//   
//   
//}




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

