/*******************************************************************************************************************//**
 * @file    phy.c
 * @author  G.Zini
 * @version 1.0
 * @date    2021 June, 4
 * @brief   KSZ8563R (Ethernet Phy) management functions
 **********************************************************************************************************************/

/* Includes ***********************************************************************************************************/
#include <string.h>
#include "main.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "stm32h7xx_hal_spi.h"
#include "version.h"
#include "phy.h"
#include "console.h"

/* Private macros *****************************************************************************************************/

/* Maximum waiting time for SPI operation */
#define DEFAULT_SPI_TIMEOUT (100/*ms*/)

/* Read/Write codes for the SPI */
#define PHY_READ          (0x60000000uL)
#define PHY_WRITE         (0x40000000uL)

/* Override the HAL definitions */
#define PHY_SPI           (SPI5)
#define PHY_SPI_FLAGS     (SPI_IFCR_SUSPC|SPI_IFCR_TSERFC|SPI_IFCR_MODFC|SPI_IFCR_TIFREC|SPI_IFCR_CRCEC\
                          |SPI_IFCR_OVRC|SPI_IFCR_UDRC|SPI_IFCR_TXTFC|SPI_IFCR_EOTC)
#define PHY_RX_DMA        (DMA1_Stream2)
#define PHY_TX_DMA        (DMA1_Stream3)
#define PHY_RX_DMA_ISR    (DMA1->LISR)
#define PHY_TX_DMA_ISR    (DMA1->LISR)
#define PHY_RX_DMA_IFCR   (DMA1->LIFCR)
#define PHY_TX_DMA_IFCR   (DMA1->LIFCR)

#define PHY_RX_DMA_TC     (DMA_LISR_TCIF2)
#define PHY_RX_DMA_HT     (DMA_LISR_HTIF2)
#define PHY_RX_DMA_TE     (DMA_LISR_TEIF2)
#define PHY_RX_DMA_DME    (DMA_LISR_DMEIF2)
#define PHY_RX_DMA_FE     (DMA_LISR_FEIF2)
#define PHY_RX_DMA_FLAGS  (DMA_LISR_TCIF2|DMA_LISR_HTIF2|DMA_LISR_TEIF2|DMA_LISR_DMEIF2|DMA_LISR_FEIF2)

#define PHY_TX_DMA_TC     (DMA_LISR_TCIF3)
#define PHY_TX_DMA_HT     (DMA_LISR_HTIF3)
#define PHY_TX_DMA_TE     (DMA_LISR_TEIF3)
#define PHY_TX_DMA_DME    (DMA_LISR_DMEIF3)
#define PHY_TX_DMA_FE     (DMA_LISR_FEIF3)
#define PHY_TX_DMA_FLAGS  (DMA_LISR_TCIF3|DMA_LISR_HTIF3|DMA_LISR_TEIF3|DMA_LISR_DMEIF3|DMA_LISR_FEIF3)

/* Utilities */
#define __DUMMY_READ(x)   while(0){volatile uint8_t u = (x);}
#define __BYTE(x)         (*(volatile uint8_t *)&(x))


/* Private typedefs ***************************************************************************************************/

typedef enum
{
    PHY_STATUS_IDLE,
    PHY_STATUS_TXADDR,
    PHY_STATUS_TXDATA,
    PHY_STATUS_RXADDR,
    PHY_STATUS_RXDATA,
    PHY_SPI_ERROR
} PhySpiStatus_t;


/* Public variables ***************************************************************************************************/

/* Defined in main.c */
extern SPI_HandleTypeDef hspi5;


/* Private variables **************************************************************************************************/

/* Mutex semaphore to lock the SPI device */
static volatile SemaphoreHandle_t PhyLockSemaphore = NULL ;
static volatile SemaphoreHandle_t PhyIsrSemaphore = NULL ;

/* State machine */
static volatile PhySpiStatus_t PhyStatus = PHY_STATUS_IDLE;
static uint8_t PhyAddrBuffer[4];
static uint8_t *PhyDataPointer;
static size_t PhyDataSize;

