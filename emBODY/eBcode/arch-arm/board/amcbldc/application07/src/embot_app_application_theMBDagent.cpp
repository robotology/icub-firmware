
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
#include "embot_core.h"
#include "embot_app_theLEDmanager.h"
#include <array>

// hal components
#include "motorhal_config.h"

// mdb components
#include "AMC_BLDC.h"

// --------------------------------------------------------------------------------------------------------------------
// - defines
// --------------------------------------------------------------------------------------------------------------------
//#define TEST_DURATION_FOC

#ifndef USE_LEGO_SETUP
#define EXTFAULT_enabled
#define EXTFAULT_handler_will_disable_motor
#endif // USE_LEGO_SETUP


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

    // the objects which measure the duration of the two above functions
    Measure *measureFOC {nullptr};
    Measure *measureTick {nullptr}; 
    
    // all the rest, which may or may not be required anymore
    Config config {};
    bool initted {false};
        
    embot::hw::motor::Pwm pwm {0};
    embot::prot::can::motor::polling::ControlMode cm {embot::prot::can::motor::polling::ControlMode::Idle};
    bool applychanges {false};
    
    static constexpr embot::hw::BTN buttonEXTfault {embot::hw::BTN::one};
    static void onEXTFAULTpressedreleased(void *owner);  
    volatile bool EXTFAULTisPRESSED {false};   
    volatile bool prevEXTFAULTisPRESSED {false};    
    volatile embot::core::Time EXTFAULTpressedtime {0};
    volatile embot::core::Time EXTFAULTreleasedtime {0};
    
    static constexpr uint8_t maxNumberOfPacketsCAN {4};  //define the max number of CAN pkt managed

    #ifdef PRINT_HISTO_DEBUG 
    void printHistogram_rxPkt(size_t cur_size); //cur_size is the currente queue size
    static constexpr uint8_t numOfBins {11}; //size of queue of the CAN drive plus 1 (in case any packet is received)
    uint32_t bin[numOfBins]{0};
    #endif

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
    AMC_BLDC_initialize();
    
    AMC_BLDC_U.ExternalFlags_p.fault_button = EXTFAULTisPRESSED;
    
    // init motor
    embot::hw::motor::init(embot::hw::MOTOR::one, {});
    
    // assign the callback to the current availability
    embot::hw::motor::setCallbackOnCurrents(embot::hw::MOTOR::one, Impl::onCurrents_FOC_innerloop, this);
    
    CAN_ID_AMC = embot::app::theCANboardInfo::getInstance().cachedCANaddress();
    
    // init motor configuration parameters from hal
    // InitConfParams.motorconfig.pole_pairs = MainConf.pwm.poles;
    //InitConfParams.motorconfig.has_hall_sens...
    //auto todo... = MainConf.encoder.nsteps = 16000;  TODO: fix (this values depends on pole_pairs and actually it is missing in mbd)
    
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
bool embot::app::application::theMBDagent::Impl::tick(std::vector<embot::prot::can::Frame> &inpframes, std::vector<embot::prot::can::Frame> &outframes)
{    
    measureTick->start();
    
    if(prevEXTFAULTisPRESSED != EXTFAULTisPRESSED)
    {
        prevEXTFAULTisPRESSED = EXTFAULTisPRESSED;  
        // and manage the transitions [pressed -> unpressed] or vice-versa and use also
        // EXTFAULTpressedtime and / or EXTFAULTreleasedtime and

        AMC_BLDC_U.ExternalFlags_p.fault_button = EXTFAULTisPRESSED;        
        
        if(true == EXTFAULTisPRESSED)
        {
            embot::app::theLEDmanager::getInstance().get(embot::hw::LED::two).on();
        }
        else
        {
            embot::app::theLEDmanager::getInstance().get(embot::hw::LED::two).off();
        }
    }
        
    // 1. reset all info 
    for (uint8_t i = 0; i < maxNumberOfPacketsCAN; i++) {
        AMC_BLDC_U.PacketsRx.packets[i].available = false;
    }
    
    // 2. check for CAN input frame
    size_t cur_size = inpframes.size();
    size_t ninputframes = cur_size;
    
    // 3. limit the number of can pkts per cycle to maxNumberOfPacketsCAN 
    if(ninputframes>maxNumberOfPacketsCAN)
        ninputframes = maxNumberOfPacketsCAN;
    
    // 4. take ninputframes frames and inset them in the supervisor input queue.
    for (uint8_t i = 0; i < ninputframes; i++) {
        
        uint32_t rx_id {0};     // frame ID
        const size_t consumedframes {1};
        
        // get the first
        embot::prot::can::Frame frame = inpframes.front();
        // copy it
        frame.copyto(rx_id, AMC_BLDC_U.PacketsRx.packets[i].length, AMC_BLDC_U.PacketsRx.packets[i].packet.PAYLOAD); 
        AMC_BLDC_U.PacketsRx.packets[i].available = true;
        AMC_BLDC_U.PacketsRx.packets[i].packet.ID = (uint16_T)rx_id;
        // clean up the first consumedframes positions
        inpframes.erase(inpframes.begin(), inpframes.begin()+consumedframes);
    }//end for
    
    // -----------------------------------------------------------------------------
    // Model Step Function (1 ms)
    // -----------------------------------------------------------------------------
    
    AMC_BLDC_step_Time();
    
    
    

    // if there an output is available, send it to the CAN Netowork
    for (uint8_t i = 0; i < maxNumberOfPacketsCAN; i++)
    {
        //if (amc_bldc.AMC_BLDC_Y.PacketsTx.packets[i].available)
        if (AMC_BLDC_Y.PacketsTx.packets[i].available)
        {
            embot::prot::can::Frame fr {AMC_BLDC_Y.PacketsTx.packets[i].packet.ID, AMC_BLDC_Y.PacketsTx.packets[i].length, AMC_BLDC_Y.PacketsTx.packets[i].packet.PAYLOAD};
            outframes.push_back(fr);
        }//end if
    } //end for
    
    uint32_t out_size = outframes.size();

    
    // If motor configuration parameters changed due to a SET_MOTOR_CONFIG message, then update hal as well (Only pole_pairs at the moment)
    // TODO: We should perform the following update inside the architectural model after a SET_MOTOR_CONFIG message has been received
    MainConf.pwm.poles = AMC_BLDC_Y.ConfigurationParameters_p.motorconfig.pole_pairs;
    
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
    embot::hw::motor::gethallstatus(embot::hw::MOTOR::one, AMC_BLDC_U.SensorsData_p.motorsensors.hallABC);
    
    // retrieve the current value of the encoder
    embot::hw::motor::Position electricalAngle {0};
    embot::hw::motor::getencoder(embot::hw::MOTOR::one, electricalAngle); 
    

    static int32_T position {0};
    static int32_T electricalAngleOld = static_cast<int32_T>(electricalAngle);
    // keep int16_t ... to manage overflow ... 
    int16_t delta = electricalAngle - electricalAngleOld;
    electricalAngleOld = electricalAngle;
    
    // calculate the current joint position
    position = position + delta / AMC_BLDC_Y.ConfigurationParameters_p.motorconfig.pole_pairs;
    
    AMC_BLDC_U.SensorsData_p.motorsensors.angle = static_cast<real32_T>(electricalAngle)*0.0054931640625f; // (60 interval angle)
    
    // convert the current from mA to A
    AMC_BLDC_U.SensorsData_p.motorsensors.Iabc[0] = 0.001f*currs.u;
    AMC_BLDC_U.SensorsData_p.motorsensors.Iabc[1] = 0.001f*currs.v;
    AMC_BLDC_U.SensorsData_p.motorsensors.Iabc[2] = 0.001f*currs.w;
    
    // -----------------------------------------------------------------------------
    // FOC Step Function (~26.6 KHz)
    // -----------------------------------------------------------------------------
    
    AMC_BLDC_step_FOC();
    
    // -----------------------------------------------------------------------------
    
    AMC_BLDC_U.SensorsData_p.jointpositions.position = static_cast<real32_T>(position) * 0.0054931640625f; // iCubDegree -> deg

    // Set the voltages
    int32_T Vabc0 = static_cast<int32_T>(AMC_BLDC_Y.ControlOutputs_p.Vabc[0] * 163.83F);
    int32_T Vabc1 = static_cast<int32_T>(AMC_BLDC_Y.ControlOutputs_p.Vabc[1] * 163.83F);
    int32_T Vabc2 = static_cast<int32_T>(AMC_BLDC_Y.ControlOutputs_p.Vabc[2] * 163.83F);
    
    embot::hw::motor::setpwm(embot::hw::MOTOR::one, Vabc0, Vabc1, Vabc2);
   
//#define DEBUG_PARAMS
#ifdef DEBUG_PARAMS
    
    static char msg2[64];
    static uint32_t counter;
    if(counter % 10 == 0)
    {
        sprintf(msg2, "%d,%d,%d,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f", \
                                 Vabc0,  \
                                 Vabc1,  \
                                 Vabc2,  \
                                 AMC_BLDC_U.SensorsData_p.motorsensors.Iabc[0],       \
                                 AMC_BLDC_U.SensorsData_p.motorsensors.Iabc[1],       \
                                 AMC_BLDC_U.SensorsData_p.motorsensors.Iabc[2],       \
                                 AMC_BLDC_U.SensorsData_p.motorsensors.angle,         \
                                 AMC_BLDC_Y.EstimatedData_p.jointvelocities.velocity, \
                                 AMC_BLDC_B.Targets_n.motorcurrent.current,           \
                                 AMC_BLDC_Y.ControlOutputs_p.Iq_fbk.current,          \
                                 AMC_BLDC_Y.ControlOutputs_p.Vq);

//        sprintf(msg2, "%d,%d,%d,%.3f,%.3f,%.3f", \
//                                 Vabc0,  \
//                                 Vabc1,  \
//                                 Vabc2,  \
//                                 AMC_BLDC_U.SensorsData_p.motorsensors.Iabc[0],       \
//                                 AMC_BLDC_U.SensorsData_p.motorsensors.Iabc[1],       \
//                                 AMC_BLDC_U.SensorsData_p.motorsensors.Iabc[2]);
        embot::core::print(msg2);
        counter = 0;
    }
    counter++;
#endif

#endif // #if defined(TEST_DURATION_FOC) 

    impl->measureFOC->stop();
}

#ifdef PRINT_HISTO_DEBUG 
void embot::app::application::theMBDagent::Impl::printHistogram_rxPkt(size_t cur_size)
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


