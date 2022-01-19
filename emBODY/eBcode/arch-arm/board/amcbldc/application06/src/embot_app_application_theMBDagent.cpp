
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_application_theMBDagent.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_button.h"
#include "embot_hw_motor.h"
#include "embot_app_theCANboardInfo.h"
#include "embot_app_scope.h"
#include "embot_hw_sys.h"
#include "embot_app_theLEDmanager.h"
#include <array>

// mdb components
#include "AMC_BLDC.h"


//#define TEST_DURATION_FOC

#define EXTFAULT_enabled
#define EXTFAULT_handler_will_disable_motor

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

namespace embot::app::application {

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
   

struct MeasureHisto : public Measure
{
    constexpr static bool enablereport {true};
    constexpr static uint32_t reportfrequency {10000};
    
    struct Config
    {
        embot::core::relTime minduration {0};
        embot::core::relTime maxduration {100*embot::core::time1microsec};  
        embot::core::relTime resolution {100*embot::core::time1microsec};
        constexpr Config(embot::core::relTime min, embot::core::relTime max, embot::core::relTime res = embot::core::time1microsec) 
                        : minduration(min), maxduration(max), resolution(res) {} 
    };
    
    embot::app::scope::SignalHisto *sighisto {nullptr};
    
    const embot::tools::Histogram::Values * vv {nullptr};
    
    // i use ram ... maybe not best solution. maybe we could also print the histo.
    std::array<uint64_t, 512> vals {};   
    uint64_t beyond {0}; 
    uint32_t counter {0};    

    MeasureHisto(const Config &config)
    {
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
        
        if(enablereport)
        {
            if(++counter >= reportfrequency)
            {
                counter = 0;
                report();  
            }  
        }        
    }
    
    void report()
    {            
        beyond = vv->beyond;
                    
        for(int i=0; i<vv->inside.size(); i++)
        {
            if(i < vals.size())
            {
                vals[i] = vv->inside[i];
            }
        }            
    }

};

} // end of namespace


constexpr bool useDUMMYforFOC {true};
constexpr bool useDUMMYforTICK {true};

struct embot::app::application::theMBDagent::Impl
{
    Impl() = default;  
        
    // the initialization code
    bool initialise();
        
    // the code called @ 1 kHz (aka every 1000 usec) inside a high priority thread
    bool tick(std::vector<embot::prot::can::Frame> &inpframes, std::vector<embot::prot::can::Frame> &outframes);

    // the code called @ 80/3 kHz (aka every 37.5 usec) inside the DMA1_Channel2_IRQHandler()
    // it must be static because we use it as a callback
    static void onCurrents_FOC_innerloop(void *owner, const embot::hw::motor::Currents * const currents);        

    // the objects whiche measure the duration of the two above functions
    Measure *measureFOC {nullptr};
    Measure *measureTick {nullptr}; 
    
    // the MBD generated classes + other glue code.
    // MBD-gen-begin ->
    
    amc_bldc_codegen::AMC_BLDC amc_bldc {};  
    
                
    // <- MBD-gen-end
    //


    // all the rest, which may or may not be required anymore
    Config config {};
    bool initted {false};
    bool already_enabled {false};
        
    embot::hw::motor::Pwm pwm {0};
    embot::prot::can::motor::polling::ControlMode cm {embot::prot::can::motor::polling::ControlMode::Idle};
    bool applychanges {false};
    
    static constexpr embot::hw::BTN buttonEXTfault {embot::hw::BTN::one};
    static void onEXTFAULTpressedreleased(void *owner);  
    volatile bool EXTFAULTisPRESSED {false};   
    volatile bool prevEXTFAULTisPRESSED {false};    
    volatile embot::core::Time EXTFAULTpressedtime {0};
    volatile embot::core::Time EXTFAULTreleasedtime {0};
    
