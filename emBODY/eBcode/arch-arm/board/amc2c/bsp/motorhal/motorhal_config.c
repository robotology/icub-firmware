
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#if 0
    The files motorhal_config.[h, c] contain macros and code which cuts aways un-necessary dependencies
    for the files provided by Giorgio Zini for the motor control of the amc2c. These files are:
    enc.[c, h], pwm.[c, h], adcm.[c, h], hall.[c, h]. The dependencies cut away are those which offer
    generic services such as logging, leds, flash storage.    
#endif

// CODE SHAPER
#if defined(USE_STM32HAL)
#include "motorhal_config.h"
#endif

// ------------------------------------------------------------------------------------------------------------------------------------------
// hw variables requyired by adcm.c, etc. whuhc gzini has palced somewhere else. maybe in main.c

#warning TODO-motorhal: for now i add in here all hw variables ....
    
#if 0

    we need: TIM1, TIM4, TIM5 for the motorhal
    the amc uses: TIM13, TIM15, TIM16 for its control loop.

#endif
    

// - enc section

// 1. the TIM5, w/ name htim5 or better w/ macro ENC_TIM is used by enc.c
//    we need: 
//    1. the variable, 
//    2. the MX_TIM5_Init(), HAL_TIM_IC_MspInit(), HAL_TIM_IC_MspDeInit() -> [files main.c and stm32h7xx_hal_msp.c]
//    3. the TIM5_IRQHandler() 
    
//TIM_HandleTypeDef htim5;

// and associated IRQ Handler, maybe in extern C ...

/**
  * @brief This function handles TIM5 global interrupt.
  */
//void TIM5_IRQHandler(void)
//{
//  /* USER CODE BEGIN TIM5_IRQn 0 */

//  /* USER CODE END TIM5_IRQn 0 */
//  HAL_TIM_IRQHandler(&htim5);
//  /* USER CODE BEGIN TIM5_IRQn 1 */

//  /* USER CODE END TIM5_IRQn 1 */
//}

// - pwm section

// 2. the TIM1 is used by pwm.c
//    1. the variable, 
//    2. the MX_TIM1_Init(), HAL_TIM_Base_MspInit(), HAL_TIM_MspPostInit(), HAL_TIM_Base_MspDeInit -> [files main.c and stm32h7xx_hal_msp.c]
//    3. the TIM1_BRK_IRQHandler(), TIM1_UP_IRQHandler(), TIM1_TRG_COM_IRQHandler(), TIM1_CC_IRQHandler(), 

//TIM_HandleTypeDef htim1;

// and associated IRQ Handler, maybe in extern C ...


// - adcm section

// 3. the ADC1 and ADC2 are used by adcm.c
//    1. the variables, 
//    2. the MX_ADC1_Init() and MX_ADC2_Init(), HAL_ADC_MspInit(), HAL_ADC_MspDeInit() -> [files main.c and stm32h7xx_hal_msp.c]
//    3. the ADC_IRQHandler() 

//ADC_HandleTypeDef hadc1;
//ADC_HandleTypeDef hadc2;

// - hall section

// 4. the TIM4 is used by hall.c
//    1. the variables, 
//    2. the MX_TIM4_Init(), HAL_TIM_Base_MspInit(), HAL_TIM_Base_MspDeInit() -> [files main.c and stm32h7xx_hal_msp.c]
//    3. the ADC_IRQHandler() 

//TIM_HandleTypeDef htim4;



#if defined(MOTORHALCONFIG_DONTUSE_FLASH)

#if defined(WIP_removedcode_MainConf)
#else
MainConfTypeDef MainConf = {0};
#endif 

#else
#endif // MOTORHALCONFIG_DONTUSE_FLASH


#if defined(MOTORHALCONFIG_REDEFINE_RTOS)

void taskENTER_CRITICAL(void)  {}
void taskEXIT_CRITICAL(void) {}
void vTaskDelay(uint32_t ms)
{
    HAL_Delay(ms);
}

void taskDISABLE_INTERRUPTS(void) {}
void taskENABLE_INTERRUPTS(void) {}

#endif

#if defined(MOTORHALCONFIG_DONTUSE_LED) 

void ledSet(uint32_t led, uint32_t v) {}

    
void LedOnOff(uint32_t led, uint32_t v) {}
#endif


#if defined(MOTORHALCONFIG_ADD_IRQHANDLERS)

#include "stm32hal.h"
    
    #warning TODO-motorhal: add in here all the required IRQ Handlers

///**
//  * @brief This function handles DMA1 channel1 global interrupt.
//  */
//void DMA1_Channel1_IRQHandler(void)
//{
//  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

//  /* USER CODE END DMA1_Channel1_IRQn 0 */
//  HAL_DMA_IRQHandler(&hdma_adc1);
//  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

//  /* USER CODE END DMA1_Channel1_IRQn 1 */
//}

///**
//  * @brief This function handles DMA1 channel2 global interrupt.
//  */
//void DMA1_Channel2_IRQHandler(void)
//{
//  /* USER CODE BEGIN DMA1_Channel2_IRQn 0 */

//  /* USER CODE END DMA1_Channel2_IRQn 0 */
//  HAL_DMA_IRQHandler(&hdma_adc2);
//  /* USER CODE BEGIN DMA1_Channel2_IRQn 1 */

//  /* USER CODE END DMA1_Channel2_IRQn 1 */
//}

///**
//  * @brief This function handles ADC1 and ADC2 global interrupt.
//  */
//void ADC1_2_IRQHandler(void)
//{
//  /* USER CODE BEGIN ADC1_2_IRQn 0 */

