
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#if 0

    Brief explanation of the application

    The main() function starts the embot::os environment.
    The function embot::os::start() executes in exclusive mode the function initSystem() and 
    when it terminates it starts the scheduling of the other threads.
    
    In this application we start a single thread called tMAIN.
    This thread starts some services in its tMAIN_startup() function and then it executes function
    tMAIN_onevent() when triggered by events. 

    The function tMAIN_startup() inits the sensors BNO055, ADS122C04 and AD7147, starts a timer 
    which sends an event to the tMAIN thread every 10 ms for sensor acquisition and finally programs 
    a GPIO to send a transmission event to the thread when triggered.
    
    The function tMAIN_onevent() executes only when events are sent to it.
        
    The events sent to the thread activate data acquisition which is done in chain (IMU, ADC, CDC) and data 
    transmission over USART.
    
    The application can be used for normal use and also for debug or trace.
    
    For normal use you need to define macro macro_APPL_MODE_STANDARD and for debuf the other macros.

#endif

#define macro_APPL_MODE_STANDARD
//#define macro_APPL_MODE_STANDARD_CYCLE_TRANSMISSION
//#define macro_APPL_MODE_STANDARD_PERIODIC_TRANSMIT
//#define macro_APPL_MODE_DEBUG
    

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
#undef  macro_enableSERIAL  
// if macro_enableSERIAL is defined, it prints in human readable format
#undef  macro_enableSERIAL_string
// if macro_enableSERIAL is defined, it prints in compact binary format
#undef  macro_enableSERIAL_binary
// if defined acquisitionPeriod is = 100 ms, else it is the normal 10 ms 
#undef  macro_enableACQUISITION_slow
// if defined, the values are asked to the chips, if undefined we tx fake values
#undef  macro_enableACQUISITION
// if enabled the tx trigger evtDATAtransmit comes (also) from a periodic timer
#undef macro_enablePERIODICtransmit
// if enabled the tx trigger evtDATAtransmit comes (also) from the exti trigger
#undef macro_enableEXTItransmit
#undef macro_enableEXTItransmitBLUEbutton
// if defined we print values on the trace port
#undef  macro_enableTRACE
// the following specifies what to print
#undef macro_enableTRACE_timeofusart
#undef macro_enableTRACE_histograms
#undef macro_enableTRACE_activations
#undef macro_enableTRACE_readings

// section for sensors' enabling
#define macro_enableSENSOR_bno055
#define macro_enableSENSOR_ads122c04
#define macro_enableSENSOR_ad7147

#if defined(macro_APPL_MODE_STANDARD)

    // use it to tx acquisitions over usart in binary format upon trigger from external interrupt
    
    #define macro_enableACQUISITION
    #undef macro_enableACQUISITION_slow
    #define macro_enableSERIAL
    #define macro_enableSERIAL_binary
    #undef  macro_enableSERIAL_string
    //#define macro_enableSERIAL_string
    // the tx is activated by external trigger only
    #define macro_enableEXTItransmit
    #undef macro_enableEXTItransmitBLUEbutton
    //#define macro_enableEXTItransmitBLUEbutton
    #undef  macro_enableCYCLEtransmit
    #undef  macro_enablePERIODICtransmit
    #undef macro_enableSLOWtxperiod
    // we disable trace
    #undef macro_enableTRACE

#elif defined(macro_APPL_MODE_STANDARD_CYCLE_TRANSMISSION)

    // use it to tx acquisitions over usart in binary format at the beginning of every cycle
    
    #define macro_enableACQUISITION
    #undef macro_enableACQUISITION_slow
    #define macro_enableSERIAL
    #define macro_enableSERIAL_binary
    #undef  macro_enableSERIAL_string
    // the tx is done at very start of cycles
    #define macro_enableCYCLEtransmit
    #undef  macro_enableEXTItransmit
    #undef macro_enablePERIODICtransmit
    #undef macro_enableSLOWtxperiod
    // we disable trace
    #undef macro_enableTRACE

