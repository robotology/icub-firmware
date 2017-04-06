

#include "EOtheLEDpulser.h"
#include "embot_app_canprotocol.h"
#include "embot_app_theCANboardInfo.h"

#include "embot.h"

#include "stm32hal.h" // to see bsp_led_init etc
#include "embot_hw.h"

#include "embot_hw_FlashStorage.h"
#include "embot_sys_theStorage.h"

#include "embot_app_theApplication.h"

#include "embot_app_application_theCANparserBasic.h"




static const embot::app::canprotocol::versionOfAPPLICATION vAP = {2, 2, 20};
static const embot::app::canprotocol::versionOfCANPROTOCOL vCP = {2, 0};

static void userdeflauncher(void* param);
static void userdefonidle(void* param);

static const embot::common::Callback atsysteminit(userdeflauncher, nullptr);

static const embot::common::Callback onidle(userdefonidle, nullptr);

static const embot::app::theApplication::StackSizes stacksizes =  { 2048, 512 };

static const embot::app::theApplication::UserDefOperations operations = { atsysteminit, onidle, {nullptr, nullptr} };


int main(void)
{ 
    embot::app::theApplication::Config config(embot::common::time1millisec, stacksizes, operations, embot::hw::sys::addressOfApplication);
    embot::app::theApplication &appl = embot::app::theApplication::getInstance();    
    
    appl.execute(config);  
    
    for(;;);    
}


static void start_evt_based(void);

static void userdeflauncher(void* param)
{
    eOledpulser_cfg_t ledconfig = {0};
    
    ledconfig.led_enable_mask   = (1 << eo_ledpulser_led_zero);
    ledconfig.led_init          = reinterpret_cast<eOint8_fp_uint8_cvoidp_t>(embot::hw::led::init_legacy); // (eOint8_fp_uint8_cvoidp_t) embot::hw::bsp::led_init_par;
    ledconfig.led_on            = reinterpret_cast<eOint8_fp_uint8_t>(embot::hw::led::on);
    ledconfig.led_off           = reinterpret_cast<eOint8_fp_uint8_t>(embot::hw::led::off);
    ledconfig.led_toggle        = reinterpret_cast<eOint8_fp_uint8_t>(embot::hw::led::toggle);
    
    eo_ledpulser_Initialise(&ledconfig);  
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero, 1000*1000, 0);    
        
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    canbrdinfo.synch(vAP, vCP);
    
    
    start_evt_based();
      
}



static void eventbasedtask_onevent(embot::sys::Task *t, embot::common::Event evt, void *p);
static void eventbasedtask_init(embot::sys::Task *t, void *p);

static const embot::common::Event evRXcanframe = 0x00000001;

static embot::sys::EventTask* eventbasedtask = nullptr;

static void alerteventbasedtask(void *arg);

static std::vector<embot::hw::can::Frame> outframes;

static void start_evt_based(void)
{
 
    // start task waiting for can messages. 
    eventbasedtask = new embot::sys::EventTask;  
    const embot::common::relTime waitEventTimeout = 50*1000; //50*1000; //5*1000*1000;    
    eventbasedtask->init(eventbasedtask_init, eventbasedtask_onevent, 4*1024, 200, waitEventTimeout, nullptr, nullptr);    
        
    // start canparser
    embot::app::application::theCANparserBasic &canparser = embot::app::application::theCANparserBasic::getInstance();
    embot::app::application::theCANparserBasic::Config config;
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

#warning BUTUSEISSETEVT for checking teh event 
    
    if(evRXcanframe == evt)
    {        
        embot::hw::can::Frame frame;
        std::uint8_t remaining = 0;
        if(embot::hw::resOK == embot::hw::can::get(embot::hw::can::Port::one, frame, remaining))
        {
            outframes.clear();            
            embot::app::application::theCANparserBasic &canparser = embot::app::application::theCANparserBasic::getInstance();
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

static void userdefonidle(void* param)
{
    static std::uint32_t cnt = 0;
    
    cnt++;
}


///


