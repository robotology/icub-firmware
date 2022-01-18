/*******************************************************************************************************************//**
 * @file    hwsemphr.h
 * @author  G.Zini
 * @version 1.0
 * @date    2021 August,25
 * @brief   Hardware semaphores
 **********************************************************************************************************************/

/* Define to prevent recursive inclusion */
#ifndef __HWSEMPHR_H
#define __HWSEMPHR_H

#ifdef __cplusplus
    extern "C" {
#endif


/* Includes ***********************************************************************************************************/
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include "FreeRTOS.h"
#include "semphr.h"


/* Public macros ******************************************************************************************************/

/* Constant for core dependant compilation */
#if defined(CORE_CM7)
#define HSEM_R_COREID_CURRENT   (HSEM_CPU1_COREID << HSEM_R_COREID_Pos)
#else  /* CORE_CM4 */
#define HSEM_R_COREID_CURRENT   (HSEM_CPU2_COREID << HSEM_R_COREID_Pos)
#endif /* CORE_CM4 */

/* Mask for significative fields in HSEM->R register */
#define HSEM_R_MASK             (HSEM_R_LOCK|HSEM_R_COREID|HSEM_R_PROCID)

/* Timeout value for HwSemphr_Lock() functions */
#define HWSEMPHR_WAIT_FOREVER   (portMAX_DELAY)



/* Public typedefs ****************************************************************************************************/

/* Pointer to the callback function */
typedef void (*HwSemphrCallback_t)(uint32_t SemMask);


/* Inline functions ***************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   Check the current core and the current process are the owner of a semaphore
 * @param   SemId   Identifier of the semaphore [0..31]
 * @param   ProdId  Identifier of the process [0..255]
 * @retval  True when the current core and the current process are the owner of the semaphore. False otherwise
 */
__STATIC_INLINE uint32_t HwSemphrIsLocked(uint32_t SemId, uint32_t ProcId)
{
    /* Check LOCK, COREID and PROCID for the selected semaphore */
    return ((HSEM->R[SemId] & HSEM_R_MASK) == (HSEM_R_LOCK|HSEM_R_COREID_CURRENT|ProcId))? 1 : 0;
}


/* Public Functions ***************************************************************************************************/

/* Functions to register/unregister the callback function associated to a semaphore */
extern HAL_StatusTypeDef HwSemphr_RegisterCallback(uint32_t SemId, HwSemphrCallback_t callbk);
extern void HwSemphr_UnRegisterCallback(uint32_t SemId);
extern HAL_StatusTypeDef HwSemphr_Lock(SemaphoreHandle_t RtosSem, uint32_t SemId, uint32_t ProcId, TickType_t Timeout);
extern HAL_StatusTypeDef HwSemphr_Release(SemaphoreHandle_t RtosSem, uint32_t SemId, uint32_t ProcId);

extern uint32_t HwSemphrExchange(uint32_t SemId, uint32_t ProcId, volatile uint32_t *pShared, uint32_t Value);
extern uint32_t HwSemphrOr(uint32_t SemId, uint32_t ProcId, volatile uint32_t *pShared, uint32_t Value);
extern uint32_t HwSemphrAnd(uint32_t SemId, uint32_t ProcId, volatile uint32_t *pShared, uint32_t Value);
extern int32_t HwSemphrAdd(uint32_t SemId, uint32_t ProcId, volatile int32_t *pShared, int32_t Value);


#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */

#endif /* __HWSEMPHR_H */
/* END OF FILE ********************************************************************************************************/
