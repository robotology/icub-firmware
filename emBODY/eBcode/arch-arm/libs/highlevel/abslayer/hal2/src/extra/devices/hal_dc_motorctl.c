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


/* @file       hal_device_motorcl.c
	@brief      This file implements internal implementation of the hal led module.
	@author     marco.maggiali@iit.it, 
    @date       26/03/2013
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_DEVICE_MOTORCTL

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_gpio.h"
#include "hal_brdcfg.h"
//#include "hal_utility_bits.h" 
#include "hal_adc.h" 
#include "hl_core.h" 
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_dc_motorctl.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// emty-section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_device_motor_supported_is(hal_motor_t id);
static void s_hal_device_motor_initted_set(hal_motor_t id);
static hal_boolval_t s_hal_device_motor_initted_is(hal_motor_t id);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_hal_device_motor_initted = 0;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_motor_init(hal_motor_t id, const hal_pwm_cfg_t *cfg)
{
 /*   const hal_gpio_map_t *gm = NULL;		  */
    hal_result_t res = hal_res_NOK_generic;
	  TIM_TimeBaseInitTypeDef TIM1_TimeBaseStructure;
  	TIM_OCInitTypeDef TIM1_OCInitStructure;
  	TIM_BDTRInitTypeDef TIM1_BDTRInitStructure;
//  	NVIC_InitTypeDef NVIC_InitStructure;
  	GPIO_InitTypeDef GPIO_InitStructure;

/* INIT MOTOR 1 and 2 */

//	if (id<2)  
	{
	//initialization of TIM1 for controlling the first 2 motors
	/* TIM1 Peripheral Configuration -------------------------------------------*/
 	/* GPIOA  Clocks enable */
  	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);

	/* GPIOA Configuration: Channel 1, 2, 3 and 4 as alternate function push-pull */
  	
	  GPIO_StructInit(&GPIO_InitStructure);  
	
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	/*Timer1 alternate function full remapping*/  
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_TIM1);  //MOT1_PHA
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_TIM1);  //MOT1_PHB
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_TIM1); //MOT2_PHA
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_TIM1); //MOT2_PHB

	/* TIM1 clock enable */
	TIM_DeInit(TIM1);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);	
//	TIM_TimeBaseStructInit(&TIM1_TimeBaseStructure);
	/* Time Base configuration */
	TIM1_TimeBaseStructure.TIM_Prescaler = PWM_PRSC;
	TIM1_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;//TIM_CounterMode_Up;
	TIM1_TimeBaseStructure.TIM_Period = PWM_PERIOD/2; // in CenterAligned mode the couter counts twice half a pwm period 
	TIM1_TimeBaseStructure.TIM_ClockDivision = 0;// TIM_CKD_DIV2;
	TIM1_TimeBaseStructure.TIM_RepetitionCounter = 0;// REP_RATE;

	TIM_TimeBaseInit(TIM1, &TIM1_TimeBaseStructure);
	
//	TIM_OCStructInit(&TIM1_OCInitStructure);
	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
	TIM1_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //indipendent channels 
	TIM1_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM1_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;                  
	TIM1_OCInitStructure.TIM_Pulse = 0x000; //dummy value
	TIM1_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM1_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;         
	TIM1_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM1_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset; //LOW_SIDE_POLARITY;          
	
	//MOTOR1 PHASE A and B
	TIM_OC1Init(TIM1, &TIM1_OCInitStructure);
	TIM1_OCInitStructure.TIM_Pulse = (PWM_PERIOD>>0); //dummy value 
	TIM_OC1Init(TIM1, &TIM1_OCInitStructure);
	TIM1_OCInitStructure.TIM_Pulse = (PWM_PERIOD>>8); //dummy value
  TIM_OC2Init(TIM1, &TIM1_OCInitStructure);
	
	//MOTOR2 PHASE A and B
	 
	TIM1_OCInitStructure.TIM_Pulse = (PWM_PERIOD>>8); //clock wise
	TIM_OC3Init(TIM1, &TIM1_OCInitStructure);
  TIM1_OCInitStructure.TIM_Pulse = (PWM_PERIOD); //ccw 
	TIM_OC4Init(TIM1, &TIM1_OCInitStructure);

