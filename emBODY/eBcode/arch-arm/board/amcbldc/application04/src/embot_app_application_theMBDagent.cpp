
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

// include mdb components
#include "can_rx_raw2struct.h"
#include "can_decoder.h"
#include "estimation_velocity.h"
#include "SupervisorFSM_RX.h"
#include "control_outer.h"
#include "control_foc.h"
#include "SupervisorFSM_TX.h"
#include "can_encoder.h"


// TODO: remove
#include "encoder.h"
#include "pwm.h"
// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::app::application::theMBDagent::Impl
{   
    Config config {};
    bool initted {false};
        
    embot::hw::motor::Pwm pwm {0};
    embot::prot::can::motor::polling::ControlMode cm {embot::prot::can::motor::polling::ControlMode::Idle};
    bool applychanges {false};
    
    Impl() = default;
    
    bool initialise();
    bool tick(const std::vector<embot::prot::can::Frame> &inpframes, std::vector<embot::prot::can::Frame> &outframes);
        
    BUS_CAN bus_can;
    BUS_CAN output;
    BUS_CAN_RX bus_can_rx;
    BUS_MESSAGES_RX bus_messages_rx;
    BUS_MESSAGES_TX bus_messages_tx;
    BUS_EVENTS_RX bus_events_rx;
    BUS_EVENTS_TX bus_events_tx;
    BUS_CAN_RX_ERRORS bus_can_rx_errors;
    
    ControlOutputs control_outputs;              // from supervisor to control
    
    Flags flags;
    Targets targets;
    ConfigurationParameters config_params;
        
    struct FOC_inputs
    {
        Flags flags;
        ConfigurationParameters config_params;
        Targets targets;
        SensorsData sensors_data;
        EstimatedData estimated_data;
        ControlOuterOutputs control_outer_outputs;   // from control_outer to control_foc

    };
    
    struct FOC_outputs
    {
        ControlOutputs foc_outputs;
    };
    
    amc_bldc_codegen::CAN_RX_raw2struct can_packet_formatter;
    amc_bldc_codegen::CAN_Decoder can_decoder;
    amc_bldc_codegen::estimation_velocity estimator;
    amc_bldc_codegen::SupervisorFSM_RX supervisor_rx;
    amc_bldc_codegen::SupervisorFSM_TX supervisor_tx;
    amc_bldc_codegen::control_outer control_outer;
    amc_bldc_codegen::CAN_Encoder can_encoder;
    
    static amc_bldc_codegen::control_foc control_foc;
    static FOC_inputs foc_inputs;
    static FOC_outputs foc_outputs;
    static bool motor_enabled_prev;
    
    // Called every 1 ms inside the FOC controller
    static void inner_foc_callback(int16_T Iuvw[3], void* rtu, void* rty)
    {
        FOC_inputs* u = (FOC_inputs*)rtu;
        FOC_outputs* y = (FOC_outputs*)rty;
        
        embot::hw::motor::gethallstatus(embot::hw::MOTOR::one, u->sensors_data.motorsensors.hallABC);
        
        embot::hw::motor::Position electricalAngle;
        embot::hw::motor::getencoder(embot::hw::MOTOR::one, electricalAngle); 
        
        static int32_T position;
        static int32_T electricalAngleOld =  static_cast<real32_T>(electricalAngle);
        int16_t delta = electricalAngle - electricalAngleOld;
        electricalAngleOld = electricalAngle;

        position = (position + delta) / 4; // TODO: clarify (position + delta) / 4 or position + (delta / 4)?
        
        u->sensors_data.motorsensors.angle = static_cast<real32_T>(electricalAngle)*0.0054931640625f; // (60 interval angle)
        
        // convert the current from mA to A
        u->sensors_data.motorsensors.Iabc[0] = 0.001f*Iuvw[0];
        u->sensors_data.motorsensors.Iabc[1] = 0.001f*Iuvw[1];
        u->sensors_data.motorsensors.Iabc[2] = 0.001f*Iuvw[2];
        
        Targets temp_targets = u->targets;
        temp_targets.motorcurrent.current *= 0.001F;  // TODO remove: temp fix (prescale from mA to A the current value)
              
        control_foc.control_foc_ISR(
            &(u->flags),
            &(u->config_params),
            &(u->sensors_data),
            &(u->estimated_data),
            &(temp_targets),
            &(u->control_outer_outputs),
            &(y->foc_outputs));
              
        // save sensor data
        //sensors_data.motorsensors.current = y->Iq_fbk_current*1000; // *0.001f in order to have A
        //sensors_data.motorsensors.angle = static_cast<real32_T>(electricalAngle * 0.0054931640625f);
        
#warning workaround. Fix this parameter in model design. 
    
        u->sensors_data.jointpositions.position = static_cast<real32_T>(position) * 0.0054931640625f; // iCubDegree -> deg
        //sensors_data.motorsensors.omega =  speed * 80 / 3 * 0.0054931640625f; // Frequency = 80/3. Angular velocity --> iCubDegree/ms
                                                                              // * 0.0054931640625f ==> icubDegree -> Deg
        // enable/disable the motor
        bool output_enable_b = static_cast<bool>(u->control_outer_outputs.out_en);
        if(motor_enabled_prev != output_enable_b)
        {
            if(output_enable_b)
            {
                embot::hw::motor::motorEnable(embot::hw::MOTOR::one);
            }
            else
            {
                embot::hw::motor::motorDisable(embot::hw::MOTOR::one);
            }
        }
        
        motor_enabled_prev = output_enable_b;
        
        // Set the voltages
        int32_T Vabc0 = static_cast<int32_T>(y->foc_outputs.Vabc[0] * 163.83F);
        int32_T Vabc1 = static_cast<int32_T>(y->foc_outputs.Vabc[1] * 163.83F);
        int32_T Vabc2 = static_cast<int32_T>(y->foc_outputs.Vabc[2] * 163.83F);
        
        embot::hw::motor::setpwmUVW(embot::hw::MOTOR::one, Vabc0, Vabc1, Vabc2);
       
#define DEBUG_PARAMS // TODO: remove
#ifdef DEBUG_PARAMS
        
        static char msg2[64];
        static uint32_t counter;
        if(counter % 1000 == 0)
        {
            //sprintf(msg2, "[%.3f -- %.3f -- %.3f]\n", sensors_data.jointpositions.position, u->sensors_data.motorsensors.current, y->foc_outputs.Iq_fbk.current);
            //sprintf(msg2, "%d -- %d -- %.3f -- %.3f -- [%d, %d, %d]\n", u->flags.control_mode, u->control_outer_outputs.out_en, foc_inputs.targets.motorcurrent.current, y->foc_outputs.Iq_fbk.current,Vabc0,Vabc1,Vabc2);
            //sprintf(msg2, "%.3f -- %d -- %.3f -- %.3f -- %.3f]\n", hallGetAngle()/182.044, encoderGetCounter(), foc_inputs.targets.motorcurrent.current, y->foc_outputs.Iq_fbk.current, u->sensors_data.motorsensors.angle);
            
            sprintf(msg2, "%d -- %d -- %.3f -- %.3f -- %.3f]\n", delta, position, u->estimated_data.jointvelocities.velocity, u->sensors_data.motorsensors.angle, y->foc_outputs.Iq_fbk.current);
            
            // TODO: print the current as well :) 

            embot::core::print(msg2);
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
    
    motor_enabled_prev = false;
    
    foc_inputs.config_params.motorconfig.Kp = 2;
    foc_inputs.config_params.motorconfig.Ki = 500;
    foc_inputs.config_params.motorconfig.Vmax = 9;
    foc_inputs.config_params.motorconfig.Vcc = 24;
    
    foc_inputs.sensors_data.jointpositions.position = 0;
    foc_inputs.sensors_data.motorsensors.temperature = 1;
    
//        config_params.PosLoopPID.P = 5.f;
//        config_params.PosLoopPID.I = 1250.f;
//        
//        config_params.VelLoopPID.P = 5.f;
//        config_params.VelLoopPID.I = 1250.f;
//        
//        config_params.DirLoopPID.P = 5.f;
//        config_params.DirLoopPID.I = 1250.f;
        
    can_decoder.init(&bus_messages_rx, &bus_events_rx, &bus_can_rx_errors);
    
    supervisor_rx.init(&flags, &targets, &config_params);
    
    supervisor_tx.init(&bus_messages_tx, &bus_events_tx);
    
    can_encoder.initialize();
    
    // init motor
    embot::hw::motor::init(embot::hw::MOTOR::one, {});
    
    embot::hw::motor::setADCcallback(embot::hw::MOTOR::one, inner_foc_callback, &foc_inputs, &foc_outputs);

    control_outer.initialize();

    control_foc.initialize();
    
    initted = true;

    return initted;
}

bool embot::app::application::theMBDagent::Impl::tick(const std::vector<embot::prot::can::Frame> &inpframes, std::vector<embot::prot::can::Frame> &outframes)
{
    uint8_t rx_data[8] {0};
    uint8_t rx_size {0};
    uint32_t rx_id {0};
    
    // check for CAN input frame
    if (inpframes.size() == 0) 
    {
        bus_can.available = 0;
    } 
    else 
    {
        bus_can.available = 1;
        embot::prot::can::Frame last_frame = inpframes.back();
        last_frame.copyto(rx_id, rx_size, rx_data);
    }
    
    bus_can.lengths = (uint8_T)rx_size;
    bus_can.packets.ID = (uint16_T)rx_id;
    
    for (int i=0;i<rx_size;i++) 
    {
        bus_can.packets.PAYLOAD[i] = (uint8_T)rx_data[i];
    }
    
    // -----------------------------------------------------------------------------
    // Step functions
    // -----------------------------------------------------------------------------
    can_packet_formatter.step(bus_can, bus_can_rx);
    
    can_decoder.step(bus_can_rx, bus_messages_rx, bus_events_rx, bus_can_rx_errors);
    
    estimator.step(foc_inputs.sensors_data, foc_inputs.estimated_data);
    
    supervisor_rx.step(bus_events_rx,
                       bus_messages_rx,
                       bus_can_rx_errors,
                       foc_inputs.estimated_data,
                       foc_inputs.sensors_data,
                       control_outputs,
                       targets,
                       config_params,
                       flags);
    
    #warning Actually the CURRENT CONTROL MODE is forced
    flags.control_mode = ControlModes_Current; // TODO: temp fix remove
        
    //static char msg2[64];
    //sprintf(msg2, "[%.3f, %.3f -- %d, %d ------- %.3f]\n", targets.motorcurrent.current, targets.motorvoltage.voltage, bus_can_rx_errors.event, bus_events_rx.desired_current, sensors_data.motorsensors.current);
    //        sprintf(msg2, "[%d, %d]\n", flags.control_mode, outer_outputs.out_enable);
    //embot::core::print(msg2);
    
    control_outer.step(flags,
                       config_params,
                       targets,
                       foc_inputs.sensors_data,
                       foc_inputs.estimated_data,
                       foc_inputs.control_outer_outputs);
        
    foc_inputs.flags = flags;
    foc_inputs.targets = targets;
    
    supervisor_tx.step(bus_messages_rx, foc_inputs.sensors_data, foc_inputs.estimated_data, control_outputs, bus_messages_tx, bus_events_tx);

    can_encoder.step(bus_messages_tx, bus_events_tx, output);
    
    // if there is an output available, send it through the CAN
    if(output.available)
    {
        embot::prot::can::Frame fr {output.packets.ID, 8, output.packets.PAYLOAD};
        outframes.push_back(fr);
    }
    
    return true;
}

bool embot::app::application::theMBDagent::Impl::motor_enabled_prev;
amc_bldc_codegen::control_foc embot::app::application::theMBDagent::Impl::control_foc;
embot::app::application::theMBDagent::Impl::FOC_inputs embot::app::application::theMBDagent::Impl::foc_inputs;
embot::app::application::theMBDagent::Impl::FOC_outputs embot::app::application::theMBDagent::Impl::foc_outputs;
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


