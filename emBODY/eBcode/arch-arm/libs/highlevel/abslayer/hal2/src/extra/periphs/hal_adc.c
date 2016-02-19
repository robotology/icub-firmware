/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Maggiali, Davide Pollarolo
 * email:   marco.maggiali@iit.it, davide.pollarolo@iit.it
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


/* @file        hal_adc.c
	@brief      This file implements internal implementation of the hal ADC peripheral.
	@author     marco.maggiali@iit.it, davide.pollarolo@iit.it
    @date       26/03/2013
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"
// - middleware interface: contains hl, stm32 etc. --------------------------------------------------------------------
#include "hal_middleware_interface.h"

#ifdef HAL_USE_ADC

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_heap.h"
#include "hal_brdcfg.h"
#include "hl_bits.h"
#include "hl_core.h" //stm32 libraries
#include "hl_adc.h"

#include "hal_motor.h" 

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_adc.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_adc_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
#define HAL_adc_id2index(t)              ((uint8_t)((t)))
#define HAL_adc_id2stmADC(p)             (s_hal_adc_stmADCmap[HAL_adc_id2index(p)])

//#define ADC1                   ADC1
#define ADC1_CLK                 RCC_APB2Periph_ADC1	//ADCCLK= 42MHz/2 21/    
#define DMA_CHANNEL0             DMA_Channel_0
#define DMA_STREAM0              DMA2_Stream0
//#define ADC1_DR_ADDRESS          ((uint32_t)0x4001204C)	//ADC1BASE +0x4C
//#define ADC1_CHANNEL_GPIO_CLK    RCC_AHB1Periph_GPIOA
								
//#define ADC2                   ADC2
#define ADC2_CLK                 RCC_APB2Periph_ADC2
#define DMA_CHANNEL1             DMA_Channel_1
#define DMA_STREAM1              DMA2_Stream2
//#define ADC2_DR_ADDRESS          ((uint32_t)0x4001214C)

//#define ADC3                   ADC3
#define ADC3_CLK                 RCC_APB2Periph_ADC3
#define DMA_CHANNEL2             DMA_Channel_2
#define DMA_STREAM2              DMA2_Stream1
//#define ADC3_DR_ADDRESS          ((uint32_t)0x4001224C)

//#define ADC_CDR_ADDRESS          ((uint32_t)0x40012308)

#define SAMPLING_TIME_CK_ADC2       ADC_SampleTime_480Cycles
#define SAMPLING_TIME_CK 		    ADC_SampleTime_15Cycles 
#define NB_CALIBRATION_CONVERSIONS  32
#define ADC_CHANNEL_RESOLUTION      4096

#define VOLTAGE_FULLSCALE           (float) 3.3
#define CURRENT_FULLSCALE           5000

#define AN_REDUCTION_FACTOR         (float) (3.0/5.0)
#define CUR_REDUCTION_FACTOR        (float) (3.2/5.0)
#define TVAUX_REDUCTION_FACTOR      (float) (3.0/5.0)
#define TVIN_REDUCTION_FACTOR       (float) (1.0/21.0)    
    
// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_adc_cfg_t hal_adc_cfg_default =
{ 
    .init_mode              = hal_adc_indipendent,   
    .simult_adc             = NULL,                   
    .numberof_cfg_channels  = 0,  
    .adc_channels           = NULL,   
    .dma_cfg                = NULL             
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    hal_adc_cfg_t       config;
    uint16_t            ADCConvertedValues[hal_adc_max_channels];
} hal_adc_internal_item_t;

typedef struct
{
    uint32_t                            inittedmask;
    hal_adc_internal_item_t*            items[hal_adc_number];   
} hal_adc_theinternals_t;



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

static ADC_TypeDef* const s_hal_adc_stmADCmap[] = { ADC1, ADC2, ADC3 };

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_adc_none_supported_is(void);
    
static hal_boolval_t s_hal_adc_supported_is(hal_adc_t id);
static void s_hal_adc_initted_set(hal_adc_t id);
static void s_hal_adc_initted_reset(hal_adc_t id);
static hal_boolval_t s_hal_adc_initted_is(hal_adc_t id);

static void s_hal_adc_current_OffsetCalibration(void);
static void s_hal_adc_current_OffsetCalibration_old(void);
static void s_hal_adc_current_StartInjectedConv(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_adc_theinternals_t s_hal_adc_theinternals =
{
    .inittedmask        = 0,
    .items              = { NULL }   
};

// it must be defined in order to use hl_spi.
const hl_adc_mapping_t* hl_adc_map = NULL;

//converted data containers
// RAW data coming from ADC channels is 12bit unsigned
static uint16_t uhADCConvertedValue[12];
static uint16_t uhADC2ConvertedValue[3];
static uint16_t AnalogMotorsInput[8]; // = {0}; should be 0 anyway

static uint16_t uhAN1 = 0;
static uint16_t uhAN2 = 0;
static uint16_t uhAN3 = 0;
static uint16_t uhAN4 = 0;
static uint16_t uhTVIN = 0;
static uint16_t uhTVAUX = 0;

static uint16_t uhCur[4] = {0,0,0,0};
static int16_t  hCurOffset[4]= {0,0,0,0};

static uint32_t uwVBATVoltage = 0;

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
//Init with DMA (Regular Conversions, not Injected using Timers)
extern hal_result_t hal_adc_dma_init()	   
{
    
    if(hal_true == s_hal_adc_none_supported_is())
    {
        return(hal_res_NOK_generic);
    }
    
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  DMA_InitTypeDef       DMA_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;

  /* Enable ADC1, ADC2 and ADC3, DMA and GPIO  ****************************************/ 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  
  RCC_APB2PeriphClockCmd(ADC1_CLK, ENABLE);
  RCC_APB2PeriphClockCmd(ADC2_CLK, ENABLE);
  RCC_APB2PeriphClockCmd(ADC3_CLK, ENABLE);
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOF, ENABLE);

  // ADC
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
//GPIO_StructInit(&GPIO_InitStructure);

  // Pin associated to which ADC?? --> some of them are in common
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
  // Common initalization structure
  ADC_CommonInitStructure.ADC_Mode = ADC_TripleMode_RegSimult;//ADC_DualMode_RegSimult;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles ;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* ADC1, ADC2, ADC3 Init ****************************************************************/
  // Single initialization for each ADC
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode =ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  // ExternalTrigger is disabled here or not?
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 4;
  ADC_Init(ADC1, &ADC_InitStructure);	
  ADC_Init(ADC2, &ADC_InitStructure);
  ADC_Init(ADC3, &ADC_InitStructure);

  // Regular channels for all the ADC
  /* ADC1 regular channel3 4 5 and 16 configuration *************************************/
  ADC_RegularChannelConfig(ADC1, ADC_Channel_3 , 1, SAMPLING_TIME_CK);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_4 , 2, SAMPLING_TIME_CK);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_5 , 3, SAMPLING_TIME_CK);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_16 , 4, SAMPLING_TIME_CK); //DUMMY

  /* ADC2 regular channel8 9 and 10 configuration *************************************/
  ADC_RegularChannelConfig(ADC2, ADC_Channel_8 , 1, SAMPLING_TIME_CK);
  ADC_RegularChannelConfig(ADC2, ADC_Channel_9 , 2, SAMPLING_TIME_CK);
  ADC_RegularChannelConfig(ADC2, ADC_Channel_10, 3, SAMPLING_TIME_CK);
  //ADC_RegularChannelConfig(ADC2, ADC_Channel_13, 4, SAMPLING_TIME_CK); //Temp Sensor
  
  /* Enable TSensor and set it as the 4th channel of ADC2 */
  ADC_TempSensorVrefintCmd(ENABLE);
  ADC_RegularChannelConfig(ADC2, ADC_Channel_TempSensor, 4, SAMPLING_TIME_CK);

  /* ADC3 regular channel14 15  6 and 7 configuration *************************************/
  ADC_RegularChannelConfig(ADC3, ADC_Channel_14 ,   1, SAMPLING_TIME_CK);
  ADC_RegularChannelConfig(ADC3, ADC_Channel_15 ,   2, SAMPLING_TIME_CK);
  ADC_RegularChannelConfig(ADC3, ADC_Channel_6 ,    3, SAMPLING_TIME_CK);
  ADC_RegularChannelConfig(ADC3, ADC_Channel_7 ,    4, SAMPLING_TIME_CK);
           
  /* Enable VBAT channel */
  //ADC_VBATCmd(ENABLE);

//ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
  /* Enable Multimode (more than one ADC) DMA request after last transfer (Single-ADC mode) */
  ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);
  /* Enable ADC1 DMA since ADC1 is the Master*/
//ADC_DMACmd(ADC1, ENABLE); 
  /* Enable ADC */
  ADC_Cmd(ADC1, ENABLE);
  ADC_Cmd(ADC2, ENABLE);
  ADC_Cmd(ADC3, ENABLE);

  //why only ADC1?
  ADC_SoftwareStartConv(ADC1);
  //ADC_SoftwareStartConv(ADC2);
  //ADC_SoftwareStartConv(ADC3);
  return hal_res_OK;
}

//Init with Injected Conversions (must be paired with the dc_motorctl initialization)
// here the conversions are triggered with TIMER1, which is initialized only in hal_dc_motorctl.c
extern hal_result_t hal_adc_ADC1_ADC3_current_init()	   
{
    if(hal_true == s_hal_adc_none_supported_is())
    {
        return(hal_res_NOK_generic);
    }    
    
  ADC_InitTypeDef       ADC_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable ADC1, ADC2, ADC3, DMA and GPIO clocks  ****************************************/ 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  
  RCC_APB2PeriphClockCmd(ADC1_CLK, ENABLE);
  //RCC_APB2PeriphClockCmd(ADC2_CLK, ENABLE);
  RCC_APB2PeriphClockCmd(ADC3_CLK, ENABLE);
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE);
  //RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE);
  //RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOF, ENABLE);

  //ADC
  /* Configure ADC Channels pin as analog input ******************************/
  // Here I'm interested only in the current signals for the 4 motors
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
  //ADC_DeInit();

  /* ADC Common Init **********************************************************/
  //ADC_CommonInitStructure.ADC_Mode =ADC_TripleMode_InjecSimult;//ADC_DualMode_RegSimult;
  //ADC_CommonInitStructure.ADC_Mode = ADC_DualMode_InjecSimult;
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* ADC1,ADC3 Init ------------------------------------------------------*/
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Left;
  ADC_InitStructure.ADC_NbrOfConversion = 3; //the first is a dummy one --> we set it to 3 cause it's the upper bound 
  ADC_Init(ADC1, &ADC_InitStructure);
  //ADC_Init(ADC2, &ADC_InitStructure);
  ADC_Init(ADC3, &ADC_InitStructure);
  
  /* ADC1 Injected group of conversions end interrupt disabling */
  // ADC_IT_JEOC --> specifies the interrupt source for Injected channel, end of conversion event.
  ADC_ITConfig(ADC1, ADC_IT_JEOC, DISABLE);
  ADC_InjectedSequencerLengthConfig(ADC1,3);
  /* ADC1 injected channel3 4 and 5 configuration *************************************/
  ADC_InjectedChannelConfig(ADC1, ADC_Channel_3 , 1, SAMPLING_TIME_CK);//CUR1
  ADC_InjectedChannelConfig(ADC1, ADC_Channel_3 , 3, SAMPLING_TIME_CK);//CUR1
  ADC_InjectedChannelConfig(ADC1, ADC_Channel_4 , 2, SAMPLING_TIME_CK);//CUR2
  
  /* ADC2 injected channel8 and 9 configuration *************************************/
  /*
  ADC_InjectedChannelConfig(ADC2, ADC_Channel_8 , 1, SAMPLING_TIME_CK);//AN1
  ADC_InjectedChannelConfig(ADC2, ADC_Channel_9 , 2, SAMPLING_TIME_CK);//AN2
  */
  
  ADC_InjectedSequencerLengthConfig(ADC3,3);	
  /* ADC3 injected channel 6 and 7 configuration *************************************/
  ADC_InjectedChannelConfig(ADC3, ADC_Channel_6 , 1, SAMPLING_TIME_CK);//CUR3
  ADC_InjectedChannelConfig(ADC3, ADC_Channel_6 , 3, SAMPLING_TIME_CK);//CUR3
  ADC_InjectedChannelConfig(ADC3, ADC_Channel_7 , 2, SAMPLING_TIME_CK);//CUR4
  
  /* Enable ADCs */
  ADC_Cmd(ADC1, ENABLE);
  //ADC_Cmd(ADC2, ENABLE);
  ADC_Cmd(ADC3, ENABLE);

  s_hal_adc_current_OffsetCalibration();

  return hal_res_OK;
}

