#include "embot.h"
#include "embot_binary.h"
#include "embot_app_theApplication.h"

#include "embot_app_application_theCANparserBasic.h"
#include "embot_app_theCANboardInfo.h"

#include "embot_hw_usb.h"
#include "faceExpressionsModule.h"

#include "EOtheLEDpulser.h"
#include "embot_hw_led.h"


#if defined(STM32HAL_BOARD_RFE)
    #include "embot_hw_bsp_rfe.h"
#else
    #error: cannot include embot_hw_bsp_rfe.h
#endif

extern uint32_t usbParser(uint8_t * RecMsg);
#if __ARMCOMPILER_VERSION < 6000000
static const embot::app::canprotocol::versionOfAPPLICATION vAP = {1, 0 , 0};
#else
static const embot::app::canprotocol::versionOfAPPLICATION vAP = {1, 0 , 0};
#endif
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
    
    // manage the led blinking
    uint32_t period = 2*EOK_reltime1sec;    
 
    eOledpulser_cfg_t ledconfig = {0};    
    ledconfig.led_enable_mask   = (1 << eo_ledpulser_led_zero);
    ledconfig.led_init          = reinterpret_cast<eOint8_fp_uint8_cvoidp_t>(embot::hw::led::init_legacy);
    ledconfig.led_on            = reinterpret_cast<eOint8_fp_uint8_t>(embot::hw::led::on); 
    ledconfig.led_off           = reinterpret_cast<eOint8_fp_uint8_t>(embot::hw::led::off);
    ledconfig.led_toggle        = reinterpret_cast<eOint8_fp_uint8_t>(embot::hw::led::toggle);    
    eo_ledpulser_Initialise(&ledconfig);    
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero, period, 0);  
        
    start_evt_based();      
}


static void eventbasedtask_onevent(embot::sys::Task *t, embot::common::EventMask evtmsk, void *p);
static void eventbasedtask_init(embot::sys::Task *t, void *p);

static const embot::common::Event evRXcanframe = 0x00000001 << 0;
static const embot::common::Event evRXusbmessage = 0x00000001 << 1;

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
    // start task waiting for can messages. 
    eventbasedtask = new embot::sys::EventTask;  
    const embot::common::relTime waitEventTimeout = 50*1000; //50*1000; //5*1000*1000;    
    eventbasedtask->init(eventbasedtask_init, eventbasedtask_onevent, 4*1024, 200, waitEventTimeout, nullptr, nullptr);    
        
    // start canparser basic
    embot::app::application::theCANparserBasic &canparserbasic = embot::app::application::theCANparserBasic::getInstance();
    embot::app::application::theCANparserBasic::Config configbasic;
    canparserbasic.initialise(configbasic);  
        
    // finally start can. i keep it as last because i dont want that the isr-handler calls its onrxframe() 
    // before the eventbasedtask is created.
    embot::hw::result_t r = embot::hw::resNOK;
    embot::hw::can::Config canconfig; // default is tx/rxcapacity=8
    canconfig.txcapacity = maxOUTcanframes;
    canconfig.onrxframe = embot::common::Callback(alerteventbasedtask, nullptr); 
    r = embot::hw::can::init(embot::hw::can::Port::one, canconfig);
    r = embot::hw::can::setfilters(embot::hw::can::Port::one, embot::app::theCANboardInfo::getInstance().getCANaddress());
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
    embot::hw::result_t r = embot::hw::can::enable(embot::hw::can::Port::one);  
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
        if(embot::hw::resOK == embot::hw::can::get(embot::hw::can::Port::one, frame, remainingINrx))
        {            
            embot::app::application::theCANparserBasic &canparserbasic = embot::app::application::theCANparserBasic::getInstance();            
//            embot::app::application::theCANparserIMU &canparserimu = embot::app::application::theCANparserIMU::getInstance();
//            embot::app::application::theCANparserTHERMO &canparserthermo = embot::app::application::theCANparserTHERMO::getInstance();
            // process w/ the basic parser, if not recognised call the parse specific of the board
            if(true == canparserbasic.process(frame, outframes))
            {                   
            }
//            else if(true == canparserimu.process(frame, outframes))
//            {               
//            }
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


