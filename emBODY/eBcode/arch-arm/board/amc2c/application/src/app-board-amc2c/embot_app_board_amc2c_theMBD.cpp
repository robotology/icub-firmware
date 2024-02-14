
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_board_amc2c_theMBD.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#if defined(STM32HAL_BOARD_AMC2C)
#include "embot_hw_bsp_amc2c.h"
//#warning READ CAREFULLY: we may use the same object for both amcbldc and amc2c
#elif defined(STM32HAL_BOARD_AMCBLDC)
#include "embot_hw_bsp_amcbldc.h"
#else
#error: choose a STM32HAL_BOARD_*
#endif

#include "embot_hw_button.h"
#include "embot_hw_motor.h"
#include "embot_app_scope.h"
#include "embot_hw_sys.h"
#include "embot_core.h"
#include "embot_app_theLEDmanager.h"
#include "embot_hw_testpoint.h"
#include <array>

// some protocol includes
#include "embot_prot_can_motor_polling.h"
#include "embot_prot_can_motor_periodic.h"


// mdb components
#include "AMC_BLDC.h"

// --------------------------------------------------------------------------------------------------------------------
// - defines
// --------------------------------------------------------------------------------------------------------------------
//#define TEST_DURATION_FOC

//#define TEST_FOC_logvalues


// If the target setup is the KOLLMORGEN motor on RED LEGO platform,
// you need to enable the macro USE_KOLLMORGEN_SETUP, otherwise
// it is implied that motor on wrist mk2 is in use

//#define EXTFAULT_enabled
#define EXTFAULT_handler_will_disable_motor

#define EXTFAULT_enabled_polling

#if defined(EXTFAULT_enabled)
#undef EXTFAULT_enabled_polling
#endif

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


namespace embot::app::board::amc2c {

struct Measure
{
    public:                      
        // interface
        virtual void start() = 0;
        virtual void stop() = 0;
        
    protected:
        virtual ~Measure() {};    // cannot delete from interface but only from derived object    
};

// this implementation does nothing
class MeasureDummy: public Measure
{
public:
           
    struct Config
    {  
        uint32_t nothing {0};
        Config() = default;
        bool isvalid() const { return true; }
    };
                
    MeasureDummy() {};
    virtual ~MeasureDummy() {};

    virtual void start() override {};
    virtual void stop() override {};        
}; 
   
// how to use it:
// 1. create a MeasureHisto w/ a proper MeasureHisto::Config 
// 2. call start() and stop() at the beginning and at the end of the piece of code
//    of which you want to measure the duration.
//    the duration will be added in a histogram whose properties are in 
//    Config::minduration, Config::maxduration, Config::resolution
// 3. the histogram can be viewed using method MeasureHisto::report(). 
//    you must place a breakpoint in it and explore the volatile variables {name, beyond, vals}
//    but you can also enable a print. you can use Config::name to differentiate amongst
//    multiple instances of MeasureHisto
// 4. method MeasureHisto::report() can be called at any time, but if Config::reportfrequency is true, 
//    is is also called at the end of stop() at the rate specified by Config::reportfrequency

struct MeasureHisto : public Measure
{    
    struct Config
    {
        const char *name {"dummy"}; 
        embot::core::relTime minduration {0};
        embot::core::relTime maxduration {100*embot::core::time1microsec};  
        embot::core::relTime resolution {100*embot::core::time1microsec};
        bool enablereport {true};
        uint32_t reportfrequency {10000};
        constexpr Config() = default;
        constexpr Config(const char *n, embot::core::relTime min, embot::core::relTime max,
                         embot::core::relTime res = embot::core::time1microsec,
                         bool er = true, uint32_t rf = 10000) 
                         : name(n), minduration(min), maxduration(max), resolution(res), 
                           enablereport(er), reportfrequency(rf){} 
    };
    
    Config config {};
        
    embot::app::scope::SignalHisto *sighisto {nullptr};
    
    const embot::tools::Histogram::Values * vv {nullptr};
    
    // i use ram ... maybe not best solution. maybe we could also print the histo.   
    static constexpr size_t nvals {512};
    volatile uint64_t vals[nvals] {0};        
    volatile uint64_t beyond {0}; 
    uint32_t counter {0};    

    MeasureHisto(const Config &cfg)
    {
        config = cfg;
        if(nullptr == config.name)
        {
            config.name = "dummy";
        }
        sighisto = new embot::app::scope::SignalHisto({config.minduration, config.maxduration, config.resolution});      
        vv = sighisto->histogram()->getvalues();        
    }
    
