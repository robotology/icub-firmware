

/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#include "embot_app_skeleton_os_evthreadcan.h"

//#define DO_TEST

#if defined(DO_TEST)
void test(uint32_t cnt);
constexpr uint32_t numMilli = 1000;
#else
constexpr uint32_t numMilli = 50;
#endif

// --------------------------------------------------------------------------------------------------------------------
// config start

constexpr embot::app::theCANboardInfo::applicationInfo applInfo 
{ 
    embot::prot::can::versionOfAPPLICATION {1, 2, 1},    
    embot::prot::can::versionOfCANPROTOCOL {2, 0}    
};

constexpr std::uint16_t threadIDLEstacksize = 512;
constexpr std::uint16_t threadINITstacksize = 2048;
constexpr std::uint16_t threadEVNTstacksize = 4096+1024;
constexpr std::uint8_t maxINPcanframes = 16;
constexpr std::uint8_t maxOUTcanframes = 48;
constexpr embot::core::relTime threadEVNTtimeout = numMilli*embot::core::time1millisec;

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


#include "embot_os_theScheduler.h"
#include "embot_app_theLEDmanager.h"
#include "embot_app_application_theCANparserBasic.h"

// TOFILL: put in here the includes for agents and the parsers
#include "embot_app_application_theCANparserIMU.h"
#include "embot_app_application_theIMU.h"

#include "embot_hw_usb.h"
#include "faceExpressionsModule.h"

#include "embot_app_application_theCANtracer.h"

namespace embot { namespace hw { namespace bsp { namespace rfe {    
    const embot::hw::LED ledBLUE = embot::hw::LED::one;    
    //const embot::hw::i2c::Descriptor descrI2Cone = embot::hw::i2c::Descriptor(embot::hw::I2C::one, 400000);    
    const embot::hw::BNO055 imuBOSCH = embot::hw::BNO055::one;
    const embot::hw::bno055::Config imuBOSCHconfig = {};        
}}}} // namespace embot { namespace hw { namespace bsp { namespace rfe {


constexpr embot::os::Event evRXusbmessage = 0x00000001 << 1;
constexpr embot::os::Event evIMUtick = 0x00000001 << 3;
constexpr embot::os::Event evIMUdataready = 0x00000001 << 4;

//expression
RfeApp::FaceExpressions faceExpressions;

//volatile embot::core::Time t_load = 0;
//volatile embot::core::Time t_display = 0;
//volatile embot::core::Time t_total = 0;

//struct Average_t
//{
//    embot::core::Time sum;
//    std::uint32_t       count;
//    embot::core::Time avg;
//    float               avg_f;
//    embot::core::Time min;
//    embot::core::Time max;
//    
//    Average_t() : sum(0), count(0), avg(0), avg_f(0.0), min(3000000), max(0){;}
//    void calculate(void){avg=sum/count;avg_f=sum/static_cast<float>(count);}
//    void set(embot::core::Time val)
//    {
//        sum +=val;
//        count++;
//        if(val<min) min=val;
//        if(val>max) max=val;
//    }
//};

//Average_t loadAvg;
//Average_t displayAvg;
//Average_t totalAvg;

//uint32_t totCount=0;


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
        
                   
    // init agent of imu
    embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
    embot::app::application::theIMU::Config configimu(embot::hw::bsp::rfe::imuBOSCH, embot::hw::bsp::rfe::imuBOSCHconfig, evIMUtick, evIMUdataready, evtsk);
    theimu.initialise(configimu);

    // init canparser imu and link it to its agent
    embot::app::application::theCANparserIMU &canparserimu = embot::app::application::theCANparserIMU::getInstance();
    embot::app::application::theCANparserIMU::Config configparserimu { &theimu };
    canparserimu.initialise(configparserimu);   
    
}

static void alerteventbasedthreadusb(void *arg)
{
    embot::os::EventThread* evtsk = reinterpret_cast<embot::os::EventThread*>(arg);
    if(nullptr != evtsk)
    {
        evtsk->setEvent(evRXusbmessage);
    }       
}


void myEVT::userdefStartup(embot::os::Thread *t, void *param) const
{
    // inside startup of evnt thread: put the init of many things ... 

    embot::hw::usb::Config config;
    config.rxcapacity = 16;
    config.onrxmessage = embot::core::Callback(alerteventbasedthreadusb, t); 
    embot::hw::usb::init(embot::hw::usb::Port::one, config);
    
    faceExpressions.init(RfeApp::Expression_t::neutral, RfeApp::Color::white, RfeApp::Brightness::medium);    
}



void myEVT::userdefOnTimeout(embot::os::Thread *t, embot::os::EventMask eventmask, void *param) const
{
    static uint32_t cnt = 0;
    cnt++; 
    
#if defined(DO_TEST)
    test(cnt);
#endif  

}


void myEVT::userdefOnEventRXcanframe(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &outframes) const
{        
    // process w/ the basic parser. if not recognised call the parsers specific of the board
    if(true == embot::app::application::theCANparserBasic::getInstance().process(frame, outframes))
    {                   
    }
    else if(true == embot::app::application::theCANparserIMU::getInstance().process(frame, outframes))
    {               
    }
 
}