#elif defined(macro_APPL_MODE_STANDARD_PERIODIC_TRANSMIT)

    // use it to tx acquisitions over usart in binary format upon trigger periodic timer @ 10 ms
    
    #define macro_enableACQUISITION
    #undef macro_enableACQUISITION_slow
    #define macro_enableSERIAL
    #define macro_enableSERIAL_binary
    #undef  macro_enableSERIAL_string
    // the tx is activated by a periodic timer @ 10 ms
    #undef  macro_enableEXTItransmit
    #undef macro_enableCYCLEtransmit
    #define macro_enablePERIODICtransmit
    #undef macro_enableSLOWtxperiod
    // we disable trace
    #undef macro_enableTRACE

#elif defined(macro_APPL_MODE_DEBUG)

    // use it to debug or trace acquisition timings

    // mode with trace only and periodic activation
    // the trace is configured below
    
//    #define macro_enableACQUISITION
//    #undef macro_enableACQUISITION_slow
//    #undef macro_enableSERIAL
//    #define macro_enableTRACE
//    #undef  macro_enableEXTItransmit
//    #define macro_enablePERIODICtransmit
//    #define macro_enableSLOWtxperiod


    // mode with serial in string form and periodic activation

    #define macro_enableACQUISITION
    #define macro_enableACQUISITION_slow
    #define macro_enableSERIAL
    #define macro_enableSERIAL_string
    //#define macro_enableSERIAL_binary
    #define  macro_enableTRACE
    //#define  macro_enableEXTItransmit
    #define macro_enablePERIODICtransmit
    #define macro_enableSLOWtxperiod
    
    // the tx is done at very start of cycles
    #undef macro_enableCYCLEtransmit



    // it prints the times of acquisition of sensors and the time for the TX over USART
    
//    #define macro_enableACQUISITION
//    #undef macro_enableACQUISITION_slow
//    #define macro_enableSERIAL
//    #define macro_enableSERIAL_binary
//    #define macro_enableTRACE
//    #define macro_enablePERIODICtransmit
//    #define macro_enableSLOWtxperiod
    

#endif



#if defined(macro_enableTRACE)
#define macro_enableTRACE_timeofusart
//#define macro_enableTRACE_histograms
//#define macro_enableTRACE_activations
#define macro_enableTRACE_readings
#endif

// the following configuration for chip ad7147 allows to operate on register STAGEx_AFE_OFFSET
// see table 36 of datasheet https://www.analog.com/media/en/technical-documentation/data-sheets/AD7147.pdf
// and comments embot::hw::ad7147::Config for more explanations.
//constexpr embot::hw::ad7147::Config ad7147config {0x2000};
//constexpr embot::hw::ad7147::Config ad7147config {34, 0}; // equal to {0x2200} -> it applies a positive AFE offset of 34 * 0.32 pF
//constexpr embot::hw::ad7147::Config ad7147config {32, 0}; // equal to {0x2000} -> it applies a positive AFE offset of 32 * 0.32 pF
// from the datasheet the value of positive AFE can be in range [0, 20] pF with steps of 0.32 pF.
// hence valid values range for first parameter of constructor Config(uint8_t POS_AFE_OFFSET, uint8_t NEG_AFE_OFFSET) is
// POS_AFE_OFFSET = [0, 62] maybe also 63 (which is 20.16 pF). 

constexpr embot::hw::ad7147::Config ad7147config {32, 0}; // equal to {0x2000} -> it applies a positive AFE offset of 32 * 0.32 pF

static void s_chips_init();
static void s_imu_start();
static void s_imu_get();
static void s_adc_start(embot::hw::ads122c04::Channel chn);
static void s_adc_get();

static void s_cdc_start(embot::hw::AD7147 cdc);
static void s_cdc_get(embot::hw::AD7147 cdc);

static void s_transmit();

// activation events for thread tMAIN

constexpr embot::os::Event evtAcquisition = embot::core::binary::mask::pos2mask<embot::os::Event>(0);
constexpr embot::os::Event evtIMUdataready = embot::core::binary::mask::pos2mask<embot::os::Event>(1);
constexpr embot::os::Event evtADCchn1ready = embot::core::binary::mask::pos2mask<embot::os::Event>(2);
constexpr embot::os::Event evtADCchn2ready = embot::core::binary::mask::pos2mask<embot::os::Event>(3);

