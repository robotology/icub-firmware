/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Maggiali
 * email:   marco.maggiali@iit.it
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

#include "hal_sys.h"
#include "as5048.h"
#include "hl_core.h"
#include "hal_gpio.h"
#include "hal_brdcfg.h"

/* @file       as5048.c
	@brief      This file implements the reading of the as5048 sensor (Absolute Magnetic sensor)
	@author     marco.maggiali@iit.it, 
    @date       26/03/2013
**/
  //#define SPI3                           SPI3
  #define SPI3_CLK                       RCC_APB1Periph_SPI3
  #define SPI3_CLK_INIT                  RCC_APB1PeriphClockCmd
  #define SPI3_IRQn                      SPI3_IRQn
  #define SPI3_IRQHANDLER                SPI3_IRQHandler

  #define SPI3_SCK_PIN                   GPIO_Pin_10
  #define SPI3_SCK_GPIO_PORT             GPIOC
  #define SPI3_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOC
  #define SPI3_SCK_SOURCE                GPIO_PinSource10
  #define SPI3_SCK_AF                    GPIO_AF_SPI3

  #define SPI3_MISO_PIN                  GPIO_Pin_11
  #define SPI3_MISO_GPIO_PORT            GPIOC
  #define SPI3_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOC
  #define SPI3_MISO_SOURCE               GPIO_PinSource11
  #define SPI3_MISO_AF                   GPIO_AF_SPI3

  #define SPI3_MOSI_PIN                  GPIO_Pin_12
  #define SPI3_MOSI_GPIO_PORT            GPIOC
  #define SPI3_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOC
  #define SPI3_MOSI_SOURCE               GPIO_PinSource12
  #define SPI3_MOSI_AF                  GPIO_AF_SPI3

  #define SPI3_NSEL_PIN                  GPIO_Pin_13
  #define SPI3_NSEL_GPIO_PORT            GPIOC
  #define SPI3_NSEL_GPIO_CLK             RCC_AHB1Periph_GPIOC

  //#define SPI2                           SPI2
  #define SPI2_CLK                       RCC_APB1Periph_SPI2
  #define SPI2_CLK_INIT                  RCC_APB1PeriphClockCmd
  #define SPI2_IRQn                      SPI2_IRQn
  #define SPI2_IRQHANDLER                SPI2_IRQHandler

  #define SPI2_SCK_PIN                   GPIO_Pin_1
  #define SPI2_SCK_GPIO_PORT             GPIOI
  #define SPI2_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOI
  #define SPI2_SCK_SOURCE                GPIO_PinSource1
  #define SPI2_SCK_AF                    GPIO_AF_SPI2

  #define SPI2_MISO_PIN                  GPIO_Pin_2
  #define SPI2_MISO_GPIO_PORT            GPIOC
  #define SPI2_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOC
  #define SPI2_MISO_SOURCE               GPIO_PinSource2
  #define SPI2_MISO_AF                   GPIO_AF_SPI2

  #define SPI2_MOSI_PIN                  GPIO_Pin_3
  #define SPI2_MOSI_GPIO_PORT            GPIOI
  #define SPI2_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOI
  #define SPI2_MOSI_SOURCE               GPIO_PinSource3
  #define SPI2_MOSI_AF                   GPIO_AF_SPI2

  #define SPI2_NSEL_PIN                  GPIO_Pin_0
  #define SPI2_NSEL_GPIO_PORT            GPIOI
  #define SPI2_NSEL_GPIO_CLK             RCC_AHB1Periph_GPIOI

  #define BSY_FLAG   0x0100 

