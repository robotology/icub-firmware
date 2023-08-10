
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
#include "EOtheCANservice.h"
#include "EOtheCANprotocol_functions.h" 

// --------------------------------------------------------------------------------------------------------------------
// - implementation 
// --------------------------------------------------------------------------------------------------------------------
 

namespace embot::app::eth::mc::messaging {
    
    void Destination::load(const eObrd_location_t &l)
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
        
        Destination destination {};
        eOcanprot_msgclass_t cls {eocanprot_msgclass_pollingMotorControl};
        uint8_t cmd {ICUBCANPROTO_POL_MC_CMD__NO_MESSAGE};
        void *value {nullptr};        
        Former former {nullptr};
                
        Command() = default; 
        Command(const Destination &d, eOcanprot_msgclass_t c, uint8_t co, void *v, Former f = dummyformer) 
            : destination(d), cls(c), cmd(co), former((nullptr == f) ? dummyformer : f) {}  
                
        void clear()
        {
            destination = {}; 
            cls = eocanprot_msgclass_pollingMotorControl; 
            cmd = ICUBCANPROTO_POL_MC_CMD__NO_MESSAGE;
            value = nullptr;
            former = nullptr;                
        }    

        void load(const Destination &d, eOcanprot_msgclass_t c, uint8_t co, void *v, Former f = dummyformer)
        {
            destination = d; 
            cls = c; 
            cmd = co;
            value = v;
            former = (nullptr == f) ? dummyformer : f; 
        }            

        bool isvalid() const { return destination.isvalid() && (nullptr != value) && (nullptr != former);  }         
              

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

        static eObrd_canlocation_t tocanlocation(const Destination &des)
        {
            eObrd_canlocation_t canloc {};
            if(true == des.isCAN())
            {            
                canloc.port = (Destination::BUS::can1 == des.bus) ? eOcanport1 : eOcanport2;   
                canloc.addr = des.address;
                canloc.insideindex = eobrd_caninsideindex_first;
            }
            else if(true == des.isICC())
            {            
                canloc.port = (Destination::BUS::icc1 == des.bus) ? eOcanport1 : eOcanport2;   
                canloc.addr = des.address;
                canloc.insideindex = eobrd_caninsideindex_first;
            }
            return canloc;
        } 
        
        static eOcanprot_command_t tocancommand(eOcanprot_msgclass_t c, uint8_t co, void *v)
        {
           return {static_cast<uint8_t>(c), co, {0, 0}, v}; 
        }
        
        static embot::prot::can::Frame toprotcanframe(const Destination &des, const eOcanprot_command_t &c, Former f)
        {
            eOcanprot_descriptor_t cd {c, tocanlocation(des)};           
            eOcanframe_t cf {0};               
            f(&cd, &cf);               
            return {cf.id, cf.size, cf.data};
        }
               
        static bool transmit(const Destination &des, const eOcanprot_command_t &cmd, Former f)
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
        
        static bool tx2can(const eObrd_canlocation_t &canloc, const eOcanprot_command_t &command)
        {
            return eores_OK == eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), const_cast<eOcanprot_command_t*>(&command), canloc);               
        }
    
        static bool tx2icc(const Destination &des, const embot::prot::can::Frame &frame)
        {
            #warning TODO: fill embot::app::eth::mc::messaging::sender::Command::tx2icc() which adds a frame in the ICC queue
            
            return true;
        }        
                
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
        
        // bus must be equal for all of them. but we can choose if can1 or can2
        Destination CANdestination {}; // address is not used because it is a broadcast message
        int16_t CANdata[4] = {0};        
        
        // iccbus must be equal for all of them.
        Destination ICCdestination {}; // address is not used because it is a broadcast message        
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
 
        if(true == CANdestination.isvalid())
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
    }        
   

} // namespace embot::app::eth::mc::messaging::sender {

 



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


