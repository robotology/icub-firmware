/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       hl_can.c
	@brief      This file contains internals for hl can utilities.
	@author     marco.accame@iit.it
    @date       11/11/2013
**/

// - modules to be built: contains the HL_USE_* macros ---------------------------------------------------------------

#include "hl_cfg_plus_modules.h"


#if     defined(HL_USE_UTIL_CAN)

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

#include "hl_can.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hl_can_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hl_can_cfg_t hl_can_cfg_default =
{
    .baudrate   = hl_can_baudrate_1mbps,
    .advcfg     = NULL
};

const hl_can_advcfg_t hl_can_advcfg_default =
{   // good for canclock @ 36mhz and bitrate 1mbps 
    .CAN_Prescaler      = 4,                    // depends on: cfg->baudrate, canclock, sjw, bs1, bs2    
    .CAN_Mode           = CAN_Mode_Normal,         
    .CAN_SJW            = CAN_BS1_3tq,          // depends on: canclock
    .CAN_BS1            = CAN_BS1_5tq,          // depends on: canclock
    .CAN_BS2            = CAN_BS1_3tq,          // depends on: canclock         
    .CAN_TTCM           = DISABLE,              // time-triggered communication mode
    .CAN_ABOM           = DISABLE,              // automatic bus-off
    .CAN_AWUM           = DISABLE,              // automatic wake-up 
    .CAN_NART           = DISABLE,              // no-automatic retransmission mode 
    .CAN_RFLM           = DISABLE,              // receive fifo locked mode 
    .CAN_TXFP           = ENABLE                // transmit fifo priority (if ENABLEd, priority amongst the pending mailboxes is driven by the request order) 
};

//__weak extern const hl_can_mapping_t* hl_can_map = NULL;



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



typedef struct
{
    hl_can_cfg_t                config;
} hl_can_internal_item_t;


typedef struct
{
    uint32_t                    inittedmask;
    hl_can_internal_item_t*     items[hl_cans_number];   
} hl_can_theinternals_t;


/* @typedef    typedef struct hl_can_bitsampling_t 
    @brief      contains the quantisation bit timing for stm32f1/2/4 library. The bit is divided in N quanta, N = bs1 + 1 + bs2.
                value bs1 is the number of quanta before the sampling quanta, and bs2 the number after. value sjw is the number
                of quanta that are possible to stretch to perform resynchronization.
    @warning    the can clock (slow APB bus) divided by 1000 must be multiple of value N = bs1+1+bs2.
                for can clock at 36MHz good values are: (CAN_BS1_5tq, CAN_BS2_3tq, CAN_SJW_3tq) because 5+1+3=9    
                for can clock at 42MHz good values are: (CAN_BS1_4tq, CAN_BS2_2tq, CAN_SJW_3tq) because 4+1+2=7 
 */
typedef struct
{
    uint8_t     bs1;    /*< use CAN_BS1_1tq -> CAN_BS1_16tq */
    uint8_t     bs2;    /*< use CAN_BS2_1tq -> CAN_BS1_8tq */
    uint8_t     sjw;    /*< use CAN_SJW_1tq -> CAN_SJW_4tq */
} hl_can_bitsampling_t;



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hl_boolval_t s_hl_can_supported_is(hl_can_t id);
static void s_hl_can_initted_set(hl_can_t id);
static hl_boolval_t s_hl_can_initted_is(hl_can_t id);


// hw related initialisation which may change with different versions of stm32fx mpus
static void s_hl_can_hw_gpio_init(hl_can_t id);
static void s_hl_can_hw_clock_init(hl_can_t id);
static hl_result_t s_hl_can_hw_registers_init(hl_can_t id);

static const hl_can_bitsampling_t* s_hl_can_get_bitsampling(uint32_t canclock);

static void s_hl_can_fill_gpio_init_altf(hl_can_t id, hl_gpio_init_t* rxinit, hl_gpio_altf_t* rxaltf, hl_gpio_init_t* txinit, hl_gpio_altf_t* txaltf);





// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hl_can_theinternals_t s_hl_can_theinternals =
{
    .inittedmask        = 0,
    .items              = { NULL }   
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hl_result_t hl_can_init(hl_can_t id, const hl_can_cfg_t *cfg)
{
    hl_can_internal_item_t* intitem = s_hl_can_theinternals.items[HL_can_id2index(id)];

    if(hl_false == s_hl_can_supported_is(id))
    {
        return(hl_res_NOK_generic);
    }
    
    if(hl_true == s_hl_can_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }    

    if(NULL == cfg)
    {
        cfg  = &hl_can_cfg_default;
    }
    
    
    // give memory to can internal item for this id ...   
    if(NULL == intitem)
    {
        // the internal item
        intitem = s_hl_can_theinternals.items[HL_can_id2index(id)] = hl_sys_heap_new(sizeof(hl_can_internal_item_t));   
    }
    
    // set config
    memcpy(&intitem->config, cfg, sizeof(hl_can_cfg_t));
     
            
    // init gpios
    s_hl_can_hw_gpio_init(id);
    
    // init clock
    s_hl_can_hw_clock_init(id);
    
    s_hl_can_initted_set(id);

    return(hl_res_OK);
}

extern hl_boolval_t hl_can_supported_is(hl_can_t id)
{
    return(s_hl_can_supported_is(id));
}


extern hl_boolval_t hl_can_initted_is(hl_can_t id)
{
    return(s_hl_can_initted_is(id));
}

extern hl_result_t hl_can_enable(hl_can_t id)
{
	hl_result_t res;
    
#if     !defined(HL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)
    if(hl_false == s_hl_can_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }
#endif
    
    res = s_hl_can_hw_registers_init(id); // hardware setup

    if(res != hl_res_OK)
    {
    	return(hl_res_NOK_generic);
    }
    
    // i dont init any isr in here ...


	return(hl_res_OK);
}


extern hl_result_t hl_can_disable(hl_can_t id) 
{
//    hl_can_internal_item_t* intitem = s_hl_can_theinternals.items[HL_can_id2index(id)];
    CAN_TypeDef* CANx = HL_can_port2peripheral(id); 

#if     !defined(HL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)    
    if(hl_false == s_hl_can_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }
#endif
   
    CAN_DeInit(CANx);

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

static hl_boolval_t s_hl_can_supported_is(hl_can_t id)
{
    if(NULL == hl_can_map)
    {
        return(hl_false);
    }
    return(hl_bits_word_bitcheck(hl_can_map->supportedmask, HL_can_id2index(id)) );
}

static void s_hl_can_initted_set(hl_can_t id)
{
    hl_bits_word_bitset(&s_hl_can_theinternals.inittedmask, HL_can_id2index(id));
}

static hl_boolval_t s_hl_can_initted_is(hl_can_t id)
{
    return(hl_bits_word_bitcheck(s_hl_can_theinternals.inittedmask, HL_can_id2index(id)));
}


static void s_hl_can_hw_gpio_init(hl_can_t id)
{

    hl_gpio_init_t gpio_init_rx;    
    hl_gpio_altf_t gpio_altf_rx;    
    hl_gpio_init_t gpio_init_tx;
    hl_gpio_altf_t gpio_altf_tx;   
    

    s_hl_can_fill_gpio_init_altf(id, &gpio_init_rx, &gpio_altf_rx, &gpio_init_tx, &gpio_altf_tx);
    
    hl_gpio_init(&gpio_init_rx);
    hl_gpio_altf(&gpio_altf_rx);
    
    hl_gpio_init(&gpio_init_tx);
    hl_gpio_altf(&gpio_altf_tx);
}


static void s_hl_can_hw_clock_init(hl_can_t id)
{
    RCC_APB1PeriphClockCmd((hl_can1 == id) ? (RCC_APB1Periph_CAN1) : (RCC_APB1Periph_CAN2), ENABLE);
}




// #if 0
// #if     defined(HL_USE_MPU_ARCH_STM32F1)
//     // the clock APB1 is 36mhz, half the max frequency of 72 mhz. (see page 49/1096 of stm32f1x reference manual Doc ID 13902 Rev 14) 
//     // the clock APB2 operates at full speed of 72 mhz. (see page 49/1096 of stm32f1x reference manual Doc ID 13902 Rev 14)
//     // can is on APB1, aka the slow bus
//     // we give a total of HL_CAN_TQ_TOTAL = 9 time quanta for the duration of a can bit. 
//     // we split the 9 time quanta in 5+1+3 = HL_CAN_TQ_BS1+1+HL_CAN_TQ_BS2
//     // we allow to stretch the bit duration in order to re-synch by maximum HL_CAN_TQ_SJW = 3 time quanta 
//     //#define HL_CAN_CLK	  		      (hl_brdcfg_cpu__theconfig.speeds.slowbus)
//     #define HL_CAN_CLK                  (SystemCoreClock/2)
//     #define HL_CAN_TQ_TOTAL             9
//     #define HL_CAN_TQ_SJW               CAN_SJW_3tq
//     #define HL_CAN_TQ_BS1               CAN_BS1_5tq
//     #define HL_CAN_TQ_BS2               CAN_BS2_3tq
// #elif    defined(HL_USE_MPU_ARCH_STM32F4)
//     // the clock APB1 is 42mhz, a fourth the max frequency of 168 mhz. (see page 23/180 of stm32f4x datasheet Doc ID 022152 Rev 3)
//     // the clock APB2 is 84mhz, a half the max frequency of 168 mhz. (see page 23/180 of stm32f4x datasheet Doc ID 022152 Rev 3)
//     // can is on APB1, aka the slow bus
//     // we give a total of HL_CAN_TQ_TOTAL = 7 time quanta for the duration of a can bit. 
//     // we split the 7 time quanta in 4+1+2 = HL_CAN_TQ_BS1+1+HL_CAN_TQ_BS2
//     // we allow to stretch the bit duration in order to re-synch by maximum HL_CAN_TQ_SJW = 3 time quanta     
//     //#define HL_CAN_CLK                  (hl_brdcfg_cpu__theconfig.speeds.slowbus)
//     #define HL_CAN_TQ_TOTAL             7
//     #define HL_CAN_TQ_SJW               CAN_SJW_3tq
//     #define HL_CAN_TQ_BS1               CAN_BS1_4tq
//     #define HL_CAN_TQ_BS2               CAN_BS2_2tq    
// #else //defined(HL_USE_CPU_ARCH_*)
//     #error ERR --> choose a HL_USE_CPU_ARCH_*
// #endif 
// #endif


static const hl_can_bitsampling_t* s_hl_can_get_bitsampling(uint32_t canclock)
{  
    // we chose to have the number of time quanta tqtotal equal to an integral divisor of 
    // (canclock / 1mhz). in such a way the CAN_Prescaler is simply = (canclock / tqtotal) / baudrate
    // for 36mhz and 42 mhz we took the values from examples of stm32f1x and stm32f4x by st microelectronics.
    // only tested values are 36mhz and 42mhz.
    static const hl_can_bitsampling_t s_hl_can_bitsampling_9tq = 
    {   // for canclocks: 36mhz, 18mhz, 9mhz (and multiples of 9)
        .bs1        = CAN_BS1_5tq,
        .bs2        = CAN_BS2_3tq,        
        .sjw        = CAN_SJW_3tq
    };
    static const hl_can_bitsampling_t s_hl_can_bitsampling_8tq = 
    {    // for canclocks: multiples of 8mhz
        .bs1        = CAN_BS1_4tq,
        .bs2        = CAN_BS2_3tq,        
        .sjw        = CAN_SJW_3tq
    };    
    static const hl_can_bitsampling_t s_hl_can_bitsampling_7tq = 
    {    // for canclocks: 42mhz, 21mHz (and multiples of 7mhz)
        .bs1        = CAN_BS1_4tq,
        .bs2        = CAN_BS2_2tq,        
        .sjw        = CAN_SJW_3tq
    };    
    

    const hl_can_bitsampling_t* ret = NULL;
    
    switch(canclock)
    {
        case 36000000:
        case 27000000:
        case 18000000:
        case  9000000:
        {
            ret = &s_hl_can_bitsampling_9tq;
        } break;
                
        case 40000000:
        case 32000000:
        case 24000000:
        case 16000000:
        case  8000000:
        {
            ret = &s_hl_can_bitsampling_8tq;
        } break;        
        
        case 42000000:
        case 35000000:
        case 28000000:
        case 21000000:
        case 14000000:
        case  7000000:
        {
            ret = &s_hl_can_bitsampling_7tq;
        } break;
        
        default:
        {
            ret = NULL;
        }   break;           
    }
    
    if(NULL == ret)
    {
        hl_sys_on_error(hl_error_unsupportedbehaviour, "can: bit sampling not supported for this canclock");       
    }
    
    return(ret);  
}


static hl_result_t s_hl_can_hw_registers_init(hl_can_t id)
{   
    hl_can_internal_item_t* intitem = s_hl_can_theinternals.items[HL_can_id2index(id)];
    hl_can_cfg_t *cfg = &intitem->config;
    CAN_TypeDef* CANx = HL_can_port2peripheral(id);
    
    CAN_InitTypeDef* init2use = NULL;
    const hl_can_bitsampling_t* bits2use = NULL;
        
    CAN_InitTypeDef CAN_InitStructure;


    if(NULL != cfg->advcfg)
    {
        // use advcfg. it has the same layout as CAN_InitTypeDef
        init2use = (CAN_InitTypeDef*) cfg->advcfg;      
    }
    else
    {
        // use CAN_InitStructure.
        init2use = &CAN_InitStructure;       
        
        // which i must init according to some cfg params and other constant choices
        
        CAN_StructInit(init2use); 
        

        RCC_ClocksTypeDef clocks;
        RCC_GetClocksFreq(&clocks);  
        
        uint32_t canclock = clocks.PCLK1_Frequency;
        
        bits2use = s_hl_can_get_bitsampling(canclock);
        uint8_t tqtotal = (bits2use->bs1+1) + 1 + (bits2use->bs2+1);
        
        uint32_t baudrate = 0;              
        switch(intitem->config.baudrate)
        {
            case hl_can_baudrate_1mbps:     baudrate = 1000000;     break;
            case hl_can_baudrate_500kbps:   baudrate =  500000;     break;
            default:                        baudrate =  500000;     break;
        }
        
        // copy the default configuration
        memcpy(init2use, &hl_can_advcfg_default, sizeof(CAN_InitTypeDef));
        
        // and now change it
        
        
        // CAN_Prescaler is the prescaler to apply to PCLK1_Frequency so that the bit has tqtotal time quanta (or ticks of the clock)
        // where tqtotal = (CAN_BS1+1) + 1 + (CAN_BS2+1) ... stm32lib use values starting from 0 and we need starting from 1 ...        
        init2use->CAN_Prescaler = (canclock / tqtotal) / baudrate;      
        init2use->CAN_SJW       = bits2use->sjw;    // max num of time quanta the hw is allowed to stretch a bit in order to re-synch
        init2use->CAN_BS1       = bits2use->bs1;    // number of time quanta in bit-segment-1 (the one before the sampling time quantum)
        init2use->CAN_BS2       = bits2use->bs2;    // number of time quanta in bit-segment-2 (the one after the sampling time quantum)       
    }
    
    
    // ok, we init the can
    
    CAN_DeInit(CANx);

	if(CAN_InitStatus_Failed == CAN_Init(CANx, init2use))
	{
		return(hl_res_NOK_generic);
	}
    
    // we do just a basic init to the filters. we use only one filter in bank0 which is ok for both CAN1 (master) and CAN2 (slave)

    // we dont assign any bank which is specific of only can2, thus both can1 and can start from bank0
    CAN_SlaveStartBank(28);

    // now we initialise bank0 to pass everything to fifo0. yes, we use only fifo0.
    CAN_FilterInitTypeDef CAN_FilterInitStructure;
	CAN_FilterInitStructure.CAN_FilterMode              = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale             = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh            = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow             = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh        = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow         = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment    = CAN_Filter_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation        = ENABLE;
    CAN_FilterInitStructure.CAN_FilterNumber            = 0;

    CAN_FilterInit(&CAN_FilterInitStructure);
   
	return(hl_res_OK);
}



static void s_hl_can_fill_gpio_init_altf(hl_can_t id, hl_gpio_init_t* rxinit, hl_gpio_altf_t* rxaltf, hl_gpio_init_t* txinit, hl_gpio_altf_t* txaltf)
{
    static const hl_gpio_init_t s_hl_can_rx_gpio_init = 
    {
#if     defined(HL_USE_MPU_ARCH_STM32F1)
        .port           = hl_gpio_portNONE,
        .mode           =
        {
            .gpio_pins      = hl_gpio_pinNONE,
            .gpio_speed     = GPIO_Speed_50MHz, // both GPIO_Speed_2MHz and GPIO_Speed_50MHz are the same as it is configured in input mode
            .gpio_mode      = GPIO_Mode_IPU        
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

    
    static const hl_gpio_init_t s_hl_can_tx_gpio_init         = 
    {
#if     defined(HL_USE_MPU_ARCH_STM32F1)
        .port           = hl_gpio_portNONE,
        .mode           =
        {
            .gpio_pins      = hl_gpio_pinNONE,
            .gpio_speed     = GPIO_Speed_50MHz, 
            .gpio_mode      = GPIO_Mode_AF_PP        
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

    // at first we copy the default configuration of pin rx and tx
    
    memcpy(rxinit, &s_hl_can_rx_gpio_init, sizeof(hl_gpio_init_t));   
    memcpy(txinit, &s_hl_can_tx_gpio_init, sizeof(hl_gpio_init_t));
    
    // then we verify the pin mapping and the altfun ... ok don't do it.
    // but you could put it in here. maybe by calling an external function which depends on the mpu
    

    if(NULL == hl_can_map)
    {
        return;
    }
    // then we set the port and pin of rx and tx
    hl_gpio_fill_init(rxinit, &hl_can_map->gpiomap[HL_can_id2index(id)].rx);
    hl_gpio_fill_init(txinit, &hl_can_map->gpiomap[HL_can_id2index(id)].tx);
    
    // then we set altfun of rx and tx
    hl_gpio_fill_altf(rxaltf, &hl_can_map->gpiomap[HL_can_id2index(id)].rx);
    hl_gpio_fill_altf(txaltf, &hl_can_map->gpiomap[HL_can_id2index(id)].tx);  

}

#endif//defined(HL_USE_UTIL_CAN)

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






