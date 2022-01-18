/*******************************************************************************************************************//**
 * @file    eeprom.c
 * @author  G.Zini
 * @version 1.0
 * @date    2021 July, 1
 * @brief   EEPROM M95512-DFMC6 management
 **********************************************************************************************************************/

/* Includes ***********************************************************************************************************/
#include <string.h>
#include "main.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "stm32h7xx_hal_spi.h"
#include "version.h"
#include "eeprom.h"
#include "console.h"

/* Private macros *****************************************************************************************************/

/* Maximum waiting time for SPI operation */
#define EE_DEFAULT_TIMEOUT  (100/*ms*/)
#define EE_WRITE_TIMEOUT    (5/*ms*/)

/* M95512-D commands */
#define EE_WREN         (0x06)
#define EE_WRDI         (0x04)
#define EE_RDSR         (0x05)
#define EE_WRSR         (0x01)
#define EE_READ         (0x03)
#define EE_WRITE        (0x02)
#define EE_RDID         (0x83)
#define EE_WRID         (0x82)
#define EE_RDLS         (0x83)
#define EE_LID          (0x82)

#define EE_PAGE_LENGTH  (128u)
#define EE_PAGE_MASK    (EE_PAGE_LENGTH-1u)


/* Private typedefs ***************************************************************************************************/

/* Private variables **************************************************************************************************/

/* Mutex semaphore to lock the SPI device */
static volatile SemaphoreHandle_t EeLockSemaphore = NULL ;
static volatile SemaphoreHandle_t EeIsrSemaphore = NULL ;
static volatile HAL_StatusTypeDef EeResult ;


/* Public variables ***************************************************************************************************/

/* Defined in main.c */
extern SPI_HandleTypeDef hspi6;


/* Interrupt handlers ------------------------------------------------------------------------------------------------*/

/*******************************************************************************************************************//**
 * @brief   SPI6 interrupt handler
 * @param   void
 * @return  void
 */
void EeSpiTransfer_Cb(SPI_HandleTypeDef *hspi)
{
    BaseType_t Woken;
    /* Transfer terminated */
    EeResult = HAL_OK;
    /* Run the scheduler if needed */
    xSemaphoreGiveFromISR(EeIsrSemaphore, &Woken);
    portYIELD_FROM_ISR(Woken);
}

void EeSpiError_Cb(SPI_HandleTypeDef *hspi)
{
    BaseType_t Woken;
    /* Transfer error */
    EeResult = HAL_ERROR;
    /* Run the scheduler if needed */
    xSemaphoreGiveFromISR(EeIsrSemaphore, &Woken);
    portYIELD_FROM_ISR(Woken);
}

/* Local functions ---------------------------------------------------------------------------------------------------*/

/*******************************************************************************************************************//**
 * @brief   Send a single 8-bits command to the EEPROM M95512-DFMC6
 * @param   cmd     8-bit Command code
 * @return  HAL_OK      Transmission terminated without errors
 *          HAL_BUSY    SPI resource busy
 *          HAL_TIMEOUT SPI resource not available or malfunctioning
 *          HAL_ERROR   Unidentified error
 */
static HAL_StatusTypeDef EeCmd(uint8_t cmd)
{
    /* Assert nSEL signal */
    HAL_GPIO_WritePin(EE_nSEL_GPIO_Port, EE_nSEL_Pin, GPIO_PIN_RESET);
    /* Start transmission */
    if (HAL_OK == HAL_SPI_Transmit_IT(&hspi6, &cmd, sizeof(uint8_t)))
    {
        /* Wait the interrupt */
        if (pdPASS == xSemaphoreTake(EeIsrSemaphore, EE_DEFAULT_TIMEOUT))
        {
            /* All done */
            EeResult = HAL_OK;
        /* Interrupt lost */
        } else EeResult = HAL_TIMEOUT;
    /* Unidentified error */
    } else EeResult = HAL_ERROR;
    /* Deassert nSEL signal */
    HAL_GPIO_WritePin(EE_nSEL_GPIO_Port, EE_nSEL_Pin, GPIO_PIN_SET);
    /* Done */
    return EeResult;
}


