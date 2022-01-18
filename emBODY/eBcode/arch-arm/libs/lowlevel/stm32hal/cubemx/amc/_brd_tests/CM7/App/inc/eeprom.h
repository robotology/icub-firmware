/*******************************************************************************************************************//**
 * @file    led.h
 * @author  G.Zini
 * @version 1.0
 * @date    2021 June, 29
 * @brief   LED indicators management
 **********************************************************************************************************************/

/* Define to prevent recursive inclusion */
#ifndef __EEPROM_H
#define __EEPROM_H

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

/* Bit values in EEPROM status register */
#define EE_SR_SRWD  (0x80)
#define EE_SR_BP1   (0x08)
#define EE_SR_BP0   (0x04)
#define EE_SR_WEL   (0x02)
#define EE_SR_WIP   (0x01)

/* Values for BP1 and BP0 bits in status register */
#define EE_SR_BP_MASK       (EE_SR_BP1|EE_SR_BP0)
#define EE_SR_BP_NONE       (0|0)
#define EE_SR_BP_TOPQUARTER (0|EE_SR_BP0)
#define EE_SR_BP_TOPHALF    (EE_SR_BP1|0)
#define EE_SR_BP_ALL        (EE_SR_BP1|EE_SR_BP0)


/* Exported functions prototypes -------------------------------------------------------------------------------------*/

extern HAL_StatusTypeDef EeInit(void);
extern HAL_StatusTypeDef EeWriteEnableLatch(FunctionalState state);
extern HAL_StatusTypeDef EeReadStatus(uint8_t *pStatus);
extern HAL_StatusTypeDef EeWriteStatus(uint8_t Status);
extern HAL_StatusTypeDef EeReadData(uint16_t adr, void *ptr, size_t size);
extern HAL_StatusTypeDef EeWriteData(uint16_t adr, const void *ptr, size_t size);

extern void EeTest(void);


#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */

#endif /* __LED_H */
/* END OF FILE ********************************************************************************************************/
