/*
 * Copyright (C) 2021 iCub Tech Facility - Istituto Italiano di Tecnologia
 * Author:  Ugo Pattacini
 * email:   ugo.pattacini@iit.it
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

#ifndef RTW_MUTEX_H
#define RTW_MUTEX_H

#include "stm32hal.h"

#define rtw_mutex_init()
#define rtw_mutex_destroy()

inline bool IsException(void) 
{
    return(__get_IPSR() != 0U);
}

inline void rtw_mutex_lock(void)
{    
    if(false == IsException())
    {
    #ifdef STM32HAL_BOARD_AMCBLDC
        // e' corretto DMA1_Channel2_IRQn perche':
        // DMA1_Channel2_IRQHandler() chiama HAL_DMA_IRQHandler(&hdma_adc2);
        // e hdma_adc2 e' associato a adc2 che gestisce acquisizioni delle correnti del motore        
        NVIC_DisableIRQ(DMA1_Channel2_IRQn);
    #endif    
    #ifdef STM32HAL_BOARD_AMC2C
        // dovrebbe essere DMA2_Stream0_IRQn
        // perche' si usa il DMA2_Stream0_IRQHandler() che chiama HAL_DMA_IRQHandler(&embot::hw::motor::bsp::amc2c::hdma_adc1); 
        // e hdma_adc1 e' associato a hADC1 (alias hadc1) che acquisisce le correnti del motore
        NVIC_DisableIRQ(DMA2_Stream0_IRQn);
        //NVIC_DisableIRQ(BDMA_Channel2_IRQn);
    #endif
    #if defined(STM32HAL_BOARD_AMCFOC_1CM7) || defined(STM32HAL_BOARD_AMCFOC_2CM4)
        // blocca la DMA2_Stream0_IRQHandler() che chiama HAL_DMA_IRQHandler(&embot::hw::motor::bldc::bsp::amcfoc::hdma_adc1);
        // dove hdma_adc1 e' associato a hadc1 che acquisisce correnti per il motore 2
        NVIC_DisableIRQ(DMA2_Stream0_IRQn); // motor 2. 
        
        // blocca la DMA2_Stream1_IRQHandler() che chiama  HAL_DMA_IRQHandler(&embot::hw::motor::bldc::bsp::amcfoc::hdma_adc2);
        // dove hdma_adc2 e' associato a hadc2 che acquisisce correnti per il motore 1
        NVIC_DisableIRQ(DMA2_Stream1_IRQn); // motor 1
    #endif        
        
    }  
}

inline void rtw_mutex_unlock(void)
{
    if(false == IsException())
    {
    #ifdef STM32HAL_BOARD_AMCBLDC
        NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    #endif
    #ifdef STM32HAL_BOARD_AMC2C
        NVIC_EnableIRQ(DMA2_Stream0_IRQn);
        // NVIC_EnableIRQ(BDMA_Channel2_IRQn);
    #endif        
    #if defined(STM32HAL_BOARD_AMCFOC_1CM7) || defined(STM32HAL_BOARD_AMCFOC_2CM4)
        NVIC_EnableIRQ(DMA2_Stream0_IRQn); // motor 2
        NVIC_EnableIRQ(DMA2_Stream1_IRQn); // motor 1
    #endif         
    }
}

#endif // RTW_MUTEX_H

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
