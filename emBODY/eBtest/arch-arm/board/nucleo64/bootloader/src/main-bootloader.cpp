

#include "EOtheLEDpulser.h"

#include "embot_app_theCANboardInfo.h"

#include "embot.h"

#include "stm32hal.h" // to see bsp_led_init etc

#include "embot_i2h.h"
#include "embot_hw.h"

#include "embot_hw_FlashStorage.h"
#include "embot_sys_theStorage.h"

#include "embot_app_theBootloader.h"

struct ActivityParam
{
    uint32_t blinkingperiod;
};

static void bl_activity(void* param);

void periodic_activity(embot::sys::Task *tsk, void *param);

static ActivityParam activity_param = {0};


int main(void)
{ 
    embot::app::theBootloader & bootloader = embot::app::theBootloader::getInstance();

    std::uint32_t param = 0;    
    embot::sys::theJumper::Command cmd = bootloader.getcommand(param);
           
    embot::app::theBootloader::Config config;
    config.userdeflauncher.callback = bl_activity;
    config.userdeflauncher.arg = &activity_param;
    activity_param.blinkingperiod = 1*EOK_reltime100ms;
    config.countdown = 5*embot::common::time1second;
    
    switch(cmd)
    {
        case embot::sys::theJumper::Command::none:
        {   // we dont change what we want to do: we exec for at 5 seconds
        } break;
        
        case embot::sys::theJumper::Command::stay:
        {   // we have received a command from the previous running process to stay in here forever
            config.countdown = 0;
            activity_param.blinkingperiod = 5*EOK_reltime100ms;
        } break; 
        
        case embot::sys::theJumper::Command::jump:
        {   // we have received command to jump to an address
            bootloader.jump(param); 
            // if we cannot jump to the specified address ... we stay in here forever
            config.countdown = 0;
            activity_param.blinkingperiod = 5*EOK_reltime10ms;
        } break;          
    }

    
    bootloader.execute(config);
    
    for(;;);
    
}



static void bl_activity(void* param)
{
    
    ActivityParam* pp = (ActivityParam*) param;
    
    uint32_t period = 0;
    
    if(nullptr != pp)
    {
        period = pp->blinkingperiod;
    }
    
    eOledpulser_cfg_t ledconfig = {0};
    
    ledconfig.led_enable_mask   = (1 << eo_ledpulser_led_zero);
    ledconfig.led_init          = bsp_led_init;
    ledconfig.led_on            = (eOint8_fp_uint8_t) bsp_led_on;
    ledconfig.led_off           = (eOint8_fp_uint8_t) bsp_led_off;
    ledconfig.led_toggle        = (eOint8_fp_uint8_t) bsp_led_toggle;
    
    eo_ledpulser_Initialise(&ledconfig);    

    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero, period, 0);  


    // also start a periodic task which checks ....
    embot::sys::PeriodicTask *taskper = new embot::sys::PeriodicTask;        
    taskper->init(nullptr, periodic_activity, 1024, 30, 100*1000, nullptr);
    
    // init the storage object ....
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    
    canbrdinfo.synch(embot::app::theCANboardInfo::Type::mtb3, 1);
    
    uint8_t adr = canbrdinfo.getCANaddress();
    adr = adr;
    if(1 == adr)
    {
        canbrdinfo.setCANaddress(2);
    }
    
}



void periodic_activity(embot::sys::Task *tsk, void *param)
{
    static uint32_t x = 0;
    if(1 == bsp_button_pressed(bsp_btn0))
    {
        embot::app::theBootloader & bootloader = embot::app::theBootloader::getInstance();
        bootloader.stopcountdown();
        
        eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero, 10*EOK_reltime100ms, 0); 
        
        x++;
        // we stop countdown and we stay in bootloader w/ reduced frequency of ....
        x = x; 
    }    
}


///


