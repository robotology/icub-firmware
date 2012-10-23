/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Marco Accame
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


/* @file     hal_stm32_spi4encoder.c
	@brief      This file implements communication between spi peripherals and encoders.
	@author     valentina.gaggero@iit.it
    @date       9/21/2010
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_SPI4ENCODER

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_stm32xx_include.h"
#include "hal_brdcfg.h"
#include "hal_stm32_base_hid.h" 
#include "hal_stm32_sys_hid.h" 

#include "hal_stm32_encoder_hid.h" 

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_spi4encoder.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------


#include "hal_stm32_spi4encoder_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_spi_port2index(p)       ((uint8_t)(p))
#define HAL_spi_mux2index(m)        ((uint8_t)(m))

#define SPI1_CLK               		 RCC_APB2Periph_SPI1
#define SPI1_GPIO_PORT               GPIOA
#define SPI1_GPIO_CLK                RCC_APB2Periph_GPIOA
#define SPI1_GPIO_PIN_SCK            GPIO_Pin_5
#define SPI1_GPIO_PIN_MISO           GPIO_Pin_6
#define SPI1_GPIO_PIN_MOSI           GPIO_Pin_7
#define SPI1_IT_IRQn                 SPI1_IRQn

//unused
#define SPI2_CLK               		 RCC_APB1Periph_SPI3
#define SPI2_GPIO_PORT               GPIOC
#define SPI2_GPIO_CLK                RCC_APB2Periph_GPIOC 
#define SPI2_GPIO_PIN_SCK            GPIO_Pin_10
#define SPI2_GPIO_PIN_MISO           GPIO_Pin_11
#define SPI2_GPIO_PIN_MOSI           GPIO_Pin_12
#define SPI2_IT_IRQn                 SPI3_IRQn



#define SPI3_CLK               		 RCC_APB1Periph_SPI3
#define SPI3_GPIO_PORT               GPIOC
#define SPI3_GPIO_CLK                RCC_APB2Periph_GPIOC 
#define SPI3_GPIO_PIN_SCK            GPIO_Pin_10
#define SPI3_GPIO_PIN_MISO           GPIO_Pin_11
#define SPI3_GPIO_PIN_MOSI           GPIO_Pin_12
#define SPI3_IT_IRQn                 SPI3_IRQn


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

// array of ptr to spi interface data.
extern hal_spi4encoder_interfdatastructure_t* hal_spi4encoder_hid_interfdata[hal_spi_ports_number] = {NULL, NULL, NULL}; 

// table for mapping stm32-peripheral into hal-periheral: it is mapped in rom
extern SPI_TypeDef * const hal_spi4encoder_interf_map[hal_spi_ports_number] = {SPI1, SPI2, SPI3};	



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_spi4encoder_supported_is(hal_spi_port_t port);
static void s_hal_spi4encoder_initted_set(hal_spi_port_t port);
static hal_boolval_t s_hal_spi4encoder_initted_is(hal_spi_port_t port);

static void s_hal_spi4encoder_RCC_conf(hal_spi_port_t port);
static void s_hal_spi4encoder_GPIO_conf(hal_spi_port_t port);
static void s_hal_spi4encoder_NVIC_conf(hal_spi_port_t port, hal_interrupt_priority_t priority);
static void s_hal_spi4encoder_encoder_read_isr(hal_spi_port_t port);
static void s_hal_spi4encoder_get_pospin(uint32_t pin, uint32_t *pos, uint8_t *is_highpin);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_spi4encoder_initted[hal_spi_ports_number] = { hal_false };


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern hal_result_t hal_spi4encoder_init(hal_spi_port_t port, hal_spi4encoder_cfg_t *cfg)
{

    SPI_InitTypeDef SPI_InitStructure;

    SPI_TypeDef * SPIx = hal_SPI4ENCODER_PERIPH_GET(port);


    if(hal_true != s_hal_spi4encoder_supported_is(port))
    {
        return(hal_res_NOK_unsupported);
    }


    if(NULL == hal_SPI4ENCODER_GET(port))
    {
  	    return(hal_res_NOK_generic);
    }

    if(NULL == cfg)
    {
        hal_base_hid_on_fatalerror(hal_fatalerror_missingconfiguration, "hal_spi4encoder_init() needs a cfg");
    }
    

    // i only init once.
    if(hal_true == s_hal_spi4encoder_initted_is(port))
    {
        return(hal_res_OK);
    } 
    else
    {
        s_hal_spi4encoder_initted_set(port);
    }


    hal_SPI4ENCODER_GET(port)->cbk_on_rx = cfg->callback_on_rx;
    hal_SPI4ENCODER_GET(port)->arg_cbk   = cfg->arg;

    
    // System clocks configuration 
    s_hal_spi4encoder_RCC_conf(port);
    
    // NVIC configuration 
    s_hal_spi4encoder_NVIC_conf(port, cfg->priority);
    
    // GPIO configuration
    s_hal_spi4encoder_GPIO_conf(port);
    
    //reset spi periph registers
    SPIx->CR1  = 0x0000;
    SPIx->CR2  = 0x0000;
    
    // SPI configuration 
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; 
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

    //in ordeer to read from encoder it is necassary use SPI_BaudRatePrescaler_64 or 	SPI_BaudRatePrescaler_32 only on SPI3.
    //since SPI1 is connected to APB2 bus(72MHz), while spi2 and 3 are connected to APB1(36MHz), it is necessary
    //to use differet prescaler in orter to have symmetrical behaviour.
    if(hal_spi_port1 == port)
    {
        SPI_InitStructure.SPI_BaudRatePrescaler = ((hal_spi_baudrate500kbps == cfg->baudrate) ? SPI_BaudRatePrescaler_128 : SPI_BaudRatePrescaler_64); 
    }
    else if(hal_spi_port3 == port)
    {
        SPI_InitStructure.SPI_BaudRatePrescaler = ((hal_spi_baudrate500kbps == cfg->baudrate) ? SPI_BaudRatePrescaler_64 : SPI_BaudRatePrescaler_32);
    }
    else
    {     //I should not get here because SPI2 is not supported with encoders.
          return(hal_res_NOK_unsupported);
    }
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 0x7; //Reset value
    SPI_Init(SPIx, &SPI_InitStructure);
    
    return(hal_res_OK);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----

void SPI1_IRQHandler(void)
{
	s_hal_spi4encoder_encoder_read_isr(hal_spi_port1);
}


void SPI2_IRQHandler(void)
{
	s_hal_spi4encoder_encoder_read_isr(hal_spi_port2);
}


void SPI3_IRQHandler(void)
{
	s_hal_spi4encoder_encoder_read_isr(hal_spi_port3);
}

// ---- isr of the module: end ------ 

extern uint32_t hal_spi4encoder_hid_getsize(const hal_cfg_t *cfg)
{
    uint32_t size = 0;
    
    if(0 != cfg->spi1_enable)
    {
        size += sizeof(hal_spi4encoder_interfdatastructure_t);
    }
    
    if(0 != cfg->spi2_enable)
    {
        size += sizeof(hal_spi4encoder_interfdatastructure_t);
    }

    if(0 != cfg->spi3_enable)
    {
        size += sizeof(hal_spi4encoder_interfdatastructure_t);
    }

    return(size);
}

extern hal_result_t hal_spi4encoder_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
{
    uint8_t *ram08 = (uint8_t*)memory;
    uint8_t i=0;

    // removed dependency from nzi data
    memset(s_hal_spi4encoder_initted, hal_false, sizeof(s_hal_spi4encoder_initted));
    for(i=0; i<hal_spi_ports_number; i++)
    {
        hal_spi4encoder_hid_interfdata[i] = NULL;
    }


    if((0 != cfg->spi1_enable) || (0 != cfg->spi2_enable) || (0 != cfg->spi3_enable))
    {
        if(NULL == memory)
        {
            hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_spi4encoder_hid_setmem(): memory missing");
            return(hal_res_NOK_generic);
        }
    }


    if(0 != cfg->spi1_enable)
    {
        hal_spi4encoder_hid_interfdata[0] = (hal_spi4encoder_interfdatastructure_t*)ram08;
        ram08 += sizeof(hal_spi4encoder_interfdatastructure_t);       
    }
    
    if(0 != cfg->spi2_enable)
    {
        hal_spi4encoder_hid_interfdata[1] = (hal_spi4encoder_interfdatastructure_t*)ram08;
        ram08 += sizeof(hal_spi4encoder_interfdatastructure_t);       
    }

    if(0 != cfg->spi3_enable)
    {
        hal_spi4encoder_hid_interfdata[2] = (hal_spi4encoder_interfdatastructure_t*)ram08;
        ram08 += sizeof(hal_spi4encoder_interfdatastructure_t);     
    }

    return(hal_res_OK);
}

/*
    Note: this function verify only busy flag because it is called when trasmitting and reading are already finished.
    If you use this function in different contest be sure to check trasmission and receiving falg.
    For more information see dataseet.
*/
extern void hal_spi4encoder_hid_disa(hal_spi_port_t port) //s_hal_spi_disa
{
    SPI_TypeDef * SPIx = hal_SPI4ENCODER_PERIPH_GET(port);

    while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET);

    hal_SPI4ENCODER_DISA(port);
}
// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_spi4encoder_supported_is(hal_spi_port_t port)
{
    return(hal_base_hid_byte_bitcheck(hal_brdcfg_spi4encoder__supported_mask, HAL_spi_port2index(port)) );
}

