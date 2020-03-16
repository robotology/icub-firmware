


#include "embot_app_theCANboardInfo.h"


#include "embot_core.h"
#include "embot_core_binary.h"

#include "stm32hal.h" // to see bsp_led_init etc

#include "embot_hw.h"
#include "embot_hw_can.h"
#include "embot_hw_led.h"
#include "embot_hw_sys.h"
#include "embot_hw_flash.h"
#include "embot_hw_FlashStorage.h"
#include "embot_os.h"
#include "embot_os_Thread.h"

#include "embot_app_theBootloader.h"
#include "embot_app_bootloader_theCANparser.h"

#include "embot_prot_can.h"

#include "embot_app_theLEDmanager.h"


struct ActivityParam
{
    uint32_t blinkingperiod;
};

static void bl_activity(void* param);

static const embot::core::relTime BlinkSlowPeriod = 500*embot::core::time1millisec;
static ActivityParam activity_param = { .blinkingperiod = BlinkSlowPeriod };

// use build 222 for all the updaterofbootloader
static const embot::prot::can::versionOfAPPLICATION vAP = {1, 4 , 222};
static const embot::prot::can::versionOfCANPROTOCOL vCP = {2, 0};

static const std::uint32_t address = embot::hw::flash::getpartition(embot::hw::FLASH::application).address;
static const std::uint32_t vectorlocation = address - embot::hw::flash::getpartition(embot::hw::FLASH::whole).address;

int main(void)
{ 
    if(0 != vectorlocation)
    {        
        embot::hw::sys::relocatevectortable(vectorlocation);
    }
    
    embot::app::theBootloader & bootloader = embot::app::theBootloader::getInstance();
           
    activity_param.blinkingperiod = BlinkSlowPeriod;     
    embot::app::theBootloader::Config config;
    config.userdeflauncher = embot::core::Callback(bl_activity, &activity_param);   
    config.countdown = 0;
               
    bootloader.execute(config);
    
    for(;;);
    
}




static void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask msk, void *p);
static void eventbasedthread_init(embot::os::Thread *t, void *p);

static const embot::os::Event evRXcanframe = 0x00000001;

static embot::os::EventThread* eventbasedthread = nullptr;

static void alerteventbasedthread(void *arg);

static std::vector<embot::prot::can::Frame> outframes;

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
        
  
    // start thread waiting for can messages.   
    const embot::core::relTime waitEventTimeout = 50*embot::core::time1millisec;    
    embot::os::EventThread::Config configEV;    
    configEV.startup = eventbasedthread_init;
    configEV.onevent = eventbasedthread_onevent;
    configEV.param = nullptr;
    configEV.stacksize = 4*1024;
    configEV.priority = embot::os::Priority::high200;
    configEV.timeout = waitEventTimeout;
    
    eventbasedthread = new embot::os::EventThread;
    eventbasedthread->start(configEV);    
        
    // start canparser
    embot::app::bootloader::theCANparser &canparser = embot::app::bootloader::theCANparser::getInstance();
    embot::app::bootloader::theCANparser::Config config;
    config.owner = embot::prot::can::Process::application;
    
    canparser.initialise(config);  

    // finally start can. i keep it as last because i dont want that the isr-handler calls its onrxframe() 
    // before the eventbasedthread is created.
    embot::hw::result_t r = embot::hw::resNOK;
    embot::hw::can::Config canconfig; // default is tx/rxcapacity=8
    canconfig.txcapacity = 12;
    canconfig.onrxframe = embot::core::Callback(alerteventbasedthread, nullptr); 
    r = embot::hw::can::init(embot::hw::CAN::one, canconfig);
    r = embot::hw::can::setfilters(embot::hw::CAN::one, embot::app::theCANboardInfo::getInstance().getCANaddress());
    r = r;    
}


static void alerteventbasedthread(void *arg)
{
    if(nullptr != eventbasedthread)
    {
        eventbasedthread->setEvent(evRXcanframe);
    }
}


static void eventbasedthread_init(embot::os::Thread *t, void *p)
{
    embot::hw::result_t r = embot::hw::can::enable(embot::hw::CAN::one);  
    r = r;  

    outframes.reserve(12);
}
    


static void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask msk, void *p)
{  
    
    if(true == embot::core::binary::mask::check(msk, evRXcanframe))
    {        
        embot::hw::can::Frame frame;
        std::uint8_t remaining = 0;
        if(embot::hw::resOK == embot::hw::can::get(embot::hw::CAN::one, frame, remaining))
        {
            outframes.clear();            
            embot::app::bootloader::theCANparser &canparser = embot::app::bootloader::theCANparser::getInstance();
            if(true == canparser.process({frame.id, frame.size, frame.data}, outframes))
            {
                std::uint8_t num = outframes.size();
                for(std::uint8_t i=0; i<num; i++)
                {
                    embot::hw::can::put(embot::hw::CAN::one, {outframes[i].id, outframes[i].size, outframes[i].data});                                       
                }

                embot::hw::can::transmit(embot::hw::CAN::one);
                
            }

            if(remaining > 0)
            {
                eventbasedthread->setEvent(evRXcanframe);                 
            }
        }        
    }
 
}


///


