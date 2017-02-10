

#include "bsp.h"

#include "EOtheLEDpulser.h"

#include "embot.h"

#include "embot_sys_theJumper.h"

#include "embot_i2h.h"
#include "embot_hw.h"
#include "embot_sys_theStorage.h"

#include "embot_i2h_FlashStorage.h"

#include "embot_app_theBootloader.h"


static void s_osal_starter(void);
void onIdle(void);
void onFatal(void);

void periodic_startup(embot::sys::Task *tsk, void *param);
void periodic_activity(embot::sys::Task *tsk, void *param);

static void eval_jump2application(void);


static void bl_activity(void);


int main(void)
{
#if 1
    
    embot::app::theBootloader & bootloader = embot::app::theBootloader::getInstance();
        
    bootloader.eval2jump();

    embot::app::theBootloader::Config config;
    config.activity = bl_activity;
    config.countdown = 5*embot::common::time1second;
    
    bootloader.execute(config);
    
    for(;;);
    
    
#else    
    eval_jump2application();
    
    embot::hw::bsp::init();
    
    
    embot::sys::theScheduler &thesystem = embot::sys::theScheduler::getInstance();
    embot::sys::theScheduler::Config cfg;
    cfg.launcher = s_osal_starter;
    cfg.launcherstacksize = 2048;
    cfg.onidle = onIdle;
    cfg.onidlestacksize = 512;
    cfg.onfatalerror = onFatal;
    cfg.clockfrequency = bsp_sys_get_clock();
    
    thesystem.init(cfg);
    thesystem.start();    

    for(;;);
#endif
}

static void eval_jump2application(void)
{
    embot::sys::theJumper& thejumper = embot::sys::theJumper::getInstance();
    embot::sys::theJumper::Config cfgJumper(bsp_sys_reset, bsp_sys_jump2address);
    thejumper.init(cfgJumper); 
    uint32_t address = 0;
    bool found = thejumper.eval(address, true);
    if(found)
    {
        address = address;
        // could not jump to address .... why? manage the error...
    }

//    thejumper.set(0x6661de00);
//    thejumper.restart();
}


static void s_osal_starter(void)
{
    
    int8_t pressed = 0;
    
    pressed = bsp_button_pressed(bsp_btn0);   
    pressed  = pressed;
    
    
    pressed = bsp_button_pressed(bsp_btn0);
    pressed = pressed;
    
    embot::sys::theTimerManager& tmrman = embot::sys::theTimerManager::getInstance();

    embot::sys::theTimerManager::Config tmrmanconfig;

    tmrman.init(tmrmanconfig);
    tmrman.start();
    
    embot::sys::theCallbackManager& cbkman = embot::sys::theCallbackManager::getInstance();
    cbkman.start();    


    eOledpulser_cfg_t ledconfig = {0};
    
    ledconfig.led_enable_mask   = (1 << eo_ledpulser_led_zero);
    ledconfig.led_init          = bsp_led_init;
    ledconfig.led_on            = (eOint8_fp_uint8_t) bsp_led_on;
    ledconfig.led_off           = (eOint8_fp_uint8_t) bsp_led_off;
    ledconfig.led_toggle        = (eOint8_fp_uint8_t) bsp_led_toggle;
    
    eo_ledpulser_Initialise(&ledconfig);    
    
   
    embot::i2h::cifStorage::cFun ccc = {nullptr};
    ccc.isinitted = bsp_flashstorage_isinitted;
    ccc.isaddressvalid = bsp_flashstorage_isaddressvalid;
    ccc.getbaseaddress = bsp_flashstorage_getbaseaddress;
    ccc.getsize = bsp_flashstorage_getsize;
    ccc.fullerase = bsp_flashstorage_fullerase;
    ccc.erase = bsp_flashstorage_erase;
    ccc.read = bsp_flashstorage_read;
    ccc.write = bsp_flashstorage_write;
    
    bsp_flashstorage_init();
    
    embot::i2h::cifStorage *cifstrg = new embot::i2h::cifStorage(ccc);
    
    
    
    embot::sys::theStorage &thestrg = embot::sys::theStorage::getInstance();    

#if 0
    // use the cifstrg
    (cifstrg);    
#else
    // use the proper derived class
    embot::i2h::FlashStorage *flashStorage = new embot::i2h::FlashStorage;
    thestrg.init(flashStorage);
#endif
    
    uint32_t base = thestrg.getBaseAddress();
    
    uint8_t buffer[32] = {0};
    uint8_t rr[32] = {0};
    buffer[0] = buffer[2] = 9;
    
    thestrg.write(base+0, 8, buffer);
    
    thestrg.read(base+0, 8, rr);
    
    buffer[1] = 1;    
    
    
    embot::sys::PeriodicTask *taskper = new embot::sys::PeriodicTask;
        
    taskper->init(periodic_startup, periodic_activity, 1024, 30, 100*1000, nullptr);

}

void onIdle(void)
{
    // called every time inside a for(;;){}
    static volatile uint8_t ii = 0;
    ii++;
    
//    embot::sys::theScheduler & system = embot::sys::theScheduler::getInstance(); 
//    embot::sys::Task *tsk1 = system.getTask();
//    tsk1 = tsk1;
    
    return;
}

void onFatal(void)
{
    uint8_t ciao = 0;
    ciao =  ciao;
    return;
}


void periodic_startup(embot::sys::Task *tsk, void *param)
{
    uint8_t ss = tsk->getPriority();
    ss = ss;
    tsk = tsk;
    
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero, EOK_reltime1sec, 0);
}

void periodic_activity(embot::sys::Task *tsk, void *param)
{
    static uint32_t x = 0;
    x++;
    x = x;    
}


static void bl_activity(void)
{
    
    eOledpulser_cfg_t ledconfig = {0};
    
    ledconfig.led_enable_mask   = (1 << eo_ledpulser_led_zero);
    ledconfig.led_init          = bsp_led_init;
    ledconfig.led_on            = (eOint8_fp_uint8_t) bsp_led_on;
    ledconfig.led_off           = (eOint8_fp_uint8_t) bsp_led_off;
    ledconfig.led_toggle        = (eOint8_fp_uint8_t) bsp_led_toggle;
    
    eo_ledpulser_Initialise(&ledconfig);    

    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero, 5*EOK_reltime100ms, 0);
       
}


///


