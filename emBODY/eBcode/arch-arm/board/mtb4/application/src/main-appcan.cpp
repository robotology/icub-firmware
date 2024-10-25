

/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_app_skeleton_os_evthreadcan.h"

// --------------------------------------------------------------------------------------------------------------------
// config start

constexpr embot::app::theCANboardInfo::applicationInfo applInfo
{
#if defined(CUSTOMIZATION_MTB4_FOR_TLR)
    embot::prot::can::versionOfAPPLICATION {20, 23, 0},
    embot::prot::can::versionOfCANPROTOCOL {20, 0}
#elif defined(USE_FIFTH_I2C)
    embot::prot::can::versionOfAPPLICATION {51, 23, 0},
    embot::prot::can::versionOfCANPROTOCOL {2, 0}
#else
    embot::prot::can::versionOfAPPLICATION {1, 23, 0},
    embot::prot::can::versionOfCANPROTOCOL {2, 0}
#endif
};

//#define USE_FIFTH_I2C 

constexpr std::uint16_t threadIDLEstacksize = 512;
constexpr std::uint16_t threadINITstacksize = 2048;
constexpr std::uint16_t threadEVNTstacksize = 5*1024;
constexpr std::uint8_t maxINPcanframes = 16;
constexpr std::uint8_t maxOUTcanframes = 48;
constexpr embot::core::relTime threadEVNTtimeout = 50*embot::core::time1millisec;

static void *paramINIT = nullptr;
static void *paramIDLE = nullptr;
static void *paramERR = nullptr;
static void *paramEVNT = nullptr;

constexpr embot::os::theTimerManager::Config tmcfg {};
constexpr embot::os::theCallbackManager::Config cmcfg {};
    
    
static const embot::app::skeleton::os::basic::sysConfig syscfg { threadINITstacksize, paramINIT, threadIDLEstacksize, paramIDLE, paramERR, tmcfg, cmcfg};

static const embot::app::skeleton::os::evthreadcan::evtConfig evtcfg { threadEVNTstacksize, paramEVNT, threadEVNTtimeout};

static const embot::app::skeleton::os::evthreadcan::canConfig cancfg { maxINPcanframes, maxOUTcanframes };

// config end
// --------------------------------------------------------------------------------------------------------------------


class mySYS final : public embot::app::skeleton::os::evthreadcan::SYSTEMevtcan
{
public:
    mySYS(const embot::app::skeleton::os::basic::sysConfig &cfg) 
        : SYSTEMevtcan(cfg) {}
        
    void userdefOnIdle(embot::os::Thread *t, void* idleparam) const override;
    void userdefonOSerror(void *errparam) const override;
    void userdefInit_Extra(embot::os::EventThread* evtsk, void *initparam) const override;
};


class myEVT final : public embot::app::skeleton::os::evthreadcan::evThreadCAN
{
public:
    myEVT(const embot::app::skeleton::os::evthreadcan::evtConfig& ecfg, const embot::app::skeleton::os::evthreadcan::canConfig& ccfg, const embot::app::theCANboardInfo::applicationInfo& a) 
        : evThreadCAN(ecfg, ccfg, a) {}
        
    void userdefStartup(embot::os::Thread *t, void *param) const override;
    void userdefOnTimeout(embot::os::Thread *t, embot::os::EventMask eventmask, void *param) const override;
    void userdefOnEventRXcanframe(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &outframes) const override;
    void userdefOnEventANYother(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, std::vector<embot::prot::can::Frame> &outframes) const override;                   
};


static const mySYS mysys { syscfg };
static const myEVT myevt { evtcfg, cancfg, applInfo };
constexpr embot::app::skeleton::os::evthreadcan::CFG cfg{ &mysys, &myevt };

// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    embot::app::skeleton::os::evthreadcan::run(cfg);
    for(;;);    
}


// - here is the tailoring of the board.

#include "embot_hw_bsp_mtb4.h"

#include "embot_os_theScheduler.h"
#include "embot_app_theLEDmanager.h"
#include "embot_app_application_theCANparserBasic.h"

#include "embot_app_application_theSkin.h"
#include "embot_app_application_theCANparserIMU.h"
#include "embot_app_application_theIMU.h"
#include "embot_app_application_theCANparserTHERMO.h"
#include "embot_app_application_theTHERMO.h"