constexpr embot::os::Event evtCDC1acquire = embot::core::binary::mask::pos2mask<embot::os::Event>(4);
constexpr embot::os::Event evtCDC1ready = embot::core::binary::mask::pos2mask<embot::os::Event>(5);
constexpr embot::os::Event evtCDC2acquire = embot::core::binary::mask::pos2mask<embot::os::Event>(6);
constexpr embot::os::Event evtCDC2ready = embot::core::binary::mask::pos2mask<embot::os::Event>(7);

constexpr embot::os::Event evtDATAtransmit = embot::core::binary::mask::pos2mask<embot::os::Event>(15);

 
#if defined(macro_enableACQUISITION_slow) 
constexpr embot::core::relTime acquisitionPeriod = 100*embot::core::time1millisec;
#else
constexpr embot::core::relTime acquisitionPeriod = 10*embot::core::time1millisec;
#endif


embot::tools::Histogram *histoIMU {nullptr};
//embot::tools::Histogram *histoUSART {nullptr};
void txrequest(void *p)
{

    embot::os::Thread *t = reinterpret_cast<embot::os::Thread *>(p);
    t->setEvent(evtDATAtransmit);
}

void tMAIN_startup(embot::os::Thread *t, void *param)
{  

#if defined(macro_enableTRACE)
    embot::core::print("macro_enableTRACE is defined");
    
#if defined(macro_enableTRACE_timeofusart)
    embot::core::print("macro_enableTRACE_timeofusart is defined");
#endif
#if defined(macro_enableTRACE_histograms)
    embot::core::print("macro_enableTRACE_histograms is defined");
#endif  
#if defined(macro_enableTRACE_activations)
    embot::core::print("macro_enableTRACE_activations is defined");
#endif
#if defined(macro_enableTRACE_readings)
    embot::core::print("macro_enableTRACE_readings is defined");
#endif
         
#endif
    
    // init imu + adc + cdc
    s_chips_init(); 

#if defined(macro_enableEXTItransmit)
    // init the ext interrupt button
    
    constexpr embot::hw::BTN buttonBLUE = embot::hw::BTN::one;
    constexpr embot::hw::BTN buttonPB8 = embot::hw::BTN::two;

    // value can be: buttonPB8 ore buttonBLUE
#if defined(macro_enableEXTItransmitBLUEbutton)
    constexpr embot::hw::BTN buttonTX = buttonBLUE; 
#else
    constexpr embot::hw::BTN buttonTX = buttonPB8; 
#endif
        
    embot::hw::button::init(buttonTX, {embot::hw::button::Mode::TriggeredOnRelease, {txrequest, t}, 0});
#endif
    
    embot::core::print("evthread-startup: chips + exti initted" );
    
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtAcquisition, t));
    embot::os::Timer::Config cfg{acquisitionPeriod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);
    
    embot::core::print("evthread-startup: started timer which sends evtAcquisition to evthread every us = " + std::to_string(acquisitionPeriod));
 
#if defined(macro_enablePERIODICtransmit)   
#if defined(macro_enableSLOWtxperiod)    
    constexpr embot::core::relTime txperiod = 100*embot::core::time1millisec;
#else
    constexpr embot::core::relTime txperiod = 10*embot::core::time1millisec;
#endif    
    embot::os::Timer *tmrTX = new embot::os::Timer;   
    embot::os::Action actTX(embot::os::EventToThread(evtDATAtransmit, t));
    embot::os::Timer::Config cfgTX{txperiod, actTX, embot::os::Timer::Mode::forever, 0};
    tmrTX->start(cfgTX);
#endif    
    
#if defined(macro_enableTRACE)
    embot::core::print("macro_enableTRACE is defined");
    
#if defined(macro_enableTRACE_timeofusart)
    embot::core::print("macro_enableTRACE_timeofusart is defined");
