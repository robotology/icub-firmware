


#include "embot_app_canprotocol.h"
#include "embot_app_theCANboardInfo.h"

#include "embot.h"
#include "embot_common.h"
#include "embot_binary.h"

#include "stm32hal.h" 
#include "embot_hw.h"
#include "embot_hw_bsp.h"
#include "embot_hw_sys.h"
#include "embot_hw_led.h"
#include "embot_hw_can.h"

#include "embot_hw_FlashStorage.h"
#include "embot_sys_theStorage.h"

#include "embot_app_theApplication.h"

#include "embot_app_application_theCANparserBasic.h"

#include "embot_app_application_theCANparserSkin.h"
#include "embot_app_application_theSkin.h"

#include "embot_app_application_theCANparserIMU.h"
#include "embot_app_application_theIMU.h"

#include "embot_app_application_theCANparserTHERMO.h"
#include "embot_app_application_theTHERMO.h"

#include "embot_hw_bsp_mtb4.h"

#include "embot_app_theLEDmanager.h"


static const embot::app::canprotocol::versionOfAPPLICATION vAP = {1, 3 , 0};
static const embot::app::canprotocol::versionOfCANPROTOCOL vCP = {2, 0};

static void userdeflauncher(void* param);
static void userdefonidle(void* param);
static void userdefonfatal(void* param);


static const embot::sys::Operation oninit = { embot::common::Callback(userdeflauncher, nullptr), 2048 };
static const embot::sys::Operation onidle = { embot::common::Callback(userdefonidle, nullptr), 512 };
static const embot::sys::Operation onfatal = { embot::common::Callback(userdefonfatal, nullptr), 64 };

#if defined(APPL_TESTZEROOFFSET)
static const std::uint32_t address = embot::hw::flash::getpartition(embot::hw::FLASH::bootloader).address;
#else
static const std::uint32_t address = embot::hw::flash::getpartition(embot::hw::FLASH::application).address;
#endif

