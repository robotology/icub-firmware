/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license SPI be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

/* @file       hl_spi.c
	@brief      This file contains internals for hl SPI utilities.
	@author     marco.accame@iit.it
    @date       02/12/2014
**/

// - modules to be built: contains the HL_USE_* macros ---------------------------------------------------------------

#include "hl_cfg_plus_modules.h"


#if     defined(HL_USE_UTIL_SPI)

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "hl_core.h"        // contains the required stm32f10x_*.h or stm32f4xx*.h header files

#include "hl_gpio.h"
#include "hl_sys.h"

#include "hl_bits.h" 

#include "hl_arch.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hl_spi.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hl_spi_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HL_spi_id2index(p)              ((uint8_t)((p)))

#define HL_spi_port2peripheral(p)       ( s_hl_spi_stmSPImap[HL_spi_id2index(p)] )



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hl_spi_cfg_t hl_spi_cfg_default =
{
    .mode           = hl_spi_mode_master, 
    .prescaler      = hl_spi_prescaler_256, 
    .advcfg         = NULL
};

// for ems001 was: SPI_CPOL_High, but SPI_CPOL_Low is OK for ems4rd 
const hl_spi_advcfg_t hl_spi_advcfg_default =
{   
    .SPI_Direction          = SPI_Direction_2Lines_FullDuplex,
    .SPI_Mode               = SPI_Mode_Master,                              // param
    .SPI_DataSize           = SPI_DataSize_8b, 
    .SPI_CPOL               = SPI_CPOL_Low, //SPI_CPOL_High, //SPI_CPOL_Low, //SPI_CPOL_Low, // SPI_CPOL_High high is ok with display and also ok with isr mode
    .SPI_CPHA               = SPI_CPHA_1Edge, //SPI_CPHA_2Edge,
    .SPI_NSS                = SPI_NSS_Soft,
    .SPI_BaudRatePrescaler  = SPI_BaudRatePrescaler_64,                      // param: depends on speed
    .SPI_FirstBit           = SPI_FirstBit_MSB, // SPI_FirstBit_MSB is ok with display, su stm3210c e' indifferente
    .SPI_CRCPolynomial      = 0x0007 // reset value
};


//__weak extern const hl_spi_mapping_t* hl_spi_map = NULL;



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



typedef struct
{
    hl_spi_cfg_t                config;
} hl_spi_internal_item_t;


typedef struct
{
    uint32_t                    inittedmask;
    hl_spi_internal_item_t*     items[hl_spis_number];   
} hl_spi_theinternals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hl_boolval_t s_hl_spi_supported_is(hl_spi_t id);
static void s_hl_spi_initted_set(hl_spi_t id);
static hl_boolval_t s_hl_spi_initted_is(hl_spi_t id);


// hw related initialisation which may change with different versions of stm32fx mpus
static void s_hl_spi_hw_gpio_init(hl_spi_t id, hl_spi_mode_t spimode);
static void s_hl_spi_hw_clock_init(hl_spi_t id);
static hl_result_t s_hl_spi_hw_registers_init(hl_spi_t id);



static void s_hl_spi_fill_gpio_init_altf(   hl_spi_t id, hl_spi_mode_t spimode,
                                            hl_gpio_init_t* sckinit, hl_gpio_altf_t* sckaltf, 
                                            hl_gpio_init_t* misoinit, hl_gpio_altf_t* misoaltf,
                                            hl_gpio_init_t* mosiinit, hl_gpio_altf_t* mosialtf
                                        );





// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

