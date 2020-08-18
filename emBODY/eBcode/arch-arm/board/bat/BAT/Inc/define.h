 /****************************************************
 *  BAT board - iCub Facility - 2015                 *
 *  Define configuration                             *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/
 
#ifndef __DEFINE_H
#define __DEFINE_H


// ----------------------------------------------------------------------------
// Define
// ----------------------------------------------------------------------------
enum{   START,
        READY,
        ON_BOARD,
        ON_MOTORS,
        ON_BOARD_MOTORS,
        OFF_BOARD,
        OFF_MOTORS,
        OFF_BOARD_MOTORS,
        FAULT,
        FAULT_BOARD,
        FAULT_MOTORS,
        FAULT_BOARD_MOTORS
};

#define VINPUT_voltage		GPIOA, GPIO_PIN_0
#define EXTPS_voltage			GPIOA, GPIO_PIN_1
#define V48motor_PG				GPIOA, GPIO_PIN_2
#define V48motor_FLT			GPIOA, GPIO_PIN_3
#define SPI1_NSEL					GPIOA, GPIO_PIN_4
#define SPI1_SCLK					GPIOA, GPIO_PIN_5
#define SPI1_MISO					GPIOA, GPIO_PIN_6
#define SPI1_MOSI					GPIOA, GPIO_PIN_7
#define RST_DSPL					GPIOA, GPIO_PIN_8
#define TX_DSPL						GPIOA, GPIO_PIN_9
#define RX_DSPL						GPIOA, GPIO_PIN_10
#define I2C1_WP						GPIOA, GPIO_PIN_12

#define GPIO1							GPIOB, GPIO_PIN_0
#define GPIO2							GPIOB, GPIO_PIN_1
#define CAN1_SHDN					GPIOB, GPIO_PIN_5
#define I2C1_SCL					GPIOB, GPIO_PIN_6
#define I2C1_SDA					GPIOB, GPIO_PIN_7
#define CAN1_RX						GPIOB, GPIO_PIN_8
#define CAN1_TX						GPIOB, GPIO_PIN_9
#define I2C2_SCL					GPIOB, GPIO_PIN_10
#define I2C2_SDA					GPIOB, GPIO_PIN_11
#define I2C2_SMBAL				GPIOB, GPIO_PIN_12
#define LED_BT						GPIOB, GPIO_PIN_13
#define CAN1_LED					GPIOB, GPIO_PIN_14

#define BATTERY_voltage		GPIOC, GPIO_PIN_0
#define V12board_voltage	GPIOC, GPIO_PIN_1
#define V12motor_voltage	GPIOC, GPIO_PIN_2
#define V12board_current	GPIOC, GPIO_PIN_3
#define V12motor_current	GPIOC, GPIO_PIN_4
#define HSM_current				GPIOC, GPIO_PIN_5
#define FAULT_ICUB				GPIOC, GPIO_PIN_6
#define FAULT_REM					GPIOC, GPIO_PIN_7
#define FAULT_GPIO				GPIOC, GPIO_PIN_8
#define JACK							GPIOC, GPIO_PIN_9
#define PC12							GPIOC, GPIO_PIN_12
#define PC13							GPIOC, GPIO_PIN_13
#define PC14							GPIOC, GPIO_PIN_14
#define PC15							GPIOC, GPIO_PIN_15

#define BT_MLDP						GPIOD, GPIO_PIN_0
#define BT_HW_WAKE				GPIOD, GPIO_PIN_1
#define BT_SW_WAKE				GPIOD, GPIO_PIN_2
#define BT_CTS						GPIOD, GPIO_PIN_3
#define BT_RTS						GPIOD, GPIO_PIN_4
#define BT_TX							GPIOD, GPIO_PIN_5
#define BT_RX							GPIOD, GPIO_PIN_6
#define V5_EN							GPIOD, GPIO_PIN_7
#define HSM_EN						GPIOD, GPIO_PIN_8
#define V12board_EN				GPIOD, GPIO_PIN_9
#define V12motor_EN				GPIOD, GPIO_PIN_10
#define LED0_BATTERY			GPIOD, GPIO_PIN_11
#define LED1_BATTERY			GPIOD, GPIO_PIN_12
#define LED2_BATTERY			GPIOD, GPIO_PIN_13
#define LED3_BATTERY			GPIOD, GPIO_PIN_14
#define LED4_BATTERY			GPIOD, GPIO_PIN_15

#define CPU								GPIOE, GPIO_PIN_7
#define CPU_Green					GPIOE, GPIO_PIN_8
#define CPU_Red						GPIOE, GPIO_PIN_9
#define MOTORS						GPIOE, GPIO_PIN_10
#define MOTORS_Green			GPIOE, GPIO_PIN_11
#define MOTORS_Red				GPIOE, GPIO_PIN_12
#define HSM_nFLT					GPIOE, GPIO_PIN_13
#define HSM_nPG						GPIOE, GPIO_PIN_14

#define EN_V12motor_ON		HAL_GPIO_WritePin(V12motor_EN, GPIO_PIN_SET);
#define EN_V12motor_OFF		HAL_GPIO_WritePin(V12motor_EN, GPIO_PIN_RESET);

#define EN_V12board_ON		HAL_GPIO_WritePin(V12board_EN, GPIO_PIN_SET);
#define EN_V12board_OFF		HAL_GPIO_WritePin(V12board_EN, GPIO_PIN_RESET);

#define HSM_EN_ON         HAL_GPIO_WritePin(HSM_EN, GPIO_PIN_SET);
#define HSM_EN_OFF        HAL_GPIO_WritePin(HSM_EN, GPIO_PIN_RESET);

#define V48motor_PG_ON		HAL_GPIO_WritePin(V48motor_PG, GPIO_PIN_RESET);
#define V48motor_PG_OFF		HAL_GPIO_WritePin(V48motor_PG, GPIO_PIN_SET);

#define V48motor_FLT_ON		HAL_GPIO_WritePin(V48motor_FLT, GPIO_PIN_RESET);
#define V48motor_FLT_OFF	HAL_GPIO_WritePin(V48motor_FLT, GPIO_PIN_SET);

#define PB1_LED_GREEN     HAL_GPIO_WritePin(CPU_Green, GPIO_PIN_SET);         HAL_GPIO_WritePin(CPU_Red, GPIO_PIN_RESET);
#define PB1_LED_RED       HAL_GPIO_WritePin(CPU_Green, GPIO_PIN_RESET);       HAL_GPIO_WritePin(CPU_Red, GPIO_PIN_SET);
#define PB1_LED_OFF       HAL_GPIO_WritePin(CPU_Green, GPIO_PIN_RESET);       HAL_GPIO_WritePin(CPU_Red, GPIO_PIN_RESET);

#define PB2_LED_GREEN     HAL_GPIO_WritePin(MOTORS_Green, GPIO_PIN_SET);      HAL_GPIO_WritePin(MOTORS_Red, GPIO_PIN_RESET);
#define PB2_LED_RED       HAL_GPIO_WritePin(MOTORS_Green, GPIO_PIN_RESET);    HAL_GPIO_WritePin(MOTORS_Red, GPIO_PIN_SET);
#define PB2_LED_OFF       HAL_GPIO_WritePin(MOTORS_Green, GPIO_PIN_RESET);    HAL_GPIO_WritePin(MOTORS_Red, GPIO_PIN_RESET);

#define FAULT_REM_ON      HAL_GPIO_WritePin(FAULT_REM, GPIO_PIN_SET);
#define FAULT_REM_OFF     HAL_GPIO_WritePin(FAULT_REM, GPIO_PIN_RESET);

#define RST_DSPL_ON       HAL_GPIO_WritePin(RST_DSPL, GPIO_PIN_SET);
#define RST_DSPL_OFF      HAL_GPIO_WritePin(RST_DSPL, GPIO_PIN_RESET);

#define V5_EN_ON          HAL_GPIO_WritePin(V5_EN, GPIO_PIN_SET);
#define V5_EN_OFF         HAL_GPIO_WritePin(V5_EN, GPIO_PIN_RESET);

#define LED_BT_ON         HAL_GPIO_WritePin(LED_BT, GPIO_PIN_RESET);
#define LED_BT_OFF        HAL_GPIO_WritePin(LED_BT, GPIO_PIN_SET);

#endif