#endif
#if defined(macro_enableTRACE_histograms)
    embot::core::print("macro_enableTRACE_histograms is defined");
#endif  
#if defined(macro_enableTRACE_activations)
    embot::core::print("macro_enableTRACE_activations is defined");
#endif
#if defined(macro_enableTRACE_readings)
    embot::core::print("macro_enableTRACE_readings is defined");
#endif
         
#endif
    
}

uint64_t timeadc_start[2] = {0, 0};
uint64_t timeadc_ready[2] = {0, 0};
uint64_t timeadc_delta[2] = {0, 0};

uint64_t timeadc_duration_of_acquisition_start[2] = {0, 0};

uint64_t timeskin_start[2] = {0, 0};
uint64_t timeskin_ready[2] = {0, 0};
uint64_t timeskin_delta[2] = {0, 0};

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

static uint64_t numberofCYCLE = 0; 
static uint8_t skin = 0;
constexpr uint8_t skindecimationfactor = 2;

void tMAIN_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{    
    if(0 == eventmask)
    {   // timeout ...          
        return;
    }

    if(true == embot::core::binary::mask::check(eventmask, evtAcquisition)) 
    {
        numberofCYCLE++;
        
#if defined(macro_enableTRACE_activations)        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("tMAIN_onevent(): evtAcquisition received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));    
#endif          
        s_imu_start();
        
#if defined(macro_enableCYCLEtransmit)
        t->setEvent(evtDATAtransmit);
        //s_transmit();
#endif        
    }
    
    if(true == embot::core::binary::mask::check(eventmask, evtIMUdataready))
    {
#if defined(macro_enableTRACE_activations)        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("tMAIN_onevent(): evtIMUdataready received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));    
#endif        
        s_imu_get();
        timeadc_start[0] = embot::core::now();
        s_adc_start(embot::hw::ads122c04::Channel::one); 
        timeadc_duration_of_acquisition_start[0] = embot::core::now() - timeadc_start[0];         
    }

    if(true == embot::core::binary::mask::check(eventmask, evtADCchn1ready))
    {
#if defined(macro_enableTRACE_activations)        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("tMAIN_onevent(): evtADCchn1ready received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));    
#endif      
        timeadc_start[1] = timeadc_ready[0] = embot::core::now();        
        s_adc_start(embot::hw::ads122c04::Channel::two);  
        timeadc_duration_of_acquisition_start[1] = embot::core::now() - timeadc_start[1];
    }
    
    if(true == embot::core::binary::mask::check(eventmask, evtADCchn2ready))
    {
#if defined(macro_enableTRACE_activations)        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("tMAIN_onevent(): evtADCchn2ready received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));    
#endif 
        timeadc_ready[1] = embot::core::now();     
        timeadc_delta[0] = timeadc_ready[0] - timeadc_start[0];     
        timeadc_delta[1] = timeadc_ready[1] - timeadc_start[1];        
        s_adc_get();
        
        // choose the skin to acquire
        uint8_t skin = numberofCYCLE % skindecimationfactor;
        
        if((0 == skin) || (1 == skin))
        {
            timeskin_start[skin] = embot::core::now();        
            s_cdc_start((0==skin) ? embot::hw::AD7147::one : embot::hw::AD7147::two); 
        }            
          
    }  

    if(true == embot::core::binary::mask::check(eventmask, evtCDC1ready))
    {
#if defined(macro_enableTRACE_activations)        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("tMAIN_onevent(): evtCDC1ready received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));    
#endif  
        timeskin_ready[0] = embot::core::now();
        timeskin_delta[0] = timeskin_ready[0] - timeskin_start[0];
        s_cdc_get(embot::hw::AD7147::one);
        
//        timeskin_start[1] = embot::core::now();            
//        s_cdc_start(embot::hw::AD7147::two);        
    }      

    if(true == embot::core::binary::mask::check(eventmask, evtCDC2ready))
    {
#if defined(macro_enableTRACE_activations)        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("tMAIN_onevent(): evtCDC2ready received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));    
#endif 
        timeskin_ready[1] = embot::core::now();          
        timeskin_delta[1] = timeskin_ready[1] - timeskin_start[1];
        s_cdc_get(embot::hw::AD7147::two);        
    }
    
    if(true == embot::core::binary::mask::check(eventmask, evtDATAtransmit))
    {
#if defined(macro_enableTRACE_activations)        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("tMAIN_onevent(): evtDATAtransmit received @ time = " + tf.to_string());    
#endif        
        s_transmit();
    }

}


