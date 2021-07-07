
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_app_skeleton_os_evthreadcan.h"

// --------------------------------------------------------------------------------------------------------------------
// config start

constexpr embot::app::theCANboardInfo::applicationInfo applInfo 
{ 
    embot::prot::can::versionOfAPPLICATION {1, 0, 0},    
    embot::prot::can::versionOfCANPROTOCOL {2, 0}    
};

constexpr std::uint16_t threadIDLEstacksize = 4096;
constexpr std::uint16_t threadINITstacksize = 4096;
constexpr std::uint16_t threadEVNTstacksize = 4096;
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
    void userdefInit_Extra(embot::os::EventThread* evthr, void *initparam) const override;
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

#define enableTRACE_all


#include "embot_os_theScheduler.h"
#include "embot_app_theLEDmanager.h"
#include "embot_app_application_theCANparserBasic.h"
#include "embot_app_application_theCANtracer.h"

#include "embot_hw_bsp.h"
#include "embot_hw_gpio.h"
#include "embot_hw_can.h"

#include "embot_app_application_theCANparserMC.h"
#include "embot_app_application_theMCagent.h"


#include "embot_hw_sys.h"


// maybe move API and implementation of the ctrl thread in dedicated files
void s_start_CTRL_thread();


constexpr embot::os::Event evtTick = embot::core::binary::mask::pos2mask<embot::os::Event>(2);
constexpr embot::core::relTime tickperiod = 2000*embot::core::time1millisec;



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


void mySYS::userdefInit_Extra(embot::os::EventThread* evthr, void *initparam) const
{
    // inside the init thread: put the init of many things ...  
    
    // led manager
    static const std::initializer_list<embot::hw::LED> alltheleds = { 
        embot::hw::LED::one, embot::hw::LED::two, embot::hw::LED::three
    };  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(alltheleds);   
    
    theleds.get(embot::hw::LED::one).pulse(embot::core::time1second); 

       
    embot::app::LEDwaveT<64> ledwave(100*embot::core::time1millisec, 30, std::bitset<64>(0b000001));
    theleds.get(embot::hw::LED::two).wave(&ledwave);     

    
    // init of can basic paser
    embot::app::application::theCANparserBasic::getInstance().initialise({});
                              
    // init agent of mc
    embot::app::application::theMCagent &themcagent = embot::app::application::theMCagent::getInstance();
    themcagent.initialise({});

    // init canparser mc and link it to its agent
    embot::app::application::theCANparserMC &canparsermc = embot::app::application::theCANparserMC::getInstance();
    embot::app::application::theCANparserMC::Config configparsermc { &themcagent };
    canparsermc.initialise(configparsermc);  


    // set priority of the can thread
    evthr->setPriority(embot::os::Priority::high40);
        
    // start the control thread
    s_start_CTRL_thread();
    
    embot::core::print("");
    embot::core::print("Quitting the os::Thread::InitThread and starting the os::theScheduler");          
}


void myEVT::userdefStartup(embot::os::Thread *t, void *param) const
{
    // inside startup of evnt thread: put the init of many things ... 
       
    //embot::core::print("userdefStartup(): start a timer which sends a tick event");
       
    // start a timer which sends a tick event
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtTick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);          
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
    else if(true == embot::app::application::theCANparserMC::getInstance().process(frame, outframes))
    {               
    }

}



void myEVT::userdefOnEventANYother(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, std::vector<embot::prot::can::Frame> &outframes) const
{
    
    static embot::core::Time startOfFAPacquisition = 0;
    
    if(true == embot::core::binary::mask::check(eventmask, evtTick)) 
    {
   
    }   
    
}


// the CTRL thread is a embot::os::EventThread, so it executes:
// - only once its _startup() function, where we start a timer which 
//   triggers a tick event every 1 ms (for now 100 ms)
// - and then its _onevent() when an event is sent to it. 


// in the _startup() and _onevent() functions we can add what we want. 

// so far, we need to measure the excution time of some test code, so, we need two functions:
void test_init();
void test_tick();
// which init and tick the following:
// - two functions: amcbldc::testcode::init(), amcbldc::testcode::tick()
// - a class embot::app::scope::Signal which measures the duration of the above tick() by using
//   - either a print of duration time
//   - or a GPIO rised and lowred which can be measured with a true oscilloscope, 
//   - or the Event Viewer tool visible by opening `Debug / OS Support / ....`.
// here are the required include files
#include "amcbldc_codetotest.h"
#include "embot_app_scope.h"

