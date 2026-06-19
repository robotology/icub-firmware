
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_bldc_theMBD.h"

int32_t ImposedMechAngle = 0;
int32_t readMechAngle = 0;
int32_t readElecAngle = 0;
uint32_t readSector = 0;
uint8_t readHALL = 0;


// --------------------------------------------------------------------------------------------------------------------
// - defines
// --------------------------------------------------------------------------------------------------------------------


// -
// - debug section
//

//#define DEBUG_LOG_ANGLE_VARIATIONS
//#define DEBUG_LOG_ANGLE_VARIATIONS__PRINT_CHANGES
//#define DEBUG_PRINT_PWM_VARIATIONS
//#define DEBUG_PRINT_CURRENTS_PWM

//#define DEBUG_PWM_min_0perc

//#define TEST_Quad_Encoder_Mot_1
//#define TEST_ADC_Analog

#if defined(TEST_Quad_Encoder_Mot_1)
    int32_t angle_global=0;
#endif


// -
// - EXT FAULT section
// -

// it enables / disables the external fault.
// if not enabled ... we just dont manage any external fault button or led

#define theMBDmotor_EXTFAULT_enabled
#undef theMBDmotor_EXTFAULT_disabled



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_app_bldc_theMC2agent.h"


//#include "embot_hw_button.h"
#include "embot_hw_motor_bldc.h"
#include "embot_app_scope.h"
#include "embot_hw_sys.h"
#include "embot_core.h"
#include "embot_app_theLEDmanager.h"
//#include "embot_hw_testpoint.h"
#include <array>

// some protocol includes
#include "embot_prot_can_motor_polling.h"
#include "embot_prot_can_motor_periodic.h"

#include "embot_app_application_CANagentMC2.h"
#include "embot_app_bldc_MBD_interface.h"
#include "embot_hw_motor_bldc_hall.h"
#include "embot_hw_motor_bldc_qenc.h"
#include "embot_hw_motor_bldc_extfault.h"
#include "embot_hw_analog.h"






// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



#if defined(theMBDmotor_MEASURES_enable_duration_tick)
constexpr bool useDUMMYforTICK {true};
#endif

#if defined(theMBDmotor_MEASURES_enable_duration_foc)
constexpr bool useDUMMYforFOC {true};
#endif

namespace embot::appl::bldc::tools {

    struct extFAULTmanager
    {                  
        extFAULTmanager() = default; 
        
        void initialize(const std::vector<embot::hw::MOTOR> &mots) 
        {
            themotors = mots;
            
            embot::hw::motor::bldc::extfault::init({});
            led = embot::hw::motor::bldc::extfault::led();
        
            faultvalues.reserve(faultcapacity);
            faultvalues.clear();

            for(size_t i=0; i<faultcapacity; i++)
            {
                faultvalues.push_back(embot::hw::motor::bldc::extfault::pressed());
            }
                    
            previsPRESSED = isPRESSED = allarepressed();  

            update();            
        }

        
        void check()
        {            
            if(faultvalues.size() >= faultcapacity)
            {
                faultvalues.erase(faultvalues.begin());
            }
            
            bool pressed = embot::hw::motor::bldc::extfault::pressed();
            faultvalues.push_back(pressed);
            
            isPRESSED = allarepressed(); 
                               
            if(previsPRESSED != isPRESSED)
            {
                previsPRESSED = isPRESSED;                
                update();
            }                
        }
        
        bool ispressed() 
        {
            return isPRESSED;
        }
        
    private:
        
        void update()
        {
            embot::app::bldc::mbd::interface::getIO2handle().set_fault(isPRESSED);
            
            for(auto m : themotors)
            {
                embot::hw::motor::bldc::fault(m, isPRESSED);
            }

            if(true == isPRESSED)
            {
                embot::app::theLEDmanager::getInstance().get(led).on();
            }
            else
            {
                embot::app::theLEDmanager::getInstance().get(led).off();
            }                        
        }
        
        bool allarepressed()
        {
#if defined(theMBDmotor_EXTFAULT_disabled) 
            bool v = false;
#else
            bool v = std::all_of(faultvalues.cbegin(), faultvalues.cend(), [](auto v){ return v; });
#endif   
            return v;        
        }
        
        embot::hw::LED led {embot::hw::LED::none};
        volatile bool isPRESSED {false};   
        volatile bool previsPRESSED {false};         
        static constexpr size_t faultcapacity {5};
        std::vector<bool> faultvalues {};            
        std::vector<embot::hw::MOTOR> themotors {};
    };    

} // namespace embot::appl::bldc::tools {