/*	
	// Automatic Output enable, Break, dead time and lock configuration
	TIM1_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
	TIM1_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
	TIM1_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1; 
	TIM1_BDTRInitStructure.TIM_DeadTime = MOTOR_DEADTIME;
	TIM1_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
	TIM1_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;
	TIM1_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
	
	TIM_BDTRConfig(TIM1, &TIM1_BDTRInitStructure);
	
	TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);
	
	TIM_ClearITPendingBit(TIM1, TIM_IT_Break);
	TIM_ITConfig(TIM1, TIM_IT_Break, ENABLE);
*/	
	/* TIM1 counter enable */
	TIM_Cmd(TIM1, ENABLE);
	/* Main PWM Output Enable */
  TIM_CtrlPWMOutputs(TIM1,ENABLE);

	//ENABLING THE EN1..EN4 L6206
	
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOE , ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; 	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_SetBits(GPIOE, GPIO_Pin_12);
	GPIO_SetBits(GPIOE, GPIO_Pin_13);
	GPIO_SetBits(GPIOE, GPIO_Pin_14);
	GPIO_SetBits(GPIOE, GPIO_Pin_15);

	//SETTING DRVFLT1..4 as output L6206

	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD , ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_2 | GPIO_Pin_3; 	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	}
	
	{
		// inizialization of the motor 3 and 4

	}


 	

    return(hal_res_OK);
}

extern hal_result_t hal_motor_and_adc_init(hal_motor_t id, const hal_pwm_cfg_t *cfg)
{
 /*   const hal_gpio_map_t *gm = NULL;		  */
    hal_result_t res = hal_res_NOK_generic;
	  TIM_TimeBaseInitTypeDef TIM1_TimeBaseStructure;
  	TIM_OCInitTypeDef TIM1_OCInitStructure;
	  TIM_TimeBaseInitTypeDef TIM8_TimeBaseStructure;
  	TIM_OCInitTypeDef TIM8_OCInitStructure;
  	TIM_BDTRInitTypeDef TIM1_BDTRInitStructure, TIM8_BDTRInitStructure;
  	NVIC_InitTypeDef NVIC_InitStructure;
  	GPIO_InitTypeDef GPIO_InitStructure;
//	ADC_InitTypeDef ADC_InitStructure;


/* INIT MOTOR 1 and 2 */

		//ENABLING THE EN1..EN4 L6206	
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOE , ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; 	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_SetBits(GPIOE, GPIO_Pin_12);
	GPIO_SetBits(GPIOE, GPIO_Pin_13);
	GPIO_SetBits(GPIOE, GPIO_Pin_14);
	GPIO_SetBits(GPIOE, GPIO_Pin_15);

	//SETTING DRVFLT1..4 as output L6206

	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD , ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_2 | GPIO_Pin_3; 	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOD, &GPIO_InitStructure);


//	if (id<2)  
	{
	//initialization of TIM1 for controlling the first 2 motors
	/* TIM1 Peripheral Configuration -------------------------------------------*/
 	/* GPIOA  Clocks enable */
  	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);

	/* GPIOA Configuration: Channel 1, 2, 3 and 4 as alternate function push-pull */
  	
	GPIO_StructInit(&GPIO_InitStructure);  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	/*Timer1 alternate function full remapping*/  
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_TIM1);  //MOT1_PHA
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_TIM1);  //MOT1_PHB
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_TIM1); //MOT2_PHA
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_TIM1); //MOT2_PHB


	/* TIM1 clock enable */
	TIM_DeInit(TIM1);
	TIM_TimeBaseStructInit(&TIM1_TimeBaseStructure);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);	
	/* Time Base configuration */
	TIM1_TimeBaseStructure.TIM_Prescaler = PWM_PRSC;
	TIM1_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned3;//TIM_CounterMode_Up;
	TIM1_TimeBaseStructure.TIM_Period = PWM_PERIOD/2; // in CenterAligned mode the couter counts twice half a pwm period 
	TIM1_TimeBaseStructure.TIM_ClockDivision = 0;// TIM_CKD_DIV2;
	TIM1_TimeBaseStructure.TIM_RepetitionCounter = REP_RATE;
	TIM_TimeBaseInit(TIM1, &TIM1_TimeBaseStructure);
	
//	TIM_OCStructInit(&TIM1_OCInitStructure);
	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
	TIM1_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //indipendent channels 
	TIM1_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM1_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;                  
	TIM1_OCInitStructure.TIM_Pulse = 0x000; //dummy value
	TIM1_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM1_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;         
	TIM1_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM1_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset; //LOW_SIDE_POLARITY;          
	
	//MOTOR1 PHASE A and B
	TIM_OC1Init(TIM1, &TIM1_OCInitStructure);
	TIM1_OCInitStructure.TIM_Pulse = (PWM_PERIOD); //dummy value 
	TIM_OC1Init(TIM1, &TIM1_OCInitStructure);
	TIM1_OCInitStructure.TIM_Pulse = (PWM_PERIOD); //dummy value
    TIM_OC2Init(TIM1, &TIM1_OCInitStructure);
	
	//MOTOR2 PHASE A and B	 
	TIM1_OCInitStructure.TIM_Pulse = (PWM_PERIOD); //clock wise
	TIM_OC3Init(TIM1, &TIM1_OCInitStructure);
    TIM1_OCInitStructure.TIM_Pulse = (PWM_PERIOD); //ccw 
	TIM_OC4Init(TIM1, &TIM1_OCInitStructure);

	
	// Automatic Output enable, Break, dead time and lock configuration
	TIM1_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
	TIM1_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
	TIM1_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1; 
	TIM1_BDTRInitStructure.TIM_DeadTime = MOTOR_DEADTIME;
