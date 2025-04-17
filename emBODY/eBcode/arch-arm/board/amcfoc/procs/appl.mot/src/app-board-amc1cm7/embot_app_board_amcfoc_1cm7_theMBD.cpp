
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_board_amcfoc_1cm7_theMBD.h"

int32_t ImposedMechAngle = 0;
int32_t readMechAngle = 0;
int32_t readElecAngle = 0;
uint32_t readSector = 0;
uint8_t readHALL = 0;

int32_t pwm1 =0;
int32_t pwm2 =0;
int32_t pwm3 =0;

int32_t CU1 = 0;
int32_t CU2 = 0;
int32_t CU3 = 0;

// --------------------------------------------------------------------------------------------------------------------
// - defines
// --------------------------------------------------------------------------------------------------------------------


// -
// - debug section
//

//#define DEBUG_IMPOSE_MECH_ANGLE  
//#define DEBUG_LOG_ANGLE_VARIATIONS
//#define DEBUG_LOG_ANGLE_VARIATIONS__PRINT_CHANGES
//#define DEBUG_PRINT_PWM_VARIATIONS
//#define DEBUG_PRINT_CURRENTS_PWM

//#define DEBUG_PWM_min_0perc

// -
// - MBD code section
// -

// it keeps or removes the MBD code
#define theMBDmotor_MBD_code_removed


// -
// - EXT FAULT section
// -

// it enables / disables the external fault.
// if enabled, it works as defined by other macros theMBDmotor_EXTFAULT_*
// if not enabled ... we just dont manage any external fault button or led

#define theMBDmotor_EXTFAULT_enabled

// if defined the pressed fault will disable the motors
// if undefined it will not. this one is useful when we dont have a fault and we want to run motors anyway
#define theMBDmotor_EXTFAULT_when_pressed_will_disable_motors

// if defined the external fault will be checked with polling of the pin and averaging of the value. 
// if not defined, the check will use an interrupt on variation of the value
#define theMBDmotor_EXTFAULT_checked_with_polling

#if defined(theMBDmotor_EXTFAULT_checked_with_polling)
    #undef theMBDmotor_EXTFAULT_checked_with_exti
#else
    #define theMBDmotor_EXTFAULT_checked_with_exti
    #warning are you sure you want to use exti? it gave problems in the past on amcbldc and we have not ever used it on teh robot
#endif


// -
// - MEASURES section
// -

// it enables or disables the measures. 
// NOTE: any measure introduces overhead, so better remove them if not necessary
#undef theMBDmotor_MEASURES_enabled

// it tells which measures (when enabled) will be taken
#if defined(theMBDmotor_MEASURES_enabled)
    #define theMBDmotor_MEASURES_enable_testpoint
    #define theMBDmotor_MEASURES_enable_duration_tick
    #define theMBDmotor_MEASURES_enable_duration_foc
#endif


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#if defined(STM32HAL_BOARD_AMCFOC_1CM7)
#include "embot_hw_bsp_amcfoc_1cm7.h"
#include "embot_app_bldc_theMC2agent.h"
#elif defined(STM32HAL_BOARD_AMC1CM7)
#include "embot_hw_bsp_amc1cm7.h"
//#warning READ CAREFULLY: we may use the same object for both amcbldc and amc1cm7
#elif defined(STM32HAL_BOARD_AMCBLDC)
#include "embot_hw_bsp_amcbldc.h"
#else
#error: choose a STM32HAL_BOARD_*
#endif

#include "embot_hw_button.h"
#include "embot_hw_motor_bldc.h"
#include "embot_app_scope.h"
#include "embot_hw_sys.h"
#include "embot_core.h"
#include "embot_app_theLEDmanager.h"
#include "embot_hw_testpoint.h"
#include <array>

// some protocol includes
#include "embot_prot_can_motor_polling.h"
#include "embot_prot_can_motor_periodic.h"

#include "embot_app_application_CANagentMC2.h"


#if defined(theMBDmotor_MBD_code_removed)
#include "embot_app_bldc_MBD_interface.h"
#else
#include "embot_app_bldc_MBD_interface.h"
//// mdb components
//#include "AMC_BLDC.h"
#endif

#include "embot_hw_motor_hall.h"
#include "embot_hw_analog.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

#if defined(theMBDmotor_MEASURES_enabled)
#if defined(theMBDmotor_MEASURES_enable_duration_tick) || defined(theMBDmotor_MEASURES_enable_duration_foc)

#warning this is very generic code that could be moved into a dedicated file

namespace embot::app::utils {

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
    
//#define PRINT_REPORT 
    
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


#endif // #if defined(theMBDmotor_MEASURES_enable_duration_tick) || defined(theMBDmotor_MEASURES_enable_duration_foc)
#endif // theMBDmotor_MEASURES_enabled


#if defined(theMBDmotor_MEASURES_enable_duration_tick)
constexpr bool useDUMMYforTICK {true};
#endif

#if defined(theMBDmotor_MEASURES_enable_duration_foc)
constexpr bool useDUMMYforFOC {true};
#endif

struct embot::app::board::amcfoc::cm7::theMBD::Impl
{
       
    Impl() = default;  
        
    // the initialization code
    bool initialise(const Config &config);
        
    // the code called @ 1 kHz (aka every 1000 usec) inside a high priority thread
    bool tick(const std::vector<embot::app::bldc::MSG> &inputmessages, std::vector<embot::app::bldc::MSG> &outputmessages);

    // called by the IRQ handler of end of currents acquisition. 
    // if pwm is 66.6 kHz it is called twice (once per motor) every 45 usec
    static void onCurrents(embot::hw::MOTOR m, const embot::hw::motor::bldc::Currents * const currs, void *owner); 
    
