
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_BLDC_MBD_INTERFACE_H_
#define __EMBOT_APP_BLDC_MBD_INTERFACE_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_prot_can.h"
#include "embot_prot_can_bootloader.h"
#include "embot_prot_can_motor.h"
#include "embot_prot_can_motor_periodic.h"
#include "embot_prot_can_motor_polling.h"
#include "embot_prot_can_analog_periodic.h"
#include "embot_prot_can_analog_polling.h"
#include "embot_prot_can_motor_board.h"
#include "embot_hw_motor_bldc.h"

#include "embot_app_bldc_MBD_api.h"

namespace embot::app::bldc::mbd::interface {
    
    // utilities
    std::string to_string(const ControlModes &cm);
    std::string to_string(const SupervisorInputLimits &sil);
    
    struct Converter
    {
        // from mbd to can
        static void tocan(const embot::app::bldc::mbd::interface::ControlModes from, embot::prot::can::motor::ControlMode &to); 
        static void tocan(const embot::app::bldc::mbd::interface::SupervisorInputLimits &from, embot::prot::can::motor::CurrentLimits &to); 
        static void tocan(const embot::app::bldc::mbd::interface::PID &from, embot::prot::can::motor::PID &to);  
        static void tocan(const embot::app::bldc::mbd::interface::MotorConfigurationExternal &from, embot::prot::can::motor::MotorConfig &to);    
        static void tocan(const uint8_t from, embot::prot::can::motor::board::foc::QEstate &to);
        static void tocan(const uint32_t from, embot::prot::can::motor::board::foc::MotorFaultState &to); 
        // from can to mbd
        static void fromcan(const embot::prot::can::motor::ControlMode from, embot::app::bldc::mbd::interface::ControlModes &to);
        static void fromcan(const embot::prot::can::motor::CurrentLimits &from, embot::app::bldc::mbd::interface::SupervisorInputLimits &to); 
        static void fromcan(const embot::prot::can::motor::PID &from, embot::app::bldc::mbd::interface::PID &to);
        static void fromcan(const embot::prot::can::motor::MotorConfig &from, embot::app::bldc::mbd::interface::MotorConfigurationExternal &to);
        static void fromcan(const int16_t from, Targets &to);  
			  static void fromcan(const embot::app::bldc::mbd::interface::MotorConfigurationExtSet &from, embot::app::bldc::mbd::interface::MotorConfigurationExtSet &to);

        // utilities
        static float millitoampere(int16_t ma)
        {
            return 0.001 * static_cast<float>(ma);
        }
        
        static int16_t amperetomilli(float a)
        {
            return static_cast<int16_t>(1000.0f * a);
        }                
    };
    

    struct IO2
    {
        struct canFOCinfo
        {
            float current {0.0f};
            float position {0.0f};
            float velocity {0.0f};
            canFOCinfo() = default;            
        };
        
        struct canSTATUSinfo
        {
            embot::app::bldc::mbd::interface::ControlModes controlmode {embot::app::bldc::mbd::interface::ControlModes::ControlModes_NotConfigured};
            uint8_t quadencoderstate {0};
            float pwmfeedback {0.0f};
            uint32_t motorfaultstate {0};
            canSTATUSinfo() = default;            
        }; 

        struct canADDITIONALSTATUSinfo
        {
            embot::app::bldc::mbd::interface::ControlModes controlmode {embot::app::bldc::mbd::interface::ControlModes::ControlModes_NotConfigured};
            uint8_t quadencoderstate {0};
            float pwmfeedback {0.0f};
            uint32_t motorfaultstate {0};
            canADDITIONALSTATUSinfo() = default;            
        };
        
        struct canDEBUGqenccalibresult
        {
            int16_t offset {0};
            bool calibrationdone {false};
            canDEBUGqenccalibresult() = default;            
        };        

        struct Qenc
        {
            float counter {0};
            float indexcounter {0};  
        };

        struct FOCinput
        {
            float electricalangle {0.0};
            embot::hw::motor::bldc::Currents currents {}; 
            float mechanicalangle {0.0};
            uint8_t hall {0};
            Qenc qenc {};
                
            FOCinput() = default;
            FOCinput(float ea, const embot::hw::motor::bldc::Currents &cu, float me, uint8_t h, const Qenc &q) 
                : electricalangle(ea), currents(cu), mechanicalangle(me), hall(h), qenc {q} {}
                    
            void load(float ea, const embot::hw::motor::bldc::Currents &cu, float me, uint8_t h, const Qenc &q)
            {
                electricalangle = ea;
                currents = cu;
                mechanicalangle = me;
                hall = h;
                qenc = q;
            }
        };  

        struct FOCoutput
        {
            embot::hw::motor::bldc::PWM3 pwm {}; 