struct embot::app::bldc::theMBD::Impl
{
       
    Impl() = default;  
        
    // the initialization code
    bool initialise(const Config &config);
        
    // the code called @ 1 kHz (aka every 1000 usec) inside a high priority thread
    bool tick(const std::vector<embot::app::bldc::MSG> &inputmessages, std::vector<embot::app::bldc::MSG> &outputmessages);
    
private:
    
    // place debug code in here that is executed either at start or end of tick()
    enum class EXEC { atSTART, atEND };
    void debugontick(EXEC ex, const std::vector<embot::app::bldc::MSG> &inputmessages, std::vector<embot::app::bldc::MSG> &outputmessages);

    // called by the IRQ handler of end of currents acquisition. 
    // if pwm is 66.6 kHz it is called twice (once per motor) every 45 usec
    static void onCurrents(embot::hw::MOTOR m, const embot::hw::motor::bldc::Currents * const currs, void *owner); 
    
    // executed once every 45 usec and called inside (or triggered by) the onCurrents() callback when all the motors have current measures
    void FOC(embot::hw::MOTOR m);  

    // data
    
    Config _config {};
    bool _initted {false};
   
    
    struct Status
    {
        Status() = default;        
        Status(embot::hw::MOTOR m) : motor(m) {}
            
        void load(const embot::hw::motor::bldc::Currents &c)
        {
            currents = c;
        }
        
        void set(const embot::hw::motor::bldc::PWM3 &p)
        {
            pwm = p;
        }
            
        embot::hw::MOTOR motor {embot::hw::MOTOR::none};
        embot::hw::motor::bldc::PWM3 pwm {0, 0, 0};
        embot::prot::can::motor::ControlMode ctrlmode {embot::prot::can::motor::ControlMode::Idle};
        embot::hw::motor::bldc::Currents currents {0, 0, 0}; 
        embot::hw::motor::bldc::HallStatus hallstatus {0}; 
        embot::hw::motor::bldc::Angle electricalangle {0}; 
        embot::hw::motor::bldc::Angle position {0};  
        embot::hw::motor::bldc::Angle qencangle {0};
        embot::hw::motor::bldc::Angle qencangleoflastindex {0};        
    };
    
    struct Motors
    {
        Motors() = default;
        
        void load(const std::initializer_list<embot::hw::MOTOR> &motors)
        {
            for(const auto &m : motors)
            {
                push_back(m);
            }
        }        
        
        size_t numberof() const
        {
            return _theids.size();
        }
        
        const std::vector<embot::hw::MOTOR> & get() const
        {
            return _theids;
        }
                        
        bool load(embot::hw::MOTOR m, const embot::hw::motor::bldc::Currents &c)
        {
            uint8_t i = index(m);
            _status[i].load(c);
            embot::core::binary::bit::set(_motorswithcurrents, i);
            
            bool alldone = embot::core::binary::mask::check(_motorswithcurrents, _maskofmotors);
            embot::core::binary::mask::clear(_motorswithcurrents, _maskofmotors);
            
            return alldone;            
        }
        
        void updatepositions(embot::hw::MOTOR m)
        {
            Status &s = _status[index(m)];
            
            s.hallstatus = embot::hw::motor::bldc::hallstatus(m);
            s.electricalangle = embot::hw::motor::bldc::angle(m, embot::hw::motor::bldc::AngleType::hall_electrical);
            s.position = embot::hw::motor::bldc::angle(m, embot::hw::motor::bldc::AngleType::hall_mechanical);
            s.qencangle = embot::hw::motor::bldc::angle(m, embot::hw::motor::bldc::AngleType::quadenc_mechanical);
            s.qencangleoflastindex = embot::hw::motor::bldc::angle(m, embot::hw::motor::bldc::AngleType::quadenc_mechanical_lastindex);
        }
        
        void set(embot::hw::MOTOR m, const embot::hw::motor::bldc::PWM3 &p)
        {
            _status[index(m)].set(p);
        }            
        
        const Status & status(embot::hw::MOTOR m) const
        {
            return _status[index(m)];
        }
       

    private:         
        
        void push_back(embot::hw::MOTOR m)
        {
            // we must verify that we insert in order MOTOR::one, MOTOR::two etc
            // or that we have a map_motor2index
            uint8_t i = _theids.size();
            _map_motor2index[embot::core::tointegral(m)] = i;
            embot::core::binary::bit::set(_maskofmotors, i);
            _theids.push_back(m);
            _status.push_back({m});
        }            