    bool addMCstatus(std::vector<embot::prot::can::Frame> &outframes);
};

      
bool embot::app::application::theMBDagent::Impl::initialise()
{
    if(true == initted)
    {
        return true;
    }
    
    embot::core::print("embot::app::application::theMBDagent::Impl::initialise()");
    
    // create the measure for the foc
    if(true == useDUMMYforFOC)
    {
        measureFOC = new MeasureDummy;
    }
    else
    {
        measureFOC = new MeasureHisto({0, 64*embot::core::time1microsec, 1});
    }
    
    // create the measure for the tick
    if(true == useDUMMYforTICK)
    {
        measureTick = new MeasureDummy;
    }
    else
    {
        measureTick = new MeasureHisto({0, 400*embot::core::time1microsec, 1});
    }
    
    // init the external fault. 
    // we call cbkOnEXTfault.execute() when the button is pressed or released.
    // in the callback we set EXTFAULTisPRESSED true or false depending on value of
    // embot::hw::button::pressed(buttonEXTfault)
    // we also disable the motors if true. 

#if defined(EXTFAULT_enabled)
    embot::core::Callback cbkOnEXTFAULT {onEXTFAULTpressedreleased, this};
    embot::hw::button::init(buttonEXTfault, {embot::hw::button::Mode::TriggeredOnPressAndRelease, cbkOnEXTFAULT, 0});
    prevEXTFAULTisPRESSED = EXTFAULTisPRESSED = embot::hw::button::pressed(buttonEXTfault);
#else
    prevEXTFAULTisPRESSED = EXTFAULTisPRESSED = false;    
#endif
    
    if(true == EXTFAULTisPRESSED)
    {
#if defined(EXTFAULT_handler_will_disable_motor)         
        embot::hw::motor::fault(embot::hw::MOTOR::one, true);
#endif        
        embot::app::theLEDmanager::getInstance().get(embot::hw::LED::two).on();
    }
    else 
    {
        embot::hw::motor::fault(embot::hw::MOTOR::one, false);
        embot::app::theLEDmanager::getInstance().get(embot::hw::LED::two).off();
    }
    
    // init MBD
    amc_bldc.initialize();
    
    amc_bldc.AMC_BLDC_U.ExternalFlags_fault_button = EXTFAULTisPRESSED;
    
    // init motor
    embot::hw::motor::init(embot::hw::MOTOR::one, {});
    
    // assign the callback to the current availability
    embot::hw::motor::setCallbackOnCurrents(embot::hw::MOTOR::one, Impl::onCurrents_FOC_innerloop, this);
    
    already_enabled = false;

    CAN_ID_AMC = embot::app::theCANboardInfo::getInstance().cachedCANaddress();

    initted = true;
    return initted;
}