/*******************************************************************************************************************//**
 * @brief   Send a single 8-bits command to the EEPROM M95512-DFMC6 followed by a single 8-bit data
 * @param   cmd     8-bit Command code
 * @param   val     Value to be sent to the EEPROM
 * @return  HAL_OK      Transfer terminated without errors
 *          HAL_BUSY    SPI resource busy
 *          HAL_TIMEOUT SPI resource not available or malfunctioning
 *          HAL_ERROR   Unidentified error
 */
static HAL_StatusTypeDef EeCmdTx(uint8_t cmd, uint8_t val)
{
    /* Assert nSEL signal */
    HAL_GPIO_WritePin(EE_nSEL_GPIO_Port, EE_nSEL_Pin, GPIO_PIN_RESET);
    /* Start transmission */
    if (HAL_OK == HAL_SPI_Transmit_IT(&hspi6, &cmd, sizeof(uint8_t)))
    {
        /* Wait the interrupt */
        if (pdPASS == xSemaphoreTake(EeIsrSemaphore, EE_DEFAULT_TIMEOUT))
        {
            /* Transmit the status register */
            if (HAL_OK == HAL_SPI_Transmit_IT(&hspi6, &val, 1))
            {
                /* Wait the interrupt */
                if (pdPASS == xSemaphoreTake(EeIsrSemaphore, EE_DEFAULT_TIMEOUT))
                {
                    /* All done */
                    EeResult = HAL_OK;
                }
                /* Interrupt lost */
                else EeResult = HAL_TIMEOUT;
            }
            /* Unidentified error */
            else EeResult = HAL_ERROR;
        /* Interrupt lost */
        } else EeResult = HAL_TIMEOUT;
    /* Unidentified error */
    } else EeResult = HAL_ERROR;
    /* Deassert nSEL signal */
    HAL_GPIO_WritePin(EE_nSEL_GPIO_Port, EE_nSEL_Pin, GPIO_PIN_SET);
    /* Done */
    return EeResult;
}


/*******************************************************************************************************************//**
 * @brief   Send a single 8-bits command to the EEPROM M95512-DFMC6 and read-back an 8-bit answer
 * @param   cmd     8-bit Command code
 * @param   ptr     Pointer to an uint8_t variable where to put the received data
 * @return  HAL_OK      Transfer terminated without errors
 *          HAL_BUSY    SPI resource busy
 *          HAL_TIMEOUT SPI resource not available or malfunctioning
 *          HAL_ERROR   Unidentified error
 */
static HAL_StatusTypeDef EeCmdRx(uint8_t cmd, uint8_t *ptr)
{
    /* Assert nSEL signal */
    HAL_GPIO_WritePin(EE_nSEL_GPIO_Port, EE_nSEL_Pin, GPIO_PIN_RESET);
    /* Start transmission */
    if (HAL_OK == HAL_SPI_Transmit_IT(&hspi6, &cmd, sizeof(uint8_t)))
    {
        /* Wait the interrupt */
        if (pdPASS == xSemaphoreTake(EeIsrSemaphore, EE_DEFAULT_TIMEOUT))
        {
            /* Read the status register */
            if (HAL_OK == HAL_SPI_Receive_IT(&hspi6, ptr, 1))
            {
                /* Wait the interrupt */
                if (pdPASS == xSemaphoreTake(EeIsrSemaphore, EE_DEFAULT_TIMEOUT))
                {
                    /* All done */
                    EeResult = HAL_OK;
                }
                /* Interrupt lost */
                else EeResult = HAL_TIMEOUT;
            }
            /* Unidentified error */
            else EeResult = HAL_ERROR;
        /* Interrupt lost */
        } else EeResult = HAL_TIMEOUT;
    /* Unidentified error */
    } else EeResult = HAL_ERROR;
    /* Deassert nSEL signal */
    HAL_GPIO_WritePin(EE_nSEL_GPIO_Port, EE_nSEL_Pin, GPIO_PIN_SET);
    /* Done */
    return EeResult;
}


