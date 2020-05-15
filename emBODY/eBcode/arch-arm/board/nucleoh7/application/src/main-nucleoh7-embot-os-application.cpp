
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#include "embot_core.h"
#include "embot_core_binary.h"

#include "embot_hw.h"
#include "embot_hw_bsp.h"
#include "embot_hw_led.h"
#include "embot_hw_button.h"
#include "embot_hw_sys.h"

#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_app_theLEDmanager.h"


// macro definition. we keep some behaviours in the same code.

// it tells to use the serial port to print data on it
#undef  enableSERIAL  
// if enableSERIAL is defined, it prints in human readable format
#undef  enableSERIAL_string
// if enableSERIAL is defined, it prints in compact binary format
#undef  enableSERIAL_binary
// if defined tickperiod is < 10 ms, else it is 1 sec 
//#define  enableTICK_fast
// if defined, the values are asked to the chips bno055 and the adc, if undefined we tx fake values
#undef  enableACQUISITION
// if defined we print values on the trace port
#undef  enableTRACE
// enableTRACE, it enable print of everything, even in the fast ticked function. else it prints only basic info
#undef  enableTRACE_all



//#define enableTICK_fast
//#define enableTRACE_all
#define enableSERIAL
#define enableACQUISITION

#if defined(enableSERIAL)
#define enableSERIAL_string
//#define enableSERIAL_binary
#endif

static void s_chips_init();
static void s_chips_tick();

static void s_imu_start();
static void s_imu_get();

static void s_adc_start();
static void s_adc_get();

static void s_transmit();


void doit(void *p)
{
    static uint32_t a = 0;
    a++;    
}

constexpr embot::os::Event evtPeriodicTick = embot::core::binary::mask::pos2mask<embot::os::Event>(0);
constexpr embot::os::Event evtIMUdataready = embot::core::binary::mask::pos2mask<embot::os::Event>(1);
constexpr embot::os::Event evtADCdataready = embot::core::binary::mask::pos2mask<embot::os::Event>(2);
constexpr embot::os::Event evtDATAtransmit = embot::core::binary::mask::pos2mask<embot::os::Event>(7);

 

#if defined(enableTICK_fast) 
constexpr embot::core::relTime tickperiod = 100*embot::core::time1millisec;
#else
constexpr embot::core::relTime tickperiod = embot::core::time1second;
#endif

constexpr embot::core::relTime txperiod = 200*embot::core::time1millisec;

void eventbasedthread_startup(embot::os::Thread *t, void *param)
{
    s_chips_init(); 
    embot::hw::sys::puts("evthread-startup: chips initted" );
    
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtPeriodicTick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);
    embot::hw::sys::puts("evthread-startup: started timer which sends evtPeriodicTick to evthread every us = " + std::to_string(tickperiod));
    
    embot::os::Timer *tmrTX = new embot::os::Timer;   
    embot::os::Action actTX(embot::os::EventToThread(evtDATAtransmit, t));
    embot::os::Timer::Config cfgTX{txperiod, actTX, embot::os::Timer::Mode::forever, 0};
    tmrTX->start(cfgTX);
}



void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{   
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }

    if(true == embot::core::binary::mask::check(eventmask, evtPeriodicTick))
    {
#if defined(enableTRACE_all)        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::hw::sys::puts("evthread-onevent: evtPeriodicTick received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));    
#endif        
//        s_chips_tick();    
        s_imu_start();
    }
    
    if(true == embot::core::binary::mask::check(eventmask, evtIMUdataready))
    {
#if defined(enableTRACE_all)        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::hw::sys::puts("evthread-onevent: evtPeriodicTick received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));    
#endif        
        s_imu_get();
        s_adc_start();        
    }
 
    if(true == embot::core::binary::mask::check(eventmask, evtADCdataready))
    {
#if defined(enableTRACE_all)        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::hw::sys::puts("evthread-onevent: evtPeriodicTick received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));    
#endif        
        s_adc_get();        
    }    
    
    if(true == embot::core::binary::mask::check(eventmask, evtDATAtransmit))
    {
#if defined(enableTRACE_all)        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::hw::sys::puts("evthread-onevent: evtPeriodicTick received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));    
#endif        
        //s_chips_tick(); 
        s_transmit();
    }    
}


