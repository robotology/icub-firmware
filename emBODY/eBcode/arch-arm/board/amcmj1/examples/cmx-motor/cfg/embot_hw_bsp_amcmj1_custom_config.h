
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Kevin Sangalli
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_BSP_AMCMJ1_CUSTOM_CONFIG_H_
#define __EMBOT_HW_BSP_AMCMJ1_CUSTOM_CONFIG_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"

#include "stm32hal.h"



//#if !defined(EMBOT_HW_BSP__appl_MOT) || !defined(STM32HAL_BOARD_AMCMJ1)
//    #error this bsp config is for amcmj1.appl.mot for either cm7 or cm4 slave
//#endif
//    
//#if !defined(EMBOT_CORE_slave)
//    #error amcmj1.appl.mot must be slave
//#endif

//#if 0
//the appl.yri must:
//- manage EEPROM 
//  EMBOT_ENABLE_hw_eeprom, EMBOT_ENABLE_hw_spi, EMBOT_ENABLE_hw_spi_three 
//- manage ICC 
//  EMBOT_ENABLE_hw_mtx, EMBOT_ENABLE_hw_icc_sig, EMBOT_ENABLE_hw_icc_mem, EMBOT_ENABLE_hw_icc_ltr
//- motor
//  EMBOT_ENABLE_hw_motor_bldc, EMBOT_ENABLE_hw_motor_bldc_pwm, EMBOT_ENABLE_hw_motor_bldc_hall, EMBOT_ENABLE_hw_motor_bldc_adc,
//  EMBOT_ENABLE_hw_motor_bldc_qenc, EMBOT_ENABLE_hw_analog, EMBOT_ENABLE_hw_motor_bldc_extfault
//- adc factory calibration. on cm7 it is retrieved from rom, on cm4 it is asked to other core via icc.
//  EMBOT_CONFIG_hw_adc_bsp_getfactorycalibration_useROM (if STM32HAL_CORE_CM7), EMBOT_CONFIG_hw_adc_bsp_getfactorycalibration_useICC
//- perfom actions on specialize()  
//  EMBOT_ENABLE_hw_bsp_specialize
//  
//- it also needs CAN (for now): CAN + 5V
//  EMBOT_ENABLE_hw_can, EMBOT_ENABLE_hw_can_5V
//  
//#endif


    
//    #if defined(STM32HAL_CORE_CM7)  
//        #define EMBOT_REDEFINE_hw_bsp_DRIVER_setuniqueid
//    #endif
        
    
    #define EMBOT_ENABLE_hw_bsp_specialize  

//    #define EMBOT_ENABLE_hw_mtx
//    #define EMBOT_ENABLE_hw_icc_sig
//    #define EMBOT_ENABLE_hw_icc_mem
//    #define EMBOT_ENABLE_hw_icc_ltr
    
//    #define EMBOT_ENABLE_hw_can    
//    #define EMBOT_ENABLE_hw_can_one    
//    #define EMBOT_ENABLE_hw_can_5V


    #undef EMBOT_ENABLE_hw_motor
    #define EMBOT_ENABLE_hw_motor_bldc
    #if defined(EMBOT_ENABLE_hw_motor_bldc)       
        #define EMBOT_ENABLE_hw_motor_bldc_pwm
        #define EMBOT_ENABLE_hw_motor_bldc_hall
        #define EMBOT_ENABLE_hw_motor_bldc_adc
        #define EMBOT_ENABLE_hw_motor_bldc_qenc
        #define EMBOT_ENABLE_hw_analog
        #define EMBOT_ENABLE_hw_motor_bldc_extfault
        // on cm7 the adc factory calibration needed by hw_analog is read from rom, on cm4 from eeprom 
        #if defined(STM32HAL_CORE_CM7) 
            #define EMBOT_CONFIG_hw_adc_bsp_getfactorycalibration_useROM
        #else    
//            #define EMBOT_CONFIG_hw_adc_bsp_getfactorycalibration_useICC
        #endif        
    #endif
  

//    #if defined(EMBOT_REDEFINE_hw_bsp_DRIVER_setuniqueid) || defined(EMBOT_ENABLE_hw_adc_bsp_getfactorycalibration_fromEEPROM) || defined(EMBOT_CONFIG_hw_adc_bsp_getfactorycalibration_useEEPROM)        
//        #define EMBOT_ENABLE_hw_spi
//        #define EMBOT_ENABLE_hw_spi_three    
//        #define EMBOT_ENABLE_hw_eeprom      
//    #endif

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


