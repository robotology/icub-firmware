
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

// needed for: eObrd_location_t and icubCanProto*
#include "EoBoards.h"

 
#include <array>
 
// redefinition of some CAN message handling, due to mixed location of the actuators
// for example: the amc board uses three actuators: two amcbldc boards over CAN bus and a third one with the amc2c
// on its cm4 core. the two amcbldc use actual CAN messaging, whereas the amc2c excnage CAN frames over inter core communication
// for this case we need to override some TX / RX CAN frame handling.


namespace embot::app::eth::mc::messaging {
    
    // this object models the destination / origin of a message.
    // it can be constructed also using the embOBJ eObrd_location_t which allows can, extcan and eth locations    
    struct Location 
    {
        // if we keep can1 = 0 and can2 = 1 we have a simple conversion vs eOcanport1 and eOcanport2
        // i dont use the eOcanport1/eOcanport2 values to remove as much as possible dependencies from embOBJ 
        enum class BUS : uint8_t {can1 = 0, can2 = 1, icc1 = 2, icc2 = 3, any = 30, none = 31}; 
        static constexpr uint8_t numberofBUSes {4};         
        // enum class TYPE : uint8_t {can, icc, none}; -> maybe later on
        
        BUS bus {BUS::none};
        uint8_t address {0};
        // uint8_t subaddress {0}; -> maybe later on
        
        constexpr Location() = default;        
        constexpr Location(const BUS b, uint8_t a) : bus(b), address(a) {}           
        constexpr Location(const eObrd_location_t &l) { load(l); }    

        constexpr bool isvalid() const { return BUS::none != bus; }  
        constexpr bool isCAN() const { return (BUS::can1 == bus) || (BUS::can2 == bus); }       
        constexpr bool isICC() const { return (BUS::icc1 == bus) || (BUS::icc2 == bus); }

    private:        
        void load(const eObrd_location_t &l);      
    };
    
} // namespace embot::app::eth::mc::messaging {       

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
        icubCanProto_calibrator_t calibrator {icubCanProto_calibration_type_undefined, 0};
        
        CALIBRATE_ENCODER() = default;
        CALIBRATE_ENCODER(const icubCanProto_calibrator_t &c) : calibrator(c) {}
    };
        

    using MOTOR_ACTUATION = int32_t;
    
                             
} // namespace embot::app::eth::mc::messaging::info {


namespace embot::app::eth::mc::messaging::sender {
    
    // in here we have objects which receive an info and send it to a destination
    
    struct Set_Control_Mode
    { 
        Location destination {};           
        info::CONTROL_MODE info{};
            
        Set_Control_Mode() = default;
        Set_Control_Mode(const Location &d, const info::CONTROL_MODE &i) : destination(d), info(i) {}
            
        bool transmit();    
    }; 


    struct Set_Current_Limit
    { 
        Location destination {};           
        info::CURRENT_LIMIT info{};
            
        Set_Current_Limit() = default;
        Set_Current_Limit(const Location &d, const info::CURRENT_LIMIT &i) : destination(d), info(i) {}
            
        bool transmit();    
    };  

    struct Set_Current_PID
    { 
        Location destination {};           
        info::CURRENT_PID info{};
            
        Set_Current_PID() = default;
        Set_Current_PID(const Location &d, const info::CURRENT_PID &i) : destination(d), info(i) {}
            
        bool transmit();    
    };  

    struct Set_Velocity_PID
    { 
        Location destination {};           
        info::VELOCITY_PID info{};
            
        Set_Velocity_PID() = default;
        Set_Velocity_PID(const Location &d, const info::VELOCITY_PID &i) : destination(d), info(i) {}
            
        bool transmit();    
    }; 

    struct Set_Motor_Config
    { 
        Location destination {};           
        info::MOTOR_CONFIG info{};
            
        Set_Motor_Config() = default;
        Set_Motor_Config(const Location &d, const info::MOTOR_CONFIG &i) : destination(d), info(i) {}
            
        bool transmit();    
    };  

    struct Calibrate_Encoder
    {
        Location destination {};           
        info::CALIBRATE_ENCODER info{};
            
        Calibrate_Encoder() = default;
        Calibrate_Encoder(const Location &d, const info::CALIBRATE_ENCODER &i) : destination(d), info(i) {}
            
        bool transmit();   
    }; 

    struct Actuate_Motors
    { 
        struct Actuation
        {
            Location destination {};           
            info::MOTOR_ACTUATION value{};
                
            Actuation() = default;
            Actuation(const Location &d, const info::MOTOR_ACTUATION &v) : destination(d), value(v) {}            
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
    {   
        int16_t current {0};
        int16_t velocity {0};
        int32_t position {0};
        
        FOCstatus1() = default;         
        FOCstatus1(int16_t c, int16_t v, int32_t p) : current(c), velocity(v), position(p) {}
    };
    
    
    struct FOCstatus2
    {   // the State2FocMsg in Motor_hid.h   
        uint8_t controlmode {0};
        uint8_t qencflags {0};          // the QEState in Motor_hid.h
        int16_t pwmfeedback {0};
        uint32_t motorfaultflags {0};   // the MotorFaultState 
        
        FOCstatus2() = default;         
    };    
                             
} // namespace embot::app::eth::mc::messaging::info {

namespace embot::app::eth::mc::messaging::receiver {
    
    // in here we have objects which receive info and send it to a destination
    
    struct sigFOCstatus1
    {   // ICUBCANPROTO_PER_MC_MSG__2FOC 
        Location source {};           
        info::FOCstatus1 info {};
            
        sigFOCstatus1() = default;
            
            void load(const Location::BUS bus, const embot::prot::can::Frame &frame, bool andprocess = true);
    }; 

    struct sigFOCstatus2
    {   // ICUBCANPROTO_PER_MC_MSG__STATUS 
        Location source {};           
        info::FOCstatus2 info {};
            
        sigFOCstatus2() = default;
            
        void load(const Location::BUS bus, const embot::prot::can::Frame &frame, bool andprocess = true);
    };
    
} // namespace embot::app::eth::mc::messaging::receiver {

 
#endif  // include-guard


