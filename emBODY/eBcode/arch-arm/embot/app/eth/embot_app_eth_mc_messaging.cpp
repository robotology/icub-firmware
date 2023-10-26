
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------


#include "embot_app_eth_mc_messaging.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

// for embot
#include "embot_prot_can.h"


// for embOBJ
#include "EoProtocol.h"
#include "EOtheCANprotocol.h"
#include "EOtheCANprotocol_functions.h" 

// --------------------------------------------------------------------------------------------------------------------
// - implementation 
// --------------------------------------------------------------------------------------------------------------------
 

namespace embot::app::eth::mc::messaging {
    
    void Location::load(const eObrd_location_t &l)
    {
        uint8_t place {l.any.place};
        if((eobrd_place_can == place) || (eobrd_place_extcan == place))
        {   // i decide to lose the .index (0 or 1) because it is barely used 
            bus = (eOcanport1 == l.can.port) ? BUS::can1 : BUS::can2;
            address = l.can.addr;                
        }
        else if(eobrd_place_eth == place)
        {   // id is 6 bits, so in range [0, 63]. 
            // we may use two bits to choose which ICC bus and four bits for address. BUT NOT NOW
            bus = BUS::icc1;
            address = l.eth.id;               
        }
    }        

} // namespace embot::app::eth::mc::messaging {


namespace embot::app::eth::mc::messaging::sender {

    // it loads the parameters of a specific command and generates what is required to transmit over can or icc
    struct Command
    {
        using Former = eOresult_t (*)(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
        
        Location destination {};
        eOcanprot_msgclass_t cls {eocanprot_msgclass_pollingMotorControl};
        uint8_t cmd {ICUBCANPROTO_POL_MC_CMD__NO_MESSAGE};
        void *value {nullptr};        
        Former former {nullptr};
                
        Command() = default; 
        Command(const Location &d, eOcanprot_msgclass_t c, uint8_t co, void *v, Former f = dummyformer) 
            : destination(d), cls(c), cmd(co), value(v), former((nullptr == f) ? dummyformer : f) {}  
                
        void clear()
        {
            destination = {}; 
            cls = eocanprot_msgclass_pollingMotorControl; 
            cmd = ICUBCANPROTO_POL_MC_CMD__NO_MESSAGE;
            value = nullptr;
            former = nullptr;                
        }    

        void load(const Location &d, eOcanprot_msgclass_t c, uint8_t co, void *v, Former f = dummyformer)
        {
            destination = d; 
            cls = c; 
            cmd = co;
            value = v;
            former = (nullptr == f) ? dummyformer : f; 
        }            

        bool isvalid() const { 
            volatile bool dd = destination.isvalid();
            volatile bool vv = nullptr != value;
            volatile bool ff = nullptr != former;
            return dd && vv && ff;
        }         
              

        bool transmit() 
        { 
            if(false == isvalid())
            {
                return false;
            }                
            return transmit(destination, cancommand(), former);       
        }; 
        
        eObrd_canlocation_t canlocation() const { return tocanlocation(destination); }
        eOcanprot_command_t cancommand() const { return tocancommand(cls, cmd, value); }
        
        static eOresult_t dummyformer(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame) { return eores_OK; }
//#warning siamo sicuri he abbia un senso il tocanlocation(0 per un ICC ???
        static eObrd_canlocation_t tocanlocation(const Location &des)
        {
            eObrd_canlocation_t canloc {};
            if(true == des.isCAN())
            {            
                canloc.port = (Location::BUS::can1 == des.bus) ? eOcanport1 : eOcanport2;   
                canloc.addr = des.address;
                canloc.insideindex = eobrd_caninsideindex_first;
            }
            else if(true == des.isICC())
            {            
                canloc.port = (Location::BUS::icc1 == des.bus) ? eOcanport1 : eOcanport2;   
                canloc.addr = des.address;
                canloc.insideindex = eobrd_caninsideindex_first;
            }
            return canloc;
        } 
        
        static eOcanprot_command_t tocancommand(eOcanprot_msgclass_t c, uint8_t co, void *v)
        {
           return {static_cast<uint8_t>(c), co, {0, 0}, v}; 
        }
        
        static embot::prot::can::Frame toprotcanframe(const Location &des, const eOcanprot_command_t &c, Former f)
        {
            eOcanprot_descriptor_t cd {c, tocanlocation(des)};           
            eOcanframe_t cf {0};               
            f(&cd, &cf);               
            return {cf.id, cf.size, cf.data};
        }
               
        static bool transmit(const Location &des, const eOcanprot_command_t &cmd, Former f)
        {
            bool r {false};
            if(true == des.isCAN())
            {
                r = tx2can(tocanlocation(des), cmd);
            }
            else if(true == des.isICC())
            {
                embot::prot::can::Frame frame = toprotcanframe(des, cmd, f);
                r = tx2icc(des, frame);
            }  

            return r;            
        }
        
        static bool tx2can(const eObrd_canlocation_t &canloc, const eOcanprot_command_t &command);    
        static bool tx2icc(const Location &des, const embot::prot::can::Frame &frame);
                
    };


} // namespace embot::app::eth::mc::messaging::sender {


namespace embot::app::eth::mc::messaging::sender {
            
