
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_PROT_CAN_MOTOR_BOARD_H_
#define __EMBOT_PROT_CAN_MOTOR_BOARD_H_

#include "embot_core.h"
#include "embot_core_binary.h"
#include "embot_prot_can.h"
#include "embot_prot_can_motor.h"

#include <cstring>


namespace embot::prot::can::motor::board  {
    
    
    // in here we define content that depends on the board.
    // for instance to fill a flag mask that has values that heavily depend on the board.
    
    namespace foc {   
   
        // for the foc board, use the following to manage Message_STATUS::Info::quadencoderstate 
        struct QEstate2FOC : embot::core::binary::mask::Mask<uint8_t>
        {
            #if 0
                 pos:   76543210
                vals:   ---CPISD
            #endif         
            enum class Flag : uint8_t
            {
                dirty                   = 0,
                stuck                   = 1,
                index_broken            = 2,
                phase_broken            = 3,
                not_calibrated          = 4
            };
            
            constexpr QEstate2FOC() = default;
            constexpr QEstate2FOC(uint8_t m) : embot::core::binary::mask::Mask<uint8_t>(m) {}
                
            QEstate2FOC(const std::initializer_list<Flag> &flags) : embot::core::binary::mask::Mask<uint8_t>(0) 
            {
                for(const auto &f : flags) set(f);        
            }
                                
            void set(Flag f)
            {
                embot::core::binary::mask::Mask<uint8_t>::set(embot::core::tointegral(f));
            }
            
            bool check(Flag f)
            {
                return embot::core::binary::mask::Mask<uint8_t>::check(embot::core::tointegral(f));
            }     
        };        


        // for the foc board, use the following to manage Message_STATUS::Info::motorfaultstate 
        // from eOmc_motorFaultState_t
      
        // flags used by the board 2foc to express its fault state
        // they are in this strange ordee because for their implementation it was used rhe  
        // struct eOmc_motorFaultState_t w/ bitfields that align like that    
        struct MotorFaultState2FOC : embot::core::binary::mask::Mask<uint32_t>
        {
            // contains the position of the flags inside a uint32_t mask
            // as the boatd 2foc uses
            enum class Flag : uint8_t
            {
                // they end up in B0
                ExternalFaultAsserted   = 7,
                UnderVoltageFailure     = 6,
                OverVoltageFailure      = 5,
                OverCurrentFailure      = 4,
                DHESInvalidValue        = 3,
                AS5045CSumError         = 2,
                DHESInvalidSequence     = 1,
                CANInvalidProtocol      = 0,
                // they end up in B1
                CAN_BufferOverRun       = 15,
                SetpointExpired         = 14,
                CAN_TXIsPasv            = 13,
                CAN_RXIsPasv            = 12,
                CAN_IsWarnTX            = 11,
                CAN_IsWarnRX            = 10,
                OverHeatingFailure      = 9,
                unused                  = 8,
                // they end up in B2
                ADCCalFailure           = 23, 
                I2TFailure              = 22,                     
                EMUROMFault             = 21,
                EMUROMCRCFault          = 20,
                EncoderFault            = 19,
                FirmwareSPITimingError  = 18,		
                AS5045CalcError         = 17,
                FirmwarePWMFatalError   = 16,
                // they end up in B3
                CAN_TXWasPasv           = 31,
                CAN_RXWasPasv           = 30,
                CAN_RTRFlagActive       = 29,
                CAN_WasWarn             = 28,
                CAN_DLCError            = 27,
                SiliconRevisionFault    = 26,
                PositionLimitUpper      = 25, 
                PositionLimitLower      = 24
            };
                                
            constexpr MotorFaultState2FOC() = default;
            constexpr MotorFaultState2FOC(uint32_t m) : embot::core::binary::mask::Mask<uint32_t>(m) {}
            MotorFaultState2FOC(const std::initializer_list<Flag> &flags) : embot::core::binary::mask::Mask<uint32_t>(0) 
            {
                for(const auto &f : flags) set(f);        
            }   
            
            void set(Flag f)
            {
                embot::core::binary::mask::Mask<uint32_t>::set(embot::core::tointegral(f));
            }
            
            bool check(Flag f)
            {
                return embot::core::binary::mask::Mask<uint32_t>::check(embot::core::tointegral(f));
            }            
        };   
    
    } //     namespace foc {   
    
} // namespace embot::prot::can::motor::board {


          
    
    
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