#if defined(ENABLE_FAPreader)
#include "embot_app_application_theFAPreader2.h"
#include "embot_app_application_theCANparserPOS.h"
#endif

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


using namespace embot::app::application;

constexpr embot::os::Event evt_RXcanframe_reserved  = embot::core::binary::mask::pos2mask<embot::os::Event>(0);
static_assert(evt_RXcanframe_reserved == embot::app::skeleton::os::evthreadcan::evThreadCAN::evRXcanframe, "this evt is reserved");


constexpr embot::os::Event evt_SKINprocess          = embot::core::binary::mask::pos2mask<embot::os::Event>(1);
constexpr embot::os::Event evt_IMUtick              = embot::core::binary::mask::pos2mask<embot::os::Event>(2);
constexpr embot::os::Event evt_IMUdataready         = embot::core::binary::mask::pos2mask<embot::os::Event>(3);
constexpr embot::os::Event evt_THERMOtick           = embot::core::binary::mask::pos2mask<embot::os::Event>(4);
constexpr embot::os::Event evt_THERMOdataready      = embot::core::binary::mask::pos2mask<embot::os::Event>(5);
 
#if defined(ENABLE_FAPreader)

constexpr embot::os::Event evt_POSacquire           = embot::core::binary::mask::pos2mask<embot::os::Event>(10);
constexpr embot::os::Event evt_POSnoreply           = embot::core::binary::mask::pos2mask<embot::os::Event>(11);
constexpr embot::os::Event evt_POStransmit          = embot::core::binary::mask::pos2mask<embot::os::Event>(12);

constexpr embot::os::Event evt_POSsnsr1_askdata     = embot::core::binary::mask::pos2mask<embot::os::Event>(13);  
constexpr embot::os::Event evt_POSsnsr1_dataready   = embot::core::binary::mask::pos2mask<embot::os::Event>(14);  
constexpr embot::os::Event evt_POSsnsr1_noreply     = 0; //embot::core::binary::mask::pos2mask<embot::os::Event>(15);
constexpr embot::os::Event evt_POSsnsr2_askdata     = embot::core::binary::mask::pos2mask<embot::os::Event>(16);  
constexpr embot::os::Event evt_POSsnsr2_dataready   = embot::core::binary::mask::pos2mask<embot::os::Event>(17);  
constexpr embot::os::Event evt_POSsnsr2_noreply     = 0; //embot::core::binary::mask::pos2mask<embot::os::Event>(18);
constexpr embot::os::Event evt_POSsnsr3_askdata     = embot::core::binary::mask::pos2mask<embot::os::Event>(19);  
constexpr embot::os::Event evt_POSsnsr3_dataready   = embot::core::binary::mask::pos2mask<embot::os::Event>(20);  
constexpr embot::os::Event evt_POSsnsr3_noreply     = 0; //embot::core::binary::mask::pos2mask<embot::os::Event>(21);
constexpr embot::os::Event evt_POSsnsr4_askdata     = embot::core::binary::mask::pos2mask<embot::os::Event>(22);  
constexpr embot::os::Event evt_POSsnsr4_dataready   = embot::core::binary::mask::pos2mask<embot::os::Event>(23);  
constexpr embot::os::Event evt_POSsnsr4_noreply     = 0; //embot::core::binary::mask::pos2mask<embot::os::Event>(24);



constexpr theFAPreader2::Sensor s1 {
    theFAPreader2::sensorType::tlv, 
    embot::hw::ANY::one,
    {   
        embot::prot::can::analog::posLABEL::zero, 
        {false, embot::prot::can::analog::polling::deciDegCalib::ROT::zero, 0}
    },
    evt_POSsnsr1_askdata,
    evt_POSsnsr1_dataready,
    evt_POSsnsr1_noreply,
    5*embot::core::time1millisec // timeout  
};

constexpr theFAPreader2::Sensor s2 {
    theFAPreader2::sensorType::tlv,
    embot::hw::ANY::two, 
    {         
        embot::prot::can::analog::posLABEL::one,  
        {false, embot::prot::can::analog::polling::deciDegCalib::ROT::zero, 0}
    },
    evt_POSsnsr2_askdata,
    evt_POSsnsr2_dataready,
    evt_POSsnsr2_noreply,
    5*embot::core::time1millisec // timeout  
};  

