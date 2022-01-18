/*******************************************************************************************************************//**
 * @file    led.h
 * @author  G.Zini
 * @version 1.0
 * @date    2021 June, 29
 * @brief   LED indicators management
 **********************************************************************************************************************/

/* Define to prevent recursive inclusion */
#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
    extern "C" {
#endif


/* Includes ----------------------------------------------------------------------------------------------------------*/
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include "main.h"
#include "stm32h7xx.h"

/* Exported macro ----------------------------------------------------------------------------------------------------*/

/* For the sake of brevity */
#define LED1    (nLED1_Pin)
#define LED2    (nLED2_Pin)
#define LED3    (nLED3_Pin)
#define LED4    (nLED4_Pin)
#define LED5    (nLED5_Pin)
#define LED6    (nLED6_Pin)

#define LED_MASK (nLED1_Pin|nLED2_Pin|nLED3_Pin|nLED4_Pin|nLED5_Pin|nLED6_Pin)
#define LED_PORT (GPIOH)
#define LED_ALL  (LED_MASK)
#define LED_NONE (0)

/* PCB corrispondence */
#define DL9     (nLED1_Pin) /* ORANGE Near CAN #1 J11 */
#define DL10    (nLED2_Pin) /* ORANGE Near CAN #2 J12 */
#define DL3     (nLED3_Pin) /* GREEN */
#define DL4     (nLED4_Pin) /* GREEN */
#define DL8     (nLED5_Pin) /* RED */
#define DL5     (nLED6_Pin) /* RED */


/* Exported inline functions -----------------------------------------------------------------------------------------*/

/*******************************************************************************************************************//**
 * @brief   Switch ON or OFF any LED combination of the board. The function is executed in a single write operation
 * @param   on    LEDs to turn ON. Any combination of LED1..LED6 is allowed. 
 * @param   off   LEDs to turn OFF. Any combination of LED1..LED6 is allowed.
 * @return  void
 */
__STATIC_INLINE void LedOnOff(uint32_t on, uint32_t off)
{
  LED_PORT->BSRR = ((on & LED_MASK)<<16) | (off & LED_MASK);
}  

/*******************************************************************************************************************//**
 * @brief   Toggle ON and OFF any LED combination
 * @param   mask  LEDs to turn ON. Any combination of nLED1_pin, nLED2_pin and nLED3_pin is allowed
 * @return  void
 */
__STATIC_INLINE void LedToggle(uint32_t mask)
{
  mask &= LED_MASK;
  LED_PORT->BSRR = (mask << 16) | (mask & ~LED_PORT->ODR);
}

/*******************************************************************************************************************//**
 * @brief   Read the LED status
 * @param   mask of the LEDs to read. Any combination of nLED1_pin, nLED2_pin and nLED3_pin is allowed
 * @return  void
 */
__STATIC_INLINE uint32_t LedRead(uint32_t mask)
{
  mask &= LED_MASK;
  return (mask & ~LED_PORT->ODR);
}


/* Exported variables ------------------------------------------------------------------------------------------------*/
/* Exported functions prototypes -------------------------------------------------------------------------------------*/

extern void LedTest(void);

#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */

#endif /* __LED_H */
/* END OF FILE ********************************************************************************************************/