/*******************************************************************************************************************//**
 * @brief   Send a single 8-bits command to the EEPROM M95512-DFMC6, followed by a 16-bits address and by a block of
 *          variable length data
 * @param   cmd     8-bit Command code
 * @param   adr     16-bit Address
 * @param   ptr     Pointer to the source buffer. It can be NULL only when size is zero
 * @param   size    Size (number of bytes) of the block of data. Value zero is valid
 * @return  HAL_OK      Transfer terminated without errors
 *          HAL_BUSY    SPI resource busy
 *          HAL_TIMEOUT SPI resource not available or malfunctioning
 *          HAL_ERROR   Unidentified error
 */
static HAL_StatusTypeDef EeCmdAdrTx(uint8_t cmd, uint16_t adr, const uint8_t *ptr, size_t size)
{
    /* Adjust arguments endianess */
    adr = __REV16(adr);
    /* Assert nSEL signal */
    HAL_GPIO_WritePin(EE_nSEL_GPIO_Port, EE_nSEL_Pin, GPIO_PIN_RESET);
    /* Start transmission of the COMMAND */
    if (HAL_OK == HAL_SPI_Transmit_IT(&hspi6, &cmd, sizeof(uint8_t)) && (HAL_OK == EeResult))
    {
        /* Wait the interrupt */
        if (pdPASS == xSemaphoreTake(EeIsrSemaphore, EE_DEFAULT_TIMEOUT))
        {
            /* Start transmission of the ADDRESS */
            if (HAL_OK == HAL_SPI_Transmit_IT(&hspi6, (void *)&adr, sizeof(uint16_t)) && (HAL_OK == EeResult))
            {
                /* Wait the interrupt */
                if (pdPASS == xSemaphoreTake(EeIsrSemaphore, EE_DEFAULT_TIMEOUT))
                {
                    /* NULL data block */
                    if (0 == size) EeResult = HAL_OK;
                    /* Start transmission */
                    else if ((NULL != ptr) && 
                             (HAL_OK == HAL_SPI_Transmit_IT(&hspi6, (void *)ptr, size)) && 
                             (HAL_OK == EeResult))
                    {
                        /* Wait the interrupt */
                        if (pdPASS == xSemaphoreTake(EeIsrSemaphore, EE_DEFAULT_TIMEOUT))
                        {
                            /* All done */
                            EeResult = HAL_OK;
                        /* Interrupt lost */
                        } else EeResult = HAL_TIMEOUT;
                    /* Unidentified error */
                    } else EeResult = HAL_ERROR;
                /* Interrupt lost */
                } else EeResult = HAL_TIMEOUT;
            /* Unidentified error */
            } else EeResult = HAL_ERROR;
        /* Interrupt lost */
        } else EeResult = HAL_TIMEOUT;
    /* Unidentified error */
    } else EeResult = HAL_ERROR;
    /* Deassert nSEL signal */
    HAL_GPIO_WritePin(EE_nSEL_GPIO_Port, EE_nSEL_Pin, GPIO_PIN_SET);
    /* Done */
    return EeResult;
}


/*******************************************************************************************************************//**
 * @brief   Send a single 8-bits command to the EEPROM M95512-DFMC6, followed by a 16-bits address, then receive a block 
 *          of variable length data
 * @param   cmd     8-bit Command code
 * @param   adr     16-bit Address
 * @param   ptr     Pointer to the destination buffer. It can be NULL only when size is zero
 * @param   size    Size (number of bytes) of the block of data. Value zero is valid
 * @return  HAL_OK      Transfer terminated without errors
 *          HAL_BUSY    SPI resource busy
 *          HAL_TIMEOUT SPI resource not available or malfunctioning
 *          HAL_ERROR   Unidentified error
 */