//	TIM1_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
	TIM1_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;
	TIM1_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
	
	TIM_BDTRConfig(TIM1, &TIM1_BDTRInitStructure);
		
	TIM_ClearITPendingBit(TIM1, TIM_IT_Break);
	TIM_ITConfig(TIM1, TIM_IT_Break, ENABLE);
	
	
	//initialization of TIM1 for controlling the first 2 motors
	/* TIM1 Peripheral Configuration -------------------------------------------*/
 	/* GPIOA  Clocks enable */
	
	
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOI , ENABLE);

	/* GPIOI Configuration: Channel 1, 2, 3 and 4 as alternate function push-pull */
  	
	GPIO_StructInit(&GPIO_InitStructure);  
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOI, &GPIO_InitStructure);
 
	/*Timer1 alternate function full remapping*/  
	GPIO_PinAFConfig(GPIOI,GPIO_PinSource5,GPIO_AF_TIM8);  //MOT3_PHA
	GPIO_PinAFConfig(GPIOI,GPIO_PinSource6,GPIO_AF_TIM8);  //MOT3_PHB
	GPIO_PinAFConfig(GPIOI,GPIO_PinSource7,GPIO_AF_TIM8); //MOT4_PHA
	GPIO_PinAFConfig(GPIOI,GPIO_PinSource2,GPIO_AF_TIM8); //MOT4_PHB


	/* TIM8 clock enable */
	TIM_DeInit(TIM8);
	TIM_TimeBaseStructInit(&TIM8_TimeBaseStructure);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8 , ENABLE);	
	/* Time Base configuration */
	TIM8_TimeBaseStructure.TIM_Prescaler = PWM_PRSC;
	TIM8_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned3;//TIM_CounterMode_Up;
	TIM8_TimeBaseStructure.TIM_Period = PWM_PERIOD/2; // in CenterAligned mode the couter counts twice half a pwm period 
	TIM8_TimeBaseStructure.TIM_ClockDivision = 0;// TIM_CKD_DIV2;
	TIM8_TimeBaseStructure.TIM_RepetitionCounter = REP_RATE;
	TIM_TimeBaseInit(TIM8, &TIM8_TimeBaseStructure);
	
//	TIM_OCStructInit(&TIM8_OCInitStructure);
	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
	TIM8_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //indipendent channels 
	TIM8_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM8_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;                  
	TIM8_OCInitStructure.TIM_Pulse = 0x000; //dummy value
	TIM8_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM8_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;         
	TIM8_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM8_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset; //LOW_SIDE_POLARITY;          
	
	//MOTOR3 PHASE A and B
	TIM_OC1Init(TIM8, &TIM8_OCInitStructure);
	TIM8_OCInitStructure.TIM_Pulse = (PWM_PERIOD); //dummy value 
	TIM_OC1Init(TIM8, &TIM8_OCInitStructure);
	TIM8_OCInitStructure.TIM_Pulse = (PWM_PERIOD); //dummy value
   TIM_OC2Init(TIM8, &TIM8_OCInitStructure);
	
	//MOTOR4 PHASE A and B	 
	TIM8_OCInitStructure.TIM_Pulse = (PWM_PERIOD); //clock wise
	TIM_OC3Init(TIM8, &TIM8_OCInitStructure);
  TIM8_OCInitStructure.TIM_Pulse = (PWM_PERIOD); //ccw 
	TIM_OC4Init(TIM8, &TIM8_OCInitStructure);

	
	// Automatic Output enable, Break, dead time and lock configuration
	TIM8_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
	TIM8_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
	TIM8_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1; 
	TIM8_BDTRInitStructure.TIM_DeadTime = MOTOR_DEADTIME;
	TIM8_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
	TIM8_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;
	TIM8_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
	
	TIM_BDTRConfig(TIM8, &TIM8_BDTRInitStructure);
		
	TIM_ClearITPendingBit(TIM8, TIM_IT_Break);
	TIM_ITConfig(TIM8, TIM_IT_Break, ENABLE);
	
	
	
	TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);
	// Resynch to have the Update evend during Undeflow
	TIM_GenerateEvent(TIM1, TIM_EventSource_Update);  
	// Clear Update Flag
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	TIM_ITConfig(TIM1, TIM_IT_Update, DISABLE);	  

	/* TIM1 counter enable */
	TIM_Cmd(TIM1, ENABLE);
	/* Main PWM Output Enable */
  TIM_CtrlPWMOutputs(TIM1,ENABLE);	  
	TIM_ITConfig(TIM8, TIM_IT_Update, DISABLE);	  
	/* TIM8 counter enable */
	TIM_Cmd(TIM8, ENABLE);
	/* Main PWM Output Enable */
  TIM_CtrlPWMOutputs(TIM8,ENABLE);

	hal_adc_inj_init();
	/* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* Enable the ADC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the Update Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
    
  /* Enable the TIM1 BRK Interrupt */
  //NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
  //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  //NVIC_Init(&NVIC_InitStructure); 
	}
    return(hal_res_OK);
}