// just to see some GPIO transformations
#include "embot_hw_gpio_bsp.h"

// i also use motors
#include "embot_hw_motor.h"

// finally, from here there is the preparation of the t_CTRL thread

embot::os::EventThread *t_CTRL {nullptr};
void tCTRL_startup(embot::os::Thread *t, void *param);
void tCTRL_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param);
// we use function tCTRL() so that we can see string "thrCTRL" in the window OS Support / Event Viewer
void thrCTRL(void* p) { embot::os::Thread *t = reinterpret_cast<embot::os::Thread*>(p); t->run(); }

constexpr embot::os::Event evt_CTRL_tick {embot::os::bitpos2event(1)};
embot::os::Timer * tCTRL_tickTimer {nullptr};
constexpr embot::core::relTime tCTRL_tickperiod {1000*embot::core::time1millisec}; 

    
void s_start_CTRL_thread()
{        
    t_CTRL = new embot::os::EventThread;
    
    embot::os::EventThread::Config tConfig { 
        6*1024, 
        embot::os::Priority::high47, 
        tCTRL_startup,
        nullptr,
        200*embot::core::time1millisec, // or also ... timeout embot::core::reltimeWaitForever
        tCTRL_onevent,
        "thrCTRL"
    };
    
    t_CTRL->start(tConfig, thrCTRL);      
}

void tCTRL_startup(embot::os::Thread *t, void *param)
{           
    embot::core::print("tCTRL_startup(): starts a timer which sends a tick event every " + embot::core::TimeFormatter(tCTRL_tickperiod).to_string());
    
    // start a timer which ticks the thread CTRL
    tCTRL_tickTimer = new embot::os::Timer;
    embot::os::Action act(embot::os::EventToThread(evt_CTRL_tick, t));
    embot::os::Timer::Config cfg{tCTRL_tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tCTRL_tickTimer->start(cfg);  

    embot::core::print("tCTRL_startup(): at every tick we shall execute a test action w/ measure of its duration");

    // ok, now all the rest
    test_init();    
}


void tCTRL_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{
    
    if(0 == eventmask)
    {   // timeout ... 
        return;
    }
    
    
    if(true == embot::core::binary::mask::check(eventmask, evt_CTRL_tick))
    {        
//        embot::core::print(std::string("tCTRL_onevent(evt_CTRL_tick): @ ") + embot::core::TimeFormatter(embot::core::now()).to_string());                 
        test_tick();
    }       
    
}



constexpr embot::app::scope::SignalType signaltype = embot::app::scope::SignalType::Dummy;
//constexpr embot::app::scope::SignalType signaltype = embot::app::scope::SignalType::EViewer;
//constexpr embot::app::scope::SignalType signaltype = embot::app::scope::SignalType::Print;
//constexpr embot::app::scope::SignalType signaltype = embot::app::scope::SignalType::GPIO;

embot::app::scope::Signal * signal {nullptr}; 

void test_init()
{     
    // initialize the code to test
    amcbldc::codetotest::init();

    // initialize the tool which measure the duration of the code to test
    if(embot::app::scope::SignalType::Dummy == signaltype)
    {       
        signal = new embot::app::scope::SignalDummy({});
    }
    else if(embot::app::scope::SignalType::EViewer == signaltype)
    {       
        signal = new embot::app::scope::SignalEViewer({amcbldc::codetotest::tick, embot::app::scope::SignalEViewer::Config::LABEL::one});
    }
    else if(embot::app::scope::SignalType::Print == signaltype)
    {
        signal = new embot::app::scope::SignalPrint({"TOCK"});
    }
    else if(embot::app::scope::SignalType::GPIO == signaltype)
    {
        constexpr embot::hw::GPIO gpioLED3 = {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::fifteen};
        //constexpr embot::hw::GPIO gpioI2Cscl = {embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::six}; // to attach it to the scope
        signal = new embot::app::scope::SignalGPIO({gpioLED3, embot::hw::gpio::State::RESET});
    }
    
}


void test_tick()
{    
    signal->on();    
    amcbldc::codetotest::tick();
    signal->off();    
}




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



