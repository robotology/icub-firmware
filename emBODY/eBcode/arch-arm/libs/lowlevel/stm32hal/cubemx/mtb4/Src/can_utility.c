/****************************************************
*  MTB4 board - iCub Facility - 2017                *
*  CAN Utility                                      *
*                                                   *
*  Written by Andrea Mura                           *
*  <andrea.mura@iit.it>                             *
****************************************************/


#include "stm32l4xx_hal.h"
#include "can.h"
#include "can_utility.h"

// -----------------------------------------------------------------------------------------------------------------------------
// CAN configuration
// -----------------------------------------------------------------------------------------------------------------------------
void CAN_Config(void)
{
  CAN_FilterConfTypeDef  sFilterConfig;
  static CanTxMsgTypeDef        TxMessage;
  static CanRxMsgTypeDef        RxMessage;

  hcan1.pTxMsg = &TxMessage;
  hcan1.pRxMsg = &RxMessage;

  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-2- Configure the CAN Filter ###########################################*/
  sFilterConfig.FilterNumber = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = 0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.BankNumber = 14;
  
  if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
    Error_Handler();
  }
  
  /*##-3- Configure Transmission process #####################################*/
  hcan1.pTxMsg->StdId = 0x621;
  hcan1.pTxMsg->ExtId = 0x01;
  hcan1.pTxMsg->RTR = CAN_RTR_DATA;
  //hcan1.pTxMsg->IDE = CAN_ID_EXT;
  hcan1.pTxMsg->IDE = CAN_ID_STD;
  hcan1.pTxMsg->DLC = 8;
}



// -----------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------------
/**
  * @brief  Transmission  complete callback in non blocking mode 
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* hcan)
{
  HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
}


/**
  * @brief  Reception complete callback in non-blocking mode.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
  if ((hcan1.pRxMsg->StdId == 0x621) && (hcan1.pRxMsg->IDE == CAN_ID_STD) && (hcan1.pRxMsg->DLC == 2))
  {
    HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
    CANBUS();
  }
  
  /* Receive */
  if (HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0) != HAL_OK)
  {
    /* Reception Error */
    Error_Handler();
  }
}
