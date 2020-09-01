
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
#include "embot_tools.h"

#include "embot_hw_bno055.h"
#include "embot_hw_ads122c04.h"
#include "embot_hw_ad7147.h"

#include <array>


// macro definition. we keep some behaviours in the same code.

// it tells to use the serial port to print data on it
#undef  enableSERIAL  
// if enableSERIAL is defined, it prints in human readable format
#undef  enableSERIAL_string
// if enableSERIAL is defined, it prints in compact binary format
#undef  enableSERIAL_binary
// if defined acquisitionPeriod is < 10 ms, else it is 1 sec 
//#define  enableACQUISITION_fast
// if defined, the values are asked to the chips bno055 and the adc, if undefined we tx fake values
#undef  enableACQUISITION
// if defined we print values on the trace port
#undef  enableTRACE
// enableTRACE, it enable print of everything, even in the fast ticked function. else it prints only basic info
#undef  enableTRACE_all



#define enableACQUISITION_fast
//#define enableTRACE_all
#define enableSERIAL
#define enableACQUISITION

#if defined(enableSERIAL)
//#define enableSERIAL_string
#define enableSERIAL_binary
#endif

static void s_chips_init();
static void s_imu_start();
static void s_imu_get();
static void s_adc_start(embot::hw::ads122c04::Channel chn);
static void s_adc_get();
static void s_transmit();


constexpr embot::os::Event evtAcquisition = embot::core::binary::mask::pos2mask<embot::os::Event>(0);
constexpr embot::os::Event evtIMUdataready = embot::core::binary::mask::pos2mask<embot::os::Event>(1);
constexpr embot::os::Event evtADCchn1ready = embot::core::binary::mask::pos2mask<embot::os::Event>(2);
constexpr embot::os::Event evtADCchn2ready = embot::core::binary::mask::pos2mask<embot::os::Event>(3);

constexpr embot::os::Event evtDATAtransmit = embot::core::binary::mask::pos2mask<embot::os::Event>(4);

 
#if defined(enableACQUISITION_fast) 
constexpr embot::core::relTime acquisitionPeriod = 10*embot::core::time1millisec;
#else
constexpr embot::core::relTime acquisitionPeriod = embot::core::time1second;
#endif

constexpr embot::core::relTime txperiod = 200*embot::core::time1millisec;

constexpr embot::hw::BTN buttonBLUE = embot::hw::BTN::one;
constexpr embot::hw::BTN buttonPB8 = embot::hw::BTN::two;
    
constexpr embot::hw::BTN buttonTX = buttonPB8; // but later on: buttonPB8 ore buttonBLUE

embot::tools::Histogram *histoIMU {nullptr};
//embot::tools::Histogram *histoUSART {nullptr};
void txrequest(void *p)
{
//    embot::hw::sys::puts(std::string("B @") + embot::core::TimeFormatter(embot::core::now()).to_string());
    embot::os::Thread *t = reinterpret_cast<embot::os::Thread *>(p);
    t->setEvent(evtDATAtransmit);
}

void eventbasedthread_startup(embot::os::Thread *t, void *param)
{       
    // init imu + adc
    s_chips_init(); 
    
    // init the ext interrupt button
    embot::hw::button::init(buttonTX, {embot::hw::button::Mode::TriggeredOnRelease, {txrequest, t}, 0});
    
    embot::hw::sys::puts("evthread-startup: chips + exti initted" );
    
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtAcquisition, t));
    embot::os::Timer::Config cfg{acquisitionPeriod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);
    
    embot::hw::sys::puts("evthread-startup: started timer which sends evtAcquisition to evthread every us = " + std::to_string(acquisitionPeriod));
    
//    embot::os::Timer *tmrTX = new embot::os::Timer;   
//    embot::os::Action actTX(embot::os::EventToThread(evtDATAtransmit, t));
//    embot::os::Timer::Config cfgTX{txperiod, actTX, embot::os::Timer::Mode::forever, 0};
//    tmrTX->start(cfgTX);
}

uint64_t timeadc_start[2] = {0, 0};
uint64_t timeadc_ready[2] = {0, 0};
uint64_t timeadc_delta[2] = {0, 0};

uint64_t timeadc_duration_of_acquisition_start[2] = {0, 0};

#if 0
static volatile bool IMUpinged {false};
static volatile bool SK0pinged {false};
static volatile bool SK1pinged {false};