constexpr theFAPreader2::Sensor s3 {
    theFAPreader2::sensorType::tlv, 
    embot::hw::ANY::three, 
    {
        embot::prot::can::analog::posLABEL::two, 
        {false, embot::prot::can::analog::polling::deciDegCalib::ROT::zero, 0}
    },
    evt_POSsnsr3_askdata,
    evt_POSsnsr3_dataready,
    evt_POSsnsr3_noreply,
    5*embot::core::time1millisec // timeout  
};  

constexpr theFAPreader2::Sensor s4 {
    theFAPreader2::sensorType::tlv, 
    embot::hw::ANY::four,
    {  
        embot::prot::can::analog::posLABEL::three,  
        {false, embot::prot::can::analog::polling::deciDegCalib::ROT::zero, 0}
    },
    evt_POSsnsr4_askdata,
    evt_POSsnsr4_dataready,
    evt_POSsnsr4_noreply,
    5*embot::core::time1millisec // timeout  
}; 
    
constexpr std::array<theFAPreader2::Sensor, theFAPreader2::numberofpositions> snsrs4 
{ 
    s1, s2, s3, s4
};

#endif // #if defined(ENABLE_FAPreader)

void mySYS::userdefInit_Extra(embot::os::EventThread* evtsk, void *initparam) const
{
    // inside the init thread: put the init of many things ...  
    
    // led manager
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(embot::core::time1second); 

    // init of can basic paser
    embot::app::application::theCANparserBasic::getInstance().initialise({});
        
    // init agent of skin 
    embot::app::application::theSkin &theskin = embot::app::application::theSkin::getInstance();
    embot::app::application::theSkin::Config configskin;
    configskin.tickevent = evt_SKINprocess;
    configskin.totask = evtsk;
    theskin.initialise(configskin, true);   
    
    // init canparser skin and link it to its agent
    embot::app::application::theCANparserSkin &canparserskin = embot::app::application::theCANparserSkin::getInstance();
    embot::app::application::theCANparserSkin::Config configparserskin { &theskin };
    canparserskin.initialise(configparserskin);  
                   
    // init agent of imu
    embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
    embot::app::application::theIMU::Config configimu(embot::hw::bsp::mtb4::imuBOSCH, embot::hw::bsp::mtb4::imuBOSCHconfig, evt_IMUtick, evt_IMUdataready, evtsk);
    theimu.initialise(configimu);

    // init canparser imu and link it to its agent
    embot::app::application::theCANparserIMU &canparserimu = embot::app::application::theCANparserIMU::getInstance();
    embot::app::application::theCANparserIMU::Config configparserimu { &theimu };
    canparserimu.initialise(configparserimu);   
    
    // init agent of thermo
    embot::app::application::theTHERMO &thethermo = embot::app::application::theTHERMO::getInstance();
    embot::app::application::theTHERMO::Config configthermo(embot::hw::bsp::mtb4::thermometer, embot::hw::bsp::mtb4::thermometerconfig, evt_THERMOtick, evt_THERMOdataready, evtsk);
    thethermo.initialise(configthermo);  

    // init canparser thermo and link it to its agent
    embot::app::application::theCANparserTHERMO &canparserthermo = embot::app::application::theCANparserTHERMO::getInstance();
    embot::app::application::theCANparserTHERMO::Config configparserthermo { &thethermo };
    canparserthermo.initialise(configparserthermo);  

#if defined(ENABLE_FAPreader)
    // init agent of pos
    embot::app::application::theFAPreader2::Config configfapreader2
    {
        embot::app::application::theFAPreader2::AcquisitionMode::daisychain,    
        40*embot::core::time1millisec,  // acquisition period
        5*embot::core::time1millisec,   // timeout for every acquisition step (1 step for fullyparallel, n for daisychain)        
        evtsk,          // reader thread: receives events acquire, noreply + all those in teh cfg of the sensors
        evtsk,          // transmitter thread: receives event transmit
        snsrs4,
        { evt_POSacquire, evt_POSnoreply, evt_POStransmit,  } 
    };
    embot::app::application::theFAPreader2 &thefapreader2 = embot::app::application::theFAPreader2::getInstance();
    thefapreader2.initialise(configfapreader2, true);

    // init canparser pos and link it to its agent
    embot::app::application::theCANparserPOS &canparserpos = embot::app::application::theCANparserPOS::getInstance();
    embot::app::application::theCANparserPOS::Config configparserpos { &thefapreader2 };
    canparserpos.initialise(configparserpos);    
#endif        
}

