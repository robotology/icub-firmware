

//#include "EOtheLEDpulser.h"
#include "embot_app_canprotocol.h"
#include "embot_app_theCANboardInfo.h"

#include "embot.h"

#include "stm32hal.h" // to see bsp_led_init etc
#include "embot_hw.h"

#include "embot_hw_FlashStorage.h"
#include "embot_sys_theStorage.h"

#include "embot_app_theApplication.h"

#include "embot_app_application_theCANparserBasic.h"

#include "embot_app_application_theCANparserSTRAIN.h"
#include "embot_app_application_theCANparserIMU.h"

#include "embot_app_application_theSTRAIN.h"
#include "embot_app_application_theIMU.h"


static const embot::app::canprotocol::versionOfAPPLICATION vAP = {1, 0 , 3};
static const embot::app::canprotocol::versionOfCANPROTOCOL vCP = {2, 0};

static void userdeflauncher(void* param);
static void userdefonidle(void* param);

static const embot::common::Callback atsysteminit(userdeflauncher, nullptr);

static const embot::common::Callback onidle(userdefonidle, nullptr);

static const embot::app::theApplication::StackSizes stacksizes =  { 2048, 512 };

static const embot::app::theApplication::UserDefOperations operations = { atsysteminit, onidle, {nullptr, nullptr} };

#if defined(APPL_TESTZEROOFFSET)
static const std::uint32_t address = embot::hw::sys::addressOfBootloader;
#else
static const std::uint32_t address = embot::hw::sys::addressOfApplication;
#endif

int main(void)
{ 
    embot::app::theApplication::Config config(embot::common::time1millisec, stacksizes, operations, address);
    embot::app::theApplication &appl = embot::app::theApplication::getInstance();    
    
    appl.execute(config);  
        
    for(;;);    
}


static void start_evt_based(void);

static void userdeflauncher(void* param)
{
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    canbrdinfo.synch(vAP, vCP);
        
    start_evt_based();      
}



static void eventbasedtask_onevent(embot::sys::Task *t, embot::common::EventMask evtmsk, void *p);
static void eventbasedtask_init(embot::sys::Task *t, void *p);

static const embot::common::Event evRXcanframe = 0x00000001;
static const embot::common::Event evSTRAINprocess = 0x00000002;

static const std::uint8_t maxOUTcanframes = 48;

static embot::sys::EventTask* eventbasedtask = nullptr;

static void alerteventbasedtask(void *arg);

static std::vector<embot::hw::can::Frame> outframes;

static void start_evt_based(void)
{ 
    // start task waiting for can messages. 
    eventbasedtask = new embot::sys::EventTask;  
    const embot::common::relTime waitEventTimeout = 50*1000; //50*1000; //5*1000*1000;    
    eventbasedtask->init(eventbasedtask_init, eventbasedtask_onevent, 4*1024, 200, waitEventTimeout, nullptr, nullptr);    
        
    // start canparser basic
    embot::app::application::theCANparserBasic &canparserbasic = embot::app::application::theCANparserBasic::getInstance();
    embot::app::application::theCANparserBasic::Config configbasic;
    canparserbasic.initialise(configbasic);  
    
    // start canparser strain2
    embot::app::application::theCANparserSTRAIN &canparserstrain = embot::app::application::theCANparserSTRAIN::getInstance();
    embot::app::application::theCANparserSTRAIN::Config configparserstrain;
    canparserstrain.initialise(configparserstrain);  
    
    // start canparser imu
    embot::app::application::theCANparserIMU &canparserimu = embot::app::application::theCANparserIMU::getInstance();
    embot::app::application::theCANparserIMU::Config configparserimu;
    canparserimu.initialise(configparserimu);      

    // start application for strain2
    embot::app::application::theSTRAIN &thestrain = embot::app::application::theSTRAIN::getInstance();
    embot::app::application::theSTRAIN::Config configstrain;
    configstrain.tickevent = evSTRAINprocess;
    configstrain.totask = eventbasedtask;
    thestrain.initialise(configstrain); 

    // finally start can. i keep it as last because i dont want that the isr-handler calls its onrxframe() 
    // before the eventbasedtask is created.
    embot::hw::result_t r = embot::hw::resNOK;
    embot::hw::can::Config canconfig; // default is tx/rxcapacity=8
    canconfig.txcapacity = maxOUTcanframes;
    canconfig.onrxframe = embot::common::Callback(alerteventbasedtask, nullptr); 
    r = embot::hw::can::init(embot::hw::can::Port::one, canconfig);
    r = r;
}


static void alerteventbasedtask(void *arg)
{
    if(nullptr != eventbasedtask)
    {
        eventbasedtask->setEvent(evRXcanframe);
    }
}


static void eventbasedtask_init(embot::sys::Task *t, void *p)
{
    embot::hw::result_t r = embot::hw::can::enable(embot::hw::can::Port::one);  
    r = r;  

    outframes.reserve(maxOUTcanframes);
    #warning --> we should init the objects which holds outframes with maxOUTcanframes ... so that no more than maxOUTcanframes are pushed_back
}
    


static void eventbasedtask_onevent(embot::sys::Task *t, embot::common::EventMask eventmask, void *p)
{     
    if(0 == eventmask)
    {   // timeout ... 
        return;
    }
    
    // we clear the frames to be trasmitted
    outframes.clear();      
    
    
    if(true == embot::common::msk::check(eventmask, evRXcanframe))
    {        
        embot::hw::can::Frame frame;
        std::uint8_t remainingINrx = 0;
        if(embot::hw::resOK == embot::hw::can::get(embot::hw::can::Port::one, frame, remainingINrx))
        {            
            embot::app::application::theCANparserBasic &canparserbasic = embot::app::application::theCANparserBasic::getInstance();
            embot::app::application::theCANparserSTRAIN &canparserstrain = embot::app::application::theCANparserSTRAIN::getInstance();
            embot::app::application::theCANparserIMU &canparserimu = embot::app::application::theCANparserIMU::getInstance();
            // process w/ the basic parser, if not recognised call the parse specific of the board
            if(true == canparserbasic.process(frame, outframes))
            {                   
            }
            else if(true == canparserstrain.process(frame, outframes))
            {               
            }
            else if(true == canparserimu.process(frame, outframes))
            {               
            }
            
            if(remainingINrx > 0)
            {
                eventbasedtask->setEvent(evRXcanframe);                 
            }
        }        
    }
    
    
    if(true == embot::common::msk::check(eventmask, evSTRAINprocess))
    {
        
        embot::app::application::theSTRAIN &thestrain = embot::app::application::theSTRAIN::getInstance();
        thestrain.tick(outframes);        
    }
    
    // if we have any packet we transmit them
    std::uint8_t num = outframes.size();
    if(num > 0)
    {
        for(std::uint8_t i=0; i<num; i++)
        {
            embot::hw::can::put(embot::hw::can::Port::one, outframes[i]);                                       
        }

        embot::hw::can::transmit(embot::hw::can::Port::one);  
    } 
 
}

static void userdefonidle(void* param)
{
    static std::uint32_t cnt = 0;
    
    cnt++;
}


///