void onIdle(embot::os::Thread *t, void* idleparam)
{
    static uint32_t i = 0;
    i++;
}

embot::os::EventThread* thr {nullptr};

void initSystem(embot::os::Thread *t, void* initparam)
{
    
    embot::os::theTimerManager::getInstance().start({});     
    embot::os::theCallbackManager::getInstance().start({});  
    
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(2*embot::core::time1second); 
       
    
    embot::os::EventThread::Config configEV { 
        6*1024, 
        embot::os::Priority::high200, 
        eventbasedthread_startup,
        nullptr,
        50*embot::core::time1millisec,
        eventbasedthread_onevent
    };
        
    // create the main thread 
    thr = new embot::os::EventThread;          
    // and start it
    thr->start(configEV);
    
}



// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    // steps:
    // 1. i init the embot::os
    // 2 i start the scheduler
    
    
    constexpr embot::os::InitThread::Config initcfg = { 4*1024, initSystem, nullptr };
    constexpr embot::os::IdleThread::Config idlecfg = { 512, nullptr, nullptr, onIdle };
    constexpr embot::core::Callback onOSerror = { };
    constexpr embot::os::Config osconfig {embot::core::time1millisec, initcfg, idlecfg, onOSerror};
    
    // embot::os::init() internally calls embot::hw::bsp::init() which also calls embot::core::init()
    embot::os::init(osconfig);
    
    // now i start the os    
    embot::os::start();

    // just because i am paranoid (thescheduler.start() never returns)
    for(;;);    
}



#include "embot_hw_bno055.h"

#include <cstring>

static void s_chips_init()
{
#if !defined(enableACQUISITION)
    
    // nothing. we just simulate the sensors
    
#else
 
    constexpr embot::core::relTime timeout = 5*embot::core::time1millisec;
    
    embot::hw::bno055::Config bno055config { embot::hw::i2c::Descriptor { embot::hw::I2C::one, 400000 } };
    embot::hw::bno055::init(embot::hw::BNO055::one, bno055config); 
    
    constexpr embot::hw::bno055::Mode mode = embot::hw::bno055::Mode::ACCGYRO;
    //constexpr embot::hw::bno055::Mode mode = embot::hw::bno055::Mode::NDOF;
    embot::hw::bno055::set(embot::hw::BNO055::one, mode, timeout);      
    
#endif
    
}

volatile embot::core::Time imu_start {0};
volatile embot::core::Time imu_stop {0};

std::tuple<int16_t, int16_t, int16_t> acc2transmit {0, 0, 0};
std::pair<uint32_t, uint32_t> adc2transmit {0, 0};
embot::core::Time imu_acquisitiontime {0};

embot::hw::bno055::Data data {};

void alertimudataisready(void *p)
{
    thr->setEvent(evtIMUdataready);
}
    
static void s_imu_start()
{
    imu_start = embot::core::now();
    embot::core::Callback cbk(alertimudataisready, nullptr);
    embot::hw::bno055::acquisition(embot::hw::BNO055::one, embot::hw::bno055::Set::A, data, cbk);    
}

static void s_imu_get()
{
    imu_stop = embot::core::now();
    imu_acquisitiontime = imu_stop - imu_start;
    acc2transmit = {data.acc.x, data.acc.y, data.acc.z};    
}


void alertadcdataisready(void *p)
{
    thr->setEvent(evtADCdataready);
}

static void s_adc_start()
{
    // nothing so far.
    // just send the event
    
    alertadcdataisready(nullptr);    
}

static void s_adc_get()
{
   // nothing 
    
    adc2transmit = {1, 1};
}

bool s_print_values(const std::tuple<int16_t, int16_t, int16_t> &acc, const std::pair<uint32_t, uint32_t> &adc);

static void s_transmit()
{
    s_print_values(acc2transmit, adc2transmit);
}

volatile HAL_StatusTypeDef r = HAL_ERROR;

