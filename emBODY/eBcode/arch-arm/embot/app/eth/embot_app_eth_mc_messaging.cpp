
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


#include "EOtheCANservice.h"
#if defined(USE_ICC_COMM) 
#include "embot_app_eth_theICCservice.h"
#include "embot_app_eth_icc_ItemCANframe.h"
#include "embot_app_eth_theICCserviceCAN.h"
#endif

// --------------------------------------------------------------------------------------------------------------------
// - implementation 
// --------------------------------------------------------------------------------------------------------------------


// #define MESSAGING_DEBUG_PRINT

#if defined(MESSAGING_DEBUG_PRINT)
#include "embot_app_eth_theErrorManager.h"
#include "embot_os_theScheduler.h"
#endif

namespace embot::app::eth::mc::messaging::sender {

    // it loads the parameters of a specific command and generates what is required to transmit over can or icc
    struct Command
    {
        using Former = eOresult_t (*)(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);
        
        embot::app::msg::Location destination {};
        eOcanprot_msgclass_t cls {eocanprot_msgclass_pollingMotorControl};
        uint8_t cmd {ICUBCANPROTO_POL_MC_CMD__NO_MESSAGE};
        void *value {nullptr};        
        Former former {nullptr};
                
        Command() = default; 
        Command(const embot::app::msg::Location &d, eOcanprot_msgclass_t c, uint8_t co, void *v, Former f = dummyformer) 
            : destination(d), cls(c), cmd(co), value(v), former((nullptr == f) ? dummyformer : f) {}  
                
        void clear()
        {
            destination = {}; 
            cls = eocanprot_msgclass_pollingMotorControl; 
            cmd = ICUBCANPROTO_POL_MC_CMD__NO_MESSAGE;
            value = nullptr;
            former = nullptr;                
        }    

        void load(const embot::app::msg::Location &d, eOcanprot_msgclass_t c, uint8_t co, void *v, Former f = dummyformer)
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

        static eObrd_canlocation_t tocanlocation(const embot::app::msg::Location &des)
        {
            eObrd_canlocation_t canloc {};
            if(true == des.isCAN())
            {            
                canloc.port = (embot::app::msg::BUS::can1 == des.getbus()) ? eOcanport1 : eOcanport2;   
                canloc.addr = des.getadr();
                canloc.insideindex = eobrd_caninsideindex_first;
            }
            else if(true == des.isICC())
            {   
                // marco.accame on 13feb2024: we need to use a eObrd_canlocation_t also for ICC because
                // we use the same former as for CAN. 
                // clearly the .port field has no meaning but it is not used, so i can safely use any value for it                 
                canloc.port = eOcanport1; // or also: (embot::app::msg::BUS::icc1 == des.getbus()) ? eOcanport1 : eOcanport2;   
                canloc.addr = des.getadr();
                canloc.insideindex = eobrd_caninsideindex_first;
            }
            return canloc;
        } 
        
        static eOcanprot_command_t tocancommand(eOcanprot_msgclass_t c, uint8_t co, void *v)
        {
           return {static_cast<uint8_t>(c), co, {0, 0}, v}; 
        }
        
        static embot::prot::can::Frame toprotcanframe(const embot::app::msg::Location &des, const eOcanprot_command_t &c, Former f)
        {
            eOcanprot_descriptor_t cd {c, tocanlocation(des)};           
            eOcanframe_t cf {0};               
            f(&cd, &cf);               
            return {cf.id, cf.size, cf.data};
        }
               
        static bool transmit(const embot::app::msg::Location &des, const eOcanprot_command_t &cmd, Former f)
        {
            bool r {false};
            if(true == des.isCAN())
            {
                r = tx2can(tocanlocation(des), cmd);
            }
            else if(true == des.isICC())
            {
#if defined(MESSAGING_DEBUG_PRINT)                
                bool printit {true};
                if((cmd.type == ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT) && (cmd.clas == eocanprot_msgclass_periodicMotorControl))
                {
                   printit = false; 
                }
                
                if(true == printit)
                {
                    std::string str = std::to_string(cmd.type) + "->" + des.to_string();
                    embot::app::eth::theErrorManager::getInstance().trace(str, {"tx2icc()",     
                              embot::os::theScheduler::getInstance().scheduled()}); 
                }   
#endif // defined(MESSAGING_DEBUG)                
                embot::prot::can::Frame frame = toprotcanframe(des, cmd, f);                
                r = tx2icc(des, frame);
            }  

            return r;            
        }
        
        static bool tx2can(const eObrd_canlocation_t &canloc, const eOcanprot_command_t &command);    
        static bool tx2icc(const embot::app::msg::Location &des, const embot::prot::can::Frame &frame);
                
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
    
