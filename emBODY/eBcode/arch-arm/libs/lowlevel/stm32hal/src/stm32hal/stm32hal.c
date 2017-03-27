/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
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


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "string.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "stm32hal.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint32_t s_stm32hal_bps_dummy_tick1msget(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static stm32hal_config_t s_stm32hal_bsp_config =
{
    .tick1ms_get = s_stm32hal_bps_dummy_tick1msget,
    .initbsp = true
};

static  stm32hal_can_configCallback_t s_stm32hal_can_CfgCallback = 
{
    .onRx = NULL,
    .onTx = NULL,
    .onError = NULL
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

// - defined in the bsp_xxxxx.c file. i am lazy and i dont want to create a .h file
extern void stm32hal_bsp_init(void);

extern stm32hal_res_t stm32hal_init(const stm32hal_config_t *cfg)
{
    static const stm32hal_config_t s_config = 
    {
        .tick1ms_get = s_stm32hal_bps_dummy_tick1msget,
        .initbsp = true
    };
    
    if(NULL == cfg)
    {
        cfg = &s_config;
    }
    
    memmove(&s_stm32hal_bsp_config, cfg, sizeof(s_stm32hal_bsp_config));
    
    if(NULL == s_stm32hal_bsp_config.tick1ms_get)
    {
        s_stm32hal_bsp_config.tick1ms_get = s_stm32hal_bps_dummy_tick1msget;
    }
    
    if(false == s_stm32hal_bsp_config.initbsp)
    {
        return stm32hal_res_OK;
    }
    
    // defined in the specific board-xxxx section
    stm32hal_bsp_init();    
    
    return stm32hal_res_OK;
}


extern stm32hal_res_t stm32hal_can_configureIRQcallback(const stm32hal_can_configCallback_t *cfgCallback)
{
    s_stm32hal_can_CfgCallback.onRx = cfgCallback->onRx;
    s_stm32hal_can_CfgCallback.onTx = cfgCallback->onTx;
    s_stm32hal_can_CfgCallback.onError = cfgCallback->onError;
    
    return stm32hal_res_OK;
}






void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* hcan)
{
    if(NULL != s_stm32hal_can_CfgCallback.onTx)
        s_stm32hal_can_CfgCallback.onTx(hcan);
}

/**
  * @brief  Reception complete callback in non-blocking mode.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
    if(NULL != s_stm32hal_can_CfgCallback.onRx)
        s_stm32hal_can_CfgCallback.onRx(hcan);
}

/**
  * @brief  Error CAN callback.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
    if(NULL != s_stm32hal_can_CfgCallback.onError)
    s_stm32hal_can_CfgCallback.onError(hcan);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static uint32_t s_stm32hal_bps_dummy_tick1msget(void)
{
    static uint32_t cnt =0;
    return cnt++;
}



// --------------------------------------------------------------------------------------------------------------------
// - redefinition of functions required by stm32 and weakly defined
// --------------------------------------------------------------------------------------------------------------------



// redefinition of the HAL_* function which we want to behave differently.

// this section keeps, so far, only tick & delay functions.
// cube-mx redefines those funtions to start the systick timer at 1 ms which increments a counter.
// however, we dont want the systick behaving like that and we have decide to give the tick functionality
// as the user wants w/ externally defined funtions.

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
//  /*Configure the SysTick to have interrupt in 1ms time basis*/
//  HAL_SYSTICK_Config(SystemCoreClock/1000);
//
//  /*Configure the SysTick IRQ priority */
//  HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority ,0);
//
//  /* Return function status */
  return HAL_OK;
}

void HAL_IncTick(void)
{
//  uwTick++;
}

uint32_t HAL_GetTick(void)
{
//  return uwTick;
    
//    osal_abstime_t t = osal_system_ticks_abstime_get() / 1000; // now t is expressed in millisec
//    return (uint32_t)t;
    
    if(NULL != s_stm32hal_bsp_config.tick1ms_get)
    {
        return s_stm32hal_bsp_config.tick1ms_get();
    }
    
    return 0;
}


void HAL_Delay(uint32_t Delay)
{
  uint32_t tickstart = 0;
  tickstart = HAL_GetTick();
  while((HAL_GetTick() - tickstart) < Delay)
  {
  }
}


void HAL_SuspendTick(void)
{
 // /* Disable SysTick Interrupt */
//  SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}


void HAL_ResumeTick(void)
{
//  /* Enable SysTick Interrupt */
//  SysTick->CTRL  |= SysTick_CTRL_TICKINT_Msk;
}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



