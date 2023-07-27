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
	@brief      This file implements internal implementation of the hal motor module.
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
#include "hal_adc.h" 
#include "hl_core.h" 
#include "hl_bits.h" 
 
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_motor.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_motor_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_motor_id2index(p)             ((uint8_t)(p))


#define EN1 	GPIO_Pin_12
#define EN2 	GPIO_Pin_13
#define EN3 	GPIO_Pin_14
#define EN4 	GPIO_Pin_15

#define ACTIVE_INTERRUPTS_CHANNELS 1

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// emty-section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    uint32_t                    inittedmask;
    hal_bool_t                  anyinitted;
    hal_bool_t                  externalfaultpressed;
} hal_motor_theinternals_t;


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

static const hal_motor_cfg_t s_hal_motor_config_default = {0};
static hal_bool_t s_hal_motor_out_enabled[hal_motors_number];


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_motor_theinternals_t s_hal_motor_theinternals =
{
    .inittedmask            = 0,
    .anyinitted             = hal_false,
    .externalfaultpressed   = hal_false   
};


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_motor_none_supported_is(void);
static hal_boolval_t s_hal_motor_supported_is(hal_motor_t id);
static void s_hal_motor_initted_set(hal_motor_t id);
static hal_boolval_t s_hal_motor_initted_is(hal_motor_t id);


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_boolval_t hal_motor_supported_is(hal_motor_t id)
{
    return(s_hal_motor_supported_is(id));
}


// Motor init with active interrupts on external fault channels. 
// marco.accame on 4 feb 2016: it is the old function hal_motor_and_adc_init() which had comment: "Motor init with ADC for currents feedback" 

//static GPIO_InitTypeDef GPIO_InitStructure;