/* Translate Standard IEEE 802.3 MIIM Registers to SPI addresses */
static const uint16_t MIIMRegistersTable[] =
{
    0x0100U,        /* 0x00h: PHY Basic Control Register */
    0x0102U,        /* 0x01h: PHY Basic Status Register */
    0x0104U,        /* 0x02h: PHY ID High Register */
    0x0106U,        /* 0x03h: PHY ID Low Register */
    0x0108U,        /* 0x04h: PHY Auto-Negotiation Advertisement Register */
    0x010AU,        /* 0x05h: PHY Auto-Negotiation Link Partner Ability Register */
    0x010CU,        /* 0x06h: PHY Auto-Negotiation Expansion Status Register */
    0x010EU,        /* 0x07h: PHY Auto-Negotiation Next Page Register */
    0x0110U,        /* 0x08h: PHY Auto-Negotiation Link Partner Next Page Ability Register */
    0x0000U,        /* 0x09h: Reserved */
    0x0000U,        /* 0x0Ah: Reserved */
    0x0000U,        /* 0x0Bh: Reserved */
    0x0000U,        /* 0x0Ch: Reserved */
    0x011AU,        /* 0x0Dh: PHY MMD Setup Register */
    0x011CU,        /* 0x0Eh: PHY MMD Data Register */
    0x0000U,        /* 0x0Fh: Reserved */
    0x0000U,        /* 0x10h: Reserved */
    0x0122U,        /* 0x11h: PHY Remote Loopback Register */
    0x0124U,        /* 0x12h: PHY Link MD Register */
    0x0126U,        /* 0x13h: PHY Link PMA/PCS Status Register */
    0x0000U,        /* 0x14h: Reserved */
    0x012AU,        /* 0x15h: PHY RXER Count Register */
    0x0000U,        /* 0x16h: Reserved */
    0x0000U,        /* 0x17h: Reserved */
    0x0000U,        /* 0x18h: Reserved */
    0x0000U,        /* 0x19h: Reserved */
    0x0000U,        /* 0x1Ah: Reserved */
    0x0136U,        /* 0x1Bh: Port Interrupt Control/Status Register */
    0x0138U,        /* 0x1Ch: PHY Auto MDI/MDI-X Register */
    0x0000U,        /* 0x1Dh: Reserved */
    0x0000U,        /* 0x1Eh: Reserved */
    0x013EU,        /* 0x1Fh: PHY Control Register */
};  


/* Callback functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   SPI6 interrupt handler
 * @param   void
 * @return  void
 */
void PhyTxComplete_Cb(SPI_HandleTypeDef *hspi)
{
    BaseType_t Woken;
    /* State machine */
    switch (PhyStatus)
    {
        case PHY_STATUS_TXADDR:
            /* Start transmission of data */
            HAL_SPI_Transmit_IT(&hspi5, PhyDataPointer, PhyDataSize);
            /* Next state */
            PhyStatus = PHY_STATUS_TXDATA;
            break;
        case PHY_STATUS_TXDATA:
            /* Deactivate the SPI-NSS signal */
            HAL_GPIO_WritePin(ETH_nSEL_GPIO_Port, ETH_nSEL_Pin, GPIO_PIN_SET);
            /* All done */
            PhyStatus = PHY_STATUS_IDLE;
            /* Run the scheduler if requested */
            xSemaphoreGiveFromISR(PhyIsrSemaphore, &Woken);
            portYIELD_FROM_ISR(Woken);
            break;
        case PHY_STATUS_RXADDR:
            /* Start receive of data */
            HAL_SPI_Receive_IT(&hspi5, PhyDataPointer, PhyDataSize);
            /* Next state */
            PhyStatus = PHY_STATUS_RXDATA;
            break;
        case PHY_STATUS_IDLE:
        case PHY_STATUS_RXDATA:
            /* Deactivate the SPI-NSS signal */
            HAL_GPIO_WritePin(ETH_nSEL_GPIO_Port, ETH_nSEL_Pin, GPIO_PIN_SET);
            /* Should never happen */
            PhyStatus = PHY_SPI_ERROR;
        case PHY_SPI_ERROR:
            /* Run the scheduler if requested */
            xSemaphoreGiveFromISR(PhyIsrSemaphore, &Woken);
            portYIELD_FROM_ISR(Woken);
            break;
    }
}