static HAL_StatusTypeDef EeCmdAdrRx(uint8_t cmd, uint16_t adr, uint8_t *ptr, size_t size)
{
    /* Adjust arguments endianess */
    adr = __REV16(adr);
    /* Assert nSEL signal */
    HAL_GPIO_WritePin(EE_nSEL_GPIO_Port, EE_nSEL_Pin, GPIO_PIN_RESET);
    /* Start transmission of the COMMAND */
    if (HAL_OK == HAL_SPI_Transmit_IT(&hspi6, &cmd, sizeof(uint8_t)) && (HAL_OK == EeResult))
    {
        /* Wait the interrupt */
        if (pdPASS == xSemaphoreTake(EeIsrSemaphore, EE_DEFAULT_TIMEOUT))
        {
            /* Start transmission of the ADDRESS */
            if (HAL_OK == HAL_SPI_Transmit_IT(&hspi6, (void *)&adr, sizeof(uint16_t)) && (HAL_OK == EeResult))
            {
                /* Wait the interrupt */
                if (pdPASS == xSemaphoreTake(EeIsrSemaphore, EE_DEFAULT_TIMEOUT))
                {
                    /* NULL data block */
                    if (0 == size) EeResult = HAL_OK;
                    /* Start receive */
                    else if ((NULL != ptr) && 
                             (HAL_OK == HAL_SPI_Receive_IT(&hspi6, (void *)ptr, size)) && 
                             (HAL_OK == EeResult))
                    {
                        /* Wait the interrupt */
                        if (pdPASS == xSemaphoreTake(EeIsrSemaphore, EE_DEFAULT_TIMEOUT))
                        {
                            /* All done */
                            EeResult = HAL_OK;
                        /* Interrupt lost */
                        } else EeResult = HAL_TIMEOUT;
                    /* Unidentified error */
                    } else EeResult = HAL_ERROR;
                /* Interrupt lost */
                } else EeResult = HAL_TIMEOUT;
            /* Unidentified error */
            } else EeResult = HAL_ERROR;
        /* Interrupt lost */
        } else EeResult = HAL_TIMEOUT;
    /* Unidentified error */
    } else EeResult = HAL_ERROR;
    /* Deassert nSEL signal */
    HAL_GPIO_WritePin(EE_nSEL_GPIO_Port, EE_nSEL_Pin, GPIO_PIN_SET);
    /* Done */
    return EeResult;
}


/*******************************************************************************************************************//**
 * @bries   Activate/Deactivate the M95512-D nHOLD signal 
 * @param   state   DISABLE HOLD control is disabled
 *                  ENABLE  HOLD control il enabled
 * @return  void
 */
static void EeHoldControl(FunctionalState state)
{
    HAL_GPIO_WritePin(EE_nHOLD_GPIO_Port, EE_nHOLD_Pin, (DISABLE == state)? GPIO_PIN_SET : GPIO_PIN_RESET);
}


/*******************************************************************************************************************//**
 * @bries   Activate/Deactivate the M95512-D nWP signal 
 * @param   state   DISABLE Write operations are disabled
 *                  ENABLE  Write operations are enabled
 * @return  void
 */
static void EeWriteControl(FunctionalState state)
{
    HAL_GPIO_WritePin(EE_nWP_GPIO_Port, EE_nWP_Pin, (DISABLE == state)? GPIO_PIN_RESET : GPIO_PIN_SET);
}


/* Exported functions ------------------------------------------------------------------------------------------------*/

/*******************************************************************************************************************//**
 * @bries   Read the M95512-D status register
 * @param   pStatus     Pointer to the destination uint8_t variable
 * @return  HAL_OK      Operation terminated without errors
 *          HAL_BUSY    SPI resource busy
 *          HAL_TIMEOUT SPI resource not available or malfunctioning
 *          HAL_ERROR   Unidentified error
 */