/*******************************************************************************
* Function Name  : as5048_init
* Description    : init of the as5048 sensor	 
*
* Input          : sensorID 0 and 1 
* Output         : None
* Return         : None
*******************************************************************************/
void as5048_init(uint8_t sensorID)
{
  
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
//  uint16_t aTxBuffer[3]={0xFFFF,0xFFFF,0xFFFF};
//  uint16_t aRxBuffer[3]={0x0,0x0,0x0};
//  uint16_t angle[3]={0x0,0x0,0x0};
//  uint16_t i=0;
  
  if (sensorID==0) //SPI3 
  {  
	  /* Peripheral Clock Enable -------------------------------------------------*/
	  /* Enable the SPI clock */
	  SPI3_CLK_INIT(SPI3_CLK, ENABLE);
	  
	  /* Enable GPIO clocks */
	  RCC_AHB1PeriphClockCmd(SPI3_SCK_GPIO_CLK | SPI3_MISO_GPIO_CLK | SPI3_MOSI_GPIO_CLK, ENABLE);
	
	  /* SPI GPIO Configuration --------------------------------------------------*/
	  /* GPIO Deinitialisation */
	
	  /* Connect SPI pins to AF5 */  
	  GPIO_PinAFConfig(SPI3_SCK_GPIO_PORT, SPI3_SCK_SOURCE, SPI3_SCK_AF);
	  GPIO_PinAFConfig(SPI3_MISO_GPIO_PORT, SPI3_MISO_SOURCE, SPI3_MISO_AF);    
	  GPIO_PinAFConfig(SPI3_MOSI_GPIO_PORT, SPI3_MOSI_SOURCE, SPI3_MOSI_AF);
	
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	
	  /* SPI SCK pin configuration */
	  GPIO_InitStructure.GPIO_Pin = SPI3_SCK_PIN;
	  GPIO_Init(SPI3_SCK_GPIO_PORT, &GPIO_InitStructure);
	  
	  /* SPI  MISO pin configuration */
	  GPIO_InitStructure.GPIO_Pin =  SPI3_MISO_PIN;
	  GPIO_Init(SPI3_MISO_GPIO_PORT, &GPIO_InitStructure);  
	
	  /* SPI  MOSI pin configuration */
	  GPIO_InitStructure.GPIO_Pin =  SPI3_MOSI_PIN;
	  GPIO_Init(SPI3_MOSI_GPIO_PORT, &GPIO_InitStructure);
	
	  /* SPI NSEL pin configuration */
	  GPIO_InitStructure.GPIO_Pin =  SPI3_NSEL_PIN;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	  GPIO_Init(SPI3_NSEL_GPIO_PORT, &GPIO_InitStructure);  
	
	  GPIO_SetBits(SPI3_NSEL_GPIO_PORT, SPI3_NSEL_PIN);	 
	
	  /* SPI configuration -------------------------------------------------------*/
	  SPI_I2S_DeInit(SPI3);
	  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	  SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
	  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	  SPI_InitStructure.SPI_CRCPolynomial = 7;
	  SPI_Init(SPI3, &SPI_InitStructure);
	
	
	  /* Configure the Priority Group to 1 bit */                
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	  
	  /* Configure the SPI interrupt priority */
	  NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	
	  //NVIC_Init(&NVIC_InitStructure);
	
	  SPI_Cmd(SPI3, ENABLE);
	  /* Enable the Rx buffer not empty interrupt */
	  //SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_RXNE, ENABLE);   
	  }
	  else
	  {
	   	  /* Peripheral Clock Enable -------------------------------------------------*/
	  /* Enable the SPI clock */
	  SPI2_CLK_INIT(SPI2_CLK, ENABLE);
	  
	  /* Enable GPIO clocks */
	  RCC_AHB1PeriphClockCmd(SPI2_SCK_GPIO_CLK | SPI2_MISO_GPIO_CLK | SPI2_MOSI_GPIO_CLK, ENABLE);
	
	  /* SPI GPIO Configuration --------------------------------------------------*/
	  /* GPIO Deinitialisation */
	
	  /* Connect SPI pins to AF5 */  
	  GPIO_PinAFConfig(SPI2_SCK_GPIO_PORT, SPI2_SCK_SOURCE, SPI2_SCK_AF);
	  GPIO_PinAFConfig(SPI2_MISO_GPIO_PORT, SPI2_MISO_SOURCE, SPI2_MISO_AF);    
	  GPIO_PinAFConfig(SPI2_MOSI_GPIO_PORT, SPI2_MOSI_SOURCE, SPI2_MOSI_AF);
	
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	
	  /* SPI SCK pin configuration */
	  GPIO_InitStructure.GPIO_Pin = SPI2_SCK_PIN;
	  GPIO_Init(SPI2_SCK_GPIO_PORT, &GPIO_InitStructure);
	  
	  /* SPI  MISO pin configuration */
	  GPIO_InitStructure.GPIO_Pin =  SPI2_MISO_PIN;
	  GPIO_Init(SPI2_MISO_GPIO_PORT, &GPIO_InitStructure);  
	
	  /* SPI  MOSI pin configuration */
	  GPIO_InitStructure.GPIO_Pin =  SPI2_MOSI_PIN;
	  GPIO_Init(SPI2_MOSI_GPIO_PORT, &GPIO_InitStructure);
	
	  /* SPI NSEL pin configuration */
	  GPIO_InitStructure.GPIO_Pin =  SPI2_NSEL_PIN;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	  GPIO_Init(SPI2_NSEL_GPIO_PORT, &GPIO_InitStructure);  
	
	  GPIO_SetBits(SPI2_NSEL_GPIO_PORT, SPI2_NSEL_PIN);	 
	
	  /* SPI configuration -------------------------------------------------------*/
	  SPI_I2S_DeInit(SPI2);
	  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	  SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
	  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	  SPI_InitStructure.SPI_CRCPolynomial = 7;
	  SPI_Init(SPI2, &SPI_InitStructure);
	
	
	  /* Configure the Priority Group to 1 bit */                
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	  
	  /* Configure the SPI interrupt priority */
	  NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	
	  // NVIC_Init(&NVIC_InitStructure);
	
	  SPI_Cmd(SPI2, ENABLE);
	  /* Enable the Rx buffer not empty interrupt */
	  //SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE); 
	  }
}