    bool Set_Control_Mode::transmit()
    {
        Command command 
        {
            destination, 
            eocanprot_msgclass_pollingMotorControl, 
            ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE, 
            const_cast<void*>(reinterpret_cast<const void*>(&info)), 
            eocanprotMCpolling_former_POL_MC_CMD__SET_CONTROL_MODE
        };
        
        return command.transmit();  
    }                    

    bool Set_Current_Limit::transmit()
    {
        Command command 
        {
            destination, 
            eocanprot_msgclass_pollingMotorControl, 
            ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT, 
            const_cast<void*>(reinterpret_cast<const void*>(&info)), 
            eocanprotMCpolling_former_POL_MC_CMD__SET_CURRENT_LIMIT
        };
        
        return command.transmit();  
    }    
    
    bool Set_Temperature_Limit::transmit()
    {
        Command command 
        {
            destination, 
            eocanprot_msgclass_pollingMotorControl, 
            ICUBCANPROTO_POL_MC_CMD__SET_TEMPERATURE_LIMIT, 
            const_cast<void*>(reinterpret_cast<const void*>(&info)), 
            eocanprotMCpolling_former_POL_MC_CMD__SET_TEMPERATURE_LIMIT
        };
        
        return command.transmit();  
    }
    
    bool Set_Current_PID::transmit()
    {
        Command command 
        {
            destination, 
            eocanprot_msgclass_pollingMotorControl, 
            ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID, 
            const_cast<void*>(reinterpret_cast<const void*>(&info)), 
            eocanprotMCpolling_former_POL_MC_CMD__SET_CURRENT_PID
        };
        
        return command.transmit();  
    }  

    bool Set_Velocity_PID::transmit()
    {
        Command command 
        {
            destination, 
            eocanprot_msgclass_pollingMotorControl, 
            ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PID, 
            const_cast<void*>(reinterpret_cast<const void*>(&info)), 
            eocanprotMCpolling_former_POL_MC_CMD__SET_VELOCITY_PID
        };
        
        return command.transmit();  
    }   

    bool Set_Motor_Config::transmit()
    {
        Command command 
        {
            destination, 
            eocanprot_msgclass_pollingMotorControl, 
            ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_CONFIG, 
            const_cast<void*>(reinterpret_cast<const void*>(&info)), 
            eocanprotMCpolling_former_POL_MC_CMD__SET_MOTOR_CONFIG
        };
        
        return command.transmit();  
    }


    bool Calibrate_Encoder::transmit()
    {
        Command command 
        {
            destination, 
            eocanprot_msgclass_pollingMotorControl, 
            ICUBCANPROTO_POL_MC_CMD__CALIBRATE_ENCODER, 
            const_cast<void*>(reinterpret_cast<const void*>(&info)), 
            eocanprotMCpolling_former_POL_MC_CMD__CALIBRATE_ENCODER
        };
        
        return command.transmit();  
    }    