extern hal_result_t hal_adc_dma_common_init()
{
    if(hal_true == s_hal_adc_none_supported_is())
    {
        return(hal_res_NOK_generic);
    }    
    
  //if (initialized == hal_false)
  //{
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  DMA_InitTypeDef       DMA_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;
    
  //Common initialization stuff
  
  //Enable the periph clocks
  RCC_APB2PeriphClockCmd(ADC1_CLK, ENABLE);
  RCC_APB2PeriphClockCmd(ADC2_CLK, ENABLE);
  RCC_APB2PeriphClockCmd(ADC3_CLK, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOF, ENABLE);
  
  //DMA 
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
 
  // GPIO pin INIT
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
  
  // ADC common init
  ADC_CommonInitStructure.ADC_Mode = ADC_TripleMode_RegSimult;//ADC_DualMode_RegSimult;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles ;
  ADC_CommonInit(&ADC_CommonInitStructure);
  
  //Set the flag
//    initialized = hal_true;
  return hal_res_OK;
// }
  return hal_res_NOK_generic;
    
}
extern hal_result_t hal_adc_dma_single_init(hal_adc_t adc)
{
    
    if(hal_true == s_hal_adc_none_supported_is())
    {
        return(hal_res_NOK_generic);
    }
    
  ADC_InitTypeDef       ADC_InitStructure;
    
  switch (adc)
  {
      case hal_adc1:
      {
        //RCC_APB2PeriphClockCmd(ADC1_CLK, ENABLE);
      
        ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
        ADC_InitStructure.ADC_ScanConvMode =ENABLE;
        ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
        ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
        ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        ADC_InitStructure.ADC_NbrOfConversion = 4;
        ADC_Init(ADC1, &ADC_InitStructure);
      
        ADC_RegularChannelConfig(ADC1, ADC_Channel_3 , 1, SAMPLING_TIME_CK);
        ADC_RegularChannelConfig(ADC1, ADC_Channel_4 , 2, SAMPLING_TIME_CK);
        ADC_RegularChannelConfig(ADC1, ADC_Channel_5 , 3, SAMPLING_TIME_CK);
        ADC_RegularChannelConfig(ADC1, ADC_Channel_16 , 4, SAMPLING_TIME_CK); //DUMMY
      
        ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);
        ADC_Cmd(ADC1, ENABLE);
        ADC_Cmd(ADC2, ENABLE);
        ADC_Cmd(ADC3, ENABLE);
          
        ADC_SoftwareStartConv(ADC1);
        break;
      }
      case hal_adc2:
      {
        //RCC_APB2PeriphClockCmd(ADC2_CLK, ENABLE);
        ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
        ADC_InitStructure.ADC_ScanConvMode =ENABLE;
        ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
        ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
        ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        ADC_InitStructure.ADC_NbrOfConversion = 4;	
        ADC_Init(ADC2, &ADC_InitStructure);
          
        ADC_RegularChannelConfig(ADC2, ADC_Channel_8 , 1, SAMPLING_TIME_CK);
        ADC_RegularChannelConfig(ADC2, ADC_Channel_9 , 2, SAMPLING_TIME_CK);
        ADC_RegularChannelConfig(ADC2, ADC_Channel_10, 3, SAMPLING_TIME_CK);
          
        ADC_TempSensorVrefintCmd(ENABLE);
        ADC_RegularChannelConfig(ADC2, ADC_Channel_TempSensor, 4, SAMPLING_TIME_CK);
        
        //ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);
        ADC_Cmd(ADC2, ENABLE);
        break;
       }
      case hal_adc3:
      {
        //RCC_APB2PeriphClockCmd(ADC3_CLK, ENABLE);
        ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
        ADC_InitStructure.ADC_ScanConvMode =ENABLE;
        ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
        ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
        ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        ADC_InitStructure.ADC_NbrOfConversion = 4;	
        ADC_Init(ADC3, &ADC_InitStructure);
          
        ADC_RegularChannelConfig(ADC3, ADC_Channel_14 , 1, SAMPLING_TIME_CK);
        ADC_RegularChannelConfig(ADC3, ADC_Channel_15 , 2, SAMPLING_TIME_CK);
        ADC_RegularChannelConfig(ADC3, ADC_Channel_6 , 3, SAMPLING_TIME_CK);
        ADC_RegularChannelConfig(ADC3, ADC_Channel_7 , 4, SAMPLING_TIME_CK);
        
        //ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);
        ADC_Cmd(ADC3, ENABLE);
        break;
       } 
  }      
  