void myEVT::userdefOnEventANYother(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, std::vector<embot::prot::can::Frame> &outframes) const
{ 
    if(true == embot::core::binary::mask::check(eventmask, evRXusbmessage))
    {        
        embot::hw::usb::Message msg;
        std::uint8_t remainingINrx = 0;
        if(embot::hw::resOK == embot::hw::usb::get(embot::hw::usb::Port::one, msg, remainingINrx))
        {   
            faceExpressions.processcommands(msg.data, msg.size);
            faceExpressions.displayExpression();
#undef RFE_SEND_CANPRINT            
#if defined(RFE_SEND_CANPRINT)                        
            RfeApp::Error errorvalue = faceExpressions.getError();
            if(RfeApp::Error::none != errorvalue)
            {
                embot::app::theCANtracer &tracer = embot::app::theCANtracer::getInstance();
                if(RfeApp::Error::SPIwasbusy == errorvalue)
                {
                    tracer.print("RFE spi busy", outframes);
                }
                else if(RfeApp::Error::SPIfailure == errorvalue)
                {
                    tracer.print("RFE spi fail", outframes);
                }              
            }
#endif            
            
            if(remainingINrx > 0)
            {
                t->setEvent(evRXusbmessage);                 
            }

        }        
    }
    
    
    if(true == embot::core::binary::mask::check(eventmask, evIMUtick))
    {        
        embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
        theimu.tick(outframes);        
    }   
    
    if(true == embot::core::binary::mask::check(eventmask, evIMUdataready))
    {        
        embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
        theimu.processdata(outframes);        
    }

}

// test area

#if defined(DO_TEST)

static uint8_t bufferZbe[] =  // i use big endianess for words 
{
    'Z',
    // left 
    '0', '2', // red
    '0', '4', // high
    '0', '0', '0', '1', '1', '1', '1', '1', // top line 0x00011111
    // rigth
    '0', 'D', // purple
    '0', '4', // high
    '0', '0', '0', '1', '1', '1', '1', '1', // top line (be)
    // mouth    
    '0', '1', // white
    '0', '4', // high
    '0', '0', '0', '4', '5', '6', '4', '5' // happy (be)
};


static uint8_t bufferZle[] =  // i use little endianess for words 
{
    'Z',
    // left 
    '0', '2', // red
    '0', '4', // high
    '1', '1', '1', '1', '0', '1', '0', '0', // top line (le)
    // rigth
    '0', 'D', // purple
    '0', '4', // high
    '1', '1', '1', '1', '0', '1', '0', '0', // top line (le)
    // mouth    
    '0', '1', // white
    '0', '4', // high
    '4', '5', '5', '6', '0', '4', '0', '0' // happy (le)
};

static uint8_t bufferB[] =  // i use ... 
{
    'B',
    '0', '1'
};

static uint8_t bufferE[] =  // all parte have expression 
{
    'E',
    '0', '2' // sad
};

static uint8_t bufferECBm[] =  // all parte have expression 
{
    'E',    // expression for all
    '0', '2', // sad
    ' ', // separator 
    'C',    // color for all
    '0', '4', // blue
    ',', // separator 
    'B',    // brightness for all
    '0', '2', // low
    ' ', // separator 
    'm',      // mouth multiple 
    '0', '3', // suprised
    '0', '2', // red
    '0', '5', // maximum   
    ' ', // separator 
    'l',      // left multiple 
    '0', '0', // neutral
    '0', '2', // red
    '0', '5', // maximum      
    ' ', // separator 
    'a',      // all multiple 
    '0', '0', // neutral
    '0', '1', // white
    '0', '5', // max 
    's', 2, 3, 5    
    
};

static uint8_t *buffer = bufferZbe;
static size_t sizeofbuffer = sizeof(bufferZbe);

void test(uint32_t cnt)
{
    //    faceExpressions.rotate(cnt);
    static volatile uint32_t picture = 0;
    picture = 0;
//    embot::core::binary::bit::set(picture, cnt%20);
    picture = 0x00032222;
    //faceExpressions.display(RfeApp::FacePart_t::all, RfeApp::Color::white, RfeApp::Brightness::medium, picture);
    volatile RfeApp::Brightness br = RfeApp::tobrightness(cnt%RfeApp::brightnessMaxNum);
   // RfeApp::PartProps pp { RfeApp::Color::purple, br, picture };
    //faceExpressions.display(RfeApp::FacePart_t::all, pp);
    
    //faceExpressions.display(RfeApp::FacePart_t::all, RfeApp::Expression_t::evil, RfeApp::Color::purple, br);
//    std::array<RfeApp::Color, 3> cols { RfeApp::Color::purple,  RfeApp::Color::purple,  RfeApp::Color::red };
//    faceExpressions.display(RfeApp::Expression_t::evil, cols, br);
    
    faceExpressions.processcommands(buffer, sizeofbuffer, false);
    faceExpressions.displayExpression();
}

#endif // #if defined(DO_TEST)
   
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

