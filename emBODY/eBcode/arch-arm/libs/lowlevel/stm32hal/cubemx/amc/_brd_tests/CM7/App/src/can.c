/*******************************************************************************************************************//**
 * @file    can.c
 * @author  G.Zini
 * @version 1.0
 * @date    2021 July, 20
 * @brief   FDCAN management for CM7 core
 *
 * Bit rate calculation:
 *
 * Bit time parameter         | Nominal      |  Data
 * ---------------------------|--------------|----------------
 * fdcan_ker_ck               | 50 MHz       | 50 MHz
 * Time_quantum (tq)          | 20 ns        | 20 ns
 * Synchronization_segment    |  1 tq        |  1 tq
 * Propagation_segment        |  1 tq        |  1 tq
 * Phase_segment_1            | 39 tq        |  6 tq
 * Phase_segment_2            | 10 tq        |  3 tq
 * Synchronization_Jump_width | 10 tq        |  4 tq
 * Bit_length                 | 50 tq = 1 us | 10 tq = 0.2 us
 * Bit_rate                   |  1 MBit/s    |  5 MBit/s
 *
 * Transceiver TCAN334G max delay Tx->Rx 180 ns
 **********************************************************************************************************************/

/* Includes ***********************************************************************************************************/
#include <string.h>
#include <stdlib.h>
#include "can.h"
#include "main.h"
#include "stm32h7xx_hal_fdcan.h"
#include "led.h"
#include "console.h"
#include "FreeRTOS.h"
#include "semphr.h"


/* Private macros *****************************************************************************************************/

#define TIME_QUANTA             (20 /*ns*/)
#define TRANSCEIVER_MAXDELAY    (180/TIME_QUANTA)
#define TRANSCEIVER_MINDELAY    (20/TIME_QUANTA)

#define CAN_STATUS_TXOK         (0x01)
#define CAN_STATUS_RXOK         (0x02)
#define CAN_STATUS_ERROR        (0x04)

#define CAN_TIMEOUT             (100/*ms*/)
#define CAN_PACKET_SIZE         (64)

#define CRC8_POLY               (0xD5)


/* Extenal variables **************************************************************************************************/

/* Defined in main.c */
extern FDCAN_HandleTypeDef hfdcan1;
extern FDCAN_HandleTypeDef hfdcan2;


/* Private typedefs ***************************************************************************************************/

/* Private variables **************************************************************************************************/

static volatile SemaphoreHandle_t Can1IsrSemaphore = NULL ;
static volatile HAL_StatusTypeDef Can1Status = HAL_OK;

static volatile SemaphoreHandle_t Can2IsrSemaphore = NULL ;
static volatile HAL_StatusTypeDef Can2Status = HAL_OK;

static char CanTxMessage[CAN_PACKET_SIZE];
static char CanRxMessage[CAN_PACKET_SIZE];


