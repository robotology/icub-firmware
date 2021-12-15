
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_application_theMBDagent.h"


#include <string>
// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor.h"
#include "embot_app_theCANboardInfo.h"

// include mdb 
#include "AMC_BLDC.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::app::application::theMBDagent::Impl
{   
    Config config {};
    bool initted {false};
    bool already_enabled {false};
        
    embot::hw::motor::Pwm pwm {0};
    embot::prot::can::motor::polling::ControlMode cm {embot::prot::can::motor::polling::ControlMode::Idle};
    bool applychanges {false};
    
    Impl() = default;
    
    bool initialise();
    bool tick(const std::vector<embot::prot::can::Frame> &inpframes, std::vector<embot::prot::can::Frame> &outframes);
    
    static amc_bldc_codegen::AMC_BLDC amc_bldc;
     
    // Called at rate ~26.6 KHz inside the FOC controller
    static void inner_foc_callback(int16_T Iuvw[3], void* rtu, void* rty)
    {
        amc_bldc_codegen::AMC_BLDC::ExtU_AMC_BLDC_T* u = (amc_bldc_codegen::AMC_BLDC::ExtU_AMC_BLDC_T*) rtu;
        amc_bldc_codegen::AMC_BLDC::ExtY_AMC_BLDC_T* y = (amc_bldc_codegen::AMC_BLDC::ExtY_AMC_BLDC_T*) rty;
        
        // retrieve the current value of the Hall sensors 
        embot::hw::motor::gethallstatus(embot::hw::MOTOR::one, u->SensorsData_motorsensors_hallAB);
        
        // retrieve the current value of the encoder
        embot::hw::motor::Position electricalAngle;
        embot::hw::motor::getencoder(embot::hw::MOTOR::one, electricalAngle); 
        
        static int32_T position;
        static int32_T electricalAngleOld =  static_cast<real32_T>(electricalAngle);
        int16_t delta = electricalAngle - electricalAngleOld;
        electricalAngleOld = electricalAngle;
        
        // calculate the current joint position
        position = position + delta / 4;  // the motor has 8 poles, hence 4 pole pairs
        
        u->SensorsData_motorsensors_angle = static_cast<real32_T>(electricalAngle)*0.0054931640625f; // (60 interval angle)
        
        // convert the current from mA to A
        u->SensorsData_motorsensors_Iabc[0] = 0.001f*Iuvw[0];
        u->SensorsData_motorsensors_Iabc[1] = 0.001f*Iuvw[1];
        u->SensorsData_motorsensors_Iabc[2] = 0.001f*Iuvw[2];
        
        // -----------------------------------------------------------------------------
        // FOC Step Function (~26.6 KHz)
        // -----------------------------------------------------------------------------
        
        amc_bldc.step_FOC();
        
        // save sensor data
        //u->SensorsData_motorsensors_curren = y->ControlOutputs_p.Iq_fbk.current * 1000; // *0.001f in order to have mA
        
#warning workaround. Fix this parameter in model design. 
    
        u->SensorsData_jointpositions_posi = static_cast<real32_T>(position) * 0.0054931640625f; // iCubDegree -> deg
        //sensors_data.motorsensors.omega =  speed * 80 / 3 * 0.0054931640625f; // Frequency = 80/3. Angular velocity --> iCubDegree/ms
                                                                                // * 0.0054931640625f ==> icubDegree -> Deg
        
        // Set the voltages
        int32_T Vabc0 = static_cast<int32_T>(y->ControlOutputs_p.Vabc[0] * 163.83F);
        int32_T Vabc1 = static_cast<int32_T>(y->ControlOutputs_p.Vabc[1] * 163.83F);
        int32_T Vabc2 = static_cast<int32_T>(y->ControlOutputs_p.Vabc[2] * 163.83F);
        
        embot::hw::motor::setpwmUVW(embot::hw::MOTOR::one, Vabc0, Vabc1, Vabc2);
       
#define DEBUG_PARAMS // TODO: remove
#ifdef DEBUG_PARAMS
        
        static char msg2[64];
        static uint32_t counter;
        if(counter % 10 == 0)
        {
            //sprintf(msg2, "%d -- %d -- %.3f -- %.3f\n", delta, position, u->SensorsData_motorsensors_angle, y->ControlOutputs_p.Iq_fbk.current);
            sprintf(msg2, "%d,%d,%d,%.3f,%.3f,%.3f,%.3f,%.3f", \
                                     Vabc0,  \
                                     Vabc1,  \
                                     Vabc2,  \
                                     u->SensorsData_motorsensors_Iabc[0],  \
                                     u->SensorsData_motorsensors_Iabc[1],  \
                                     u->SensorsData_motorsensors_Iabc[2],  \
                                     u->SensorsData_motorsensors_angle,    \
                                     y->ControlOutputs_p.Iq_fbk.current);
            
            //sprintf(msg2, "%d,%d,%.3f,%.3f,%.3f", delta, position, y->EstimatedData_p.jointvelocities.velocity, u->SensorsData_motorsensors_angle, y->ControlOutputs_p.Iq_fbk.current);
            //embot::core::print(msg2);
            counter = 0;
        }
        counter++;
#endif
    }
};