    // executed once every 45 usec and called inside (or triggered by) the onCurrents() callback when all the motors have current measures
    void FOC(embot::hw::MOTOR m);  

    // it acquires the positions
    void updatePosition(embot::hw::MOTOR m);
    
    // it loads the currents inside the Impl::_item
    void loadCurrents(embot::hw::MOTOR m, const embot::hw::motor::bldc::Currents * const currs, bool &allmotorsok);


    // all the rest
    
    Config _config {};
    bool initted {false};
   
    
    struct Item
    {
//        Config config {}; // what about it? it contains the adr
//        bool initted {false}; // what about it?
        embot::hw::motor::bldc::PWM3 pwm {};
        embot::prot::can::motor::ControlMode ctrlmode {embot::prot::can::motor::ControlMode::Idle};
        embot::hw::motor::bldc::Currents currents {}; 
        embot::hw::motor::bldc::HallStatus hallstatus {}; 
        embot::hw::motor::bldc::Angle electricalangle {}; 
        embot::hw::motor::bldc::Angle position {};  
        bool currOK {false};            
    }; 

    std::array<Item, embot::hw::motor::bldc::MAXnumber> _items {};    


#if defined(theMBDmotor_EXTFAULT_enabled)  
        
//    // they may stay as one. better to refactor however. but later.
//    struct Fault
//    {   
//        static constexpr embot::hw::BTN buttonEXTfault {embot::hw::BTN::one};
//        static constexpr embot::hw::LED ledEXTfault {embot::hw::LED::two};
//        
//        volatile bool EXTFAULTisPRESSED {false};   
//        volatile bool prevEXTFAULTisPRESSED {false};    
//        volatile embot::core::Time EXTFAULTpressedtime {0};
//        volatile embot::core::Time EXTFAULTreleasedtime {0}; 
//#if defined(theMBDmotor_EXTFAULT_checked_with_polling)    
//        std::vector<bool> faultvalues {};
//        static constexpr size_t faultcapacity {5};
//#endif  
//            
//        Fault() = default;   

//#if defined(theMBDmotor_EXTFAULT_checked_with_exti)    
//        static void onEXTFAULTpressedreleased(void *owner);  
//#endif
//    

//#if defined(theMBDmotor_EXTFAULT_checked_with_polling)    
//        void onEXTFAULTpolling();
//#endif 
//        
//    };
//    
//    Fault _fault {};
        
    embot::hw::BTN buttonEXTfault {embot::hw::BTN::one};
    embot::hw::LED ledEXTfault {embot::hw::LED::two};
    
    volatile bool EXTFAULTisPRESSED {false};   
    volatile bool prevEXTFAULTisPRESSED {false};    
    volatile embot::core::Time EXTFAULTpressedtime {0};
    volatile embot::core::Time EXTFAULTreleasedtime {0};
    
#if defined(theMBDmotor_EXTFAULT_checked_with_exti)    
    static void onEXTFAULTpressedreleased(void *owner);  
#endif
    

#if defined(theMBDmotor_EXTFAULT_checked_with_polling)    
    void onEXTFAULTpolling();
    std::vector<bool> faultvalues {};
    static constexpr size_t faultcapacity {5};
#endif    
    
#endif
   
    
    
    embot::app::msg::BUS bus2use {embot::app::msg::BUS::none};
    
    // do we need to use a vector<embot::app::bldc::Rounder> NO! 
    embot::app::bldc::Rounder _rounder {};
        

#if defined(theMBDmotor_MEASURES_enabled)
        
    // the objects which measure the duration of the two above functions
#if defined(theMBDmotor_MEASURES_enable_duration_foc)
    Measure *measureFOC {nullptr};
#endif

#if defined(theMBDmotor_MEASURES_enable_duration_tick)
    Measure *measureTick {nullptr}; 
#endif


#if defined(theMBDmotor_MEASURES_enable_testpoint)
    // shall i keep the testpeoints ??? maybe 
    static constexpr embot::hw::TESTPOINT tp1 {embot::hw::TESTPOINT::one};
    static constexpr embot::hw::TESTPOINT tp2 {embot::hw::TESTPOINT::two};
#endif        

#endif // #if defined(theMBDmotor_MEASURES_enabled)


//    embot::app::bldc::mbd::interface::IO2 io2 {&iterative_motion_controller_U, &iterative_motion_controller_Y};    
    