/*******************************************************************************************************************//**
 * @brief   SPI6 interrupt handler
 * @param   void
 * @return  void
 */
void PhyRxComplete_Cb(SPI_HandleTypeDef *hspi)
{
    BaseType_t Woken;
    /* State machine */
    switch (PhyStatus)
    {
        case PHY_STATUS_RXDATA:
            /* Deactivate the SPI-NSS signal */
            HAL_GPIO_WritePin(ETH_nSEL_GPIO_Port, ETH_nSEL_Pin, GPIO_PIN_SET);
            /* All done */
            PhyStatus = PHY_STATUS_IDLE;
            /* Run the scheduler if requested */
            xSemaphoreGiveFromISR(PhyIsrSemaphore, &Woken);
            portYIELD_FROM_ISR(Woken);
            break;
        case PHY_STATUS_IDLE:
        case PHY_STATUS_TXADDR:
        case PHY_STATUS_TXDATA:
        case PHY_STATUS_RXADDR:
            /* Deactivate the SPI-NSS signal */
            HAL_GPIO_WritePin(ETH_nSEL_GPIO_Port, ETH_nSEL_Pin, GPIO_PIN_SET);
            /* Should never happen */
            PhyStatus = PHY_SPI_ERROR;
        case PHY_SPI_ERROR:
            /* Run the scheduler if requested */
            xSemaphoreGiveFromISR(PhyIsrSemaphore, &Woken);
            portYIELD_FROM_ISR(Woken);
            break;
    }
}

/*******************************************************************************************************************//**
 * @brief   SPI6 interrupt handler
 * @param   void
 * @return  void
 */
void PhyError_Cb(SPI_HandleTypeDef *hspi)
{
    BaseType_t Woken;
    /* Deactivate the SPI-NSS signal */
    HAL_GPIO_WritePin(ETH_nSEL_GPIO_Port, ETH_nSEL_Pin, GPIO_PIN_SET);
    /* Should never happen */
    PhyStatus = PHY_SPI_ERROR;
    /* Run the scheduler if needed */
    xSemaphoreGiveFromISR(PhyIsrSemaphore, &Woken);
    portYIELD_FROM_ISR(Woken);
}


/* Private functions **************************************************************************************************/

/*******************************************************************************************************************//**
 * @bries   Activate the KSZ8563R hardware RESET. 
 * @param   void
 * @return  void
 */
static void PhyReset(void)
{
    /* Reset the KSZ8563R */
    HAL_GPIO_WritePin(ETH_nRST_GPIO_Port, ETH_nRST_Pin, GPIO_PIN_RESET);
    __HAL_RCC_SPI5_FORCE_RESET();
    HAL_Delay(10);
    HAL_GPIO_WritePin(ETH_nRST_GPIO_Port, ETH_nRST_Pin, GPIO_PIN_SET);
    __HAL_RCC_SPI5_RELEASE_RESET();
    HAL_Delay(10);
}

/* Exported functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   Initilaize the Ethernet physical interface
 * @param   void
 * @return  HAL_OK      Init OK
 *          HAL_ERROR   Errors detected
 */