return hal_res_OK;
    
}


extern hal_result_t hal_adc_common_structure_init()
{
    if(hal_true == s_hal_adc_none_supported_is())
    {
        return(hal_res_NOK_generic);
    }    
    
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  
  //ADC common init
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//ADC_DualMode_RegSimult; //ADC_TripleMode_RegSimult
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles ;
  ADC_CommonInit(&ADC_CommonInitStructure);
  
  return hal_res_OK;
}

extern hal_result_t hal_adc_dma_init_ADC1_ADC3_hall_sensor_current ()
{
    
    if(hal_true == s_hal_adc_none_supported_is())
    {
        return(hal_res_NOK_generic);
    }
    
      ADC_InitTypeDef       ADC_InitStructure;
      ADC_CommonInitTypeDef ADC_CommonInitStructure;
      DMA_InitTypeDef       DMA_InitStructure;
      GPIO_InitTypeDef      GPIO_InitStructure;

      /* Enable ADC1 and ADC3, DMA2 and GPIO  ****************************************/ 
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  
      RCC_APB2PeriphClockCmd(ADC1_CLK, ENABLE);
      RCC_APB2PeriphClockCmd(ADC3_CLK, ENABLE);
      RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE);
      RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE);
      RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOF, ENABLE);

      // ADC
      /* DMA2 Stream0 channel0 configuration for ADC1 **************************************/
      DMA_DeInit(DMA_STREAM0);
      DMA_InitStructure.DMA_Channel = DMA_CHANNEL0;
      DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_ADDRESS; 
      DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) &AnalogMotorsInput;//(uint32_t) --> address is defined in 32bit
      DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
      DMA_InitStructure.DMA_BufferSize = 4;
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
      
      /* DMA2 Stream2 channel2 configuration for ADC3 **************************************/
      DMA_DeInit(DMA_STREAM2);
      DMA_InitStructure.DMA_Channel = DMA_CHANNEL2;
      DMA_InitStructure.DMA_PeripheralBaseAddr = ADC3_DR_ADDRESS;
      DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)  &AnalogMotorsInput[4];
      DMA_Init(DMA_STREAM2, &DMA_InitStructure);
      DMA_Cmd(DMA_STREAM2, ENABLE);
         
      /* Configure ADC Channels pin as analog input ******************************/
      
      // Pin associated to analog channels   
      GPIO_StructInit(&GPIO_InitStructure);
      //AN1-AN2
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;	  
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
      GPIO_Init(GPIOB, &GPIO_InitStructure);
   
      //CUR1-CUR2
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 ;	  
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
      GPIO_Init(GPIOA, &GPIO_InitStructure);
      
      //AN3-AN4-CUR3-CUR4
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9; 	  
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
      GPIO_Init(GPIOF, &GPIO_InitStructure);
       
      /* ADC1, ADC3 Init ****************************************************************/
      // Single initialization for each ADC
      ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
      ADC_InitStructure.ADC_ScanConvMode = ENABLE;
      ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
      ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; // seems disabled...
      ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
      ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
      ADC_InitStructure.ADC_NbrOfConversion = 4;
      ADC_Init(ADC1, &ADC_InitStructure);	
      ADC_Init(ADC3, &ADC_InitStructure);

      // Regular channels for all the ADC
      /* ADC1 regular channel8,9,3,4 configuration *************************************/
      ADC_RegularChannelConfig(ADC1, ADC_Channel_8 , 1, SAMPLING_TIME_CK);
      ADC_RegularChannelConfig(ADC1, ADC_Channel_3 , 2, SAMPLING_TIME_CK);
      ADC_RegularChannelConfig(ADC1, ADC_Channel_9 , 3, SAMPLING_TIME_CK);
      ADC_RegularChannelConfig(ADC1, ADC_Channel_4 , 4, SAMPLING_TIME_CK);
      //ADC_RegularChannelConfig(ADC1, ADC_Channel_3 , 4, SAMPLING_TIME_CK); //dummy conversion needed?

      /* ADC3 regular channel14,15,6,7 configuration *************************************/
      ADC_RegularChannelConfig(ADC3, ADC_Channel_14 ,   1, SAMPLING_TIME_CK);
      ADC_RegularChannelConfig(ADC3, ADC_Channel_6 ,    2, SAMPLING_TIME_CK);
      ADC_RegularChannelConfig(ADC3, ADC_Channel_15 ,   3, SAMPLING_TIME_CK);
      ADC_RegularChannelConfig(ADC3, ADC_Channel_7 ,    4, SAMPLING_TIME_CK);
      //ADC_RegularChannelConfig(ADC3, ADC_Channel_6 ,    4, SAMPLING_TIME_CK); //dummy conversion needed?
           
      /* Enable DMA request after last transfer (Single-ADC mode) */
      ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
      ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);
    
      /* Enable ADC1-ADC3 DMA */
      ADC_DMACmd(ADC1, ENABLE);
      ADC_DMACmd(ADC3, ENABLE);
      
      /* Enable ADC */
      ADC_Cmd(ADC1, ENABLE);
      ADC_Cmd(ADC3, ENABLE);

      /* Start first conversions, the others will be done periodically */
      ADC_SoftwareStartConv(ADC1);
      ADC_SoftwareStartConv(ADC3);
      
      //start the calibration of currents (cause we want to have values around zero everytime we switch on the board)
      s_hal_adc_current_OffsetCalibration();
      
      return hal_res_OK;  
}
/*
Vale: tvin ==> input voltage. (Voltage supply to the board, from 12V to 50V. Usually 12V.
      tvaux ==> auxiliary voltage (5V) 
      temperature is inetrnal temp of micro.
*/
extern hal_result_t hal_adc_dma_init_ADC2_tvaux_tvin_temperature ()
{
    
    if(hal_true == s_hal_adc_none_supported_is())
    {
        return(hal_res_NOK_generic);
    }
    
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  DMA_InitTypeDef       DMA_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;

  /* Enable ADC2 ****************************************/ 
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_DMA2, ENABLE);  
  RCC_APB2PeriphClockCmd( ADC2_CLK, ENABLE);
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE);
    
  // ADC
  /* DMA2 Stream0 channel0  configuration **************************************/
  DMA_DeInit(DMA_STREAM1);
  DMA_InitStructure.DMA_Channel = DMA_CHANNEL1; // | DMA_CHANNEL1;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC2_DR_ADDRESS;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)  &uhADC2ConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = 3;
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
  DMA_Init(DMA_STREAM1, &DMA_InitStructure);
  DMA_Cmd(DMA_STREAM1, ENABLE);

  /* Configure ADC Channels pin as analog input ******************************/
  /* Pin associated to:
    - TV AUX (PA5)
    - TV IN (PC0)
  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ;	  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;	  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* ADC2 Init ****************************************************************/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; // seems disabled...
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 3;	
  ADC_Init(ADC2, &ADC_InitStructure);

  /* ADC2 regular channel5 10 configuration *************************************/
  /* Channels associated to:
    - TV AUX (PA5)
    - TV IN (PC0)
  */
  ADC_RegularChannelConfig(ADC2, ADC_Channel_5 , 1, SAMPLING_TIME_CK_ADC2);
  ADC_RegularChannelConfig(ADC2, ADC_Channel_10 , 2, SAMPLING_TIME_CK_ADC2);
         
  /* Enable TSensor and set it as the 3th channel in rank of ADC2 */
  ADC_TempSensorVrefintCmd(ENABLE);
  ADC_RegularChannelConfig(ADC2, ADC_Channel_TempSensor, 3, SAMPLING_TIME_CK_ADC2);
    
  ADC_DMARequestAfterLastTransferCmd(ADC2, ENABLE);
  
  /* Enable ADC2 DMA */
  ADC_DMACmd(ADC2, ENABLE);
 
  /* Enable ADC */
  ADC_Cmd(ADC2, ENABLE);
 
  // Start the conversion, the others will be done periodically
  ADC_SoftwareStartConv(ADC2);
  return hal_res_OK;
}