void myEVT::userdefStartup(embot::os::Thread *t, void *param) const
{
    // inside startup of event thread: yiu can put the init of many things ...     
}


void myEVT::userdefOnTimeout(embot::os::Thread *t, embot::os::EventMask eventmask, void *param) const
{
    static uint32_t cnt = 0;
    cnt++;    
}


void myEVT::userdefOnEventRXcanframe(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &outframes) const
{        
    // process w/ the basic parser. if not recognised call the parsers specific of the board
    if(true == embot::app::application::theCANparserBasic::getInstance().process(frame, outframes))
    {                   
    }
    else if(true == embot::app::application::theCANparserSkin::getInstance().process(frame, outframes))
    {
#if defined(ENABLE_FAPreader)        
        // we have received a SKIN msg. so must deactivate the POS
        embot::app::application::theFAPreader2::getInstance().deactivate();     
#endif        
    }
    else if(true == embot::app::application::theCANparserIMU::getInstance().process(frame, outframes))
    {               
    }
    else if(true == embot::app::application::theCANparserTHERMO::getInstance().process(frame, outframes))
    {               
    }  
#if defined(ENABLE_FAPreader)    
    else if(true == embot::app::application::theCANparserPOS::getInstance().process(frame, outframes))
    {
        // we have received a POS msg. so must deactivate the SKIN
        embot::app::application::theSkin::getInstance().deactivate(); 
    }  
#endif    
}

void myEVT::userdefOnEventANYother(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, std::vector<embot::prot::can::Frame> &outframes) const
{
        
    if(true == embot::core::binary::mask::check(eventmask, evt_SKINprocess))
    {
        embot::app::application::theSkin &theskin = embot::app::application::theSkin::getInstance();
        theskin.tick(outframes);
        
        // we operate on the skin triangles by calling a skin.tick(outframes);
        // the evt_SKINprocess is emitted by a periodic timer started by the theSkin at the 
        // reception of a specific message.


        // the .tick(outframes) will do whatever it needs to do and it may emit some 
        // can frames for transmission. the can frames can be up to 16x2 = 32.        
    }
        
    if(true == embot::core::binary::mask::check(eventmask, evt_IMUtick))
    {        
        embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
        theimu.tick(outframes);        
    }   
    
    if(true == embot::core::binary::mask::check(eventmask, evt_IMUdataready))
    {        
        embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
        theimu.processdata(outframes);        
    }    

    if(true == embot::core::binary::mask::check(eventmask, evt_THERMOtick))
    {        
        embot::app::application::theTHERMO &thethermo = embot::app::application::theTHERMO::getInstance();
        thethermo.tick(outframes);        
    }   
    
    if(true == embot::core::binary::mask::check(eventmask, evt_THERMOdataready))
    {        
        embot::app::application::theTHERMO &thethermo = embot::app::application::theTHERMO::getInstance();
        thethermo.processdata(outframes);        
    }

#if defined(ENABLE_FAPreader)
    embot::app::application::theFAPreader2 &thefapreader2 = embot::app::application::theFAPreader2::getInstance();
    if(true == thefapreader2.isvalid(eventmask))
    {
        thefapreader2.process(eventmask, outframes);
    }
#endif
    
}


// - customizations
// - in here commenst about customizations
#if defined(CUSTOMIZATION_MTB4_FOR_TLR)
    #warning CUSTOMIZATION_MTB4_FOR_TLR is active: see what changes it takes
    // the TLR customization changes the IDs of the CAN frames emitted periodically to broadcast the 12 values of the skin taxel so that:
    // - the first frame has unchanged ID formed by the following three nibbles [cls | adr | trg] 
    //   with cls = 4 = embot::protocol::can::Cls::periodicSkin, adr = address of transmitting mtb4 board, trg = number of the triangle.
    // - the second frame has an ID different from standard protocol because the value of cls is now = 6 = embot::protocol::can::Cls::periodicForFutureUse
    // it also has a embot::prot::can::versionOfCANPROTOCOL {20, 0} 
#endif


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------





