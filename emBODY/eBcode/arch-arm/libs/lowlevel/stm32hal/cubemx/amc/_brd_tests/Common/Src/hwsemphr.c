/*******************************************************************************************************************//**
 * @file    hwsemphr.c
 * @author  G.Zini
 * @version 1.0
 * @date    2021 August,25
 * @brief   Hardware semaphores
 **********************************************************************************************************************/

/* Includes ***********************************************************************************************************/
#include <string.h>
#include "hwsemphr.h"

/* Maximum number of TAKE */
#define HWSEMPHR_MAX_RETRY  (10)


/* Local variables ****************************************************************************************************/

/* Table of callback functions associated to hardware semaphores */
/*static*/ HwSemphrCallback_t HwSemphrCallbackTable[HSEM_SEMID_MAX+1] = {0};


/* Callback functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   Semaphore Released Callback (overrides functions in stm32hxx_hal_hsem)
 * @param   SemMask     Mask of Released semaphores
 * @retval  None
 */
void HAL_HSEM_FreeCallback(uint32_t SemMask)
{
    uint32_t SemId;
    HwSemphrCallback_t callbk;
    /* Scan all hardware semaphores */
    for (SemId = HSEM_SEMID_MIN; SemId <= HSEM_SEMID_MAX; SemId++)
    {
        /* Verify this RTOS semaphore is to release */
        if ((0 != (SemMask & __HAL_HSEM_SEMID_TO_MASK(SemId))) && 
            (NULL != (callbk = HwSemphrCallbackTable[SemId])))
        {
            /* Launch the callback function */
            (*callbk)(SemId);
        }
    }
    /* Enable interrupt, because HAL disables it */
    HAL_HSEM_ActivateNotification(SemMask);
}


/* Public functions ***************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   Register a callback function associated to an hardware semaphore 
 * @param   SemId   Identifier of the semaphore (0..31)
 * @param   CallBk  Pointer to the callback function
 * @retval  HAL_OK      Registration done without errors
 *          HAL_ERROR   Arguments error or function already registered
 */
HAL_StatusTypeDef HwSemphr_RegisterCallback(uint32_t SemId, HwSemphrCallback_t CallBk)
{
    /* Check arguments */
    if ((HSEM_SEMID_MAX >= SemId) && (NULL != CallBk) && (NULL == HwSemphrCallbackTable[SemId]))
    {
        /* Register the callback function */
        HwSemphrCallbackTable[SemId] = CallBk;

        taskDISABLE_INTERRUPTS();
        /* Enable the interrupt and associated callback functions */
        HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(SemId));
        taskENABLE_INTERRUPTS();

        /* Done */
        return HAL_OK;
    }
    /* Already registered */
    return HAL_ERROR;
}


/*******************************************************************************************************************//**
 * @brief   Remove a previously registered function
 * @param   SemId   Identifier of the semaphore (0..31)
 * @retval  none
 */
void HwSemphr_UnRegisterCallback(uint32_t SemId)
{
    /* Check arguments */
    if (HSEM_SEMID_MAX >= SemId)
    {
        taskDISABLE_INTERRUPTS();
        /* Disable the notification */
        HAL_HSEM_DeactivateNotification(__HAL_HSEM_SEMID_TO_MASK(SemId));
        /* Remove the registered callback function */
        HwSemphrCallbackTable[SemId] = NULL;
        taskENABLE_INTERRUPTS();
    }
}

/*******************************************************************************************************************//**
 * @brief   Lock an RTOS semaphore and its assiocated hardware semapore
 * @param   RtosSem     Handler of a previously defined counting RTOS semaphore
 * @param   SemId       Identifier of the hardware semaphore [0..31]
 * @param   ProcId      Identifier of the current process
 * @param   Timeout     Timeout value in ticks unit. The value WAIT_FOREVER is allowed
 * @retval  HAL_OK      Hardware and RTOS semaphores taken
 *          HAL_TIMEOUT Failed to take the RTOS semaphore within the specified timeout value
 *          HAL_BUSY    Failed to take the hardware semaphore after HSEM_SEMID_MAX retries
 *          HAL_ERROR   Arguments error
 */
HAL_StatusTypeDef HwSemphr_Lock(SemaphoreHandle_t RtosSem, uint32_t SemId, uint32_t ProcId, TickType_t Timeout)
{
    unsigned retry;
    /* Check arguments */
    if ((NULL != RtosSem) && (HSEM_SEMID_MAX >= SemId))
    {
        /* Retry if cannot take the hardware interrupt */
        for (retry = 0; HWSEMPHR_MAX_RETRY > retry ; ++retry)
        {
            /* Take the RTOS semaphore */
            if (pdPASS != xSemaphoreTake(RtosSem, Timeout)) return HAL_TIMEOUT;
            /* Take the hardware semaphore */
            if (HAL_OK == HAL_HSEM_Take(SemId, ProcId))
            {
                /* All done */
                return HAL_OK;
            }
            /* Hardware semaphore not taken, repeat */
            xSemaphoreGive(RtosSem);
        }
        /* Retries exhausted */
        return HAL_BUSY;
    }
    /* Arguments error */
    return HAL_ERROR;
}