extern hal_result_t hal_motor_init(hal_motor_t id, const hal_motor_cfg_t *cfg)
{
    
    if(hal_true == s_hal_motor_none_supported_is())
    {
        return(hal_res_NOK_generic);
    }
    
    if(NULL == cfg)
    {
        cfg = &s_hal_motor_config_default;
    }
    
    id = hal_motorALL;
    
    /*   const hal_gpio_map_t *gm = NULL;		  */
//    hal_result_t res = hal_res_NOK_generic;
	TIM_TimeBaseInitTypeDef TIM1_TimeBaseStructure;
  	TIM_OCInitTypeDef TIM1_OCInitStructure;
	TIM_TimeBaseInitTypeDef TIM8_TimeBaseStructure;
  	TIM_OCInitTypeDef TIM8_OCInitStructure;
  	TIM_BDTRInitTypeDef TIM1_BDTRInitStructure; 
    TIM_BDTRInitTypeDef TIM8_BDTRInitStructure;
  	NVIC_InitTypeDef NVIC_InitStructure;
  	GPIO_InitTypeDef GPIO_InitStructure;
    //ADC_InitTypeDef ADC_InitStructure;

    /* INIT MOTOR 1 and 2 */
	//ENABLING THE EN1..EN4 L6206	
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOE , ENABLE);
	GPIO_StructInit(&GPIO_InitStructure);  
	GPIO_InitStructure.GPIO_Pin = EN1 | EN2 | EN3 | EN4; 	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_SetBits(GPIOE, EN1);
	GPIO_SetBits(GPIOE, EN2);
	GPIO_SetBits(GPIOE, EN3);
	GPIO_SetBits(GPIOE, EN4);

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
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	/*Timer1 alternate function full remapping*/  
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM1); //TIM1_BKIN  
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
	
    //TIM_OCStructInit(&TIM1_OCInitStructure);
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
	TIM1_OCInitStructure.TIM_Pulse = (PWM_PERIOD); //dummy value ----> why this value and not 0?? 
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
	TIM1_BDTRInitStructure.TIM_Break = TIM_Break_Enable; //needed cause enables the input
	TIM1_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
	TIM1_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
	
	TIM_BDTRConfig(TIM1, &TIM1_BDTRInitStructure);
		
	TIM_ClearITPendingBit(TIM1, TIM_IT_Break);
    TIM_ITConfig(TIM1, TIM_IT_Break, ENABLE);

	//initialization of TIM8 for controlling the last 2 motors
	/* TIM8 Peripheral Configuration -------------------------------------------*/
 	/* GPIOI  Clocks enable */
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOI , ENABLE);

	/* GPIOI Configuration: Channel 1, 2, 3 and 4 as alternate function push-pull */  	
	GPIO_StructInit(&GPIO_InitStructure);  
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOI, &GPIO_InitStructure);
 
	/*Timer8 alternate function full remapping*/  
	GPIO_PinAFConfig(GPIOI,GPIO_PinSource4,GPIO_AF_TIM8); //TIM8_BKIN
	GPIO_PinAFConfig(GPIOI,GPIO_PinSource5,GPIO_AF_TIM8); //MOT3_PHA
	GPIO_PinAFConfig(GPIOI,GPIO_PinSource6,GPIO_AF_TIM8); //MOT3_PHB
	GPIO_PinAFConfig(GPIOI,GPIO_PinSource7,GPIO_AF_TIM8); //MOT4_PHA
	GPIO_PinAFConfig(GPIOI,GPIO_PinSource2,GPIO_AF_TIM8); //MOT4_PHB

	/* TIM8 clock enable */
	TIM_DeInit(TIM8);
	TIM_TimeBaseStructInit(&TIM8_TimeBaseStructure);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8 , ENABLE);	
	/* Time Base configuration */
	TIM8_TimeBaseStructure.TIM_Prescaler = PWM_PRSC;
	TIM8_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned3;//TIM_CounterMode_Up;
	TIM8_TimeBaseStructure.TIM_Period = PWM_PERIOD/2; //in CenterAligned mode the couter counts twice half a pwm period 
	TIM8_TimeBaseStructure.TIM_ClockDivision = 0; // TIM_CKD_DIV2;
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
	TIM8_OCInitStructure.TIM_Pulse = (PWM_PERIOD); //dummy value ----> why this value and not 0?? 
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
	TIM8_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Disable; //TIM1 -> TIM_OSSIState_Enable
	TIM8_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1; 
	TIM8_BDTRInitStructure.TIM_DeadTime = MOTOR_DEADTIME;
#if (ACTIVE_INTERRUPTS_CHANNELS == 2)
	TIM8_BDTRInitStructure.TIM_Break =  TIM_Break_Enable; //needed cause enables the input
#else
    TIM8_BDTRInitStructure.TIM_Break =  TIM_Break_Disable;
#endif
	TIM8_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
	TIM8_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
	
	TIM_BDTRConfig(TIM8, &TIM8_BDTRInitStructure);
		
	TIM_ClearITPendingBit(TIM8, TIM_IT_Break);
	TIM_ITConfig(TIM8, TIM_IT_Break, ENABLE);
	
    /*----------------------------------------------------*/
    
    // Output trigger for TIM1 --> it's used to trigger ADC conversion
	TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);
	// Resynch to have the Update evend during Undeflow
	TIM_GenerateEvent(TIM1, TIM_EventSource_Update);  
	// Clear Update Flag
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
    
	TIM_ITConfig(TIM1, TIM_IT_Update, DISABLE);	  
	/* TIM1 counter enable */
	TIM_Cmd(TIM1, ENABLE);
	/* Main PWM Output Enable */
    TIM_CtrlPWMOutputs(TIM1,DISABLE); //now disabled at the beginning
    
	TIM_ITConfig(TIM8, TIM_IT_Update, DISABLE);    
	/* TIM8 counter enable */
	TIM_Cmd(TIM8, ENABLE);
	/* Main PWM Output Enable */
    TIM_CtrlPWMOutputs(TIM8,DISABLE); //now disabled at the beginning

    // Init the ADC to have current values: now disabled cause this feedback is not needed 
	//hal_adc_ADC1_ADC3_current_init();