HAL_StatusTypeDef PhyInit(void)
{
    /* Not initilaized yet */
    if ((NULL == PhyLockSemaphore) && (NULL == PhyIsrSemaphore))
    {
        /* Create the resource lock semaphore */
        if (NULL != (PhyLockSemaphore = xSemaphoreCreateMutex()))
        {
            /* Create the ISR semaphore */
            if (NULL != (PhyIsrSemaphore = xSemaphoreCreateBinary()))
            {
                /* Take the semaphore if not already taken */
                xSemaphoreTake(PhyIsrSemaphore, 0);
                
                /* Init SPI5 */
                MX_SPI5_Init();

                /* Register the callback functions */
                HAL_SPI_RegisterCallback(&hspi5, HAL_SPI_TX_COMPLETE_CB_ID, PhyTxComplete_Cb);
                HAL_SPI_RegisterCallback(&hspi5, HAL_SPI_RX_COMPLETE_CB_ID, PhyRxComplete_Cb);
                HAL_SPI_RegisterCallback(&hspi5, HAL_SPI_ERROR_CB_ID, PhyError_Cb);
                
                /* Start the state-machine */
                PhyStatus = PHY_STATUS_IDLE;
                
                /* Relase the resource */
                xSemaphoreGive(PhyLockSemaphore);
                /* All DONE */
                return HAL_OK;
            }
            /* Cannot create the ISR semaphore (memory overflow) */
            PhyIsrSemaphore = NULL;
            vSemaphoreDelete(PhyLockSemaphore);
        }
        /* Cannot create the resource lock semaphore (memory overflow) */
        PhyLockSemaphore = NULL;
    }
    /* Errors detected */
    return HAL_ERROR;
}


/*******************************************************************************************************************//**
 * @brief   De-Initilaize the Ethernet subsystem
 * @param   void
 * @return  void
 */
void PhyDeInit(void)
{
    /* DeInit SPI interface */
    HAL_SPI_DeInit(&hspi5);

    /* Reset the KSZ8563R */
    PhyReset();

    /* Remove the ISR semaphore */
    if (NULL != PhyIsrSemaphore)
    {
        vSemaphoreDelete(PhyIsrSemaphore);
        PhyIsrSemaphore = NULL;
    }

    /* Remove the Resource-Lock semaphore */
    if (NULL != PhyLockSemaphore) 
    {
        vSemaphoreDelete(PhyLockSemaphore);
        PhyLockSemaphore = NULL;
    }
}


/*******************************************************************************************************************//**
 * @brief   Read one or more registers from the register space of the KSZ8563R
 * @param   uRegNbr     Register number
 *          pData       Pointer to the buffer where the data are to be loaded
 *          uSize       Total number to be received
 * @return  HAL_OK      Data received without errors
 *          HAL_BUSY    The SPI resource is busy
 *          HAL_TIMEOUT The operation did not finished
 *          HAL_ERROR   Bus errors detected
 */
HAL_StatusTypeDef PhyRead(uint32_t uReg, uint8_t *pData, size_t uSize)
{
    HAL_StatusTypeDef uResult = HAL_OK;

    /* Check argumeents */
    if ((NULL != pData) && (0 < uSize))
    {
        /* Format the register address */
        uReg = PHY_READ | ((uReg << 5) & 0x1FFFFFE0u);

        /* Acquire the SPI resource */
        if (pdPASS == xSemaphoreTake(PhyLockSemaphore, DEFAULT_SPI_TIMEOUT))
            
        {
            /* Should be true */
            if (PHY_STATUS_IDLE == PhyStatus)
            {
                /* Format buffer in big/endian mode */
                PhyAddrBuffer[0] = uReg >> 24;
                PhyAddrBuffer[1] = uReg >> 16;
                PhyAddrBuffer[2] = uReg >> 8;
                PhyAddrBuffer[3] = uReg >> 0;
                
                /* Store the parameters */
                PhyDataPointer = (uint8_t *)pData;
                PhyDataSize = uSize;
                PhyStatus = PHY_STATUS_RXADDR;

                /* Activate the SPI-NSS signal */
                HAL_GPIO_WritePin(ETH_nSEL_GPIO_Port, ETH_nSEL_Pin, GPIO_PIN_RESET);

                /* Start transmission of the address */
                if (HAL_OK == HAL_SPI_Transmit_IT(&hspi5, PhyAddrBuffer, sizeof(PhyAddrBuffer)))
                {
                    /* Wait for the end of the transfer */
                    if (pdPASS == xSemaphoreTake(PhyIsrSemaphore, DEFAULT_SPI_TIMEOUT))
                    {
                        /* Report the result of the transfer */
                        uResult = (PhyStatus == PHY_STATUS_IDLE)? HAL_OK: HAL_ERROR;
                    }
                    /* Interrupt lost */
                    else uResult = HAL_TIMEOUT;
                }
                /* Unknown error */
                else uResult = HAL_ERROR;

            /* Relase the SPI resource */
            xSemaphoreGive(PhyLockSemaphore);
            }
            /* SPI stalled */
            else uResult = HAL_ERROR;
        }
        /* Resource locked by other tasks */
        else uResult = HAL_BUSY;
    }
    /* Arguments error */
    else uResult = HAL_ERROR;
    /* All done */
    return uResult;
}


