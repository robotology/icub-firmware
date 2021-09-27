
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

#include "can_decoder.h"

#include "can_rx_raw2struct.h"

#include "SupervisorFSM_RX.h"
#include "SupervisorFSM_TX.h"

#include "control_outer.h"
#include "control_foc.h"

#include "can_encoder.h"

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
		
		BUS_CAN_RX bus_can_rx;
		BUS_MESSAGES_RX bus_messages_rx;
		BUS_EVENTS_RX bus_events_rx;
		BUS_CAN_RX_ERRORS bus_can_rx_errors;
		
		SensorsData sensors_data;
		MotorSensors motor_sensors;
		
		Flags flags;
		Targets targets;
		ConfigurationParameters config_params;
		
		struct OuterOutputs {
			boolean_T velocity_enable;
			boolean_T current_enable;
			boolean_T out_enable;
			real32_T motor_current;
		};
		
		struct rtu_control_foc_T
    {
        boolean_T rtu_Flags_PID_reset = false;
        
        real32_T rtu_Config_motorconfig_Kp =    5.f;
        real32_T rtu_Config_motorconfig_Ki = 1250.f;
        
        real32_T rtu_Config_motorconfig_Kbemf  = 0.f;
        real32_T rtu_Config_motorconfig_Rphase = 0.f;
        
        real32_T rtu_Config_motorconfig_Vmax = 12.f;
        real32_T rtu_Config_motorconfig_Vcc  = 24.f;
        
        real32_T rtu_Sensors_motorsensors_Iabc[3] = {0,0,0};
        real32_T rtu_Sensors_motorsensors_angl_k = 0;
        real32_T rtu_Sensors_motorsensors_omeg_k = 0;
        uint8_T rtu_Sensors_motorsensors_hall_e  = 0;
        
        real32_T rtu_Targets_motorcurrent_curr_c = 0;
        real32_T rtu_Targets_motorvoltage_volt_e = 0;
        real32_T rtu_OuterOutputs_motorcurrent_d = 0;
        
        boolean_T rtu_OuterOutputs_vel_en = 0;
        boolean_T rtu_OuterOutputs_cur_en = 0;
        boolean_T rtu_OuterOutputs_out_en = 0;
    };
		
		struct rty_control_foc_T
    {
        uint16_T rty_Vabc_PWM_ticks[3];
        real32_T rty_Iq_fbk_current;
    };
		
		rtu_control_foc_T rtu_control_foc;
		rty_control_foc_T rty_control_foc;
		
		OuterOutputs outer_outputs;
		
		BUS_MESSAGES_TX bus_messages_tx;
		BUS_EVENTS_TX bus_events_tx;
		
		BUS_CAN output;
		
		can_messaging::CAN_RX_raw2struct can_packet_formatter;
		can_messaging::CAN_Decoder can_decoder;
		
		SupervisorFSM_RXModelClass supervisor_rx;
		SupervisorFSM_TXModelClass supervisor_tx;
		
		control_outerModelClass control_outer;
		static control_focModelClass control_foc;
		
		can_messaging::CAN_Encoder can_encoder;
		
		
		static void inner_foc_callback(int16_T Iuvw[3], void* rtu, void* rty)
    {
        rtu_control_foc_T* u = (rtu_control_foc_T*)rtu;
        rty_control_foc_T* y = (rty_control_foc_T*)rty;
         
				#warning gethallstatus not found
        //embot::hw::motor::gethallstatus(embot::hw::MOTOR::one, u->rtu_Sensors_motorsensors_hall_e);
        
        embot::hw::motor::Position electricalAngle;
        embot::hw::motor::getencoder(embot::hw::MOTOR::one, electricalAngle);
        
        u->rtu_Sensors_motorsensors_angl_k = real32_T(electricalAngle)*0.0054931640625f;
        
        static int noflood = 27000;
        if (++noflood > 28000)
        {
            noflood = 0;
            static char msg[64];
            
            sprintf(msg, "FOC %d %f\n", 
            u->rtu_Sensors_motorsensors_hall_e,u->rtu_Sensors_motorsensors_angl_k);
            
            embot::core::print(msg);
        }
        
        u->rtu_Sensors_motorsensors_Iabc[0] = 0.001f*Iuvw[0];
        u->rtu_Sensors_motorsensors_Iabc[1] = 0.001f*Iuvw[1];
        u->rtu_Sensors_motorsensors_Iabc[2] = 0.001f*Iuvw[2];
        
        #warning there is an extra argument called rtu_Sensors_motorsensors_hall_e not part of the generated function
				/*control_foc.control_foc_ISR(
            &(u->rtu_Flags_PID_reset), 
            &(u->rtu_Config_motorconfig_Kp), 
            &(u->rtu_Config_motorconfig_Ki), 
            &(u->rtu_Config_motorconfig_Kbemf), 
            &(u->rtu_Config_motorconfig_Rphase), 
            &(u->rtu_Config_motorconfig_Vmax), 
            &(u->rtu_Config_motorconfig_Vcc), 
            u->rtu_Sensors_motorsensors_Iabc[0],
            &(u->rtu_Sensors_motorsensors_angl_k),        
            &(u->rtu_Sensors_motorsensors_omeg_k), 
            &(u->rtu_Sensors_motorsensors_hall_e), 
            &(u->rtu_Targets_motorcurrent_curr_c), 
            &(u->rtu_Targets_motorvoltage_volt_e), 
            &(u->rtu_OuterOutputs_vel_en), 
            &(u->rtu_OuterOutputs_cur_en), 
            &(u->rtu_OuterOutputs_out_en), 
            &(u->rtu_OuterOutputs_motorcurrent_d), 
            y->rty_Vabc_PWM_ticks,
            &(y->rty_Iq_fbk_current)
        );*/
        
				#warning setpwmUVW not found
        // embot::hw::motor::setpwmUVW(embot::hw::MOTOR::one, y->rty_Vabc_PWM_ticks[0], y->rty_Vabc_PWM_ticks[1], y->rty_Vabc_PWM_ticks[2]);
    }
};
        

