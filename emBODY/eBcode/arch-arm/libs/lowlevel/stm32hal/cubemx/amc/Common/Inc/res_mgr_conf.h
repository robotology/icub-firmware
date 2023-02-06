/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    res_mgr_conf.h
  * @author  MCD Application Team
  * @brief   Resources Manager configuration template file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef  RES_MGR_CONF_H__
#define  RES_MGR_CONF_H__

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#include "stm32h7xx_hal.h"
/** @addtogroup Utilities
  * @{
  */

/** @addtogroup Common
  * @{
  */

/** @addtogroup RES_MGR_TABLE
  * @{
  */

/** @defgroup RES_MGR_TABLE
  * @brief
  * @{
  */

/** @defgroup RES_MGR_Exported_Defines
  * @{
  */

/* This Part may be filled/customized by CubeMX/User */
/******** Shared Resource IDs *************************************************/
enum
{
  RESMGR_ID_ADC1,
  RESMGR_ID_ADC12_COMMON,
  RESMGR_ID_ADC2,
  RESMGR_ID_ADC3,
  RESMGR_ID_BDMA,
  RESMGR_ID_CRC,
  RESMGR_ID_CRS,
  RESMGR_ID_DAC1,
  RESMGR_ID_DEBUG,
  RESMGR_ID_DMA1,
  RESMGR_ID_DMA1_Stream2,
  RESMGR_ID_DMA1_Stream3,
  RESMGR_ID_DMA2,
  RESMGR_ID_DMAMUX1,
  RESMGR_ID_DMAMUX1_Channel2,
  RESMGR_ID_DMAMUX1_Channel3,
  RESMGR_ID_DMAMUX2,
  RESMGR_ID_ETH,
  RESMGR_ID_FDCAN_CCU,
  RESMGR_ID_FDCAN1,
  RESMGR_ID_FDCAN2,
  RESMGR_ID_GPIO,
  RESMGR_ID_HRTIM,
  RESMGR_ID_HSEM,
  RESMGR_ID_I2C1,
  RESMGR_ID_I2C2,
  RESMGR_ID_I2C3,
  RESMGR_ID_I2C4,
  RESMGR_ID_IWDG1,
  RESMGR_ID_IWDG2,
  RESMGR_ID_LPTIM1,
  RESMGR_ID_MDMA,
  RESMGR_ID_PWR,
  RESMGR_ID_RNG,
  RESMGR_ID_RTC,
  RESMGR_ID_SPI1,
  RESMGR_ID_SPI2,
  RESMGR_ID_SPI3,
  RESMGR_ID_SPI5,
  RESMGR_ID_SPI6,
  RESMGR_ID_SYSCFG,
  RESMGR_ID_TIM1,
  RESMGR_ID_TIM16,
  RESMGR_ID_TIM17,
  RESMGR_ID_TIM4,
  RESMGR_ID_TIM5,
  RESMGR_ID_TIM6,
  RESMGR_ID_USB_OTG_FS,
  RESMGR_ID_VREFBUF,
  RESMGR_ID_WWDG1,
  RESMGR_ID_WWDG2,
  /* USER CODE BEGIN 0 */
  /* USER CODE END 0 */
  RESMGR_ID_RESMGR_TABLE,
};

#define RESMGR_ENTRY_NBR    ((uint32_t)RESMGR_ID_RESMGR_TABLE + 1UL)
#define RESMGR_ID_ALL                      0x0000FFFFU
#define RESMGR_ID_NONE                     0xFFFFFFFFU

/**
  * @}
  */

/** @defgroup RES_MGR_Default_ResTable
  * @{
  */

#define RESMGR_USE_DEFAULT_TBL

#define RES_DEFAULT_ASSIGN_NONE   0
#define RES_DEFAULT_ASSIGN_CPU1   1
#define RES_DEFAULT_ASSIGN_CPU2   2

