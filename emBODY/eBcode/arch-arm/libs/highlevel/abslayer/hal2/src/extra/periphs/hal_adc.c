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


/* @file       hal_periph_adc.c
	@brief      This file implements internal implementation of the hal led module.
	@author     marco.maggiali@iit.it, 
    @date       26/03/2013
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_ADC

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_gpio.h"
#include "hal_brdcfg.h"
#include "hl_core.h"

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_adc.h"
#include "hal_dc_motorctl.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

//#define ADC1                     ADC1
#define ADC1_CLK                 RCC_APB2Periph_ADC1		 //ADCCLK= 42MHz/2 21/    
#define DMA_CHANNEL0             DMA_Channel_0
#define DMA_STREAM0              DMA2_Stream0
#define ADC1_DR_ADDRESS          ((uint32_t)0x4001204C)	//ADC1BASE +0x4C
#define ADC1_CHANNEL_GPIO_CLK    RCC_AHB1Periph_GPIOA
								
//#define ADC2                     ADC2
#define ADC2_CLK                 RCC_APB2Periph_ADC2
#define DMA_CHANNEL1             DMA_Channel_1
#define DMA_STREAM2              DMA2_Stream2
#define ADC2_DR_ADDRESS          ((uint32_t)0x4001214C)

//#define ADC3                     ADC3
#define ADC3_CLK                 RCC_APB2Periph_ADC3
#define DMA_CHANNEL2             DMA_Channel_2
//#define DMA_STREAM0              DMA2_Stream0
#define ADC3_DR_ADDRESS          ((uint32_t)0x4001224C)

#define ADC_CDR_ADDRESS          ((uint32_t)0x40012308)

#define SAMPLING_TIME_CK 		 ADC_SampleTime_15Cycles //	  ADC_SampleTime_15Cycles
#define NB_CONVERSIONS 32
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


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
volatile uint16_t uhADCConvertedValue[12];

volatile uint16_t uhAN1 = 0;
volatile uint16_t uhAN2 = 0;
volatile uint16_t uhAN3 = 0;
volatile uint16_t uhAN4 = 0;
volatile uint16_t uhTVIN = 0;
volatile uint16_t uhTVAUX = 0;

volatile uint16_t uhCur[4] = {0,0,0,0};
volatile int16_t  hCurOffset[4]= {0,0,0,0};


volatile uint32_t uwVBATVoltage = 0;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

/**
  * @brief  ADC1 channels 3,4,5 ADC2 8,9,10 ADC3 channels 14,15,6,7,VBAT with DMA configuration
  * @note   This function Configure the ADC peripheral  
            1) Enable peripheral clocks
            2) DMA2_Stream0 channel2 configuration
            3) Configure ADC Channels pin as analog input
            4) ADC1 channels 3,4,5 ADC2 8,9,10 ADC3 channels 14,15,6,7,VBAT with DMA configuration
  * @param  None
  * @retval None

  */

extern hal_result_t hal_adc_dma_init()	   
{

	  ADC_InitTypeDef       ADC_InitStructure;
	  ADC_CommonInitTypeDef ADC_CommonInitStructure;
	  DMA_InitTypeDef       DMA_InitStructure;
	  GPIO_InitTypeDef      GPIO_InitStructure;
	
	  /* Enable ADC1, ADC2 and ADC3 DMA  ****************************************/ 
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  
	  RCC_APB2PeriphClockCmd(ADC1_CLK, ENABLE);
	  RCC_APB2PeriphClockCmd(ADC2_CLK, ENABLE);
	  RCC_APB2PeriphClockCmd(ADC3_CLK, ENABLE);
	  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE);
	  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE);
	  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE);
	  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOF, ENABLE);

	  //					ADC
	  /* DMA2 Stream0 channel0  configuration **************************************/
	  DMA_DeInit(DMA2_Stream0);
	  DMA_InitStructure.DMA_Channel = DMA_CHANNEL0;// | DMA_CHANNEL1;  
	  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC_CDR_ADDRESS; //ADC1_DR_ADDRESS;
	  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) &uhADCConvertedValue;
	  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	  DMA_InitStructure.DMA_BufferSize = 12;
	  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;         
	  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	  DMA_Init(DMA_STREAM0, &DMA_InitStructure);
	  DMA_Cmd(DMA_STREAM0, ENABLE);
	
	/* Configure ADC Channels pin as analog input ******************************/