    std::vector<embot::prot::can::Frame> caninputframes {};
    std::vector<embot::prot::can::Frame> canoutputframes {};
        

#if defined(DEBUG_IMPOSE_MECH_ANGLE)          
    embot::hw::motor::hall::bsp::debug::Wave debugwave1 {};
#endif
        
};


      
bool embot::app::board::amcfoc::cm7::theMBD::Impl::initialise(const Config &config)
{
    if(true == initted)
    {
        return true;
    }
    
    _config = config;
    
    embot::core::print("embot::app::board::amcfoc::cm7::theMBD::Impl::initialise()");
    
    
    // init motors: because if we delay that .... the embot::hw::motor::bldc::fault() will not work
    embot::hw::motor::bldc::init(embot::hw::MOTOR::one, {});
    embot::hw::motor::bldc::init(embot::hw::MOTOR::two, {});

#if defined(theMBDmotor_MEASURES_enable_duration_foc)    
    // create the measure for the foc
    if(true == useDUMMYforFOC)
    {
        measureFOC = new MeasureDummy;
    }
    else
    {
        measureFOC = new MeasureHisto({"foc", 0, 64*embot::core::time1microsec, 1, true, 100*1000});
    }
#endif

    
#if defined(theMBDmotor_MEASURES_enable_duration_tick)
    // create the measure for the tick
    if(true == useDUMMYforTICK)
    {
        measureTick = new MeasureDummy;
    }
    else
    {
        measureTick = new MeasureHisto({"tick", 0, 400*embot::core::time1microsec, 1, true, 10*1000});
    }    
#endif  


#if defined(theMBDmotor_EXTFAULT_enabled)
    
    // init the external fault. 
    // we call cbkOnEXTfault.execute() when the button is pressed or released.
    // in the callback we set EXTFAULTisPRESSED true or false depending on value of
    // embot::hw::button::pressed(buttonEXTfault)
    // we also disable the motors if true. 

    buttonEXTfault = embot::hw::bsp::amcfoc::cm7::EXTFAULTbutton();
    ledEXTfault = embot::hw::bsp::amcfoc::cm7::EXTFAULTled();
    
#if defined(theMBDmotor_EXTFAULT_checked_with_exti)
    
    embot::core::Callback cbkOnEXTFAULT {onEXTFAULTpressedreleased, this};
    embot::hw::button::init(buttonEXTfault, {embot::hw::button::Mode::TriggeredOnPressAndRelease, cbkOnEXTFAULT, 0});
    prevEXTFAULTisPRESSED = EXTFAULTisPRESSED = embot::hw::button::pressed(buttonEXTfault);
    
#elif defined(theMBDmotor_EXTFAULT_checked_with_polling)
    
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
#if defined(theMBDmotor_EXTFAULT_when_pressed_will_disable_motors)         
        embot::hw::motor::bldc::fault(embot::hw::MOTOR::one, true);
        embot::hw::motor::bldc::fault(embot::hw::MOTOR::two, true);
#endif        
        embot::app::theLEDmanager::getInstance().get(ledEXTfault).on();
    }
    else 
    {
        embot::hw::motor::bldc::fault(embot::hw::MOTOR::one, false);
        embot::hw::motor::bldc::fault(embot::hw::MOTOR::two, false);
        embot::app::theLEDmanager::getInstance().get(ledEXTfault).off();
    }
    
#endif // #if defined(theMBDmotor_EXTFAULT_enabled)

    
#if defined(theMBDmotor_MEASURES_enable_testpoint)
    embot::hw::testpoint::init(tp1);
    embot::hw::testpoint::init(tp2);
#endif
    
    
    // ..............................
    
    std::array<uint8_t, embot::app::bldc::theMC2agent::numberofmotors> adrs {static_cast<uint8_t>(config.adr), static_cast<uint8_t>(config.adr+1)};
    
    embot::app::bldc::theMC2agent::getInstance().initialise({adrs, {}});
    
#if defined(theMBDmotor_MBD_code_removed)
    
    // just init it
    embot::app::bldc::mbd::interface::init();
    embot::app::bldc::mbd::interface::getIO2handle().set_fault(EXTFAULTisPRESSED);
    
#else  

    // just init it
    embot::app::bldc::mbd::interface::init();
    embot::app::bldc::mbd::interface::getIO2handle().set_fault(EXTFAULTisPRESSED);        
        
//    #erorr this is the old code    
//    // init MBD
//    AMC_BLDC_initialize();
//    
//    AMC_BLDC_U.ExternalFlags_p.fault_button = EXTFAULTisPRESSED;
        
#endif
    
//    // init motors
//    embot::hw::motor::bldc::init(embot::hw::MOTOR::one, {});
//    embot::hw::motor::bldc::init(embot::hw::MOTOR::two, {});


#if defined(DEBUG_IMPOSE_MECH_ANGLE)  
    debugwave1.configure({});
#endif

    // assign the callback to the current availability         
    embot::hw::motor::bldc::set(embot::hw::MOTOR::one, {embot::hw::MOTOR::one, Impl::onCurrents, this});
    embot::hw::motor::bldc::set(embot::hw::MOTOR::two, {embot::hw::MOTOR::two, Impl::onCurrents, this});    
        
        
#if defined(theMBDmotor_MBD_code_removed)
    // nothing to set
#else    
    CAN_ID_AMC = _config.adr;
#endif        
        
    
    // init the rounder used to get the Vcc
    _rounder.init(0.0f, 1); // default initial value=0, decimals=1
    
    
#if defined(DEBUG_PWM_min_0perc)
    
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();        
    theleds.get(embot::hw::LED::one).pulse(2*embot::core::time1second);
    
#else

//    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();        
//    theleds.get(embot::hw::LED::one).pulse(4*embot::core::time1second);

#endif
        
    initted = true;
    return initted;
}

#if defined(theMBDmotor_EXTFAULT_enabled)

#if defined(theMBDmotor_EXTFAULT_checked_with_polling)  

void embot::app::board::amcfoc::cm7::theMBD::Impl::onEXTFAULTpolling()
{
    if(faultvalues.size() >= faultcapacity)
    {
        faultvalues.erase(faultvalues.begin());
    }
    bool pressed = embot::hw::button::pressed(buttonEXTfault);
    faultvalues.push_back(pressed);
    bool allpressed = std::all_of(faultvalues.cbegin(), faultvalues.cend(), [](auto v){ return v; });
    
    EXTFAULTisPRESSED = allpressed; 
#if defined(theMBDmotor_MBD_code_removed)
    embot::app::bldc::mbd::interface::getIO2handle().set_fault(EXTFAULTisPRESSED);
#else 
    embot::app::bldc::mbd::interface::getIO2handle().set_fault(EXTFAULTisPRESSED);    
//    AMC_BLDC_U.ExternalFlags_p.fault_button = EXTFAULTisPRESSED;
#endif
    
    if(true == EXTFAULTisPRESSED)
    {
        EXTFAULTpressedtime = embot::core::now();
#if defined(theMBDmotor_EXTFAULT_when_pressed_will_disable_motors) 
        embot::hw::motor::bldc::fault(embot::hw::MOTOR::one, true);
        embot::hw::motor::bldc::fault(embot::hw::MOTOR::two, true);
#endif         
    }
    else
    {
        embot::hw::motor::bldc::fault(embot::hw::MOTOR::one, false);
        embot::hw::motor::bldc::fault(embot::hw::MOTOR::two, false);
        EXTFAULTreleasedtime = embot::core::now();
    }
    
}

