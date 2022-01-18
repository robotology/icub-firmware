/*******************************************************************************************************************//**
 * @file    eth.c
 * @author  G.Zini
 * @version 1.0
 * @date    2021 June, 4
 * @brief   Ethernet management functions
 **********************************************************************************************************************/

/* Includes ***********************************************************************************************************/
#include <string.h>
#include "main.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "stm32h7xx_hal_spi.h"
#include "version.h"
#include "eth.h"
#include "phy.h"
#include "console.h"


/* Private macros *****************************************************************************************************/
/* Maximum waiting time for SPI operation */

#define DEFAULT_ETH_TIMEOUT (100/*ms*/)

#define CRCPOLY24               (0x5D6DCBu)
#define IIT_MAC_ADDRESS         (0x709A0B000000ULL)

#define ETH_STATUS_TXCOMPLETE   (0x00000001u)
#define ETH_STATUS_RXCOMPLETE   (0x00000002u)
#define ETH_STATUS_DMAERROR     (0x00000004u)
#define ETH_STATUS_MACERROR     (0x00000008u)

#define CACHE_ALIGN(a,s)        ((uint32_t *)((uint32_t)(a)&0xFFFFFFE0u)),((s)+(((uint32_t)(a))&0x0000001Fu))

#define EthPacket(size)         struct{EthFrame_t header;uint8_t payload[(size)];}


/* Public variables ***************************************************************************************************/

extern ETH_HandleTypeDef heth;
extern ETH_TxPacketConfig TxConfig;
extern void MX_ETH_Init(void);
extern void HAL_ETH_MspInit(ETH_HandleTypeDef* heth);


/* Private variables **************************************************************************************************/

/* Mutex semaphore to lock the SPI device */
static volatile SemaphoreHandle_t EthLockSemaphore = NULL ;
static volatile SemaphoreHandle_t EthIsrSemaphore = NULL ;
static volatile uint32_t EthStatus = 0;
static const char TestString[] = "The quick brown fox jumps over the lazy dog. ";
static __align(32) EthPacket(52) TxPacket;  
static __align(32) EthPacket(52) RxPacket;


/* Private functions **************************************************************************************************/

/*******************************************************************************************************************//**
 * @bries   Calculates the CRC24 code for hashing the MAC address from the microcontroller UID
 * @param   pCRC    Pointer to the initila value of the CRC code. The function updates the value according to the
 *                  input data
 * @param   uIn     Input data to be hashed
 * @return  void
 */
static void crc24(uint32_t *pCrc, uint32_t uIn)
{
    uint32_t crc = 0;
    unsigned u;
    if (NULL != pCrc)
    {
        crc = *pCrc;
        for (u = 0; u < 32; u++)
        {
            crc <<= 1;
            if (crc & 0x01000000u) crc ^= CRCPOLY24;
            if (uIn & 0x00000001u) crc |= 0x00000001u;
            uIn >>= 1;
        }
        *pCrc = crc;
    }
}


/* Callback functions *************************************************************************************************/

void EthTxComplete_cb(ETH_HandleTypeDef * heth)
{
    /* Signal the result */
    EthStatus |= ETH_STATUS_TXCOMPLETE;
}

void EthRxComplete_cb(ETH_HandleTypeDef * heth)
{
    BaseType_t Woken;
    /* Signal the result */
    EthStatus |= ETH_STATUS_RXCOMPLETE;
    /* Signal the interrupt */
    xSemaphoreGiveFromISR(EthIsrSemaphore, &Woken);
    /* Run the scheduler if needed */
    portYIELD_FROM_ISR(Woken);
}

void EthDmaError_cb(ETH_HandleTypeDef * heth)
{
    BaseType_t Woken;
    /* Signal the result */
    EthStatus |= ETH_STATUS_DMAERROR;
    /* Signal the interrupt */
    xSemaphoreGiveFromISR(EthIsrSemaphore, &Woken);
    /* Run the scheduler if needed */
    portYIELD_FROM_ISR(Woken);
}

void EthMacError_cb(ETH_HandleTypeDef * heth)
{
    BaseType_t Woken;
    /* Signal the result */
    EthStatus |= ETH_STATUS_MACERROR;
    /* Signal the interrupt */
    xSemaphoreGiveFromISR(EthIsrSemaphore, &Woken);
    /* Run the scheduler if needed */
    portYIELD_FROM_ISR(Woken);
}


