
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#ifndef MOTORHAL_CONFIG_H
#define MOTORHAL_CONFIG_H

#ifdef __cplusplus
    extern "C" {
#endif
        
        
#if !defined(USE_STM32HAL)   
#define USE_STM32HAL
#endif        

#define MOTORHALCONFIG_DONTUSE_CONSOLE
#define MOTORHALCONFIG_DONTUSE_FLASH
#define MOTORHALCONFIG_DONTUSE_UTILITIES
#define MOTORHALCONFIG_DONTUSE_LED
#define MOTORHALCONFIG_DONTUSE_TESTS       
#define MOTORHALCONFIG_ADD_IRQHANDLERS   
#define MOTORHALCONFIG_REDEFINE_RTOS

#define MOTORHALCONFIG_DONTUSE_RUNTIMECURR_FILTERING

#if defined(MOTORHALCONFIG_DONTUSE_FLASH)
        
#include "pwm.h"
#include "encoder.h"
#include "analog.h"
typedef struct
{
    pwm_ConfTypeDef pwm;
    encoder_ConfTypeDef encoder;
    analogConfTypeDef analog;
} MainConfTypeDef;

extern MainConfTypeDef MainConf;

#endif     


#if defined(MOTORHALCONFIG_DONTUSE_UTILITIES)

// Get the MSB of an integer. The compiler generates an immediate constant value when the argument is a constant
// value known at compile time. Use macro msb(x) for not constant values
// @param   x   Integer constant value
// @return      Bit number of the left-most not-zero bit of the argument, or -1 when the argument is 0
#define MSB(x)      ((x)?MSB$64(((uint64_t)(x))):-1)
#define MSB$64(x)   (x&0xFFFFFFFF00000000LL?(32+MSB$32(x>>32)):MSB$32(x))
#define MSB$32(x)   (x&0x00000000FFFF0000LL?(16+MSB$16(x>>16)):MSB$16(x))
#define MSB$16(x)   (x&0x000000000000FF00LL?(8+MSB$8(x>>8)):MSB$8(x))
#define MSB$8(x)    (x&0x00000000000000F0LL?(4+MSB$4(x>>4)):MSB$4(x))
#define MSB$4(x)    (x&0x000000000000000CLL?(2+MSB$2(x>>2)):MSB$2(x))
#define MSB$2(x)    (x&0x0000000000000002LL?1:0)

#endif // MOTORHALCONFIG_DONTUSE_UTILITIES


#if defined(MOTORHALCONFIG_REDEFINE_RTOS)

void taskENTER_CRITICAL(void);
void taskEXIT_CRITICAL(void);
void vTaskDelay(uint32_t ms);

#endif

#if defined(MOTORHALCONFIG_DONTUSE_LED) 

enum { LED2 = 0 };
void ledSet(uint32_t led, uint32_t v);

#endif

#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */

#endif // MOTORHAL_CONFIG_H

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