//	  GPIO_StructInit(&GPIO_InitStructure);
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 ;	  
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;	  
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	  GPIO_Init(GPIOB, &GPIO_InitStructure);	  

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;	  
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	  GPIO_Init(GPIOC, &GPIO_InitStructure);
	  
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9 ; 	  
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	  GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	  /* ADC Common Init **********************************************************/
	  ADC_CommonInitStructure.ADC_Mode =ADC_TripleMode_RegSimult;//ADC_DualMode_RegSimult;
	  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;
	  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles ;
	  ADC_CommonInit(&ADC_CommonInitStructure);
	
	  /* ADC123 Init ****************************************************************/	
	  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	  ADC_InitStructure.ADC_ScanConvMode =ENABLE;
	  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	  ADC_InitStructure.ADC_NbrOfConversion = 4;
	  ADC_Init(ADC1, &ADC_InitStructure);	
	  ADC_Init(ADC2, &ADC_InitStructure);
	  ADC_Init(ADC3, &ADC_InitStructure);

	  /* ADC1 regular channel3 4 and 5 configuration *************************************/
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_3 , 1, SAMPLING_TIME_CK);
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_4 , 2, SAMPLING_TIME_CK);
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_5 , 3, SAMPLING_TIME_CK);
		ADC_RegularChannelConfig(ADC1, ADC_Channel_16 , 4, SAMPLING_TIME_CK); //DUMMY


	  /* ADC2 regular channel3 4 and 5 configuration *************************************/
	  ADC_RegularChannelConfig(ADC2, ADC_Channel_8 , 1, SAMPLING_TIME_CK);
   	ADC_RegularChannelConfig(ADC2, ADC_Channel_9 , 2, SAMPLING_TIME_CK);
  	ADC_RegularChannelConfig(ADC2, ADC_Channel_10, 3, SAMPLING_TIME_CK);
 	//	ADC_RegularChannelConfig(ADC2, ADC_Channel_13, 4, SAMPLING_TIME_CK); //Temp Sensor
	  /* ADC3 regular channel3 4 and 5 configuration *************************************/
	  ADC_RegularChannelConfig(ADC3, ADC_Channel_14 ,   1, SAMPLING_TIME_CK);
	  ADC_RegularChannelConfig(ADC3, ADC_Channel_15 ,   2, SAMPLING_TIME_CK);
		ADC_RegularChannelConfig(ADC3, ADC_Channel_6 ,    3, SAMPLING_TIME_CK);
		ADC_RegularChannelConfig(ADC3, ADC_Channel_7 ,    4, SAMPLING_TIME_CK);
	  	  	   
	   /* Enable VBAT channel */
    //ADC_VBATCmd(ENABLE); 
	  ADC_TempSensorVrefintCmd(ENABLE);
	  ADC_RegularChannelConfig(ADC2, ADC_Channel_TempSensor, 4, SAMPLING_TIME_CK);
	    
	 /* Enable DMA request after last transfer (Single-ADC mode) */
//	  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
	  ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);
		/* Enable ADC1 DMA since ADC1 is the Master*/
 //   ADC_DMACmd(ADC1, ENABLE); 
	  /* Enable ADC */
	  ADC_Cmd(ADC1, ENABLE);
	  ADC_Cmd(ADC2, ENABLE);
	  ADC_Cmd(ADC3, ENABLE);

	  ADC_SoftwareStartConv(ADC1);
	//	ADC_SoftwareStartConv(ADC2);
	//	ADC_SoftwareStartConv(ADC3);
	  return hal_res_OK;
}