bool embot::app::application::theMBDagent::Impl::initialise()
{
    if(true == initted)
    {
        return true;
    }
    
    amc_bldc.initialize();
    
    // init motor
    embot::hw::motor::init(embot::hw::MOTOR::one, {});
    
    embot::hw::motor::setADCcallback(embot::hw::MOTOR::one, inner_foc_callback, &amc_bldc.AMC_BLDC_U , &amc_bldc.AMC_BLDC_Y);
    
    initted = true;
    already_enabled = false;

    return initted;
}

// Called every 1 ms
bool embot::app::application::theMBDagent::Impl::tick(const std::vector<embot::prot::can::Frame> &inpframes, std::vector<embot::prot::can::Frame> &outframes)
{
    uint8_t rx_data[8] {0};
    uint8_t rx_size {0};
    uint32_t rx_id {0};
    
    // check for CAN input frame
    if (inpframes.size() == 0) 
    {
        amc_bldc.AMC_BLDC_U.PacketsRx_available = 0;
    } 
    else
    {
        // retrieve the CAN frame
        amc_bldc.AMC_BLDC_U.PacketsRx_available = 1;
        embot::prot::can::Frame last_frame = inpframes.back();
        last_frame.copyto(rx_id, rx_size, rx_data);
    }
    
    // save the CAN frame into the input structure of the model
    amc_bldc.AMC_BLDC_U.PacketsRx_lengths = (uint8_T)rx_size;
    amc_bldc.AMC_BLDC_U.PacketsRx_packets_ID = (uint16_T)rx_id;
    
    for (int i=0;i<rx_size;i++) 
    {
        amc_bldc.AMC_BLDC_U.PacketsRx_packets_PAYLOAD[i] = (uint8_T)rx_data[i];
    }
    
    
    #warning: Temp workaround, duplicated logic for out_en (see inside OuterController)
        
        
    if(amc_bldc.AMC_BLDC_B.Flags_k.control_mode == ControlModes_NotConfigured     || \
             amc_bldc.AMC_BLDC_B.Flags_k.control_mode == ControlModes_Idle        || \
             amc_bldc.AMC_BLDC_B.Flags_k.control_mode == ControlModes_HwFaultCM)
    {
        embot::hw::motor::motorDisable(embot::hw::MOTOR::one);
        already_enabled = false;
    }
    else
    {
        if(!already_enabled){
            embot::hw::motor::motorEnable(embot::hw::MOTOR::one);
        }
        already_enabled = true;
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
    
    return true;
}

amc_bldc_codegen::AMC_BLDC embot::app::application::theMBDagent::Impl::amc_bldc;

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

bool embot::app::application::theMBDagent::tick(const std::vector<embot::prot::can::Frame> &inpframes, std::vector<embot::prot::can::Frame> &outframes)
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


