
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_BLDC_ENC_BSP_H_
#define __EMBOT_HW_MOTOR_BLDC_ENC_BSP_H_


#include "embot_hw_motor_bldc_enc.h"



namespace embot::hw::motor::bldc::enc::bsp {
    
    bool init(embot::hw::MOTOR m);
    bool deinit(embot::hw::MOTOR m);    
    int32_t counter(embot::hw::MOTOR m);

 
#if 0 
.\obj\h7disco.axf: Error: L6218E: Undefined symbol embot::hw::motor::bldc::enc::Enc1DeInit() (referred from embot_hw_motor_bldc_enc.o).
.\obj\h7disco.axf: Error: L6218E: Undefined symbol embot::hw::motor::bldc::enc::Enc2DeInit() (referred from embot_hw_motor_bldc_enc.o).
.\obj\h7disco.axf: Error: L6218E: Undefined symbol embot::hw::motor::bldc::enc::Enc1Init(embot::hw::MOTOR) (referred from embot_hw_motor_bldc_enc.o).
.\obj\h7disco.axf: Error: L6218E: Undefined symbol embot::hw::motor::bldc::enc::Enc2Init(embot::hw::MOTOR) (referred from embot_hw_motor_bldc_enc.o).
.\obj\h7disco.axf: Error: L6218E: Undefined symbol embot::hw::motor::bldc::enc::angle(embot::hw::MOTOR) (referred from embot_hw_motor_bldc_bsp_amcmj1.o).
.\obj\h7disco.axf: Error: L6218E: Undefined symbol embot::hw::motor::bldc::enc::GetencIndexAngle(embot::hw::MOTOR) (referred from embot_hw_motor_bldc_bsp_amcmj1.o).
#endif    
 
    
} // namespace embot::hw::motor::bldc::enc::bsp {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

