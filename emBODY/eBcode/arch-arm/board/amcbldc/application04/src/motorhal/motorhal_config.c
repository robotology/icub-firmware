
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#if 0
    The files motorhal_config.[h, c] contain macros and code which cuts aways un-necessary dependencies
    for the files provided by Giorgio Zini for the motor control of the amcbldc. These files are:
    encoder.[c, h], pwm.[c, h], analog.[c, h]. The dependencies cut away are those which offer
    generic services such as logging, leds, flash storage.    
#endif

// CODE SHAPER
#if defined(USE_STM32HAL)
#include "motorhal_config.h"
#endif

#if defined(HALCONFIG_DONTUSE_FLASH)

MainConfTypeDef MainConf = {0};

#else
#endif // HALCONFIG_DONTUSE_FLASH


#if defined(HALCONFIG_ADD_IRQHANDLERS)

#include "stm32hal.h"

/**
  * @brief This function handles DMA1 channel1 global interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel2 global interrupt.
  */
void DMA1_Channel2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel2_IRQn 0 */

  /* USER CODE END DMA1_Channel2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc2);
  /* USER CODE BEGIN DMA1_Channel2_IRQn 1 */

  /* USER CODE END DMA1_Channel2_IRQn 1 */
}

/**
  * @brief This function handles ADC1 and ADC2 global interrupt.
  */
void ADC1_2_IRQHandler(void)
{
  /* USER CODE BEGIN ADC1_2_IRQn 0 */

  /* USER CODE END ADC1_2_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc1);
  HAL_ADC_IRQHandler(&hadc2);
  /* USER CODE BEGIN ADC1_2_IRQn 1 */

  /* USER CODE END ADC1_2_IRQn 1 */
}

///**
//  * @brief This function handles USB low priority interrupt remap.
//  */
//void USB_LP_IRQHandler(void)
//{
//  /* USER CODE BEGIN USB_LP_IRQn 0 */

//  /* USER CODE END USB_LP_IRQn 0 */
//  HAL_PCD_IRQHandler(&hpcd_USB_FS);
//  /* USER CODE BEGIN USB_LP_IRQn 1 */

//  /* USER CODE END USB_LP_IRQn 1 */
//}

/**
  * @brief This function handles TIM1 break interrupt and TIM15 global interrupt.
  */
void TIM1_BRK_TIM15_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_BRK_TIM15_IRQn 0 */

  /* USER CODE END TIM1_BRK_TIM15_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_BRK_TIM15_IRQn 1 */

  /* USER CODE END TIM1_BRK_TIM15_IRQn 1 */
}

/**
  * @brief This function handles TIM1 update interrupt and TIM16 global interrupt.
  */
void TIM1_UP_TIM16_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_TIM16_IRQn 0 */

  /* USER CODE END TIM1_UP_TIM16_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_UP_TIM16_IRQn 1 */

  /* USER CODE END TIM1_UP_TIM16_IRQn 1 */
}

/**
  * @brief This function handles TIM1 trigger and commutation interrupts and TIM17 global interrupt.
  */
void TIM1_TRG_COM_TIM17_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_TRG_COM_TIM17_IRQn 0 */

  /* USER CODE END TIM1_TRG_COM_TIM17_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_TRG_COM_TIM17_IRQn 1 */

  /* USER CODE END TIM1_TRG_COM_TIM17_IRQn 1 */
}

/**
  * @brief This function handles TIM1 capture compare interrupt.
  */
void TIM1_CC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_CC_IRQn 0 */

  /* USER CODE END TIM1_CC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_CC_IRQn 1 */

  /* USER CODE END TIM1_CC_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */

  /* USER CODE END TIM3_IRQn 0 */
  HAL_TIM_IRQHandler(&htim3);
  /* USER CODE BEGIN TIM3_IRQn 1 */

  /* USER CODE END TIM3_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

  /* USER CODE END EXTI15_10_IRQn 1 */
}

///**
//  * @brief This function handles I2C4 event interrupt / I2C4 wake-up interrupt through EXTI line 42.
//  */
//void I2C4_EV_IRQHandler(void)
//{
//  /* USER CODE BEGIN I2C4_EV_IRQn 0 */

//  /* USER CODE END I2C4_EV_IRQn 0 */
//  HAL_I2C_EV_IRQHandler(&hi2c4);
//  /* USER CODE BEGIN I2C4_EV_IRQn 1 */

//  /* USER CODE END I2C4_EV_IRQn 1 */
//}

///**
//  * @brief This function handles I2C4 error interrupt.
//  */
//void I2C4_ER_IRQHandler(void)
//{
//  /* USER CODE BEGIN I2C4_ER_IRQn 0 */

//  /* USER CODE END I2C4_ER_IRQn 0 */
//  HAL_I2C_ER_IRQHandler(&hi2c4);
//  /* USER CODE BEGIN I2C4_ER_IRQn 1 */

//  /* USER CODE END I2C4_ER_IRQn 1 */
//}


#endif

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