extern hal_result_t hal_adc_inj_init()	   
{

	  ADC_InitTypeDef       ADC_InitStructure;
	  GPIO_InitTypeDef      GPIO_InitStructure;
	  ADC_CommonInitTypeDef ADC_CommonInitStructure;

	  /* Enable ADC1, ADC2 and ADC3 DMA  ****************************************/ 
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  
	  RCC_APB2PeriphClockCmd(ADC1_CLK, ENABLE);
	  RCC_APB2PeriphClockCmd(ADC2_CLK, ENABLE);
	  RCC_APB2PeriphClockCmd(ADC3_CLK, ENABLE);
	  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE);
	  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE);
	  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE);
	  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOF, ENABLE);

	  //					ADC
	
	/* Configure ADC Channels pin as analog input ******************************/
	  //Configure CUR1 CUR2 (PA3 e PA4) and CUR3 and CUR4 (PF8 e PF9)
	  GPIO_StructInit(&GPIO_InitStructure);
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 ;	  
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);
	  	  
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9  ;	  
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	  GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	  /* ADC1 ADC2 ADC3 registers reset ----------------------------------------------------*/
  	  ADC_DeInit();

	  /* ADC Common Init **********************************************************/
	  ADC_CommonInitStructure.ADC_Mode =ADC_TripleMode_InjecSimult;//ADC_DualMode_RegSimult;
	  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	  ADC_CommonInit(&ADC_CommonInitStructure);

	  /* ADC1 Init **********************************************************/
	  
	  ADC_StructInit(&ADC_InitStructure);
	  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	 // ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Left;
	  ADC_InitStructure.ADC_NbrOfConversion = 3;	 //the first is a dummy one
	  ADC_Init(ADC1, &ADC_InitStructure);

	  /* ADC2 Configuration ------------------------------------------------------*/
	  ADC_StructInit(&ADC_InitStructure);  
	  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	//  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Left;
	  ADC_InitStructure.ADC_NbrOfConversion = 3;	 //the first is a dummy one
	  ADC_Init(ADC2, &ADC_InitStructure);

	  /* ADC3 Configuration ------------------------------------------------------*/
	  ADC_StructInit(&ADC_InitStructure);  
	  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	//  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Left;
	  ADC_InitStructure.ADC_NbrOfConversion = 3;	 //the first is a dummy one
	  ADC_Init(ADC3, &ADC_InitStructure);

	  /* ADC1 Injected group of conversions end interrupt disabling */
 	  ADC_ITConfig(ADC1, ADC_IT_JEOC, DISABLE);
	  ADC_InjectedSequencerLengthConfig(ADC1,3);
	  /* ADC1 regular channel3 4 and 5 configuration *************************************/
	  ADC_InjectedChannelConfig(ADC1, ADC_Channel_3 , 1, SAMPLING_TIME_CK);//CUR1
	  ADC_InjectedChannelConfig(ADC1, ADC_Channel_3 , 3, SAMPLING_TIME_CK);//CUR1
	  ADC_InjectedChannelConfig(ADC1, ADC_Channel_4 , 2, SAMPLING_TIME_CK);//CUR2
 	  
	  /* ADC2 regular channel8 and 9 configuration *************************************/
	  ADC_InjectedChannelConfig(ADC2, ADC_Channel_8 , 1, SAMPLING_TIME_CK);//AN1
	  ADC_InjectedChannelConfig(ADC2, ADC_Channel_9 , 2, SAMPLING_TIME_CK);//AN2
	  
	  ADC_InjectedSequencerLengthConfig(ADC3,3);	
	  /* ADC3 regular channel 6 and 7 configuration *************************************/
	  ADC_InjectedChannelConfig(ADC3, ADC_Channel_6 , 1, SAMPLING_TIME_CK);//CUR3
	  ADC_InjectedChannelConfig(ADC3, ADC_Channel_6 , 3, SAMPLING_TIME_CK);//CUR3
	  ADC_InjectedChannelConfig(ADC3, ADC_Channel_7 , 2, SAMPLING_TIME_CK);//CUR4
	    
	
	  /* Enable ADC */
	  ADC_Cmd(ADC1, ENABLE);
	  ADC_Cmd(ADC2, ENABLE);
	  ADC_Cmd(ADC3, ENABLE);



	  hal_adc_currentCalibration();

	  return hal_res_OK;
}