extern uint16_t hal_adc_get(uint16_t ADC_TYPE, uint16_t channel)
{   
    if ((ADC_TYPE < 1) || (ADC_TYPE > 3) || (channel > hal_adc_max_channels))
        return 0;
    
    //data saved here using DMA
	return	uhADCConvertedValue[((ADC_TYPE-1))+(channel*3)];
}

extern uint16_t hal_adc_get_tvaux_tvin_temperature_raw(uint16_t channel)
{    
	if (channel > 2)
        return 0;
      
    return	uhADC2ConvertedValue[channel];
}

/*
Vale: tvin ==> input voltage. (Voltage supply to the board, from 12V to 50V. Usually 12V.
      tvaux ==> auxiliary voltage (5V) 
*/

extern uint32_t hal_adc_get_tvaux_tvin_mV(uint16_t channel)
{    
	if (channel > 1)
        return 0;
   
    uint32_t result = uhADC2ConvertedValue[channel];
    
    //rescaling from 0mV to 3300mV and applying the reduction factor
    switch (channel)
    {
        case 0:
            result = (1000 * (1.0/TVAUX_REDUCTION_FACTOR)  * VOLTAGE_FULLSCALE * result)  / ADC_CHANNEL_RESOLUTION;
            break;
        case 1:
            result = (1000 * (1.0/TVIN_REDUCTION_FACTOR)   * VOLTAGE_FULLSCALE * result) / ADC_CHANNEL_RESOLUTION;
            break;
    }
    
    return	result;
}

extern uint32_t hal_adc_get_supplyVoltage_mV(void)
{    
    return(hal_adc_get_tvaux_tvin_mV(1));
}

extern int16_t hal_adc_get_current(uint16_t channel)
{
    if (channel > 3)
        return 0;
	return	uhCur[channel];
}

extern uint16_t hal_adc_get_hall_sensor_analog_input_raw(uint8_t motor)
{
    if(hal_false == hal_motor_supported_is((hal_motor_t)motor))
    {
        return(hal_NA16);
    }    
    
//    if (motor > 3)
//        return 0;
    
    //swap motor 0 and motor 1
    if ((motor == 0) || (motor == 1)) motor = !motor;
    
	return	AnalogMotorsInput[motor*2];
}

extern hal_dma_voltage_t hal_adc_get_hall_sensor_analog_input_mV(uint8_t motor)
{
    if(hal_false == hal_motor_supported_is((hal_motor_t)motor))
    {
        return(hal_NA32);
    }
    
    //swap motor 0 and motor 1
    if ((motor == 0) || (motor == 1)) motor = !motor;
    
    //rescaling from 0mV to 3300mV and applying the reduction factor
    uint32_t result = (1000 * (1.0/AN_REDUCTION_FACTOR)  * VOLTAGE_FULLSCALE * AnalogMotorsInput[motor*2])  / ADC_CHANNEL_RESOLUTION;
  
	return	result;
}

