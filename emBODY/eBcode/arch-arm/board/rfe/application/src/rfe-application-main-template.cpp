

/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#include "embot_code_application_evntskcan.h"

// --------------------------------------------------------------------------------------------------------------------
// config start

constexpr embot::app::theCANboardInfo::applicationInfo applInfo 
{ 
    embot::app::canprotocol::versionOfAPPLICATION {1, 1, 3},    
    embot::app::canprotocol::versionOfCANPROTOCOL {2, 0}    
};

constexpr std::uint16_t taskIDLEstacksize = 512;
constexpr std::uint16_t taskINITstacksize = 2048;
constexpr std::uint16_t taskEVNTstacksize = 4096;
constexpr std::uint8_t maxINPcanframes = 16;
constexpr std::uint8_t maxOUTcanframes = 48;
constexpr embot::common::relTime taskEVNTtimeout = 50*embot::common::time1millisec;

static void *paramINIT = nullptr;
static void *paramIDLE = nullptr;
static void *paramERR = nullptr;
static void *paramEVNT = nullptr;

constexpr embot::sys::theTimerManager::Config tmcfg {};
constexpr embot::sys::theCallbackManager::Config cmcfg {};
    
    
static const embot::code::application::core::sysConfig syscfg { taskINITstacksize, paramINIT, taskIDLEstacksize, paramIDLE, paramERR, tmcfg, cmcfg};

static const embot::code::application::evntskcan::evtConfig evtcfg { taskEVNTstacksize, paramEVNT, taskEVNTtimeout};

static const embot::code::application::evntskcan::canConfig cancfg { maxINPcanframes, maxOUTcanframes };

// config end
// --------------------------------------------------------------------------------------------------------------------


class mySYS final : public embot::code::application::evntskcan::SYSTEMevtcan
{
public:
    mySYS(const embot::code::application::core::sysConfig &cfg) 
        : SYSTEMevtcan(cfg) {}
        
    void userdefOnIdle(embot::sys::Task *t, void* idleparam) const override;
    void userdefonOSerror(void *errparam) const override;
    void userdefInit_Extra(embot::sys::EventTask* evtsk, void *initparam) const override;
};


class myEVT final : public embot::code::application::evntskcan::EVNTSKcan
{
public:
    myEVT(const embot::code::application::evntskcan::evtConfig& ecfg, const embot::code::application::evntskcan::canConfig& ccfg, const embot::app::theCANboardInfo::applicationInfo& a) 
        : EVNTSKcan(ecfg, ccfg, a) {}
        
    void userdefStartup(embot::sys::Task *t, void *param) const override;
    void userdefOnTimeout(embot::sys::Task *t, embot::common::EventMask eventmask, void *param) const override;
    void userdefOnEventRXcanframe(embot::sys::Task *t, embot::common::EventMask eventmask, void *param, const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &outframes) const override;
    void userdefOnEventANYother(embot::sys::Task *t, embot::common::EventMask eventmask, void *param, std::vector<embot::hw::can::Frame> &outframes) const override;                   
};


static const mySYS mysys { syscfg };
static const myEVT myevt { evtcfg, cancfg, applInfo };
constexpr embot::code::application::evntskcan::CFG cfg{ &mysys, &myevt };

// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    embot::code::application::evntskcan::run(cfg);
    for(;;);    
}


// - here is the tailoring of the board.



#include "embot_hw_bsp_rfe.h"

#include "embot_sys_theScheduler.h"
#include "embot_app_theLEDmanager.h"
#include "embot_app_application_theCANparserBasic.h"

// TOFILL: put in here the includes for agents and the parsers
#include "embot_app_application_theCANparserIMU.h"
#include "embot_app_application_theIMU.h"

#include "embot_hw_usb.h"
#include "faceExpressionsModule.h"


constexpr embot::common::Event evRXusbmessage = 0x00000001 << 1;
constexpr embot::common::Event evIMUtick = 0x00000001 << 3;
constexpr embot::common::Event evIMUdataready = 0x00000001 << 4;

//expression
RfeApp::FaceExpressions faceExpressions;

volatile embot::common::Time t_load = 0;
volatile embot::common::Time t_display = 0;
volatile embot::common::Time t_total = 0;

struct Average_t
{
    embot::common::Time sum;
    std::uint32_t       count;
    embot::common::Time avg;
    float               avg_f;
    embot::common::Time min;
    embot::common::Time max;
    