//  /* USER CODE END ADC1_2_IRQn 0 */
//  HAL_ADC_IRQHandler(&hadc1);
//  HAL_ADC_IRQHandler(&hadc2);
//  /* USER CODE BEGIN ADC1_2_IRQn 1 */

//  /* USER CODE END ADC1_2_IRQn 1 */
//}

/////**
////  * @brief This function handles USB low priority interrupt remap.
////  */
////void USB_LP_IRQHandler(void)
////{
////  /* USER CODE BEGIN USB_LP_IRQn 0 */

////  /* USER CODE END USB_LP_IRQn 0 */
////  HAL_PCD_IRQHandler(&hpcd_USB_FS);
////  /* USER CODE BEGIN USB_LP_IRQn 1 */

////  /* USER CODE END USB_LP_IRQn 1 */
////}

///**
//  * @brief This function handles TIM1 break interrupt and TIM15 global interrupt.
//  */
//void TIM1_BRK_TIM15_IRQHandler(void)
//{
//  /* USER CODE BEGIN TIM1_BRK_TIM15_IRQn 0 */

//  /* USER CODE END TIM1_BRK_TIM15_IRQn 0 */
//  HAL_TIM_IRQHandler(&htim1);
//  /* USER CODE BEGIN TIM1_BRK_TIM15_IRQn 1 */

//  /* USER CODE END TIM1_BRK_TIM15_IRQn 1 */
//}

///**
//  * @brief This function handles TIM1 update interrupt and TIM16 global interrupt.
//  */
//void TIM1_UP_TIM16_IRQHandler(void)
//{
//  /* USER CODE BEGIN TIM1_UP_TIM16_IRQn 0 */

//  /* USER CODE END TIM1_UP_TIM16_IRQn 0 */
//  HAL_TIM_IRQHandler(&htim1);
//  /* USER CODE BEGIN TIM1_UP_TIM16_IRQn 1 */

//  /* USER CODE END TIM1_UP_TIM16_IRQn 1 */
//}

///**
//  * @brief This function handles TIM1 trigger and commutation interrupts and TIM17 global interrupt.
//  */
//void TIM1_TRG_COM_TIM17_IRQHandler(void)
//{
//  /* USER CODE BEGIN TIM1_TRG_COM_TIM17_IRQn 0 */

//  /* USER CODE END TIM1_TRG_COM_TIM17_IRQn 0 */
//  HAL_TIM_IRQHandler(&htim1);
//  /* USER CODE BEGIN TIM1_TRG_COM_TIM17_IRQn 1 */

//  /* USER CODE END TIM1_TRG_COM_TIM17_IRQn 1 */
//}

///**
//  * @brief This function handles TIM1 capture compare interrupt.
//  */
//void TIM1_CC_IRQHandler(void)
//{
//  /* USER CODE BEGIN TIM1_CC_IRQn 0 */

//  /* USER CODE END TIM1_CC_IRQn 0 */
//  HAL_TIM_IRQHandler(&htim1);
//  /* USER CODE BEGIN TIM1_CC_IRQn 1 */

//  /* USER CODE END TIM1_CC_IRQn 1 */
//}

///**
//  * @brief This function handles TIM2 global interrupt.
//  */
//void TIM2_IRQHandler(void)
//{
//  /* USER CODE BEGIN TIM2_IRQn 0 */

//  /* USER CODE END TIM2_IRQn 0 */
//  HAL_TIM_IRQHandler(&htim2);
//  /* USER CODE BEGIN TIM2_IRQn 1 */

//  /* USER CODE END TIM2_IRQn 1 */
//}

///**
//  * @brief This function handles TIM3 global interrupt.
//  */
//void TIM3_IRQHandler(void)
//{
//  /* USER CODE BEGIN TIM3_IRQn 0 */

//  /* USER CODE END TIM3_IRQn 0 */
//  HAL_TIM_IRQHandler(&htim3);
//  /* USER CODE BEGIN TIM3_IRQn 1 */

//  /* USER CODE END TIM3_IRQn 1 */
//}

//// marco.accame: removed because managed by the embot::hw::bsp
///**
//  * @brief This function handles EXTI line[15:10] interrupts.
//  */
////void EXTI15_10_IRQHandler(void)
////{
////  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

////  /* USER CODE END EXTI15_10_IRQn 0 */
////  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
////  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
////  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

////  /* USER CODE END EXTI15_10_IRQn 1 */
////}

/////**
////  * @brief This function handles I2C4 event interrupt / I2C4 wake-up interrupt through EXTI line 42.
////  */
////void I2C4_EV_IRQHandler(void)
////{
////  /* USER CODE BEGIN I2C4_EV_IRQn 0 */

////  /* USER CODE END I2C4_EV_IRQn 0 */
////  HAL_I2C_EV_IRQHandler(&hi2c4);
////  /* USER CODE BEGIN I2C4_EV_IRQn 1 */

////  /* USER CODE END I2C4_EV_IRQn 1 */
////}

/////**
////  * @brief This function handles I2C4 error interrupt.
////  */
////void I2C4_ER_IRQHandler(void)
////{
////  /* USER CODE BEGIN I2C4_ER_IRQn 0 */

////  /* USER CODE END I2C4_ER_IRQn 0 */
////  HAL_I2C_ER_IRQHandler(&hi2c4);
////  /* USER CODE BEGIN I2C4_ER_IRQn 1 */

////  /* USER CODE END I2C4_ER_IRQn 1 */
////}


#endif // #if defined(MOTORHALCONFIG_ADD_IRQHANDLERS)

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
