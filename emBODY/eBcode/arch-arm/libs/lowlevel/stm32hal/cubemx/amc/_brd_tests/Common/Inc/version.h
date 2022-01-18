/*******************************************************************************************************************//**
 * @file    version.h
 * @author  G.Zini
 * @version 1.0
 * @date    2021 June, 7
 * @brief   Version and revision constants
 **********************************************************************************************************************/

/* Define to prevent recursive inclusion ******************************************************************************/
#ifndef __VERSION_H
#define __VERSION_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ***********************************************************************************************************/
#include "main.h"
#include "utilities.h"
#include "stm32h745xx.h"


/* Exported macro *****************************************************************************************************/

/* Program version and revision */
#define __SW_VERSION__      1
#define __SW_REVISION__     0

/* Board hardware configuration */
#define __HW_VERSION__      (((VER0_GPIO_Port->IDR)&(VER1_Pin|VER0_Pin))>>LSB(VER0_Pin))

/* Microcontroller ID:
 *  0x0450      STM32H745/755 and STM32H747/757
 */
#define __DEVICE_ID__       (DBGMCU->IDCODE & 0x00000FFFUL)

/* Microcontroller revision:
 *  0x2001      Revision X
 *  0x2003      Revision V
 */
#define __DEVICE_REVISION__ (DBGMCU->IDCODE >> 16U)
#define __DEVICE_REVX__     (0x2001)           
#define __DEVICE_REVV__     (0x2003)           

/* Pointer to the unique ID (UID[])
 * NOTE: Unique ID array contains 3 uint32_t values. 
 * The first UID[0] varies most rapidly
 */
#define UID                 ((uint32_t *)(UID_BASE))

#ifdef DUAL_BANK
  #define FLASH_BANK_NUMBER   (2)
  #define FLASH_BANK_TYPE     "dual-bank"
#else
  #define FLASH_BANK_NUMBER   (1)
  #define FLASH_BANK_TYPE     "single-bank"
#endif

#ifdef __cplusplus
  } /* extern "C" */
#endif

#endif /* __VERSION_H */
/* END OF FILE ********************************************************************************************************/
