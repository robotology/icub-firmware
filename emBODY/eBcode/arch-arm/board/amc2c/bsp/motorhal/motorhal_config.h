
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#ifndef __MOTORHAL_CONFIG_H_
#define __MOTORHAL_CONFIG_H_

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
        
#define MOTORHALCONFIG_MOT_BREAK_IRQ_remove
        
//#define MOTORHALCONFIG_enabletests
        
#define MOTORHALCONFIG_TESTPOINT_DEBUG_remove

#define MOTORHALCONFIG_DONTUSE_RUNTIMECURR_FILTERING

#if defined(MOTORHALCONFIG_DONTUSE_FLASH)
        
#include "adcm.h"
#include "enc.h"
#include "hall.h"
#include "pwm.h"

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

// marco.accame: this macro gets the number of zeros below a binary numer. as an example: LSB(0x7000) = LSV(0b0111-0000-0000-0000) = 12
// it is used to shift down a value.

// Get the LSB of an integer. The compiler generates an immediate constant value when the argument is a constant
// value known at compile time. Use macro lsb(x) for not constant values
// @param   x   Integer constant value
// @return      Bit number of the right-most not-zero bit of the argument, or -1 when the argument is 0
#define LSB(x)      ((x)?LSB$64(((uint64_t)(x))):-1)
#define LSB$64(x)   (x&0x00000000FFFFFFFFLL?(LSB$32(x)):(32+LSB$32(x>>32)))
#define LSB$32(x)   (x&0x000000000000FFFFLL?(LSB$16(x)):(16+LSB$16(x>>16)))
#define LSB$16(x)   (x&0x00000000000000FFLL?(LSB$8(x)):(8+LSB$8(x>>8)))
#define LSB$8(x)    (x&0x000000000000000FLL?(LSB$4(x)):(4+LSB$4(x>>4)))
#define LSB$4(x)    (x&0x0000000000000003LL?(LSB$2(x)):(2+LSB$2(x>>2)))
#define LSB$2(x)    (x&0x0000000000000001LL?0:1)

#endif // MOTORHALCONFIG_DONTUSE_UTILITIES


#if defined(MOTORHALCONFIG_REDEFINE_RTOS)

void taskENTER_CRITICAL(void);
void taskEXIT_CRITICAL(void);
void vTaskDelay(uint32_t ms);

void taskDISABLE_INTERRUPTS(void);
void taskENABLE_INTERRUPTS(void);

#endif

#if defined(MOTORHALCONFIG_DONTUSE_LED) 

enum { LED2 = 0, DL3 =0, DL4 = 0, DL8 = 0 };
void ledSet(uint32_t led, uint32_t v);
void LedOnOff(uint32_t led, uint32_t v);

#endif

#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */

#endif // __MOTORHAL_CONFIG_H_

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