static void s_hal_spi4encoder_initted_set(hal_spi_port_t port)
{
    s_hal_spi4encoder_initted[HAL_spi_port2index(port)] = hal_true;
}

static hal_boolval_t s_hal_spi4encoder_initted_is(hal_spi_port_t port)
{
    return(s_hal_spi4encoder_initted[HAL_spi_port2index(port)]);
}


static void s_hal_spi4encoder_encoder_read_isr(hal_spi_port_t port)
{
    uint32_t tmp = 0;
	uint8_t dummytx = 0x0;
    SPI_TypeDef * SPIx = hal_SPI4ENCODER_PERIPH_GET(port);
    hal_spi_mux_t mux = (hal_spi_mux_t)hal_SPI4ENCODER_ENCODER_ID_GET(port);
    hal_encoder_t enc = hal_encoder_hid_from_spiportmux_to_encoder(port, mux);

	hal_SPI4ENCODER_ENCDATA_GET(port).buff_encoder[hal_SPI4ENCODER_READBYTE_POS_GET(port)] = SPI_I2S_ReceiveData(SPIx);
	hal_SPI4ENCODER_READBYTE_POS_INCR(port);
	
	if(3 == hal_SPI4ENCODER_READBYTE_POS_GET(port))
	{
		//before disabling peripheral, it is necessary wait until it's free
		while( SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) != RESET );
		
        hal_spi4encoder_hid_disa(port); //disable peripheral
 		hal_brdcfg_spi4encoder__encoder_disable(port, mux);		
		     
		hal_SPI4ENCODER_IT_RX_DISA(port);   //disable interrupt rx
		
		//formatting encoders data
		tmp = ((hal_SPI4ENCODER_ENCDATA_GET(port).buff_encoder[0] & 0x7F) << 16) | (hal_SPI4ENCODER_ENCDATA_GET(port).buff_encoder[1] << 8) | (hal_SPI4ENCODER_ENCDATA_GET(port).buff_encoder[2] & 0xE0) ;
		tmp = tmp >> 5;
		hal_SPI4ENCODER_ENCDATA_GET(port).encoder_result[hal_SPI4ENCODER_ENCODER_ID_GET(port)] = tmp;
		

		//reset variable user for reading
        hal_SPI4ENCODER_ENCODER_ID_RESET(port);
        hal_SPI4ENCODER_READBYTE_POS_RESET(port);

        // allow other tasks to restart a new read cycle of three calls of the isr
        hal_SPI4ENCODER_ENCODER_BUSY_RESET(port);


		//invoke call back  function
		if( NULL != hal_SPI4ENCODER_GET(port)->cbk_on_rx )
        {
            void *p =  (void*)enc;//hal_SPI4ENCODER_GET(port)->arg_cbk;
		    hal_SPI4ENCODER_GET(port)->cbk_on_rx(p);
        }
		
	}
	else
	{
		SPI_I2S_SendData(SPIx, dummytx);       
	}
}