void onIdle(embot::os::Thread *t, void* idleparam)
{
    static uint32_t i = 0;
    i++;
}

embot::os::EventThread* thr {nullptr};

void tMAIN(void* p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }

void initSystem(embot::os::Thread *t, void* initparam)
{
    
    embot::os::theTimerManager::getInstance().start({});     
    embot::os::theCallbackManager::getInstance().start({});  
    
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(2*embot::core::time1second); 
       
    
    embot::os::EventThread::Config tMAINconfig { 
        6*1024, 
        embot::os::Priority::high40, 
        tMAIN_startup,
        nullptr,
        50*embot::core::time1millisec,
        tMAIN_onevent
    };
        
    // create the main thread 
    thr = new embot::os::EventThread;          
    // and start it
    thr->start(tMAINconfig, tMAIN);   
}


// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    // configuration of the embot::os environment
    constexpr embot::os::InitThread::Config initcfg = { 4*1024, initSystem, nullptr };
    constexpr embot::os::IdleThread::Config idlecfg = { 1024, nullptr, nullptr, onIdle };
    constexpr embot::core::Callback onOSerror = { };
    constexpr embot::os::Config osconfig {1000*embot::core::time1microsec, initcfg, idlecfg, onOSerror};
    
    // embot::os::init() internally calls embot::hw::bsp::init() which also calls embot::core::init()
    embot::os::init(osconfig);
    
    // now i start the os
    // at first it is executed the tINIT thread in exclusive mode which calls initSystem() to init 
    // user defined threads and then stops execution allowing the system to schedule all other threads. 
    // note: function embot::os::start() is marked [[noreturn]] and hence ... it never returns     
    embot::os::start();   
}




#include <cstring>

static void s_chips_init()
{

#if defined(macro_enableTRACE_histograms)     
    histoIMU = new embot::tools::Histogram;
    histoIMU->init({0, 2500, 100});
    
//    histoUSART = new embot::tools::Histogram;
//    histoUSART->init({0, 500, 50});
#endif
    
#if !defined(macro_enableACQUISITION)
    
    // nothing. we just simulate the sensors
    
#else
 
    constexpr embot::core::relTime timeout = 5*embot::core::time1millisec;

#if defined(macro_enableSENSOR_bno055)    
    embot::hw::bno055::init(embot::hw::BNO055::one, {});     
    constexpr embot::hw::bno055::Mode mode = embot::hw::bno055::Mode::ACCGYRO;
    //constexpr embot::hw::bno055::Mode mode = embot::hw::bno055::Mode::NDOF;
    embot::hw::bno055::set(embot::hw::BNO055::one, mode, timeout);   
#endif

#if defined(macro_enableSENSOR_ads122c04)        
    embot::hw::ads122c04::init(embot::hw::ADS122C04::one, {});   
#endif
 

#if defined(macro_enableSENSOR_ad7147)        
    volatile embot::hw::result_t rr1 = embot::hw::result_t::NOK;
    volatile embot::hw::result_t rr2 = embot::hw::result_t::NOK;
    rr1 = embot::hw::ad7147::init(embot::hw::AD7147::one, ad7147config);
    rr2 = embot::hw::ad7147::init(embot::hw::AD7147::two, ad7147config);           
    rr1 = rr1;
    rr2 = rr2;
#endif
        
#endif
    
}

volatile embot::core::Time imu_start {0};
volatile embot::core::Time imu_stop {0};

