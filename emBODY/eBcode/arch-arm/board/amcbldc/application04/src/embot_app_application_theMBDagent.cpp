
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
		
		BUS_CAN bus_can;
		
		BUS_CAN_RX bus_can_rx;
		BUS_MESSAGES_RX bus_messages_rx;
		BUS_EVENTS_RX bus_events_rx;
		BUS_CAN_RX_ERRORS bus_can_rx_errors;
		
		SensorsData sensors_data;
		MotorSensors motor_sensors;
		
		Flags flags;
		Targets targets;
		
		BUS_MESSAGES_TX bus_messages_tx;
		BUS_EVENTS_TX bus_events_tx;
		
		BUS_CAN output;
		
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

		sensors_data.jointpositions.position = 0;
		
		motor_sensors.temperature = 1;
		motor_sensors.threshold.current_high = 2;
		motor_sensors.threshold.voltage_high = 2;
		motor_sensors.threshold.temperature_high = 2;
		motor_sensors.current = 1;
		
		
		can_decoder.init(&bus_messages_rx, &bus_events_rx, &bus_can_rx_errors);
		
		supervisor_rx.init(&flags, &targets);
		
		supervisor_tx.init(&bus_messages_tx, &bus_events_tx);
		
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