    void start()
    {     
        sighisto->on();
    }
            
    void stop()
    {
        sighisto->off();
        
        if(config.enablereport)
        {
            if(++counter >= config.reportfrequency)
            {
                counter = 0;
                report();  
            }  
        }        
    }
    
#define PRINT_REPORT 
    
    void report()
    {            
        beyond = vv->beyond;
                    
        for(int i=0; i<vv->inside.size(); i++)
        {
            if(i < nvals)
            {
                vals[i] = vv->inside[i];
            }
        }
        // place a breakpoint in here and inspect: beyond and vals[]
        beyond = beyond; 
        // but you can also use embot::core::print() to collect values from the printf window
        // the following code can be easily changed according to needs
#if defined(PRINT_REPORT)
        std::string str = std::string("name, beyond, histo = " ) + config.name + ", " + std::to_string(beyond);
        for(int v=0; v<nvals; v++)
        {
            str += ", ";
            str += std::to_string(vals[v]);
        }
        embot::core::print(str);  
#endif        
    }

};

} // end of namespace


constexpr bool useDUMMYforFOC {true};
constexpr bool useDUMMYforTICK {true};

struct embot::app::board::amc2c::theMBD::Impl
{
       
    Impl() = default;  
        
    // the initialization code
    bool initialise(const Config &config);
        
    // the code called @ 1 kHz (aka every 1000 usec) inside a high priority thread
    bool tick(const std::vector<embot::prot::can::Frame> &inpframes, std::vector<embot::prot::can::Frame> &outframes);

    // the code called @ 80/3 kHz (aka every 37.5 usec) inside the DMA1_Channel2_IRQHandler()
    // it must be static because we use it as a callback
    static void onCurrents_FOC_innerloop(void *owner, const embot::hw::motor::Currents * const currents);        

    // the objects which measure the duration of the two above functions
    Measure *measureFOC {nullptr};
    Measure *measureTick {nullptr}; 
    
    // all the rest, which may or may not be required anymore
    Config _config {};
    bool initted {false};
        
    embot::hw::motor::Pwm pwm {0};
    embot::prot::can::motor::polling::ControlMode cm {embot::prot::can::motor::polling::ControlMode::Idle};
    bool applychanges {false};
    
    embot::hw::BTN buttonEXTfault {embot::hw::BTN::one};
    embot::hw::LED ledEXTfault {embot::hw::LED::two};
    static void onEXTFAULTpressedreleased(void *owner);  
    volatile bool EXTFAULTisPRESSED {false};   
    volatile bool prevEXTFAULTisPRESSED {false};    
    volatile embot::core::Time EXTFAULTpressedtime {0};
    volatile embot::core::Time EXTFAULTreleasedtime {0};
    
    void onEXTFAULTpolling();
    std::vector<bool> faultvalues {};
    static constexpr size_t faultcapacity {5};
    
    #ifdef PRINT_HISTO_DEBUG 
    void printHistogram_rxPkt(size_t cur_size); //cur_size is the currente queue size
    static constexpr uint8_t numOfBins {11}; //size of queue of the CAN drive plus 1 (in case any packet is received)
    uint32_t bin[numOfBins]{0};
    #endif

