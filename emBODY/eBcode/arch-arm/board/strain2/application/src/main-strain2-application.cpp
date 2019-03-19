/*
 * Copyright (C) 2018 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/


#undef TEST_ENABLED

#undef TEST_HW_PGA308
#undef TEST_HW_ADC
#undef TEST_HW_TIM
#undef TEST_HW_ONEWIRE
#undef TEST_DSP
#undef TEST_HW_SI7051
#undef TEST_HW_BNO055

#if defined(TEST_ENABLED)
void tests_launcher_init();
void tests_tick();
#endif // #if defined(TEST_ENABLED)

#include "embot_app_theLEDmanager.h"
#include "embot_app_canprotocol.h"
#include "embot_app_theCANboardInfo.h"

#include "embot.h"

#include "embot_common.h"
#include "embot_binary.h"
#include "embot_dsp.h"

#include "stm32hal.h" // to see bsp_led_init etc
#include "embot_hw.h"
#include "embot_hw_sys.h"

#include "embot_hw_FlashStorage.h"
#include "embot_sys_theStorage.h"

#include "embot_app_theApplication.h"

#include "embot_app_application_theCANparserBasic.h"

#include "embot_app_application_theCANparserSTRAIN.h"
#include "embot_app_application_theCANparserIMU.h"
#include "embot_app_application_theCANparserTHERMO.h"

#include "embot_app_application_theSTRAIN.h"
#include "embot_app_application_theIMU.h"
#include "embot_app_application_theTHERMO.h"

#include "embot_hw_onewire.h"
#include "embot_hw_timer.h"
#include "embot_hw_pga308.h"
#include "embot_hw_adc.h"


#include "embot_hw_bsp_strain2.h"

#if defined(TEST_ENABLED)

#if defined(TEST_SI_ORIG)
//#include "embot_hw_si705x.h"
#else
#include "embot_hw_si7051.h"

const embot::hw::SI7051::Sensor SI7051sensor = embot::hw::bsp::strain2::thermometerSGAUGES;
const embot::hw::SI7051::Config SI7051config = embot::hw::bsp::strain2::thermometerSGAUGESconfig;

#endif
#endif // TEST_ENABLED


static const embot::app::canprotocol::versionOfAPPLICATION vAP = {2, 0, 1};
static const embot::app::canprotocol::versionOfCANPROTOCOL vCP = {2, 0};

static void userdeflauncher(void* param);
static void userdefonidle(void* param);
static void userdefonfatal(void* param);


static const embot::sys::Operation oninit = { embot::common::Callback(userdeflauncher, nullptr), 2048 };
static const embot::sys::Operation onidle = { embot::common::Callback(userdefonidle, nullptr), 512 };
static const embot::sys::Operation onfatal = { embot::common::Callback(userdefonfatal, nullptr), 64 };

#if defined(APPL_TESTZEROOFFSET)
static const std::uint32_t address = eembot::hw::flash::getpartition(embot::hw::FLASH::bootloader).address;
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
static const embot::common::Event evSTRAINtick = 0x00000001 << 1;
static const embot::common::Event evSTRAINdataready = 0x00000001 << 2;
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
   
    embot::sys::EventTask::Config configEV;
    
    configEV.startup = eventbasedtask_init;
    configEV.onevent = eventbasedtask_onevent;
    configEV.param = nullptr;
    configEV.stacksize = 4*1024;
    configEV.priority = 200;
    configEV.timeout = waitEventTimeout;
    eventbasedtask->start(configEV);
    
    
#if defined(TEST_ENABLED)
    tests_launcher_init();
    return;
#endif // #if defined(TEST_ENABLED)      
        
    // start canparser basic
    embot::app::application::theCANparserBasic &canparserbasic = embot::app::application::theCANparserBasic::getInstance();
    embot::app::application::theCANparserBasic::Config configbasic;
    canparserbasic.initialise(configbasic);  
    
    
    // start agent of strain
    embot::app::application::theSTRAIN &thestrain = embot::app::application::theSTRAIN::getInstance();
    embot::app::application::theSTRAIN::Config configstrain(evSTRAINtick, evSTRAINdataready, eventbasedtask);
    thestrain.initialise(configstrain); 
    
    // start canparser strain and link it to its agent
    embot::app::application::theCANparserSTRAIN &canparserstrain = embot::app::application::theCANparserSTRAIN::getInstance();
    embot::app::application::theCANparserSTRAIN::Config configparserstrain { &thestrain };
    canparserstrain.initialise(configparserstrain);  
                   
    // start agent of imu
    embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
    embot::app::application::theIMU::Config configimu(embot::hw::bsp::strain2::imuBOSCH, embot::hw::bsp::strain2::imuBOSCHconfig, evIMUtick, evIMUdataready, eventbasedtask);
    theimu.initialise(configimu);

    // start canparser imu and link it to its agent
    embot::app::application::theCANparserIMU &canparserimu = embot::app::application::theCANparserIMU::getInstance();
    embot::app::application::theCANparserIMU::Config configparserimu { &theimu };
    canparserimu.initialise(configparserimu);   
    
    // start agent of thermo
    embot::app::application::theTHERMO &thethermo = embot::app::application::theTHERMO::getInstance();
    embot::app::application::theTHERMO::Config configthermo(embot::hw::bsp::strain2::thermometerSGAUGES, embot::hw::bsp::strain2::thermometerSGAUGESconfig, evTHERMOtick, evTHERMOdataready, eventbasedtask);
    thethermo.initialise(configthermo);  

    // start canparser thermo and link it to its agent
    embot::app::application::theCANparserTHERMO &canparserthermo = embot::app::application::theCANparserTHERMO::getInstance();
    embot::app::application::theCANparserTHERMO::Config configparserthermo { &thethermo };
    canparserthermo.initialise(configparserthermo); 

    // finally start can. i keep it as last because i dont want that the isr-handler calls its onrxframe() 
    // before the eventbasedtask is created.
    embot::hw::result_t r = embot::hw::resNOK;
    embot::hw::can::Config canconfig; // default is tx/rxcapacity=8
    canconfig.txcapacity = maxOUTcanframes;
    canconfig.onrxframe = embot::common::Callback(alerteventbasedtask, nullptr); 
    r = embot::hw::can::init(embot::hw::CAN::one, canconfig);
    r = embot::hw::can::setfilters(embot::hw::CAN::one, embot::app::theCANboardInfo::getInstance().getCANaddress());
    r = r;

#if 0  
    // it starts the tx immediately  
    embot::hw::sys::delay(10000);
    thestrain.configure(20*1000);
    thestrain.start(embot::app::canprotocol::analog::polling::Message_SET_TXMODE::StrainMode::txUncalibrated);
#endif 
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
    //#warning --> we should init the objects which holds outframes with maxOUTcanframes ... so that no more than maxOUTcanframes are pushed_back
}
    


static void eventbasedtask_onevent(embot::sys::Task *t, embot::common::EventMask eventmask, void *p)
{   
    if(0 == eventmask)
    {   // timeout ... 
#if defined(TEST_ENABLED)        
        tests_tick();  
#endif // #if defined(TEST_ENABLED)        
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
            embot::app::application::theCANparserSTRAIN &canparserstrain = embot::app::application::theCANparserSTRAIN::getInstance();
            embot::app::application::theCANparserIMU &canparserimu = embot::app::application::theCANparserIMU::getInstance();
            embot::app::application::theCANparserTHERMO &canparserthermo = embot::app::application::theCANparserTHERMO::getInstance();
            // process w/ the basic parser, if not recognised call the parse specific of the board
            if(true == canparserbasic.process(frame, outframes))
            {                   
            }
            else if(true == canparserstrain.process(frame, outframes))
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
    
    
    if(true == embot::binary::mask::check(eventmask, evSTRAINtick))
    {        
        embot::app::application::theSTRAIN &thestrain = embot::app::application::theSTRAIN::getInstance();
        thestrain.tick(outframes);        
    }
            
    if(true == embot::binary::mask::check(eventmask, evSTRAINdataready))
    {        
        embot::app::application::theSTRAIN &thestrain = embot::app::application::theSTRAIN::getInstance();
        thestrain.processdata(outframes);        
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


// - all the tests are in here

#if defined(TEST_ENABLED)

#if defined(TEST_HW_ADC)

// test adc
static std::uint16_t bufferSix[6] = {0};

static volatile std::uint8_t adcdmadone = 0;

static void adcdmadone_set(void *p)
{
     adcdmadone = 1;
}

static void adcdmadone_clr(void *p)
{
    adcdmadone = 0;
}

static bool adcdmadone_isset()
{
    if(0 == adcdmadone)
    {
        return false;
    }
    
    return true;
}

static embot::common::Time delta = 0;
static std::uint16_t items[6] = {0};

#endif // #if defined(TEST_HW_ADC)



#if defined(TEST_HW_TIM)

embot::hw::timer::Timer timer2use = embot::hw::timer::Timer::six;
static const std::uint32_t maxpulses = 10;
static std::uint64_t tt0 = 0;
static std::uint64_t del =0;
static std::int64_t diff =0;
void toggleled(void*)
{
    static std::uint32_t count = 0;
    static std::uint8_t on = 0;
    static std::uint32_t num = 0;
    if(count == 5000)
    {
        
        std::uint64_t tt1 = embot::sys::timeNow();
        
        del = tt1 - tt0;
        del = del;
        
        diff = del - 500000;
        
        tt0 = tt1;
        
        if(0 == on)
        {
            embot::hw::led::off(embot::hw::led::LED::one); 
            on = 1;
        }
        else 
        {
            embot::hw::led::on(embot::hw::led::LED::one);
            on = 0;
            num++;
        }
        //embot::hw::led::toggle(embot::hw::led::LED::one);
        count = 0;
    }
    count ++;
    
    if(num >= maxpulses)
    {
        static std::uint8_t x = 0;
        num = 0;
        embot::hw::timer::stop(timer2use);
        embot::hw::timer::Config con;
        con.onexpiry.callback = toggleled;
        if(0 == x)
        {
            con.time = embot::common::time1microsec * 50;
            embot::hw::timer::configure(timer2use, con);
            x = 1;
        }
        else
        {
            con.time = embot::common::time1microsec * 100;
            embot::hw::timer::configure(timer2use, con);
            x = 0;
        }
        embot::hw::timer::start(timer2use);
    }
}

void test_tim_init(void)
{
    embot::hw::led::init(embot::hw::led::LED::one);
    
    embot::hw::led::on(embot::hw::led::LED::one); 
    embot::hw::led::off(embot::hw::led::LED::one); 
    
    embot::hw::timer::Config con;
    con.time = embot::common::time1microsec * 100;
    con.onexpiry.callback = toggleled;
    embot::hw::timer::init(timer2use, con);
    embot::hw::timer::start(timer2use);
}

#endif // defined(TEST_HW_TIM)




#if defined(TEST_DSP)
static std::int16_t mat1[3*3] = {0};
static std::int16_t vec1[3*2] = {0};
static std::int16_t vec2[3*2] = {0};
static std::int16_t vin[3] = {0};
static std::int16_t vout[3] = {0};

embot::dsp::q15::matrix ma1;
embot::dsp::q15::matrix ma2;
embot::dsp::q15::matrix ma3;

embot::dsp::q15::matrix ve1;
embot::dsp::q15::matrix ve2;
#endif // #if defined(TEST_DSP)

void ciao(void *p)
{
    static uint32_t io = 0;
    io++;
}

void tests_launcher_init()
{
#if defined(TEST_HW_BNO055)
    embot::hw::BNO055::init(embot::hw::bsp::strain2::imuBOSCH, embot::hw::bsp::strain2::imuBOSCHconfig); 

    std::uint8_t dat08[16] = {0};
    embot::common::Data data(dat08, 8);
    
    embot::hw::BNO055::Info info;
    embot::hw::BNO055::get(embot::hw::bsp::strain2::imuBOSCH, info, embot::common::time1second);
    
    bool val = info.isvalid();
    val = val;
    
    embot::hw::BNO055::read(embot::hw::bsp::strain2::imuBOSCH, embot::hw::BNO055::Register::CHIP_ID, data, embot::common::time1second);
    dat08[0] = dat08[0];

    std::memset(dat08, 0, sizeof(dat08));
    embot::common::Callback cbk(ciao, nullptr);
    embot::hw::BNO055::read(embot::hw::bsp::strain2::imuBOSCH, embot::hw::BNO055::Register::CHIP_ID, data, cbk);
    for(;;)
    {
        if(true == embot::hw::BNO055::operationdone(embot::hw::bsp::strain2::imuBOSCH))
        {
            break;
        }        
    }
    
    
    const embot::hw::BNO055::Mode mode2use = embot::hw::BNO055::Mode::NDOF; // NDOF // AMG // IMU //NDOF_FMC_OFF
    std::memset(dat08, 0, sizeof(dat08));
    
    embot::hw::BNO055::read(embot::hw::bsp::strain2::imuBOSCH, embot::hw::BNO055::Register::DATASET_START, data, embot::common::time1second);    
    embot::hw::BNO055::write(embot::hw::bsp::strain2::imuBOSCH, embot::hw::BNO055::Register::OPR_MODE, static_cast<std::uint8_t>(mode2use), embot::common::time1second);
    embot::hw::BNO055::read(embot::hw::bsp::strain2::imuBOSCH, embot::hw::BNO055::Register::DATASET_START, data, embot::common::time1second);
    embot::hw::BNO055::read(embot::hw::bsp::strain2::imuBOSCH, embot::hw::BNO055::Register::DATASET_START, data, embot::common::time1second);
    
    embot::hw::sys::delay(10*1000);
#endif
    
#if defined(TEST_HW_SI7051)       
    embot::hw::SI7051::init(SI7051sensor, SI7051config);       
#endif
    
#if defined(TEST_HW_PGA308)
    
    embot::hw::PGA308::Config pga308cfg;
        
    // common settings
    pga308cfg.powerongpio = embot::hw::stm32GPIO(EN_2V8_GPIO_Port, EN_2V8_Pin);
    pga308cfg.poweronstate = embot::hw::gpio::State::SET;
    pga308cfg.onewireconfig.rate = embot::hw::onewire::Rate::tenKbps;
    pga308cfg.onewireconfig.usepreamble =  true;
    pga308cfg.onewireconfig.preamble = 0x55;
    
    // from embot::hw::PGA308::one to embot::hw::PGA308::six
    
    // embot::hw::PGA308::zero
    pga308cfg.onewirechannel = embot::hw::onewire::Channel::one;
    pga308cfg.onewireconfig.gpio = embot::hw::stm32GPIO(W_STRAIN1_GPIO_Port, W_STRAIN1_Pin);
    embot::hw::PGA308::init(embot::hw::PGA308::Amplifier::one, pga308cfg);
    
    // embot::hw::PGA308::two
    pga308cfg.onewirechannel = embot::hw::onewire::Channel::two;
    pga308cfg.onewireconfig.gpio = embot::hw::stm32GPIO(W_STRAIN2_GPIO_Port, W_STRAIN2_Pin);
    embot::hw::PGA308::init(embot::hw::PGA308::Amplifier::two, pga308cfg);
    
    // embot::hw::PGA308::three
    pga308cfg.onewirechannel = embot::hw::onewire::Channel::three;
    pga308cfg.onewireconfig.gpio = embot::hw::stm32GPIO(W_STRAIN3_GPIO_Port, W_STRAIN3_Pin);
    embot::hw::PGA308::init(embot::hw::PGA308::Amplifier::three, pga308cfg);
        
    // embot::hw::PGA308::four
    pga308cfg.onewirechannel = embot::hw::onewire::Channel::four;
    pga308cfg.onewireconfig.gpio = embot::hw::stm32GPIO(W_STRAIN4_GPIO_Port, W_STRAIN4_Pin);
    embot::hw::PGA308::init(embot::hw::PGA308::Amplifier::four, pga308cfg);    
    
    // embot::hw::PGA308::five
    pga308cfg.onewirechannel = embot::hw::onewire::Channel::five;
    pga308cfg.onewireconfig.gpio = embot::hw::stm32GPIO(W_STRAIN5_GPIO_Port, W_STRAIN5_Pin);
    embot::hw::PGA308::init(embot::hw::PGA308::Amplifier::five, pga308cfg);     

    // embot::hw::PGA308::six
    pga308cfg.onewirechannel = embot::hw::onewire::Channel::six;
    pga308cfg.onewireconfig.gpio = embot::hw::stm32GPIO(W_STRAIN6_GPIO_Port, W_STRAIN6_Pin);
    embot::hw::PGA308::init(embot::hw::PGA308::Amplifier::six, pga308cfg);   
    
#endif    
    
    
#if defined(TEST_HW_ADC)   
    embot::hw::adc::Config adcConf;
    adcConf.numberofitems = 6;
    adcConf.destination = bufferSix;
    adcConf.oncompletion.callback = adcdmadone_set;
    embot::hw::adc::init(embot::hw::adc::Port::one, adcConf);
#endif // #if defined(TEST_HW_ADC)
    
    
#if defined(TEST_HW_TIM)    
    test_tim_init();
#endif

    
#if defined(TEST_HW_ONEWIRE)  

    embot::hw::onewire::Config con;
    con.preamble = 0x55;
    con.usepreamble = true;
    con.rate = embot::hw::onewire::Rate::tenKbps;
    con.gpio.load(W_STRAIN1_GPIO_Port, W_STRAIN1_Pin);
    embot::hw::onewire::init(embot::hw::onewire::Channel::one, con);

#endif    
    
}

void counter(void *p)
{
    static int count = 0;
    
    count++;
}

void tests_tick() 
{
    
#if defined(TEST_HW_BNO055)
    
    embot::common::Time starttime = embot::sys::timeNow();
    embot::common::Time endtime = starttime;
    
    std::uint8_t value = 0;
    std::uint8_t values[32] = {0};
    embot::common::Data data;
    const int ntimes = 1000;
    embot::common::Callback cbk(counter, nullptr);
    
#define MODE_DIRECT
    
    embot::hw::BNO055::Data datafull;
    embot::common::Triple<float> acc;
    embot::common::Triple<float> gyr;
    embot::common::Triple<float> eul;
    std::uint8_t calibmag = 0;
    std::uint8_t calibacc = 0;
    int num = 0;
    for(int i=0; i<ntimes; i++)
    {
      
        #if defined(MODE_DIRECT)
        embot::hw::BNO055::acquisition(embot::hw::bsp::strain2::imuBOSCH, embot::hw::BNO055::Set::FULL, datafull, cbk);
        #else
        embot::hw::BNO055::acquisition(embot::hw::bsp::strain2::imuBOSCH, embot::hw::BNO055::Set::FULL, cbk);
        #endif
                
        
        for(;;)
        {
            if(true == embot::hw::BNO055::operationdone(embot::hw::bsp::strain2::imuBOSCH))
            {
                break;
            }
        }
        
        #if defined(MODE_DIRECT)
        #else
        embot::hw::BNO055::read(embot::hw::bsp::strain2::imuBOSCH, datafull);
        #endif        
        
        datafull.getACC(acc);
        datafull.getGYR(gyr);
        datafull.getEUL(eul);
        calibacc = datafull.calibrationOfACC();
        calibmag = datafull.calibrationOfMAG();
        value = value;    
    }


    endtime = embot::sys::timeNow();
    
    static embot::common::Time delta = 0;
    delta = endtime - starttime;
    delta = delta;
    num = num;   

    // boh, 2 ms per fulldata .....
    
    
#endif
    
    
#if defined(TEST_HW_SI7051)
    
    embot::common::Time starttime = embot::sys::timeNow();
    embot::common::Time endtime = starttime;
      
    embot::hw::SI7051::Temperature temp;    

    const int ntimes = 1000;
    embot::common::Callback cbk(counter, nullptr);
    int num = 0;
    for(int i=0; i<ntimes; i++)
    {
   
        embot::hw::SI7051::acquisition(SI7051sensor, cbk);
        
        for(;;)
        {
            if(true == embot::hw::SI7051::operationdone(SI7051sensor))
            {
                break;
            }
        }
        
        embot::hw::SI7051::read(SI7051sensor, temp);    
     
    }


    endtime = embot::sys::timeNow();
    
    static embot::common::Time delta = 0;
    delta = endtime - starttime;
    delta = delta;
    num = num;
    // 7 millisec each
    
#endif
    
    
#if defined(TEST_HW_ADC)
        
    adcdmadone_clr(nullptr);
    
    embot::common::Time starttime = embot::sys::timeNow();
    embot::common::Time endtime = starttime;
    
    embot::hw::adc::start(embot::hw::adc::Port::one);
 
    for(;;)
    {
        if(true == adcdmadone_isset())
        {
            endtime = embot::sys::timeNow();
            break;
        }
    }
    
    delta = endtime - starttime;
    delta = delta;
    
    if(delta > 666)
    {
        endtime ++; 
        starttime ++;            
    }
    
    
    std::memset(items, 0xff, sizeof(items)); 
    embot::hw::adc::get(embot::hw::adc::Port::one, items);
        
#endif  // #if defined(TEST_HW_ADC)   


#if defined(TEST_HW_ONEWIRE) 
    
        embot::hw::onewire::write(embot::hw::onewire::Channel::one, 0x07, 0x1248);
    
#endif // #if defined(TEST_HW_ONEWIRE)  

#if defined(TEST_DSP)
    
    static int sizE = 3;
    
    sizE ++;
    
    std::vector<int> ciaO(sizE);
    
    ciaO[0] = 1;
    
    bool satu = true;
    embot::dsp::Q15 ciao = embot::dsp::q15::convert(-0.5f, satu);
    satu = satu;
        
    ma1.load(3, 3, mat1);
    ma1.diagonal(embot::dsp::q15::negOne);
    //ma1.fill(embot::dsp::q15::posOneHalf);
    ma1.set(0, 0, embot::dsp::q15::posOneHalf+embot::dsp::q15::posOneFourth); 
    ma1.set(0, 1, embot::dsp::q15::posOneHalf+embot::dsp::q15::posOneFourth); 
    ma1.set(0, 2, embot::dsp::q15::negOneHalf);
    
    ve1.load(3, 1, vin);
    ve1.set(0, 0, embot::dsp::q15::posOneNearly);
    ve1.set(1, 0, embot::dsp::q15::posOneNearly);
    ve1.set(2, 0, embot::dsp::q15::posOneNearly);
    
    ve2.load(3, 1, vout);
    ve2.clear();
    
    // now we multiply matrix 3x3 w/ vector 3x1
    
    bool sat =  false;
    embot::dsp::q15::multiply(ma1, ve1, ve2, sat);
    sat = sat;
    

    ma2.load(3, 2, vec1);
    ma2.fill(embot::dsp::q15::posOneHalf);

    ma3.load(3, 2, vec2);
    ma3.clear();

    bool saturated =  false;
    embot::dsp::q15::multiply(ma1, ma2, ma3, saturated);
    saturated = saturated;

    double res = embot::dsp::q15::convert(ma3.get(0, 0));
    res = res;
    embot::dsp::Q15 v = 0;
    v = embot::dsp::q15::convert(-0.500, saturated);
    res = embot::dsp::q15::convert(v);
    v = embot::dsp::q15::convert(-0.250, saturated);
    res = embot::dsp::q15::convert(v);
    v = embot::dsp::q15::convert(-0.125, saturated);
    res = embot::dsp::q15::convert(v);       
    v = embot::dsp::q15::posOneNearly;
    res = embot::dsp::q15::convert(v);
    res =  res;

    v = embot::dsp::q15::opposite(v);
    res = embot::dsp::q15::convert(v);
    res = res;

    v = embot::dsp::q15::negOne;
    res = embot::dsp::q15::convert(v);
    v = embot::dsp::q15::opposite(v);
    res = embot::dsp::q15::convert(v); 
    v = embot::dsp::q15::opposite(v);
    res = embot::dsp::q15::convert(v);        

    res = embot::dsp::q15::convert(v);
    v = v;
    
#endif // #if defined(TEST_DSP)    
    
}

#endif // #if defined(TEST_ENABLED)

///


