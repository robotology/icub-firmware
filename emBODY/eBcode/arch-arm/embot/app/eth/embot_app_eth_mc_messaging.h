
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#ifndef __EMBOT_APP_ETH_MC_MESSAGING__
#define __EMBOT_APP_ETH_MC_MESSAGING__


#include "embot_core.h"
#include "embot_core_binary.h"
#include "embot_prot_can.h"
#include "embot_prot_can_motor_polling.h" 
#include "embot_prot_can_motor_periodic.h"

// needed for: icubCanProto*
#include "iCubCanProto_types.h"

 
#include <array>

#include "embot_app_msg.h"

      

namespace embot::app::eth::mc::messaging::info {
    
    // these objects hold a neutral info (as much as possible) for the messaging
    // they are used by the similar object in namespace ::sender
    
    struct CONTROL_MODE
    {   // icubCanProto_controlmode_t or embot::prot::can::motor::polling::ControlMode
        uint8_t controlmode {0};
        
        CONTROL_MODE() = default;   
        CONTROL_MODE(uint8_t c) : controlmode(c) {}            
        CONTROL_MODE(icubCanProto_controlmode_t c) : controlmode(static_cast<uint8_t>(c)) {}
        CONTROL_MODE(embot::prot::can::motor::polling::ControlMode c) : controlmode(static_cast<uint8_t>(c)) {}
    };    


    struct CURRENT_LIMIT
    {   // milli-ampere
        int16_t nominal {0};
        int16_t peak {0};
        int16_t overload {0};
        
        CURRENT_LIMIT() = default;
        CURRENT_LIMIT(int16_t n, int16_t p, int16_t o) : nominal(n), peak(p), overload(o) {}
    };
    
    struct TEMPERATURE_LIMIT
    {   // the measurement unit is 0.1 C
        int16_t temperature {0};
        
        TEMPERATURE_LIMIT() = default;
        TEMPERATURE_LIMIT(int16_t t) : temperature(t) {}
    };
 
    struct PID16bit
    {
        int16_t Kp {0};
        int16_t Ki {0};
        int16_t Kd {0};
        uint8_t Ks {0};
        
        PID16bit() = default;
        PID16bit(int16_t p, int16_t i, int16_t d, uint8_t s) : Kp(p), Ki(i), Kd(d), Ks(s) {}
    };  

    using CURRENT_PID = PID16bit;
    
    using VELOCITY_PID = PID16bit;
    
    struct GenericData
    {   // but we can use less bytes
        uint8_t bytes[8] = {0}; 
        
        GenericData() = default;
        GenericData(uint64_t b8) { std::memmove(bytes, &b8, 8); }
        GenericData(void *p) { if(nullptr != p) { std::memmove(bytes, p, 8); } }
    };  

    
    using MOTOR_CONFIG = GenericData;    


    struct CALIBRATE_ENCODER
    {   
        icubCanProto_calibrator_t calibrator {icubCanProto_calibration_type0_hard_stops, 0};
        
        CALIBRATE_ENCODER() = default;
        CALIBRATE_ENCODER(const icubCanProto_calibrator_t &c) : calibrator(c) {}
    };
        

    using MOTOR_ACTUATION = int32_t;
    
                             
} // namespace embot::app::eth::mc::messaging::info {


namespace embot::app::eth::mc::messaging::sender {
    
    // in here we have objects which receive an info and send it to a destination
    
    struct Set_Control_Mode
    { 
        embot::app::msg::Location destination {};           
        info::CONTROL_MODE info{};
            
        Set_Control_Mode() = default;
        Set_Control_Mode(const embot::app::msg::Location &d, const info::CONTROL_MODE &i) : destination(d), info(i) {}
            
        bool transmit();    
    }; 


    struct Set_Current_Limit
    { 
        embot::app::msg::Location destination {};           
        info::CURRENT_LIMIT info{};
            
        Set_Current_Limit() = default;
        Set_Current_Limit(const embot::app::msg::Location &d, const info::CURRENT_LIMIT &i) : destination(d), info(i) {}
            
        bool transmit();    
    };

    struct Set_Temperature_Limit
    {
        embot::app::msg::Location destination {};
        info::TEMPERATURE_LIMIT info{};
        
        Set_Temperature_Limit() = default;
        Set_Temperature_Limit(const embot::app::msg::Location &d, const info::TEMPERATURE_LIMIT &i) : destination(d), info(i) {}
        
        bool transmit();
    };

    struct Set_Current_PID
    { 
        embot::app::msg::Location destination {};           
        info::CURRENT_PID info{};
            
        Set_Current_PID() = default;
        Set_Current_PID(const embot::app::msg::Location &d, const info::CURRENT_PID &i) : destination(d), info(i) {}
            
        bool transmit();    
    };  

    struct Set_Velocity_PID
    { 
        embot::app::msg::Location destination {};           
        info::VELOCITY_PID info{};
            
        Set_Velocity_PID() = default;
        Set_Velocity_PID(const embot::app::msg::Location &d, const info::VELOCITY_PID &i) : destination(d), info(i) {}
            
        bool transmit();    
    }; 

    struct Set_Motor_Config
    { 
        embot::app::msg::Location destination {};           
        info::MOTOR_CONFIG info{};
            
        Set_Motor_Config() = default;
        Set_Motor_Config(const embot::app::msg::Location &d, const info::MOTOR_CONFIG &i) : destination(d), info(i) {}
            
        bool transmit();    
    };  

