/*******************************************************************************************************************//**
 * @file    shared.c
 * @author  G.Zini
 * @version 1.0
 * @date    2021 July, 14
 * @brief   Shared variables
 **********************************************************************************************************************/

/* Includes ***********************************************************************************************************/
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "shared.h"
#include "hwsemphr.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "console.h"


/* Local macros *******************************************************************************************************/

/* Semaphores related constants */
#define SHARED_FIFO_LENGTH  (128)
#define SHARED_HSEM_ID      (1)


/* Core-indipendent constant definitions */
#if defined CORE_CM7
  #define SharedTxFifo      (SharedMailbox.cm4)
  #define SharedRxFifo      (SharedMailbox.cm7)
  #define SHARED_TXPROC_ID  (100)
  #define SHARED_RXPROC_ID  (101)
#elif defined CORE_CM4
  #define SharedTxFifo      (SharedMailbox.cm7)
  #define SharedRxFifo      (SharedMailbox.cm4)
  #define SHARED_TXPROC_ID  (200)
  #define SHARED_RXPROC_ID  (201)
#else
  #error CORE_CM7 or CORE_CM4 must be defined
#endif


/* Local typedefs *****************************************************************************************************/

typedef struct 
{
    uint32_t frst;
    uint32_t nmbr;
    uint8_t  fifo[SHARED_FIFO_LENGTH];
} SharedFifo_t;


typedef struct
{
    SharedFifo_t cm4;
    SharedFifo_t cm7;
} SharedMailbox_t;


/* Local variables ****************************************************************************************************/

/* RTOS semaphores (counting type) */
static SemaphoreHandle_t SharedTxSemaphore;
static SemaphoreHandle_t SharedRxSemaphore;


/* Public Variables ***************************************************************************************************/

/* Tx and Rx Buffers shared between cores */
SharedMailbox_t SharedMailbox __attribute__((section ("Shared$$Sections")));


/* Local functions ****************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   Check the status of a specified FIFO memory
 * @param   pFifo   Pointer to a SharedFifo_t structure holding the FIFO
 * @retval  HAL_OK      The FIFO is not yet FULL. A new data can be stored
 *          HAL_ERROR   The FIFO is FULL
 */
static HAL_StatusTypeDef SharedFifoNotFull(SharedFifo_t *pFifo)
{
    return (SHARED_FIFO_LENGTH > pFifo->nmbr)? HAL_OK : HAL_ERROR;
}


/*******************************************************************************************************************//**
 * @brief   Check the status of a specified FIFO memory
 * @param   pFifo   Pointer to a SharedFifo_t structure holding the FIFO
 * @retval  HAL_OK      The FIFO is not yet EMPTY. A new data can be read
 *          HAL_ERROR   The FIFO is EMPTY
 */
static HAL_StatusTypeDef SharedFifoNotEmpty(SharedFifo_t *pFifo)
{
    return (0 != pFifo->nmbr)? HAL_OK : HAL_ERROR;
}

/*******************************************************************************************************************//**
 * @brief   Take the SHARED_HSEM_ID hardware semaphore and write a uint8_t value into the specified FIFO memory.
 *          WARNING: the function is blocking
 * @param   pFifo   Pointer to a SharedFifo_t structure holding the destination FIFO
 * @param   Value   New value to be stored in the destination FIFO
 * @retval  HAL_OK      Function terminated without errors
 *          HAL_BUSY    FIFO Full
 *          HAL_ERROR   Argument(s) error
 */
static HAL_StatusTypeDef SharedFifoTx(volatile SharedFifo_t *pFifo, uint8_t Value)
{
    uint32_t index;
    if (NULL != pFifo)
    {
        while (1)
        {
            taskDISABLE_INTERRUPTS();
            if (HAL_OK == HAL_HSEM_Take(SHARED_HSEM_ID, SHARED_TXPROC_ID))
            {
                __DSB();
                if (SHARED_FIFO_LENGTH > pFifo->nmbr)
                {
                    if (SHARED_FIFO_LENGTH <= (index = pFifo->frst + pFifo->nmbr)) index -= SHARED_FIFO_LENGTH;
                    pFifo->fifo[index] = Value;
                    ++(pFifo->nmbr);
                    __DSB();
                    HAL_HSEM_Release(SHARED_HSEM_ID, SHARED_TXPROC_ID);
                    taskENABLE_INTERRUPTS();
                    return HAL_OK;
                }
                HAL_HSEM_Release(SHARED_HSEM_ID, SHARED_TXPROC_ID);
                taskENABLE_INTERRUPTS();
                return HAL_BUSY;
            }
            HAL_HSEM_Release(SHARED_HSEM_ID, SHARED_TXPROC_ID);
            taskENABLE_INTERRUPTS();
            taskYIELD();
        }
    }
    return HAL_ERROR;
}

/*******************************************************************************************************************//**
 * @brief   Take the SHARED_HSEM_ID hardware semaphore and read a uint8_t value out of the specified FIFO memory.
 *          WARNING: the function is blocking
 * @param   pFifo   Pointer to a SharedFifo_t structure holding the source FIFO
 * @param   pValue  Pointer to the destination variable. NULL value discard the data read from the FIFO
 * @retval  HAL_OK      Function terminated without errors
 *          HAL_BUSY    FIFO empty
 *          HAL_ERROR   Argument(s) error
 */