#endif // #if defined(theMBDmotor_EXTFAULT_checked_with_polling)  

#if defined(theMBDmotor_EXTFAULT_checked_with_exti)  
void embot::app::board::amcfoc::cm7::theMBD::Impl::onEXTFAULTpressedreleased(void *owner)
{
    Impl * impl = reinterpret_cast<Impl*>(owner);
    if(nullptr == impl)
    {
        return;
    }
    
    impl->EXTFAULTisPRESSED = embot::hw::button::pressed(impl->buttonEXTfault);
#if defined(theMBDmotor_MBD_code_removed)
    embot::app::bldc::mbd::interface::getIO2handle().set_fault(EXTFAULTisPRESSED);
#else
    embot::app::bldc::mbd::interface::getIO2handle().set_fault(EXTFAULTisPRESSED);
//    AMC_BLDC_U.ExternalFlags_p.fault_button = impl->EXTFAULTisPRESSED;
#endif
    
    if(true == impl->EXTFAULTisPRESSED)
    {
        impl->EXTFAULTpressedtime = embot::core::now();
#if defined(theMBDmotor_EXTFAULT_when_pressed_will_disable_motors) 
        embot::hw::motor::bldc::fault(embot::hw::MOTOR::one, true);
        embot::hw::motor::bldc::fault(embot::hw::MOTOR::two, true);
#endif         
    }
    else
    {
        embot::hw::motor::bldc::fault(embot::hw::MOTOR::one, false);
        embot::hw::motor::bldc::fault(embot::hw::MOTOR::two, false);
        impl->EXTFAULTreleasedtime = embot::core::now();
    }
}
#endif // #if defined(theMBDmotor_EXTFAULT_checked_with_exti)  

#endif // #if defined(theMBDmotor_EXTFAULT_enabled)

#include "embot_hw_motor_hall.h"