static void s_hal_spi4encoder_RCC_conf(hal_spi_port_t port)
{
  switch(port)
  {
  	case hal_spi_port1:
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | SPI1_GPIO_CLK | HAL_BRDCFG_SPI4ENCODER__SPI1_GPIO_PORT_CS_CLOCK , ENABLE);   
		RCC_APB2PeriphClockCmd(SPI1_CLK, ENABLE); //RCC->APB1ENR |= 0x00008000;

		break;
	}	

  	case hal_spi_port2:
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | SPI2_GPIO_CLK | HAL_BRDCFG_SPI4ENCODER__SPI2_GPIO_PORT_CS_CLOCK, ENABLE);   
		RCC_APB1PeriphClockCmd(SPI2_CLK, ENABLE); //RCC->APB1ENR |= 0x00008000;

		break;
	}


  	case hal_spi_port3:
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | SPI3_GPIO_CLK | HAL_BRDCFG_SPI4ENCODER__SPI3_GPIO_PORT_CS_CLOCK, ENABLE);   //RCC->APB3ENR |=  0x00000019;
		RCC_APB1PeriphClockCmd(SPI3_CLK, ENABLE); //RCC->APB1ENR |= 0x00008000;
		break;
	}

  }

}


static void s_hal_spi4encoder_GPIO_conf(hal_spi_port_t port)
{ 
  uint32_t pos;
  uint8_t high_pin;

#warning --> in stm example .... sck and mosi are GPIO_Speed_50MHz / GPIO_Mode_AF_PP (OK as in here) but miso is GPIO_Mode_IN_FLOATING

	switch(port)
	{
	  case hal_spi_port1:
	  {
		s_hal_spi4encoder_get_pospin(SPI1_GPIO_PIN_SCK, &pos, &high_pin);

		if(high_pin)
		{
			SPI1_GPIO_PORT->CRH &= ~(0xF << (pos*4 ));
			SPI1_GPIO_PORT->CRH |= (0xB << (pos*4 ));   // out max 50mhz, altfun push pull
		}
		else
		{
			SPI1_GPIO_PORT->CRL &= ~(0xF << (pos*4 ));
			SPI1_GPIO_PORT->CRL |= (0xB << (pos*4 ));
		}


        
		s_hal_spi4encoder_get_pospin(SPI1_GPIO_PIN_MOSI, &pos, &high_pin);

		if(high_pin)
		{
			SPI1_GPIO_PORT->CRH &= ~(0xF << (pos*4 ));
			SPI1_GPIO_PORT->CRH |= (0xB << (pos*4 ));   // out max 50mhz, altfun push pull
		}
		else
		{
			SPI1_GPIO_PORT->CRL &= ~(0xF << (pos*4 ));
			SPI1_GPIO_PORT->CRL |= (0xB << (pos*4 ));
		}


		s_hal_spi4encoder_get_pospin(SPI1_GPIO_PIN_MISO, &pos, &high_pin);

		if(high_pin)
		{
			SPI1_GPIO_PORT->CRH &= ~(0xF << (pos*4 ));
			SPI1_GPIO_PORT->CRH |= (0x8 << (pos*4 ));   // input with pull-up / pull-down
            #warning --> acemor says: it is strange that the miso is not configured in af mode .....
		}
		else
		{
			SPI1_GPIO_PORT->CRL &= ~(0xF << (pos*4 ));
			SPI1_GPIO_PORT->CRL |= (0x8 << (pos*4 ));   
		}
			  break;
	  }


	  case hal_spi_port2:	//non c'e bisogno di ramap --> da verificare
	  {
		s_hal_spi4encoder_get_pospin(SPI2_GPIO_PIN_SCK, &pos, &high_pin);

		if(high_pin)
		{
			SPI2_GPIO_PORT->CRH &= ~(0xF << (pos*4 ));
			SPI2_GPIO_PORT->CRH |= (0xB << (pos*4 ));
		}
		else
		{
			SPI2_GPIO_PORT->CRL &= ~(0xF << (pos*4 ));
			SPI2_GPIO_PORT->CRL |= (0xB << (pos*4 ));
		}


		s_hal_spi4encoder_get_pospin(SPI2_GPIO_PIN_MOSI, &pos, &high_pin);

		if(high_pin)
		{
			SPI2_GPIO_PORT->CRH &= ~(0xF << (pos*4 ));
			SPI2_GPIO_PORT->CRH |= (0xB << (pos*4 ));
		}
		else
		{
			SPI2_GPIO_PORT->CRL &= ~(0xF << (pos*4 ));
			SPI2_GPIO_PORT->CRL |= (0xB << (pos*4 ));
		}


		s_hal_spi4encoder_get_pospin(SPI2_GPIO_PIN_MISO, &pos, &high_pin);

		if(high_pin)
		{
			SPI2_GPIO_PORT->CRH &= ~(0xF << (pos*4 ));
			SPI2_GPIO_PORT->CRH |= (0x8 << (pos*4 ));
		}
		else
		{
			SPI2_GPIO_PORT->CRL &= ~(0xF << (pos*4 ));
			SPI2_GPIO_PORT->CRL |= (0x8 << (pos*4 ));
		}
			  break;
	  }

	  case hal_spi_port3:
	  {
		/*il codice sottostante corrisponderebbe a scrivere nei registri:
		//		 GPIOC->CRH   &= 0xFFF000FF;
		// 		 GPIOC->CRH   |= 0x000B8B00; 
		Scrivendo nei registri si perderebbe portabilita' */
		
		GPIO_PinRemapConfig(GPIO_Remap_SPI3, ENABLE);

		s_hal_spi4encoder_get_pospin(SPI3_GPIO_PIN_SCK, &pos, &high_pin);

		if(high_pin)
		{
			SPI3_GPIO_PORT->CRH &= ~(0xF << (pos*4 ));
			SPI3_GPIO_PORT->CRH |= (0xB << (pos*4 ));
		}
		else
		{
			SPI3_GPIO_PORT->CRL &= ~(0xF << (pos*4 ));
			SPI3_GPIO_PORT->CRL |= (0xB << (pos*4 ));
		}


		s_hal_spi4encoder_get_pospin(SPI3_GPIO_PIN_MOSI, &pos, &high_pin);

		if(high_pin)
		{
			SPI3_GPIO_PORT->CRH &= ~(0xF << (pos*4 ));
			SPI3_GPIO_PORT->CRH |= (0xB << (pos*4 ));
		}
		else
		{
			SPI3_GPIO_PORT->CRL &= ~(0xF << (pos*4 ));
			SPI3_GPIO_PORT->CRL |= (0xB << (pos*4 ));
		}


		s_hal_spi4encoder_get_pospin(SPI3_GPIO_PIN_MISO, &pos, &high_pin);

		if(high_pin)
		{
			SPI3_GPIO_PORT->CRH &= ~(0xF << (pos*4 ));
			SPI3_GPIO_PORT->CRH |= (0x8 << (pos*4 ));
		}
		else
		{
			SPI3_GPIO_PORT->CRL &= ~(0xF << (pos*4 ));
			SPI3_GPIO_PORT->CRL |= (0x8 << (pos*4 ));
		}
			  break;
	  }
	}//end swich 

	hal_brdcfg_spi4encoder__chipSelect_init(port);
}