extern uint16_t hal_get_adc(uint16_t ADC_TYPE, uint16_t channel)
{
	return	uhADCConvertedValue[((ADC_TYPE-1))+(channel*3)];
}

void hal_adc_currentCalibration(void)
{
  static uint16_t bIndex;
  
  /* ADC1 Injected group of conversions end interrupt disabling */
  ADC_ITConfig(ADC1, ADC_IT_JEOC, DISABLE);
  ADC_ITConfig(ADC2, ADC_IT_JEOC, DISABLE);
  ADC_ITConfig(ADC3, ADC_IT_JEOC, DISABLE);

  /* ADC1 Injected conversions trigger is given by software and enabled */ 
  //ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_TRGO);   
  //ADC_ExternalTrigInjectedConvCmd(ADC1,ENABLE);  

  /* ADC1 Injected conversions configuration */ 

  /* Clear the ADC1 JEOC pending flag */
  ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);  
  ADC_SoftwareStartInjectedConv(ADC1);
 //ADC_ClearFlag(ADC3, ADC_FLAG_JEOC);  
 //ADC_SoftwareStartInjectedConvCmd(ADC3,ENABLE); 
	
  /* ADC Channel used for current reading are read 
     in order to get zero currents ADC values*/ 

  for(bIndex=0; bIndex <NB_CONVERSIONS; bIndex++)
  {
    while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_JEOC)) { }
    
   	hCurOffset[0] += (ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_3)>>5);
    hCurOffset[1] += (ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_2)>>5);     
	  hCurOffset[2] += (ADC_GetInjectedConversionValue(ADC3,ADC_InjectedChannel_3)>>5);
    hCurOffset[3] += (ADC_GetInjectedConversionValue(ADC3,ADC_InjectedChannel_2)>>5);    
     
    /* Clear the ADC1 JEOC pending flag */
    ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);    
    ADC_SoftwareStartInjectedConv(ADC1);
  }
  hal_adc_currentConfig();  
}

/*******************************************************************************
* Function Name  : hal_adc_currentConfig
* Description    : This function configure ADC1 for 2 shunt current 
*                  reading and ADC3 for 2 shunt current reading after a 
*                  calibration of the four utilized ADC Channels
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void hal_adc_currentConfig(void)
{

    
  /* ADC1 Injected conversions trigger is TIM1 TRGO */ 
  ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_TRGO); 
//  ADC_ExternalTrigInjectedConvEdgeConfig(ADC1, ADC_ExternalTrigInjecConvEdge_Rising); 
  ADC_ExternalTrigInjectedConvEdgeConfig(ADC1, ADC_ExternalTrigInjecConvEdge_Rising);   //ADC_ExternalTrigInjectedConvCmd(ADC3,ENABLE);
  
  /* Bus voltage protection initialization*/                            
//  ADC_AnalogWatchdogCmd(ADC1,ADC_AnalogWatchdog_SingleInjecEnable);
//  ADC_AnalogWatchdogSingleChannelConfig(ADC1,BUS_VOLT_FDBK_CHANNEL);
//  ADC_AnalogWatchdogThresholdsConfig(ADC1, OVERVOLTAGE_THRESHOLD>>3,0x00);
  
  /* ADC1 Injected group of conversions end and Analog Watchdog interrupts enabling */

//  ADC_ITConfig(ADC1, ADC_IT_JEOC | ADC_IT_AWD, ENABLE);
  ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);

}