    bool Set_Motor_Param_Config::transmit()
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
        embot::app::msg::Location destinations[embot::app::msg::numberofBUSes] {};
        int16_t payloads[embot::app::msg::numberofBUSes][Actuate_Motors::maxnumberofmotors] = {0}; 

        // in here i go through the actuations and i fill the destinations and the payloads for every bus
        for(uint8_t i=0; i<actuations.size(); i++)
        {
            if((true == actuations[i].destination.isvalid()) && (actuations[i].destination.getadr() > 0) && (actuations[i].destination.getadr() <= Actuate_Motors::maxnumberofmotors) )
            {
                uint8_t pos = embot::core::tointegral(actuations[i].destination.getbus());
                destinations[pos] = {actuations[i].destination.getbus(), sourceaddress}; // for the streaming we must pass the source, not the destination
                payloads[pos][actuations[i].destination.getadr()-1] = actuations[i].value;               
            }
        }

        // in here for every valid destination i ...
        bool r {true};
        for(uint8_t d=0; d<embot::app::msg::numberofBUSes; d++)
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
        embot::app::msg::Location CANdestination{}; // address is not used because it is a broadcast message
        int16_t CANdata[4] = {0};        
        
        // iccbus must be equal for all of them.
        embot::app::msg::Location ICCdestination {}; // address is not used because it is a broadcast message        
        int16_t ICCdata[4] = {0};

        
        // in here i go through the actuations and i fill the payload for the can and the icc
        for(uint8_t i=0; i<actuations.size(); i++)
        {
            if(true == actuations[i].destination.isCAN())
            {
                // bus must be equal for all of them. so, we choose the last one. address is dont care
                CANdestination.bus = actuations[i].destination.getbus();
                CANdata[actuations[i].destination.address-1] = actuations[i].value;
            }
            else if(true == actuations[i].destination.isICC())
            {
                // bus must be equal for all of them. so, we choose the last one. address is dont care
                ICCdestination.bus = actuations[i].destination.getbus();
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
   
    bool Set_PID::transmit()
    {
        Command command 
        {
            destination, 
            eocanprot_msgclass_pollingMotorControl, 
            ICUBCANPROTO_POL_MC_CMD__SET_PID, 
            const_cast<uint8_t*>(descriptor.getstream()),
            eocanprotMCpolling_former_POL_MC_CMD__SET_PID
        };
        
        return command.transmit();  
    } 
    
    bool Set_Motor_Param::transmit()
    {
        Command command 
        {
            destination, 
            eocanprot_msgclass_pollingMotorControl, 
            ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_PARAM, 
            const_cast<uint8_t*>(descriptor.getstream()),
            eocanprotMCpolling_former_POL_MC_CMD__SET_MOTOR_PARAM
        };
        
        return command.transmit();  
    }
    
} // namespace embot::app::eth::mc::messaging::sender {

 

namespace embot::app::eth::mc::messaging::sender {
    
    bool Command::tx2can(const eObrd_canlocation_t &canloc, const eOcanprot_command_t &command)
    {
#if defined(MESSAGING_DEBUG_PRINT)         
        bool printit {true};
        
        if((command.type == ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT) && (command.clas == eocanprot_msgclass_periodicMotorControl))
        {
           printit = false; 
        }
        
        if(true == printit)
        {
            std::string str = std::to_string(command.type);
            embot::app::eth::theErrorManager::getInstance().trace(str, {"tx2can()",     
                      embot::os::theScheduler::getInstance().scheduled()}); 
        }
#endif // #if defined(MESSAGING_DEBUG_PRINT)         
        return eores_OK == eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), const_cast<eOcanprot_command_t*>(&command), canloc);               
    }

    bool Command::tx2icc(const embot::app::msg::Location &des, const embot::prot::can::Frame &frame)
    {
        //#warning TODO: fill embot::app::eth::mc::messaging::sender::Command::tx2icc() which adds a frame in the ICC queue
        // embot::core::print(frame.to_string());
//        #warning TODO: solve the use of theICCservice by ems etc
#if defined(USE_ICC_COMM)
#if defined(debugNOicc)
#else
  
        embot::app::eth::icc::ItemCANframe icf {des, frame};         
        
#if defined(useICCserviceCAN)     
        embot::app::eth::icc::theICCserviceCAN::getInstance().put(icf);  
#else
        embot::app::eth::icc::theICCservice::getInstance().put(embot::app::eth::icc::theICCservice::Pipe::one, icf.item());        
#endif
        
#endif        
#endif        
        return true;
    } 
        
} // namespace embot::app::eth::mc::messaging::sender {



namespace embot::app::eth::mc::messaging::receiver {
    
    // in here we have objects which receive a frame and build up the source and info
    
    sigFOCstatus1::sigFOCstatus1(const embot::app::msg::BUS bus, const embot::prot::can::Frame &frame)
    {
        load(bus, frame);
    }
    