bool embot::app::application::theMBDagent::Impl::initialise()
{
    if(true == initted)
    {
        return true;
    }   

		sensors_data.jointpositions.position = 0;
		
		motor_sensors.temperature = 1;
		motor_sensors.threshold.current_high = 2;
		motor_sensors.threshold.voltage_high = 2;
		motor_sensors.threshold.temperature_high = 2;
		motor_sensors.current = 1;
		
		config_params.PosLoopPID.P = 5.f;
		config_params.PosLoopPID.I = 1250.f;
		
		config_params.VelLoopPID.P = 5.f;
		config_params.VelLoopPID.I = 1250.f;
		
		config_params.DirLoopPID.P = 5.f;
		config_params.DirLoopPID.I = 1250.f;
		
		can_decoder.init(&bus_messages_rx, &bus_events_rx, &bus_can_rx_errors);
		
		supervisor_rx.init(&flags, &targets);
		
		supervisor_tx.init(&bus_messages_tx, &bus_events_tx);
		
		can_encoder.initialize();
		
		control_outer.initialize();
		
    initted = true;        
    return initted;   
}

bool embot::app::application::theMBDagent::Impl::tick(const std::vector<embot::prot::can::Frame> &inpframes, std::vector<embot::prot::can::Frame> &outframes)
{

		uint8_t rx_data[8] {0};
		uint8_t rx_size {0};
		uint32_t rx_id {0};
		
		
		if (inpframes.size() == 0) {
				bus_can.available = 0;
		} else {
				bus_can.available = 1;
				embot::prot::can::Frame last_frame = inpframes.back();
				last_frame.copyto(rx_id, rx_size, rx_data);
		}
		
		bus_can.lengths = (uint8_T)rx_size;
		
		bus_can.packets.ID = (uint16_T)rx_id;
		
		for (int i=0;i<rx_size;i++) {
			bus_can.packets.PAYLOAD[i] = (uint8_T)rx_data[i];
		}
		
		can_packet_formatter.step(bus_can, bus_can_rx);
		
		
		can_decoder.step(bus_can_rx, bus_messages_rx, bus_events_rx, bus_can_rx_errors);
		
		
		InternalMessages internal_messages;
		
    supervisor_rx.step(internal_messages, motor_sensors, bus_events_rx, bus_messages_rx, bus_can_rx_errors, flags, targets);
		
		control_outer.step(&flags.control_mode, &flags.PID_reset, 
												&config_params.velocitylimits.limits[0],
												&config_params.motorconfig.reduction,
												&config_params.motorconfig.has_speed_sens,
												&config_params.PosLoopPID.P,
												&config_params.PosLoopPID.I,
												&config_params.PosLoopPID.D,
												&config_params.PosLoopPID.N,
												&config_params.VelLoopPID.P,
												&config_params.VelLoopPID.I,
												&config_params.VelLoopPID.D,
												&config_params.VelLoopPID.N,
												&config_params.DirLoopPID.P,
												&config_params.DirLoopPID.I,
												&config_params.DirLoopPID.D,
												&config_params.DirLoopPID.N,
												&sensors_data.jointpositions.position,
												&sensors_data.motorsensors.omega,
												&targets.jointpositions.position,
												&targets.jointvelocities.velocity,
												&outer_outputs.velocity_enable,
												&outer_outputs.current_enable,
												&outer_outputs.out_enable,
												&outer_outputs.motor_current);		
		
		sensors_data.jointpositions.position++;
		
		if (sensors_data.jointpositions.position > 2000) {
			sensors_data.jointpositions.position = 0;
		}
		
		
		supervisor_tx.step(sensors_data, bus_messages_rx, bus_messages_tx, bus_events_tx);
		
		can_encoder.step(bus_messages_tx, bus_events_tx, output);
		
//		std::string control_mode_string {"Not Configured"};
		
//		if (supervisor_control_mode == 1) {
//			control_mode_string = "Idle";
//		} else if (supervisor_control_mode == 5) {
//			control_mode_string = "Velocity";
//		}
//		
//		if (rtb_control_mode == 0) {
//		 	embot::core::print("Event Set Control Mode: False | Value of supervisor control mode variable: " + control_mode_string);
//		} else {
//		 	embot::core::print("Event Set Control Mode: True  | Value of supervisor control mode variable: " + control_mode_string);
//		}
		
    if(output.available)
    {
        embot::prot::can::Frame fr {output.packets.ID, 8, output.packets.PAYLOAD};
    
        outframes.push_back(fr);
    }
    
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