static SPI_TypeDef* const s_hl_spi_stmSPImap[] = { SPI1, SPI2, SPI3 };

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hl_spi_theinternals_t s_hl_spi_theinternals =
{
    .inittedmask        = 0,
    .items              = { NULL }   
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern uint32_t hl_spi_speedofbus_get(hl_spi_t id)
{
    uint32_t res = 0;
    
    if(hl_false == s_hl_spi_supported_is(id))
    {
        return(0);
    }
    
    RCC_ClocksTypeDef clocks;
    RCC_GetClocksFreq(&clocks);  
    
    switch(id)
    {
        case hl_spi1:
        {
            res = clocks.PCLK2_Frequency;      
        } break;

        case hl_spi2:
        case hl_spi3:
        {
            res = clocks.PCLK1_Frequency;
        } break;  

        default:
        {
            res = 0;
        } break;        
    }  
    
    return(res);
}


extern hl_result_t hl_spi_init(hl_spi_t id, const hl_spi_cfg_t *cfg)
{
    hl_spi_internal_item_t* intitem = s_hl_spi_theinternals.items[HL_spi_id2index(id)];

    if(hl_false == s_hl_spi_supported_is(id))
    {
        return(hl_res_NOK_generic);
    }
    
    if(hl_true == s_hl_spi_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }    

    if(NULL == cfg)
    {
        cfg  = &hl_spi_cfg_default;
    }
    
    if(hl_spi_mode_slave == cfg->mode)
    {
        hl_sys_on_error(hl_error_unsupportedbehaviour, "spi master mode is unsupported");
    }
    
    
    // give memory to SPI internal item for this id ...   
    if(NULL == intitem)
    {
        // the internal item
        intitem = s_hl_spi_theinternals.items[HL_spi_id2index(id)] = hl_sys_heap_new(sizeof(hl_spi_internal_item_t));   
    }
    
    // set config
    memcpy(&intitem->config, cfg, sizeof(hl_spi_cfg_t));
    
    // i use the initialisation order suggested in file stm32f4xx_spi.c in section: ##### How to use this driver #####
    // which is: hw_clock_init(), gpio_init(), hw_enable() .... BECAUSE:
    // in hal2 it was tested with gpio_init() and then hw_clock_init() and then hw_enable() and it worked.
    // the important thing however is to keep hw_enable() as last operation.
    // see original note on HAL2
    
    // HAL2: acemor: very important info.
    // HAL2: init the miso and mosi gpio before calling hw_init. 
    // HAL2: because if the spi is already initted and it detects mosi or miso low it sets
    // HAL2: register SPI_SR2.BUSY to 1, which makes things hang up.    
               
    // init clock
    s_hl_spi_hw_clock_init(id);
    
    // init gpios
    s_hl_spi_hw_gpio_init(id, cfg->mode);    
    
    s_hl_spi_initted_set(id);

    return(hl_res_OK);
}

extern hl_boolval_t hl_spi_supported_is(hl_spi_t id)
{
    return(s_hl_spi_supported_is(id));
}


extern hl_boolval_t hl_spi_initted_is(hl_spi_t id)
{
    return(s_hl_spi_initted_is(id));
}

extern hl_result_t hl_spi_enable(hl_spi_t id)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)
    if(hl_false == s_hl_spi_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }
#endif
    
    hl_result_t res = s_hl_spi_hw_registers_init(id); // hardware setup

    if(res != hl_res_OK)
    {
    	return(hl_res_NOK_generic);
    }
    
    // i dont init any isr in here ...


	return(hl_res_OK);
}


extern hl_result_t hl_spi_disable(hl_spi_t id) 
{
    SPI_TypeDef* SPIx = HL_spi_port2peripheral(id); 

#if     !defined(HL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)    
    if(hl_false == s_hl_spi_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }
#endif
   
    SPI_I2S_DeInit(SPIx);

    return(hl_res_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----


// ---- isr of the module: end ------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hl_boolval_t s_hl_spi_supported_is(hl_spi_t id)
{
    if(NULL == hl_spi_map)
    {
        return(hl_false);
    }
    return(hl_bits_word_bitcheck(hl_spi_map->supportedmask, HL_spi_id2index(id)) );
}

static void s_hl_spi_initted_set(hl_spi_t id)
{
    hl_bits_word_bitset(&s_hl_spi_theinternals.inittedmask, HL_spi_id2index(id));
}

static hl_boolval_t s_hl_spi_initted_is(hl_spi_t id)
{
    return(hl_bits_word_bitcheck(s_hl_spi_theinternals.inittedmask, HL_spi_id2index(id)));
}


static void s_hl_spi_hw_gpio_init(hl_spi_t id, hl_spi_mode_t spimode)
{

    hl_gpio_init_t gpio_init_sck;    
    hl_gpio_altf_t gpio_altf_sck;    
    hl_gpio_init_t gpio_init_miso;
    hl_gpio_altf_t gpio_altf_miso;   
    hl_gpio_init_t gpio_init_mosi;
    hl_gpio_altf_t gpio_altf_mosi;       

    s_hl_spi_fill_gpio_init_altf(id, spimode, &gpio_init_sck, &gpio_altf_sck, &gpio_init_miso, &gpio_altf_miso, &gpio_init_mosi, &gpio_altf_mosi);
    
    hl_gpio_init(&gpio_init_sck);
    hl_gpio_altf(&gpio_altf_sck);
    
    hl_gpio_init(&gpio_init_miso);
    hl_gpio_altf(&gpio_altf_miso);
    
    hl_gpio_init(&gpio_init_mosi);
    hl_gpio_altf(&gpio_altf_mosi);    
}


static void s_hl_spi_hw_clock_init(hl_spi_t id)
{
    SPI_TypeDef* SPIx = HL_spi_port2peripheral(id); 
        
    switch(id)
    {
        case hl_spi1:
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);       
        } break;

        case hl_spi2:
        {
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
        } break;  

        case hl_spi3:
        {
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
        } break;        
    }
    
    // the DeInit() is put just to keep teh same operations used in HAL2, where there is a RCC_APB2PeriphResetCmd( , ENABLE) and then DISABLE
    // however it may be redundant.
    SPI_I2S_DeInit(SPIx);    
}