std::tuple<int16_t, int16_t, int16_t> acc2transmit {0, 0, 0};
std::pair<uint32_t, uint32_t> adc2transmit {0, 0};
std::array<uint16_t, 24> cdc2transmit = {0};

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
#if defined(macro_enableSENSOR_bno055)     
    embot::hw::bno055::acquisition(embot::hw::BNO055::one, embot::hw::bno055::Set::A, data, cbk); 
#else
    data.acc.x = 100; data.acc.y = 200; data.acc.z = 300;
    cbk.execute();
#endif    
}

static void s_imu_get()
{
    imu_stop = embot::core::now();
    imu_acquisitiontime = imu_stop - imu_start;
    acc2transmit = {data.acc.x, data.acc.y, data.acc.z}; 

#if defined(macro_enableTRACE_histograms)    
    histoIMU->add(imu_acquisitiontime);
    static uint32_t cnt = 0;
    cnt++;
    if((cnt%1000) == 0)
    {
//        embot::core::print(std::string("imu read in ") + embot::core::TimeFormatter(imu_acquisitiontime).to_string());
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
        embot::core::print(std::string("histo IMU -> ") + str);
                                    
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
#if defined(macro_enableSENSOR_ads122c04)     
    embot::hw::ads122c04::acquisition(embot::hw::ADS122C04::one, chn, cbk); 
#else
    cbk.execute();
#endif    
}

static void s_adc_get()
{
#if defined(macro_enableSENSOR_ads122c04)    
    embot::hw::ads122c04::Values v {};
    embot::hw::ads122c04::read(embot::hw::ADS122C04::one, v); 
    adc2transmit = {v.v1, v.v2};
#else
    adc2transmit = {5000, 6000};
#endif    
}

void alertCDC1isready(void *p)
{
    thr->setEvent(evtCDC1ready);
}

void alertCDC2isready(void *p)
{
    thr->setEvent(evtCDC2ready);
}

static void s_cdc_start(embot::hw::AD7147 cdc)
{
    embot::core::Callback cbk((embot::hw::AD7147::one == cdc) ? alertCDC1isready : alertCDC2isready, nullptr);    
#if defined(macro_enableSENSOR_ad7147)      
    embot::hw::ad7147::acquisition(cdc, cbk);  
#else
    cbk.execute();
#endif    
}

static void s_cdc_get(embot::hw::AD7147 cdc)
{
#if 1 // enable true data

    embot::hw::ad7147::Values v {};    
#if defined(macro_enableSENSOR_ad7147)          
    embot::hw::ad7147::read(cdc, v);
#else
    v.fill(1);
#endif        
        
    size_t offset = (embot::hw::AD7147::one == cdc) ? 0 : 12;

    for(size_t i=0; i<v.size(); i++)
    {
        cdc2transmit[i+offset] = v[i];
    }


#else  // fake data used to test transmission of skin data over the serial link   
    uint16_t values[12] = {0};
    // in here i should read the 12 values directly from the device 
    // but for i di fake acquisition
    
    // fake retrieval
    static uint16_t offset = 0;
    for(int i=0; i<12; i++)
    {
        values[i] = offset + i;
    }
    
    offset += 16;
    
    if(embot::hw::AD7147::one == cdc)
    {
        for(int i=0; i<12; i++)
        {
            cdc2transmit[i+0] = values[i];
        }
    }
    else
    {
        for(int i=0; i<12; i++)
        {
            cdc2transmit[i+12] = values[i];
        }        
    }
    
#endif
    
}

bool s_print_values(const std::tuple<int16_t, int16_t, int16_t> &acc, const std::pair<uint32_t, uint32_t> &adc);

static void s_transmit()
{
    s_print_values(acc2transmit, adc2transmit);
}



bool s_print_values(const std::tuple<int16_t, int16_t, int16_t> &acc, const std::pair<uint32_t, uint32_t> &adc)
{   
    char text[512] = {0};
    
#if defined(macro_enableTRACE_readings)
    
    embot::core::print(" ");  
    
    std::string imutime = embot::core::TimeFormatter(imu_acquisitiontime).to_string();
    std::tuple<float, float, float> a {0.01 * std::get<0>(acc), 0.01 * std::get<1>(acc), 0.01 * std::get<2>(acc)};    
        
    embot::core::print( "BNO055: acc = (" + std::to_string(std::get<0>(a)) + ", " + std::to_string(std::get<1>(a)) + ", " + std::to_string(std::get<2>(a)) + ") m/(s*s)" +
                        "in " + imutime
        );  
     
    std::string adc1time = embot::core::TimeFormatter(timeadc_delta[0]).to_string();
    std::string adc2time = embot::core::TimeFormatter(timeadc_delta[1]).to_string();
            
    embot::core::print( "ADS122C04: (" + std::to_string(adc.first) + ", " + std::to_string(adc.second) + ")" + 
                        " in (" + adc1time + ", " + adc2time + ")"
    );     
        

    std::string skin1time = embot::core::TimeFormatter(timeskin_delta[0]).to_string();
    std::string skin2time = embot::core::TimeFormatter(timeskin_delta[1]).to_string();        
        
    std::string taxels02to13 {};
    std::string taxels14to24 {};
    for(size_t i=0; i<12; i++)
    {
        taxels02to13 += std::to_string(cdc2transmit[i]); 
        taxels02to13 += " ";
        taxels14to24 += std::to_string(cdc2transmit[i+12]);
        taxels14to24 += " ";
    }
    embot::core::print( std::string("P02->P13 = (") + 
                        taxels02to13 +         
                        ")" +
                        "in " + skin1time
    );        
    
    embot::core::print( std::string("P14->P24 = (") + 
                        taxels14to24 +         
                        ")" +
                        "in " + skin2time
    );  
    
             
#endif    
    
#if defined(macro_enableSERIAL) && defined(macro_enableSERIAL_string)
  
    // this prints in hex the entire range of values
    snprintf(text, sizeof(text), 
        "-> %08x %08x %08x %08x %08x [%08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x]\r\n\n", 
    
        std::get<0>(acc), std::get<1>(acc), std::get<2>(acc), adc.first, adc.second,
        cdc2transmit[ 0], cdc2transmit[ 1], cdc2transmit[ 2], cdc2transmit[ 3], cdc2transmit[ 4], cdc2transmit[ 5], cdc2transmit[ 6], cdc2transmit[ 7],
        cdc2transmit[ 8], cdc2transmit[ 9], cdc2transmit[10], cdc2transmit[11], cdc2transmit[12], cdc2transmit[13], cdc2transmit[14], cdc2transmit[15], 
        cdc2transmit[16], cdc2transmit[17], cdc2transmit[18], cdc2transmit[19], cdc2transmit[20], cdc2transmit[21], cdc2transmit[22], cdc2transmit[23]
    );        
    HAL_UART_Transmit(&huart3, reinterpret_cast<uint8_t*>(text), std::strlen(text), 0xFFFF);    
    
#elif defined(macro_enableSERIAL) && defined(macro_enableSERIAL_binary)   

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
    
    // so far we have used 20 bytes.
    
    // cdc -> 24 values, each with 2 bytes .... but we use four -> total = 96 bytes
    
    for(int i=0; i<cdc2transmit.size(); i++)
    {
        tmp = cdc2transmit[i];
        text[pos++] =  (tmp & 0xff000000) >> 24;
        text[pos++] =  (tmp & 0x00ff0000) >> 16;
        text[pos++] =  (tmp & 0x0000ff00) >> 8;
        text[pos++] =  (tmp & 0x000000ff); 
    }
    // total bytes = 20 + 96 = 116
    // it takes about S0:m1:u327 to transmit the 116 bytes 

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
    
#if defined(macro_enableTRACE_timeofusart)     
    embot::core::print(std::string("USART TX time: ") + embot::core::TimeFormatter(embot::core::now()-t0).to_string());
#endif

#endif
 

    return true;
}


// - oldies

//bool s_get_values(std::tuple<int16_t, int16_t, int16_t> &acc, std::pair<uint32_t, uint32_t> &adc)
//{
//#if !defined(macro_enableACQUISITION)
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