int main(void)
{ 
    embot::app::theApplication::Config config(embot::common::time1millisec, oninit, onidle, onfatal, address);
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

static void userdefonfatal(void *param)
{
}



static void eventbasedtask_onevent(embot::sys::Task *t, embot::common::EventMask evtmsk, void *p);
static void eventbasedtask_init(embot::sys::Task *t, void *p);

static const embot::common::Event evRXcanframe = 0x00000001 << 0;
static const embot::common::Event evSKINprocess = 0x00000001 << 1;

static const embot::common::Event evIMUtick = 0x00000001 << 3;
static const embot::common::Event evIMUdataready = 0x00000001 << 4;
static const embot::common::Event evTHERMOtick = 0x00000001 << 5;
static const embot::common::Event evTHERMOdataready = 0x00000001 << 6;


static const std::uint8_t maxOUTcanframes = 48;

static embot::sys::EventTask* eventbasedtask = nullptr;

static void alerteventbasedtask(void *arg);

static std::vector<embot::hw::can::Frame> outframes;

static void start_evt_based(void)
{    
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(embot::common::time1second); 
        
    // start task waiting for can messages. 
    eventbasedtask = new embot::sys::EventTask;  
    const embot::common::relTime waitEventTimeout = 50*1000; //50*1000; //5*1000*1000;       
    embot::sys::EventTask::Config configEV(4*1024, 200, waitEventTimeout, eventbasedtask_init, eventbasedtask_onevent, nullptr);
    eventbasedtask->start(configEV);
        
    // start canparser basic
    embot::app::application::theCANparserBasic &canparserbasic = embot::app::application::theCANparserBasic::getInstance();
    embot::app::application::theCANparserBasic::Config configparserbasic;
    canparserbasic.initialise(configparserbasic);  
    
    // start canparser skin
    embot::app::application::theCANparserSkin &canparserskin = embot::app::application::theCANparserSkin::getInstance();
    embot::app::application::theCANparserSkin::Config configparserskin;
    canparserskin.initialise(configparserskin);  
    
    // start canparser imu
    embot::app::application::theCANparserIMU &canparserimu = embot::app::application::theCANparserIMU::getInstance();
    embot::app::application::theCANparserIMU::Config configparserimu;
    canparserimu.initialise(configparserimu);    

    // start canparser thermo
    embot::app::application::theCANparserTHERMO &canparserthermo = embot::app::application::theCANparserTHERMO::getInstance();
    embot::app::application::theCANparserTHERMO::Config configparserthermo;
    canparserthermo.initialise(configparserthermo);     
    
    // start agent of skin 
    embot::app::application::theSkin &theskin = embot::app::application::theSkin::getInstance();
    embot::app::application::theSkin::Config configskin;
    configskin.tickevent = evSKINprocess;
    configskin.totask = eventbasedtask;
    theskin.initialise(configskin);   

    // start agent of imu
    embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
    embot::app::application::theIMU::Config configimu(embot::hw::bsp::mtb4::imuBOSCH, embot::hw::bsp::mtb4::imuBOSCHconfig, evIMUtick, evIMUdataready, eventbasedtask);
    theimu.initialise(configimu);   

    // start agent of thermo
    embot::app::application::theTHERMO &thethermo = embot::app::application::theTHERMO::getInstance();
    embot::app::application::theTHERMO::Config configthermo(embot::hw::bsp::mtb4::thermometer, embot::hw::bsp::mtb4::thermometerconfig, evTHERMOtick, evTHERMOdataready, eventbasedtask);
    thethermo.initialise(configthermo);         
    
    // finally start can. i keep it as last because i dont want that the isr-handler calls its onrxframe() 
    // before the eventbasedtask is created.
    embot::hw::result_t r = embot::hw::resNOK;
    embot::hw::can::Config canconfig; // default is tx/rxcapacity=8
    canconfig.txcapacity = maxOUTcanframes;
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

    outframes.reserve(maxOUTcanframes);
}
    


static void eventbasedtask_onevent(embot::sys::Task *t, embot::common::EventMask eventmask, void *p)
{     
    if(0 == eventmask)
    {   // timeout ... 
        return;
    }
    
    // we clear the frames to be trasmitted
    outframes.clear();      
    
    
    if(true == embot::binary::mask::check(eventmask, evRXcanframe))
    {        
        embot::hw::can::Frame frame;
        std::uint8_t remainingINrx = 0;
        if(embot::hw::resOK == embot::hw::can::get(embot::hw::CAN::one, frame, remainingINrx))
        {            
            embot::app::application::theCANparserBasic &canparserbasic = embot::app::application::theCANparserBasic::getInstance();
            embot::app::application::theCANparserSkin &canparserskin = embot::app::application::theCANparserSkin::getInstance();
            embot::app::application::theCANparserIMU &canparserimu = embot::app::application::theCANparserIMU::getInstance();
            embot::app::application::theCANparserTHERMO &canparserthermo = embot::app::application::theCANparserTHERMO::getInstance();
            // process w/ the basic parser, if not recognised call the parse specific of the board
            if(true == canparserbasic.process(frame, outframes))
            {                   
            }
            else if(true == canparserskin.process(frame, outframes))
            {               
            }
            else if(true == canparserimu.process(frame, outframes))
            {               
            }
            else if(true == canparserthermo.process(frame, outframes))
            {               
            }
            
            if(remainingINrx > 0)
            {
                eventbasedtask->setEvent(evRXcanframe);                 
            }
        }        
    }
    
    if(true == embot::binary::mask::check(eventmask, evSKINprocess))
    {
        embot::app::application::theSkin &theskin = embot::app::application::theSkin::getInstance();
        theskin.tick(outframes);
        
        // we operate on the skin triangles by calling a skin.tick(outframes);
        // the evSKINprocess is emitted  by:
        // 1. a periodic timer started at the reception of a specific message.


        // the .tick(outframes) will do whatever it needs to do and it may emit some 
        // can frames for transmission. the can frames can be up to 16x2 = 32.
        // hence, how many packets? max of replies = 8 + max of broadcast = 32 --> 40.
        
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

    if(true == embot::binary::mask::check(eventmask, evTHERMOtick))
    {        
        embot::app::application::theTHERMO &thethermo = embot::app::application::theTHERMO::getInstance();
        thethermo.tick(outframes);        
    }   
    
    if(true == embot::binary::mask::check(eventmask, evTHERMOdataready))
    {        
        embot::app::application::theTHERMO &thethermo = embot::app::application::theTHERMO::getInstance();
        thethermo.processdata(outframes);        
    }
    
    // if we have any packet we transmit them
    std::uint8_t num = outframes.size();
    if(num > 0)
    {
        for(std::uint8_t i=0; i<num; i++)
        {
            embot::hw::can::put(embot::hw::CAN::one, outframes[i]);                                       
        }

        embot::hw::can::transmit(embot::hw::CAN::one);  
    } 
 
}

static void userdefonidle(void* param)
{
    static std::uint32_t cnt = 0;
    
    cnt++;
}


///