static hl_result_t s_hl_spi_hw_registers_init(hl_spi_t id)
{   
    hl_spi_internal_item_t* intitem = s_hl_spi_theinternals.items[HL_spi_id2index(id)];
    hl_spi_cfg_t *cfg = &intitem->config;
    SPI_TypeDef* SPIx = HL_spi_port2peripheral(id);
    
    static const uint16_t s_hl_spi_stm32_baudrateprescalers[] = 
    { 
        0,  // prescaler 1 is not available .... 
        SPI_BaudRatePrescaler_2,    SPI_BaudRatePrescaler_4,    SPI_BaudRatePrescaler_8,    SPI_BaudRatePrescaler_16, 
        SPI_BaudRatePrescaler_32,   SPI_BaudRatePrescaler_64,   SPI_BaudRatePrescaler_128,  SPI_BaudRatePrescaler_256
    }; hl_VERIFYproposition(a2, hl_spi_prescaler_002 == 1); hl_VERIFYproposition(a4, hl_spi_prescaler_004 == 2);  hl_VERIFYproposition(a256, hl_spi_prescaler_256 == 8);
    
    SPI_InitTypeDef* init2use = NULL;
        
    SPI_InitTypeDef SPI_InitStructure;


    if(NULL != cfg->advcfg)
    {
        // use advcfg. it has the same layout as SPI_InitTypeDef
        init2use = (SPI_InitTypeDef*) cfg->advcfg;      
    }
    else
    {
        // use SPI_InitStructure.
        init2use = &SPI_InitStructure;       
        
        // which i must init according to some cfg params and other constant choices
        
        SPI_StructInit(init2use);      
        
        // copy the default configuration
        memcpy(init2use, &hl_spi_advcfg_default, sizeof(SPI_InitTypeDef));
        
        // and now change it
                
        // apply only mode and prescaler       
        init2use->SPI_Mode              = (hl_spi_mode_master == cfg->mode) ? (SPI_Mode_Master) : (SPI_Mode_Slave);  
        init2use->SPI_BaudRatePrescaler = s_hl_spi_stm32_baudrateprescalers[cfg->prescaler];      
    }
    
    
    // ok, we init the SPI
    
    SPI_I2S_DeInit(SPIx);

	SPI_Init(SPIx, init2use);
    
    
	return(hl_res_OK);
}