        uint8_t index(embot::hw::MOTOR m) const
        { 
            return _map_motor2index[embot::core::tointegral(m)]; 
        }
        
        std::vector<Status> _status {};
        std::vector<embot::hw::MOTOR> _theids {};
        std::array<uint8_t, embot::hw::motor::bldc::MAXnumber> _map_motor2index {0};
        uint8_t _maskofmotors {0};
        uint8_t _motorswithcurrents {0};        
       
    };
    
    Motors _themotors {};
    
    embot::appl::bldc::tools::extFAULTmanager _extFAULTman {};
    
    embot::app::msg::BUS _bus2use {embot::app::msg::BUS::none};
    
    embot::app::bldc::Rounder _vcc_rounder {};
            
    std::vector<embot::prot::can::Frame> _caninputframes {};
    std::vector<embot::prot::can::Frame> _canoutputframes {};
        
    std::vector<embot::app::bldc::mbd::interface::IO2::FOCinput> _input {};
        
    std::vector<embot::app::bldc::mbd::interface::IO2::FOCoutput> _output {};
        
};


      
bool embot::app::bldc::theMBD::Impl::initialise(const Config &config)
{
    if(true == _initted)
    {
        return true;
    }
    
    _config = config;
    
    embot::core::print("embot::app::bldc::theMBD::Impl::initialise()");
    
    // load the supported motors
    _themotors.load(embot::hw::motor::bldc::supported());
           
    
    // init the motors we have
    for(const auto & m : _themotors.get())
    {
        embot::hw::motor::bldc::init(m, {});
    }
       
    
    // the CAN MC2 agent
    
    std::array<uint8_t, embot::app::bldc::theMC2agent::numberofmotors> adrs {static_cast<uint8_t>(config.adr), static_cast<uint8_t>(config.adr+1)};    
    embot::app::bldc::theMC2agent::getInstance().initialise({adrs, {}});
    
    
    // the mbd interface
    _input.resize(_themotors.numberof());
    _output.resize(_themotors.numberof());
    embot::app::bldc::mbd::interface::init();         
    // and the external fault manager
    _extFAULTman.initialize(_themotors.get());
    // and use it     
    embot::app::bldc::mbd::interface::getIO2handle().set_fault(_extFAULTman.ispressed());
    

    // assign the current callback
    for(const auto &m : _themotors.get())
    {
        embot::hw::motor::bldc::set(m, {m, Impl::onCurrents, this});
    }
                    
    // init the rounder used to get the Vcc
    _vcc_rounder.init(0.0f, 1); // default initial value=0, decimals=1
    
    
#if defined(theMBDmotor_MEASURES_enable_duration_foc)    
    measureFOC = (false == useDUMMYforFOC) ? new MeasureHisto({"foc", 0, 64*embot::core::time1microsec, 1, true, 100*1000}) : new MeasureDummy;
#endif
 
#if defined(theMBDmotor_MEASURES_enable_duration_tick)
    measureTick = (false == useDUMMYforTICK) ? new MeasureHisto({"tick", 0, 400*embot::core::time1microsec, 1, true, 10*1000}) : new MeasureDummy;
#endif  
      
        
    _initted = true;
    return _initted;
}




// called every 1 ms, 
// inputmessages.size() is always <= 4, outputframes.size() is always = 0

bool embot::app::bldc::theMBD::Impl::tick(const std::vector<embot::app::bldc::MSG> &inputmessages, std::vector<embot::app::bldc::MSG> &outputmessages)
{    
#if defined(theMBDmotor_MEASURES_enable_duration_tick)
    measureTick->start();
#endif
    
    debugontick(EXEC::atSTART, inputmessages, outputmessages);
    
    // at first i check the external faults    
    _extFAULTman.check();  
    
    // then i lock the bus to be can or icc
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
        _bus2use = inputmessages[0].location.getbus();
    }