/*******************************************************************************************************************//**
 * @brief   Write one or more registers in the register space of the KSZ8563R
 * @param   uRegNbr     Register number
 *          pData       Pointer to the buffer of the data to be transmitted
 *          uSize       Total number to be fransmitted
 * @return  HAL_OK      Data transmitted without errors
 *          HAL_BUSY    The SPI resource is busy
 *          HAL_TIMEOUT The operation did not finished
 *          HAL_ERROR   Bus errors detected
 */
HAL_StatusTypeDef PhyWrite(uint32_t uReg, const uint8_t *pData, size_t uSize)
{
    HAL_StatusTypeDef uResult = HAL_OK;

    /* Check argumeents */
    if ((NULL != pData) && (0 < uSize))
    {
        /* Format the register address */
        uReg = PHY_WRITE | ((uReg << 5) & 0x1FFFFFE0u);

        /* Acquire the SPI resource */
        if (pdPASS == xSemaphoreTake(PhyLockSemaphore, DEFAULT_SPI_TIMEOUT))
        {
            /* Should be true */
            if (PHY_STATUS_IDLE == PhyStatus)
            {
                /* Format buffer in big/endian mode */
                PhyAddrBuffer[0] = uReg >> 24;
                PhyAddrBuffer[1] = uReg >> 16;
                PhyAddrBuffer[2] = uReg >> 8;
                PhyAddrBuffer[3] = uReg >> 0;
                
                /* Store the parameters */
                PhyDataPointer = (uint8_t *)pData;
                PhyDataSize = uSize;
                PhyStatus = PHY_STATUS_TXADDR;

                /* Activate the SPI-NSS signal */
                HAL_GPIO_WritePin(ETH_nSEL_GPIO_Port, ETH_nSEL_Pin, GPIO_PIN_RESET);

                /* Start transmission of the address */
                if (HAL_OK == HAL_SPI_Transmit_IT(&hspi5, PhyAddrBuffer, sizeof(PhyAddrBuffer)))
                {
                    /* Wait for the end of the transfer */
                    if (pdPASS == xSemaphoreTake(PhyIsrSemaphore, DEFAULT_SPI_TIMEOUT))
                    {
                        /* Report the result of the transfer */
                        uResult = (PhyStatus == PHY_STATUS_IDLE)? HAL_OK: HAL_ERROR;
                    }
                    /* Interrupt lost */
                    else uResult = HAL_TIMEOUT;
                }
                /* Unknown error */
                else uResult = HAL_ERROR;

                /* Deactivate the SPI-NSS signal */
                HAL_GPIO_WritePin(ETH_nSEL_GPIO_Port, ETH_nSEL_Pin, GPIO_PIN_SET);
            }
            /* SPI stalled */
            else uResult = HAL_ERROR;
            /* Relase the SPI resource */
            xSemaphoreGive(PhyLockSemaphore);
        }
        /* Resource locked by other tasks */
        else uResult = HAL_BUSY;
    }
    /* Arguments error */
    else uResult = HAL_ERROR;
    /* All done */
    return uResult;
}


/*******************************************************************************************************************//**
 * @brief   Read from the KSZ8563R register with 8, 16, 32 or 48 bits width. The function manages the Endianes of
 *          the required variabiles
 * @param   pData       Pointer to the destination variable
 *          addr        Register address
 * @return  HAL_OK      Data received without errors
 *          HAL_BUSY    The SPI resource is busy
 *          HAL_TIMEOUT The operation did not finished
 *          HAL_ERROR   Bus errors detected
 */