/*******************************************************************************
* Function Name  : as5048_read
* Description    : read the as5048 sensor	 
*
* Input          : sensorID 0 and 1 
* Output         : None
* Return         : the 14 bit value of the encoder 
*******************************************************************************/

uint16_t * as5048_read(uint8_t sensorID) 
 {
 	  uint16_t value[3]= {0x3FFF,0x3FFF,0x3FFF};

      if (sensorID==0)
	  {
	  GPIO_ResetBits(SPI3_NSEL_GPIO_PORT, SPI3_NSEL_PIN);
	  SPI_I2S_SendData(SPI3, 0xffff);   //send a dummy value 
	  while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE)==0);
	  value[0]=SPI_I2S_ReceiveData(SPI3)&0x3fff;  //remove the parity and errorflag bit
//	  while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY));
		
		GPIO_ResetBits(SPI3_NSEL_GPIO_PORT, SPI3_NSEL_PIN);
	  SPI_I2S_SendData(SPI3, 0xffff);   //send a dummy value 
	  while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE)==0);
	  value[1]=SPI_I2S_ReceiveData(SPI3)&0x3fff;  //remove the parity and errorflag bit
//	  while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY));

		GPIO_ResetBits(SPI3_NSEL_GPIO_PORT, SPI3_NSEL_PIN);
	  SPI_I2S_SendData(SPI3, 0xffff);   //send a dummy value 
	  while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE)==0);
	  value[2]=SPI_I2S_ReceiveData(SPI3)&0x3fff;  //remove the parity and errorflag bit
			
	  //angle[0] +=(aRxBuffer[0]&0x3fff);
	  while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY));
   	  GPIO_SetBits(SPI3_NSEL_GPIO_PORT, SPI3_NSEL_PIN);
	  }
	  else 
	  {
	  GPIO_ResetBits(SPI2_NSEL_GPIO_PORT, SPI2_NSEL_PIN);
	  SPI_I2S_SendData(SPI2, 0xffff);   //send a dummy value 
		while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE)==0);
	  value[0]=SPI_I2S_ReceiveData(SPI2)&0x3fff;  //remove the parity and errorflag bit
			 while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	  //angle[0] +=(aRxBuffer[0]&0x3fff);			
// test 3 sensors 
		SPI_I2S_SendData(SPI2, 0xffff);   //send a dummy value
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE)==0);
	  value[1]=SPI_I2S_ReceiveData(SPI2)&0x3fff;  //remove the parity and errorflag bit
			 while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	  //angle[0] +=(aRxBuffer[0]&0x3fff);			
		SPI_I2S_SendData(SPI2, 0xffff);   //send a dummy value 
	  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE)==0);
	  value[2]=SPI_I2S_ReceiveData(SPI2)&0x3fff;  //remove the parity and errorflag bit
	  //angle[0] +=(aRxBuffer[0]&0x3fff);
///			
			 
	  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	  GPIO_SetBits(SPI2_NSEL_GPIO_PORT, SPI2_NSEL_PIN);
	  } 
	  return value;
 }