#if 0
    // marco.accame on 23 nov 2021: as descrived in note tagged _HAL_TAG_USE_OF_NVIC_PRIORITIES
    // we cannot change priority group
	/* Configure one bit for preemption priority */
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
#endif

    /* Enable the ADC Interrupt: now disabled cause this feedback is not needed */
    /*
    NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    */

    // marco.accame on 23 nov 2021: as descrived in note tagged _HAL_TAG_USE_OF_NVIC_PRIORITIES
    // allowed priorities are in range [0, 13]
    // but ... better keeping priority 1 only for the ETH, so we move it up to 4 
    /* Enable the Update Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4; // was 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
      
    // this part was previously commented, now reactivated

    //Set the motor not in fault before activating the interrupts
    s_hal_motor_theinternals.externalfaultpressed = hal_false;

    // marco.accame on 23 nov 2021: as descrived in note tagged _HAL_TAG_USE_OF_NVIC_PRIORITIES
    // allowed priorities are in range [0, 13]
    // but ... as ETH has priority 1 we use 3 in here.
    
    /* Enable the TIM1 BRK Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // it was 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 
	
	/* Enable the TIM8 BRK Interrupt */
    // I'm not enabling it...why?
    // The hardware supports two different faults channels...but phisically (on the robot) we decided to use only one wire (corresponding to TIM1Break);
    // in this situation, if I enable the TIM8Break IRQ it detects ALWAYS a fault situation, preventing the normal behaviour of the board
#if (ACTIVE_INTERRUPTS_CHANNELS == 2)
    NVIC_InitStructure.NVIC_IRQChannel = TIM8_BRK_TIM12_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // it was 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
	}
 
    {
        uint8_t m=0;
        for (m=0; m<hal_motors_number; ++m)
        {
            s_hal_motor_out_enabled[m] = hal_false;
        }
    }
 
    // set all motors which are supported also initted
    if(hal_motorALL == id)
    {
        uint8_t ii=0;
        for(ii=0; ii<hal_motors_number; ii++)
        {
            if(hal_true == s_hal_motor_supported_is((hal_motor_t)ii))
            {
                s_hal_motor_theinternals.anyinitted = hal_true;
                s_hal_motor_initted_set((hal_motor_t)ii);
            }
        }
    }
    else if(id < hal_motors_number)
    {
        s_hal_motor_initted_set(id);
    }
    
    return(hal_res_OK);    
}