HAL_StatusTypeDef PhyRead_8bit(uint32_t addr, uint8_t *pData)
{
    return PhyRead(addr, (void *)pData, sizeof(*pData));
}
/* ... */
HAL_StatusTypeDef PhyRead_16bit(uint32_t addr, uint16_t *pData)
{
    HAL_StatusTypeDef Result;
    uint16_t value;
    if (HAL_OK == (Result = PhyRead(addr, (void *)&value, sizeof(value))))
    {
        if (NULL != pData) *pData = __REV16(value);
    }
    return Result;
}
/* ... */
HAL_StatusTypeDef PhyRead_32bit(uint32_t addr, uint32_t *pData)
{
    HAL_StatusTypeDef Result;
    uint32_t value;
    if (HAL_OK == (Result = PhyRead(addr, (void *)&value, sizeof(value))))
    {
        if (NULL != pData) *pData = __REV(value);
    }
    return Result;
}
/* ... */
HAL_StatusTypeDef PhyRead_48bit(uint32_t addr, uint64_t *pData)
{
    HAL_StatusTypeDef Result;
    uint8_t buf[6];
    uint64_t uVar;
    if (HAL_OK == (Result = PhyRead(addr, (void *)buf, sizeof(buf))))
    {
        if (NULL != pData)
        {
            uVar = ((uint64_t)buf[0])<<40u
                 | ((uint64_t)buf[1])<<32u
                 | ((uint64_t)buf[2])<<24u
                 | ((uint64_t)buf[3])<<16u
                 | ((uint64_t)buf[4])<<8u
                 | ((uint64_t)buf[5])<<0u;
            *pData = uVar;
        }
    }
    return Result;
}


/*******************************************************************************************************************//**
 * @brief   Write to a single KSZ8563R register with 8, 16, 32 or 48 bits width. The function manages the Endianes of
 *          the required variabiles
 * @param   pData       Pointer to the destination variable
 *          addr        Register address
 * @return  HAL_OK      Data received without errors
 *          HAL_BUSY    The SPI resource is busy
 *          HAL_TIMEOUT The operation did not finished
 *          HAL_ERROR   Bus errors detected
 */
HAL_StatusTypeDef PhyWrite_8bit(uint32_t addr, uint8_t value)
{
    return PhyWrite(addr, (const void *)&value, sizeof(value));
}
/* ... */
HAL_StatusTypeDef PhyWrite_16bit(uint32_t addr, uint16_t value)
{
    value = __REV16(value);
    return PhyWrite(addr, (const void *)&value, sizeof(value));
}
/* ... */
HAL_StatusTypeDef PhyWrite_32bit(uint32_t addr, uint32_t value)
{
    value = __REV(value);
    return PhyWrite(addr, (const void *)&value, sizeof(value));
}
/* ... */
HAL_StatusTypeDef PhyWrite_48bit(uint32_t addr, uint64_t value)
{
    uint32_t buf[6];
    buf[0] = value >> 40u;
    buf[1] = value >> 32u;
    buf[2] = value >> 24u;
    buf[3] = value >> 16u;
    buf[4] = value >> 8u;
    buf[5] = value >> 0u;
    return PhyWrite(addr, (const void *)buf, sizeof(buf));
}


/*******************************************************************************************************************//**
 * @brief   Read one of the standard MIIM register as stated by IEEE 802.3 clause 22
 * @param   Port        Port number. Numbers 1 and 2 are the PHY ports. Number 3 is the MAC interface
 *          Reg         Register number. It can be any valid register in the range 0..31
 *          pValue      Pointer to the destination variable
 * @return  HAL_OK      Data received without errors
 *          HAL_BUSY    The SPI resource is busy
 *          HAL_TIMEOUT The operation did not finished
 *          HAL_ERROR   Bus errors detected
 */
