
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
constexpr embot::core::relTime ostick {500*embot::core::time1microsec}; // {embot::core::time1millisec};
    
    
static const embot::app::skeleton::os::basic::sysConfig syscfg {    
    threadINITstacksize, 
    paramINIT, 
    threadIDLEstacksize, 
    paramIDLE, 
    paramERR, 
    tmcfg, 
    cmcfg,
    ostick};

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

#include "embot_app_application_theCANparserMBD.h"
#include "embot_app_application_theMBDagent.h"



#include "embot_os_rtos.h"

struct shared_t
{
    embot::os::rtos::mutex_t *rxm {nullptr};
    std::vector<embot::prot::can::Frame> rxCANvector {};
    embot::os::rtos::mutex_t *txm {nullptr};    
    std::vector<embot::prot::can::Frame> txCANvector {};  

    shared_t() 
    {
        rxm = embot::os::rtos::mutex_new();
        txm = embot::os::rtos::mutex_new();
        rxCANvector.reserve(10);
        txCANvector.reserve(10);
    } 

    size_t sizeofrx()
    {
        size_t r {0};
        embot::os::rtos::mutex_take(rxm, embot::core::reltimeWaitForever);
        r = rxCANvector.size();
        embot::os::rtos::mutex_release(rxm);    
        return r;
    }    
 

    size_t sizeoftx()
    {
        size_t r {0};
        embot::os::rtos::mutex_take(txm, embot::core::reltimeWaitForever);
        r = txCANvector.size();
        embot::os::rtos::mutex_release(txm);    
        return r;
    } 
    
    bool addrx(const embot::prot::can::Frame &f)
    {
        embot::os::rtos::mutex_take(rxm, embot::core::reltimeWaitForever);
        rxCANvector.push_back(f);
        embot::os::rtos::mutex_release(rxm);        
        return true;
    }

    bool getrx(embot::prot::can::Frame &f, size_t &remaining)
    {
        bool ret = false;
        remaining = 0;
        embot::prot::can::Frame ff {};
        embot::os::rtos::mutex_take(rxm, embot::core::reltimeWaitForever);
        if(!rxCANvector.empty())
        {
            ff = rxCANvector.front();
            f = ff;
            rxCANvector.erase(rxCANvector.begin());
            remaining = rxCANvector.size();
            ret = true;            
        }
        embot::os::rtos::mutex_release(rxm);        
        return ret;
    }  
    
    bool addtx(const embot::prot::can::Frame &f)
    {
        embot::os::rtos::mutex_take(txm, embot::core::reltimeWaitForever);
        txCANvector.push_back(f);
        embot::os::rtos::mutex_release(txm);        
        return true;
    } 

    bool gettx(embot::prot::can::Frame &f, size_t &remaining)
    {
        bool ret = false;
        remaining = 0;
        embot::prot::can::Frame ff {};
        embot::os::rtos::mutex_take(txm, embot::core::reltimeWaitForever);
        if(!txCANvector.empty())
        {
            ff = txCANvector.front();
            f = ff;
            txCANvector.erase(txCANvector.begin());
            remaining = txCANvector.size();
            ret = true;            
        }
        embot::os::rtos::mutex_release(txm);        
        return ret;
    }      
};

shared_t *shared {nullptr};

void s_shared_start()
{
    shared = new shared_t;
}

#include "embot_hw_sys.h"

embot::os::EventThread *t_COMM {nullptr};
embot::os::EventThread *t_CTRL {nullptr};

// maybe move API and implementation of the ctrl thread in dedicated files
void s_start_CTRL_thread();


void s_shared_start();


constexpr embot::core::relTime tickperiod = 2000*embot::core::time1millisec;


// to thread comm
constexpr embot::os::Event evt_COMM_tick = embot::core::binary::mask::pos2mask<embot::os::Event>(2);
constexpr embot::os::Event evt_COMM_canmctransmit = embot::core::binary::mask::pos2mask<embot::os::Event>(3);

// to thread ctrl
constexpr embot::os::Event evt_CTRL_tick {embot::os::bitpos2event(1)};
constexpr embot::os::Event evt_CTRL_canmcdecode {embot::os::bitpos2event(2)};


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
                              

    // set priority of the can thread
    t_COMM = evthr;
    evthr->setPriority(embot::os::Priority::high40);
        
                
    // start the control thread
    s_start_CTRL_thread();
        
    s_shared_start();
    
    embot::core::print("");
    embot::core::print("Quitting the os::Thread::InitThread and starting the os::theScheduler");          
}


void myEVT::userdefStartup(embot::os::Thread *t, void *param) const
{
    // inside startup of evnt thread: put the init of many things ... 
       
    //embot::core::print("userdefStartup(): start a timer which sends a tick event");
       
    // start a timer which sends a tick event
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evt_COMM_tick, t));
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
    else
    {
        // i pass the frame to another thread
        shared->addrx(frame);
        // i alert the thread
        t_CTRL->setEvent(evt_CTRL_canmcdecode);
    }

}



void myEVT::userdefOnEventANYother(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, std::vector<embot::prot::can::Frame> &outframes) const
{
 
    if(true == embot::core::binary::mask::check(eventmask, evt_COMM_canmctransmit)) 
    {
        size_t remaining {0};
        embot::prot::can::Frame frame {};
        if(true == shared->gettx(frame, remaining))
        {
            outframes.push_back(frame);
        }

        if(remaining > 0)
        {
            t->setEvent(evt_COMM_canmctransmit);
        }            
    
    }   
    
    if(true == embot::core::binary::mask::check(eventmask, evt_COMM_tick)) 
    {
   
    }   
    
}