// Called every 1 ms
bool embot::app::board::amcfoc::cm7::theMBD::Impl::tick(const std::vector<embot::app::bldc::MSG> &inputmessages, std::vector<embot::app::bldc::MSG> &outputmessages)
{ 

#if defined(DEBUG_IMPOSE_MECH_ANGLE)  
    
    debugwave1.tick();

    float a = debugwave1.get();
    ImposedMechAngle = static_cast<int32_t>(a);
    embot::hw::motor::hall::bsp::debug::set(embot::hw::MOTOR::one, a, embot::hw::motor::bldc::AngleType::hall_mechanical);
    
#endif // #if defined(DEBUG_IMPOSE_MECH_ANGLE)  


#if defined(DEBUG_LOG_ANGLE_VARIATIONS)  
    
    static uint32_t readMechAngle_prev = 0;
    static uint32_t readElecAngle_prev = 0;
    static uint32_t readSector_prev = 0;
    static uint32_t readHALL_prev = 0;
    
    static float me_p = 0;
    static float el_p = 0;
    static float me = 0;
    static float el = 0;
    
    me_p = me;
    el_p = el;
    
    #warning debug mech angle
    float tmp =0;
    tmp = embot::hw::motor::bldc::angle(embot::hw::MOTOR::one, embot::hw::motor::bldc::AngleType::hall_mechanical); // in deg
    me = tmp;
    readMechAngle = std::floor(tmp); // in  deg
    tmp = embot::hw::motor::bldc::angle(embot::hw::MOTOR::one, embot::hw::motor::bldc::AngleType::hall_electrical); // in deg
    el = tmp;
    readElecAngle = std::floor(tmp); // in 0.1 deg
    readSector = embot::hw::motor::hall::sector(embot::hw::MOTOR::one);
    readHALL = embot::hw::motor::bldc::hall(embot::hw::MOTOR::one);
    
//    if(readSector_prev != readMechAngle_prev)
    if(readSector_prev != readSector)
    {

#if defined(DEBUG_LOG_ANGLE_VARIATIONS__PRINT_CHANGES)  
        //  print them
      
        embot::core::TimeFormatter tf {embot::core::now()};

        embot::core::print(tf.to_string() + ": mech = " + std::to_string(me_p) + " -> " + std::to_string(me) +  "; " + 
                                      "elec = " + std::to_string(el_p) + " -> " + std::to_string(el) +  "; " +
                                      "sect = " + std::to_string(readSector_prev) + " -> " + std::to_string(readSector) +  "; " +
                                      "hall = " + std::to_string(readHALL_prev) + " -> " + std::to_string(readHALL) +  "; "

        
//        embot::core::print(tf.to_string() + ": mech = " + std::to_string(readMechAngle_prev) + " -> " + std::to_string(readMechAngle) +  "; " + 
//                                              "elec = " + std::to_string(readElecAngle_prev) + " -> " + std::to_string(readElecAngle) +  "; " +
//                                              "sect = " + std::to_string(readSector_prev) + " -> " + std::to_string(readSector) +  "; " +
//                                              "hall = " + std::to_string(readHALL_prev) + " -> " + std::to_string(readHALL) +  "; "
        
        );
#endif
        // and now change them
        
        readMechAngle_prev = readMechAngle;
        readElecAngle_prev = readElecAngle;
        readSector_prev = readSector;
        readHALL_prev = readHALL;        
    }
    
#endif // #if defined(DEBUG_LOG_ANGLE_VARIATIONS)    
    

//#if defined PRINT_1MS    

//    static embot::hw::motor::bldc::Angle aa[2] = {0, 0};
//    
//    embot::hw::motor::bldc::Angle a0 = embot::hw::motor::bldc::angle(embot::hw::MOTOR::one, embot::hw::motor::bldc::AngleType::hall_mechanical);
////    embot::hw::motor::bldc::Angle a1 = embot::hw::motor::bldc::angle(embot::hw::MOTOR::two, embot::hw::motor::bldc::AngleType::hall_mechanical);
//    
//    if(aa[0] != a0)
//    {
//        embot::core::TimeFormatter tf {embot::core::now()};
//        embot::core::print(tf.to_string() + ": mech angle MOTOR::one = " + std::to_string(a0) + " (was: " +  std::to_string(aa[0]));
//        aa[0] = a0;
//    }

////    if(aa[1] != a1)
////    {
////        embot::core::TimeFormatter tf {embot::core::now()};
////        embot::core::print(tf.to_string() + ": mech angle MOTOR::two = " + std::to_string(a1) + " (was: " +  std::to_string(aa[1]));
////        aa[1] = a1;
////    }  
//    
//    
//    static embot::hw::motor::bldc::Angle bb[2] = {0, 0};
//    
//    embot::hw::motor::bldc::Angle b0 = embot::hw::motor::bldc::angle(embot::hw::MOTOR::one, embot::hw::motor::bldc::AngleType::hall_electrical);
////    embot::hw::motor::bldc::Angle b1 = embot::hw::motor::bldc::angle(embot::hw::MOTOR::two, embot::hw::motor::bldc::AngleType::hall_electrical);
//    
//    if(bb[0] != b0)
//    {
//        embot::core::TimeFormatter tf {embot::core::now()};
//        uint8_t sector = embot::hw::motor::hall::sector(embot::hw::MOTOR::one);
//        embot::core::print(tf.to_string() + ": elec angle MOTOR::one = " + std::to_string(b0) + " (was: " +  std::to_string(bb[0]) + 
//        "), sector =  " + std::to_string(sector));
//        bb[0] = b0;
//    }

////    if(bb[1] != b1)
////    {
////        embot::core::TimeFormatter tf {embot::core::now()};
////        embot::core::print(tf.to_string() + ": elc angle MOTOR::two = " + std::to_string(b1) + " (was: " +  std::to_string(bb[1]));
////        bb[1] = b1;
////    }      

////    static uint8_t halls[2] = {0, 0};
////    
////    uint8_t hallsnow[2] = {0, 0};
////    hallsnow[0] = embot::hw::motor::hall::bsp::triple(embot::hw::MOTOR::one);
////    hallsnow[1] = embot::hw::motor::hall::bsp::triple(embot::hw::MOTOR::two);
////    
////    if(hallsnow[0] != halls[0])
////    {
////        embot::core::print("hall value for " + embot::hw::motor::bldc::to_string(embot::hw::MOTOR::one) + " = " + std::to_string(hallsnow[0]));
////        halls[0] = hallsnow[0];
////    }        

////    if(hallsnow[1] != halls[1])
////    {
////        embot::core::print("hall value for " + embot::hw::motor::bldc::to_string(embot::hw::MOTOR::two) + " = " + std::to_string(hallsnow[1]));
////        halls[1] = hallsnow[1];
////    } 
//#endif

    
#if defined(COMM_BUS_LOCKED_AT_FIRST_MC_RECEPTION)
    // the first time we receive a message we use its bus forever    
    static bool lockedtobus {false};

    if((false == lockedtobus) && (false == inputmessages.empty()))
    {
        lockedtobus = true;
        bus2use = inputmessages[0].location.getbus();
    }
#else
    // we allow the bus to change in the lifetime of the board. 
    // we reply to the bus of the first input message    
    if(false == inputmessages.empty())
    {
        bus2use = inputmessages[0].location.getbus();
    }
#endif    

    // in here... 
    // inputmessages.size() is always <= 4.
    // outputframes.size() is always = 0
    
#if defined(theMBDmotor_MEASURES_enable_testpoint)    
    embot::hw::testpoint::on(tp1);
#endif
    
#if defined(theMBDmotor_MEASURES_enable_duration_tick)
    measureTick->start();
#endif

#if defined(theMBDmotor_EXTFAULT_enabled)
#if defined(theMBDmotor_EXTFAULT_checked_with_polling)  
    
    onEXTFAULTpolling();
    
    #warning attenzione.... per pulizia meglio mettere il codice che segue dentro onEXTFAULTpolling() 
    
    if(prevEXTFAULTisPRESSED != EXTFAULTisPRESSED)
    {
        prevEXTFAULTisPRESSED = EXTFAULTisPRESSED;  
        // and manage the transitions [pressed -> unpressed] or vice-versa and use also
        // EXTFAULTpressedtime and / or EXTFAULTreleasedtime and
#if defined(theMBDmotor_MBD_code_removed)        
        embot::app::bldc::mbd::interface::getIO2handle().set_fault(EXTFAULTisPRESSED);
#else
        embot::app::bldc::mbd::interface::getIO2handle().set_fault(EXTFAULTisPRESSED);
//        AMC_BLDC_U.ExternalFlags_p.fault_button = EXTFAULTisPRESSED;        
#endif
        
        if(true == EXTFAULTisPRESSED)
        {
            embot::app::theLEDmanager::getInstance().get(ledEXTfault).on();
        }
        else
        {
            embot::app::theLEDmanager::getInstance().get(ledEXTfault).off();
        }
    }
    
#endif // #if defined(theMBDmotor_EXTFAULT_checked_with_polling)      
#endif // #if defined(theMBDmotor_EXTFAULT_enabled)


#if defined(theMBDmotor_MBD_code_removed)
    
    // todo
    // 1. get the number of input frames
    // 2. decode them and produce a neutral format info that the MBD will accept. that for each motor.
    // 3. add to this info the vcc used by the board
    // 4. pass everything to MBD
    // 5. MBD.tick()
    // 6. retrieve the output data, form the reply can frames, fill the vector of output frames
    
    caninputframes.clear();
    canoutputframes.clear();
    
    for(const auto i : inputmessages)
    {
        caninputframes.push_back(i.frame);
    }

    
    volatile float vin = embot::hw::analog::getVin();
    vin = 48.0f;
    
    embot::app::bldc::theMC2agent::getInstance().tick(caninputframes, {EXTFAULTisPRESSED, vin}, canoutputframes);
    
    for(const auto o : canoutputframes)
    {
        embot::app::msg::Location l {bus2use, _config.adr};
        embot::app::bldc::MSG msg {l, o};
        outputmessages.push_back(msg);
    }  
        
    
#else

    #error this is the old code
    // add any input can frame into the supervisor input queue
    
    size_t ninputframes = std::min(inputmessages.size(), static_cast<size_t>(CAN_MAX_NUM_PACKETS));
    
    
    for(uint8_t i=0; i<CAN_MAX_NUM_PACKETS; i++) 
    {
        AMC_BLDC_U.PacketsRx.packets[i].available = false;
    }
    
    for(uint8_t i=0; i<ninputframes; i++) 
    {        
        uint32_t rx_id {0};
        inputmessages[i].frame.copyto(rx_id, AMC_BLDC_U.PacketsRx.packets[i].length, AMC_BLDC_U.PacketsRx.packets[i].packet.PAYLOAD); 
        AMC_BLDC_U.PacketsRx.packets[i].packet.ID = (uint16_T)rx_id;
        AMC_BLDC_U.PacketsRx.packets[i].available = true;
    }
    
    // read Vcc (in Volts)
    AMC_BLDC_U.SensorsData_p.supplyvoltagesensors.voltage = _rounder.getRoundedValueOf(embot::hw::motor::getVIN());

// just a print debug    
//    constexpr embot::core::Time freq { embot::core::time1second };
//    static embot::core::Time tt = embot::core::now();
//    embot::core::Time n = embot::core::now();
//    if((n-tt) > freq)
//    {
//        tt = n;
//        embot::core::print("Vcc = " + std::to_string(embot::hw::motor::getVIN()));
//    }
    
    
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
            embot::app::msg::Location l {bus2use, _config.adr};
            embot::app::bldc::MSG msg {l, fr};
            outputmessages.push_back(msg);
        }
    } 
    