#ifdef RESMGR_USE_DEFAULT_TBL
static const uint8_t Default_ResTbl[RESMGR_ENTRY_NBR] = {
/* RES_DEFAULT_ASSIGN_NONE:Not assigned | RES_DEFAULT_ASSIGN_CPU1:Assigned to core1 | RES_DEFAULT_ASSIGN_CPU2:Assigned to core2 */
/* core1 -> CPU1, CortexM7 for STM32H7 */
/* core2 -> CPU2, CortexM4 for STM32H7 */
  RES_DEFAULT_ASSIGN_CPU2,  /* RESMGR_ID_ADC1 */
  RES_DEFAULT_ASSIGN_CPU2,  /* RESMGR_ID_ADC12_COMMON */
  RES_DEFAULT_ASSIGN_CPU2,  /* RESMGR_ID_ADC2 */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_ADC3 */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_BDMA */
  RES_DEFAULT_ASSIGN_CPU2,  /* RESMGR_ID_CRC */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_CRS */
  RES_DEFAULT_ASSIGN_CPU2,  /* RESMGR_ID_DAC1 */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_DEBUG */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_DMA1 */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_DMA1_Stream2 */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_DMA1_Stream3 */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_DMA2 */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_DMAMUX1 */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_DMAMUX1_Channel2 */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_DMAMUX1_Channel3 */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_DMAMUX2 */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_ETH */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_FDCAN_CCU */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_FDCAN1 */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_FDCAN2 */
  RES_DEFAULT_ASSIGN_NONE,  /* RESMGR_ID_GPIO */
  RES_DEFAULT_ASSIGN_CPU2,  /* RESMGR_ID_HRTIM */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_HSEM */
  RES_DEFAULT_ASSIGN_CPU2,  /* RESMGR_ID_I2C1 */
  RES_DEFAULT_ASSIGN_CPU2,  /* RESMGR_ID_I2C2 */
  RES_DEFAULT_ASSIGN_CPU2,  /* RESMGR_ID_I2C3 */
  RES_DEFAULT_ASSIGN_CPU2,  /* RESMGR_ID_I2C4 */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_IWDG1 */
  RES_DEFAULT_ASSIGN_CPU2,  /* RESMGR_ID_IWDG2 */
  RES_DEFAULT_ASSIGN_CPU2,  /* RESMGR_ID_LPTIM1 */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_MDMA */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_PWR */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_RNG */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_RTC */
  RES_DEFAULT_ASSIGN_CPU2,  /* RESMGR_ID_SPI1 */
  RES_DEFAULT_ASSIGN_CPU2,  /* RESMGR_ID_SPI2 */
  RES_DEFAULT_ASSIGN_CPU2,  /* RESMGR_ID_SPI3 */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_SPI5 */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_SPI6 */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_SYSCFG */
  RES_DEFAULT_ASSIGN_CPU2,  /* RESMGR_ID_TIM1 */
  RES_DEFAULT_ASSIGN_CPU2,  /* RESMGR_ID_TIM16 */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_TIM17 */
  RES_DEFAULT_ASSIGN_CPU2,  /* RESMGR_ID_TIM4 */
  RES_DEFAULT_ASSIGN_CPU2,  /* RESMGR_ID_TIM5 */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_TIM6 */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_USB_OTG_FS */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_VREFBUF */
  RES_DEFAULT_ASSIGN_CPU1,  /* RESMGR_ID_WWDG1 */
  RES_DEFAULT_ASSIGN_CPU2,  /* RESMGR_ID_WWDG2 */
/* USER CODE BEGIN 1 */
/* USER CODE END 1 */
  RES_DEFAULT_ASSIGN_NONE /* RESMGR_ID_RESMGR_TABLE               */
};
#endif
/**
  * @}
  */

/* End of CubeMX/User Part*/

/** @defgroup RES_MGR_Lock_Procedure
  * @{
  */

/* Customized Lock Procedure definition  begin */

/* USER CODE BEGIN 2 */

#define HSEM_ID_RES_TABLE                      31U

#define RESMGR_TBL_LOCK(lock_id)                          \
{                                                         \
  while (HAL_HSEM_FastTake(HSEM_ID_RES_TABLE) != HAL_OK)  \
  {                                                       \
  }                                                       \
}                                                         \

#define RESMGR_TBL_UNLOCK(lock_id)                        \
{                                                         \
  HAL_HSEM_Release(HSEM_ID_RES_TABLE,0);                  \
}                                                         \

/* USER CODE END 2 */

/* Customized Lock Procedure definition  end */

/* USER CODE BEGIN 3 */
/* USER CODE END 3 */

/**
  * @}
  */

/** @defgroup RES_MGR_RPMSG extension
  * @{
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
#endif /* RES_MGR_CONF_H__ */