/*******************************************************************************************************************//**
 * @brief   Release RTOS semaphore and its assiocated hardware semapore
 * @param   RtosSem     Handler of a previously defined counting RTOS semaphore
 * @param   SemId       Identifier of the hardware semaphore [0..31]
 * @param   ProcId      Identifier of the current process
 * @retval  HAL_OK      Hardware and RTOS semaphores released
 *          HAL_BUSY    The caller does not own the semaphores
 *          HAL_ERROR   Arguments error
 */
HAL_StatusTypeDef HwSemphr_Release(SemaphoreHandle_t RtosSem, uint32_t SemId, uint32_t ProcId)
{
    /* Check arguments and the locked status*/
    if ((NULL != RtosSem) && (HSEM_SEMID_MAX >= SemId))
    {
        /* Check the semaphore owner */
        if (0 != HwSemphrIsLocked(SemId, ProcId))
        {
            /* Release the hardware semaphore */
            HAL_HSEM_Release(SemId, ProcId);
            /*
             * The RTOS semaphore will be given by the callback function
             */
            /* All done */
            return HAL_OK;
        }
        /* Not locked by the caller */
        return HAL_BUSY;
    }
    /* Argument error */
    return HAL_ERROR;
}


/*******************************************************************************************************************//**
 * @brief   Take an hardware semaphore and exchange the value of a shared memory variable with a specified value, then
 *          release the semaphore
 * @param   SemId       Identifier of the hardware semaphore [0..31]
 * @param   ProcId      Identifier of the current process [0..255]
 * @param   pShared     Pointer to a uint32_t variable in shared memory area
 * @param   Value       New value to be written in *pShared
 * @retval  uint32_t    Previous value of *pShared
 */
uint32_t HwSemphrExchange(uint32_t SemId, uint32_t ProcId, volatile uint32_t *pShared, uint32_t Value)
{
    uint32_t Result;
    while (1)
    {
        __disable_irq();
        if (HAL_OK == HAL_HSEM_Take(SemId, ProcId))
        {
            Result = *pShared;
            *pShared = Value;
            HAL_HSEM_Release(SemId, ProcId);
            __enable_irq();
            return Result;
        }
        __enable_irq();
        taskYIELD();
    }
}


/*******************************************************************************************************************//**
 * @brief   Take an hardware semaphore and perform the bit-wise OR of a shared memory variable with a given value, then
 *          release the semaphore
 * @param   SemId       Identifier of the hardware semaphore [0..31]
 * @param   ProcId      Identifier of the current process [0..255]
 * @param   pShared     Pointer to a uint32_t variable in shared memory area
 * @param   Value       Value to be ORed with *pShared
 * @retval  uint32_t    Previous value of *pShared
 */
uint32_t HwSemphrOr(uint32_t SemId, uint32_t ProcId, volatile uint32_t *pShared, uint32_t Value)
{
    uint32_t Result;
    while (1)
    {
        __disable_irq();
        if (HAL_OK == HAL_HSEM_Take(SemId, ProcId))
        {
            Result = *pShared;
            *pShared = Result | Value;
            HAL_HSEM_Release(SemId, ProcId);
            __enable_irq();
            return Result;
        }
        __enable_irq();
        taskYIELD();
    }
}


/*******************************************************************************************************************//**
 * @brief   Take an hardware semaphore and perform the bit-wise AND of a shared memory variable with a given value, then
 *          release the semaphore
 * @param   SemId       Identifier of the hardware semaphore [0..31]
 * @param   ProcId      Identifier of the current process [0..255]
 * @param   pShared     Pointer to a uint32_t variable in shared memory area
 * @param   Value       Value to be ANDed with *pShared
 * @retval  uint32_t    Previous value of *pShared
 */
uint32_t HwSemphrAnd(uint32_t SemId, uint32_t ProcId, volatile uint32_t *pShared, uint32_t Value)
{
    uint32_t Result;
    while (1)
    {
        __disable_irq();
        if (HAL_OK == HAL_HSEM_Take(SemId, ProcId))
        {
            Result = *pShared;
            *pShared = Result & Value;
            HAL_HSEM_Release(SemId, ProcId);
            __enable_irq();
            return Result;
        }
        __enable_irq();
        taskYIELD();
    }
}


/*******************************************************************************************************************//**
 * @brief   Take an hardware semaphore and algebraically add a shared memory variable with a given value, then release
 *          the semaphore
 * @param   SemId       Identifier of the hardware semaphore [0..31]
 * @param   ProcId      Identifier of the current process [0..255]
 * @param   pShared     Pointer to a uint32_t variable in shared memory area
 * @param   Value       Value to be ANDed with *pShared
 * @retval  int32_t     Previous value of *pShared
 */
int32_t HwSemphrAdd(uint32_t SemId, uint32_t ProcId, volatile int32_t *pShared, int32_t Value)
{
    uint32_t Result;
    while (1)
    {
        __disable_irq();
        if (HAL_OK == HAL_HSEM_Take(SemId, ProcId))
        {
            Result = *pShared;
            *pShared = Result + Value;
            HAL_HSEM_Release(SemId, ProcId);
            __enable_irq();
            return Result;
        }
        __enable_irq();
        taskYIELD();
    }
}


/* END OF FILE ********************************************************************************************************/