    bool Actuate_Motors::transmit()
    {
        bool ret {false};
        
#if 1
        // this is the version that enables to map the Actuate_Motors::maxnumberofmotors (4) values into ...
        Location destinations[Location::numberofBUSes] {};
        int16_t payloads[Location::numberofBUSes][Actuate_Motors::maxnumberofmotors] = {0}; 

        // in here i go through the actuations and i fill the destinations and the payloads for every bus
        for(uint8_t i=0; i<actuations.size(); i++)
        {
            if((true == actuations[i].destination.isvalid()) && (actuations[i].destination.address > 0) && (actuations[i].destination.address <= Actuate_Motors::maxnumberofmotors) )
            {
                uint8_t pos = embot::core::tointegral(actuations[i].destination.bus);
                destinations[pos] = {actuations[i].destination.bus, sourceaddress}; // for the streaming we must pass the source, not the destination
                payloads[pos][actuations[i].destination.address-1] = actuations[i].value;               
            }
        }

        // in here for every valid destination i ...
        bool r {true};
        for(uint8_t d=0; d<Location::numberofBUSes; d++)
        {
            if(true == destinations[d].isvalid())
            {
                Command command 
                {
                    destinations[d], // the source is zero
                    eocanprot_msgclass_periodicMotorControl, 
                    ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT, 
                    reinterpret_cast<void*>(payloads[d]), 
                    eocanprotMCperiodic_former_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT 
                    // for CAN we dont need the former: eocanprotMCperiodic_former_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT  
                    // but we need it for the icc                     
                };
                ret = command.transmit();
            }                       
        }
        
        return ret;
#else        
        // bus must be equal for all of them. but we can choose if can1 or can2
        Location CANdestination{}; // address is not used because it is a broadcast message
        int16_t CANdata[4] = {0};        
        
        // iccbus must be equal for all of them.
        Location ICCdestination {}; // address is not used because it is a broadcast message        
        int16_t ICCdata[4] = {0};

        
        // in here i go through the actuations and i fill the payload for the can and the icc
        for(uint8_t i=0; i<actuations.size(); i++)
        {
            if(true == actuations[i].destination.isCAN())
            {
                // bus must be equal for all of them. so, we choose the last one. address is dont care
                CANdestination.bus = actuations[i].destination.bus;
                CANdata[actuations[i].destination.address-1] = actuations[i].value;
            }
            else if(true == actuations[i].destination.isICC())
            {
                // bus must be equal for all of them. so, we choose the last one. address is dont care
                ICCdestination.bus = actuations[i].destination.bus;
                ICCdata[actuations[i].destination.address-1] = actuations[i].value;
            }
        }

        // ok, now i process can and icc
        

        
        // use Command ...
        Command command {};
        
        if(true == CANdestination.isvalid())
        {
            // ok, we have some data to send to CAN
            
            command.load
            (
                CANdestination,
                eocanprot_msgclass_periodicMotorControl, 
                ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT, 
                reinterpret_cast<void*>(CANdata), 
                nullptr // for CAN we dont need the former: eocanprotMCperiodic_former_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT            
            );
                        
            ret = command.transmit();             
        }
 
        if(true == ICCdestination.isvalid())
        {
            // ok, we have some data to send to ICC
            command.load
            (
                ICCdestination,
                eocanprot_msgclass_periodicMotorControl, 
                ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT, 
                reinterpret_cast<void*>(ICCdata), 
                eocanprotMCperiodic_former_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT            
            );
                        
            ret = command.transmit();              
        } 

        return ret;
#endif        
    }        
   

} // namespace embot::app::eth::mc::messaging::sender {

 
#include "EOtheCANservice.h"

namespace embot::app::eth::mc::messaging::sender {
    
    bool Command::tx2can(const eObrd_canlocation_t &canloc, const eOcanprot_command_t &command)
    {
        return eores_OK == eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), const_cast<eOcanprot_command_t*>(&command), canloc);               
    }

    bool Command::tx2icc(const Location &des, const embot::prot::can::Frame &frame)
    {
        //#warning TODO: fill embot::app::eth::mc::messaging::sender::Command::tx2icc() which adds a frame in the ICC queue
        embot::core::print(frame.to_string());
        return true;
    } 
        
} // namespace embot::app::eth::mc::messaging::sender {


#include "Controller.h"

namespace embot::app::eth::mc::messaging::receiver {
    
    // in here we have objects which receive an info and send it to a destination
    
    void sigFOCstatus1::load(const Location::BUS bus, const embot::prot::can::Frame &frame,  bool andprocess)
    {
        source.bus = bus;
        source.address = (frame.id & 0x0f0) >> 4;
        info.current = ((int16_t*)frame.data)[0];
        info.velocity = ((int16_t*)frame.data)[1];
        info.position = ((int32_t*)frame.data)[1];
        
        if(false == andprocess)
        {
            return;
        }
        
        // cannot use eocanprotMCperiodic_parser_PER_MC_MSG__2FOC() because ... inside we have s_eocanprotMCperiodic_get_entity() that uses the can mapping
        // so, we need an object that tells us the motorindex given the source. We should fill the map {Location, index} somewhere in EOtheMotionController.c
        // for now we dont need it, so ... we dont do any new code
  
        eOprotIndex_t motorindex = 0; // for example ...
        eOmc_motor_t *motor = reinterpret_cast<eOmc_motor_t*>(eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, motorindex));
        motor->status.basic.mot_current = info.current;
        motor->status.basic.mot_velocity = info.velocity;
        motor->status.basic.mot_position = info.position;
        MController_update_motor_odometry_fbk_can(motorindex, (void*)frame.data);
    }
    
    void sigFOCstatus2::load(const Location::BUS bus, const embot::prot::can::Frame &frame,  bool andprocess)
    {
        source.bus = bus;
        source.address = (frame.id & 0x0f0) >> 4;
        info.controlmode = frame.data[0];
        info.qencflags = frame.data[1];
        info.pwmfeedback = ((int16_t*)frame.data)[2];
        info.motorfaultflags = ((uint32_t*)frame.data)[4];
        
        if(false == andprocess)
        {
            return;
        }
        
        // cannot use eocanprotMCperiodic_parser_PER_MC_MSG__STATUS() because ... inside we have s_eocanprotMCperiodic_get_entity() that uses the can mapping
        // so, we need an object that tells us the motorindex given the source. We should fill the map {Location, index} somewhere in EOtheMotionController.c
        // for now we dont need it, so ... we dont do any new code
        
        eOprotIndex_t motorindex = 0; // for example ...
        MController_update_motor_state_fbk(motorindex, (void*)frame.data);
    }    

} // namespace embot::app::eth::mc::messaging::receiver {

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


