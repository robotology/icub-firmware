
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_APP_APPLICATION_THEPZMDRIVER_H_
#define _EMBOT_APP_APPLICATION_THEPZMDRIVER_H_


#include "embot_app_application_theCANparserMC.h"

#include "embot_core.h"
#include "embot_hw.h"
#include "embot_os.h"
#include "embot_os_Thread.h"
//#include "embot_prot_can.h"
//#include "embot_prot_can_motor_periodic.h"
#include <array>
#include <memory>

namespace embot { namespace app { namespace application {
           
    class thePZMdriver : public CANagentMC
    {
    public:
        static thePZMdriver& getInstance();
        
        // this object is very simple. 
        // it inits the pzm hw driver and sets it with zero velocities (so that it does not move)
        // when a message motor::periodic::CMD::EMSTO2FOC_DESIRED_CURRENT arrives the object marks the arrival time and
        // imposes the three velocities to the pzm hw driver
        // an internal periodic timer (of period timeoutofsetpoint/2) verifies that the age of the latest setpoint is not older than timeoutofsetpoint
        // and if it is older it set the velocities to zero.    

        static constexpr size_t numberofmotors {3};        
 
        struct Config
        {
            embot::os::Thread *owner {nullptr};
            embot::os::Event evmove {embot::os::bitpos2event(0)};
            embot::os::Event evstop {embot::os::bitpos2event(1)};
            embot::core::relTime timeout {50*embot::core::time1millisec};              
            constexpr Config() = default;
            constexpr Config(embot::os::Thread *o, embot::os::Event em, embot::os::Event es, embot::core::relTime t) : owner(o), evmove(em), evstop(es), timeout(t) {}
            bool isvalid() const { return (nullptr != owner ) && (0 != evmove) && (0 != evstop) && (evmove != evstop) && (0 != timeout); }
        }; 
        
        
        bool initialise(const Config &config);   
        
        bool process(embot::os::EventMask evtmask);
        
        bool start();
        bool stop(); 
               
        
        // interface to CANagentMC
        virtual bool get(const embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info &info);
        
    private:
        thePZMdriver(); 
        ~thePZMdriver(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;    
    };       


}}} // namespace embot { namespace app { namespace application


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
