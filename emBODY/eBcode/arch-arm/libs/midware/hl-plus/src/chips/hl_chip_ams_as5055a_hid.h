/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _HL_CHIP_AMS_AS5055A_HID_H_
#define _HL_CHIP_AMS_AS5055A_HID_H_


/* @file       hl_chip_ams_as5055a_hid.h
    @brief      This header file keeps hidden interface to a ST L3G4200D gyroscope
    @author     marco.accame@iit.it
    @date       10/29/2013
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"



// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hl_chip_ams_as5055a.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------

#if defined(TEST_AMS_AS5055A_AS_MAGGIALI)

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

  // m.a: SPI2_MISO is PC2
  
  #define SPI2_MISO_PIN                  GPIO_Pin_2
  #define SPI2_MISO_GPIO_PORT            GPIOC
  #define SPI2_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOC
  #define SPI2_MISO_SOURCE               GPIO_PinSource2
  #define SPI2_MISO_AF                   GPIO_AF_SPI2
  
  // m.a: SPI2_MOSI is PCI3 on schematics, thus i impose it. i remove any sign of PC13
  
  //m.a: #define SPI2_MOSI_PIN                  GPIO_Pin_13
  #define SPI2_MOSI_PIN                  GPIO_Pin_3
  #define SPI2_MOSI_GPIO_PORT            GPIOI
  //m.a: #define SPI2_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOC
  #define SPI2_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOI
  //m.a: #define SPI2_MOSI_SOURCE               GPIO_PinSource13
  #define SPI2_MOSI_SOURCE               GPIO_PinSource3
  #define SPI2_MOSI_AF                   GPIO_AF_SPI2

   // m.a: SPI2_NSEL is PI0
   
  #define SPI2_NSEL_PIN                  GPIO_Pin_0
  #define SPI2_NSEL_GPIO_PORT            GPIOI
  #define SPI2_NSEL_GPIO_CLK             RCC_AHB1Periph_GPIOI

  #define BSY_FLAG   0x0100

#endif


// - definition of the hidden struct implementing the object ----------------------------------------------------------
// empty-section


// - declaration of extern hidden variables ---------------------------------------------------------------------------
// empty-section

// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