static HAL_StatusTypeDef SharedFifoRx(volatile SharedFifo_t *pFifo, uint8_t *pValue)
{
    if (NULL != pFifo)
    {
        while (1)
        {
            taskDISABLE_INTERRUPTS();
            if (HAL_OK == HAL_HSEM_Take(SHARED_HSEM_ID, SHARED_RXPROC_ID))
            {
                __DSB();
                if (0 < pFifo->nmbr)
                {
                    if (NULL != pValue) *pValue = pFifo->fifo[pFifo->frst];
                    if (SHARED_FIFO_LENGTH <= ++(pFifo->frst)) pFifo->frst = 0;
                    --(pFifo->nmbr);
                    __DSB();
                    HAL_HSEM_Release(SHARED_HSEM_ID, SHARED_RXPROC_ID);
                    taskENABLE_INTERRUPTS();
                    return HAL_OK;
                }
                HAL_HSEM_Release(SHARED_HSEM_ID, SHARED_RXPROC_ID);
                taskENABLE_INTERRUPTS();
                return HAL_BUSY;
            }
            HAL_HSEM_Release(SHARED_HSEM_ID, SHARED_RXPROC_ID);
            taskENABLE_INTERRUPTS();
            taskYIELD();
        }
    }
    return HAL_ERROR;
}


/* Callback functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   Callback function. This function is called by the HAL_HSEM_FreeCallback() that is called by the HSEM
 *          interrupt handler
 * @param   SemId   Not used
 * @retval  void
 */
void SharedCallback(uint32_t SemId)
{
    UNUSED(SemId);
    BaseType_t Woken = pdFALSE;
    if (HAL_OK  == SharedFifoNotEmpty(&SharedRxFifo)) xSemaphoreGiveFromISR(SharedRxSemaphore, &Woken);
    if (HAL_OK  == SharedFifoNotFull(&SharedTxFifo))  xSemaphoreGiveFromISR(SharedTxSemaphore, &Woken);
    portYIELD_FROM_ISR(Woken);
}


/* Public functions ***************************************************************************************************/


/*******************************************************************************************************************//**
 * @brief   Send a uint8_t to the other CORE
 * @param   Value       uint8_t value to be sent to the other core
 * @retval  HAL_OK      Function terminated without errors
 *          HAL_TIMEOUT Timeout period elapsed (FIFO full)
 *          HAL_ERROR   Argument(s) error
 */
HAL_StatusTypeDef SharedTxChar(uint8_t Value, TickType_t Timeout)
{
    HAL_StatusTypeDef Result;
    while (HAL_BUSY == (Result = SharedFifoTx(&SharedTxFifo, Value)))
    {
        if (pdPASS != xSemaphoreTake(SharedTxSemaphore, Timeout)) return HAL_TIMEOUT;
    }
    return Result;
}


/*******************************************************************************************************************//**
 * @brief   Receive a uint8_t from the other CORE
 * @param   pValue      Pointer to the uint8_t destination variable
 * @retval  HAL_OK      Function terminated without errors
 *          HAL_TIMEOUT Timeout period elapsed (FIFO empty)
 *          HAL_ERROR   Argument(s) error
 */
HAL_StatusTypeDef SharedRxChar(uint8_t *pValue, TickType_t Timeout)
{
    HAL_StatusTypeDef Result;
    do
    {
        if (pdPASS != xSemaphoreTake(SharedRxSemaphore, Timeout)) return HAL_TIMEOUT;
    } while (HAL_BUSY == (Result = SharedFifoRx(&SharedRxFifo, pValue)));
    return Result;
}

/*******************************************************************************************************************//**
 * @brief   Init the shared message management system
 * @param   void
 * @retval  HAL_OK      No errors detected
 *          HAL_ERROR   Error(s) detected. Most probably the memory ran out of space
 */
HAL_StatusTypeDef SharedInit(void)
{
    /* Create the RTOS semaphores and the hardware semaphore */
    if  ((NULL != (SharedRxSemaphore = xSemaphoreCreateCounting(1, 0))) &&
         (NULL != (SharedTxSemaphore = xSemaphoreCreateCounting(1, 1))) &&
         (HAL_OK == HwSemphr_RegisterCallback(SHARED_HSEM_ID, SharedCallback)))
    {
        /* All done */
        return HAL_OK;
    }
    /* Out of memory */
    return HAL_ERROR;
}


#if defined CORE_CM7
__attribute__((noreturn)) void SharedTransmitTask(void *arg)
{
    char ch;
    SharedTxChar('?', HWSEMPHR_WAIT_FOREVER);
    while(1)
    {
        ch = coRxChar();
        SharedTxChar((uint8_t)ch, HWSEMPHR_WAIT_FOREVER);
    }
}

HAL_StatusTypeDef SharedTest(void)
{
    char ch;
    HAL_StatusTypeDef Result;
    TaskHandle_t SharedTransmitHandle;
    if (pdPASS == xTaskCreate(SharedTransmitTask, "SharedTransmitTask", 128, NULL, 
                               uxTaskPriorityGet(NULL), &SharedTransmitHandle))
    {
         while (1)
        {
            SharedRxChar((uint8_t *)&ch, HWSEMPHR_WAIT_FOREVER);
            if ('\0' == ch) {Result = HAL_OK; break;}
            coTxChar(ch);
        }
        vTaskDelete(SharedTransmitHandle);
    } else Result = HAL_ERROR;
    return Result;
}
#elif defined CORE_CM4
bool SharedRxReady(void)
{
    return (0 < SharedRxFifo.nmbr)? true : false;
}

bool SharedTxReady(void)
{
    return (SHARED_FIFO_LENGTH > SharedTxFifo.nmbr)? true : false;
}
#endif


/* END OF FILE ********************************************************************************************************/
