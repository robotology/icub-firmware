

#include "embot_app_theCANboardInfo.h"
#include "embot.h"
#include "embot_common.h"
#include "embot_binary.h"
#include "stm32hal.h" // to see bsp_led_init etc
#include "embot_i2h.h"
#include "embot_hw.h"
#include "embot_hw_can.h"
#include "embot_hw_led.h"
#include "embot_hw_FlashStorage.h"
#include "embot_sys_theStorage.h"
#include "embot_app_theBootloader.h"
#include "embot_app_bootloader_theCANparser.h"
#include "embot_app_canprotocol.h"
#include "embot_app_theLEDmanager.h"
#include "embot_hw_flash.h"


struct ActivityParam
{
    uint32_t blinkingperiod;
};

static void bl_activity(void* param);

static const embot::common::relTime BlinkSlowPeriod = 500*embot::common::time1millisec;

static ActivityParam activity_param = { .blinkingperiod = BlinkSlowPeriod };


// use build 222 for all the updaterofbootloader
static const embot::app::canprotocol::versionOfAPPLICATION vAP = {2, 100 , 1};
static const embot::app::canprotocol::versionOfCANPROTOCOL vCP = {2, 0};

int main(void)
{ 
#if defined(APPL_TESTZEROOFFSET)
#else    
    embot::hw::sys::relocatevectortable(embot::hw::flash::getpartition(embot::hw::FLASH::application).address);
#endif
    
    embot::app::theBootloader & bootloader = embot::app::theBootloader::getInstance();
           
    activity_param.blinkingperiod = BlinkSlowPeriod;     
    embot::app::theBootloader::Config config;
    config.userdeflauncher = embot::common::Callback(bl_activity, &activity_param);   
    config.countdown = 0;
               
    bootloader.execute(config);
    
    for(;;);
    
}



static void eventbasedtask_onevent(embot::sys::Task *t, embot::common::Event evt, void *p);
static void eventbasedtask_init(embot::sys::Task *t, void *p);

static const embot::common::Event evRXcanframe = 0x00000001;

static embot::sys::EventTask* eventbasedtask = nullptr;

static void alerteventbasedtask(void *arg);

static std::vector<embot::hw::can::Frame> outframes;

static void bl_activity(void* param)
{
    // manage the led blinking
    ActivityParam* pp = (ActivityParam*) param;    
    uint32_t period = 0;    
    if(nullptr != pp)
    {
        period = pp->blinkingperiod;
    }    

    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(period, 0);     

    // manage the basic canboardinfo fot this special application    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    canbrdinfo.synch(vAP, vCP);
        
  
    // start task waiting for can messages.   
    const embot::common::relTime waitEventTimeout = 50*embot::common::time1millisec;    
    embot::sys::EventTask::Config configEV;    
    configEV.startup = eventbasedtask_init;
    configEV.onevent = eventbasedtask_onevent;
    configEV.param = nullptr;
    configEV.stacksize = 4*1024;
    configEV.priority = 200;
    configEV.timeout = waitEventTimeout;
    
    eventbasedtask = new embot::sys::EventTask;
    eventbasedtask->start(configEV);    
    
    // start canparser
    embot::app::bootloader::theCANparser &canparser = embot::app::bootloader::theCANparser::getInstance();
    embot::app::bootloader::theCANparser::Config config;
    config.owner = embot::app::canprotocol::Process::application;
    
    canparser.initialise(config);  

    // finally start can. i keep it as last because i dont want that the isr-handler calls its onrxframe() 
    // before the eventbasedtask is created.
    embot::hw::result_t r = embot::hw::resNOK;
    embot::hw::can::Config canconfig; // default is tx/rxcapacity=8
    canconfig.txcapacity = 12;
    canconfig.onrxframe = embot::common::Callback(alerteventbasedtask, nullptr); 
    r = embot::hw::can::init(embot::hw::CAN::one, canconfig);
    r = embot::hw::can::setfilters(embot::hw::CAN::one, embot::app::theCANboardInfo::getInstance().getCANaddress());
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
    embot::hw::result_t r = embot::hw::can::enable(embot::hw::CAN::one);  
    r = r;  

    outframes.reserve(12);
}
    


static void eventbasedtask_onevent(embot::sys::Task *t, embot::common::Event evt, void *p)
{  
    
    if(true == embot::binary::mask::check(evt, evRXcanframe))
    {        
        embot::hw::can::Frame frame;
        std::uint8_t remaining = 0;
        if(embot::hw::resOK == embot::hw::can::get(embot::hw::CAN::one, frame, remaining))
        {
            outframes.clear();            
            embot::app::bootloader::theCANparser &canparser = embot::app::bootloader::theCANparser::getInstance();
            if(true == canparser.process(frame, outframes))
            {
                std::uint8_t num = outframes.size();
                for(std::uint8_t i=0; i<num; i++)
                {
                    embot::hw::can::put(embot::hw::CAN::one, outframes[i]);                                       
                }

                embot::hw::can::transmit(embot::hw::CAN::one);
                
            }

            if(remaining > 0)
            {
                eventbasedtask->setEvent(evRXcanframe);                 
            }
        }        
    }
 
}


///