#endif // #if defined(theMBDmotor_MBD_code_removed)
 

#if defined(theMBDmotor_MEASURES_enable_duration_tick)
    measureTick->stop();
#endif

#if defined(theMBDmotor_MEASURES_enable_testpoint)
    embot::hw::testpoint::off(tp1);
#endif

    return true;
}


void embot::app::board::amcfoc::cm7::theMBD::Impl::onCurrents(embot::hw::MOTOR m, const embot::hw::motor::bldc::Currents * const currs, void *owner)
{
    Impl * impl = reinterpret_cast<Impl*>(owner);
    if((nullptr == impl) || (nullptr == currs) || (false == embot::hw::motor::bldc::initialised(m)))
    {
        return;
    } 

    // 1. update the currents and retrieve boolean info if we have the latest currents for every motor
    
    bool allmotorshavethelatestcurrents = false;
    impl->loadCurrents(m, currs, allmotorshavethelatestcurrents);

    
    // 3. if so, call the FOC.
    //   - option 1: just inside this functions, so executed by the IRQ handler
    //   - option 2: send an evt to a high priority thread the executes it
    
    if(true == allmotorshavethelatestcurrents)
    {
        impl->FOC(m);
    }
    
    
}

void embot::app::board::amcfoc::cm7::theMBD::Impl::loadCurrents(embot::hw::MOTOR m, const embot::hw::motor::bldc::Currents * const currs, bool &allmotorsok)
{
    _items[embot::core::tointegral(m)].currents = *currs; 
    
    _items[embot::core::tointegral(m)].currOK = true;
    
    // now verify if all motors have updated currents
    // check if we have received the currenst for all motors
    //    to be done by a bitmask that is set for each motor, then checked and if ok allmotorsok gets true and mask cleared
    
    if((_items[0].currOK == true) && (_items[1].currOK == true))
    {
        allmotorsok = true;
        _items[0].currOK = _items[1].currOK = false;
    }
    else
    {
        allmotorsok = false;
    }
}

void embot::app::board::amcfoc::cm7::theMBD::Impl::updatePosition(embot::hw::MOTOR m)
{
    _items[embot::core::tointegral(m)].hallstatus = embot::hw::motor::bldc::hall(m);    
    // the following two must be verified carefully. previous implementation used the following:
    // - electical angle uses number of poles, so: BE SURE WE USE THEM ....
    // - position is computed incrementally, so: KEEP a static variable for it ....        
    _items[embot::core::tointegral(m)].electricalangle = embot::hw::motor::bldc::angle(m, embot::hw::motor::bldc::AngleType::hall_electrical);
    _items[embot::core::tointegral(m)].position = embot::hw::motor::bldc::angle(m, embot::hw::motor::bldc::AngleType::hall_mechanical);   
}

