
/*
 * Copyright (C) 2026 Mesh - Istituto Italiano di Tecnologia
 * Author:  Kevin Sangalli
 * email:   kevin.sangalli@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_GPIO_BSP_AMCMJ1_H_
#define __EMBOT_HW_GPIO_BSP_AMCMJ1_H_

//CubeMX PIN defines
#define TRACECLK_Pin GPIO_PIN_2
#define TRACECLK_GPIO_Port GPIOE
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define ETH_SCLK_Pin GPIO_PIN_3
#define ETH_SCLK_GPIO_Port GPIOD
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define PWR_nFAIL_Pin GPIO_PIN_10
#define PWR_nFAIL_GPIO_Port GPIOA
#define PWR_nFAIL_EXTI_IRQn EXTI15_10_IRQn
#define EE_MOSI_Pin GPIO_PIN_6
#define EE_MOSI_GPIO_Port GPIOD
#define JTDI_Pin GPIO_PIN_15
#define JTDI_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define RTC_XTAL1_Pin GPIO_PIN_14
#define RTC_XTAL1_GPIO_Port GPIOC
#define TRACED0_Pin GPIO_PIN_3
#define TRACED0_GPIO_Port GPIOE
#define IMU_SDA_Pin GPIO_PIN_7
#define IMU_SDA_GPIO_Port GPIOB
#define ETH_nRST_Pin GPIO_PIN_15
#define ETH_nRST_GPIO_Port GPIOG
#define nLED2_Pin GPIO_PIN_5
#define nLED2_GPIO_Port GPIOD
#define CAN1_TX_Pin GPIO_PIN_1
#define CAN1_TX_GPIO_Port GPIOD
#define CAN1_RX_Pin GPIO_PIN_0
#define CAN1_RX_GPIO_Port GPIOD
#define EE_SCLK_Pin GPIO_PIN_10
#define EE_SCLK_GPIO_Port GPIOC
#define RTC_XTAL2_Pin GPIO_PIN_15
#define RTC_XTAL2_GPIO_Port GPIOC
#define TRACED1_Pin GPIO_PIN_4
#define TRACED1_GPIO_Port GPIOE
#define IMU_SCL_Pin GPIO_PIN_8
#define IMU_SCL_GPIO_Port GPIOB
#define ETH_nSEL_Pin GPIO_PIN_9
#define ETH_nSEL_GPIO_Port GPIOB
#define nLED1_Pin GPIO_PIN_4
#define nLED1_GPIO_Port GPIOD
#define TRACED3_Pin GPIO_PIN_12
#define TRACED3_GPIO_Port GPIOC
#define TP1_MCO_Pin GPIO_PIN_8
#define TP1_MCO_GPIO_Port GPIOA
#define CAN1_STBY_Pin GPIO_PIN_9
#define CAN1_STBY_GPIO_Port GPIOA
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define PWR_VAUXEN_Pin GPIO_PIN_13
#define PWR_VAUXEN_GPIO_Port GPIOC
#define TRACED2_Pin GPIO_PIN_5
#define TRACED2_GPIO_Port GPIOE
#define ETH_TXD1_Pin GPIO_PIN_14
#define ETH_TXD1_GPIO_Port GPIOG
#define EE_MISO_Pin GPIO_PIN_11
#define EE_MISO_GPIO_Port GPIOC
#define ETH_TXD0_Pin GPIO_PIN_13
#define ETH_TXD0_GPIO_Port GPIOG
#define ETH_nPME_Pin GPIO_PIN_7
#define ETH_nPME_GPIO_Port GPIOG
#define ETH_nPME_EXTI_IRQn EXTI9_5_IRQn
#define PWR_VAUXOK_Pin GPIO_PIN_4
#define PWR_VAUXOK_GPIO_Port GPIOF
#define PWR_VAUXOK_EXTI_IRQn EXTI4_IRQn
#define IMU_INT2_Pin GPIO_PIN_5
#define IMU_INT2_GPIO_Port GPIOG
#define IMU_INT2_EXTI_IRQn EXTI9_5_IRQn
#define ETH_nIRQ_Pin GPIO_PIN_6
#define ETH_nIRQ_GPIO_Port GPIOG
#define ETH_nIRQ_EXTI_IRQn EXTI9_5_IRQn
#define TP2_Pin GPIO_PIN_1
#define TP2_GPIO_Port GPIOB
#define VER1_Pin GPIO_PIN_14
#define VER1_GPIO_Port GPIOD
#define VER2_Pin GPIO_PIN_15
#define VER2_GPIO_Port GPIOD
#define OSC_IN_Pin GPIO_PIN_0
#define OSC_IN_GPIO_Port GPIOH
#define ETH_MDC_Pin GPIO_PIN_1
#define ETH_MDC_GPIO_Port GPIOC
#define TP3_Pin GPIO_PIN_0
#define TP3_GPIO_Port GPIOC
#define IMU_INT1_Pin GPIO_PIN_1
#define IMU_INT1_GPIO_Port GPIOG
#define IMU_INT1_EXTI_IRQn EXTI1_IRQn
#define VER0_Pin GPIO_PIN_13
#define VER0_GPIO_Port GPIOD
#define ETH_MISO_Pin GPIO_PIN_2
#define ETH_MISO_GPIO_Port GPIOC
#define ETH_MOSI_Pin GPIO_PIN_3
#define ETH_MOSI_GPIO_Port GPIOC
#define ETH_CRSDV_Pin GPIO_PIN_7
#define ETH_CRSDV_GPIO_Port GPIOA
#define ETH_RXD1_Pin GPIO_PIN_5
#define ETH_RXD1_GPIO_Port GPIOC
#define EE_WP_Pin GPIO_PIN_15
#define EE_WP_GPIO_Port GPIOE
#define ETH_REFCLK_Pin GPIO_PIN_1
#define ETH_REFCLK_GPIO_Port GPIOA
#define ETH_RXD0_Pin GPIO_PIN_4
#define ETH_RXD0_GPIO_Port GPIOC
#define CAN_SHDN_Pin GPIO_PIN_8
#define CAN_SHDN_GPIO_Port GPIOE
#define ETH_MDIO_Pin GPIO_PIN_2
#define ETH_MDIO_GPIO_Port GPIOA
#define TP4_Pin GPIO_PIN_15
#define TP4_GPIO_Port GPIOF
#define nLED_RGB_G_Pin GPIO_PIN_9
#define nLED_RGB_G_GPIO_Port GPIOD
#define EE_nSEL_Pin GPIO_PIN_4
#define EE_nSEL_GPIO_Port GPIOA
#define ETH_TXEN_Pin GPIO_PIN_11
#define ETH_TXEN_GPIO_Port GPIOB
#define nLED_RGB_R_Pin GPIO_PIN_8
#define nLED_RGB_R_GPIO_Port GPIOD
#define nLED_RGB_B_Pin GPIO_PIN_10
#define nLED_RGB_B_GPIO_Port GPIOD
    


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



