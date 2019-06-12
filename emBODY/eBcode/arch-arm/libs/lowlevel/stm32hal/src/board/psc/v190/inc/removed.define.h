/****************************************************
*  3SG board - iCub Facility - 2019                 *
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

#define LED_ON          HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
#define LED_OFF         HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

#define MPU9250_AD0_0   HAL_GPIO_WritePin(MPU9250_AD0_GPIO_Port, MPU9250_AD0_Pin, GPIO_PIN_RESET);
#define MPU9250_AD0_1   HAL_GPIO_WritePin(MPU9250_AD0_GPIO_Port, MPU9250_AD0_Pin, GPIO_PIN_SET);


#endif