    static constexpr embot::hw::TESTPOINT tp1 {embot::hw::TESTPOINT::one};
    static constexpr embot::hw::TESTPOINT tp2 {embot::hw::TESTPOINT::two};
};


      
bool embot::app::board::amc2c::theMBD::Impl::initialise(const Config &config)
{
    if(true == initted)
    {
        return true;
    }
    
    _config = config;
    
    embot::core::print("embot::app::board::amc2c::theMBD::Impl::initialise()");
    
    // create the measure for the foc
    if(true == useDUMMYforFOC)
    {
        measureFOC = new MeasureDummy;
    }
    else
    {
        measureFOC = new MeasureHisto({"foc", 0, 64*embot::core::time1microsec, 1, true, 100*1000});
    }
    
    // create the measure for the tick
    if(true == useDUMMYforTICK)
    {
        measureTick = new MeasureDummy;
    }
    else
    {
        measureTick = new MeasureHisto({"tick", 0, 400*embot::core::time1microsec, 1, true, 10*1000});
    }
    
    // init the external fault. 
    // we call cbkOnEXTfault.execute() when the button is pressed or released.
    // in the callback we set EXTFAULTisPRESSED true or false depending on value of
    // embot::hw::button::pressed(buttonEXTfault)
    // we also disable the motors if true. 

    buttonEXTfault = embot::hw::bsp::amc2c::EXTFAULTbutton();
    ledEXTfault = embot::hw::bsp::amc2c::EXTFAULTled();
    
#if defined(EXTFAULT_enabled)
    
    embot::core::Callback cbkOnEXTFAULT {onEXTFAULTpressedreleased, this};
    embot::hw::button::init(buttonEXTfault, {embot::hw::button::Mode::TriggeredOnPressAndRelease, cbkOnEXTFAULT, 0});
    prevEXTFAULTisPRESSED = EXTFAULTisPRESSED = embot::hw::button::pressed(buttonEXTfault);
    
#elif defined(EXTFAULT_enabled_polling)
    
    faultvalues.reserve(faultcapacity);
    faultvalues.clear();

    embot::hw::button::init(buttonEXTfault, {embot::hw::button::Mode::Polling, {}, 0});
    for(size_t i=0; i<faultcapacity; i++)
    {
        faultvalues.push_back(embot::hw::button::pressed(buttonEXTfault));
    }
    bool allpressed = std::all_of(faultvalues.cbegin(), faultvalues.cend(), [](auto v){ return v; });
    prevEXTFAULTisPRESSED = EXTFAULTisPRESSED = allpressed;    
    
#else
    
    prevEXTFAULTisPRESSED = EXTFAULTisPRESSED = false;  
    
#endif
    
    if(true == EXTFAULTisPRESSED)
    {
#if defined(EXTFAULT_handler_will_disable_motor)         
        embot::hw::motor::fault(embot::hw::MOTOR::one, true);
#endif        
        embot::app::theLEDmanager::getInstance().get(ledEXTfault).on();
    }
    else 
    {
        embot::hw::motor::fault(embot::hw::MOTOR::one, false);
        embot::app::theLEDmanager::getInstance().get(ledEXTfault).off();
    }
    
    // Init testpoint for debug
    embot::hw::testpoint::init(tp1);
    embot::hw::testpoint::init(tp2);
    // init MBD
    AMC_BLDC_initialize();
    
    AMC_BLDC_U.ExternalFlags_p.fault_button = EXTFAULTisPRESSED;
    
    // init motor
    embot::hw::motor::init(embot::hw::MOTOR::one, {});
    
    // assign the callback to the current availability
    embot::hw::motor::setCallbackOnCurrents(embot::hw::MOTOR::one, Impl::onCurrents_FOC_innerloop, this);
    
    CAN_ID_AMC = _config.adr;
        
    initted = true;
    return initted;
}

void embot::app::board::amc2c::theMBD::Impl::onEXTFAULTpolling()
{
    if(faultvalues.size() >= faultcapacity)
    {
        faultvalues.erase(faultvalues.begin());
    }
    bool pressed = embot::hw::button::pressed(buttonEXTfault);
    faultvalues.push_back(pressed);
    bool allpressed = std::all_of(faultvalues.cbegin(), faultvalues.cend(), [](auto v){ return v; });
    
    EXTFAULTisPRESSED = allpressed;       
    AMC_BLDC_U.ExternalFlags_p.fault_button = EXTFAULTisPRESSED;
    
    if(true == EXTFAULTisPRESSED)
    {
        EXTFAULTpressedtime = embot::core::now();
#if defined(EXTFAULT_handler_will_disable_motor) 
        embot::hw::motor::fault(embot::hw::MOTOR::one, true);
#endif         
    }
    else
    {
        embot::hw::motor::fault(embot::hw::MOTOR::one, false);
        EXTFAULTreleasedtime = embot::core::now();
    }
    
}

void embot::app::board::amc2c::theMBD::Impl::onEXTFAULTpressedreleased(void *owner)
{
    Impl * impl = reinterpret_cast<Impl*>(owner);
    if(nullptr == impl)
    {
        return;
    }
    
    impl->EXTFAULTisPRESSED = embot::hw::button::pressed(impl->buttonEXTfault);
    AMC_BLDC_U.ExternalFlags_p.fault_button = impl->EXTFAULTisPRESSED;
    
    if(true == impl->EXTFAULTisPRESSED)
    {
        impl->EXTFAULTpressedtime = embot::core::now();
#if defined(EXTFAULT_handler_will_disable_motor) 
        embot::hw::motor::fault(embot::hw::MOTOR::one, true);
#endif         
    }
    else
    {
        embot::hw::motor::fault(embot::hw::MOTOR::one, false);
        impl->EXTFAULTreleasedtime = embot::core::now();
    }
}

