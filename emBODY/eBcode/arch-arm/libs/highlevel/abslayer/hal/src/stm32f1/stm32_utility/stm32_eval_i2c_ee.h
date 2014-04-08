/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/


/**
  ******************************************************************************
  * @file    stm32_eval_i2c_ee.h
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    03/01/2010
  * @brief   This file contains all the functions prototypes for the stm32_eval_i2c_ee
  *          firmware driver.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_EVAL_I2C_EE_H
#define __STM32_EVAL_I2C_EE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
//#include "stm32_eval.h"  IIT_EXT  [removed]
#include "stm32f1.h" //IIT_EXT  [added]
#include "hal_brdcfg.h" //[IIT_EXT  added]


/** @addtogroup Utilities
  * @{
  */
  
/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup Common
  * @{
  */
  
/** @addtogroup STM32_EVAL_I2C_EE
  * @{
  */  

/** @defgroup STM32_EVAL_I2C_EE_Exported_Types
  * @{
  */ 

/**
  * @}
  */
  
/** @defgroup STM32_EVAL_I2C_EE_Exported_Constants
  * @{
  */
//IIT added
#ifdef USE_EMS001_BOARD
    #define sEE_AT24C512B
#endif

//IIT note:
// the eeprom installed on ems001 board is AT24C512B; it beahaves like sEE_M24C64_32,
//so it is necessary to define sEE_M24C64_32 macro in order to obtain same behaviour of stm eeprom functions.


#if !defined (sEE_M24C08) && !defined (sEE_M24C64_32)
/* Use the defines below the choose the EEPROM type */
/*#define sEE_M24C08*/  /* Support the device: M24C08. */
/* note: Could support: M24C01, M24C02, M24C04 and M24C16 if the blocks and 
   HW address are correctly defined*/
#define sEE_M24C64_32  /* Support the devices: M24C32 and M24C64 */
#endif

#ifdef sEE_M24C64_32
/* For M24C32 and M24C64 devices, E0,E1 and E2 pins are all used for device 
  address selection (ne need for additional address lines). According to the 
  Harware connection on the board (on STM3210C-EVAL board E0 = E1 = E2 = 0) */

 #define sEE_HW_ADDRESS     0xA0   /* E0 = E1 = E2 = 0 */ 
 //IIT note
 //The address of eeprom for emss001 and mcbstm32c is the same

#elif defined (sEE_M24C08)
/* The M24C08W contains 4 blocks (128byte each) with the adresses below: E2 = 0 
   EEPROM Addresses defines */
 #define sEE_Block0_ADDRESS 0xA0   /* E2 = 0 */ 
 /*#define sEE_Block1_ADDRESS 0xA2*/ /* E2 = 0 */  
 /*#define sEE_Block2_ADDRESS 0xA4*/ /* E2 = 0 */
 /*#define sEE_Block3_ADDRESS 0xA6*/ /* E2 = 0 */
#endif /* sEE_M24C64_32*/

#define I2C_SPEED              400000
#define I2C_SLAVE_ADDRESS7     0xA0

#if defined (sEE_M24C08)
 #define sEE_PAGESIZE    16
#elif defined (sEE_M24C64_32) && !defined (sEE_AT24C512B) //IIT modified
 #define sEE_PAGESIZE    32
#elif defined (sEE_AT24C512B)
  #define sEE_PAGESIZE    128
#endif

// IIT: taken from driver V4.5.0   
/* Maximum Timeout values for flags and events waiting loops. These timeouts are
   not based on accurate values, they just guarantee that the application will 
   not remain stuck if the I2C communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */   
#define sEE_FLAG_TIMEOUT         ((uint32_t)0x1000)
//#define sEE_FLAG_TIMEOUT         ((uint32_t)0x10000000)
#define sEE_LONG_TIMEOUT         ((uint32_t)(10 * sEE_FLAG_TIMEOUT))

/* Maximum number of trials for sEE_WaitEepromStandbyState() function */
#define sEE_MAX_TRIALS_NUMBER     1500
   
/* Defintions for the state of the DMA transfer */   
#define sEE_STATE_READY           0
#define sEE_STATE_BUSY            1
#define sEE_STATE_ERROR           2
   
#define sEE_OK                    0
#define sEE_FAIL                  1   

 
/**
  * @}
  */ 
  
/** @defgroup STM32_EVAL_I2C_EE_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM32_EVAL_I2C_EE_Exported_Functions
  * @{
  */ 
void sEE_DeInit(void);
void sEE_Init(void);
void sEE_WriteByte(uint8_t* pBuffer, uint16_t WriteAddr);
//IIT: changed the API as in V3.5 to give sEE_OK on success and sEE_FAIL on failure 
uint32_t sEE_WritePage(uint8_t* pBuffer, uint16_t WriteAddr, uint8_t NumByteToWrite);
uint32_t sEE_WriteBuffer(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite);
uint32_t sEE_ReadBuffer(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead);
uint32_t sEE_WaitEepromStandbyState(void);

//IIT added
void sEE_Init_without_i2c(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32_EVAL_I2C_EE_H */
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

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/