extern hal_result_t hal_motor_pwmset(hal_motor_t id, int32_t pwmvalue)
{

	if (pwmvalue>=0) 
	{
		if (pwmvalue<=PWM_PERIOD/2) {}
		else 
		{
			pwmvalue=PWM_PERIOD/2;
		}
		if (pwmvalue>=PWM_MINDUTY) {}
		else 
		{
			pwmvalue=PWM_MINDUTY;
		}
	}
	else
	{
		if (-pwmvalue<=PWM_PERIOD/2) {}
		else
		{ 
			pwmvalue=-PWM_PERIOD/2;
		}
		if (-pwmvalue>=PWM_MINDUTY) {}
		else 
		{
			pwmvalue=-PWM_MINDUTY;
		}
	}

	switch (id)
	{
		case 0:	   //motor1
		{
			if (pwmvalue>0) 
			{
				TIM_SetCompare1(TIM1, pwmvalue);
				TIM_SetCompare2(TIM1, 0);		
			}
			else
			{
				pwmvalue=-pwmvalue;
				TIM_SetCompare1(TIM1, 0);
				TIM_SetCompare2(TIM1, pwmvalue);
			}
		}
		break;
		case 1:	  //motor2
		{
			if (pwmvalue>0) 
			{
				TIM_SetCompare3(TIM1, pwmvalue);
				TIM_SetCompare4(TIM1, 0);		
			}
			else
			{
				pwmvalue=-pwmvalue;
				TIM_SetCompare3(TIM1, 0);
				TIM_SetCompare4(TIM1, pwmvalue);
			}
		}
		break;
		case 2:	   //motor3
		{
			if (pwmvalue>0) 
			{
				TIM_SetCompare1(TIM8, pwmvalue);
				TIM_SetCompare2(TIM8, 0);		
			}
			else
			{
			    pwmvalue=-pwmvalue;
				TIM_SetCompare1(TIM8, 0);
				TIM_SetCompare2(TIM8, pwmvalue);
			}
		}
		break;
		case 3:		//motor4
		{
			if (pwmvalue>0) 
			{
				TIM_SetCompare3(TIM8, pwmvalue);
				TIM_SetCompare4(TIM8, 0);		
			}
			else
			{
			    pwmvalue=-pwmvalue;
				TIM_SetCompare3(TIM8, 0);
				TIM_SetCompare4(TIM8, pwmvalue);
			}
		}
		break;
		default:
		{
		}
		break;
	}

	return hal_res_OK;
}
extern int16_t hal_motor_pwmget(hal_motor_t id)
{
	int16_t pwm=0;
		switch (id)
		{
			case 0:		//motor1
			{
				pwm= TIM1->CCR1; //take the pwmvalue 
				if (pwm==0) 	pwm=-TIM1->CCR2; //take the pwmvalue
			}
			break;
			case 1:		//motor2
			{
				pwm= TIM1->CCR3; //take the pwmvalue 
				if (pwm==0) 	pwm=-TIM1->CCR4; //take the pwmvalue
			}
			break;
			case 2:		//motor3
			{
				pwm= TIM8->CCR1; //take the pwmvalue 
				if (pwm==0) 	pwm=-TIM8->CCR2; //take the pwmvalue
			}
			break;
			case 3:	    //motor4
			{
				pwm= TIM8->CCR3; //take the pwmvalue 
				if (pwm==0) 	pwm=-TIM8->CCR4; //take the pwmvalue
			}
			break;
			default: 
			{
				pwm=0;
			}
			break;
		}
 		return pwm;
}


#endif//HAL_USE_DEVICE_MOTORCL

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