// Called every 1 ms
bool embot::app::board::amc2c::theMBD::Impl::tick(const std::vector<embot::prot::can::Frame> &inpframes, std::vector<embot::prot::can::Frame> &outframes)
{ 

    // in here... 
    // inpframes.size() is always <= 4.
    // outputframes.size() is always = 0
    //embot::hw::testpoint::on(tp1);

    measureTick->start();
    
    onEXTFAULTpolling();
    
    if(prevEXTFAULTisPRESSED != EXTFAULTisPRESSED)
    {
        prevEXTFAULTisPRESSED = EXTFAULTisPRESSED;  
        // and manage the transitions [pressed -> unpressed] or vice-versa and use also
        // EXTFAULTpressedtime and / or EXTFAULTreleasedtime and

        AMC_BLDC_U.ExternalFlags_p.fault_button = EXTFAULTisPRESSED;        
        
        if(true == EXTFAULTisPRESSED)
        {
            embot::app::theLEDmanager::getInstance().get(ledEXTfault).on();
        }
        else
        {
            embot::app::theLEDmanager::getInstance().get(ledEXTfault).off();
        }
    }
        
    // add any input can frame into the supervisor input queue
    
    size_t ninputframes = std::min(inpframes.size(), static_cast<size_t>(CAN_MAX_NUM_PACKETS));
    
    for(uint8_t i=0; i<CAN_MAX_NUM_PACKETS; i++) 
    {
        AMC_BLDC_U.PacketsRx.packets[i].available = false;
    }
    
    for(uint8_t i=0; i<ninputframes; i++) 
    {        
        uint32_t rx_id {0};
        inpframes[i].copyto(rx_id, AMC_BLDC_U.PacketsRx.packets[i].length, AMC_BLDC_U.PacketsRx.packets[i].packet.PAYLOAD); 
        AMC_BLDC_U.PacketsRx.packets[i].packet.ID = (uint16_T)rx_id;
        AMC_BLDC_U.PacketsRx.packets[i].available = true;
    }
    
    // read Vcc
    AMC_BLDC_U.SensorsData_p.supplyvoltagesensors.voltage = 44; // TODO: read the voltage from ADC (temporary hardcoded)

    
    // -----------------------------------------------------------------------------
    // Model Step Function (1 ms)
    // -----------------------------------------------------------------------------
    
    AMC_BLDC_step_Time_1ms();
    

    // -----------------------------------------------------------------------------
    // Thermal Model Step Function (10 ms)
    // -----------------------------------------------------------------------------
    
#ifdef ENABLE_THERMAL_MODEL // (Temporary disabled)
    static uint8_t thermal_model_counter = 0;

    if(thermal_model_counter % 10 == 0)
    {
        AMC_BLDC_step_Time_10ms();
        thermal_model_counter = 0;
    }
    thermal_model_counter++;
#endif
    
    
    // get any out can frame
    
    for(uint8_t i=0; i<CAN_MAX_NUM_PACKETS; i++)
    {
        if(true == AMC_BLDC_Y.PacketsTx.packets[i].available)
        {
            embot::prot::can::Frame fr {AMC_BLDC_Y.PacketsTx.packets[i].packet.ID, AMC_BLDC_Y.PacketsTx.packets[i].length, AMC_BLDC_Y.PacketsTx.packets[i].packet.PAYLOAD};
            outframes.push_back(fr);
        }
    } 
    
    
    measureTick->stop();
    
    //embot::hw::testpoint::off(tp1);
    return true;
}

#if defined(TEST_FOC_logvalues)

struct dbgFOCvalues
{
    embot::hw::motor::Currents currents {0, 0, 0};
    uint8_t hall {0};    
    embot::hw::motor::Position electricalangle {0};
    int32_t position {0};
    float jointangle {0.0};
    std::array<int32_t, 3> pwms {0, 0, 0};   
    
    dbgFOCvalues() = default;    
};

dbgFOCvalues dbgFOC {};

#endif

