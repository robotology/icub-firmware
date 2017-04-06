

#include "EOtheLEDpulser.h"

#include "embot_app_theCANboardInfo.h"

#include "embot.h"

#include "stm32hal.h" // to see bsp_led_init etc

#include "embot_i2h.h"
#include "embot_hw.h"

#include "embot_hw_FlashStorage.h"
#include "embot_sys_theStorage.h"

#include "embot_app_theBootloader.h"
#include "embot_app_bootloader_theCANparser.h"

#include "embot_app_canprotocol.h"


struct ActivityParam
{
    uint32_t blinkingperiod;
};

static void bl_activity(void* param);
static ActivityParam activity_param = {0};

static const std::uint8_t NumOfSecondsBeforeJump = 5; 
static const embot::common::relTime BlinkFastPeriod = 1*EOK_reltime100ms;
static const embot::common::relTime BlinkSlowPeriod = 5*EOK_reltime100ms;
static const embot::common::relTime BlinkMadlyPeriod = 5*EOK_reltime10ms;

static const embot::app::canprotocol::versionOfBOOTLOADER vBL = {0, 129};
static const std::uint8_t defADDRESS = 1;
static const char defaultInfo32[] = {"I am a stm32l4"};

int main(void)
{ 
    embot::app::theBootloader & bootloader = embot::app::theBootloader::getInstance();

    std::uint32_t param = 0;    
    embot::sys::theJumper::Command cmd = bootloader.getcommand(param);
           
    embot::app::theBootloader::Config config;
    config.userdeflauncher.callback = bl_activity;
    config.userdeflauncher.arg = &activity_param;
    activity_param.blinkingperiod = BlinkFastPeriod;
    config.countdown = NumOfSecondsBeforeJump*embot::common::time1second;
    
    switch(cmd)
    {
        case embot::sys::theJumper::Command::none:
        {   // we dont change what we want to do: we exec for at 5 seconds
        } break;
        
        case embot::sys::theJumper::Command::stay:
        {   // we have received a command from the previous running process to stay in here forever
            config.countdown = 0;
            activity_param.blinkingperiod = BlinkSlowPeriod;
        } break; 
        
        case embot::sys::theJumper::Command::jump:
        {   // we have received command to jump to an address
            bootloader.jump(param); 
            // if we cannot jump to the specified address ... we stay in here forever
            config.countdown = 0;
            activity_param.blinkingperiod = BlinkMadlyPeriod;
        } break;          
    }

    // manage the basic canboardinfo
//    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();    
//    canbrdinfo.synch(embot::app::canprotocol::Board::mtb, vBL, 1);
    
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
    eOledpulser_cfg_t ledconfig = {0};    
    ledconfig.led_enable_mask   = (1 << eo_ledpulser_led_zero);
    ledconfig.led_init          = reinterpret_cast<eOint8_fp_uint8_cvoidp_t>(embot::hw::led::init_legacy);
    ledconfig.led_on            = reinterpret_cast<eOint8_fp_uint8_t>(embot::hw::led::on); 
    ledconfig.led_off           = reinterpret_cast<eOint8_fp_uint8_t>(embot::hw::led::off);
    ledconfig.led_toggle        = reinterpret_cast<eOint8_fp_uint8_t>(embot::hw::led::toggle);    
    eo_ledpulser_Initialise(&ledconfig);    
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero, period, 0);  

    // manage the basic canboardinfo
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();    
    canbrdinfo.synch(embot::app::canprotocol::Board::mtb, vBL, defADDRESS, defaultInfo32);
    
  
    // start task waiting for can messages. 
    eventbasedtask = new embot::sys::EventTask;  
    const embot::common::relTime waitEventTimeout = 50*1000; //50*1000; //5*1000*1000;    
    eventbasedtask->init(eventbasedtask_init, eventbasedtask_onevent, 4*1024, 200, waitEventTimeout, nullptr, nullptr);    
        
    // start canparser
    embot::app::bootloader::theCANparser &canparser = embot::app::bootloader::theCANparser::getInstance();
    embot::app::bootloader::theCANparser::Config config;
    canparser.initialise(config);  

    // finally start can. i keep it as last because i dont want that the isr-handler calls its onrxframe() 
    // before the eventbasedtask is created.
    embot::hw::result_t r = embot::hw::resNOK;
    embot::hw::can::Config canconfig; // default is tx/rxcapacity=8
    canconfig.txcapacity = 12;
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

    outframes.reserve(12);
}
    


static void eventbasedtask_onevent(embot::sys::Task *t, embot::common::Event evt, void *p)
{  
    
    if(true == embot::common::msk::check(evt, evRXcanframe))
    {        
        embot::hw::can::Frame frame;
        std::uint8_t remaining = 0;
        if(embot::hw::resOK == embot::hw::can::get(embot::hw::can::Port::one, frame, remaining))
        {
            outframes.clear();            
            embot::app::bootloader::theCANparser &canparser = embot::app::bootloader::theCANparser::getInstance();
            if(true == canparser.process(frame, outframes))
            {
                std::uint8_t num = outframes.size();
                for(std::uint8_t i=0; i<num; i++)
                {
                    embot::hw::can::put(embot::hw::can::Port::one, outframes[i]);                                       
                }

                embot::hw::can::transmit(embot::hw::can::Port::one);
                
            }

            if(remaining > 0)
            {
                eventbasedtask->setEvent(evRXcanframe);                 
            }
        }        
    }
 
}


///


