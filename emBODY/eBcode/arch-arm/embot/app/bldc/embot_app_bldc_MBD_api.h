
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_BLDC_MBD_API_H_
#define __EMBOT_APP_BLDC_MBD_API_H_

#include "embot_core.h"
#include "stdint.h" 

#if 0

    this file contains the API that is specified by the generated and it insert that into 
    the embot::app::bldc::mbd::interface namespace.
    it also adds some c++ data structures for easier manipulation.
    
    however, in here we msut manage the changes of the generated api.
    
    for the case of the amcfoc (but hopefully also for other boards, it includes 
    
    #include "iterative_motion_controller_types.h"
    #include "iterative_motion_controller.h"
    
    maybe we can manage variability using some sort of MBD_API_VERSION

#endif


#define MBD_API_VERSION_1_0 0x10


#define MBD_API_VERSION MBD_API_VERSION_1_0


#if (MBD_API_VERSION == MBD_API_VERSION_1_0) 

    // this version is ok for amcfoc and maybe also for amcmj1
    // with matlab generated files inside arch-arm/mbd/amcfoc



    #include "iterative_motion_controller_types.h"
    #include "iterative_motion_controller.h"

    // some consistency checks.
    // in SHA-1: b481fc47a2a1e18e09c0d43e0ec536447ef9a0e0
    // AMCFOC: new Motor Parameters + AMCBLDC/AMC2C: Planner, Calibration, new Motor Parameter and new CAN messages (#651)
    // it was 
    static_assert(sizeof(DW_iterative_motion_controller_T) == 9648, "check of block signals and states");
    static_assert(sizeof(ExtU_iterative_motion_controller_T) == 1004, "check of ext inputs");
    static_assert(sizeof(ExtY_iterative_motion_controller_T) == 644, "check of ext outputs");

namespace embot::app::bldc::mbd::interface {
    
    constexpr size_t maxMOTORs {2};
}


namespace embot::app::bldc::mbd::interface {
    
    // we rename the C types to be visible inside the embot::app::bldc::mbd::interface namespace
    
    using MotorConfigurationExtSet = MotorConfigurationExtSet;
    using DriverSensors = DriverSensors;
    using SensorsData = SensorsData;
    using ExternalFlags = ExternalFlags;
    using EventTypes = EventTypes;
    using Targets = Targets;
    using ControlModes = ControlModes;
    using PID = PID;
    using SupervisorInputLimits = SupervisorInputLimits;
    using MotorConfigurationExternal = MotorConfigurationExternal;
    using ReceivedEvents = ReceivedEvents;
    using FOCOutputs = FOCOutputs;
    using EstimatedData = EstimatedData;
    using HardwareFaults = HardwareFaults;
    using Flags = Flags;
    using Thresholds = Thresholds;
    using PIDsConfiguration = PIDsConfiguration;
    using MotorConfiguration = MotorConfiguration;
    using ActuatorConfiguration = ActuatorConfiguration; // contains: 
//    using BUS_MSG_FOC = BUS_MSG_FOC;
    using MCControlModes = MCControlModes;
//    using BUS_FLAGS_TX = BUS_FLAGS_TX;
//    using BUS_MSG_STATUS = BUS_MSG_STATUS;
    using BUS_MESSAGES_TX = BUS_MESSAGES_TX;
    using BUS_STATUS_TX = BUS_STATUS_TX;
//    using EstimationVelocityModes = EstimationVelocityModes;
//    using RT_MODEL_iterative_motion_controller_T = RT_MODEL_iterative_motion_controller_T;
    
    // derived types
    using ExtU_iterative_motion_controller_T = ExtU_iterative_motion_controller_T;
    using ExtY_iterative_motion_controller_T = ExtY_iterative_motion_controller_T;
    

    inline void MBD_initialize()
    {
        AMCFOC_initialize();
    }
    
    inline void MBD_step()
    {
        AMCFOC_step_Time_1ms();
    }  
    
    inline void MBD_foc()
    {
        AMCFOC_step_FOC();
    } 


    // we also add a c++ class that much helps
    
    struct State
    {        
        State() = default;
        
        State(ExtY_iterative_motion_controller_T &out)
        {
            load(out);         
        }
        
        void load(ExtY_iterative_motion_controller_T &out)
        {
            for(uint8_t m=0; m<nmot; m++)
            {
                actcfg[m] = &out.ConfigurationParameters[m];
                messages[m] = &out.Messages[m];
                messagesflags[m] = &out.MessagesFlags[m];
           }           
        }
        
        bool isvalid() const
        {
            return nullptr != actcfg[0];
        }
        
    public:
        static constexpr uint8_t nmot {maxMOTORs};
        ActuatorConfiguration* actcfg[nmot] {nullptr, nullptr};
        BUS_MESSAGES_TX* messages[nmot] {nullptr, nullptr};
        BUS_STATUS_TX* messagesflags[nmot] {nullptr, nullptr};            
    };


} // namespace embot::app::bldc::mbd::interface {

#endif // #if (MBD_API_VERSION == MBD_API_VERSION_1_0)


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