HAL_StatusTypeDef EeReadStatus(uint8_t *pStatus)
{
    if (NULL != pStatus)
    {
        /* Acquire the SPI resource */
        if (pdPASS == xSemaphoreTake(EeLockSemaphore, EE_DEFAULT_TIMEOUT))
        {
            /* Read the status register */
            EeResult = EeCmdRx(EE_RDSR, pStatus);
        /* SPI bus busy */
        } else EeResult = HAL_BUSY;
        /* Relase the SPI resource */
        xSemaphoreGive(EeLockSemaphore);
    } 
    /* Arguments error */
    else EeResult = HAL_ERROR;
    /* Done */
    return EeResult;
}
    
    
/*******************************************************************************************************************//**
 * @bries   Write the M95512-D status register. The function sends the WREN command before writing and waits until the
 *          bit WIP in status register is reset after writing
 * @param   Status      Value to be written in the status register. It can be a combination of the following walues
 *                      @arg    EE_STATUS_
 * @return  HAL_OK      Operation terminated without errors
 *          HAL_BUSY    SPI resource busy
 *          HAL_TIMEOUT SPI resource not available or malfunctioning
 *          HAL_ERROR   Unidentified error
 */
HAL_StatusTypeDef EeWriteStatus(uint8_t Status)
{
    /* Acquire the SPI resource */
    if (pdPASS == xSemaphoreTake(EeLockSemaphore, EE_DEFAULT_TIMEOUT))
    {
        /* Enable write and write status register */
        if ((HAL_OK == (EeResult = EeCmd(EE_WREN))) &&
            (HAL_OK == (EeResult = EeCmdTx(EE_WRSR, Status))))
        {
            /* Less than 5 ms required */
            unsigned timeout = EE_WRITE_TIMEOUT;
            /* Read the status register */
            while ((HAL_OK == (EeResult = EeCmdRx(EE_RDSR, &Status))) && (0 != (Status & EE_SR_WIP)))
            {
                /* Check the timeout */
                if (0 == timeout--)
                {
                    /* Timeout expired */
                    EeResult = HAL_TIMEOUT;
                    break;
                }
                /* Call the scheduler */
                vTaskDelay(1);
            }
        }
    } 
    /* SPI bus busy */
    else EeResult = HAL_BUSY;
    /* Relase the SPI resource */
    xSemaphoreGive(EeLockSemaphore);
    /* Done */
    return EeResult;
}
    

/*******************************************************************************************************************//**
 * @bries   Read a block of data from the M95512-D memory
 * @param   adr         Source address of the M95512-D memory
 *          ptr         Pointer to the destination buffer
 *          size        Number of bytes to read
 * @return  HAL_OK      Operation terminated without errors
 *          HAL_BUSY    SPI resource busy
 *          HAL_TIMEOUT SPI resource not available or malfunctioning
 *          HAL_ERROR   Unidentified error
 */
HAL_StatusTypeDef EeReadData(uint16_t adr, void *ptr, size_t size)
{
    if (NULL != ptr)
    {
        /* Acquire the SPI resource */
        if (pdPASS == xSemaphoreTake(EeLockSemaphore, EE_DEFAULT_TIMEOUT))
        {
            /* Read the required block of data */
            EeResult = EeCmdAdrRx(EE_READ, adr, (uint8_t *)ptr, size);
        /* SPI bus busy */
        } else EeResult = HAL_BUSY;
        /* Relase the SPI resource */
        xSemaphoreGive(EeLockSemaphore);
    } 
    /* Arguments error */
    else EeResult = HAL_ERROR;
    /* Done */
    return EeResult;
}


