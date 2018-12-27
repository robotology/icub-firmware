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
#include "adc.h"
#include "PGA308.h"

uint8_t *RxCanBuffer;
extern uint8_t PGA308_channel;
extern uint8_t REG_OK;
extern uint8_t REGISTER;
extern uint8_t CHANNEL;
extern uint8_t PGA308_StartCalib;

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
  //HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_values, 6);
}


/**
  * @brief  Reception complete callback in non-blocking mode.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
  if ((hcan1.pRxMsg->StdId == 0x621) && (hcan1.pRxMsg->IDE == CAN_ID_STD) && (hcan1.pRxMsg->DLC == 4))  // Write the registers
  {
    RxCanBuffer = hcan1.pRxMsg->Data;
    
    if(RxCanBuffer[1] == 0xF2){       // CFG0
      if(RxCanBuffer[2] == 0x01){     // CFG0_GO
        PGA308[RxCanBuffer[0]].CFG0 = (PGA308[RxCanBuffer[0]].CFG0 & 0x1FFF) | (RxCanBuffer[3]<<13);
      }
      if(RxCanBuffer[2] == 0x02){     // CFG0_MUX
        PGA308[RxCanBuffer[0]].CFG0 = (PGA308[RxCanBuffer[0]].CFG0 & 0xEFFF) | (RxCanBuffer[3]<<12);
      }
      if(RxCanBuffer[2] == 0x03){     // CFG0_GI
        PGA308[RxCanBuffer[0]].CFG0 = (PGA308[RxCanBuffer[0]].CFG0 & 0xF0FF) | (RxCanBuffer[3]<<8);
      }
      if(RxCanBuffer[2] == 0x04){     // CFG0_OS
        PGA308[RxCanBuffer[0]].CFG0 = (PGA308[RxCanBuffer[0]].CFG0 & 0xFF00) | RxCanBuffer[3];
      }
      PGA308_WriteRegister(RxCanBuffer[0], RAM_CFG0, PGA308[RxCanBuffer[0]].CFG0);
    }
    else{
      PGA308_WriteRegister(RxCanBuffer[0], RxCanBuffer[1], (((RxCanBuffer[2])<<8) + RxCanBuffer[3]) );
    }
  }
  if ((hcan1.pRxMsg->StdId == 0x621) && (hcan1.pRxMsg->IDE == CAN_ID_STD) && (hcan1.pRxMsg->DLC == 2))
  {
    RxCanBuffer = hcan1.pRxMsg->Data;
    if(RxCanBuffer[0] == 0x00 && RxCanBuffer[1] == 0x01){   // Start sensor calibration
      REG_OK=0;
      REGISTER=RAM_CFG0;
      CHANNEL=1;
      PGA308_StartCalib=1;
    }
    else if(RxCanBuffer[0] == 0xA0){                        // Read the ZDAC register
      PGA308_ReadRegister(RxCanBuffer[1], RAM_ZDAC);
    }
    else if(RxCanBuffer[0] == 0xA2){                        // Read the CFG0 register
      PGA308_ReadRegister(RxCanBuffer[1], RAM_CFG0);
    }
  }
  
  
  /* Receive */
  if (HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0) != HAL_OK)
  {
    /* Reception Error */
    //Error_Handler();
  }
}