extern uint16_t hal_adc_get_current_motor_raw(uint8_t motor)
{
    // marco.accame: best thing is to verify vs initialisation of the assocaited adc peripheral, but for now we accept this check.
    if(hal_false == hal_motor_supported_is((hal_motor_t)motor))
    {
        return(hal_NA16);
    }
    
    //swap motor 0 and motor 1
    if ((motor == 0) || (motor == 1)) motor = !motor;
    
	return	(uint16_t)(AnalogMotorsInput[motor*2 + 1] - hCurOffset[motor]);
}

extern hal_dma_current_t hal_adc_get_current_motor_mA(uint8_t motor)
{
    // marco.accame: best thing is to verify vs initialisation of the assocaited adc peripheral, but for now we accept this check.
    if(hal_false == hal_motor_supported_is((hal_motor_t)motor))
    {
        return(hal_NA16);
    }

    //swap motor 0 and motor 1
    if ((motor == 0) || (motor == 1)) motor = !motor;
    
    //rescaling from -5000mA to 5000mA and applying the reduction factor
    int16_t result = ((1.0/CUR_REDUCTION_FACTOR) * CURRENT_FULLSCALE * (AnalogMotorsInput[motor*2 + 1] - hCurOffset[motor] - ADC_CHANNEL_RESOLUTION/2))/(ADC_CHANNEL_RESOLUTION/2);
    
	return	result;
}

/*-------------------NEW APIs BEGIN-------------------------------------------*/

extern hal_result_t hal_adc_init(hal_adc_t id, const hal_adc_cfg_t *cfg)
{
    //check if supported & initted
    hal_adc_internal_item_t* intitem = s_hal_adc_theinternals.items[HAL_adc_id2index(id)];
    hal_result_t res = hal_res_NOK_generic;

	//If adc not supported return not OK
    if(hal_false == s_hal_adc_supported_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
	// Set the default config if cfg is NULL
    if(NULL == cfg)
    {
        cfg = &hal_adc_cfg_default;
    }
    
	// Check initialization
    if(hal_true == s_hal_adc_initted_is(id))
    {
		//Memory comparison
        if(0 == memcmp(cfg, &intitem->config, sizeof(hal_adc_cfg_t)))
        {   // ok only if the previously used config is the same as the current one
            return(hal_res_OK);
        }
        else
        {
            return(hal_res_NOK_generic);
        }
    }   
    
    // if it does not have ram yet, then attempt to allocate it.
    if(NULL == intitem)
    {
        intitem = s_hal_adc_theinternals.items[HAL_adc_id2index(id)] = hal_heap_new(sizeof(hal_adc_internal_item_t));
        // minimal initialisation of the internal item
        // nothing to init.      
    }
	// Copy configuration in a protected memory
    memcpy(&intitem->config, cfg, sizeof(hal_adc_cfg_t));  
    
    ADC_InitTypeDef       ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    DMA_InitTypeDef       DMA_InitStructure;
    
    switch (intitem->config.init_mode)
    {
        case hal_adc_indipendent:
  
          // should use hal_dma_init
          /* DMA2 Stream2 channel1  configuration **************************************/
          RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_DMA2, ENABLE);  
          DMA_DeInit(DMA_STREAM1);
          DMA_InitStructure.DMA_Channel = DMA_CHANNEL1; // | DMA_CHANNEL1;  
          DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC2_DR_ADDRESS; // which address? --> now is set the ADC2 Data Register addr
          DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) &uhADC2ConvertedValue;
          DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
          DMA_InitStructure.DMA_BufferSize = 3;
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
          DMA_Init(DMA_STREAM1, &DMA_InitStructure);
          DMA_Cmd(DMA_STREAM1, ENABLE);

          /* Configure ADC Channels pin as analog input ******************************/
          /* Pin associated to:
            - TV AUX (PA5)
            - TV IN (PC0)
          */
          hal_gpio_cfg_t gpio_cfg;
          gpio_cfg.altcfg = NULL;
          gpio_cfg.dir = hal_gpio_dirAN;
          
          hal_gpio_t gpioVAUX;
          gpioVAUX.port = hal_gpio_portA;
          gpioVAUX.pin = hal_gpio_pin5;
          hal_gpio_init(gpioVAUX, &gpio_cfg);
          
          hal_gpio_t gpioVIN;
          gpioVIN.port = hal_gpio_portC;
          gpioVIN.pin = hal_gpio_pin0;
          hal_gpio_init(gpioVIN, &gpio_cfg);
          
          //should use hl_adc_init and hal_adc_enable
          RCC_APB2PeriphClockCmd( ADC2_CLK, ENABLE);
          /* ADC Common Init **********************************************************/
          // Common initalization structure
          ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//ADC_DualMode_RegSimult;
          ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
          ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; // must be enabled only for multi-mode
          ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
          ADC_CommonInit(&ADC_CommonInitStructure);

          /* ADC2 Init ****************************************************************/
          ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
          ADC_InitStructure.ADC_ScanConvMode = ENABLE;
          ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
          ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; // seems disabled...
          ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
          ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
          ADC_InitStructure.ADC_NbrOfConversion = 3;	
          ADC_Init(ADC2, &ADC_InitStructure);

          // should use hl_adc_set_regularchannels
          /* ADC2 regular channel5 10 configuration *************************************/
          /* Channels associated to:
            - TV AUX (PA5)
            - TV IN (PC0)
          */
          ADC_RegularChannelConfig(ADC2, ADC_Channel_5 , 1, SAMPLING_TIME_CK);
          ADC_RegularChannelConfig(ADC2, ADC_Channel_10 , 2, SAMPLING_TIME_CK);
                 
          /* Enable TSensor and set it as the 3th channel in rank of ADC2 */
          ADC_TempSensorVrefintCmd(ENABLE);
          ADC_RegularChannelConfig(ADC2, ADC_Channel_TempSensor, 3, SAMPLING_TIME_CK);
            
          ADC_DMARequestAfterLastTransferCmd(ADC2, ENABLE);
          
          /* Enable ADC2 DMA */
          ADC_DMACmd(ADC2, ENABLE);
         
          /* Enable ADC */
          ADC_Cmd(ADC2, ENABLE);
         
          // Start the conversion, the others will be done periodically
          ADC_SoftwareStartConv(ADC2);
        case hal_adc_dualmode_reg_simult:
            //.... handle the other cases
        
        default:
            break;
    }
    
    return hal_res_OK;
}