//Dato in input l'identificativo del pin, restituisce la posizione	e se deve essere configurato con il registro high o low.
static void s_hal_spi4encoder_get_pospin(uint32_t pin, uint32_t *pos, hal_bool_t *is_highpin)
{
	uint8_t i;
	uint32_t tmp_pid = 0;
	
	tmp_pid = pin;
	*is_highpin = hal_false;
	
	if(tmp_pid >= GPIO_Pin_8)
	{
		*is_highpin = hal_true;;
		tmp_pid = tmp_pid >> 8;		
	}
	for(i=0; (i<16) & (tmp_pid != GPIO_Pin_0); i++)
	{
		tmp_pid = tmp_pid >> 1;		
	}
	*pos = i;
}



static void s_hal_spi4encoder_NVIC_conf(hal_spi_port_t port, hal_interrupt_priority_t priority)
{
#if 1
    IRQn_Type SPIx_IT_IRQn = SPI1_IT_IRQn;
    
    switch(port)
    {
        case hal_spi_port1: { SPIx_IT_IRQn = SPI1_IT_IRQn; } break; 
        case hal_spi_port2: { SPIx_IT_IRQn = SPI2_IT_IRQn; } break;
        case hal_spi_port3: { SPIx_IT_IRQn = SPI3_IT_IRQn; } break;
        default:            { SPIx_IT_IRQn = SPI1_IT_IRQn; } break; 
    }

    if(hal_int_priorityNONE != priority)
    {
        // enable irq in nvic
        hal_sys_irqn_priority_set(SPIx_IT_IRQn, priority);
        hal_sys_irqn_enable(SPIx_IT_IRQn);
    }

#else
  NVIC_InitTypeDef NVIC_InitStructure;

  #warning --> acemor removed it to maintain 16 priorities
  ///* 1 bit for pre-emption priority, 3 bits for subpriority */
  //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  switch(port)
  {
  	case hal_spi_port1:   
    { 
        NVIC_InitStructure.NVIC_IRQChannel = SPI1_IT_IRQn;
	} break;
	
	case hal_spi_port2:   
    {   
        NVIC_InitStructure.NVIC_IRQChannel = SPI2_IT_IRQn;
	} break;
	
	case hal_spi_port3:   
    {   
        NVIC_InitStructure.NVIC_IRQChannel = SPI3_IT_IRQn;
	} break;
  }


  NVIC_Init(&NVIC_InitStructure);
#endif
}


 
#endif//HAL_USE_SPI4ENCODER

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