// the CTRL thread is a embot::os::EventThread, so it executes:
// - only once its _startup() function, where we start a timer which 
//   triggers a tick event every 1 ms (for now 100 ms)
// - and then its _onevent() when an event is sent to it. 


// in the _startup() and _onevent() functions we can add what we want. 

// so far, we need to measure the excution time of some test code, so, we need two functions:
void mbd_mc_init();
void mbd_mc_canparse(const embot::prot::can::Frame &rxframe, std::vector<embot::prot::can::Frame> &outframes);
void mbd_mc_tick(std::vector<embot::prot::can::Frame> &inpframes, std::vector<embot::prot::can::Frame> &outframes);


// finally, from here there is the preparation of the t_CTRL thread


void tCTRL_startup(embot::os::Thread *t, void *param);
void tCTRL_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param);
// we use function tCTRL() so that we can see string "thrCTRL" in the window OS Support / Event Viewer
void thrCTRL(void* p) { embot::os::Thread *t = reinterpret_cast<embot::os::Thread*>(p); t->run(); }

embot::os::Timer * tCTRL_tickTimer {nullptr};
constexpr embot::core::relTime tCTRL_tickperiod {1*embot::core::time1millisec}; 

    
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

std::vector<embot::prot::can::Frame> outctrlframes {};
std::vector<embot::prot::can::Frame> inputframes {};

    
void tCTRL_startup(embot::os::Thread *t, void *param)
{
    
    outctrlframes.reserve(5);
    inputframes.reserve(5);

    // init agent of mc. it also init motor
    embot::app::application::theMBDagent &thembdagent = embot::app::application::theMBDagent::getInstance();
    thembdagent.initialise({});

    // init canparser mc and link it to its agent
    embot::app::application::theCANparserMBD &canparsermbd = embot::app::application::theCANparserMBD::getInstance();
    embot::app::application::theCANparserMBD::Config configparsermbd { &thembdagent };
    canparsermbd.initialise(configparsermbd);  

        
//    embot::core::print("tCTRL_startup(): starts a timer which sends a tick event every " + embot::core::TimeFormatter(tCTRL_tickperiod).to_string());
    
    // start a timer which ticks the thread CTRL
    tCTRL_tickTimer = new embot::os::Timer;
    embot::os::Action act(embot::os::EventToThread(evt_CTRL_tick, t));
    embot::os::Timer::Config cfg{tCTRL_tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tCTRL_tickTimer->start(cfg);  

//    embot::core::print("tCTRL_startup(): at every tick we shall execute a test action w/ measure of its duration");

    // ok, now all the rest
    mbd_mc_init();    
}


void tCTRL_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{
    
    if(0 == eventmask)
    {   // timeout ... 
        return;
    }
    
    outctrlframes.clear();
    
    if(true == embot::core::binary::mask::check(eventmask, evt_CTRL_canmcdecode))
    {
        size_t num = shared->sizeofrx();
        
        for(int i=0; i<num; i++)
        {
            size_t remaining {0};
            embot::prot::can::Frame frame {};
            if(true == shared->getrx(frame, remaining))
            {
                inputframes.push_back(frame);
            }
        }           
    }    
    
    if(true == embot::core::binary::mask::check(eventmask, evt_CTRL_tick))
    {        
//        embot::core::print(std::string("tCTRL_onevent(evt_CTRL_tick): @ ") + embot::core::TimeFormatter(embot::core::now()).to_string());                 
        mbd_mc_tick(inputframes, outctrlframes);
       
    } 

    
    // now trigger transmission (if any)
    size_t ss = outctrlframes.size();
    if(ss > 0)
    {
        for(size_t i=0; i<ss; i++)
        {
            shared->addtx(outctrlframes[i]);
        }
    }
            
    if(shared->sizeoftx() > 0)
    {
        t_COMM->setEvent(evt_COMM_canmctransmit);
    }    
    
}


// interface for the model-based-designed control which is inside embot::app::application::theMBDagent

// called just once at startup
void mbd_mc_init()
{     
    embot::app::application::theMBDagent::getInstance().initialise({});
}

// called in asynch mode when a can frame arrives. 
// it must:
// - process the frame, 
// - do actions, 
// - fill in the reply can frames
void mbd_mc_canparse(const embot::prot::can::Frame &rxframe,
                     std::vector<embot::prot::can::Frame> &outframes)
{
    // now we use the c++ parser / agent
    embot::app::application::theCANparserMBD::getInstance().process(rxframe, outframes);
}

// called every 1 ms and always after mbd_mc_canparse(). 
// it may add can frames to be transmitted, e.g., those containing
// the status of the control
void mbd_mc_tick(std::vector<embot::prot::can::Frame> &inpframes, std::vector<embot::prot::can::Frame> &outframes)
{ 
    // now we use the c++ agent to tick control and get an output 
    // vector of frames to transmit  

//    size_t num = shared->sizeofrx();
//    
//    for(int i=0; i<num; i++)
//    {
//        size_t remaining {0};
//        embot::prot::can::Frame frame {};
//        if(true == shared->getrx(frame, remaining))
//        {
//            mbd_mc_canparse(frame, outctrlframes);
//            //embot::app::application::theCANparserMC::getInstance().process(frame, outctrlframes);
//        }
//    }
//    
//    std::vector<embot::prot::can::Frame> inpframes {};

//    size_t num = shared->sizeofrx();
//    
//    for(int i=0; i<num; i++)
//    {
//        size_t remaining {0};
//        embot::prot::can::Frame frame {};
//        if(true == shared->getrx(frame, remaining))
//        {
//            inpframes.push_back(frame);
//        }
//    }
   
        
    embot::app::application::theMBDagent::getInstance().tick(inpframes, outframes);
    inputframes.clear();
    
}




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



