

#include "stm32hal.h" // to see bsp_* functions and types

#include "EOtheLEDpulser.h"

#include "embot.h"

#include "embot_sys_theJumper.h"

#include "embot_i2h.h"
#include "embot_hw.h"

#include "embot_app_theCANboardInfo.h"

#include "embot_app_theApplication.h"

#include "embot_app_theLoop.h"



void periodic_startup(embot::sys::Task *tsk, void *param);
void periodic_activity(embot::sys::Task *tsk, void *param);

static void userdeflauncher(void* param);


static void loop_startup(void *param);
static void loop_activity(void *param);

#undef USE_HW_CAN

int main(void)
{   
    embot::app::theApplication::Config config(embot::common::time1millisec, userdeflauncher, nullptr);
    embot::app::theApplication &appl = embot::app::theApplication::getInstance();    
    
    appl.execute(config);    
}

void tskPeriodic(void* p)
{
    embot::sys::registerNameOfTask(p);
    //eom_task_Start((EOMtask*)p);
}

static void userdeflauncher(void* param)
{
    bool pressed = false;
    
    embot::hw::bsp::btn_init(embot::hw::bsp::BTN::zero);
    
    pressed = embot::hw::bsp::btn_ispressed(embot::hw::bsp::BTN::zero);   
    pressed = pressed;
    
    
    pressed = embot::hw::bsp::btn_ispressed(embot::hw::bsp::BTN::zero);
    pressed = pressed;
    


    eOledpulser_cfg_t ledconfig = {0};
    
    ledconfig.led_enable_mask   = (1 << eo_ledpulser_led_zero);
    ledconfig.led_init          = reinterpret_cast<eOint8_fp_uint8_cvoidp_t>(embot::hw::bsp::led_init_par); // (eOint8_fp_uint8_cvoidp_t) embot::hw::bsp::led_init_par;
    ledconfig.led_on            = reinterpret_cast<eOint8_fp_uint8_t>(embot::hw::bsp::led_on);
    ledconfig.led_off           = reinterpret_cast<eOint8_fp_uint8_t>(embot::hw::bsp::led_off);
    ledconfig.led_toggle        = reinterpret_cast<eOint8_fp_uint8_t>(embot::hw::bsp::led_toggle);
    
    eo_ledpulser_Initialise(&ledconfig);    
    
    embot::sys::theJumper& thejumper = embot::sys::theJumper::getInstance();

    
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    std::uint8_t adr = canbrdinfo.getCANaddress();
    adr = adr;
    
    embot::sys::PeriodicTask *taskper = new embot::sys::PeriodicTask;
        
    taskper->init(periodic_startup, periodic_activity, 1024, 30, 100*1000, nullptr, tskPeriodic); 
    //taskper->init(periodic_startup, periodic_activity, 1024, 30, 100*1000, nullptr, nullptr);     
    
    
    embot::app::theLoop::Config loopconfig;
    loopconfig.period = embot::common::time1millisec;
    loopconfig.startup.callback = loop_startup;
    loopconfig.activity.callback = loop_activity;
    embot::app::theLoop &theloop = embot::app::theLoop::getInstance();
    theloop.start(loopconfig);        
}



void periodic_startup(embot::sys::Task *tsk, void *param)
{
    uint8_t ss = tsk->getPriority();
    ss = ss;
    tsk = tsk;
    
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero, 4*EOK_reltime1sec, 0);
}

void periodic_activity(embot::sys::Task *tsk, void *param)
{
    static uint32_t x = 0;
    x++;
    x = x;   

    if(1 == bsp_button_pressed(bsp_btn0))
    {
        // we go back to bootloader
        embot::sys::theJumper& thejumper = embot::sys::theJumper::getInstance();
        thejumper.set(embot::sys::theJumper::Command::stay, 0);
        thejumper.restart();
        x = x; 
    }
    
}

static void txoneframe(void *p)
{
    static uint32_t ss = 0;
    
    ss++;
}

static void loop_startup(void *param)
{
#if defined(USE_HW_CAN)
    embot::hw::can::Config config;
    config.ontxframe.callback = txoneframe;
    embot::hw::can::init(embot::hw::can::Port::one, config);
    embot::hw::can::enable(embot::hw::can::Port::one);
#endif       
}

static void loop_activity(void *param)
{
    static std::uint32_t cnt = 0;
    cnt++;
    cnt = cnt;   
    
#if defined(USE_HW_CAN)
    embot::hw::can::Frame frame;
    frame.id = cnt;
    frame.size = 1;
    frame.data[0] = 7;
    
    embot::hw::can::put(embot::hw::can::Port::one, frame);
    uint8_t ss = embot::hw::can::outputqueuesize(embot::hw::can::Port::one);
    
    if(0 == (cnt%20))
    {
        embot::hw::can::transmit(embot::hw::can::Port::one);
    }
    
    ss = embot::hw::can::outputqueuesize(embot::hw::can::Port::one);
    ss = ss;
#endif    
}




///