/*******************************************************************************************************************//**
 * @bries   Write a block of data to the M95512-D memory. The function sends the WREN command before writing and waits
 *          until the bit WIP in status register is reset after writing. The function manages the PAGING of the EEPROM
 *          as specified by manufacturer
 * @param   adr         Destination address of the M95512-D memory
 *          ptr         Pointer to the source buffer
 *          size        Number of bytes to write
 * @return  HAL_OK      Operation terminated without errors
 *          HAL_BUSY    SPI resource busy
 *          HAL_TIMEOUT SPI resource not available or malfunctioning
 *          HAL_ERROR   Unidentified error
 */
HAL_StatusTypeDef EeWriteData(uint16_t adr, const void *ptr, size_t size)
{
    /* Check arguments */
    if ((NULL != ptr) && (0 < size))
    {
        /* Acquire the SPI resource */
        if (pdPASS == xSemaphoreTake(EeLockSemaphore, EE_DEFAULT_TIMEOUT))
        {
            /* Number of bytes to write up to the end of page */
            size_t delta = EE_PAGE_LENGTH - (adr & EE_PAGE_MASK);
            /* Loop to write page-by-page as fast as possible */
            for ( ; size ; )
            {
                /* Adjust delta for the last page */
                if (delta > size) delta = size;
                /* Enable write and write to the status register */
                if ((HAL_OK == (EeResult = EeCmd(EE_WREN))) &&
                    (HAL_OK == (EeResult = EeCmdAdrTx(EE_WRITE, adr, ptr, delta))))
                {
                    /* Less than 5 ms required */
                    unsigned timeout = EE_WRITE_TIMEOUT;
                    uint8_t Status;
                    /* Read the status register */
                    while ((HAL_OK == (EeResult = EeCmdRx(EE_RDSR, &Status))) && (0 != (Status & EE_SR_WIP)))
                    {
                        /* Check the timeout */
                        if (0 == timeout--)
                        {
                            /* Relase the SPI resource */
                            xSemaphoreGive(EeLockSemaphore);
                            /* Timeout expired */
                            return (EeResult = HAL_TIMEOUT);
                        }
                        /* Call the scheduler */
                        vTaskDelay(1);
                    }
                }
                /* Update pointers and counters */
                size -= delta;
                adr += delta;
                ptr = ((uint8_t *)ptr)+delta;
                delta = EE_PAGE_LENGTH;
            }

        } 
        /* SPI bus busy */
        else EeResult = HAL_BUSY;
        /* Relase the SPI resource */
        xSemaphoreGive(EeLockSemaphore);
    }
    /* Arguments error */
    else EeResult = HAL_ERROR;
    /* Done */
    return EeResult;
}


/*******************************************************************************************************************//**
 * @bries   Activate/Deactivate the M95512-D internal WRITE-ENABLE latch
 * @param   state   DISABLE Write operations are disabled
 *                  ENABLE  Write operations are enabled
 * @return  void
 */
HAL_StatusTypeDef EeWriteEnableLatch(FunctionalState state)
{
    return EeCmd((DISABLE != state)? EE_WREN : EE_WRDI);
}


/*******************************************************************************************************************//**
 * @brief   Initilaize the Ethernet subsystem
 * @param   void
 * @return  HAL_OK      Data received without errors
 *          HAL_ERROR   Bus errors detected
 */