void embot::app::board::amcfoc::cm7::theMBD::Impl::FOC(embot::hw::MOTOR m)
{
    // in here we call everything for the motors on the list
    
    static constexpr std::initializer_list<embot::hw::MOTOR> themotors {embot::hw::MOTOR::one, embot::hw::MOTOR::two}; 
    
    // in here i make sure the class has all the info required for the FOC 
    // [x] the currents were loaded w/ loadCurrents()
    // [ ] i need the position of the rotor 
    // [ ] anything else ?
    
//    debugwave1.tick();

//    float a = debugwave1.get();
//    ImposedMechAngle = static_cast<int32_t>(a);
//    embot::hw::motor::hall::bsp::debug::set(embot::hw::MOTOR::one, a, embot::hw::motor::bldc::AngleType::hall_mechanical);
//    
//    readMechAngle = embot::hw::motor::bldc::angle(embot::hw::MOTOR::one, embot::hw::motor::bldc::AngleType::hall_mechanical);
//    readElecAngle = embot::hw::motor::bldc::angle(embot::hw::MOTOR::one, embot::hw::motor::bldc::AngleType::hall_electrical);
//    readHALL = embot::hw::motor::bldc::hall(embot::hw::MOTOR::one);
    
    for(const auto m : themotors)
    {
        updatePosition(m);
    }
            
    
    // i call MBD code and i get the PWM3
    
//    std::array<embot::hw::motor::bldc::PWM3, embot::hw::motor::bldc::MAXnumber> pwmtoapply {{ {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} }};
    
#if defined(theMBDmotor_MBD_code_removed)
    #warning add MBD code for the FOC 
        
    std::array<embot::app::bldc::mbd::interface::IO2::FOCinput, embot::hw::motor::bldc::MAXnumber> input {};
        
    std::array<embot::app::bldc::mbd::interface::IO2::FOCoutput, embot::hw::motor::bldc::MAXnumber> output {};

    for(const auto m : themotors)
    {
        uint8_t hall = _items[embot::core::tointegral(m)].hallstatus;
        float electricalangle = _items[embot::core::tointegral(m)].electricalangle;
        float mechanicalangle = _items[embot::core::tointegral(m)].position;
        
        input[embot::core::tointegral(m)].load(electricalangle, _items[embot::core::tointegral(m)].currents, mechanicalangle, hall); 
    }
    
    embot::app::bldc::mbd::interface::foc(input, output);
    
#if defined(DEBUG_PRINT_CURRENTS_PWM)
    
    static uint32_t cnt {0};
    static constexpr uint32_t CNT {20000};
    if(CNT == ++cnt)
    {
        cnt = 0;
//        uint8_t sec = embot::hw::motor::hall::sector(embot::hw::MOTOR::one);
        embot::core::print("currs = (" + std::to_string(input[0].currents.u) + ", " + std::to_string(input[0].currents.v) + ", " + std::to_string(input[0].currents.w) + ")");           
    
    }


#endif    
    
//    // just for test:
//    pwmtoapply[embot::core::tointegral(embot::hw::MOTOR::one)] = {10, 50, 60};    
//#else 
//    #warning MBD code for the FOC is missing
//    #if 0
//    in here i run a for loop for each motor, where,
//    - i give: 
//      - hall status [bits], 
//      - electrical angle [deg],
//      - position [deg],
//      - currents [A]    
//    - i get:
//      - pwm for the three phases [perc]
//    #endif
#endif   


#if defined(DEBUG_PRINT_PWM_VARIATIONS)
   static embot::hw::motor::bldc::PWM3 prevPWM {0, 0, 0};
   if( (output[0].pwm.u != prevPWM.u) || (output[0].pwm.v != prevPWM.v) || (output[0].pwm.w != prevPWM.w) )
   {
       uint8_t sec = embot::hw::motor::hall::sector(embot::hw::MOTOR::one);
       auto pwm = output[0].pwm;
//       embot::core::print("sect, pwm = " + std::to_string(pwm.u) + ", " + std::to_string(pwm.v) + ", " + std::to_string(pwm.w) + ", " + std::to_string(sec));
       prevPWM = output[0].pwm;
   }
#endif // #if defined(DEBUG_PRINT_PWM_VARIATIONS)  

    // i apply pwm
    
    for(const auto m : themotors)
    {
#if defined(DEBUG_PWM_min_0perc)
        _items[embot::core::tointegral(m)].pwm.u = std::clamp(output[embot::core::tointegral(m)].pwm.u - 5.0f, 0.0f, 100.0f);
        _items[embot::core::tointegral(m)].pwm.v = std::clamp(output[embot::core::tointegral(m)].pwm.v - 5.0f, 0.0f, 100.0f);   
        _items[embot::core::tointegral(m)].pwm.w = std::clamp(output[embot::core::tointegral(m)].pwm.w - 5.0f, 0.0f, 100.0f);        
#else        
        _items[embot::core::tointegral(m)].pwm = output[embot::core::tointegral(m)].pwm; 
#endif
        
        // uvw
        float u = _items[embot::core::tointegral(m)].pwm.u;
        float v = _items[embot::core::tointegral(m)].pwm.v;
        float w = _items[embot::core::tointegral(m)].pwm.w;
        
//        // wvu
//        _items[embot::core::tointegral(m)].pwm.u = w;
//        _items[embot::core::tointegral(m)].pwm.v = v;
//        _items[embot::core::tointegral(m)].pwm.w = u;

//        // uwv
//        _items[embot::core::tointegral(m)].pwm.u = u;
//        _items[embot::core::tointegral(m)].pwm.v = w;
//        _items[embot::core::tointegral(m)].pwm.w = v;
 

//        // vuw -> vibra e suona
//        _items[embot::core::tointegral(m)].pwm.u = v;
//        _items[embot::core::tointegral(m)].pwm.v = u;
//        _items[embot::core::tointegral(m)].pwm.w = w;

        // vwu -> gira
        _items[embot::core::tointegral(m)].pwm.u = v;
        _items[embot::core::tointegral(m)].pwm.v = w;
        _items[embot::core::tointegral(m)].pwm.w = u;
        
        if(m == embot::hw::MOTOR::one)
        {
            pwm1 = static_cast<int32_t>(_items[embot::core::tointegral(m)].pwm.u);
            pwm2 = static_cast<int32_t>(_items[embot::core::tointegral(m)].pwm.v);
            pwm3 = static_cast<int32_t>(_items[embot::core::tointegral(m)].pwm.w);
            
            CU1 = static_cast<int32_t>(1000*_items[embot::core::tointegral(m)].currents.u);
            CU2 = static_cast<int32_t>(1000*_items[embot::core::tointegral(m)].currents.v);
            CU3 = static_cast<int32_t>(1000*_items[embot::core::tointegral(m)].currents.w);
        }
        
        embot::hw::motor::bldc::set(m, _items[embot::core::tointegral(m)].pwm);        
    }
    
}