static volatile bool BOHpinged {false};

std::array<bool, 128> pinged {false};
volatile uint32_t cnt {0};

void testi2c()
{
    
    if(cnt >= 128)
    {
       cnt = cnt; 
    }
    else
    {
    // ping the board    
        pinged[cnt] = embot::hw::i2c::ping(embot::hw::I2C::one, 2*cnt);        
    }
    // imu -> 41 (0x29)
    // sk0 -> 44 (0x2c)
    // sk1 -> 45 (0x2d)
    // ads -> 64 (0x40)
//    IMUpinged = embot::hw::i2c::ping(embot::hw::I2C::one, 0x52);
//    SK0pinged = embot::hw::i2c::ping(embot::hw::I2C::one, 0x58);
//    SK1pinged = embot::hw::i2c::ping(embot::hw::I2C::one, 0x5A);
//    BOHpinged = embot::hw::i2c::ping(embot::hw::I2C::one, 0x6C);

    
//    IMUpinged = IMUpinged;
    
    cnt++;
}

// 0x58 (con << 1)
// 0x5A (con << 1)
#endif

void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{ 
//  testi2c();    
    if(0 == eventmask)
    {   // timeout ...          
        return;
    }

#if 0
    if(true == embot::core::binary::mask::check(eventmask, evtAcquisition)) 
    {
#if defined(enableTRACE_all)        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::hw::sys::puts("evthread-onevent: evtAcquisition received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));    
#endif        
        s_imu_start();
    }
    
    if(true == embot::core::binary::mask::check(eventmask, evtIMUdataready))
    {
#if defined(enableTRACE_all)        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::hw::sys::puts("evthread-onevent: evtIMUdataready received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));    
#endif        
        s_imu_get();
        timeadc_start[0] = embot::core::now();
        s_adc_start(embot::hw::ads122c04::Channel::one); 
        timeadc_duration_of_acquisition_start[0] = embot::core::now() - timeadc_start[0];         
    }

    if(true == embot::core::binary::mask::check(eventmask, evtADCchn1ready))
    {
#if defined(enableTRACE_all)        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::hw::sys::puts("evthread-onevent: evtADCchn1ready received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));    
#endif      
        timeadc_start[1] = timeadc_ready[0] = embot::core::now();        
        s_adc_start(embot::hw::ads122c04::Channel::two);  
        timeadc_duration_of_acquisition_start[1] = embot::core::now() - timeadc_start[1];
    }
    
    if(true == embot::core::binary::mask::check(eventmask, evtADCchn2ready))
    {
#if defined(enableTRACE_all)        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::hw::sys::puts("evthread-onevent: evtADCchn2ready received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));    
#endif 
        timeadc_ready[1] = embot::core::now();     
        timeadc_delta[0] = timeadc_ready[0] - timeadc_start[0];     
        timeadc_delta[1] = timeadc_ready[1] - timeadc_start[1];        
        s_adc_get();        
    }    
    
    if(true == embot::core::binary::mask::check(eventmask, evtDATAtransmit))
    {
#if defined(enableTRACE_all)        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::hw::sys::puts("evthread-onevent: evtDATAtransmit received @ time = " + tf.to_string());    
#endif        
        s_transmit();
    }
#endif

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

// usart 921600

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




#include <cstring>

static void s_chips_init()
{

#if defined(enableTRACE_all)     
    histoIMU = new embot::tools::Histogram;
    histoIMU->init({0, 2500, 100});
    
//    histoUSART = new embot::tools::Histogram;
//    histoUSART->init({0, 500, 50});
#endif
    
#if !defined(enableACQUISITION)
    
    // nothing. we just simulate the sensors
    
#else
 
    constexpr embot::core::relTime timeout = 5*embot::core::time1millisec;
    constexpr uint32_t i2cspeed400k = 400000;
    constexpr uint32_t i2cspeed1m = 1000000;
    
    constexpr uint32_t i2cspeed = i2cspeed400k;
    
    embot::hw::bno055::Config bno055config { embot::hw::i2c::Descriptor { embot::hw::I2C::one, i2cspeed } };
    embot::hw::bno055::init(embot::hw::BNO055::one, bno055config); 
    
    constexpr embot::hw::bno055::Mode mode = embot::hw::bno055::Mode::ACCGYRO;
    //constexpr embot::hw::bno055::Mode mode = embot::hw::bno055::Mode::NDOF;
    embot::hw::bno055::set(embot::hw::BNO055::one, mode, timeout);   


    embot::hw::ads122c04::Config adsconfig { embot::hw::i2c::Descriptor { embot::hw::I2C::one, i2cspeed } };
    embot::hw::ads122c04::init(embot::hw::ADS122C04::one, adsconfig);   

    volatile embot::hw::result_t rr1 = embot::hw::result_t::NOK;
    volatile embot::hw::result_t rr2 = embot::hw::result_t::NOK;
    
    //embot::core::delay(500*embot::core::time1millisec);
    embot::hw::ad7147::Config skconfig { embot::hw::i2c::Descriptor { embot::hw::I2C::one, i2cspeed } };
    rr1 = embot::hw::ad7147::init(embot::hw::AD7147::one, skconfig);
    rr2 = embot::hw::ad7147::init(embot::hw::AD7147::two, skconfig);      
        
    rr1 = rr1;
    rr2 = rr2;
    
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

#if defined(enableTRACE_all)    
    histoIMU->add(imu_acquisitiontime);
    static uint32_t cnt = 0;
    cnt++;
    if((cnt%1000) == 0)
    {
//        embot::hw::sys::puts(std::string("imu read in ") + embot::core::TimeFormatter(imu_acquisitiontime).to_string());
        char str[256] = {0};
        const embot::tools::Histogram::Values *v = histoIMU->getvalues();
        snprintf(str, sizeof(str), "(%d) %f [%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f] %f", 
                                    v->inside.size(),
                                    100.0f*v->below/static_cast<float>(v->total),
                                    100.0f*v->inside[0]/static_cast<float>(v->total),
                                    100.0f*v->inside[1]/static_cast<float>(v->total),
                                    100.0f*v->inside[2]/static_cast<float>(v->total),
                                    100.0f*v->inside[3]/static_cast<float>(v->total),
                                    100.0f*v->inside[4]/static_cast<float>(v->total),  
                                    100.0f*v->inside[5]/static_cast<float>(v->total),
                                    100.0f*v->inside[6]/static_cast<float>(v->total), 
                                    100.0f*v->inside[7]/static_cast<float>(v->total),     
                                    100.0f*v->inside[8]/static_cast<float>(v->total), 
                                    100.0f*v->inside[9]/static_cast<float>(v->total), 
                                    100.0f*v->inside[10]/static_cast<float>(v->total), 
                                    100.0f*v->inside[11]/static_cast<float>(v->total),     
                                    100.0f*v->inside[12]/static_cast<float>(v->total), 
                                    100.0f*v->inside[13]/static_cast<float>(v->total),   
                                    100.0f*v->inside[14]/static_cast<float>(v->total), 
                                    100.0f*v->inside[15]/static_cast<float>(v->total), 
                                    100.0f*v->inside[16]/static_cast<float>(v->total), 
                                    100.0f*v->inside[17]/static_cast<float>(v->total),     
                                    100.0f*v->inside[18]/static_cast<float>(v->total), 
                                    100.0f*v->inside[19]/static_cast<float>(v->total),   
                                    100.0f*v->inside[20]/static_cast<float>(v->total), 
                                    100.0f*v->inside[21]/static_cast<float>(v->total),     
                                    100.0f*v->inside[22]/static_cast<float>(v->total), 
                                    100.0f*v->inside[23]/static_cast<float>(v->total), 
                                    100.0f*v->inside[24]/static_cast<float>(v->total),                                        
                                    100.0f*v->beyond/static_cast<float>(v->total)
                                    );
        embot::hw::sys::puts(std::string("histo IMU -> ") + str);
                                    
        histoIMU->reset();
    }  
#endif        
}


void alertadc01isready(void *p)
{
    thr->setEvent(evtADCchn1ready);
}

void alertadc02isready(void *p)
{
    thr->setEvent(evtADCchn2ready);
}

static void s_adc_start(embot::hw::ads122c04::Channel chn)
{  
    embot::core::Callback cbk((embot::hw::ads122c04::Channel::one == chn) ? alertadc01isready : alertadc02isready, nullptr);
    embot::hw::ads122c04::acquisition(embot::hw::ADS122C04::one, chn, cbk);   
}

static void s_adc_get()
{
    embot::hw::ads122c04::Values v {};
    embot::hw::ads122c04::read(embot::hw::ADS122C04::one, v); 
    adc2transmit = {v.v1, v.v2};
}

bool s_print_values(const std::tuple<int16_t, int16_t, int16_t> &acc, const std::pair<uint32_t, uint32_t> &adc);

static void s_transmit()
{
    s_print_values(acc2transmit, adc2transmit);
}



bool s_print_values(const std::tuple<int16_t, int16_t, int16_t> &acc, const std::pair<uint32_t, uint32_t> &adc)
{   
    char text[64] = {0};
    
#if defined(enableSERIAL) && defined(enableSERIAL_string)

#if 0
    // test for validation of acc
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
    snprintf(text, sizeof(text), "%04x %04x %04x %04x %04x\n", std::get<0>(acc), std::get<1>(acc), std::get<2>(acc), adc.first, adc.second);        
    HAL_UART_Transmit(&huart3, reinterpret_cast<uint8_t*>(text), std::strlen(text), 0xFFFF);    
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
    
//   	text[pos++] = '\n'; 
    embot::core::Time t0 = embot::core::now();
    HAL_UART_Transmit(&huart3, reinterpret_cast<uint8_t*>(text), pos, 0xFFFF);
    
#if defined(enableTRACE_all)     
    embot::hw::sys::puts(std::string("USART TX time: ") + embot::core::TimeFormatter(embot::core::now()-t0).to_string());
#endif

#endif
 

#if defined(enableTRACE_all)  
    std::tuple<float, float, float> a {0.01 * std::get<0>(acc), 0.01 * std::get<1>(acc), 0.01 * std::get<2>(acc)};
    embot::hw::sys::puts("BNO055: acc = (" + std::to_string(std::get<0>(a)) + ", " + std::to_string(std::get<1>(a)) + ", " + std::to_string(std::get<2>(a)) + ") m/(s*s)" );  
    embot::hw::sys::puts("ADC: (" + std::to_string(adc.first) + ", " + std::to_string(adc.second) + ")" );      
#endif    
    
    return true;
}


// - oldies

//bool s_get_values(std::tuple<int16_t, int16_t, int16_t> &acc, std::pair<uint32_t, uint32_t> &adc)
//{
//#if !defined(enableACQUISITION)
//    
//    constexpr int32_t accmin = -30;
//    constexpr int32_t accmax = +270;
//    
//    constexpr uint32_t adcmin = 0;
//    constexpr uint32_t adcmax = +300;
//    
//    static int16_t accx = accmin+0;
//    static int16_t accy = accmin+10;
//    static int16_t accz = accmin+20;
//    static uint32_t adc1 = adcmin+0;
//    static uint32_t adc2 = adcmin+10;    
//    
//    acc = {accx, accy, accz};
//    adc = {adc1, adc2};
//    
//    // implements triangular waveforms
//    accx++; accy++; accz++; adc1++; adc2++;
//    if(accx >= accmax) accx = accmin;
//    if(accy >= accmax) accy = accmin;
//    if(accz >= accmax) accz = accmin;
//    if(adc1 >= adcmax) adc1 = adcmin;
//    if(adc2 >= adcmax) adc2 = adcmin;    

//#else

//    acc = {0, 0, 0};
//    adc = {1, 1};
//    
//    // so far only bno055
//    #if 1
//    
//    imu_start = embot::core::now();
//    embot::hw::bno055::Data data;        
//    if(embot::hw::result_t::OK == embot::hw::bno055::acquisition(embot::hw::BNO055::one, embot::hw::bno055::Set::A, data))
//    {
//        for(;;)
//        {
//            if(true == embot::hw::bno055::operationdone(embot::hw::BNO055::one))
//            {
//                break;
//            }
//        }
//        
//        // ok, we have the values in data        
//        acc = {data.acc.x, data.acc.y, data.acc.z};        
//    }
//    imu_stop = embot::core::now();
//    #else
//    // better mode because it gives a timeout BUT to be tested
//    embot::hw::bno055::Data data; 
//    constexpr embot::core::relTime timeout = 10*embot::core::time1millisec;   
//    if(embot::hw::result_t::OK == embot::hw::bno055::acquisition(embot::hw::BNO055::one, embot::hw::bno055::Set::AMG, data, timeout))
//    {      
//        // ok, we have the values in data        
//        acc = {data.acc.x, data.acc.y, data.acc.z};        
//    }        
//    #endif

//#endif    
//       
//    return true;    
//}




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