/*******************************************************************************
* Function Name  : ADC1_IRQHandler
* Description    : This function handles ADC1, ADC2 and ADC3 global interrupts requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_IRQHandler(void)
{
  int32_t pwm[4]={0,0,0,0};
  int16_t deadtime= 0;//; MOTOR_DEADTIME+0;
  int16_t Cur[4]={0,0,0,0};	
  //if(ADC_GetITStatus(ADC1, ADC_IT_JEOC) == SET))
  if((ADC1->SR & ADC_FLAG_JEOC) == ADC_FLAG_JEOC)
  {
    //It clear JEOC flag
    ADC1->SR = ~(u32)ADC_FLAG_JEOC;
      
    if (1)//(SVPWMEOCEvent())
    {
         
      //MCL_Calc_BusVolt();
	    //TIM_GetCapture1(TIM_TypeDef* TIMx);
		pwm[0]= TIM1->CCR1; //take the pwmvalue 
		if (pwm[0]==0) 	pwm[0]=TIM1->CCR2; //take the pwmvalue

		pwm[1]=TIM1->CCR3; //take the pwmvalue
		if (pwm[1]==0) 	pwm[1]=TIM1->CCR4; //take the pwmvalue

		if (pwm[0]>deadtime)
		{
		 Cur[0]=(Cur[0]>>1)+((ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_3)-hCurOffset[0])>>1) ;
//   uhCur[0]=(int16_t)(((pwm[0]-deadtime)*Cur[0])/(3360-deadtime<<1)); //PWM_PERIOD-2*deadtime  6720-168 =
	   uhCur[0]=Cur[0];
		}
		else  uhCur[0]=0;

		if (pwm[1]>deadtime)
		{
	    Cur[1]=(Cur[1]>>1)+((ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_2)-hCurOffset[1])>>1);
//   uhCur[1]=(int16_t)(((pwm[1]-deadtime)*Cur[1])/(3360-deadtime<<1)); //PWM_PERIOD-2*deadtime  6720-168 =
		uhCur[1]=Cur[1];
		}
		else  uhCur[1]=0;

    }
  }
  if((ADC2->SR & ADC_FLAG_JEOC) == ADC_FLAG_JEOC)
  {
  //It clear JEOC flag
    ADC2->SR = ~(u32)ADC_FLAG_JEOC;
  }
  if((ADC3->SR & ADC_FLAG_JEOC) == ADC_FLAG_JEOC)
  {
    //It clear JEOC flag
    ADC3->SR = ~(u32)ADC_FLAG_JEOC;
      
    if (1)//(SVPWMEOCEvent())
    {
         
   	    pwm[2]= TIM8->CCR1; //take the pwmvalue 
		if (pwm[2]==0) 	pwm[2]=TIM8->CCR2; //take the pwmvalue

		pwm[3]=TIM8->CCR3; //take the pwmvalue
		if (pwm[3]==0) 	pwm[3]=TIM8->CCR4; //take the pwmvalue
		
		if (pwm[2]>deadtime)
		{
		Cur[2]=(Cur[2]>>1)+((ADC_GetInjectedConversionValue(ADC3, ADC_InjectedChannel_3)-hCurOffset[2])>>1) ;
//		uhCur[2]=(int16_t)(((pwm[2]-deadtime)*Cur[2])/(3360-deadtime<<1)); //PWM_PERIOD-2*deadtime  6720-168 =
	 	uhCur[2]=Cur[2];
		}
		else  uhCur[2]=0;

		if (pwm[3]>deadtime)
		{
		Cur[3]=(Cur[3]>>1)+((ADC_GetInjectedConversionValue(ADC3, ADC_InjectedChannel_2)-hCurOffset[3])>>1);
//	uhCur[3]=(int16_t)(((pwm[3]-deadtime)*Cur[3])/(3360-deadtime<<1)); //PWM_PERIOD-2*deadtime  6720-168 =
  uhCur[3]=Cur[3];
		}
		else  uhCur[3]=0;

    }

  }
	//if(ADC_GetITStatus(ADC1, ADC_IT_AWD) == SET)
    //ADC_ClearFlag(ADC1, ADC_FLAG_AWD);
}

extern uint16_t hal_get_current(uint16_t channel)
{
	return	uhCur[channel];
}
#endif