    struct Calibrate_Encoder
    {
        embot::app::msg::Location destination {};           
        info::CALIBRATE_ENCODER info{};
            
        Calibrate_Encoder() = default;
        Calibrate_Encoder(const embot::app::msg::Location &d, const info::CALIBRATE_ENCODER &i) : destination(d), info(i) {}
            
        bool transmit();   
    }; 

    struct Actuate_Motors
    { 
        struct Actuation
        {
            embot::app::msg::Location destination {};           
            info::MOTOR_ACTUATION value{};
                
            Actuation() = default;
            Actuation(const embot::app::msg::Location &d, const info::MOTOR_ACTUATION &v) : destination(d), value(v) {}            
        };
        
        static constexpr uint8_t maxnumberofmotors {4};
        
        std::array<Actuation, maxnumberofmotors> actuations {};
        uint8_t number {0};
        uint8_t sourceaddress {0};
                    
        Actuate_Motors() = default;
        Actuate_Motors(const std::array<Actuation, maxnumberofmotors> &a, uint8_t saddr) : actuations(a), number(maxnumberofmotors), sourceaddress(saddr) {}
            
        void clear() { number = 0; }
        void push_back(const Actuation &a) { if(number < maxnumberofmotors) { actuations[number++] = a; } }

        bool transmit();    
    };   
    
} // namespace embot::app::eth::mc::messaging::sender {


namespace embot::app::eth::mc::messaging::info {
    
    struct FOCstatus1
    {   // ICUBCANPROTO_PER_MC_MSG__2FOC   
        int16_t current {0};
        int16_t velocity {0};
        int32_t position {0};
        
        uint8_t payload[8] = {0};
        
        FOCstatus1() = default;         
        //FOCstatus1(int16_t c, int16_t v, int32_t p) : current(c), velocity(v), position(p) {}
    };
    
    
    struct FOCstatus2
    {   // ICUBCANPROTO_PER_MC_MSG__STATUS 
        // we use the struct State2FocMsg that is in Motor_hid.h   
        uint8_t controlmode {0};
        uint8_t qencflags {0};          // the QEState in Motor_hid.h
        int16_t pwmfeedback {0};
        uint32_t motorfaultflags {0};   // the MotorFaultState 
        
        uint8_t payload[8] = {0};
        
        FOCstatus2() = default;         
    };    
    
    struct Print
    {   // ICUBCANPROTO_PER_MC_MSG__PRINT (=6)
        uint8_t s {0};          // signature of the string
        uint8_t n {0};          // sequence number [0, 5] 
        char substring[7] {0};  // can be of lenght [1, 6]
        uint8_t nchars {0};    
        bool endofburst {true};
        uint8_t payload[8] = {0};
        
        Print() = default;         
    };        
                             
} // namespace embot::app::eth::mc::messaging::info {

namespace embot::app::eth::mc::messaging::receiver {
    
    // in here we have objects which receive a frame and build up the source and info
    
    enum class MessageType 
    {
        FOCstatus1 = 0,
        FOCstatus2 = 1,
        Print = 3
    };
    
    struct sigFOCstatus1
    {   // ICUBCANPROTO_PER_MC_MSG__2FOC 
        embot::app::msg::Location source {};           
        info::FOCstatus1 info {};
            
        sigFOCstatus1() = default;
        sigFOCstatus1(const embot::app::msg::BUS bus, const embot::prot::can::Frame &frame);   
                    
        void load(const embot::app::msg::BUS bus, const embot::prot::can::Frame &frame);
    }; 

    struct sigFOCstatus2
    {   // ICUBCANPROTO_PER_MC_MSG__STATUS 
        embot::app::msg::Location source {};           
        info::FOCstatus2 info {};
            
        sigFOCstatus2() = default;
        sigFOCstatus2(const embot::app::msg::BUS bus, const embot::prot::can::Frame &frame); 
            
        void load(const embot::app::msg::BUS bus, const embot::prot::can::Frame &frame);
    };

    struct sigPrint
    {   // ICUBCANPROTO_PER_MC_MSG__PRINT 
        embot::app::msg::Location source {};           
        info::Print info {};
            
        sigPrint() = default;
        sigPrint(const embot::app::msg::BUS bus, const embot::prot::can::Frame &frame); 
            
        void load(const embot::app::msg::BUS bus, const embot::prot::can::Frame &frame);
    };
    
    // and we also have an agent that operates on the received info
    
    class Agent
    {
    public:
               
        // interface: returns true is we need to tx a reply else false
        virtual bool get(const sigFOCstatus1 &msg) = 0;
        virtual bool get(const sigFOCstatus2 &msg) = 0;
        virtual bool get(const sigPrint &msg) = 0;
        
    public:
        virtual ~Agent() {}         
    };
    
    class dummyAgent : public Agent 
    {
    public:
        
        dummyAgent() {}
        virtual ~dummyAgent() {}
            
        bool get(const sigFOCstatus1 &msg) override { return false; } 
        bool get(const sigFOCstatus2 &msg) override { return false; } 
        bool get(const sigPrint &msg) override { return false; }         
    };   


    // and now we ... get an agent ... this funtion must be defined by the application
    
    Agent * agent();
    // and we load it    
    void load(Agent *a);
   
    
    // which is used by this one
    bool parse(const embot::app::msg::BUS bus, const embot::prot::can::Frame &frame);
    
} // namespace embot::app::eth::mc::messaging::receiver {

 
#endif  // include-guard