/*-------------------NEW APIs END-------------------------------------------*/


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_hal_adc_current_OffsetCalibration(void)
{
  uint8_t conv_index;
  static uint8_t attempts = 0;
  
  //I want EOC flag only at the end of the all the group of conversions, for both ADCs    
  ADC_EOCOnEachRegularChannelCmd(ADC1, DISABLE);
  ADC_EOCOnEachRegularChannelCmd(ADC3, DISABLE);    
  
  /* ADC Channel used for current reading are read 
     in order to get zero currents ADC values*/ 
  for(conv_index = 0; conv_index < NB_CALIBRATION_CONVERSIONS; conv_index++)
  {
    //while the conversions are not ended, wait..how to know that?
    //while ((DMA_GetFlagStatus(DMA_STREAM0, DMA_IT_TCIF0) == RESET) || (DMA_GetFlagStatus(DMA_STREAM2, DMA_IT_TCIF2) == RESET)) { };
    //while ((DMA_GetFIFOStatus(DMA_STREAM0) != DMA_FIFOStatus_Full ) ||  (DMA_GetFIFOStatus(DMA_STREAM0) !=  DMA_FIFOStatus_Full)) { };
      
    //don't really know if this is the right way to do that
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)) { attempts ++; if (attempts>=100) break;} 
    attempts = 0;
    // no more dividing directly by 32, because this was causing a loss of precision
    // now: - 1) dividing (32 times) by 2, adding the values every iteration - 2) Dividing by 16 before setting the final offset
    // this preserves precision and find the right offset
    hCurOffset[0] +=  AnalogMotorsInput[1] >> 1; //AnalogMotorsInput[i]/2
    hCurOffset[1] +=  AnalogMotorsInput[3] >> 1;
    
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
    
    while (!ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC)) { attempts ++; if (attempts>=100) break;}
    attempts = 0;
    
    hCurOffset[2] +=  AnalogMotorsInput[5] >> 1;
    hCurOffset[3] +=  AnalogMotorsInput[7] >> 1;
      
    ADC_ClearFlag(ADC3, ADC_FLAG_EOC);
  }
   
  hCurOffset[0] = (hCurOffset[0] >> 4) - ADC_CHANNEL_RESOLUTION/2; // offset is the difference between the initial values and saturationvalue/2 (which represents 0mA)
  hCurOffset[1] = (hCurOffset[1] >> 4) - ADC_CHANNEL_RESOLUTION/2;
  hCurOffset[2] = (hCurOffset[2] >> 4) - ADC_CHANNEL_RESOLUTION/2;
  hCurOffset[3] = (hCurOffset[3] >> 4) - ADC_CHANNEL_RESOLUTION/2;

}