bool s_get_values(std::tuple<int16_t, int16_t, int16_t> &acc, std::pair<uint32_t, uint32_t> &adc);
bool s_print_values(const std::tuple<int16_t, int16_t, int16_t> &acc, const std::pair<uint32_t, uint32_t> &adc);


static void s_chips_tick()
{   
    std::tuple<int16_t, int16_t, int16_t> acc {0, 0, 0};
    std::pair<uint32_t, uint32_t> adc {0, 0};
        
    s_get_values(acc, adc);
    
    s_print_values(acc, adc);      
}

bool s_get_values(std::tuple<int16_t, int16_t, int16_t> &acc, std::pair<uint32_t, uint32_t> &adc)
{
#if !defined(enableACQUISITION)
    
    constexpr int32_t accmin = -30;
    constexpr int32_t accmax = +270;
    
    constexpr uint32_t adcmin = 0;
    constexpr uint32_t adcmax = +300;
    
    static int16_t accx = accmin+0;
    static int16_t accy = accmin+10;
    static int16_t accz = accmin+20;
    static uint32_t adc1 = adcmin+0;
    static uint32_t adc2 = adcmin+10;    
    
    acc = {accx, accy, accz};
    adc = {adc1, adc2};
    
    // implements triangular waveforms
    accx++; accy++; accz++; adc1++; adc2++;
    if(accx >= accmax) accx = accmin;
    if(accy >= accmax) accy = accmin;
    if(accz >= accmax) accz = accmin;
    if(adc1 >= adcmax) adc1 = adcmin;
    if(adc2 >= adcmax) adc2 = adcmin;    

#else

    acc = {0, 0, 0};
    adc = {1, 1};
    
    // so far only bno055
    #if 1
    
    imu_start = embot::core::now();
    embot::hw::bno055::Data data;        
    if(embot::hw::result_t::OK == embot::hw::bno055::acquisition(embot::hw::BNO055::one, embot::hw::bno055::Set::A, data))
    {
        for(;;)
        {
            if(true == embot::hw::bno055::operationdone(embot::hw::BNO055::one))
            {
                break;
            }
        }
        
        // ok, we have the values in data        
        acc = {data.acc.x, data.acc.y, data.acc.z};        
    }
    imu_stop = embot::core::now();
    #else
    // better mode because it gives a timeout BUT to be tested
    embot::hw::bno055::Data data; 
    constexpr embot::core::relTime timeout = 10*embot::core::time1millisec;   
    if(embot::hw::result_t::OK == embot::hw::bno055::acquisition(embot::hw::BNO055::one, embot::hw::bno055::Set::AMG, data, timeout))
    {      
        // ok, we have the values in data        
        acc = {data.acc.x, data.acc.y, data.acc.z};        
    }        
    #endif

#endif    
       
    return true;    
}


