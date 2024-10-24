
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


// this is the memory of the controller that the MBD code contains inside iterative_motion_controller.cpp
// comment it out if you compile also iterative_motion_controller.cpp
//#define DEFINE_MBD_MEMORY_IN_HERE

#if defined(DEFINE_MBD_MEMORY_IN_HERE)
#warning the mock allocates memory for the MBD (iterative_motion_controller_U and iterative_motion_controller_Y. uncomment if you compile the MBD files
extern "C"
{
  // External inputs (root inport signals with default storage)
  ExtU_iterative_motion_controller_T iterative_motion_controller_U {};

  // External outputs (root outports fed by signals with default storage)
  ExtY_iterative_motion_controller_T iterative_motion_controller_Y {};
}
#endif

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
        
//        Status_iterative_motion_controller_T *status = _thembddata.io2->get_status();
        State state = _thembddata.io2->get_state();
        
        switch(e.event_type)
        {
            case EventTypes_SetLimit:
            {
//                status->currentlimits[e.motor_id] = e.limits_content;
                state.actcfg[e.motor_id]->thresholds.motorNominalCurrents = e.limits_content.nominal;
                state.actcfg[e.motor_id]->thresholds.motorPeakCurrents = e.limits_content.peak;
                state.actcfg[e.motor_id]->thresholds.motorOverloadCurrents = e.limits_content.overload;
                _thembddata.LIMcurOK[e.motor_id] = true;                
            } break;
            
            case EventTypes_SetControlMode:
            {
                _thembddata.io2->set_controlmode(e.control_mode_content, e.motor_id);
            } break;

            case EventTypes_SetMotorConfig:
            {
//                status->motorconfig[e.motor_id] = e.motor_config_content;
                state.actcfg[e.motor_id]->motor.externals = e.motor_config_content;
                _thembddata.MOTcfgOK[e.motor_id] = true;                               
            } break;

            case EventTypes_SetPid:
            {
                if(ControlModes_Current == e.pid_content.type)
                {
//                    status->pidcurrent[e.motor_id] = e.pid_content; 
                    state.actcfg[e.motor_id]->pids.currentPID = e.pid_content;                     
                    _thembddata.PIDcurOK[e.motor_id] = true;                    
                }
                else if(ControlModes_Velocity == e.pid_content.type)
                {
//                    status->pidvelocity[e.motor_id] = e.pid_content;
                    state.actcfg[e.motor_id]->pids.velocityPID = e.pid_content;
                    _thembddata.PIDvelOK[e.motor_id] = true;
                }
            } break;

            case EventTypes_SetTarget:
            {
//                status->target[e.motor_id] = e.targets_content.current;
                _thembddata.TRGjustarrived[e.motor_id] = true;
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
        for(const auto &i : _thembddata.io2->get_input()->EventsList)
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
                
                embot::app::bldc::mbd::interface::IO2::canFOCinfo cfi {};
                cfi.current = 2.0f;
                cfi.position = 45.0;
                cfi.velocity = 70.0f;
                _thembddata.io2->set(cfi, m);    
                 
                
                // 2. embot::prot::can::motor::periodic::CMD::STATUS
                    
                embot::app::bldc::mbd::interface::IO2::canSTATUSinfo csi {};
                               
                csi.controlmode = _thembddata.io2->get_controlmode(m); // - controlmode is maintained  
                csi.quadencoderstate = 0; // no flags
                csi.pwmfeedback = 30.0; // we impose it
                csi.motorfaultstate = 0; // no flags

                
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