HAL_StatusTypeDef PhyRead_MIIM(uint32_t Port, uint32_t Reg, uint16_t *pValue)
{
    HAL_StatusTypeDef Result = HAL_ERROR;
    uint16_t Addr;

    /* Check arguments */
    if ((NULL != pValue) && (1 <= Port) && (3 >= Port) && (31 >= Reg))
    {
        /* Translate the MIIM address to KSZ8593R register address */
        Addr = MIIMRegistersTable[Reg] | (Port<<12);
        /* Read the 16 bits register */
        if (0 != Addr) Result = PhyRead_16bit(Addr, pValue);
    }
    /* Report the result */
    return Result;
}


/*******************************************************************************************************************//**
 * @brief   Write one of the standard MIIM register as stated by IEEE 802.3 clause 22
 * @param   Port        Port number. Numbers 1 and 2 are the PHY ports. Number 3 is the MAC interface
 *          Reg         Register number. It can be any valid register in the range 0..31
 *          Value       Value to be written into the specified register
 * @return  HAL_OK      Data received without errors
 *          HAL_BUSY    The SPI resource is busy
 *          HAL_TIMEOUT The operation did not finished
 *          HAL_ERROR   Bus errors detected
 */
HAL_StatusTypeDef PhyWrite_MIIM(uint32_t Port, uint32_t Reg, uint16_t Value)
{
    HAL_StatusTypeDef Result = HAL_ERROR;
    uint16_t Addr;

    /* Check arguments */
    if ((1 <= Port) && (3 >= Port) && (31 >= Reg))
    {
        /* Translate the MIIM address to KSZ8593R register address */
        Addr = MIIMRegistersTable[Reg] | (Port<<12);
        /* Write the 16 bits register */
        if (0 != Addr) Result = PhyWrite_16bit(Addr, Value);
    }
    /* Report the result */
    return Result;
}


/*******************************************************************************************************************//**
 * @brief   Configures a set of KSZ8563R register in a list. The function handles 8, 16 and 32 bit registers. Other
 *          widths must be managed one byte at a time
 * @param   pTable      Pointer to a table of PhyConf_t elements
 * @param   length      Number of items in the table
 * @return  HAL_OK      Configuration done
 *          HAL_BUSY    The SPI resource is busy
 *          HAL_TIMEOUT The operation did not finished
 *          HAL_ERROR   Bus errors detected
 */
HAL_StatusTypeDef PhyConf(const PhyConf_t *pTable, unsigned length)
{
    union {uint32_t u32; uint16_t u16; uint8_t u8;} val;
    /* Check arguments */
    if (NULL != pTable)
    {
        /* Zero length is accepted */
        for ( ; length-- ; pTable++)
        {
            switch (pTable->size)
            {
                /* 8 bits register */
                case sizeof(uint8_t):
                    if (HAL_OK != PhyRead_8bit(pTable->addr, &(val.u8))) return HAL_ERROR;
                    {
                        /* Mask and merge the required switches */
                        val.u8 = (val.u8 & (uint8_t)pTable->mask) | (uint8_t)pTable->value;
                        if (HAL_OK != PhyWrite_8bit(pTable->addr, val.u8)) return HAL_ERROR;
                    }
                    break;
                /* 16 bits register */
                case sizeof(uint16_t):
                    if (HAL_OK != PhyRead_16bit(pTable->addr, &(val.u16))) return HAL_ERROR;
                    {
                        val.u16 = (val.u16 & (uint16_t)pTable->mask) | (uint16_t)pTable->value;
                        if (HAL_OK != PhyWrite_16bit(pTable->addr, val.u16)) return HAL_ERROR;
                    }
                    break;
                /* 32 bits register */
                case sizeof(uint32_t):
                    if (HAL_OK != PhyRead_32bit(pTable->addr, &(val.u32))) return HAL_ERROR;
                    {
                        val.u32 = (val.u32 & (uint32_t)pTable->mask) | (uint32_t)pTable->value;
                        if (HAL_OK != PhyWrite_32bit(pTable->addr, val.u32)) return HAL_ERROR;
                    }
                    break;
                /* Wrong size */
                default:
                    return HAL_ERROR;
            }
        }
        return HAL_OK;
    }
    return HAL_ERROR;
}


/* END OF FILE ********************************************************************************************************/