    Average_t() : sum(0), count(0), avg(0), avg_f(0.0), min(3000000), max(0){;}
    void calculate(void){avg=sum/count;avg_f=sum/static_cast<float>(count);}
    void set(embot::common::Time val)
    {
        sum +=val;
        count++;
        if(val<min) min=val;
        if(val>max) max=val;
    }
};

Average_t loadAvg;
Average_t displayAvg;
Average_t totalAvg;

uint32_t totCount=0;


void mySYS::userdefOnIdle(embot::sys::Task *t, void* idleparam) const
{
    static int a = 0;
    a++;        
}

void mySYS::userdefonOSerror(void *errparam) const
{
    static int code = 0;
    embot::sys::theScheduler::getInstance().getOSerror(code);
    for(;;);    
}


void mySYS::userdefInit_Extra(embot::sys::EventTask* evtsk, void *initparam) const
{
    // inside the init task: put the init of many things ...  
    
    // led manager
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(embot::common::time1second); 

    // init of can basic paser
    embot::app::application::theCANparserBasic::getInstance().initialise({});
        
                   
    // init agent of imu
    embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
    embot::app::application::theIMU::Config configimu(embot::hw::bsp::rfe::imuBOSCH, embot::hw::bsp::rfe::imuBOSCHconfig, evIMUtick, evIMUdataready, evtsk);
    theimu.initialise(configimu);

    // init canparser imu and link it to its agent
    embot::app::application::theCANparserIMU &canparserimu = embot::app::application::theCANparserIMU::getInstance();
    embot::app::application::theCANparserIMU::Config configparserimu { &theimu };
    canparserimu.initialise(configparserimu);   
            
}

static void alerteventbasedtaskusb(void *arg)
{
    embot::sys::EventTask* evtsk = reinterpret_cast<embot::sys::EventTask*>(arg);
    if(nullptr != evtsk)
    {
        evtsk->setEvent(evRXusbmessage);
    }       
}


void myEVT::userdefStartup(embot::sys::Task *t, void *param) const
{
    // inside startup of evnt task: put the init of many things ... 

     embot::hw::usb::Config config;
     config.rxcapacity = 20;
     config.onrxmessage = embot::common::Callback(alerteventbasedtaskusb, t); 
     embot::hw::usb::init(embot::hw::usb::Port::one, config);
    
     faceExpressions.init();    
}


void myEVT::userdefOnTimeout(embot::sys::Task *t, embot::common::EventMask eventmask, void *param) const
{
    static uint32_t cnt = 0;
    cnt++;    
}


void myEVT::userdefOnEventRXcanframe(embot::sys::Task *t, embot::common::EventMask eventmask, void *param, const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &outframes) const
{        
    // process w/ the basic parser. if not recognised call the parsers specific of the board
    if(true == embot::app::application::theCANparserBasic::getInstance().process(frame, outframes))
    {                   
    }
    else if(true == embot::app::application::theCANparserIMU::getInstance().process(frame, outframes))
    {               
    }
 
}

void myEVT::userdefOnEventANYother(embot::sys::Task *t, embot::common::EventMask eventmask, void *param, std::vector<embot::hw::can::Frame> &outframes) const
{    
    if(true == embot::binary::mask::check(eventmask, evRXusbmessage))
    {        
        embot::hw::usb::Message msg;
        std::uint8_t remainingINrx = 0;
        if(embot::hw::resOK == embot::hw::usb::get(embot::hw::usb::Port::one, msg, remainingINrx))
        {   
            embot::common::Time t1 = embot::sys::now();
            faceExpressions.loadNewExpression(msg.data, msg.size);
            embot::common::Time t2 = embot::sys::now();
            faceExpressions.displayExpression();
            embot::common::Time t3 = embot::sys::now();
            
            t_load = t2-t1;
            t_display = t3-t2;
            t_total = t3-t1;
            
            loadAvg.set(t_load);
            displayAvg.set(t_display);
            totalAvg.set(t_total);
            
            totCount++;
            
            if(totCount==5)
            {
                loadAvg.calculate();
                displayAvg.calculate();
                totalAvg.calculate();
                totCount=0;
            }
            
            if(remainingINrx > 0)
            {
                t->setEvent(evRXusbmessage);                 
            }

        }        
    }
    
    
    if(true == embot::binary::mask::check(eventmask, evIMUtick))
    {        
        embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
        theimu.tick(outframes);        
    }   
    
    if(true == embot::binary::mask::check(eventmask, evIMUdataready))
    {        
        embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
        theimu.processdata(outframes);        
    }

}


   
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