/* Local functions ****************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
uint8_t crc_rand(uint8_t *pCrc)
{
    unsigned n;
    uint8_t crc = *pCrc;
    for (n = 0; n < 8; n++)
    {
        if (crc & 0x80)
        {
            crc <<= 1;
            crc ^= CRC8_POLY;
        }
        else crc <<= 1;
    }
    return *pCrc = crc;
}


//* Callback functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void Can1_TxBufferComplete_cb(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes)
{
    BaseType_t Woken;
    if (1 == BufferIndexes)
    {
        /* Transmission done */
        Can1Status |= CAN_STATUS_TXOK;
    }
    else
    {
        /* Some errors detected */
        Can1Status |= CAN_STATUS_ERROR;
        /* Run the scheduler if needed */
        xSemaphoreGiveFromISR(Can1IsrSemaphore, &Woken);
        portYIELD_FROM_ISR(Woken);
    }
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void Can2_TxBufferComplete_cb(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes)
{
    BaseType_t Woken;
    if (1 == BufferIndexes)
    {
        /* Transmission done */
        Can2Status |= CAN_STATUS_TXOK;
    }
    else
    {
        /* Some errors detected */
        Can2Status |= CAN_STATUS_ERROR;
        /* Run the scheduler if needed */
        xSemaphoreGiveFromISR(Can2IsrSemaphore, &Woken);
        portYIELD_FROM_ISR(Woken);
    }
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void Can1_RxNewMsg_cb(FDCAN_HandleTypeDef *hfdcan)
{
    BaseType_t Woken;
    /* Message recived ok */
    Can1Status |= CAN_STATUS_RXOK;
    /* Run the scheduler if needed */
    xSemaphoreGiveFromISR(Can1IsrSemaphore, &Woken);
    portYIELD_FROM_ISR(Woken);
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void Can2_RxNewMsg_cb(FDCAN_HandleTypeDef *hfdcan)
{
    BaseType_t Woken;
    /* Message recived ok */
    Can2Status |= CAN_STATUS_RXOK;
    /* Run the scheduler if needed */
    xSemaphoreGiveFromISR(Can2IsrSemaphore, &Woken);
    portYIELD_FROM_ISR(Woken);
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void Can1_Error_cb(FDCAN_HandleTypeDef *hfdcan)
{
    BaseType_t Woken;
    /* Some errors detected */
    Can1Status |= CAN_STATUS_ERROR;
    /* Run the scheduler if needed */
    xSemaphoreGiveFromISR(Can1IsrSemaphore, &Woken);
    portYIELD_FROM_ISR(Woken);
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void Can2_Error_cb(FDCAN_HandleTypeDef *hfdcan)
{
    BaseType_t Woken;
    /* Some errors detected */
    Can2Status |= CAN_STATUS_ERROR;
    /* Run the scheduler if needed */
    xSemaphoreGiveFromISR(Can2IsrSemaphore, &Woken);
    portYIELD_FROM_ISR(Woken);
}


/* Exported functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
HAL_StatusTypeDef CanInit(void)
{
    FDCAN_FilterTypeDef RxFilterConfig;

    /* Not initializad yet ? */
    if ((NULL == Can1IsrSemaphore) && (NULL == Can2IsrSemaphore))
    {
        /* Create CAN1 ISR semaphores */
        if (NULL != (Can1IsrSemaphore = xSemaphoreCreateBinary()))
        {
            /* Create CAN2 ISR semaphores */
            if (NULL != (Can2IsrSemaphore = xSemaphoreCreateBinary()))
            {
                /* Take the semaphore if not already taken */
                xSemaphoreTake(Can1IsrSemaphore, 0);
                /* Take the semaphore if not already taken */
                xSemaphoreTake(Can2IsrSemaphore, 0);

                /* Init CAN1 peripheral */
                MX_FDCAN1_Init();
                HAL_FDCAN_MspInit(&hfdcan1);
                /* Register the function callbacks */
                HAL_FDCAN_RegisterTxBufferCompleteCallback(&hfdcan1, Can1_TxBufferComplete_cb);
                HAL_FDCAN_RegisterCallback(&hfdcan1, HAL_FDCAN_RX_BUFFER_NEW_MSG_CB_ID, Can1_RxNewMsg_cb);
                HAL_FDCAN_RegisterCallback(&hfdcan1, HAL_FDCAN_ERROR_CALLBACK_CB_ID, Can1_Error_cb);
                HAL_FDCAN_RegisterCallback(&hfdcan1, HAL_FDCAN_TIMEOUT_OCCURRED_CB_ID, Can1_Error_cb);
                /* Init CAN1 Rx filters */
                RxFilterConfig.IdType = FDCAN_EXTENDED_ID;
                RxFilterConfig.FilterIndex = 0;
                RxFilterConfig.FilterType = FDCAN_FILTER_MASK;
                RxFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXBUFFER;
                RxFilterConfig.FilterID1 = 0x0AAAAAAA;
                RxFilterConfig.FilterID2 = 0x1FFFFFFF;
                RxFilterConfig.RxBufferIndex = 0;
                RxFilterConfig.IsCalibrationMsg = 0;
                HAL_FDCAN_ConfigFilter(&hfdcan1, &RxFilterConfig);
                /* Configure CAN1 global filter */
                HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);
                /* Configure the Transceiver-Delay compensation circuit */
                HAL_FDCAN_ConfigTxDelayCompensation(&hfdcan1, TRANSCEIVER_MAXDELAY, TRANSCEIVER_MINDELAY);
                HAL_FDCAN_EnableTxDelayCompensation(&hfdcan1);
                /* Activate the external driver out of STANDBY */
                HAL_GPIO_WritePin(CAN1_STBY_GPIO_Port, CAN1_STBY_Pin, GPIO_PIN_RESET);

                /* Init CAN2 peripheral */
                MX_FDCAN2_Init();
                HAL_FDCAN_MspInit(&hfdcan2);
                /* Register the function callbacks */
                HAL_FDCAN_RegisterTxBufferCompleteCallback(&hfdcan2, Can2_TxBufferComplete_cb);
                HAL_FDCAN_RegisterCallback(&hfdcan2, HAL_FDCAN_RX_BUFFER_NEW_MSG_CB_ID, Can2_RxNewMsg_cb);
                HAL_FDCAN_RegisterCallback(&hfdcan2, HAL_FDCAN_ERROR_CALLBACK_CB_ID, Can2_Error_cb);
                HAL_FDCAN_RegisterCallback(&hfdcan2, HAL_FDCAN_TIMEOUT_OCCURRED_CB_ID, Can2_Error_cb);
                /* Init CAN2 Rx filters */
                RxFilterConfig.IdType = FDCAN_EXTENDED_ID;
                RxFilterConfig.FilterIndex = 0;
                RxFilterConfig.FilterType = FDCAN_FILTER_MASK;
                RxFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXBUFFER;
                RxFilterConfig.FilterID1 = 0x15555555;
                RxFilterConfig.FilterID2 = 0x1FFFFFFF;
                RxFilterConfig.RxBufferIndex = 0;
                RxFilterConfig.IsCalibrationMsg = 0;
                HAL_FDCAN_ConfigFilter(&hfdcan2, &RxFilterConfig);
                /* Configure CAN2 global filter */
                HAL_FDCAN_ConfigGlobalFilter(&hfdcan2, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);
                /* Configure the Transceiver-Delay compensation circuit */
                HAL_FDCAN_ConfigTxDelayCompensation(&hfdcan2, TRANSCEIVER_MAXDELAY, TRANSCEIVER_MINDELAY);
                HAL_FDCAN_EnableTxDelayCompensation(&hfdcan2);
                /* Activate the external driver out of STANDBY */
                HAL_GPIO_WritePin(CAN2_STBY_GPIO_Port, CAN2_STBY_Pin, GPIO_PIN_RESET);
                
                /* Activate both driver out of SHUTDOWN mode */
                HAL_GPIO_WritePin(CAN_SHDN_GPIO_Port, CAN_SHDN_Pin, GPIO_PIN_RESET);
                HAL_Delay(10);

                /* All done */
                return HAL_OK;
            }
            /* Remove the semaphore */
            vSemaphoreDelete(Can1IsrSemaphore);
            Can1IsrSemaphore = NULL;
        }
    }
    return HAL_ERROR;
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void CanDeInit(void)
{
    /* Disable transceiver (low power mode) */
    HAL_GPIO_WritePin(CAN_SHDN_GPIO_Port, CAN_SHDN_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CAN1_STBY_GPIO_Port, CAN1_STBY_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CAN2_STBY_GPIO_Port, CAN2_STBY_Pin, GPIO_PIN_SET);
    /* Deinit FDCAN1 and FDCAN2 peripheral */
    HAL_FDCAN_DeInit(&hfdcan1);
    HAL_FDCAN_MspDeInit(&hfdcan1);
    HAL_FDCAN_DeInit(&hfdcan2);
    HAL_FDCAN_MspDeInit(&hfdcan2);
    /* Remove semaphores */
    vSemaphoreDelete(Can1IsrSemaphore);
    Can1IsrSemaphore = NULL;
    vSemaphoreDelete(Can2IsrSemaphore);
    Can2IsrSemaphore = NULL;
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void CanTest(void)
{
    FDCAN_TxHeaderTypeDef TxHeader;
    FDCAN_RxHeaderTypeDef RxHeader;
    unsigned i, n;
    uint8_t crc;
    
    coprintf("CAN-bus test started\n");
    CanInit();

    /* Reset status words */
    Can1Status = HAL_OK;
    Can2Status = HAL_OK;

    /* Start FDCAN1 */
    if ((HAL_OK != HAL_FDCAN_ActivateNotification(&hfdcan1, 
                                                  FDCAN_IT_TX_COMPLETE|FDCAN_IT_RX_BUFFER_NEW_MESSAGE, 
                                                  FDCAN_TX_BUFFER0)) ||
        (HAL_OK != HAL_FDCAN_Start(&hfdcan1)))
    {
        coprintf(">>> FDCAN1 start failed\n");
        goto __exit;
    }
    
    /* Start FDCAN2 */
    if ((HAL_OK != HAL_FDCAN_ActivateNotification(&hfdcan2, 
                                                  FDCAN_IT_TX_COMPLETE|FDCAN_IT_RX_BUFFER_NEW_MESSAGE, 
                                                  FDCAN_TX_BUFFER0)) ||
        (HAL_OK != HAL_FDCAN_Start(&hfdcan2)))
    {
        coprintf(">>> FDCAN2 start failed\n");
        goto __exit;
    }

    /* Request the loopback cable connection */
    do {coprintf("Connect loopback cable and press ENTER when ready\n");
    } while ('\r' != coRxChar());

    /* Advertisement */
    coprintf("Loopback test started\n");
    
    /* Random seed */
    crc = 0x55;
    /* Test cycle */
    for (i = 0; i < 10000 ; i++)
    {
        /* Generate a pseudo random packet */
        for (n = 0; n < CAN_PACKET_SIZE; n++) CanTxMessage[n] = crc_rand(&crc);

        /* Add messages to CAN1 Tx buffer */
        TxHeader.Identifier = 0x15555555;
        TxHeader.IdType = FDCAN_EXTENDED_ID;
        TxHeader.TxFrameType = FDCAN_DATA_FRAME;
        TxHeader.DataLength = FDCAN_DLC_BYTES_64;
        TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
        TxHeader.BitRateSwitch = FDCAN_BRS_ON;
        TxHeader.FDFormat = FDCAN_FD_CAN;
        TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
        TxHeader.MessageMarker = 0;
        if ((HAL_OK != HAL_FDCAN_AddMessageToTxBuffer(&hfdcan1, &TxHeader, (void *)CanTxMessage, FDCAN_TX_BUFFER0)) ||
            (HAL_OK != HAL_FDCAN_EnableTxBufferRequest(&hfdcan1, FDCAN_TX_BUFFER0)))
        {
            coprintf(">>> FDCAN1 TX message failed\n");
            goto __exit;
        }

        /* Wait interrupt */
        if (pdPASS != xSemaphoreTake(Can2IsrSemaphore, CAN_TIMEOUT))
        {
            if (1 != Can1Status) coprintf(">>> FDCAN1 TX error, ");
            if (2 != Can2Status) coprintf(">>> FDCAN2 RX error, ");
            coprintf("Loopback test failed\n");
            goto __exit;
        }

        /* Read the received message */
        if (HAL_OK != HAL_FDCAN_GetRxMessage(&hfdcan2, FDCAN_RX_BUFFER0, &RxHeader, (void *)CanRxMessage))
        {
            coprintf(">>> FDCAN2 memory read error\n");
            goto __exit;
        }
        
        /* Check received data */
        if (0 != memcmp(CanRxMessage, CanTxMessage, CAN_PACKET_SIZE))
        {
            coprintf(">>> FDCAN2 Rx data error\n");
            goto __exit;
        }

        /* Generate a new pseudo random packet */
        for (n = 0; n < CAN_PACKET_SIZE; n++) CanTxMessage[n] = crc_rand(&crc);

        /* Add messages to CAN2 Tx buffer */
        TxHeader.Identifier = 0x0AAAAAAA;
        TxHeader.IdType = FDCAN_EXTENDED_ID;
        TxHeader.TxFrameType = FDCAN_DATA_FRAME;
        TxHeader.DataLength = FDCAN_DLC_BYTES_64;
        TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
        TxHeader.BitRateSwitch = FDCAN_BRS_ON;
        TxHeader.FDFormat = FDCAN_FD_CAN;
        TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
        TxHeader.MessageMarker = 0;
        if ((HAL_OK != HAL_FDCAN_AddMessageToTxBuffer(&hfdcan2, &TxHeader, (void *)CanTxMessage, FDCAN_TX_BUFFER0)) ||
            (HAL_OK != HAL_FDCAN_EnableTxBufferRequest(&hfdcan2, FDCAN_TX_BUFFER0)))
        {
            coprintf(">>> FDCAN2 TX message failed\n");
            goto __exit;
        }

        /* Wait interrupt */
        if (pdPASS != xSemaphoreTake(Can1IsrSemaphore, CAN_TIMEOUT))
        {
            if (1 != Can2Status) coprintf(">>> FDCAN2 TX error, ");
            if (2 != Can1Status) coprintf(">>> FDCAN1 RX error, ");
            coprintf("Loopback test failed\n");
            goto __exit;
        }

        /* Read the received message */
        if (HAL_OK != HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_BUFFER0, &RxHeader, (void *)CanRxMessage))
        {
            coprintf(">>> FDCAN1 memory read error\n");
            goto __exit;
        }
        
        /* Check received data */
        if (0 != memcmp(CanRxMessage, CanTxMessage, CAN_PACKET_SIZE))
        {
            coprintf(">>> FDCAN1 Rx data error\n");
            goto __exit;
        }
    }

    /* All done */
    coprintf("No errors detected\n");

    /* Stop the FDCAN peripherals */
    HAL_FDCAN_Stop(&hfdcan1);
    HAL_FDCAN_Stop(&hfdcan2);
    
__exit:
    CanDeInit();
    coprintf("CAN-bus test terminated\n");
}


/* END OF FILE ********************************************************************************************************/