/* Exported functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   Calculate a MAC address from the UID of the microcontroller. The OUI part of the address is set to the
 *          IIT reserved code (70:9A:00:xx:xx:xx). The fixed part depends by the UID of the microcontroller and by
 *          the argument 'seed'.
 * @param   seed        Starting value of the CRC algorithm. The function returns identical MAC values for a given seed
 *                      argument and a given microcontroller unique-identifier.
 * @return  uint64_t    MAC Address in the format 70:9A:0B:xx:xx:xx. Bits 47..24 are the IIT reserved address, bits
 *                      23..0 are a pseudo-random hash originated with the Unique Identifier (UID) of the 
 *                      microcontroller. It is recomanded to clear bit 0 for the microcontroller MAC address, and to
 *                      set bit 0 for the network-switch MAC address.
 */
uint64_t EthHashAddress(uint32_t seed)
{
    crc24(&seed, UID[0]);
    crc24(&seed, UID[1]);
    crc24(&seed, UID[2]);
    return IIT_MAC_ADDRESS | (uint64_t)(seed & 0x00FFFFFFUL) ;
}


/*******************************************************************************************************************//**
 * @brief   Ethernet initialization
 * @param   void
 * @return  void
 */
HAL_StatusTypeDef EthInit(void)
{
    /* Not initilaized yet */
    if ((NULL == EthLockSemaphore) && (NULL == EthIsrSemaphore))
    {
        /* Create the resource lock semaphore */
        if (NULL != (EthLockSemaphore = xSemaphoreCreateMutex()))
        {
            /* Create the ISR semaphore */
            if (NULL != (EthIsrSemaphore = xSemaphoreCreateBinary()))
            {
                /* Take the semaphore if not already taken */
                xSemaphoreTake(EthIsrSemaphore, 0);
                /* Init the Ethernet peripheral */
                MX_ETH_Init();
                /* Register the callback function */
                HAL_ETH_RegisterCallback(&heth, HAL_ETH_TX_COMPLETE_CB_ID, EthTxComplete_cb);
                HAL_ETH_RegisterCallback(&heth, HAL_ETH_RX_COMPLETE_CB_ID, EthRxComplete_cb);
                HAL_ETH_RegisterCallback(&heth, HAL_ETH_DMA_ERROR_CB_ID, EthDmaError_cb);
                HAL_ETH_RegisterCallback(&heth, HAL_ETH_MAC_ERROR_CB_ID, EthMacError_cb);
                /* Status machine IDLE */
                EthStatus = 0 ;
                /* Relase the resource */
                xSemaphoreGive(EthLockSemaphore);
                /* All DONE */
                return HAL_OK;
            }
            /* Remove the resource lock semaphore  */
            vSemaphoreDelete(EthLockSemaphore);
            EthLockSemaphore = NULL;
        }
    }
    /* Errors detected */
    return HAL_ERROR;
}


/*******************************************************************************************************************//**
 * @brief   Ethernet Deinitialization
 * @param   void
 * @return  void
 */
void EthDeInit(void)
{
    /* DeInit ETH interface */
    HAL_ETH_DeInit(&heth);
    /* Reset ETH interface */
    __HAL_RCC_ETH1MAC_FORCE_RESET();
    __HAL_RCC_ETH1MAC_RELEASE_RESET();
    /* Remove the TX ISR semaphore */
    if (NULL != EthIsrSemaphore)
    {
        vSemaphoreDelete(EthIsrSemaphore);
        EthIsrSemaphore = NULL;
    }
    /* Remove the Resource-Lock semaphore */
    if (NULL != EthLockSemaphore) 
    {
        vSemaphoreDelete(EthLockSemaphore);
        EthLockSemaphore = NULL;
    }
}



/*******************************************************************************************************************//**
 * @brief   Test Ethernet and KSZ8563R
 * @param   void
 * @return  void
 */