static void s_hal_adc_current_OffsetCalibration_old(void)
{
  static uint16_t bIndex;
  
  /* ADC1 Injected group of conversions end interrupt disabling */
  ADC_ITConfig(ADC1, ADC_IT_JEOC, DISABLE);
  //ADC_ITConfig(ADC2, ADC_IT_JEOC, DISABLE);
  ADC_ITConfig(ADC3, ADC_IT_JEOC, DISABLE);

  /* ADC1 Injected conversions trigger is given by software and enabled */
  /*    
  //ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_TRGO);   
  //ADC_ExternalTrigInjectedConvCmd(ADC1,ENABLE);  
  */
    
  /* ADC1 Injected conversions configuration */   
  /* Clear the ADC1 JEOC pending flag */
  ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);  
  ADC_SoftwareStartInjectedConv(ADC1);
    
  // should start also ADC3 conversion to compute the offset for CUR3 and CUR4 
  ADC_ClearFlag(ADC3, ADC_FLAG_JEOC); // ---> added
  ADC_SoftwareStartInjectedConv(ADC3); // ---> added
	
  /* ADC Channel used for current reading are read 
     in order to get zero currents ADC values*/ 
  for(bIndex=0; bIndex <NB_CALIBRATION_CONVERSIONS; bIndex++)
  {
    while((!ADC_GetFlagStatus(ADC1,ADC_FLAG_JEOC)) || (!ADC_GetFlagStatus(ADC3,ADC_FLAG_JEOC))) { } // ---> added
    
   	hCurOffset[0] += (ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_3)>>5);
    hCurOffset[1] += (ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_2)>>5);     
	hCurOffset[2] += (ADC_GetInjectedConversionValue(ADC3,ADC_InjectedChannel_3)>>5);
    hCurOffset[3] += (ADC_GetInjectedConversionValue(ADC3,ADC_InjectedChannel_2)>>5);    
     
    /* Clear the ADC1 JEOC pending flag */ //need to re-do this for every cycle because the interrupt routine is not active yet
    ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);    
    ADC_SoftwareStartInjectedConv(ADC1);
    
    ADC_ClearFlag(ADC3, ADC_FLAG_JEOC); // ---> added
    ADC_SoftwareStartInjectedConv(ADC3);// ---> added
  }
  s_hal_adc_current_StartInjectedConv();  
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
static void s_hal_adc_current_StartInjectedConv(void)
{
  //here is the setting of the trigger source
  /* ADC1 Injected conversions trigger is TIM1 TRGO */ 
  ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_TRGO); 
  ADC_ExternalTrigInjectedConvEdgeConfig(ADC1, ADC_ExternalTrigInjecConvEdge_Rising);   //ADC_ExternalTrigInjectedConvCmd(ADC3,ENABLE);
    
  ADC_ExternalTrigInjectedConvConfig(ADC3, ADC_ExternalTrigInjecConv_T1_TRGO);          //---> added
  ADC_ExternalTrigInjectedConvEdgeConfig(ADC3, ADC_ExternalTrigInjecConvEdge_Rising);   //---> added
 
  /* Bus voltage protection initialization - OPTIONAL*/
  /*    
  ADC_AnalogWatchdogCmd(ADC1,ADC_AnalogWatchdog_SingleInjecEnable);
  ADC_AnalogWatchdogSingleChannelConfig(ADC1,BUS_VOLT_FDBK_CHANNEL);
  ADC_AnalogWatchdogThresholdsConfig(ADC1, OVERVOLTAGE_THRESHOLD>>3,0x00);
  
  //ADC1 Injected group of conversions end and Analog Watchdog interrupts enabling
  ADC_ITConfig(ADC1, ADC_IT_JEOC | ADC_IT_AWD, ENABLE);
  */    
  /* ADC1 Injected group of conversions end interrupts enabling */
  // (only ADC1 cause we check all the values using 1 ISR)     
  ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
  ADC_ITConfig(ADC3, ADC_IT_JEOC, ENABLE);  //---> added
}
/*******************************************************************************
* Function Name  : ADC1_IRQHandler
* Description    : This function handles ADC1, ADC2 and ADC3 global interrupts requests. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
// is activated only inside hal_dc_motorctl.c
// it seems that this interrupt is really triggered only when a real motor is attached (the trigger source is activated in hal_dc_motorctl)
void ADC_IRQHandler(void)
{
  int32_t pwm[4]={0,0,0,0};
  int16_t deadtime= 0;//; MOTOR_DEADTIME+0;
  int16_t Cur[4]={0,0,0,0};	
  //if(ADC_GetITStatus(ADC1, ADC_IT_JEOC) == SET))
  
  // finished conversion for ADC1
  if((ADC1->SR & ADC_FLAG_JEOC) == ADC_FLAG_JEOC)
  {
    //It clear JEOC flag
    ADC1->SR = ~(u32)ADC_FLAG_JEOC;
      
    if (1)//(SVPWMEOCEvent())
    {
        //MCL_Calc_BusVolt();
	    //TIM_GetCapture1(TIM_TypeDef* TIMx);
        // Store the pwm values for the 4 motors
		pwm[0]= TIM1->CCR1; //take the pwmvalue 
		if (pwm[0]==0) 	pwm[0]=TIM1->CCR2; //take the pwmvalue

		pwm[1]= TIM1->CCR3; //take the pwmvalue
		if (pwm[1]==0) 	pwm[1]=TIM1->CCR4; //take the pwmvalue

		if (pwm[0]>deadtime)
		{
         // store the value inside the output array
		 Cur[0]=(Cur[0]>>1)+((ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_3)-hCurOffset[0])>>1) ;
//       uhCur[0]=(int16_t)(((pwm[0]-deadtime)*Cur[0])/(3360-deadtime<<1)); //PWM_PERIOD-2*deadtime  6720-168 =
         uhCur[0]=Cur[0];
		}
		else  uhCur[0]=0;

		if (pwm[1]>deadtime)
		{
        // store the value inside the output array
	    Cur[1]=(Cur[1]>>1)+((ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_2)-hCurOffset[1])>>1);
//   uhCur[1]=(int16_t)(((pwm[1]-deadtime)*Cur[1])/(3360-deadtime<<1)); //PWM_PERIOD-2*deadtime  6720-168 =
		uhCur[1]=Cur[1];
		}
		else  uhCur[1]=0;
    }
  }
  
  // finished conversion for ADC2
  if((ADC2->SR & ADC_FLAG_JEOC) == ADC_FLAG_JEOC)
  {
  //It clear JEOC flag
    ADC2->SR = ~(u32)ADC_FLAG_JEOC;
  }
  
  // finished conversion for ADC3
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


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------



static hal_boolval_t s_hal_adc_none_supported_is(void)
{
    if(0 == hal_adc__theboardconfig.supportedmask)
    {
        return(hal_true);
    }
    return(hal_false);
}


static hal_boolval_t s_hal_adc_supported_is(hal_adc_t id)
{
    return((hal_boolval_t)hl_bits_word_bitcheck(hal_adc__theboardconfig.supportedmask, HAL_adc_id2index(id)) );
}

static void s_hal_adc_initted_set(hal_adc_t id)
{
    hl_bits_word_bitset(&s_hal_adc_theinternals.inittedmask, HAL_adc_id2index(id));
}

static void s_hal_adc_initted_reset(hal_adc_t id)
{
    hl_bits_word_bitclear(&s_hal_adc_theinternals.inittedmask, HAL_adc_id2index(id));
}

static hal_boolval_t s_hal_adc_initted_is(hal_adc_t id)
{
    return((hal_boolval_t)hl_bits_word_bitcheck(s_hal_adc_theinternals.inittedmask, HAL_adc_id2index(id)));
}

#endif // HAL_USE_ADC


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