#endif  
     
      
    // prepare what is needed by the MC2 agent and call it
    
    _caninputframes.clear();
    _canoutputframes.clear();
    
    for(const auto i : inputmessages)
    {
        _caninputframes.push_back(i.frame);
    }
    
    volatile float vin = embot::hw::analog::get(embot::hw::analog::Value::Vin);
    
    embot::app::bldc::theMC2agent::getInstance().tick(_caninputframes, {_extFAULTman.ispressed(), vin}, _canoutputframes);
    
    // then prepare outputmessages
    
    for(const auto &o : _canoutputframes)
    {
        if(embot::app::msg::BUS::none != _bus2use)
        {
            embot::app::msg::Location l {_bus2use, _config.adr};
            embot::app::bldc::MSG msg {l, o};
            outputmessages.push_back(msg);
        }
    }

    debugontick(EXEC::atEND, inputmessages, outputmessages);

#if defined(theMBDmotor_MEASURES_enable_duration_tick)
    measureTick->stop();
#endif
    return true;
}


void embot::app::bldc::theMBD::Impl::onCurrents(embot::hw::MOTOR m, const embot::hw::motor::bldc::Currents * const currs, void *owner)
{
    Impl * impl = reinterpret_cast<Impl*>(owner);
    if((nullptr == impl) || (nullptr == currs) || (false == embot::hw::motor::bldc::initialised(m)))
    {
        return;
    } 

    // 1. update the currents and retrieve boolean info if we have the latest currents for every motor
    
//    bool allmotorshavethelatestcurrents = false;
//    impl->loadCurrents(m, currs, allmotorshavethelatestcurrents);
    bool allmotorshavethelatestcurrents = impl->_themotors.load(m, *currs);
    
    // 3. if so, call the FOC.
    //   - option 1: just inside this functions, so executed by the IRQ handler
    //   - option 2: send an evt to a high priority thread the executes it
    
    if(true == allmotorshavethelatestcurrents)
    {
        impl->FOC(m);
    }
       
}


void embot::app::bldc::theMBD::Impl::FOC(embot::hw::MOTOR m)
{
    // in here we call everything for the motors on the list
    
    //static constexpr std::initializer_list<embot::hw::MOTOR> themotors {embot::hw::MOTOR::one, embot::hw::MOTOR::two}; 
    
    // in here i make sure the class has all the info required for the FOC 
    // [x] the currents were loaded w/ loadCurrents()
    // [ ] i need the position of the rotor 
    // [ ] anything else ?

    
    _themotors.updatepositions(m);
    
                
    // i call MBD code and i get the PWM3
    
//    std::array<embot::hw::motor::bldc::PWM3, embot::hw::motor::bldc::MAXnumber> pwmtoapply {{ {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} }};
    

    for(const auto & m : _themotors.get())
    {
        const Status &s = _themotors.status(m);       
        _input[embot::core::tointegral(m)].load(s.electricalangle, s.currents, s.position, s.hallstatus, { s.qencangle, s.qencangleoflastindex }); 
    }
    
    embot::app::bldc::mbd::interface::foc(_input, _output);

    // i apply pwm
    
    for(const auto & m : _themotors.get())
    {
#if defined(DEBUG_PWM_min_0perc)
        embot::hw::motor::bldc::PWM3 pwm {
            std::clamp(output[embot::core::tointegral(m)].pwm.u - 5.0f, 0.0f, 100.0f),
            std::clamp(output[embot::core::tointegral(m)].pwm.v - 5.0f, 0.0f, 100.0f),
            std::clamp(output[embot::core::tointegral(m)].pwm.w - 5.0f, 0.0f, 100.0f)
        };
#else
       embot::hw::motor::bldc::PWM3 pwm = _output[embot::core::tointegral(m)].pwm;
#endif   
        
       _themotors.set(m, pwm);            
        embot::hw::motor::bldc::set(m, pwm);        
    }
}


void embot::app::bldc::theMBD::Impl::debugontick(EXEC ex, const std::vector<embot::app::bldc::MSG> &inputmessages, std::vector<embot::app::bldc::MSG> &outputmessages)
{
    if(ex == EXEC::atSTART)
    {
        
    }
    else if(ex == EXEC::atEND)
    {
        
    }
    
}

// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::bldc::theMBD& embot::app::bldc::theMBD::getInstance()
{
    static theMBD* p = new theMBD();
    return *p;
}

embot::app::bldc::theMBD::theMBD()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::bldc::theMBD::~theMBD() { }
        

bool embot::app::bldc::theMBD::initialise(const Config &config)
{
    return pImpl->initialise(config);
}

bool embot::app::bldc::theMBD::tick(const std::vector<embot::app::bldc::MSG> &inputmessages, std::vector<embot::app::bldc::MSG> &outputmessages)
{
    return pImpl->tick(inputmessages, outputmessages);
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