            FOCoutput() = default;
            FOCoutput(const embot::hw::motor::bldc::PWM3 &p) 
                : pwm(p) {}
        };         
        
        IO2() = default;
        
        IO2(embot::app::bldc::mbd::interface::ExtU_iterative_motion_controller_T *i, embot::app::bldc::mbd::interface::ExtY_iterative_motion_controller_T *o)
           : input(i), output(o) 
        {
            state.load(*o);
        }
        
        void load(embot::app::bldc::mbd::interface::ExtU_iterative_motion_controller_T *i, embot::app::bldc::mbd::interface::ExtY_iterative_motion_controller_T *o)
        {
            input = i;
            output = o;
            state.load(*o);
        }
        
        constexpr bool isvalid() const { return (nullptr != input) && (nullptr != output); }
             
       
        // events: the events are pushed back inside a fifo so that
        // the MBD can evaluate them. the fifo has a limited capacity,
        // so the pushback() returns false when capacity is reached
        void event_clearall();
        bool event_pushback(const embot::app::bldc::mbd::interface::ControlModes cm, uint8_t motor);
        bool event_pushback(const embot::app::bldc::mbd::interface::SupervisorInputLimits &limits, uint8_t motor); 
        bool event_pushback(const embot::app::bldc::mbd::interface::PID &pid, uint8_t motor);
        bool event_pushback(const embot::app::bldc::mbd::interface::MotorConfigurationExternal &mc, uint8_t motor);
        bool event_pushback(const embot::app::bldc::mbd::interface::Targets &t, uint8_t motor);
				bool event_pushback(const embot::app::bldc::mbd::interface::MotorConfigurationExtSet &param, uint8_t motor);
        
        // setter
        void set_fault(const bool pressed);
        void set_controlmode(const embot::app::bldc::mbd::interface::ControlModes cm, uint8_t motor);     
        void set_temperature(float celsiusdegrees, uint8_t motor);
        void set_powersupply(float volt, uint8_t motor);
        void set(const FOCinput &i, uint8_t motor);
        
        // getter
        bool get_fault() const;
        embot::app::bldc::mbd::interface::ControlModes get_controlmode(uint8_t motor) const;        
        bool get_transmit(embot::prot::can::motor::periodic::CMD cmd, uint8_t motor) const;         
        void get(canFOCinfo &info, uint8_t motor) const;
        void get(canSTATUSinfo &info, uint8_t motor) const;        
        float get_temperature(uint8_t motor) const;
        void get(FOCoutput &o, uint8_t motor) const;
        void get(canDEBUGqenccalibresult &info, uint8_t motor) const;

        void get_current_limits(uint8_t motor, embot::app::bldc::mbd::interface::SupervisorInputLimits &cl);
        void get_current_pid(uint8_t motor, embot::app::bldc::mbd::interface::PID &pid);
        void get_velocity_pid(uint8_t motor, embot::app::bldc::mbd::interface::PID &pid);
        void get_pos_pid(uint8_t motor, embot::app::bldc::mbd::interface::PID &pid);
        void get_motor_config(uint8_t motor, embot::app::bldc::mbd::interface::MotorConfigurationExternal &mc);

    public:
        // this section must be public only if we         
        // use it for DEBUG w/ a mock MBD engine
        // else it should be private
        void set(const canFOCinfo &info, uint8_t motor); 
        void set(const canSTATUSinfo &info, uint8_t motor); 
        embot::app::bldc::mbd::interface::ExtU_iterative_motion_controller_T * get_input() const;
        embot::app::bldc::mbd::interface::ExtY_iterative_motion_controller_T * get_output() const;
        embot::app::bldc::mbd::interface::State& get_state();

    private:
        
        // -- pointers to mbd data.       
        // they point to the global variables inside the generated code
        embot::app::bldc::mbd::interface::ExtU_iterative_motion_controller_T * input {nullptr};
        embot::app::bldc::mbd::interface::ExtY_iterative_motion_controller_T * output {nullptr};  

        embot::app::bldc::mbd::interface::State state {};

    private:
        
        // -- handling of event queue
        uint8_t evtpos {0};        
        int8_t getposofevent(embot::app::bldc::mbd::interface::EventTypes evt, uint8_t motor);
        bool addevent(const embot::app::bldc::mbd::interface::ReceivedEvents& re, uint8_t motor);
        
        // - others
        
    };
    

} // namespace embot::app::bldc::mbd::interface {


namespace embot::app::bldc::mbd::interface {
    
    IO2& getIO2handle();    
    
    void init();

    void tick();
    
    void foc(const std::array<IO2::FOCinput, 2> &i, std::array<IO2::FOCoutput, 2> &o);
    
} // namespace embot::app::bldc::mbd::interface {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
