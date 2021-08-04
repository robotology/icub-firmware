
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

#include "embot_hw_motor.h"
#include "embot_app_theCANboardInfo.h"

#include "can_decoder.h"

#include "can_rx_raw2struct.h"

#include "SupervisorFSM_RX.h"
#include "SupervisorFSM_TX.h"

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
		
				// Variables For CAN Decoder
    MCControlModes rtb_mode;
    CANErrorTypes rtb_type;
    uint16_T rtb_peak;
    uint16_T rtb_overload;
    int16_T rtb_nominal;
    int16_T rtb_current_o;
    boolean_T rtb_motor;
    boolean_T rtb_motor_e;
    boolean_T rtb_control_mode;
    boolean_T rtb_current_limit;
    boolean_T rtb_desired_current;
    boolean_T rtb_event;
		
		// Input Variables For Packet Formatter
		uint8_T formatter_available;
		uint8_T packet_length;
		uint16_T packet_ID;
		uint8_T packet_payload[8];
		
		// Output Variables For Packet Formatter/Input Variables For Decoder
		uint8_T decoder_available;
		CANClassTypes packet_CLS;
		uint8_T packet_SRC;
		uint8_T packet_DST_TYP;
		uint8_T packet_LEN;
		boolean_T packet_M;
		uint8_T packet_OPC;
		uint8_T packet_ARG[7];

		// Variables For Supervisor RX
		const BoardState board_state_gnd = BoardState_HardwareConfigured;
		const BoardCommand board_command_gnd = BoardCommand_ForceIdle;
		ControlModes supervisor_control_mode;
		boolean_T pid_reset;
		real32_T position;
		real32_T velocity;
		real32_T current;
		real32_T voltage;
		
		// JointPositions
		real32_T joint_position {0};
		
		// MotorSensors
		real32_T iabc[3] {0,0,0};
		real32_T angle {0};
		real32_T omega {0};
		real32_T temperature {1};
		real32_T motor_sensors_voltage {0};
		real32_T current_threshold {0};
		real32_T cu_k_threshold {2};
		real32_T voltage_threshold {0};
		real32_T vo_k_threshold {2};
		real32_T temperature_threshold {0};
		real32_T te_c_threshold {2};
		real32_T motor_sensors_current {1};
		
		
		// Variables For Supervisor TX
		boolean_T foc;
		real32_T foc_current;
		real32_T foc_position;
		real32_T foc_velocity;
		boolean_T event_foc;
		
		// Variables For CAN Encoder 
		uint8_T output_available;
		uint16_T output_id;
		uint8_T output_data[8];
		
		can_messaging::CAN_RX_raw2struct can_packet_formatter;
		can_messaging::CAN_Decoder can_decoder;
		
		SupervisorFSM_RXModelClass supervisor_rx;
		SupervisorFSM_TXModelClass supervisor_tx;
		
		can_messaging::CAN_Encoder can_encoder;
		
};
        

bool embot::app::application::theMBDagent::Impl::initialise()
{
    if(true == initted)
    {
        return true;
    }   

		can_decoder.init(&rtb_motor, &rtb_mode, &rtb_motor_e, &rtb_nominal,
											&rtb_peak, &rtb_overload, &rtb_current_o, &rtb_control_mode,
											&rtb_current_limit, &rtb_desired_current, &rtb_event, &rtb_type);
		
		
		supervisor_rx.init(&supervisor_control_mode, &pid_reset,
												&position, &velocity, &current, &voltage);
		
		supervisor_tx.init(&current, &position, &velocity);
		
		can_encoder.initialize();
		
    initted = true;        
    return initted;   
}

bool embot::app::application::theMBDagent::Impl::tick(const std::vector<embot::prot::can::Frame> &inpframes, std::vector<embot::prot::can::Frame> &outframes)
{

		uint8_t rx_data[8] {0};
		uint8_t rx_size {0};
		uint32_t rx_id {0};
		
		
		if (inpframes.size() == 0) {
				formatter_available = 0;
		} else {
				formatter_available = 1;
				embot::prot::can::Frame last_frame = inpframes.back();
				last_frame.copyto(rx_id, rx_size, rx_data);
		}
		
		packet_length = (uint8_T)rx_size;
		
		packet_ID = (uint16_T)rx_id;
		
		for (int i=0;i<rx_size;i++) {
			packet_payload[i] = (uint8_T)rx_data[i];
		}
		
		can_packet_formatter.step(&formatter_available, &packet_length, &packet_ID, &packet_payload[0],
															&decoder_available, &packet_CLS, &packet_SRC, &packet_DST_TYP, &packet_LEN, &packet_M, &packet_OPC, &packet_ARG[0]);
		
		
		can_decoder.step(&decoder_available, &packet_CLS, &packet_SRC, &packet_DST_TYP, &packet_LEN, &packet_M, &packet_OPC, &packet_ARG[0],
										&rtb_motor, &rtb_mode, &rtb_motor_e, &rtb_nominal, &rtb_peak, &rtb_overload, &rtb_current_o, &rtb_control_mode, &rtb_current_limit, &rtb_desired_current, &rtb_event, &rtb_type);
		
    supervisor_rx.step((const BoardState*)board_state_gnd, (const BoardCommand*)board_command_gnd,
												&iabc[0], &angle, &omega, &temperature, &motor_sensors_voltage,
												&current_threshold, &cu_k_threshold, &voltage_threshold, &vo_k_threshold, &temperature_threshold, &te_c_threshold,
												&motor_sensors_current,
												&rtb_event, &rtb_control_mode, &rtb_current_limit, &rtb_desired_current,
												&rtb_motor, &rtb_mode, &rtb_motor_e, &rtb_nominal, &rtb_peak, &rtb_overload, &rtb_current_o,
												&supervisor_control_mode, &pid_reset, &position, &velocity, &current, &voltage);
		
		joint_position=joint_position + 1;
		
		if (joint_position > 2000) {
			joint_position = 0;
		}
		
		supervisor_tx.step(&joint_position, &omega, &motor_sensors_current, &rtb_mode, 
												&foc_current, &foc_position, &foc_velocity, &event_foc);
		
		
		can_encoder.step(&foc_current, &foc_position, &foc_velocity, &event_foc,
										&output_available, &output_id, &output_data[0]);
		
		
		std::string control_mode_string {"Not Configured"};
		
		if (supervisor_control_mode == 1) {
			control_mode_string = "Idle";
		} else if (supervisor_control_mode == 5) {
			control_mode_string = "Velocity";
		}
		
		if (rtb_control_mode == 0) {
		 	embot::core::print("Event Set Control Mode: False | Value of supervisor control mode variable: " + control_mode_string);
		} else {
		 	embot::core::print("Event Set Control Mode: True  | Value of supervisor control mode variable: " + control_mode_string);
		}
		
    if(output_available)
    {
        uint8_t data[8] = {0};
        embot::prot::can::Frame fr {output_id, 8, output_data};
    
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


