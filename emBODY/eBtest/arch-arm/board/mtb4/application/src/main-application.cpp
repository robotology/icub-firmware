
//VALE ; removed api hal library. When i'll do library, I'll restore this file and I'll update stm32hal_bsp.h
//#include "stm32hal.h" // to see bsp_* functions and types
//I include following files
#warning VALE: Am I sure i need these includes here???
#include "main.h"
#include "stm32l4xx_hal.h"
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"


#include "EOtheLEDpulser.h"

#include "embot.h"

#include "embot_i2h.h"
#include "embot_hw.h"

#include "embot_app_theCANboardInfo.h"

#include "embot_app_theApplication.h"

#include "embot_app_theLoop.h"

#include "embot_app_theCANservice.h"



void periodic_startup(embot::sys::Task *tsk, void *param);
void periodic_activity(embot::sys::Task *tsk, void *param);

static void mytestCAN(void* param);


static void loop_startup(void *param);
static void loop_activity(void *param);

#define USE_HW_CAN

using namespace embot::app;

extern uint8_t tx_ended;

int main(void)
{   
    embot::app::theApplication::Config config(embot::common::time1millisec, mytestCAN, nullptr);
    embot::app::theApplication &appl = embot::app::theApplication::getInstance();    
    
    appl.execute(config);    
}

void tskPeriodic(void* p)
{
    embot::sys::registerNameOfTask(p);
    //eom_task_Start((EOMtask*)p);
}

static void mytestCAN(void* param)
{
    
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    std::uint8_t adr = canbrdinfo.getCANaddress();
    adr = adr;
    
    embot::sys::PeriodicTask *taskper = new embot::sys::PeriodicTask;
        
    taskper->init(periodic_startup, periodic_activity, 1024, 30, 3000 * 1000, nullptr, tskPeriodic); 
    //taskper->init(periodic_startup, periodic_activity, 1024, 30, 100*1000, nullptr, nullptr);     
    
    
//    embot::app::theLoop::Config loopconfig;
//    loopconfig.period = embot::common::time1millisec;
//    loopconfig.startup.callback = loop_startup;
//    loopconfig.activity.callback = loop_activity;
//    embot::app::theLoop &theloop = embot::app::theLoop::getInstance();
//    theloop.start(loopconfig);        
}



void periodic_startup(embot::sys::Task *tsk, void *param)
{
    uint8_t ss = tsk->getPriority();
    ss = ss;
    tsk = tsk;
    
    can::theCANservice::Config can_srv_cfg;
    can_srv_cfg.canstabilizationtime = 100; 
    can_srv_cfg.rxqueuesize = 10;
    can_srv_cfg.txqueuesize = 10;
    
    can::SkinProtocol *skin_proto = new can::SkinProtocol();
    can_srv_cfg.protocol = skin_proto;
    
    can::theCANservice & myCANsrv_singleton = embot::app::can::theCANservice::getInstance();
    
    bool res = myCANsrv_singleton.init(can_srv_cfg);
    res = res;
}

void periodic_activity(embot::sys::Task *tsk, void *param)
{
    can::theCANservice & myCANsrv_singleton = embot::app::can::theCANservice::getInstance();
    bool res = myCANsrv_singleton.parse();
    
    static uint8_t count=0;
    uint8_t aux=0;

    can::Message can_msg;
    can_msg.clas = 4;
    can_msg.type = 0;
    can_msg.value = &count;
    
    aux=count;
    can::Address dest = 1;
    res = myCANsrv_singleton.add(can_msg, dest);
    aux+=16;can_msg.value = &aux;
    res = myCANsrv_singleton.add(can_msg, dest);
    aux+=16;can_msg.value = &aux;
    res = myCANsrv_singleton.add(can_msg, dest);
    aux+=16;can_msg.value = &aux;
    res = myCANsrv_singleton.add(can_msg, dest);
    aux+=16;can_msg.value = &aux;
    res = myCANsrv_singleton.add(can_msg, dest);
    aux+=16;can_msg.value = &aux;
    res = myCANsrv_singleton.add(can_msg, dest);
    aux+=16;can_msg.value = &aux;
    res = myCANsrv_singleton.add(can_msg, dest);
    aux+=16;can_msg.value = &aux;
    res = myCANsrv_singleton.add(can_msg, dest);
    aux+=16;can_msg.value = &aux;
    res = myCANsrv_singleton.add(can_msg, dest);
    aux+=16;can_msg.value = &aux;
    res = myCANsrv_singleton.add(can_msg, dest);
     
    uint8_t txframes=0;
    if(tx_ended)
    {

        myCANsrv_singleton.transmit(0,txframes);
    }

   while(!tx_ended) 
   {
       HAL_Delay(1);
       can_msg.clas = 4;
       can_msg.type = 2;
       can_msg.value = nullptr;
       myCANsrv_singleton.add(can_msg, dest);
       
   }

    can_msg.clas = 4;
    can_msg.type = 1;
    can_msg.value=nullptr;
    myCANsrv_singleton.add(can_msg, dest);
    myCANsrv_singleton.transmit(0,txframes);

    if(count == 254)
        count = 0;
    else
        count++;

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


