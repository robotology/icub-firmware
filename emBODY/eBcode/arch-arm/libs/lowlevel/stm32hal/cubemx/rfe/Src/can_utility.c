/****************************************************
*  RFE_MASTER board - iCub Facility - 2018          *
*  CAN Utility                                      *
*                                                   *
*  Written by Andrea Mura                           *
*  <andrea.mura@iit.it>                             *
****************************************************/


#include "stm32l4xx_hal.h"
#include "can.h"
#include "can_utility.h"

uint8_t *RxCanBuffer;

uint8_t ubKeyNumber; // = 0x0;

CAN_TxHeaderTypeDef   TxHeader;
CAN_RxHeaderTypeDef   RxHeader;
uint8_t               TxData[8];
uint8_t               RxData[8];
uint32_t              TxMailbox;

// -----------------------------------------------------------------------------------------------------------------------------
// CAN configuration
// -----------------------------------------------------------------------------------------------------------------------------
void CAN_Config(void)
{
  CAN_FilterTypeDef  sFilterConfig;

  /*##-2- Configure the CAN Filter ###########################################*/
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;

  if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
    Error_Handler();
  }

  /*##-3- Start the CAN peripheral ###########################################*/
  if (HAL_CAN_Start(&hcan1) != HAL_OK)
  {
    /* Start Error */
    Error_Handler();
  }

  /*##-4- Activate CAN RX notification #######################################*/
  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    /* Notification Error */
    Error_Handler();
  }

  /*##-5- Configure Transmission process #####################################*/
  TxHeader.StdId = 0x621;
  TxHeader.ExtId = 0x01;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.DLC = 8;
  TxHeader.TransmitGlobalTime = DISABLE;
}



// -----------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------------
/**
  * @brief  Transmission Mailbox 0 complete callback.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef* hcan)
{
  //HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_values, 6);
}


/**
  * @brief  Rx Fifo 0 message pending callback
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  /* Get RX message */
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
  {
    /* Reception Error */
    Error_Handler();
  }

  /* Display LEDx */
  if ((RxHeader.StdId == 0x621) && (RxHeader.IDE == CAN_ID_STD) && (RxHeader.DLC == 8))
  {
    //LED_Display(RxData[0]);
    //ubKeyNumber = RxData[0];
  }
}