// the old one ...
// for now, keep it for reference

//void embot::app::board::amcfoc::cm7::theMBD::Impl::onCurrents_FOC_innerloop(void *owner, const embot::hw::motor::Currents * const currents)
//{
//    Impl * impl = reinterpret_cast<Impl*>(owner);
//    if((nullptr == impl) || (nullptr == currents))
//    {
//        return;
//    }    

//#if defined(theMBDmotor_MEASURES_enable_duration_foc)    
//    impl->measureFOC->start();
//#endif
//    
//    // 1. copy currents straight away, so that we can use them
//    embot::hw::motor::Currents currs = *currents;
//   
//    
//    // remember to manage impl->EXTFAULTisPRESSED ............

//    // retrieve the current value of the Hall sensors
//    uint8_t hall {0};    
//    embot::hw::motor::gethallstatus(embot::hw::MOTOR::one, hall);
//    
//#if defined(theMBDmotor_MBD_code_removed)
//#else    
//    AMC_BLDC_U.SensorsData_p.motorsensors.hallABC = hall;
//#endif
//    
//    // retrieve the current value of the encoder
//    embot::hw::motor::Position electricalAngle {0};
//    embot::hw::motor::getencoder(embot::hw::MOTOR::one, electricalAngle); 
//    

//    static int32_t position {0};
//    static int32_t electricalAngleOld = static_cast<int32_t>(electricalAngle);
//    // keep int16_t ... to manage overflow ... 
//    int16_t delta = electricalAngle - electricalAngleOld;
//    electricalAngleOld = electricalAngle;

//#if defined(theMBDmotor_MBD_code_removed)
//#else
//    
//    // calculate the current joint position
//    uint8_t polepairs = (0 != AMC_BLDC_Y.ConfigurationParameters_p.motorconfig.pole_pairs) ? AMC_BLDC_Y.ConfigurationParameters_p.motorconfig.pole_pairs : 1;
//    position = position + delta / polepairs;
//    
//    AMC_BLDC_U.SensorsData_p.motorsensors.angle = static_cast<real32_T>(electricalAngle)*0.0054931640625f; // (60 interval angle)
//    
//    // convert the current from mA to A
//    AMC_BLDC_U.SensorsData_p.motorsensors.Iabc[0] = 0.001f*currs.u;
//    AMC_BLDC_U.SensorsData_p.motorsensors.Iabc[1] = 0.001f*currs.v;
//    AMC_BLDC_U.SensorsData_p.motorsensors.Iabc[2] = 0.001f*currs.w;
//    
//    // -----------------------------------------------------------------------------
//    // FOC Step Function (~16.6 KHz)
//    // -----------------------------------------------------------------------------
//    
//    AMC_BLDC_step_FOC();
//    
//    // -----------------------------------------------------------------------------
//    
//    AMC_BLDC_U.SensorsData_p.jointpositions.position = static_cast<real32_T>(position) * 0.0054931640625f; // iCubDegree -> deg
//    
//    embot::hw::motor::PWMperc pwmperc 
//    {
//        AMC_BLDC_Y.ControlOutputs_p.Vabc[0], AMC_BLDC_Y.ControlOutputs_p.Vabc[1], AMC_BLDC_Y.ControlOutputs_p.Vabc[2]
//    };
//    embot::hw::motor::setPWM(embot::hw::MOTOR::one, pwmperc);

//#endif

//#if defined(theMBDmotor_MEASURES_enable_duration_foc)
//    impl->measureFOC->stop();
//#endif    
//}


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::board::amcfoc::cm7::theMBD& embot::app::board::amcfoc::cm7::theMBD::getInstance()
{
    static theMBD* p = new theMBD();
    return *p;
}

embot::app::board::amcfoc::cm7::theMBD::theMBD()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::board::amcfoc::cm7::theMBD::~theMBD() { }
        

bool embot::app::board::amcfoc::cm7::theMBD::initialise(const Config &config)
{
    return pImpl->initialise(config);
}

bool embot::app::board::amcfoc::cm7::theMBD::tick(const std::vector<embot::app::bldc::MSG> &inputmessages, std::vector<embot::app::bldc::MSG> &outputmessages)
{
    return pImpl->tick(inputmessages, outputmessages);
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


