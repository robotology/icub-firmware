
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#if 0
    The files motorhal_config.[h, c] contain macros and code which cuts aways un-necessary dependencies
    for the files provided by Giorgio Zini for the motor control of the amc2c. These files are:
    enc.[c, h], pwm.[c, h], adcm.[c, h], hall.[c, h]. The dependencies cut away are those which offer
    generic services such as logging, leds, flash storage.    
#endif

// CODE SHAPER
#if defined(USE_STM32HAL)
#include "motorhal_config.h"
#endif



#if defined(MOTORHALCONFIG_DONTUSE_FLASH)

#else
#endif // MOTORHALCONFIG_DONTUSE_FLASH


#if defined(MOTORHALCONFIG_REDEFINE_RTOS)

void taskENTER_CRITICAL(void)  {}
void taskEXIT_CRITICAL(void) {}
void vTaskDelay(uint32_t ms)
{
    HAL_Delay(ms);
}

void taskDISABLE_INTERRUPTS(void) {}
void taskENABLE_INTERRUPTS(void) {}

#endif

#if defined(MOTORHALCONFIG_DONTUSE_LED) 

void ledSet(uint32_t led, uint32_t v) {}

    
void LedOnOff(uint32_t led, uint32_t v) {}
#endif


#if defined(MOTORHALCONFIG_ADD_IRQHANDLERS)

    // they are in the bps file

#endif // #if defined(MOTORHALCONFIG_ADD_IRQHANDLERS)

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