    void sigFOCstatus1::load(const embot::app::msg::BUS bus, const embot::prot::can::Frame &frame)
    {
        embot::app::msg::ADR a = static_cast<embot::app::msg::ADR>((frame.id & 0x0f0) >> 4);
        source.set(bus, a);
        
        std::memmove(info.payload, frame.data, sizeof(info.payload));
        
        info.current = ((int16_t*)frame.data)[0];
        info.velocity = ((int16_t*)frame.data)[1];
        info.position = ((int32_t*)frame.data)[1];
        
    }
    
    sigFOCstatus2::sigFOCstatus2(const embot::app::msg::BUS bus, const embot::prot::can::Frame &frame)
    {
        load(bus, frame);
    }
    
    void sigFOCstatus2::load(const embot::app::msg::BUS bus, const embot::prot::can::Frame &frame)
    {
        embot::app::msg::ADR a = static_cast<embot::app::msg::ADR>((frame.id & 0x0f0) >> 4);
        source.set(bus, a);
        
        std::memmove(info.payload, frame.data, sizeof(info.payload));
        
        info.controlmode = frame.data[0];
        info.qencflags = frame.data[1];
        info.pwmfeedback = ((int16_t*)frame.data)[2];
        info.motorfaultflags = ((uint32_t*)frame.data)[4];
    } 

    
    sigPrint::sigPrint(const embot::app::msg::BUS bus, const embot::prot::can::Frame &frame)
    {
        load(bus, frame);
    }
    
    void sigPrint::load(const embot::app::msg::BUS bus, const embot::prot::can::Frame &frame)
    {
        embot::app::msg::ADR a = static_cast<embot::app::msg::ADR>((frame.id & 0x0f0) >> 4);
        source.set(bus, a);
        
        std::memmove(info.payload, frame.data, sizeof(info.payload));
        
        info.endofburst = (frame.data[0] == 0x86);
        
        info.s = frame.data[1] >> 4;
        info.n = frame.data[1] & 0x0f;
        info.nchars = frame.size - 2;
        for(uint8_t i=0; i<info.nchars; i++)
        {
            info.substring[i] = frame.data[2+i];
        }
        info.substring[info.nchars] = 0;
    } 
    
    dummyAgent dummyagent {};
    Agent *theagent {&dummyagent};
    
    void load(Agent *a)
    {
        theagent = (nullptr == a) ? (&dummyagent) : (a);
    }
    
    bool parse(const embot::app::msg::BUS bus, const embot::prot::can::Frame &frame)
    {
        bool r {false};

        embot::prot::can::Clas cls {embot::prot::can::frame2clas(frame)};
        std::uint8_t cmd {embot::prot::can::frame2cmd(frame)};
        
        switch(cls)
        {
            case embot::prot::can::Clas::pollingMotorControl:
            {
                // maybe inside embot::prot::can::motor::polling ->
                // 0x07 GET_CONTROL_MODE, 91 GET_FIRMWARE_VERSION
                
                r = false;
            } break;

            case embot::prot::can::Clas::periodicMotorControl:
            {
                // inside embot::prot::can::motor::polling ->
                // FOC = 0, STATUS = 3, PRINT = 6, 
                //  not this one  EMSTO2FOC_DESIRED_CURRENT = 15  
                if(embot::core::tointegral(embot::prot::can::motor::periodic::CMD::FOC) == cmd)
                {
                    embot::app::eth::mc::messaging::receiver::sigFOCstatus1 s {bus, frame};
                    // and now i call the agent that processes what i need                         
                    theagent->get(s); 
                    r = true;                    
                }                
                else if(embot::core::tointegral(embot::prot::can::motor::periodic::CMD::STATUS) == cmd)
                {
                    embot::app::eth::mc::messaging::receiver::sigFOCstatus2 s {bus, frame};
                    // and now i call the agent that processes what i need 
                    theagent->get(s); 
                    r = true;  
                } 
                else if(embot::core::tointegral(embot::prot::can::motor::periodic::CMD::PRINT) == cmd)
                {
                    embot::app::eth::mc::messaging::receiver::sigPrint s {bus, frame};
                    // and now i call the agent that processes what i need 
                    theagent->get(s); 
                    r = true;  
                }             
            } break;
            
            case embot::prot::can::Clas::bootloader:
            case embot::prot::can::Clas::pollingAnalogSensor:
            case embot::prot::can::Clas::periodicAnalogSensor:
            case embot::prot::can::Clas::periodicInertialSensor:
            case embot::prot::can::Clas::periodicBattery:            
            default:
            {
                // add diagnostic message / trace ...
                r = false;
            } break;
        }        
        
        
        return r;        
        
    }

} // namespace embot::app::eth::mc::messaging::receiver {

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