static void s_hl_spi_fill_gpio_init_altf(   hl_spi_t id, hl_spi_mode_t spimode,
                                            hl_gpio_init_t* sckinit, hl_gpio_altf_t* sckaltf, 
                                            hl_gpio_init_t* misoinit, hl_gpio_altf_t* misoaltf,
                                            hl_gpio_init_t* mosiinit, hl_gpio_altf_t* mosialtf
                                        )
{
    static const hl_gpio_init_t s_hl_spi_sckmosi_master_gpio_init = 
    {
#if     defined(HL_USE_MPU_ARCH_STM32F1)
        .port           = hl_gpio_portNONE,
        .mode           =
        {
            .gpio_pins      = hl_gpio_pinNONE,
            .gpio_speed     = GPIO_Speed_50MHz,
            .gpio_mode      = GPIO_Mode_AF_OD        
        }
#elif   defined(HL_USE_MPU_ARCH_STM32F4)  
        .port        = hl_gpio_portNONE,
        .mode        =
        {
            .gpio_pins      = hl_gpio_pinNONE,
            .gpio_mode      = GPIO_Mode_AF,
            .gpio_speed     = GPIO_Speed_50MHz,
            .gpio_otype     = GPIO_OType_PP,
            .gpio_pupd      = GPIO_PuPd_UP
        }
#else //defined(HL_USE_MPU_ARCH_*)
    #error ERR --> choose a HL_USE_MPU_ARCH_*
#endif 
    };

    static const hl_gpio_init_t s_hl_spi_sckmosi_slave_gpio_init = 
    {
#if     defined(HL_USE_MPU_ARCH_STM32F1)
        .port           = hl_gpio_portNONE,
        .mode           =
        {
            .gpio_pins      = hl_gpio_pinNONE,
            .gpio_speed     = GPIO_Speed_50MHz,
            .gpio_mode      = GPIO_Mode_IN_FLOATING        
        }
#elif   defined(HL_USE_MPU_ARCH_STM32F4)  
        .port        = hl_gpio_portNONE,
        .mode        =
        {
            .gpio_pins      = hl_gpio_pinNONE,
            .gpio_mode      = GPIO_Mode_AF,
            .gpio_speed     = GPIO_Speed_50MHz,
            .gpio_otype     = GPIO_OType_PP,
            .gpio_pupd      = GPIO_PuPd_DOWN
        }
#else //defined(HL_USE_MPU_ARCH_*)
    #error ERR --> choose a HL_USE_MPU_ARCH_*
#endif 
    };    
    
    static const hl_gpio_init_t s_hl_spi_miso_master_gpio_init         = 
    {
#if     defined(HL_USE_MPU_ARCH_STM32F1)
        .port           = hl_gpio_portNONE,
        .mode           =
        {
            .gpio_pins      = hl_gpio_pinNONE,
            .gpio_speed     = GPIO_Speed_50MHz, 
            .gpio_mode      = GPIO_Mode_IN_FLOATING        
        }
#elif   defined(HL_USE_MPU_ARCH_STM32F4)  
        .port        = hl_gpio_portNONE,
        .mode        =
        {
            .gpio_pins      = hl_gpio_pinNONE,
            .gpio_mode      = GPIO_Mode_AF,
            .gpio_speed     = GPIO_Speed_50MHz,
            .gpio_otype     = GPIO_OType_PP,
            .gpio_pupd      = GPIO_PuPd_NOPULL
        }
#else //defined(HL_USE_MPU_ARCH_*)
    #error ERR --> choose a HL_USE_MPU_ARCH_*
#endif 
    };

    static const hl_gpio_init_t s_hl_spi_miso_slave_gpio_init         = 
    {
#if     defined(HL_USE_MPU_ARCH_STM32F1)
        .port           = hl_gpio_portNONE,
        .mode           =
        {
            .gpio_pins      = hl_gpio_pinNONE,
            .gpio_speed     = GPIO_Speed_50MHz, 
            .gpio_mode      = GPIO_Mode_AF_OD        
        }
#elif   defined(HL_USE_MPU_ARCH_STM32F4)  
        .port        = hl_gpio_portNONE,
        .mode        =
        {
            .gpio_pins      = hl_gpio_pinNONE,
            .gpio_mode      = GPIO_Mode_AF,
            .gpio_speed     = GPIO_Speed_50MHz,
            .gpio_otype     = GPIO_OType_PP,
            .gpio_pupd      = GPIO_PuPd_DOWN
        }
#else //defined(HL_USE_MPU_ARCH_*)
    #error ERR --> choose a HL_USE_MPU_ARCH_*
#endif 
    };    
    // at first we copy the default configuration of pin sck, miso, and mosi
    
    if(hl_spi_mode_master == spimode)
    {
        memcpy(sckinit, &s_hl_spi_sckmosi_master_gpio_init, sizeof(hl_gpio_init_t));   
        memcpy(misoinit, &s_hl_spi_miso_master_gpio_init, sizeof(hl_gpio_init_t));
        memcpy(mosiinit, &s_hl_spi_sckmosi_master_gpio_init, sizeof(hl_gpio_init_t));
    }
    else
    {
        memcpy(sckinit, &s_hl_spi_sckmosi_slave_gpio_init, sizeof(hl_gpio_init_t));   
        memcpy(misoinit, &s_hl_spi_miso_slave_gpio_init, sizeof(hl_gpio_init_t));
        memcpy(mosiinit, &s_hl_spi_sckmosi_slave_gpio_init, sizeof(hl_gpio_init_t));
    }
    
    // then we verify the pin mapping and the altfun ... ok don't do it.
    // but you could put it in here. maybe by calling an external function which depends on the mpu
    

    if(NULL == hl_spi_map)
    {
        return;
    }
    // then we set the port and pin of sck, miso, and mosi
    hl_gpio_fill_init(sckinit, &hl_spi_map->gpiomap[HL_spi_id2index(id)].sck);
    hl_gpio_fill_init(misoinit, &hl_spi_map->gpiomap[HL_spi_id2index(id)].miso);
    hl_gpio_fill_init(mosiinit, &hl_spi_map->gpiomap[HL_spi_id2index(id)].mosi);
    
    // then we set altfun of of sck, miso, and mosi
    hl_gpio_fill_altf(sckaltf, &hl_spi_map->gpiomap[HL_spi_id2index(id)].sck);
    hl_gpio_fill_altf(misoaltf, &hl_spi_map->gpiomap[HL_spi_id2index(id)].miso);  
    hl_gpio_fill_altf(mosialtf, &hl_spi_map->gpiomap[HL_spi_id2index(id)].mosi);  

}

#endif//defined(HL_USE_UTIL_SPI)

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