bool s_print_values(const std::tuple<int16_t, int16_t, int16_t> &acc, const std::pair<uint32_t, uint32_t> &adc)
{   
    char text[64] = {0};
    
#if defined(enableSERIAL) && defined(enableSERIAL_string)

#if 1

    if(0 != imu_acquisitiontime)
    {
        float ax = 0.01f * std::get<0>(acc);
        float ay = 0.01f * std::get<1>(acc);
        float az = 0.01f * std::get<2>(acc);
        embot::hw::sys::puts(std::string("acceleration [m/s^2] = (") + 
                std::to_string(ax) + " " +
                std::to_string(ay) + " " +
                std::to_string(az) + 
                ") read in " + embot::core::TimeFormatter(imu_acquisitiontime).to_string()
        );
    }

#else    
    // this prints in hex the entire range of values
    //snprintf(text, sizeof(text), "%04x %04x %04x %08x %08x\n", std::get<0>(acc), std::get<1>(acc), std::get<2>(acc), adc.first, adc.second);

    // JUST FOR TEST: this instead prints in hex but only 1 byte (the least significant byte)
    snprintf(text, sizeof(text), "%02x %02x %02x %02x %02x\n", std::get<0>(acc), std::get<1>(acc), std::get<2>(acc), adc.first, adc.second);
		//snprintf(text, sizeof(text), "%02x\n", std::get<0>(acc));	//LUCA
    
//    snprintf(text, sizeof(text), "0123456789abcdef0123 ");
//    embot::core::Time t0 = embot::core::now();
        
    HAL_UART_Transmit(&huart3, reinterpret_cast<uint8_t*>(text), std::strlen(text), 0xFFFF);
    
//    embot::core::Time t1 = embot::core::now();
//    embot::hw::sys::puts("tx " + std::to_string( std::strlen(text)) + " in: " + embot::core::TimeFormatter(t1-t0).to_string());

#endif
    
#elif defined(enableSERIAL) && defined(enableSERIAL_binary)   
    
    //snprintf(text, sizeof(text), "%d\n", static_cast<uint8_t>(std::get<0>(acc)));   
    // writing into text[] in big endian order all values with the same integer 32 bit type.
    // i use int32_t. 
    // the acceleration values, which are in int16_t, are first transformed into int32_t
    // to keep their sign and then copied in big endian order.
    // the adc values, which have only 24 signisficant bits are just copied in big endian order as they are.

    volatile int32_t tmp = 0;
    volatile uint8_t pos = 0;
    // acc.x
    tmp = std::get<0>(acc);
    text[pos++] =  (tmp & 0xff000000) >> 24;
    text[pos++] =  (tmp & 0x00ff0000) >> 16;
    text[pos++] =  (tmp & 0x0000ff00) >> 8;
    text[pos++] =  (tmp & 0x000000ff);
    // acc.y
    tmp = std::get<1>(acc);
    text[pos++] =  (tmp & 0xff000000) >> 24;
    text[pos++] =  (tmp & 0x00ff0000) >> 16;
    text[pos++] =  (tmp & 0x0000ff00) >> 8;
    text[pos++] =  (tmp & 0x000000ff);
    // acc.z
    tmp = std::get<2>(acc);
    text[pos++] =  (tmp & 0xff000000) >> 24;
    text[pos++] =  (tmp & 0x00ff0000) >> 16;
    text[pos++] =  (tmp & 0x0000ff00) >> 8;
    text[pos++] =  (tmp & 0x000000ff);
    // adc.first
    tmp = adc.first;
    text[pos++] =  (tmp & 0xff000000) >> 24;
    text[pos++] =  (tmp & 0x00ff0000) >> 16;
    text[pos++] =  (tmp & 0x0000ff00) >> 8;
    text[pos++] =  (tmp & 0x000000ff);
    // adc.second
    tmp = adc.second;
    text[pos++] =  (tmp & 0xff000000) >> 24;
    text[pos++] =  (tmp & 0x00ff0000) >> 16;
    text[pos++] =  (tmp & 0x0000ff00) >> 8;
    text[pos++] =  (tmp & 0x000000ff); 
    
    // at the end we add a new line.
    // the new line in binary value is 0x0A ... do we really need to transmit it? 
    // if the receiver expects just a fixed number of bytes then the terminator character '\n' is useless
    // else if it looks for the terminator '\n' to stop the rx, ... there is the possibility that it stops if data contains the byte 0x0A.

// following code detects the presence of a '\n' inside data.
//    volatile bool found = false;
//    volatile int index = -1;
//    for(volatile int i=0; i<20; i++)
//    {
//        if(text[i] == '\n')
//        {
//            found = true;
//            index = i;
//        }
//    }
//    found = found;
//    index = index;
    
   	text[pos++] = '\n'; 

    HAL_UART_Transmit(&huart3, reinterpret_cast<uint8_t*>(text), pos, 0xFFFF);
        

    
#endif
 

#if defined(enableTRACE_all)  
    std::tuple<float, float, float> a {0.01 * std::get<0>(acc), 0.01 * std::get<1>(acc), 0.01 * std::get<2>(acc)};
    embot::hw::sys::puts("BNO055: acc = (" + std::to_string(std::get<0>(a)) + ", " + std::to_string(std::get<1>(a)) + ", " + std::to_string(std::get<2>(a)) + ") m/(s*s)" );  
#endif    
    
    return true;
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