void EthTest(void)
{
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    uint32_t adr;
    TickType_t tick;
    ETH_BufferTypeDef TxBuffer;
    ETH_MACFilterConfigTypeDef FilterConfig;
    HAL_StatusTypeDef result;

    coprintf("Ethernet test started\n");
    
    /* Init the KSZ8563R */
    if (HAL_OK != PhyInit())
    {
        coprintf(">>> KSZ8563R init error\n"); 
        goto __quit;
    }

    /* Init the Eth peripheral*/
    if (HAL_OK != EthInit())
    {
        coprintf(">>> KSZ8563R periheral init error\n"); 
        goto __exit;
    }

    /* Configure the I/O driver strenght */
    if (HAL_OK != (result = PhyWrite_8bit(adr = 0x0100, u8 = 0x41))) goto __write_err;
    if (HAL_OK != (result = PhyWrite_8bit(adr = 0x010D, u8 = 0x77))) goto __write_err;
    
    /* Test of the RESET value of some KSZ8563R registers */
    coprintf("Testing KSZ8563R reset status\n");
    if (HAL_OK != (result = PhyRead_8bit (adr=0x0000, &u8 )) || (u8  != 0x00))              goto __read_err;
    if (HAL_OK != (result = PhyRead_16bit(adr=0x0001, &u16)) || (u16 != 0x9893))            goto __read_err;
    if (HAL_OK != (result = PhyRead_8bit (adr=0x000F, &u8 )) || (u8  != 0x3C))              goto __read_err;
    if (HAL_OK != (result = PhyRead_48bit(adr=0x0302, &u64)) || (u64 != 0x0010A1FFFFFFULL)) goto __read_err;
    if (HAL_OK != (result = PhyRead_16bit(adr=0x1104, &u16)) || (u16 != 0x0022))            goto __read_err;
    if (HAL_OK != (result = PhyRead_16bit(adr=0x1106, &u16)) || (u16 != 0x1637))            goto __read_err;
    if (HAL_OK != (result = PhyRead_16bit(adr=0x2104, &u16)) || (u16 != 0x0022))            goto __read_err;
    if (HAL_OK != (result = PhyRead_16bit(adr=0x2106, &u16)) || (u16 != 0x1637))            goto __read_err;

    /* Test strap configurations */
    coprintf("Testing KSZ8563R strap options\n");
    if (HAL_OK != (result = PhyRead_32bit(adr=0x0104, &u32))) goto __read_err;
    if (0 != (u32 & 0x80000000UL))
        coprintf("Warning: BAD configuration straps for IBA management REG%04X=%08X\n", adr, u32);
    if (HAL_OK != (result = PhyRead_8bit(adr=0x0300, &u8))) goto __read_err;
    if (u8 != 0x01) 
        coprintf("Warning: BAD configuration straps for START switch function REG%04X=%02X\n", adr, u8);
    if (HAL_OK != (result = PhyRead_16bit(adr=0x1100, &u16))) goto __read_err;
    if ((u16 & 0xDF80) != 0x1100)
        coprintf("Warning: BAD configuration straps for port 1 Auto/Speed functions REG%04X=%04X\n", adr, u16);
    if (HAL_OK != (result = PhyRead_16bit(adr=0x2100, &u16))) goto __read_err;
    if ((u16 & 0xDF80) != 0x1100)
        coprintf("Warning: BAD configuration straps for port 2 Auto/Speed functions REG%04X=%04X\n", adr, u16);
    if (HAL_OK != (result = PhyRead_8bit(adr=0x3301, &u8))) goto __read_err;
    if (u8 != 0x09) 
        coprintf("Warning: BAD configuration straps for MII/RMII/RGMII modes REG%04X=%02X\n", adr, u8);

    /* Enable ETH_nPME from KSZ8563R to microcontroller */
    if (HAL_OK != (result = PhyWrite_8bit(adr = 0x0006, u8 = 0x02))) goto __write_err;

    /* Test ETH_nPME and ETH_nIRQ signals */
    if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(ETH_nPME_GPIO_Port, ETH_nPME_Pin))
        {coprintf(">>> ERROR: Signal ETH_nPME stuck LOW\n"); goto __exit;}
    if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(ETH_nIRQ_GPIO_Port, ETH_nIRQ_Pin))
        {coprintf(">>> ERROR: Signal ETH_nIRQ stuck LOW\n"); goto __exit;}

    /* Read the programmed MAC Address */
    coprintf("Main MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n", 
              (heth.Instance->MACA0LR>>0)  & 0xFF, /* First received byte */
              (heth.Instance->MACA0LR>>8)  & 0xFF,
              (heth.Instance->MACA0LR>>16) & 0xFF,
              (heth.Instance->MACA0LR>>24) & 0xFF,
              (heth.Instance->MACA0HR>>0)  & 0xFF,
              (heth.Instance->MACA0HR>>8)  & 0xFF);

    /* Set remote loopback from port #1 to port #2 */
    coprintf("Testing RMII interface\n");

    if (HAL_OK != (result = PhyWrite_16bit(adr=0x1122, 0x00F4))) goto __write_err; /* Port #1 Remote loopback disable */
    if (HAL_OK != (result = PhyWrite_16bit(adr=0x1108, 0x0181))) goto __write_err; /* Port #1 100Base-Tx, FDPX. IEEE 802.3 */
    if (HAL_OK != (result = PhyWrite_16bit(adr=0x1138, 0x0000))) goto __write_err; /* Port #1 auto MDI/MDI-X enable */
    if (HAL_OK != (result = PhyWrite_16bit(adr=0x2108, 0x0181))) goto __write_err; /* Port #2 100Base-Tx, FDPX. IEEE 802.3 */
    if (HAL_OK != (result = PhyWrite_16bit(adr=0x2138, 0x0000))) goto __write_err; /* Port #2 auto MDI/MDI-X enable */
    if (HAL_OK != (result = PhyWrite_16bit(adr=0x2122, 0x01F4))) goto __write_err; /* Port #2 Remote loopback enable */
    if (HAL_OK != (result = PhyWrite_16bit(adr=0x2100, 0x3300))) goto __write_err; /* Port #2 100Mbps, auto-neg, restart, FDPX */
    if (HAL_OK != (result = PhyWrite_16bit(adr=0x1100, 0x3300))) goto __write_err; /* Port #1 100Mbps, auto-neg, restart, FDPX */

    /* Wait for the loopback cable to be inserted */
    coprintf("Please, connect the loopback cable ...\n %2u ", u8 = 60);
    tick = xTaskGetTickCount();
    do
    {
        /* Check the number of ticks elapsed */
        if ((unsigned)(xTaskGetTickCount() - tick) >= 1000u)
        {
            /* Print a timeout counter */
            coprintf("\r %2u ", --u8);
            if (0 == u8) {coprintf("\rTimeout elapsed\n"); goto __exit;}
            tick += 1000;
        }
        if (coRxReady() && ('\x1B' == coRxChar())) {coprintf("\rTest interrupted\n"); goto __exit;}
        /* Read the PHY Basic Status Register */
        if (HAL_OK != (result = PhyRead_16bit(adr=0x1102, &u16))) goto __read_err;
    /* Wait until the Auto-negotiaition is complete */
    } while (0 == (u16 & 0x0020));

    coprintf("\rLoopback cable detected\n");

    /* Check the Auto-negotiaition result */
    if (HAL_OK != (result = PhyRead_8bit(adr=0x1030, &u8))) goto __read_err;
    if (0x0C != (u8 & 0x1C))
    {
        /* Something went wrong */
        coprintf(">>> Autonegotation failed %02X\n", u8);
        goto __exit;
    }

    coprintf("Tx/Rx test started\n");
    HAL_Delay(1000);
    
    /* Clear the DMA destination buffer */
    memset(&RxPacket, 0, sizeof(RxPacket));
    SCB_CleanInvalidateDCache_by_Addr((void *)&RxPacket, sizeof(RxPacket));

    /* Prepare the Rx filter and buffer */
    memset(&FilterConfig, 0, sizeof(FilterConfig));
    FilterConfig.ReceiveAllMode = ENABLE;
    HAL_ETH_SetMACFilterConfig(&heth, &FilterConfig);
    HAL_ETH_DescAssignMemory(&heth, 0, (uint8_t *)&RxPacket, NULL);

    /* Prepare the message to be transmitted */
    memset(TxPacket.header.dst, 0, sizeof(TxPacket.header.dst));
    memset(TxPacket.header.src, 0, sizeof(TxPacket.header.src));
    memcpy(TxPacket.payload, TestString, sizeof(TestString));
    TxBuffer.buffer = (uint8_t *)&TxPacket;
    TxBuffer.len = sizeof(TxPacket);
    TxBuffer.next = NULL;
    TxConfig.TxBuffer = &TxBuffer;
    TxConfig.Length = sizeof(TxPacket);
    SCB_CleanInvalidateDCache_by_Addr((void *)&TxConfig, sizeof(TxConfig));
    SCB_CleanInvalidateDCache_by_Addr((void *)&TxPacket, sizeof(TxPacket));

    EthStatus = 0;
    
    /* Start ETH peripheral */
    if (HAL_OK != HAL_ETH_Start_IT(&heth))
    {
        coprintf(">>> HAL_ETH_Start_IT() Error\n");
        goto __exit;
    }

    /* Start transmission */
    if (HAL_OK != HAL_ETH_Transmit_IT(&heth, &TxConfig))
    {
        HAL_ETH_Stop_IT(&heth);
        coprintf(">>> HAL_ETH_Transmit_IT() Error\n");
        goto __exit;
    }

    /* Wait for the Rx termination */
    if ((pdPASS != xSemaphoreTake(EthIsrSemaphore, 500)) ||
        (0 == (EthStatus & ETH_STATUS_TXCOMPLETE)) || 
        (0 == (EthStatus & ETH_STATUS_RXCOMPLETE)))
    {
        HAL_ETH_Stop_IT(&heth);
        coprintf(">>> Tx/Rx interrupts not detected\n");
        goto __exit;
    }

    /* Check other errors */
    if (0 != (EthStatus & ETH_STATUS_DMAERROR))
    {
        HAL_ETH_Stop_IT(&heth);
        coprintf(">>> DMA errors detected\n");
        goto __exit;
    }

    if (0 != (EthStatus & ETH_STATUS_MACERROR))
    {
        HAL_ETH_Stop_IT(&heth);
        coprintf(">>> MAC errors detected\n");
        goto __exit;
    }

    /* Stop DMA interrupts */
    HAL_ETH_Stop_IT(&heth);
    SCB_CleanInvalidateDCache_by_Addr((void *)&RxPacket, sizeof(RxPacket));

    /* Compare the Tx and the Rx packet */
    if (0 != memcmp(TxPacket.payload, RxPacket.payload, sizeof(TxPacket.payload)))
    {
        coprintf(">>> Packet errors detected\n");
        goto __exit;
    }

    /* Disable the remote loopback */
    if (HAL_OK != (result = PhyWrite_16bit(adr=0x2122, 0x00F4))) goto __write_err;
    /* Incredible: it works! */
    coprintf("Tx/Rx test terminated without errors\n");

    /* Configure KSZ8563R LEDs in software mode */
    coprintf("Testing KSZ8563R LEDs\n");
    if (HAL_OK != (result = PhyWrite_32bit(adr=0x0124, 0x0000000FUL))) goto __write_err;
    if (HAL_OK != (result = PhyWrite_32bit(adr=0x0120, 0x0000000FUL))) goto __write_err;
    /* Test LEDs */
    coprintf("Press SPACE to step forward\n"
             "Press ENTER to terminate...\n");
    while (1)
    {
        char ch;
        coprintf("-> DL6 Red\n");
        if (HAL_OK != (result = PhyWrite_32bit(adr=0x0124, 0x00000000Eu))) goto __write_err;
        do {if ('\r' == (ch = coRxChar())) goto __led_exit;} while (' ' != ch);
        coprintf("-> DL1 Green\n");
        if (HAL_OK != (result = PhyWrite_32bit(adr=0x0124, 0x00000000Du))) goto __write_err;
        do {if ('\r' == (ch = coRxChar())) goto __led_exit;} while (' ' != ch);
        coprintf("-> DL7 Red\n");
        if (HAL_OK != (result = PhyWrite_32bit(adr=0x0124, 0x00000000Bu))) goto __write_err;
        do {if ('\r' == (ch = coRxChar())) goto __led_exit;} while (' ' != ch);
        coprintf("-> DL2 Green\n");
        if (HAL_OK != (result = PhyWrite_32bit(adr=0x0124, 0x000000007u))) goto __write_err;
        do {if ('\r' == (ch = coRxChar())) goto __led_exit;} while (' ' != ch);
        coprintf("-> All ON\n");
        if (HAL_OK != (result = PhyWrite_32bit(adr=0x0124, 0x000000000u))) goto __write_err;
        do {if ('\r' == (ch = coRxChar())) goto __led_exit;} while (' ' != ch);
        coprintf("-> All OFF\n");
        if (HAL_OK != (result = PhyWrite_32bit(adr=0x0124, 0x00000000Fu))) goto __write_err;
        do {if ('\r' == (ch = coRxChar())) goto __led_exit;} while (' ' != ch);
    }
    
__led_exit:
    /* Restore LEDs status */
    if (HAL_OK != (result = PhyWrite_32bit(adr=0x0124, 0x0000000FUL))) goto __write_err;
    if (HAL_OK != (result = PhyWrite_32bit(adr=0x0120, 0x00000000UL))) goto __write_err;


__exit:
    EthDeInit();
    PhyDeInit();
__quit:
    coprintf("Ethernet test terminated\n");
    return;
__read_err:
    coprintf(">>> KSZ8563R read error (%u) at %04x\n", result, adr);
    goto __exit;
__write_err:
    coprintf(">>> KSZ8563R write error (%u) at %04x\n", result, adr);
    goto __exit;
}


/* END OF FILE ********************************************************************************************************/