void embot::app::board::amc2c::theMBD::Impl::onCurrents_FOC_innerloop(void *owner, const embot::hw::motor::Currents * const currents)
{
    Impl * impl = reinterpret_cast<Impl*>(owner);
    if((nullptr == impl) || (nullptr == currents))
    {
        return;
    }    
        
//    impl->measureFOC->start();
    
    // 1. copy currents straight away, so that we can use them
    embot::hw::motor::Currents currs = *currents;
   
    
    // remember to manage impl->EXTFAULTisPRESSED ............

    // retrieve the current value of the Hall sensors
    uint8_t hall {0};    
    embot::hw::motor::gethallstatus(embot::hw::MOTOR::one, hall);
    AMC_BLDC_U.SensorsData_p.motorsensors.hallABC = hall;
    
    // retrieve the current value of the encoder
    embot::hw::motor::Position electricalAngle {0};
    embot::hw::motor::getencoder(embot::hw::MOTOR::one, electricalAngle); 
    

    static int32_T position {0};
    static int32_T electricalAngleOld = static_cast<int32_T>(electricalAngle);
    // keep int16_t ... to manage overflow ... 
    int16_t delta = electricalAngle - electricalAngleOld;
    electricalAngleOld = electricalAngle;
    
    // calculate the current joint position
    uint8_t polepairs = (0 != AMC_BLDC_Y.ConfigurationParameters_p.motorconfig.pole_pairs) ? AMC_BLDC_Y.ConfigurationParameters_p.motorconfig.pole_pairs : 1;
    position = position + delta / polepairs;
    
    AMC_BLDC_U.SensorsData_p.motorsensors.angle = static_cast<real32_T>(electricalAngle)*0.0054931640625f; // (60 interval angle)
    
    // convert the current from mA to A
    AMC_BLDC_U.SensorsData_p.motorsensors.Iabc[0] = 0.001f*currs.u;
    AMC_BLDC_U.SensorsData_p.motorsensors.Iabc[1] = 0.001f*currs.v;
    AMC_BLDC_U.SensorsData_p.motorsensors.Iabc[2] = 0.001f*currs.w;
    
    // -----------------------------------------------------------------------------
    // FOC Step Function (~16.6 KHz)
    // -----------------------------------------------------------------------------
    
    AMC_BLDC_step_FOC();
    
    // -----------------------------------------------------------------------------
    
    AMC_BLDC_U.SensorsData_p.jointpositions.position = static_cast<real32_T>(position) * 0.0054931640625f; // iCubDegree -> deg
    
    embot::hw::motor::PWMperc pwmperc 
    {
        AMC_BLDC_Y.ControlOutputs_p.Vabc[0], AMC_BLDC_Y.ControlOutputs_p.Vabc[1], AMC_BLDC_Y.ControlOutputs_p.Vabc[2]
    };
    embot::hw::motor::setPWM(embot::hw::MOTOR::one, pwmperc);



//    impl->measureFOC->stop();
}

#ifdef PRINT_HISTO_DEBUG 
void embot::app::board::amc2c::theMBD::Impl::printHistogram_rxPkt(size_t cur_size)
{
    if(cur_size<numOfBins)
    {
        bin[cur_size] ++;
    }
    else
    {
        //error;Currently this situation is not possible
    }
        
    static uint32_t cnt = 0;
    if((++cnt % 500) == 1)
    {
        embot::core::print(std::string("bin[0] = ") + std::to_string(bin[0]) + ", bin[1] = " + std::to_string(bin[1]) + 
                            std::string(", bin[2] = ") + std::to_string(bin[2]) + ", bin[3] = " + std::to_string(bin[3]) +
                            std::string(", bin[4] = ") + std::to_string(bin[4]) + ", bin[5] = " + std::to_string(bin[5]) +
                            std::string(", bin[6] = ") + std::to_string(bin[6]) + ", bin[7] = " + std::to_string(bin[7]) +
                            std::string(", bin[8] = ") + std::to_string(bin[8]) + ", bin[9] = " + std::to_string(bin[9]) +
                            std::string(", bin[10] = ") + std::to_string(bin[10]) +
                            " @ " + embot::core::TimeFormatter(embot::core::now()).to_string());

        for(uint8_t j=0; j<numOfBins; j++)
        {
            bin[j]=0;
        }
    }
}
#endif

// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::board::amc2c::theMBD& embot::app::board::amc2c::theMBD::getInstance()
{
    static theMBD* p = new theMBD();
    return *p;
}

embot::app::board::amc2c::theMBD::theMBD()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::board::amc2c::theMBD::~theMBD() { }
        

bool embot::app::board::amc2c::theMBD::initialise(const Config &config)
{
    return pImpl->initialise(config);
}

bool embot::app::board::amc2c::theMBD::tick(const std::vector<embot::prot::can::Frame> &inpframes, std::vector<embot::prot::can::Frame> &outframes)
{
    return pImpl->tick(inpframes, outframes);
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


