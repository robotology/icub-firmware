#include "embot.h"
#include "embot_binary.h"
#include "embot_app_theApplication.h"

#include "embot_app_application_theCANparserBasic.h"
#include "embot_app_theCANboardInfo.h"

#include "embot_hw_usb.h"
#include "faceExpressionsModule.h"


#include "embot_hw_led.h"
#include "embot_app_theLEDmanager.h"


#include "embot_hw_bsp_rfe.h"

#include "embot_app_application_theCANparserIMU.h"
#include "embot_app_application_theIMU.h"


extern uint32_t usbParser(uint8_t * RecMsg);

static const embot::app::canprotocol::versionOfAPPLICATION vAP = {1, 1 , 1};
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
static const embot::common::Event evRXusbmessage = 0x00000001 << 1;

static const embot::common::Event evIMUtick = 0x00000001 << 3;
static const embot::common::Event evIMUdataready = 0x00000001 << 4;

static const std::uint8_t maxOUTcanframes = 48;

static embot::sys::EventTask* eventbasedtask = nullptr;

static void alerteventbasedtask(void *arg);
static void alerteventbasedtaskusb(void *arg);
static std::vector<embot::hw::can::Frame> outframes;

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


static void start_evt_based(void)
{     
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(embot::common::time1second); 
        
    // start task waiting for can messages. 
    eventbasedtask = new embot::sys::EventTask;  
    const embot::common::relTime waitEventTimeout = 50*1000; //50*1000; //5*1000*1000;    
   
    embot::sys::EventTask::Config configEV;
    
    configEV.startup = eventbasedtask_init;
    configEV.onevent = eventbasedtask_onevent;
    configEV.param = nullptr;
    configEV.stacksize = 4*1024;
    configEV.priority = 200;
    configEV.timeout = waitEventTimeout;
    eventbasedtask->start(configEV);
    
    
    // start canparser basic
    embot::app::application::theCANparserBasic &canparserbasic = embot::app::application::theCANparserBasic::getInstance();
    embot::app::application::theCANparserBasic::Config configbasic;
    canparserbasic.initialise(configbasic);  
    
    // start agent of imu
    embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
    embot::app::application::theIMU::Config configimu(embot::hw::bsp::rfe::imuBOSCH, embot::hw::bsp::rfe::imuBOSCHconfig, evIMUtick, evIMUdataready, eventbasedtask);
    theimu.initialise(configimu);    
        
    // start canparser imu and its agent
    embot::app::application::theCANparserIMU &canparserimu = embot::app::application::theCANparserIMU::getInstance();
    embot::app::application::theCANparserIMU::Config configparserimu { &theimu };
    canparserimu.initialise(configparserimu);      

               
        
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

static void alerteventbasedtaskusb(void *arg)
{
    if(nullptr != eventbasedtask)
    {
        eventbasedtask->setEvent(evRXusbmessage);
    }
}



static void eventbasedtask_init(embot::sys::Task *t, void *p)
{
    embot::hw::result_t r = embot::hw::can::enable(embot::hw::CAN::one);  
    r = r;     
    
    outframes.reserve(maxOUTcanframes);
    //#warning --> we should init the objects which holds outframes with maxOUTcanframes ... so that no more than maxOUTcanframes are pushed_back
    
 
     embot::hw::usb::Config config;
     config.rxcapacity = 20;
     config.onrxmessage = embot::common::Callback(alerteventbasedtaskusb, nullptr); 
     embot::hw::usb::init(embot::hw::usb::Port::one, config);
    
    faceExpressions.init();
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
            embot::app::application::theCANparserIMU &canparserimu = embot::app::application::theCANparserIMU::getInstance();
            // process w/ the basic parser, if not recognised call the parse specific of the board
            if(true == canparserbasic.process(frame, outframes))
            {                   
            }
            else if(true == canparserimu.process(frame, outframes))
            {               
            }
//            else if(true == canparserthermo.process(frame, outframes))
//            {               
//            }
            
            if(remainingINrx > 0)
            {
                eventbasedtask->setEvent(evRXcanframe);                 
            }
        }        
    }
    
    
    if(true == embot::binary::mask::check(eventmask, evRXusbmessage))
    {        
        embot::hw::usb::Message msg;
        std::uint8_t remainingINrx = 0;
        if(embot::hw::resOK == embot::hw::usb::get(embot::hw::usb::Port::one, msg, remainingINrx))
        {   
            embot::common::Time t1 = embot::sys::timeNow();
            faceExpressions.loadNewExpression(msg.data, msg.size);
            embot::common::Time t2 = embot::sys::timeNow();
            faceExpressions.displayExpression();
            embot::common::Time t3 = embot::sys::timeNow();
            
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
                eventbasedtask->setEvent(evRXusbmessage);                 
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


