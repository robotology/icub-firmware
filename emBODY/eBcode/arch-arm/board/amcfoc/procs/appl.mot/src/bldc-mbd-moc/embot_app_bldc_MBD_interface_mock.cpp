
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_bldc_MBD_interface_mock.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_core.h"

#include "embot_app_bldc_MBD_interface.h"

#include "embot_prot_can_motor_periodic.h"

#include <array>

// --------------------------------------------------------------------------------------------------------------------
// - defines
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


namespace embot::app::bldc::mbd::interface::mock {
    
    

//    // in here i just wrap something around the MBD type, so that they are more manageable
//    // i dont want to invest much in that because the ::mock is just a test
    
    struct theMBDdata
    {
        embot::app::bldc::mbd::interface::IO2 *io2 {nullptr};

        theMBDdata() = default;
            
        theMBDdata(embot::app::bldc::mbd::interface::IO2 *i) : io2(i) {}
            
            
        void load(embot::app::bldc::mbd::interface::IO2 *i)
        {
            io2 = i;
        }
        
        void clear()
        {
            // i assume io2 is not nullptr            
            io2->event_clearall();            
            targetclear();
        } 
        
        void targetclear()
        {
            TRGjustarrived[0] = TRGjustarrived[1] = false;
        }
        
        bool faulted {false};
        bool PIDcurOK[2] {false, false};
        bool PIDvelOK[2] {false, false};
        bool LIMcurOK[2] {false, false};
        bool MOTcfgOK[2] {false, false};
        bool TRGjustarrived[2] = {false, false};
        

        bool emit(uint8_t m) const
        {
            bool ctrlmodeOK {false};
            ControlModes c = io2->get_controlmode(m);
            ctrlmodeOK = (c != ControlModes_NotConfigured) && (c != ControlModes_Idle);
            return PIDcurOK[m] && PIDvelOK[m] && LIMcurOK[m] && MOTcfgOK[m] && ctrlmodeOK && TRGjustarrived[m];
        }             
        
    };
    
        
    theMBDdata _thembddata {};
        
            
    void init(embot::app::bldc::mbd::interface::IO2 *io2)
    {        
        _thembddata.load(io2);
        _thembddata.clear();        
    }
    
    
    void process(const ReceivedEvents &e)
    {
        if(EventTypes_None == e.event_type)
        {
            return;
        }
        
        Status_iterative_motion_controller_T *status = _thembddata.io2->get_status();
        
        switch(e.event_type)
        {
            case EventTypes_SetLimit:
            {
                status->currentlimits[e.motor] = e.limits_content;
                _thembddata.LIMcurOK[e.motor] = true;                
            } break;
            
            case EventTypes_SetControlMode:
            {
                _thembddata.io2->set_controlmode(e.control_mode_content, e.motor);
            } break;

            case EventTypes_SetMotorConfig:
            {
                status->motorconfig[e.motor] = e.motor_config_content;
                _thembddata.MOTcfgOK[e.motor] = true;                               
            } break;

            case EventTypes_SetPid:
            {
                if(ControlModes_Current == e.pid_content.type)
                {
                    status->pidcurrent[e.motor] = e.pid_content;  
                    _thembddata.PIDcurOK[e.motor] = true;                    
                }
                else if(ControlModes_Velocity == e.pid_content.type)
                {
                    status->pidvelocity[e.motor] = e.pid_content;
                    _thembddata.PIDvelOK[e.motor] = true;
                }
            } break;

            case EventTypes_SetTarget:
            {
                status->target[e.motor] = e.targets_content.current;
                _thembddata.TRGjustarrived[e.motor] = true;
            } break; 

            default: {} break;
        }       
        
    }    
    
    
    void process_01_input()
    {

        // flags 
        // faulted stays inside IO2::input but can be retrieved by _thembddata.io2->get_fault()
        _thembddata.faulted = _thembddata.io2->get_fault();
        
        // i remove the targets
        _thembddata.targetclear();
        
        // retrieve the message events
        for(const auto &i : _thembddata.io2->get_input()->Events)
        {
            process(i); 
        }  

        // sensor data 
        // they stay inside IO2::input and can be retrieved w/ following code        
        //for(uint8_t m=0; m<2; m++)
        //{
        //    process(_thembddata.io2->get_input()->SensorData[m], m);
        //}     
        
    }
    
    size_t num {0};
    void process_02_activity()
    {
        // for now, ... increment a counter and ...
        num++;
        
        // fill 
    }
    
    void process_03_output()
    {
        // fill up the output status
        
        for(uint8_t m=0; m<2; m++)
        {
            if(true == _thembddata.emit(m))
            {
                // ok, i emit
                _thembddata.io2->get_output()->Flags_d[m].enable_sending_msg_status = true;
                
                
                // i fill what will be used for the CAN messages:
                
                // 1. embot::prot::can::motor::periodic::CMD::FOC 
                // - current
                 _thembddata.io2->get_output()->Estimates[m].Iq_filtered = 2.0f;

                // - velocity
                _thembddata.io2->set_velocity(70.0, m);
                
                // - position
                _thembddata.io2->set_position(45.0, m); 
                
                _thembddata.io2->get_output()->FOCOutputs_h[m].Vq = num;
                // etc.
                
                
                // 2. embot::prot::can::motor::periodic::CMD::STATUS
                // - controlmode is already inside output
                // - quadencoderstate ...
                // - pwmfeedback ...
                // - motorfaultstate ...
                // 
                
                // 3. embot::prot::can::motor::periodic::CMD::ADDITIONAL_STATUS
                // - temperature
                _thembddata.io2->get_output()->Estimates[m].motor_temperature = 40.0f;
                
                
            }
            else
            {
                _thembddata.io2->get_output()->Flags_d[m].enable_sending_msg_status = false;
            }
            
        }
                
        
        // and finally ... 
        
        _thembddata.targetclear();
    }        
}





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