HAL_StatusTypeDef EeInit(void)
{
    /* Not initilaized yet */
    if ((NULL == EeLockSemaphore) && (NULL == EeIsrSemaphore))
    {
        /* Create the resource lock semaphore */
        if (NULL != (EeLockSemaphore = xSemaphoreCreateMutex()))
        {
            /* Create the ISR semaphore */
            if (NULL != (EeIsrSemaphore = xSemaphoreCreateBinary()))
            {
                /* Take the semaphore if not already taken */
                xSemaphoreTake(EeIsrSemaphore, 0);
                
                /* Disable HOLD state */
                EeHoldControl(DISABLE);
                /* Disable Write control */
                EeWriteControl(DISABLE);
                /* Init SPI5 */
                MX_SPI6_Init();
                HAL_SPI_MspInit(&hspi6);
                /* Reset the state machine */
                EeResult = HAL_OK;
                /* Register the call-back functions */
                HAL_SPI_RegisterCallback(&hspi6, HAL_SPI_TX_COMPLETE_CB_ID, EeSpiTransfer_Cb);
                HAL_SPI_RegisterCallback(&hspi6, HAL_SPI_RX_COMPLETE_CB_ID, EeSpiTransfer_Cb);
                HAL_SPI_RegisterCallback(&hspi6, HAL_SPI_ERROR_CB_ID, EeSpiError_Cb);

                xSemaphoreGive(EeLockSemaphore);
                /* All DONE */
                return HAL_OK;
            }
            /* Cannot create the ISR semaphore (memory overflow) */
            EeIsrSemaphore = NULL;
            vSemaphoreDelete(EeLockSemaphore);
        }
        /* Cannot create the resource lock semaphore (memory overflow) */
        EeLockSemaphore = NULL;
    }
    /* Errors detected */
    return HAL_ERROR;
}


/*******************************************************************************************************************//**
 * @brief   De-Initilaize the Ethernet subsystem
 * @param   void
 * @return  void
 */
void EeDeInit(void)
{
    /* Lock the resource */
    xSemaphoreTake(EeIsrSemaphore, 0);
    /* Init SPI interface */
    HAL_SPI_DeInit(&hspi6);
    /* Disable HOLD state */
    EeHoldControl(DISABLE);
    /* Disable Write control */
    EeWriteControl(DISABLE);
    /* Remove semaphores */
    vSemaphoreDelete(EeLockSemaphore);
    EeLockSemaphore = NULL;
    vSemaphoreDelete(EeIsrSemaphore);
    EeIsrSemaphore = NULL;
}


static const char TestString[] = "The quick brown fox jumps over the lazy dog. 0123456789\n";
uint8_t buffer[sizeof(TestString)];

void EeTest(void)
{
    uint8_t Status;

    /* Starting */
    coprintf("EEPROM test started\n");

    /* Init the device */
    if (HAL_OK != EeInit())
    {
        coprintf(">>> EEPROM init error\n");
        goto __exit;
    }

    /* Write the status register */
    coprintf("EEPROM status register r/w test\n");
    if (HAL_OK != EeWriteStatus(Status = 0))
    {
        coprintf(">>> EEPROM status register write error\n");
        goto __exit;
    }
    /* Write the status register */
    if ((HAL_OK != EeReadStatus(&Status)) || (0 != Status))
    {
        coprintf(">>> EEPROM status register read error (%02X)\n", Status);
        goto __exit;
    }

    /* Write the test string */
    coprintf("EEPROM data r/w test\n");
    if (HAL_OK != EeWriteData(0xFF00, TestString, sizeof(TestString)))
    {
        coprintf(">>> EEPROM data write error\n");
        goto __exit;
    }
    /* Read-back the test string */
    if (HAL_OK != EeReadData(0xFF00, buffer, sizeof(buffer)))
    {
        coprintf(">>> EEPROM data read error\n");
        goto __exit;
    }
    /* Compare the buffer with the TestString */
    if (0 != memcmp(buffer, TestString, sizeof(TestString)))
    {
        coprintf(">>> EEPROM data check error\n");
        goto __exit;
    }

    /* Delete the used memory area */
    coprintf("EEPROM clear\n");
    memset(buffer, 0xFF, sizeof(buffer));
    /* Delete the EEPROM */
    if (HAL_OK != EeWriteData(0xFF00, buffer, sizeof(buffer)))
    {
        coprintf(">>> EEPROM data write error\n");
        goto __exit;
    }

__exit:
    EeDeInit();
    coprintf("EEPROM test terminated\n");
}


/* END OF FILE ********************************************************************************************************/
