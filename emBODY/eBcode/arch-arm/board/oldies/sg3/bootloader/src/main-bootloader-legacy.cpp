
#include "embot_app_theCANboardInfo.h"

#include "embot.h"
#include "embot_common.h"
#include "embot_binary.h"

#include "stm32hal.h" // to see bsp_led_init etc

#include "embot_hw.h"
#include "embot_hw_led.h"

#include "embot_hw_FlashStorage.h"
#include "embot_sys_theStorage.h"

#include "embot_app_theBootloader.h"
#include "embot_app_bootloader_theCANparser.h"

#include "embot_app_canprotocol.h"

#include "embot_app_theLEDmanager.h"


struct ActivityParam
{
    uint32_t blinkingperiod;
};

static void bl_activity(void* param);


static const std::uint8_t NumOfSecondsBeforeJump = 5; 
static const embot::core::relTime BlinkFastPeriod = 200*embot::core::time1millisec;
static const embot::core::relTime BlinkSlowPeriod = 500*embot::core::time1millisec;
static const embot::core::relTime BlinkMadlyPeriod = 50*embot::core::time1millisec;

static ActivityParam activity_param = { .blinkingperiod = BlinkFastPeriod };

static const embot::app::canprotocol::versionOfBOOTLOADER vBL = {1, 2};
static const std::uint8_t defADDRESS = 13;
static const char defaultInfo32[] = {"I am a sg3"};

int main(void)
{ 
    embot::app::theBootloader & bootloader = embot::app::theBootloader::getInstance();

    std::uint32_t param = 0;    
    embot::app::theJumper::Command cmd = bootloader.getcommand(param);
           
    activity_param.blinkingperiod = BlinkFastPeriod;
    embot::app::theBootloader::Config config;
    config.userdeflauncher = embot::core::Callback(bl_activity, &activity_param);
    config.countdown = NumOfSecondsBeforeJump*embot::core::time1second;
    
    switch(cmd)
    {
        case embot::app::theJumper::Command::none:
        {   // we dont change what we want to do: we exec for at 5 seconds
        } break;
        
        case embot::app::theJumper::Command::stay:
        {   // we have received a command from the previous running process to stay in here forever
            config.countdown = 0;
            activity_param.blinkingperiod = BlinkSlowPeriod;
        } break; 
        
        case embot::app::theJumper::Command::jump:
        {   // we have received command to jump to an address
            bootloader.jump(param); 
            // if we cannot jump to the specified address ... we stay in here forever
            config.countdown = 0;
            activity_param.blinkingperiod = BlinkMadlyPeriod;
        } break;          
    }

    
    bootloader.execute(config);
    
    for(;;);
    
}





static void eventbasedtask_onevent(embot::sys::Task *t, embot::core::Event evt, void *p);
static void eventbasedtask_init(embot::sys::Task *t, void *p);

static const embot::core::Event evRXcanframe = 0x00000001;

static embot::sys::EventTask* eventbasedtask = nullptr;

static void alerteventbasedtask(void *arg);

static std::vector<embot::hw::can::Frame> outframes;


static void bl_activity(void* param)
{
    // manage the led blinking
    ActivityParam* pp = (ActivityParam*) param;    
    embot::core::relTime period = 0;    
    if(nullptr != pp)
    {
        period = pp->blinkingperiod;
    }    

    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(period, 0); 
       

    // manage the basic canboardinfo
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();    
    canbrdinfo.synch(embot::app::canprotocol::Board::sg3, vBL, defADDRESS, defaultInfo32);
    
  
    // start task waiting for can messages.  
    const embot::core::relTime waitEventTimeout = 50*1000; //50*1000; //5*1000*1000;            
    embot::sys::EventTask::Config configEV;    
    configEV.startup = eventbasedtask_init;
    configEV.onevent = eventbasedtask_onevent;
    configEV.param = nullptr;
    configEV.stacksize = 4*1024;
    configEV.priority = embot::sys::Priority::high200;
    configEV.timeout = waitEventTimeout;
    
    eventbasedtask = new embot::sys::EventTask; 
    eventbasedtask->start(configEV);
        
    // start canparser
    embot::app::bootloader::theCANparser &canparser = embot::app::bootloader::theCANparser::getInstance();
    embot::app::bootloader::theCANparser::Config config;
    canparser.initialise(config);  

    // finally start can. i keep it as last because i dont want that the isr-handler calls its onrxframe() 
    // before the eventbasedtask is created.
    embot::hw::result_t r = embot::hw::resNOK;
    embot::hw::can::Config canconfig; // default is tx/rxcapacity=8
    canconfig.txcapacity = 12;
    canconfig.onrxframe = embot::core::Callback(alerteventbasedtask, nullptr); 
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
    


static void eventbasedtask_onevent(embot::sys::Task *t, embot::core::EventMask eventmask, void *p)
{  
    
    if(true == embot::core::binary::mask::check(eventmask, evRXcanframe))
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