void embot::app::application::theMBDagent::Impl::onEXTFAULTpressedreleased(void *owner)
{
    Impl * impl = reinterpret_cast<Impl*>(owner);
    if(nullptr == impl)
    {
        return;
    }
    
    impl->EXTFAULTisPRESSED = embot::hw::button::pressed(buttonEXTfault);
    
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
bool embot::app::application::theMBDagent::Impl::tick(std::vector<embot::prot::can::Frame> &inpframes, std::vector<embot::prot::can::Frame> &outframes)
{    
    measureTick->start();
    
    if(prevEXTFAULTisPRESSED != EXTFAULTisPRESSED)
    {
        prevEXTFAULTisPRESSED = EXTFAULTisPRESSED;  
        // and manage the transitions [pressed -> unpressed] or vice-versa and use also
        // EXTFAULTpressedtime and / or EXTFAULTreleasedtime and

        amc_bldc.AMC_BLDC_U.ExternalFlags_fault_button = EXTFAULTisPRESSED;        
        
        if(true == EXTFAULTisPRESSED)
        {
            embot::app::theLEDmanager::getInstance().get(embot::hw::LED::two).on();
        }
        else
        {
            embot::app::theLEDmanager::getInstance().get(embot::hw::LED::two).off();
        }
    }
    
    uint8_t rx_data[8] {0};
    uint8_t rx_size {0};
    uint32_t rx_id {0};
    
    
    // check for CAN input frame
    size_t ninputframes = inpframes.size();
    if(0 == ninputframes) 
    {
        amc_bldc.AMC_BLDC_U.PacketsRx_available = 0;
    } 
    else
    {   
        // retrieve the CAN frames, use them, remove them from the vector
        // so far we consume only one frame every tick
        size_t consumedframes {1};
        amc_bldc.AMC_BLDC_U.PacketsRx_available = 1;
        // get the first
        embot::prot::can::Frame frame = inpframes.front();
        // copy it
        frame.copyto(rx_id, rx_size, rx_data);
        
        if((rx_data[0] == 0x65) && (rx_id == 0x1))
        {
            amc_bldc.AMC_BLDC_U.PacketsRx_available = 0;    // TODO: FIX IN MBD
        }
        
//        if(rx_id == 0x10F)
//        {
//            static char msg2[64];
//            static uint32_t counter;
//            if(counter % 100 == 0)
//            {
//                sprintf(msg2, "%x %x", rx_data[1], rx_data[0]);
//                embot::core::print(msg2);
//                counter = 0;
//            }
//            counter++;
//        }
        
//        static uint32_t cnt = 0;
//        if((++cnt % 500) == 1)
//        {
//            embot::core::print(std::string("size = ") + std::to_string(ninputframes) + ", d[0] = " + std::to_string(rx_data[0]) + 
//                           ", consumed = " + std::to_string(consumedframes) + " @ " + embot::core::TimeFormatter(embot::core::now()).to_string());
//        }
        
        // clean up the first consumedframes positions
        inpframes.erase(inpframes.begin(), inpframes.begin()+consumedframes);
    }
    
    // save the CAN frame into the input structure of the model
    amc_bldc.AMC_BLDC_U.PacketsRx_lengths = (uint8_T)rx_size;
    amc_bldc.AMC_BLDC_U.PacketsRx_packets_ID = (uint16_T)rx_id;
    
    for (int i=0;i<rx_size;i++) 
    {
        amc_bldc.AMC_BLDC_U.PacketsRx_packets_PAYLOAD[i] = (uint8_T)rx_data[i];
    }
    
    
    // -----------------------------------------------------------------------------
    // Model Step Function (1 ms)
    // -----------------------------------------------------------------------------
    
    amc_bldc.step_Time();
    
    
    // if there is an output available, send it through the CAN
    if(amc_bldc.AMC_BLDC_Y.PacketsTx.available)
    {
        embot::prot::can::Frame fr {amc_bldc.AMC_BLDC_Y.PacketsTx.packets.ID, 8, amc_bldc.AMC_BLDC_Y.PacketsTx.packets.PAYLOAD};
        outframes.push_back(fr);
    }
    
    bool txstatus = amc_bldc.AMC_BLDC_Y.Flags_p.DBG;
    if(true == txstatus)
    {
        addMCstatus(outframes);
    }

    measureTick->stop();
    
    
    return true;
}


void embot::app::application::theMBDagent::Impl::onCurrents_FOC_innerloop(void *owner, const embot::hw::motor::Currents * const currents)
{
    Impl * impl = reinterpret_cast<Impl*>(owner);
    if((nullptr == impl) || (nullptr == currents))
    {
        return;
    }    
        
    impl->measureFOC->start();
    
    // 1. copy currents straight away, so that we can use them
    embot::hw::motor::Currents currs = *currents;
   
#if defined(TEST_DURATION_FOC)        
    embot::hw::sys::delay(25);
#else
    
    // remember to manage impl->EXTFAULTisPRESSED ............

    // retrieve the current value of the Hall sensors 
    embot::hw::motor::gethallstatus(embot::hw::MOTOR::one, impl->amc_bldc.AMC_BLDC_U.SensorsData_motorsensors_hallAB);
    
    // retrieve the current value of the encoder
    embot::hw::motor::Position electricalAngle {0};
    embot::hw::motor::getencoder(embot::hw::MOTOR::one, electricalAngle); 
    

    static int32_T position {0};
    static int32_T electricalAngleOld = static_cast<int32_T>(electricalAngle);
    // keep int16_t ... to manage overflow ... 
    int16_t delta = electricalAngle - electricalAngleOld;
    electricalAngleOld = electricalAngle;
    
    // calculate the current joint position
    position = position + delta / 7;  // the motor has 14 poles, hence 7 pole pairs
    
    impl->amc_bldc.AMC_BLDC_U.SensorsData_motorsensors_angle = static_cast<real32_T>(electricalAngle)*0.0054931640625f; // (60 interval angle)
    
    // convert the current from mA to A
    impl->amc_bldc.AMC_BLDC_U.SensorsData_motorsensors_Iabc[0] = 0.001f*currs.u;
    impl->amc_bldc.AMC_BLDC_U.SensorsData_motorsensors_Iabc[1] = 0.001f*currs.v;
    impl->amc_bldc.AMC_BLDC_U.SensorsData_motorsensors_Iabc[2] = 0.001f*currs.w;
    
    // -----------------------------------------------------------------------------
    // FOC Step Function (~26.6 KHz)
    // -----------------------------------------------------------------------------
    
    impl->amc_bldc.step_FOC();
    
    // save sensor data
    //impl->amc_bldc.AMC_BLDC_U.SensorsData_motorsensors_curren = impl->amc_bldc.AMC_BLDC_Y.ControlOutputs_p.Iq_fbk.current * 1000; // *0.001f in order to have mA
    
#warning workaround. Fix this parameter in model design. 

    impl->amc_bldc.AMC_BLDC_U.SensorsData_jointpositions_posi = static_cast<real32_T>(position) * 0.0054931640625f; // iCubDegree -> deg
    //sensors_data.motorsensors.omega =  speed * 80 / 3 * 0.0054931640625f; // Frequency = 80/3. Angular velocity --> iCubDegree/ms
                                                                            // * 0.0054931640625f ==> icubDegree -> Deg    
    // Set the voltages
    int32_T Vabc0 = static_cast<int32_T>(impl->amc_bldc.AMC_BLDC_Y.ControlOutputs_p.Vabc[0] * 163.83F);
    int32_T Vabc1 = static_cast<int32_T>(impl->amc_bldc.AMC_BLDC_Y.ControlOutputs_p.Vabc[1] * 163.83F);
    int32_T Vabc2 = static_cast<int32_T>(impl->amc_bldc.AMC_BLDC_Y.ControlOutputs_p.Vabc[2] * 163.83F);
    
    embot::hw::motor::setpwm(embot::hw::MOTOR::one, Vabc0, Vabc1, Vabc2);
   
//#define DEBUG_PARAMS // TODO: remove
#ifdef DEBUG_PARAMS
    
    static char msg2[64];
    static uint32_t counter;
    if(counter % 1000 == 0)
    {
        sprintf(msg2, "%d %d,%d,%d,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f", \
                                 impl->amc_bldc.AMC_BLDC_Y.Flags_p.control_mode, \
                                 Vabc0,  \
                                 Vabc1,  \
                                 Vabc2,  \
                                 impl->amc_bldc.AMC_BLDC_U.SensorsData_motorsensors_Iabc[0],  \
                                 impl->amc_bldc.AMC_BLDC_U.SensorsData_motorsensors_Iabc[1],  \
                                 impl->amc_bldc.AMC_BLDC_U.SensorsData_motorsensors_Iabc[2],  \
                                 impl->amc_bldc.AMC_BLDC_U.SensorsData_motorsensors_angle,    \
                                 impl->amc_bldc.AMC_BLDC_B.Targets_n.motorcurrent.current,    \
                                 impl->amc_bldc.AMC_BLDC_Y.ControlOutputs_p.Iq_fbk.current);
        embot::core::print(msg2);
        counter = 0;
    }
    counter++;
#endif

#endif // #if defined(TEST_DURATION_FOC) 

    impl->measureFOC->stop();
}

uint8_t remapControlMode(uint8_t controlMode)
{
    switch(controlMode){
        case 0: 
            return 0xb0; // NotConfigured
            break;
        case 1: 
            return MCControlModes_Idle;
            break;
        case 4: 
            return MCControlModes_Current;
            break;
        case 6:
            return MCControlModes_OpenLoop;
        default: 
            return 0x99; // TODO: Fix!
            break;
    }
}


bool embot::app::application::theMBDagent::Impl::addMCstatus(std::vector<embot::prot::can::Frame> &outframes)
{
    embot::prot::can::motor::polling::ControlMode ctrlmode {embot::prot::can::motor::polling::ControlMode::Idle};
    
    embot::prot::can::motor::periodic::Message_STATUS msg {};
    embot::prot::can::motor::periodic::Message_STATUS::Info info {};

    info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();
    info.controlmode = remapControlMode(amc_bldc.AMC_BLDC_Y.Flags_p.control_mode);         
    info.faultstate = (true == EXTFAULTisPRESSED) ? 0x00000001 : 0x00000000;
    // etc
        
    msg.load(info);
          
    embot::prot::can::Frame frame0;
    msg.get(frame0);
    outframes.push_back(frame0);
    
    return true;
}

// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::application::theMBDagent& embot::app::application::theMBDagent::getInstance()
{
    static theMBDagent* p = new theMBDagent();
    return *p;
}

embot::app::application::theMBDagent::theMBDagent()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::application::theMBDagent::~theMBDagent() { }
        

bool embot::app::application::theMBDagent::initialise(const Config &config)
{
    pImpl->config = config;
    return pImpl->initialise();
}

bool embot::app::application::theMBDagent::tick(std::vector<embot::prot::can::Frame> &inpframes, std::vector<embot::prot::can::Frame> &outframes)
{
    return pImpl->tick(inpframes, outframes);
}


// interface to CANagentMBD

bool embot::app::application::theMBDagent::onrecognisedframe(void *p)
{
    embot::core::print("theMBDagent is called because theCNAparserMBD has recognised a frame");
    
    return true;
}

bool embot::app::application::theMBDagent::get(const embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info &info)
{
    embot::core::print("theMBDagent (legacy mode) received EMSTO2FOC_DESIRED_CURRENT[]: " + std::to_string(info.current[0]) + ", " + std::to_string(info.current[1]) + ", " + std::to_string(info.current[2]));
    
    pImpl->pwm = info.current[0];
    pImpl->applychanges = true;
    
    return true; 
}


bool embot::app::application::theMBDagent::get(const embot::prot::can::motor::polling::Message_SET_CONTROL_MODE::Info &info)
{
    embot::core::print("theMBDagent (legacy mode) received SET_CONTROL_MODE[]: " + embot::prot::can::motor::polling::tostring(info.controlmode) + " for motindex " + embot::prot::can::motor::polling::tostring(info.motorindex));

    pImpl->cm = info.controlmode;
    pImpl->applychanges = true;
    
    return true;
}

bool embot::app::application::theMBDagent::get(const embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::Info &info, embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::ReplyInfo &replyinfo)
{    
    embot::core::print("theMBDagent (legacy mode) received GET_CONTROL_MODE[]: for motindex " + embot::prot::can::motor::polling::tostring(info.motorindex));
    replyinfo.motorindex = info.motorindex;
    replyinfo.controlmode =  pImpl->cm;
    return true;
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


