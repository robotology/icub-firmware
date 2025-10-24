
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_BLDC_MBD_API_H_
#define __EMBOT_APP_BLDC_MBD_API_H_

#include "embot_core.h"

#include "stdint.h" // because i am annoyed by seeing file <cstdint> non recognised.



//#define MBD_API_VERSION_draft

#define MBD_API_VERSION_1_0

#if defined(MBD_API_VERSION_1_0)
#define MBD_API_standalone_usage
#endif


#if defined(MBD_API_VERSION_1_0)


#if defined(MBD_API_standalone_usage)

    // in standalone usage we need to cut out everything but the two .h files inside the folder mbd-api-v1  
//    using boolean_T = bool;
    using real32_T = float;    
    using uint8_T = uint8_t;
    using uint32_T = uint32_t;
    using int16_T = std::int16_t;    

    #define N_MOTORS 2
    #define MAX_EVENTS_PER_TICK 4


#include "mbd-api-v1/iterative_motion_controller_types.h"
#include "mbd-api-v1/iterative_motion_controller.h"

#else

#include "iterative_motion_controller_types.h"
#include "iterative_motion_controller.h"
#endif

namespace embot::app::bldc::mbd::interface {
    
    //  the types inside _types.h that we need 
    
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
    


#if 0

note:

    cambi in output
    
    ExtY_iterative_motion_controller_T ha aggiunto:

        BUS_MESSAGES_TX Messages[N_MOTORS];  // '<Root>/Messages'
        BUS_STATUS_TX MessagesFlags[N_MOTORS];// '<Root>/MessagesFlags'
   
   ed ha modificato:
        ActuatorConfiguration ConfigurationParameters[N_MOTORS];// '<Root>/ConfigurationParameters'    
   dove forse stanno le cose che ci servono..     

#endif

#if 0    
    ActuatorConfiguration contains:
    - Thresholds:
      - jntVelMax
      - motorNominalCurrents
      - motorPeakCurrents, etc....
      
    - PIDsConfiguration:
      - currentPID,
      - velocityPID
      - positionPID
      
    - MotorConfiguration:
      - MotorConfigurationExternal
      - Kbemf, Rphase, etc.
#endif


    
    struct State
    {
        static constexpr uint8_t nmot {2};
        ActuatorConfiguration* actcfg[nmot] {nullptr, nullptr};
        BUS_MESSAGES_TX* messages[nmot] {nullptr, nullptr};
        BUS_STATUS_TX* messagesflags[nmot] {nullptr, nullptr};
        
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
        
        constexpr bool isvalid() const
        {
            return nullptr != actcfg[0];
        }
    };


} // namespace embot::app::bldc::mbd::interface {

#endif // #if defined(MBD_API_VERSION_1_0)

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