extern hal_result_t hal_motor_pwmset(hal_motor_t id, int16_t pwmvalue)
{
    
#if !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)       
    if(hal_false == s_hal_motor_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif
    
    // choose the right pwm to assign
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

    // switch case to set the pwm to the right motor
	switch (id)
	{
		case 0:	  
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
        case 1:	   
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
		case 2:	  
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
		case 3:		
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
	    // Motor 0+1
		case 4:		
		{
			if (pwmvalue>0) 
			{
				TIM_SetCompare1(TIM1, pwmvalue);
				TIM_SetCompare2(TIM1, 0);	
				TIM_SetCompare3(TIM1, pwmvalue);
				TIM_SetCompare4(TIM1, 0);				
			}
			else
			{
				pwmvalue=-pwmvalue;
				TIM_SetCompare1(TIM1, 0);
				TIM_SetCompare2(TIM1, pwmvalue);
				TIM_SetCompare3(TIM1, 0);
				TIM_SetCompare4(TIM1, pwmvalue);
			}
		}
		break;
		// Motor 2+3
		case 5:		
		{
			if (pwmvalue>0) 
			{
				TIM_SetCompare3(TIM8, pwmvalue);
				TIM_SetCompare4(TIM8, 0);			
				TIM_SetCompare1(TIM8, pwmvalue);
				TIM_SetCompare2(TIM8, 0);		
			}
			else
			{
				pwmvalue=-pwmvalue;
				TIM_SetCompare3(TIM8, 0);
				TIM_SetCompare4(TIM8, pwmvalue);
				TIM_SetCompare1(TIM8, 0);
				TIM_SetCompare2(TIM8, pwmvalue);
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
    
#if !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)       
    if(hal_false == s_hal_motor_initted_is(id))
    {
        return(0);
    }
#endif    
    
	int16_t pwm=0;
	switch (id)
	{
		case 0:		
		{
			pwm= TIM1->CCR3; //take the pwmvalue 
			if (pwm==0) 	pwm=-TIM1->CCR4; //take the pwmvalue
            break;
        }
   case 1:		
		{
			pwm= TIM1->CCR1; //take the pwmvalue 
			if (pwm==0) 	pwm=-TIM1->CCR2; //take the pwmvalue
            break;
        }
		case 2:		
		{
			pwm= TIM8->CCR1; //take the pwmvalue 
			if (pwm==0) 	pwm=-TIM8->CCR2; //take the pwmvalue
            break;
        }
		case 3:	    
		{
			pwm= TIM8->CCR3; //take the pwmvalue 
			if (pwm==0) 	pwm=-TIM8->CCR4; //take the pwmvalue
			break;
        } 
	  case 4:	    
		{
			pwm= TIM1->CCR3; //take the pwmvalue 
			if (pwm==0) 	pwm=-TIM1->CCR4; //take the pwmvalue
			break;
        }

		// Motor 0+1		
	  case 4:	    
		{
			pwm= TIM1->CCR3; //take the pwmvalue 
			if (pwm==0) 	pwm=-TIM1->CCR4; //take the pwmvalue
			break;
        }
		// Motor 2+3
	  case 5:	    
		{
			pwm= TIM8->CCR1; //take the pwmvalue 
			if (pwm==0) 	pwm=-TIM8->CCR2; //take the pwmvalue
      break;
        }
				
		default:
		{
			pwm=0;
            break;
        }
	}
 	return pwm;
}


extern hal_bool_t hal_motor_externalfaulted(void)
{
    //if(hal_true == s_hal_motor_theinternals.anyinitted)
    return(s_hal_motor_theinternals.externalfaultpressed);
}

extern hal_bool_t hal_motor_external_fault_active(void)
{
    
    //return(1 == GPIO_ReadInputDataBit(GPIOA, GPIO_PinSource6));
    
    static hal_gpio_t fault_pin = {.port = hal_gpio_portA, .pin = hal_gpio_pin6};
    
    return(hal_gpio_getval(fault_pin) == hal_gpio_valHIGH);
}

extern hal_result_t hal_motor_enable(hal_motor_t id)
{
    
#if !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)       
    if(hal_false == s_hal_motor_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif
    
	switch (id)
	{
    case 0:
        {
		  TIM_CtrlPWMOutputs(TIM1,ENABLE);
			GPIO_SetBits(GPIOE, EN2);
			s_hal_motor_out_enabled[0] = hal_true;
		}
		break;
		case 1:
		{
		  TIM_CtrlPWMOutputs(TIM1,ENABLE);
			GPIO_SetBits(GPIOE, EN1);
      s_hal_motor_out_enabled[1] = hal_true;
		}
		break;
	    case 2:
        {
		  TIM_CtrlPWMOutputs(TIM8,ENABLE);
			GPIO_SetBits(GPIOE, EN3);
			s_hal_motor_out_enabled[2] = hal_true;
		}
		break;
		case 3:
		{
		  TIM_CtrlPWMOutputs(TIM8,ENABLE);
			GPIO_SetBits(GPIOE, EN4);
      s_hal_motor_out_enabled[3] = hal_true;
		}
		break;
 	  // Motor 0+1
    case 4:
        {
		  TIM_CtrlPWMOutputs(TIM1,ENABLE);
			GPIO_SetBits(GPIOE, EN2);
			GPIO_SetBits(GPIOE, EN1);
      s_hal_motor_out_enabled[4] = hal_true;
		}
		break;

  	// Motor 2+3
    case 5:
        {
		  TIM_CtrlPWMOutputs(TIM8,ENABLE);
			GPIO_SetBits(GPIOE, EN3);
			GPIO_SetBits(GPIOE, EN4);
      s_hal_motor_out_enabled[5] = hal_true;
		}
		break;
		default:
		{
			TIM_CtrlPWMOutputs(TIM1,ENABLE);
		    TIM_CtrlPWMOutputs(TIM8,ENABLE);
			GPIO_SetBits(GPIOE, EN1);
			GPIO_SetBits(GPIOE, EN2);
			GPIO_SetBits(GPIOE, EN3);
			GPIO_SetBits(GPIOE, EN4);
            s_hal_motor_out_enabled[0] = hal_true;
            s_hal_motor_out_enabled[1] = hal_true;
            s_hal_motor_out_enabled[2] = hal_true;
            s_hal_motor_out_enabled[3] = hal_true;
		}
		break;
	}
}
extern hal_result_t hal_motor_disable(hal_motor_t id)
{
    
#if !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)       
    if(hal_false == s_hal_motor_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
#endif
    
	switch (id)
	{
        case 0:
		{
		    if (s_hal_motor_out_enabled[1] == hal_false) TIM_CtrlPWMOutputs(TIM1,DISABLE);
			GPIO_ResetBits(GPIOE, EN2);
            s_hal_motor_out_enabled[0] = hal_false;
		}
		break;
		case 1:
		{
		    if (s_hal_motor_out_enabled[0] == hal_false) TIM_CtrlPWMOutputs(TIM1,DISABLE);
			GPIO_ResetBits(GPIOE, EN1);
            s_hal_motor_out_enabled[1] = hal_false;
		}
		break;
	    case 2:
		{
		    if (s_hal_motor_out_enabled[3] == hal_false) TIM_CtrlPWMOutputs(TIM8,DISABLE);
			GPIO_ResetBits(GPIOE, EN3);
            s_hal_motor_out_enabled[2] = hal_false;
		}
		break;
		case 3:
		{
		    if (s_hal_motor_out_enabled[2] == hal_false) TIM_CtrlPWMOutputs(TIM8,DISABLE);
			GPIO_ResetBits(GPIOE, EN4);
            s_hal_motor_out_enabled[3] = hal_false;
		}
		break;
		// Motor 0+1
		case 4: 
		{
		    if (s_hal_motor_out_enabled[1] == hal_false) TIM_CtrlPWMOutputs(TIM1,DISABLE);
			GPIO_ResetBits(GPIOE, EN2);
            s_hal_motor_out_enabled[0] = hal_false;
		    if (s_hal_motor_out_enabled[0] == hal_false) TIM_CtrlPWMOutputs(TIM1,DISABLE);
			GPIO_ResetBits(GPIOE, EN1);
            s_hal_motor_out_enabled[1] = hal_false;
		}
		break;
		// Motor 2+3
		case 5:
		{
		    if (s_hal_motor_out_enabled[3] == hal_false) TIM_CtrlPWMOutputs(TIM8,DISABLE);
			GPIO_ResetBits(GPIOE, EN3);
            s_hal_motor_out_enabled[2] = hal_false;
		    if (s_hal_motor_out_enabled[2] == hal_false) TIM_CtrlPWMOutputs(TIM8,DISABLE);
			GPIO_ResetBits(GPIOE, EN4);
            s_hal_motor_out_enabled[3] = hal_false;
		}
		break;
		default:
		{
			TIM_CtrlPWMOutputs(TIM1,DISABLE);
		    TIM_CtrlPWMOutputs(TIM8,DISABLE);
			GPIO_ResetBits(GPIOE, EN1);
			GPIO_ResetBits(GPIOE, EN2);
			GPIO_ResetBits(GPIOE, EN3);
			GPIO_ResetBits(GPIOE, EN4);
            s_hal_motor_out_enabled[0] = hal_false;
            s_hal_motor_out_enabled[1] = hal_false;
            s_hal_motor_out_enabled[2] = hal_false;
            s_hal_motor_out_enabled[3] = hal_false;
		}
		break;
	}
	return hal_res_OK;
}


extern hal_result_t hal_motor_reenable_break_interrupts(void)
{    
    if(hal_false == s_hal_motor_theinternals.anyinitted)
    {
        return(hal_res_NOK_generic);
    }
    
    //calling this function, I presume that the fault is no more occurring
    s_hal_motor_theinternals.externalfaultpressed = hal_false;
    
    //reenables the interrupts
    TIM_ClearITPendingBit(TIM1, TIM_IT_Break);
    TIM_ITConfig(TIM1, TIM_IT_Break, ENABLE);
    
#if (ACTIVE_INTERRUPTS_CHANNELS == 2)
    TIM_ClearITPendingBit(TIM8, TIM_IT_Break);
    TIM_ITConfig(TIM8, TIM_IT_Break, ENABLE);
#endif
    
    return(hal_res_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----

/*******************************************************************************
* Function Name  : TIM1_BRK_IRQHandler
* Description    : This function handles TIM1 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void TIM1_BRK_TIM9_IRQHandler(void)
{
  /* TO DO HERE:
  - deactivate the interrupt on TIM1 Break
  - disable motors 1-2
  - if only this interrupt channel is enabled, disable also motors 3-4
  - set the flag signalling the fault
  - clear pending bit
  */    
  // write somewhere disable due to external fault

    if(SET == TIM_GetFlagStatus(TIM1, TIM_FLAG_Break))
    {
        TIM_ITConfig(TIM1, TIM_IT_Break, DISABLE);
        hal_motor_disable(hal_motor1);
        hal_motor_disable(hal_motor2);
#if (ACTIVE_INTERRUPTS_CHANNELS == 1)
        hal_motor_disable(hal_motor3);
        hal_motor_disable(hal_motor4);
#endif
		
        TIM_ClearITPendingBit(TIM1, TIM_IT_Break);
        
        s_hal_motor_theinternals.externalfaultpressed = hal_true;
    }
}

#if (ACTIVE_INTERRUPTS_CHANNELS == 2)
/*******************************************************************************
* Function Name  : TIM8_BRK_IRQHandler
* Description    : This function handles TIM8 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_BRK_TIM12_IRQHandler(void)
{
  /* TO DO HERE:
  - deactivate the interrupt on TIM1 Break
  - disable motors 3-4
  - set the flag signalling the fault
  - clear pending bit
  */    
  // write somewhere disable due to external fault 
  if(SET == TIM_GetFlagStatus(TIM8, TIM_FLAG_Break))
  {
      TIM_ITConfig(TIM8, TIM_IT_Break, DISABLE);
      hal_motor_disable(motor3);
      hal_motor_disable(motor4);
      s_hal_motor_theinternals.externalfaultpressed = hal_true;
      TIM_ClearITPendingBit(TIM8, TIM_IT_Break);
  }
}
#endif

// ---- isr of the module: end ------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_motor_none_supported_is(void)
{
    if(0 == hal_motor__theboardconfig.supportedmask)
    {
        return(hal_true);
    }
    return(hal_false);
}


static hal_boolval_t s_hal_motor_supported_is(hal_motor_t id)
{
    if(id >= hal_motors_number)
    {
        return(hal_false);
    }
    return((hal_boolval_t)hl_bits_word_bitcheck(hal_motor__theboardconfig.supportedmask, HAL_motor_id2index(id)) );
}

static void s_hal_motor_initted_set(hal_motor_t id)
{	
	 hl_bits_word_bitset(&s_hal_motor_theinternals.inittedmask, HAL_motor_id2index(id));
}

static void s_hal_motor_initted_reset(hal_motor_t id)
{
    hl_bits_word_bitclear(&s_hal_motor_theinternals.inittedmask, HAL_motor_id2index(id));
}

static hal_boolval_t s_hal_motor_initted_is(hal_motor_t id)
{  
	if(id >= hal_motors_number)
    {
        return(hal_false);
    }    
    return((hal_boolval_t)hl_bits_word_bitcheck(s_hal_motor_theinternals.inittedmask, HAL_motor_id2index(id)));
}
    


// --------------------------------------------------------------------------------------------------------------------
// - obsolete code
// --------------------------------------------------------------------------------------------------------------------


///** @fn         extern hal_result_t hal_motor_init(hal_motor_t id, const hal_motor_cfg_t *cfg)
//    @brief      This function initializes a motor. It can be called once per motor. 

//    @details    On ARM-STM32 architecture, the registers of the timers are 16 bits, and holds bot the counter and the prescaler.
//                As a result, the precision and maximum range is chosen as follows: (prec, max) = (1000 us, 6400ms), 
//                (100 us, 640ms), (10 us, 64ms), (1 us, 8ms). 
//    @param      motor           The motor to initialise. 
//    @param      cfg             The configuration. It cannot be NULL.

//    @return     hal_res_NOK_generic in case the motor isn't configured or not supported by board.
//                hal_res_NOK_unsupported if the chosen pwm cfg is not supported
//                hal_res_NOK_nullpointer if @e cfg is NULL
//                hal_res_OK otherwise
//  */
//extern hal_result_t hal_motor_init(hal_motor_t id, const hal_motor_cfg_t *cfg);

//extern hal_result_t hal_motor_init(hal_motor_t id, const hal_motor_cfg_t *cfg)
//{
//    
//    // marco.accame: so far initialization is done for all ports and cfg is not used 
//    //               use id = hal_motorALL
//    
//    #warning TODO: remove possibility of multiple init(). add a deinit().
//    
//    
// /*   const hal_gpio_map_t *gm = NULL;		  */
////    hal_result_t res = hal_res_NOK_generic;
//    
//	TIM_TimeBaseInitTypeDef TIM1_TimeBaseStructure;
//  	TIM_OCInitTypeDef TIM1_OCInitStructure;
//  	TIM_BDTRInitTypeDef TIM1_BDTRInitStructure;
////  NVIC_InitTypeDef NVIC_InitStructure;
//  	GPIO_InitTypeDef GPIO_InitStructure;

///* INIT MOTOR 1 and 2 */

////	if (id<2)  
//	{
//	//initialization of TIM1 for controlling the first 2 motors
//	/* TIM1 Peripheral Configuration -------------------------------------------*/
// 	/* GPIOA  Clocks enable */
//  	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);

//	/* GPIOA Configuration: Channel 1, 2, 3 and 4 as alternate function push-pull */
//	GPIO_StructInit(&GPIO_InitStructure);  
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
//  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
//  	GPIO_Init(GPIOA, &GPIO_InitStructure);
// 
//	/*Timer1 alternate function full remapping*/  
//	GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_TIM1);  //MOT1_PHA
//	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_TIM1);  //MOT1_PHB
//	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_TIM1); //MOT2_PHA
//	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_TIM1); //MOT2_PHB

//	/* TIM1 clock enable */
//	TIM_DeInit(TIM1);
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);	
////	TIM_TimeBaseStructInit(&TIM1_TimeBaseStructure);
//	/* Time Base configuration */
//	TIM1_TimeBaseStructure.TIM_Prescaler = PWM_PRSC;
//	TIM1_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;//TIM_CounterMode_Up;
//	TIM1_TimeBaseStructure.TIM_Period = PWM_PERIOD/2; // in CenterAligned mode the couter counts twice half a pwm period 
//	TIM1_TimeBaseStructure.TIM_ClockDivision = 0;// TIM_CKD_DIV2;
//	TIM1_TimeBaseStructure.TIM_RepetitionCounter = 0;// REP_RATE;

//	TIM_TimeBaseInit(TIM1, &TIM1_TimeBaseStructure);
//	
////	TIM_OCStructInit(&TIM1_OCInitStructure);
//	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
//	TIM1_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //indipendent channels 
//	TIM1_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
//	TIM1_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;                  
//	TIM1_OCInitStructure.TIM_Pulse = 0x000; //dummy value
//	TIM1_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
//	TIM1_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;         
//	TIM1_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
//	TIM1_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset; //LOW_SIDE_POLARITY;          
//	
//	//MOTOR1 PHASE A and B
//	TIM_OC1Init(TIM1, &TIM1_OCInitStructure);
//	TIM1_OCInitStructure.TIM_Pulse = (PWM_PERIOD>>0); //dummy value 
//	TIM_OC1Init(TIM1, &TIM1_OCInitStructure);
//	TIM1_OCInitStructure.TIM_Pulse = (PWM_PERIOD>>8); //dummy value
//    TIM_OC2Init(TIM1, &TIM1_OCInitStructure);
//	
//	//MOTOR2 PHASE A and B 
//	TIM1_OCInitStructure.TIM_Pulse = (PWM_PERIOD>>8); //clock wise
//	TIM_OC3Init(TIM1, &TIM1_OCInitStructure);
//    TIM1_OCInitStructure.TIM_Pulse = (PWM_PERIOD); //ccw 
//	TIM_OC4Init(TIM1, &TIM1_OCInitStructure);

//    // Automatic Output enable, Break, dead time and lock configuration
//    /*
//	TIM1_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
//	TIM1_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
//	TIM1_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1; 
//	TIM1_BDTRInitStructure.TIM_DeadTime = MOTOR_DEADTIME;
//	TIM1_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
//	TIM1_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;
//	TIM1_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
//	
//	TIM_BDTRConfig(TIM1, &TIM1_BDTRInitStructure);
//	
//	TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);
//	
//	TIM_ClearITPendingBit(TIM1, TIM_IT_Break);
//	TIM_ITConfig(TIM1, TIM_IT_Break, ENABLE);
//    */	
//	/* TIM1 counter enable */
//	TIM_Cmd(TIM1, ENABLE);
//	/* Main PWM Output Enable */
//    TIM_CtrlPWMOutputs(TIM1,ENABLE);

//	//ENABLING THE EN1..EN4 L6206...?
//	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOE , ENABLE);

//	GPIO_StructInit(&GPIO_InitStructure);  
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; 	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
//    GPIO_Init(GPIOE, &GPIO_InitStructure);

//	GPIO_SetBits(GPIOE, GPIO_Pin_12);
//	GPIO_SetBits(GPIOE, GPIO_Pin_13);
//	GPIO_SetBits(GPIOE, GPIO_Pin_14);
//	GPIO_SetBits(GPIOE, GPIO_Pin_15);

//	//SETTING DRVFLT1..4 as output L6206...?

//	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD , ENABLE);

//	GPIO_StructInit(&GPIO_InitStructure);  

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_2 | GPIO_Pin_3; 	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
//    GPIO_Init(GPIOD, &GPIO_InitStructure);
//    }
///* INIT MOTOR 3 and 4 */
//	{
//		// inizialization of the motor 3 and 4
//        // ?? how ??
//	}
//    
//    // set all motors whciha re supported also initted
//    if(hal_motorALL == id)
//    {
//        uint8_t ii=0;
//        for(ii=0; ii<hal_motors_number; ii++)
//        {
//            if(hal_true == s_hal_motor_supported_is((hal_motor_t)ii))
//            {
//                s_hal_motor_initted_set((hal_motor_t)ii);
//            }
//        }
//    }
//    else if(id < hal_motors_number)
//    {
//        s_hal_motor_initted_set(id);
//    }
//    
//    return(hal_res_OK);
//}


#endif//HAL_USE_DEVICE_MOTORCL

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



